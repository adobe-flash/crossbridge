/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_PoolObject__
#define __avmplus_PoolObject__


namespace avmplus
{
#ifdef VMCFG_NANOJIT
    class CodeMgr;
#endif

    // This is intended to be exactly the size of a double in memory.
    // (We use compile-time assertions to verify this.)
    class GCDouble : public MMgc::GCObject
    {
    public:
        double value;
    };

#ifdef VMCFG_FLOAT
    // The floats actually take 8 bytes in heap - all allocations are 8-byte
    // aligned. So we need to add some padding here.
    // (We use compile-time assertions to verify GCFloat is 8 bytes.)
    class GCFloat : public MMgc::GCObject
    {
    public:
        float value;
    private:
        int32_t padding;
    };

    // This is intended to be exactly the size of a float4_t in memory.
    // (We use compile-time assertions to verify this.)
    class GCFloat4 : public MMgc::GCObject
    {
    public:
        float4_t value;
    };
#endif // VMCFG_FLOAT

    // Used within ConstantStringContainer
    union ConstantStringData
    {
        Stringp     str;
        const uint8_t* abcPtr;
    };

    // Used within PoolObject.
    //
    // The macros for exact tracing fail us here: The interpretation
    // of each element depends on data from the pool, so we can't treat this
    // with GC_STRUCTURES.  Instead there's a hand-written tracer.  No doubt
    // we could concoct a mechanism to handle this case but we should only
    // do so if we see more instances of it.

    class ConstantStringContainer : public MMgc::GCTraceableObject
    {
    private:
        ConstantStringContainer(PoolObject* pool) : pool(pool) {}

    public:
        REALLY_INLINE static ConstantStringContainer* create(MMgc::GC* gc, size_t extra, PoolObject* pool)
        {
            return new (gc, MMgc::kExact, extra) ConstantStringContainer(pool);
        }

        virtual bool gcTrace(MMgc::GC* gc, size_t cursor);

        PoolObject* const pool;

        // The real length is max(1,pool->constantStringCount).
        ConstantStringData data[1];
    };

    /**
     * The PoolObject class is a container for the pool of resources
     * decoded from an ABC file: the constant pool, the methods
     * defined in the ABC, the classes defined in the ABC, and so on.
     */
    class GC_CPP_EXACT(PoolObject, MMgc::GCFinalizedObject)
    {
        friend class AbcParser;
        friend class ConstantStringContainer;
        friend class DomainMgr;

        PoolObject(AvmCore* core, ScriptBuffer& sb, const uint8_t* startpos, ApiVersion apiVersion);

    public:
        static PoolObject* create(AvmCore* core, ScriptBuffer& sb, const uint8_t* startpos, ApiVersion apiVersion);

        ~PoolObject();

        int32_t getAPI();

        ApiVersion getApiVersion();

        void initPrecomputedMultinames();
        const Multiname* precomputedMultiname(int32_t index);

        static void destroyPrecomputedMultinames(ExactStructContainer<HeapMultiname>* self);

        // search metadata record at meta_pos for name, return true if present
        bool hasMetadataName(const uint8_t* meta_pos, const String* name);
        const uint8_t* getMetadataInfoPos(uint32_t index);

        /** deferred parsing */
        void parseMultiname(const uint8_t *pos, Multiname& m) const;

        Traits* resolveTypeName(uint32_t index, const Toplevel* toplevel, bool allowVoid=false);
        Traits* resolveTypeName(const uint8_t*& pc, const Toplevel* toplevel, bool allowVoid=false);

        void resolveBindingNameNoCheck(uint32_t index, Multiname &m, const Toplevel* toplevel) const;
        void resolveBindingNameNoCheck(const uint8_t* &p, Multiname &m, const Toplevel* toplevel) const;

        Traits* resolveParameterizedType(const Toplevel* toplevel, Traits* base, Traits* type_param) const;

        void parseMultiname(Multiname& m, uint32_t index) const;

        Namespacep getNamespace(int32_t index) const;
        NamespaceSetp getNamespaceSet(int32_t index) const;
        bool hasString(int32_t index) const;
        Stringp getString(int32_t index) const;

        Atom getLegalDefaultValue(const Toplevel* toplevel, uint32_t index, CPoolKind kind, Traits* t);
        static bool isLegalDefaultValue(BuiltinType bt, Atom value);
        
#ifdef VMCFG_FLOAT
        bool hasFloatSupport() const        { return version >= 0x002F0010; /* (47<<16|16), i.e. Cyril, major v.47, minor v.16 */ }
#endif 
        bool hasExceptionSupport() const    { return version != 0x002E000F; /* (46<<16|15), i.e. major v.46, minor v.15 */ }

        ScriptBuffer code();
        bool isCodePointer(const uint8_t* pos);

        int32_t uniqueId() const;
        void    setUniqueId(int32_t val);

    public:
        uint32_t classCount() const;
        Traits* getClassTraits(uint32_t i) const;

        uint32_t scriptCount() const;
        Traits* getScriptTraits(uint32_t i) const;

        uint32_t methodCount() const;
        MethodInfo* getMethodInfo(uint32_t i) const;
#ifdef DEBUGGER
        DebuggerMethodInfo* getDebuggerMethodInfo(uint32_t i) const;
#endif
        Stringp getMethodInfoName(uint32_t i);

