/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_MethodInfo__
#define __avmplus_MethodInfo__

#ifdef AVMPLUS_VERBOSE
    #define verbose_only(...)        __VA_ARGS__
#else
    #define verbose_only(...)
#endif

#ifdef verify
#undef verify
#endif

namespace avmplus
{
     class ScopeOrTraits : public MMgc::GCInlineObject
     {
     private:
        static const uintptr_t IS_SCOPE = 0x01;
     private:
        uintptr_t _scopeOrTraits;   // if LSB clr, Traits*   if LSB set, ScopeTypeChain*
     public:
         explicit ScopeOrTraits(Traits* t);
         bool hasScope() const;
         Traits* getTraits() const;
         const ScopeTypeChain* getScope() const;
         void setTraits(MMgc::GC* gc, void* container, Traits* t);
         void setScope(MMgc::GC* gc, void* container, const ScopeTypeChain* s);

         REALLY_INLINE void gcTrace(MMgc::GC* gc) { gc->TraceLocation(&_scopeOrTraits); }
     };

#ifdef DEBUGGER
    class AbcFile;
    class GC_CPP_EXACT(DebuggerMethodInfo, MMgc::GCTraceableObject)
    {
    private:
        explicit DebuggerMethodInfo(int32_t _local_count, uint32_t _codeSize, int32_t _max_scopes);

    public:
        static DebuggerMethodInfo* create(AvmCore* core, int32_t _local_count, uint32_t _codeSize, int32_t _max_scopes);

        GC_DATA_BEGIN(DebuggerMethodInfo)

        GCMember<AbcFile>       GC_POINTER(file);               // the abc file from which this method came
        int32_t                 firstSourceLine;    // source line number where function starts
        int32_t                 lastSourceLine;     // source line number where function ends
        int32_t                 offsetInAbc;        // offset in abc file
        uint32_t                codeSize;           // abc size pre-jit, native size post jit
        const int32_t           local_count;        // FIXME: merge with localCount above; this one may be visible to a debugger?
        const int32_t           max_scopes;         // FIXME: merge with maxScopeDepth above; this one is not used by the VM but may be visible to a debugger?
        Stringp                 GC_POINTERS_SMALL(localNames[1], local_count);      // array of names for args and locals in framep order, written with explicit WBRC (actually local_count)

        GC_DATA_END(DebuggerMethodInfo)
    };
#endif

    class MethodSignature;
    class Deoptimizer;

    /**
     * MethodInfo is the base class for all functions that
     * can be executed by the VM: Actionscript functions,
     * native functions, etc.
     */
    class GC_CPP_EXACT_WITH_HOOK(MethodInfo, MethodInfoProcHolder)
    {
        friend class CodegenLIR;
        friend class BaseExecMgr;
        friend class OSR;
    public:
        enum InitMethodStub { kInitMethodStub };

    private:
        // ctor for all normal methods.
        MethodInfo(int32_t _method_id,
                   PoolObject* pool,
                   const uint8_t* abc_info_pos,
                   uint8_t abcFlags,
                   const NativeMethodInfo* native_info);

        // special ctor for the methodinfo generated for slot-initialization when no init method is present
        MethodInfo(InitMethodStub, Traits* declTraits);

#ifdef VMCFG_AOT
        MethodInfo(InitMethodStub, Traits* declTraits, const NativeMethodInfo* native_info, const AvmThunkNativeHandler& handler, int32_t method_id);
#endif

    public:
        REALLY_INLINE static MethodInfo* create(MMgc::GC* gc,
                                                int32_t _method_id,
                                                PoolObject* pool,
                                                const uint8_t* abc_info_pos,
                                                uint8_t abcFlags,
                                                const NativeMethodInfo* native_info)
        {
            return new (gc, MMgc::kExact) MethodInfo(_method_id, pool, abc_info_pos, abcFlags, native_info);
        }

        REALLY_INLINE static MethodInfo* create(MMgc::GC* gc, InitMethodStub stub, Traits* declTraits)
        {
            return new (gc, MMgc::kExact) MethodInfo(stub, declTraits);
        }

#ifdef VMCFG_AOT
        REALLY_INLINE static MethodInfo* create(MMgc::GC* gc, InitMethodStub stub, Traits* declTraits, const NativeMethodInfo* native_info, const AvmThunkNativeHandler& handler, int32_t method_id)
        {
            return new (gc, MMgc::kExact) MethodInfo(stub, declTraits, native_info, handler, method_id);
        }
#endif

