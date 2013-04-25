/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_DomainEnv__
#define __avmplus_DomainEnv__

namespace avmplus
{

    // ScriptEnvMap is just syntactic sugar (on top of Hashtable) to simplify
    // code in DomainMgr. If/when we add a true generic Map/Hashtable type,
    // this can be replaced; for now, it serves simply to ensure that the
    // proper Atom<->GenericObject transmutation is in place for this map.
    class GC_CPP_EXACT(ScriptEnvMap, MMgc::GCFinalizedObject)
    {
    private:
        REALLY_INLINE ScriptEnvMap(MMgc::GC* gc)
        {
            ht.initialize(gc);
        }
    
    public:
        REALLY_INLINE static ScriptEnvMap* create(MMgc::GC* gc)
        {
            return new (gc, MMgc::kExact) ScriptEnvMap(gc);
        }

        REALLY_INLINE void add(MethodInfo* mi, ScriptEnv* se)
        {
            ht.add(AvmCore::genericObjectToAtom(mi), AvmCore::genericObjectToAtom(se));
        }
        
        REALLY_INLINE ScriptEnv* get(MethodInfo* mi)
        {
            return (ScriptEnv*)AvmCore::atomToGenericObject(ht.get(AvmCore::genericObjectToAtom(mi)));
        }

    GC_DATA_BEGIN(ScriptEnvMap)
    protected:
        InlineHashtable GC_STRUCTURE(ht);
    GC_DATA_END(ScriptEnvMap)

    };

    // an ABC
    class GC_CPP_EXACT(DomainEnv, MMgc::GCFinalizedObject)
    {
        friend class DomainMgr;
        friend class MopsRangeCheckFilter;
    private:
        DomainEnv(AvmCore* core, Domain* domain, DomainEnv* base, uint32_t baseCount);
    public:
        static DomainEnv* newDomainEnv(AvmCore* core, Domain* domain, DomainEnv* base);
        virtual ~DomainEnv();

        inline Domain* domain() const { return m_domain; }
        // see note in newDomainEnv about why this is always valid, even if m_baseCount == 1
        inline DomainEnv* base() const { return m_bases[1]; }

        Toplevel* toplevel() const;
        void setToplevel(Toplevel *t) { m_toplevel = t; }

        /**
         * global memory access glue
         */
        enum {
            // Must be at least 8 [ie, largest single load/store op we provide]
            // But using larger values allows us to collapse a lot of range checks in the JIT
            GLOBAL_MEMORY_MIN_SIZE = 1024
        };

        REALLY_INLINE uint8_t* globalMemoryBase() const { return m_globalMemoryBase; }
        REALLY_INLINE uint32_t globalMemorySize() const { return m_globalMemorySize; }

        // global memory object accessor
        ByteArrayObject* get_globalMemory() const { return m_globalMemoryProviderObject; }
        bool set_globalMemory(ByteArrayObject* providerObject);

        void notifyGlobalMemoryChanged(uint8_t* newBase, uint32_t newSize);

    private:
        // subscribes to the memory object "mem" such that "mem" will call our
        // notifyGlobalMemoryChanged when it moves
        bool globalMemorySubscribe(ByteArrayObject* providerObject);
        // stops "mem" from notifying us if it moves
        bool globalMemoryUnsubscribe(ByteArrayObject* providerObject);

    private:

        // allocate "scratch" as a struct to make it easier to allocate pre-zeroed
        struct Scratch
        {
            uint8_t scratch[GLOBAL_MEMORY_MIN_SIZE];
        };

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(DomainEnv)

    private:
        // This is used by DomainMgr to simplify lookups of ScriptEnv definitions
        // by name. See DomainMgr for more info.
        GCMember<ScriptEnvMap>          GC_POINTER(m_scriptEnvMap);
        GCMember<Domain>                GC_POINTER(  m_domain);       // Domain associated with this DomainEnv
        GCMember<Toplevel>              GC_POINTER(  m_toplevel);
        // scratch memory to use if the memory object is NULL...
        // allocated via mmfx_new, which is required by nanojit
        Scratch*                        m_globalMemoryScratch;
        // backing store / current size for global memory
        uint8_t*                        m_globalMemoryBase;
        uint32_t                        m_globalMemorySize;
        // the actual memory object (can be NULL)
        GCMember<ByteArrayObject>       GC_POINTER(  m_globalMemoryProviderObject);
        // note that m_baseCount is actually the number of bases, plus one:
        // we always add ourself (!) to the front of the list, to simplify
        // processing in DomainMgr.
        uint32_t const                  m_baseCount; // number of entries in m_bases
        DomainEnv*                      GC_POINTERS_SMALL(m_bases[1], m_baseCount);

        GC_DATA_END(DomainEnv)
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_DomainEnv__ */