        void dynamicizeStrings();

#ifdef AVMPLUS_VERBOSE
        bool isVerbose(uint32_t flag, MethodInfo* mi=NULL);
#endif

    private:
        void setupConstantStrings(uint32_t count);

    // ------------------------ DATA SECTION BEGIN
    GC_DATA_BEGIN(PoolObject)

    public:
        AvmCore * const core;

        /** constants */
        DataList<int32_t>       GC_STRUCTURE(cpool_int);
        DataList<uint32_t>      GC_STRUCTURE(cpool_uint);
        GCList<GCDouble>        GC_STRUCTURE(cpool_double);
#ifdef VMCFG_FLOAT
        GCList<GCFloat>         GC_STRUCTURE(cpool_float); // We allocate 8 bytes/float in heap.
        GCList<GCFloat4>        GC_STRUCTURE(cpool_float4);
#endif
        RCList<Namespace>       GC_STRUCTURE(cpool_ns);
        GCList<NamespaceSet>    GC_STRUCTURE(cpool_ns_set);
#ifdef VMCFG_HALFMOON
        // tables of pinned pointers for compiler-generated constants.
        GCList<GCDouble>        GC_STRUCTURE(cpool_const_double);
        RCList<String>          GC_STRUCTURE(cpool_const_string);
#endif

#ifndef AVMPLUS_64BIT
        // lists to keep int/uint atoms "sticky".
        // @todo this can/should go away when we convert to 64-bit Box atoms.
        AtomList GC_STRUCTURE(cpool_int_atoms);
        AtomList GC_STRUCTURE(cpool_uint_atoms);
#endif

        DataList<uint32_t> GC_STRUCTURE(cpool_mn_offsets);

        /** metadata -- ptrs into ABC, not gc-allocated */
        UnmanagedPointerList<const uint8_t*> GC_STRUCTURE(metadata_infos);

        /** domain */
        GCMember<Domain> GC_POINTER(domain);

        /** # of elements in metadata array */
        uint32_t metadataCount;

        /** # of elements in cpool array */
        uint32_t constantCount;
        uint32_t constantIntCount;
        uint32_t constantUIntCount;
        uint32_t constantDoubleCount;
#ifdef VMCFG_FLOAT
        uint32_t constantFloatCount;
        uint32_t constantFloat4Count;
#endif
        uint32_t constantStringCount;
        uint32_t constantNsCount;
        uint32_t constantNsSetCount;

    private:
        GCMember<ExactStructContainer<HeapMultiname> > GC_POINTER(precompNames);   // a GCFinalizedObject

    public:
#ifdef VMCFG_NANOJIT
        CodeMgr* codeMgr;   // points to unmanaged memory and so, not traced
#endif

    private:
        int32_t version;

        // "loaded" Traits/Scripts are the Traits/ScriptEnvs that are actually
        // defined in this Domain. "cached" Traits/Scripts are the ones that
        // actually should be used for a given name lookup; the cached versions
        // take precedence over the loaded ones (on a freeze-on-first-use basis)
        // to ensure that the types associated with a name can't change as new
        // Domains are loaded. See DomainMgr for more info.
        GCMember<MultinameTraitsHashtable>          GC_POINTER(m_loadedTraits);
        GCMember<MultinameTraitsHashtable>          GC_POINTER(m_cachedTraits);
        GCMember<MultinameMethodInfoHashtable>      GC_POINTER(m_loadedScripts);
        GCMember<MultinameMethodInfoHashtable>      GC_POINTER(m_cachedScripts);

        GCMember<ScriptBufferImpl>                  GC_POINTER(_code);
        const uint8_t * const                       _abcStart;
        // start of static ABC string data
        const uint8_t *                             _abcStringStart;
        // points behind end of ABC string data - see AbcParser.cpp
        const uint8_t *                             _abcStringEnd;

        GCMember<ConstantStringContainer>           GC_POINTER(_abcStrings);                // The length is constantStringCount
        GCList<Traits>                              GC_STRUCTURE(_classes);
        GCList<Traits>                              GC_STRUCTURE(_scripts);
        GCList<MethodInfo>                          GC_STRUCTURE(_methods);
#ifdef DEBUGGER
        GCList<DebuggerMethodInfo>                  GC_STRUCTURE(_method_dmi);
#endif
        // Only allocated & populated if core->config.methodName is true.
        // Indexed by MethodInfo::_method_id, if the value is positive, it's an index into cpool_string;
        // if negative, an index into cpool_mn.
        // Always safe because those indices are limited to 30 bits.
        DataList<int32_t>                           GC_STRUCTURE(_method_name_indices);
        ApiVersion const                            _apiVersion;
        int32_t                                     _uniqueId; // _uniqueId + mi->method_id() produces a unique id for methods

    public:
    #ifdef AVMPLUS_VERBOSE
        uint32_t                    verbose_vb;
    #endif
        // @todo, privatize & make into bitfield (requires API churn)
        bool                        isBuiltin;  // true if this pool is baked into the player.  used to control whether callees will set their context.

    #ifdef VMCFG_AOT
        const AOTInfo* aotInfo; // points to non-gc memory
        MMgc::GCRoot *aotRoot;
    #endif

    GC_DATA_END(PoolObject)
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_PoolObject__ */