        bool usesCallerContext() const;

        // Builtin + non-native functions always need the dxns code emitted
        // Builtin + native functions have flags to specify if they need the dxns code
        bool usesDefaultXmlNamespace() const;

        void resolveSignature(const Toplevel* toplevel);

#ifdef DEBUGGER
    public:
        Atom boxOneLocal(FramePtr src, int32_t srcPos, const uint8_t* sstArr);
        void unboxOneLocal(Atom src, FramePtr dst, int32_t dstPos, const uint8_t* sstArr);

        void boxLocals(FramePtr src, int32_t srcPos, const uint8_t* sstArr, Atom* dest, int32_t destPos, int32_t length);
        void unboxLocals(const Atom* src, int32_t srcPos, const uint8_t* sstArr, FramePtr dest, int32_t destPos, int32_t length);

        void setFile(AbcFile* file);
        void setRegName(int32_t index, Stringp name);

        Stringp getArgName(int32_t index);
        Stringp getLocalName(int32_t index);

        AbcFile* file() const;
        int32_t firstSourceLine() const;
        int32_t lastSourceLine() const;
        int32_t offsetInAbc() const;
        uint32_t codeSize() const;
        int32_t local_count() const;
        int32_t max_scopes() const;

        void updateSourceLines(int32_t linenum, int32_t offset);

    private:
        Stringp getRegName(int32_t index) const;
        DebuggerMethodInfo* dmi() const;
#endif

    public:
        void makeIntoPrototypeFunction(const Toplevel* toplevel, const ScopeTypeChain* fscope);
        bool makeMethodOf(Traits* type);
        bool isConstructor() const;

        uint32_t hasExceptions() const;
        uint32_t hasMethodBody() const;
        uint32_t hasOptional() const;
        uint32_t isNative() const;
        uint32_t isNonInterruptible();
        uint32_t isResolved() const;
        uint32_t isFinal() const;
        uint32_t needActivation() const;
        uint32_t needArguments() const;
        uint32_t needClosure() const;
        uint32_t needRest() const;
        uint32_t lazyRest() const;   // Valid only during pass2 of verification or later
        uint32_t needRestOrArguments() const;
        uint32_t setsDxns() const;
        uint32_t isStaticInit() const;
        uint32_t hasFailedJit() const;
        uint32_t isInterpreted() const;
        uint32_t unboxThis() const;
        uint32_t onlyUntypedParameters() const;

        void setUnboxThis();
        void setStaticInit();
        void setHasFailedJit();
        void setHasExceptions();
        void setLazyRest();
        void setNeedsDxns();
        void setFinal();
        void setOverride();
        void makeNonInterruptible();
        void setKind(TraitKind kind);

#ifdef VMCFG_AOT
        uint32_t isAotCompiled() const;
        void setAotCompiled(const AvmThunkNativeHandler& handler);
#endif
        uint32_t osrEnabled() const;
        void setOSR(uint32_t threshold);

    public:

        Stringp FASTCALL getMethodNameWithTraits(Traits* declaringTraits, bool includeAllNamespaces = false) const;
        Stringp FASTCALL getMethodName(bool includeAllNamespaces = false) const;

        PrintWriter& print(PrintWriter& prw) const;
#ifdef DEBUGGER
        /**
         * Basically the same as AvmPlusScriptableObject::bytesUsed().
         */
        uint64_t bytesUsed();
#endif

    public:

        PoolObject* pool() const;
#ifdef VMCFG_AOT
        AvmThunkNativeMethodHandler handler_method() const;
        AvmThunkNativeFunctionHandler handler_function() const;
#endif

        const uint8_t* abc_body_pos() const;
        void set_abc_body_pos(const uint8_t* p);
        uint32_t parse_code_length() const; // Parse code_len from ABC.

        ExceptionHandlerTable* abc_exceptions() const;
        void set_abc_exceptions(MMgc::GC* gc, ExceptionHandlerTable* e);

    #ifdef VMCFG_WORDCODE
        ExceptionHandlerTable* word_code_exceptions() const;
        void set_word_code_exceptions(MMgc::GC* gc, ExceptionHandlerTable* e);
        const uintptr_t* word_code_start() const;
        void set_word_code(MMgc::GC* gc, TranslatedCode* translated_code);
    #endif

        int32_t  method_id() const;
        uint32_t unique_method_id() const;

        Traits* declaringTraits() const;
        const ScopeTypeChain* declaringScope() const;
        Traits* activationTraits() const;
        const ScopeTypeChain* activationScope() const;

        void init_activationTraits(Traits* t);
        void init_activationScope(const ScopeTypeChain*);

        MethodSignaturep getMethodSignature();
        void update_max_stack(int32_t max_stack);
#ifdef VMCFG_FLOAT
        void forceLargeVarSize();
        int32_t varShift();  // effectively, log2(varSize), i.e. 3 or 4. See exec.h for details.
#endif // VMCFG_FLOAT

        /**
         * Determine whether a function is trivial.  A function is (conservatively) trivial
         * if all these conditions hold:
         *
         *   - it always terminates
         *   - it always terminates by returning, never by throwing exceptions
         *     (apart from system errors like stack overflow and out-of-memory)
         *   - it always returns 'undefined'
         *   - it does not update any non-local locations
         *
         * Stronger definitions of triviality are possible - for example, the condition on
         * returning 'undefined' is a hack to avoid dealing with data flow analysis and
         * determining whether RETURNVALUE will throw an exception - and some functions that
         * are trivial by the above definition may still not be recognized by the implementation,
         * obviously.  The only condition is that if isTrivial returns true then the function
         * is trivial by the above definition.
         */
        bool isTrivial();

    private:
        MethodSignature* FASTCALL _getMethodSignature();
        MethodSignature* FASTCALL _buildMethodSignature(const Toplevel* toplevel);
        void _installRefToResolvedMethodSignature(MethodSignature* ms);

        bool computeIsTrivial();

    private:
        struct NativeInfo
        {
            GprMethodProc thunker;
#if defined(VMCFG_TELEMETRY_SAMPLER) && !defined(VMCFG_AOT)
            GprMethodProc samplerThunker;
#endif
#ifdef VMCFG_AOT
            AvmThunkNativeHandler handler;
#endif
        };

        struct AbcInfo
        {
            const uint8_t*          body_pos;
            ExceptionHandlerTable*  exceptions;     // we write this once, in Verifier, with an explicit WB.  so no GCMember<>.
            uint32_t                countdown;      // Counts down calls plus backedges.
    #ifdef VMCFG_WORDCODE
            struct
            {
                TranslatedCode*         translated_code;    // The object that contains the code pointed to by body_pos, written with explicit WB
                // We write this once, in WordcodeTranslator, with an explicit WB.  so no GCMember<>.
                // The contents are the same as the 'exceptions' structure above, except the 'from', 'to', and 'target' fields.
                ExceptionHandlerTable*  exceptions;
            } word_code;
    #endif
        };

        void gcTraceHook_MethodInfo(MMgc::GC* gc);

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(MethodInfo)

    private:
        GCMember<MMgc::GCWeakRef> GC_POINTER(_msref); // our MethodSignature
        ScopeOrTraits             GC_STRUCTURE(_declarer);
        ScopeOrTraits             GC_STRUCTURE(_activation);
        PoolObject* const         GC_POINTER(_pool);
        const uint8_t* const      _abc_info_pos;      // pointer to abc MethodInfo record
        const int                 _method_id;
        union
        // This union is traced by the gcTraceHook()
        {
            NativeInfo            _native;            // stuff used only for Native methods (formerly in NativeMethod)
            AbcInfo               _abc;               // stuff used only for bytecode methods (formerly in MethodInfo)
        };
#ifdef AVMPLUS_SAMPLER
        mutable GCMember<String>  GC_POINTER(_methodName);
#endif

#ifdef VMCFG_HALFMOON
    public:
        // Per function metadata for deoptimization
        // Should be allocated separtely from MethodInfo, because it need only
        // exit for functions compiled with speculative code.
        // TODO: we're only getting away w/o GC here because we never free these.
        Deoptimizer* _deoptimizer;
        Deoptimizer* _armed_deoptimizers;
#endif

    private:
        // -------- FLAGS SECTION
        // (Set in ABC) need arguments[0..argc].
        uint32_t                _needArguments:1;

        // (Set in ABC) need activation object
        uint32_t                _needActivation:1;

        // (Set in ABC) need arguments[param_count+1..argc]
        uint32_t                _needRest:1;

        // (Set in ABC) has optional parameters
        uint32_t                _hasOptional:1;

        // (Set in ABC) allow extra args, but dont capture them
        uint32_t                _ignoreRest:1;

        // (Set in ABC) method is native
        uint32_t                _isNative:1;

        // (Set in ABC) method sets default namespace
        uint32_t                _setsDxns:1;

        // (Set in ABC)  method has table for parameter names
        uint32_t                _hasParamNames:1;

        // Set iff this is a getter.
        uint32_t                _isGetter:1;

        // Set iff this is a setter.
        uint32_t                _isSetter:1;

        // Set iff this is an override of an inherited method.
        uint32_t                _isOverride:1;

        uint32_t                _isNonInterruptible:1;
        uint32_t                _unboxThis:1;
        uint32_t                _needsCodeContext:1;
        uint32_t                _needsDxns:1;
        uint32_t                _hasExceptions:1;

#ifdef VMCFG_VERIFYALL
        uint32_t                _isVerified:1;
        uint32_t                _isVerifyPending:1;
#endif

        // indicates method is final, no overrides allowed
        uint32_t                _isFinal:1;

        // indicates the function is a method, that pushes the
        // receiver object onto the scope chain at method entry
        uint32_t                _needClosure:1;

        // cleared to indicate that a function has no bytecode body
        uint32_t                _hasMethodBody:1;

        // set once the signature types have been resolved and
        // override signatures have been checked
        uint32_t                _isResolved:1;

        // set to indictate that a function is a static initializer
        uint32_t                _isStaticInit:1;

        // set to indicate that a function has been compiled
        // to native code by the jit compiler
        uint32_t                _isJitImpl:1;

        // set to indicate that an attempted jit compilation has failed
        uint32_t                _hasFailedJit:1;

        // true if execution mechanism is the interpreter
        uint32_t                _isInterpImpl:1;

        // see isTrivial() for definition of these two flags
        uint32_t                _isTrivial:1;
        uint32_t                _isNonTrivial:1;

        // Set by setLazyRest when the verifier discovers that the method's rest array
        // or arguments array can be allocated lazily (usually not at all).  Only makes
        // sense in combination with _needRest or _needArguments.
        uint32_t                _lazyRest:1;

        // Set by _buildMethodSignature if all fixed or optional parameters
        // to the function are untyped.  (We use this to optimize the 'arguments'
        // array, see RestArgAnalyzer.)
        uint32_t                _onlyUntypedParameters:1;

// begin AVMPLUS_UNCHECKED_HACK
        // Note, this means "makeIntoPrototypeFunction has been called on me",
        // *not* "I am a function on a prototype object".
        uint32_t                _isProtoFunc:1;
        uint32_t                _isUnchecked:1;
// end AVMPLUS_UNCHECKED_HACK

        // Set if makeMethodOf() has ever been called for this MethodInfo.
        uint32_t                _hasMakeMethodOfBeenCalled:1;

#ifdef VMCFG_AOT
        uint32_t                _isAotCompiled:1;
#endif

        // If true, Function.apply & call() use a fast path that only works
        // for JIT and Native code, not interpreted code.
        uint32_t                _apply_fastpath:1;

        // If true, execution policy permits OSR for this method.
        // This does not guarantee that OSR will apply, however.  See OSR::isSupported() for details.
        uint32_t                _osr_enabled:1;

#ifdef VMCFG_FLOAT
        // if true, the function operates on float4_t values, and needs a double VARSIZE
        uint32_t                _has128bitLocals:1;
#endif
        GC_DATA_END(MethodInfo)
    // ------------------------ DATA SECTION END
    };

    class GC_CPP_EXACT_WITH_HOOK(MethodSignature, QCachedItem)
    {
        friend class MethodInfo;
    public:
        REALLY_INLINE static MethodSignature* create(MMgc::GC* gc, size_t extra, int32_t param_count)
        {
            return new (gc, MMgc::kExact, extra) MethodSignature(param_count);
        }

    private:
        MethodSignature(int32_t param_count);

        union AtomOrType
        {
            Traits* paramType;
            Atom defaultValue;
        };

    public:
        Traits* returnTraits() const;
        BuiltinType returnTraitsBT() const;

        int32_t param_count() const;
        int32_t optional_count() const;
        int32_t rest_offset() const;

        int32_t max_stack() const;
        int32_t local_count() const;
        int32_t max_scope() const;
        int32_t frame_size() const;
        int32_t stack_base() const;
        int32_t scope_base() const;

    #ifdef VMCFG_WORDCODE
    #else
        const uint8_t* abc_code_start() const;
    #endif

        int32_t requiredParamCount() const;

        Traits* paramTraits(int32_t i) const;
        BuiltinType paramTraitsBT(int32_t i) const;
        Atom getDefaultValue(int32_t i) const;

        bool allowExtraArgs() const;
        bool argcOk(int32_t argc) const;
        void boxArgs(AvmCore* core, int32_t argc, const uint32_t* ap, Atom* out) const;

    private:
        void gcTraceHook_MethodSignature(MMgc::GC* gc);

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(MethodSignature)

    private:
        Traits*     GC_POINTER(_returnTraits);      // written with explicit WB
    #ifdef VMCFG_WORDCODE
    #else
        const uint8_t*  _abc_code_start; // start of ABC body
    #endif
        int         _param_count;       // number of declared parameters including optionals
        int         _optional_count;    // last optional_count params are optional
        int         _rest_offset;       // offset to first rest arg, including the instance parameter. this is sum(sizeof(paramType(0..N)))
        int         _max_stack;         // abc-only: max stack
        int         _local_count;       // abc-only: maximum number of local registers
        int         _max_scope;         // abc-only: maximum depth of local scope stack
        int         _frame_size;        // abc-only: total size of frame in number of Atoms, derived from other values above
        // Note that these two flags are "bool" because we will have to pad the struct anyway;
        // might as well avoid per-bit access as long as we have the space.
        bool        _isNative;          // dupe of owner's flag of same name.
        bool        _allowExtraArgs;    // == _needRest | _needArguments | _ignoreRest
        AtomOrType  _args[1];           // lying, actually 1+param_count+optional_count.  Traced by gcTraceHook().

        GC_DATA_END(MethodSignature)
    // ------------------------ DATA SECTION END
    };


    /**
     * MethodRecognizer is an interface used to determine if a MethodInfo matches
     * some criteria (e.g. a name).
     *
     * It also doubles as factory to produce recognizers based on a string input.
     */
    class MethodRecognizer
    {
    public:
        static MethodRecognizer* parse(const char** s, char separator);

        virtual bool matches(const MethodInfo* m) const = 0;

        virtual ~MethodRecognizer() {}
        verbose_only( virtual PrintWriter& print(PrintWriter& prw) const = 0; )
    };

    /**
     * A MethodRecognizer that performs an exact match comparision against a method name
     */
    class MethodNameRecognizer : /* implements */ public MethodRecognizer
    {
    public:
        MethodNameRecognizer(const char* pattern, size_t length);
        virtual bool matches(const MethodInfo* m) const;
        verbose_only( virtual PrintWriter& print(PrintWriter& prw) const; )

    private:
        const char* _pattern;
        size_t _length;
    };

    /**
     * A MethodRecognizer that performs an Perl Regular Expression comparision against a method name
     */
    class MethodNameRegExRecognizer : /* implements */ public MethodRecognizer
    {
    public:
        MethodNameRegExRecognizer(const char* pattern, size_t length);
        virtual ~MethodNameRegExRecognizer();
        virtual bool matches(const MethodInfo* m) const;
        verbose_only( virtual PrintWriter& print(PrintWriter& prw) const; )

    private:
        const char* _pattern;
        void* _regex;
        bool _invalid;  // bad expression?
    };

    /**
     * A MethodRecognizer that performs compares a method's unique id against a range of id's.
     */
    class MethodIdRecognizer : /* implements */ public MethodRecognizer
    {
    public:
        MethodIdRecognizer(size_t low, size_t high);
        virtual bool matches(const MethodInfo* m) const;
        verbose_only( virtual PrintWriter& print(PrintWriter& prw) const; )

    private:
        size_t _low, _high;
    };

    /** Utility function for accessing possibly-unaligned doubles */
    double unpack_double(const void* src);

    /**
     * Utility function to access values on the stack, then box them as
     * Atoms, including handling of possibly unaligned double and float/float4
     * values.  Similar to makeAtom() in jit-calls.h (keep them sync'd).
     */
    Atom nativeLocalToAtom(AvmCore* core, void* src, SlotStorageType sst);
}

#undef verbose_only

#endif /* __avmplus_MethodInfo__ */
