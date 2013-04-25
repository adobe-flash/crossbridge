/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __nanojit_Assembler__
#define __nanojit_Assembler__


namespace nanojit
{
    /**
     * Some notes on this Assembler (Emitter).
     *
     *   The class RegAlloc is essentially the register allocator from MIR
     *
     *   The Assembler class parses the LIR instructions starting at any point and converts
     *   them to machine code.  It does the translation using expression trees which are simply
     *   LIR instructions in the stream that have side-effects.  Any other instruction in the
     *   stream is simply ignored.
     *   This approach is interesting in that dead code elimination occurs for 'free', strength
     *   reduction occurs fairly naturally, along with some other optimizations.
     *
     *   A negative is that we require state as we 'push' and 'pop' nodes along the tree.
     *   Also, this is most easily performed using recursion which may not be desirable in
     *   the mobile environment.
     *
     */

    #define STACK_GRANULARITY        sizeof(void *)

    class MetaDataWriter;

    // Basics:
    // - 'entry' records the state of the native machine stack at particular
    //   points during assembly.  Each entry represents four bytes.
    //
    // - Parts of the stack can be allocated by LIR_allocp, in which case each
    //   slot covered by the allocation contains a pointer to the LIR_allocp
    //   LIns.
    //
    // - The stack also holds spilled values, in which case each slot holding
    //   a spilled value (one slot for 32-bit values, two slots for 64-bit
    //   values) contains a pointer to the instruction defining the spilled
    //   value.
    //
    // - Each LIns has a "reservation" which includes a stack index,
    //   'arIndex'.  Combined with AR, it provides a two-way mapping between
    //   stack slots and LIR instructions.
    //
    // - Invariant: the two-way mapping between active stack slots and their
    //   defining/allocating instructions must hold in both directions and be
    //   unambiguous.  More specifically:
    //
    //   * An LIns can appear in at most one contiguous sequence of slots in
    //     AR, and the length of that sequence depends on the opcode (1 slot
    //     for instructions producing 32-bit values, 2 slots for instructions
    //     producing 64-bit values, N slots for LIR_allocp).
    //
    //   * An LIns named by 'entry[i]' must have an in-use reservation with
    //     arIndex==i (or an 'i' indexing the start of the same contiguous
    //     sequence that 'entry[i]' belongs to).
    //
    //   * And vice versa:  an LIns with an in-use reservation with arIndex==i
    //     must be named by 'entry[i]'.
    //
    //   * If an LIns's reservation names has arIndex==0 then LIns should not
    //     be in 'entry[]'.
    //
    class AR
    {
    private:
        uint32_t        _highWaterMark;                 /* index of highest entry used since last clear() */
        LIns*           _entries[ NJ_MAX_STACK_ENTRY ]; /* maps to 4B contiguous locations relative to the frame pointer.
                                                            NB: _entries[0] is always unused */

        #ifdef _DEBUG
        static LIns* const BAD_ENTRY;
        #endif

        bool isEmptyRange(uint32_t start, uint32_t nStackSlots) const;
        static uint32_t nStackSlotsFor(LIns* ins);

    public:
        AR();

        uint32_t stackSlotsNeeded() const;

        void clear();
        void freeEntryAt(uint32_t i);
        uint32_t reserveEntry(LIns* ins); /* return 0 if unable to reserve the entry */

        #ifdef _DEBUG
        void validateQuick();
        void validateFull();
        void validate();
        bool isValidEntry(uint32_t idx, LIns* ins) const; /* return true iff idx and ins are matched */
        void checkForResourceConsistency(const RegAlloc& regs);
        void checkForResourceLeaks() const;
        #endif

        class Iter
        {
        private:
            const AR& _ar;
            // '_i' points to the start of the entries for an LIns, or to the first NULL entry.
            uint32_t _i;
        public:
            inline Iter(const AR& ar) : _ar(ar), _i(1) { }
            bool next(LIns*& ins, uint32_t& nStackSlots, int32_t& offset);             // get the next one (moves iterator forward)
        };
    };

    inline AR::AR()
    {
         _entries[0] = NULL;
         clear();
    }

    inline /*static*/ uint32_t AR::nStackSlotsFor(LIns* ins)
    {
        uint32_t n = 0;
        if (ins->isop(LIR_allocp)) {
            n = ins->size() >> 2;
        } else {
            switch (ins->retType()) {
            case LTy_I:   n = 1;          break;
            case LTy_F:   n = 1;          break; 
            case LTy_F4:  n = 4;          break; 
            CASE64(LTy_Q:)
            case LTy_D:   n = 2;          break;
            case LTy_V:   NanoAssert(0);  break;
            default:      NanoAssert(0);  break;
            }
        }
        return n;
    }

    inline uint32_t AR::stackSlotsNeeded() const
    {
        // NB: _highWaterMark is an index, not a count
        return _highWaterMark + 1;
    }

    #ifndef AVMPLUS_ALIGN16
        #ifdef _MSC_VER
            #define AVMPLUS_ALIGN16(type) __declspec(align(16)) type
        #else
            #define AVMPLUS_ALIGN16(type) type __attribute__ ((aligned (16)))
        #endif
    #endif

    class Noise
    {
        public:
            virtual ~Noise() {}

            // produce a random number from 0-maxValue for the JIT to use in attack mitigation
            virtual uint32_t getValue(uint32_t maxValue) = 0;
    };

    // error codes
    enum AssmError
    {
         None = 0
        ,StackFull
        ,UnknownBranch
        ,BranchTooFar
    };

    typedef SeqBuilder<NIns*> NInsList;
    typedef HashMap<NIns*, LIns*> NInsMap;
#if NJ_USES_IMMD_POOL
    typedef HashMap<uint64_t, uint64_t*> ImmDPoolMap;
#endif
#if NJ_USES_IMMF4_POOL
    #if defined(_WIN32) && !defined(NANOJIT_X64) //AVMSYSTEM_32BIT
        // The Win32 compiler for i386 provides only 8-byte alignment for the stack,
        // thus the required 16-byte alignment for float4_t cannot be guaranteed.
        // Unfortunately, HashMap members pass the key as a value parameter in several
        // places, causing compilation errors.  As a workaround, we encapsulate the
        // key value in a "proxy" class whose instances need not themselves be aligned,
        // but are coercible to properly-aligned copy of the underlying float4_t value.
        class float4_key {
        private:
            float _val[4];
        public:
            float4_key() {} // for alignof macro, don't call.
            float4_key(const float4_t& p) { _val[0] = f4_x(p); _val[1] = f4_y(p); _val[2] = f4_z(p); _val[3] = f4_w(p); }
            operator float4_t() const { float4_t f4value = { _val[0], _val[1], _val[2], _val[3] }; return f4value; }
            bool keyEquals(const float4_key& other) const {  return VMPI_memcmp(_val, other._val, sizeof(_val)) == 0; }
        };

        template <> struct DefaultKeysEqual<float4_key> {
            static bool keysEqual(const float4_key& x, const float4_key& y) {
                return x.keyEquals(y);
            }
        };

        typedef HashMap<float4_key, float4_t*> ImmF4PoolMap;
    #else
        // Other supported compilers provide for the necessary stack alignment.
        typedef HashMap<float4_t, float4_t*> ImmF4PoolMap;
    #endif
#endif //NJ_USES_IMMF4_POOL

#ifdef VMCFG_VTUNE
    class avmplus::CodegenLIR;
#endif

    class LabelState
    {
    public:
        RegAlloc regs;
        NIns *addr;
        LabelState(NIns *a, RegAlloc &r) : regs(r), addr(a)
        {}
    };

    class LabelStateMap
    {
        Allocator& alloc;
        HashMap<LIns*, LabelState*> labels;
    public:
        LabelStateMap(Allocator& alloc) : alloc(alloc), labels(alloc)
        {}

        void clear() { labels.clear(); }
        void add(LIns *label, NIns *addr, RegAlloc &regs);
        LabelState *get(LIns *);
    };

    /**
     * Some architectures (i386, X64) can emit two branches that need patching
     * in some situations. This is returned by asm_branch() implementations
     * with 0, 1 or 2 of these fields set to a non-NULL value. (If only 1 is set, 
     * it must be patch1, not patch2.)
     */
    struct Branches 
    {
        NIns* const branch1;
        NIns* const branch2;
        inline explicit Branches(NIns* b1 = NULL, NIns* b2 = NULL) 
            : branch1(b1)
            , branch2(b2)
        {
        }
    };

    /** map tracking the register allocation state at each bailout point
     *  (represented by SideExit*) in a trace fragment. */
    typedef HashMap<SideExit*, RegAlloc*> RegAllocMap;

    /**
     * Information about the activation record for the method is built up
     * as we generate machine code.  As part of the prologue, we issue
     * a stack adjustment instruction and then later patch the adjustment
     * value.  Temporary values can be placed into the AR as method calls
     * are issued.   Also LIR_allocp instructions will consume space.
     */
    class Assembler
    {
        friend class RegAlloc; // allow allocReg to evict instructions; allow init functions to access config settings
        friend class VerboseBlockReader;
            #ifdef NJ_VERBOSE
        public:
            // Buffer for holding text as we generate it in reverse order.
            StringList* _outputCache;

            // Outputs the format string and 'outlineEOL', and resets
            // 'outline' and 'outlineEOL'.
            void outputf(const char* format, ...);

        private:
            static int disassembler_printf(void* arg, const char* fmt, ...);
            static void* disassembler_event(void* cookie, const char* event, void* arg);

            // Log controller object.  Contains what-stuff-should-we-print
            // bits, and a sink function for debug printing.
            LogControl* _logc;

            // Buffer used in most of the output function.  It must big enough
            // to hold both the output line and the 'outlineEOL' buffer, which
            // is concatenated onto 'outline' just before it is printed.
            static char  outline[8192];
            // Buffer used to hold extra text to be printed at the end of some
            // lines.
            static char  outlineEOL[512];

            // Outputs 'outline' and 'outlineEOL', and resets them both.
            // Output goes to '_outputCache' if it's non-NULL, or is printed
            // directly via '_logc'.
            void output();

            void maybe_disassemble();
            void disassemble(void* from, void* to);

            // Sets 'outlineEOL'.
            void setOutputForEOL(const char* format, ...);

            void printRegState();
            void printActivationState();
            #endif // NJ_VERBOSE

        public:
            #ifdef VMCFG_VTUNE
            void* vtuneHandle;
            #endif

            Assembler(CodeAlloc& codeAlloc, Allocator& dataAlloc, Allocator& alloc,
                      LogControl* logc, const Config& config, MetaDataWriter* mdWriter = NULL);

            void        compile(Fragment *frag, Allocator& alloc, bool optimize
                                verbose_only(, LInsPrinter*));

            CodeList*   endAssembly(Fragment* frag);
            void        assemble(Fragment* frag, LirFilter* reader);
            void        beginAssembly(Fragment *frag);

            void        setNoiseGenerator(Noise* noise)  { _noise = noise; } // used for attack mitigation; setting to 0 disables all mitigations

            void        releaseRegisters();
            void        patch(GuardRecord *lr);
            void        patch(SideExit *exit);
            AssmError   error()               { return _err; }
            void        setError(AssmError e) { _err = e; }
            void        cleanupAfterError();
            void        clearNInsPtrs();
            void        reset();

            debug_only ( void       pageValidate(); )

            // support calling out from a fragment ; used to debug the jit
            debug_only( void        resourceConsistencyCheck(); )
            debug_only( void        registerConsistencyCheck(); )

            /**
             * Force the instruction to a stack slot, and mark it live.
             * Used for capturing stack slots to be recorded in metatdata
             * and referenced by the runtime.
             */
            int32_t    forceStackIndex(LIns* ins);

        private:
            void        gen(LirFilter* toCompile);
            NIns*       genPrologue();
            NIns*       genEpilogue();

            uint32_t    arReserve(LIns* ins);
            void        arFree(LIns* ins);
            void        arReset();

            void        evictAllActiveRegs() {
                // The evicted set will be be intersected with activeSet(),
                // so use an all-1s mask to avoid an extra load or call.
                evictSomeActiveRegs(~RegisterMask(0));
            }
            void        evictSomeActiveRegs(RegisterMask regs);
            void        evictScratchRegsExcept(RegisterMask ignore);
            void        intersectRegisterState(RegAlloc& saved);
            void        unionRegisterState(RegAlloc& saved);
            void        assignSaved(RegAlloc &saved, RegisterMask skip);

            Register    getBaseReg(LIns *ins, int &d, RegisterMask allow);
            void        getBaseReg2(RegisterMask allowValue, LIns* value, Register& rv,
                                    RegisterMask allowBase, LIns* base, Register& rb, int &d);
#if NJ_USES_IMMD_POOL
            const uint64_t* findImmDFromPool(uint64_t q);
#endif
#if NJ_USES_IMMF4_POOL
            const float4_t* findImmF4FromPool(const float4_t& q);
#endif

            int         findMemFor(LIns* ins);
            Register    findRegFor(LIns* ins, RegisterMask allow);
            void        findRegFor2(RegisterMask allowa, LIns* ia, Register &ra,
                                    RegisterMask allowb, LIns *ib, Register &rb);
            Register    findSpecificRegFor(LIns* ins, Register r);
            Register    findSpecificRegForUnallocated(LIns* ins, Register r);
            Register    deprecated_prepResultReg(LIns *ins, RegisterMask allow);
            Register    prepareResultReg(LIns *ins, RegisterMask allow);
            void        deprecated_freeRsrcOf(LIns *ins);
            void        freeResourcesOf(LIns *ins);
            void        evictIfActive(Register r);
            void        evict(LIns* vic);

            void        getBaseIndexScale(LIns* addp, LIns** base, LIns** index, int* scale);

            void        codeAlloc(NIns *&start, NIns *&end, NIns *&eip
                                  verbose_only(, size_t &nBytes)
                                  , size_t byteLimit=0);


            bool deprecated_isKnownReg(Register r) {
                return r != deprecated_UnknownReg;
            }

            Allocator&          alloc;              // for items with same lifetime as this Assembler
            CodeAlloc&          _codeAlloc;         // for code we generate
            Allocator&          _dataAlloc;         // for data used by generated code
            Fragment*           _thisfrag;
            RegAllocMap         _branchStateMap;
            NInsMap             _patches;
            LabelStateMap       _labels;
            Noise*              _noise;             // object to generate random noise used when hardening enabled.
        #if NJ_USES_IMMD_POOL
            ImmDPoolMap         _immDPool;
        #endif
        #if NJ_USES_IMMF4_POOL
            ImmF4PoolMap        _immF4Pool;
        #endif

            // We generate code into two places:  normal code chunks, and exit
            // code chunks (for exit stubs).  We use a hack to avoid having to
            // parameterize the code that does the generating -- we let that
            // code assume that it's always generating into a normal code
            // chunk (most of the time it is), and when we instead need to
            // generate into an exit code chunk, we set _inExit to true and
            // temporarily swap all the code/exit variables below (using
            // swapCodeChunks()).  Afterwards we swap them all back and set
            // _inExit to false again.
        public:
            CodeList*   codeList;               // finished blocks of code.
        private:
            bool        _inExit, vpad2[3];
            NIns        *codeStart, *codeEnd;   // current normal code chunk
            NIns        *exitStart, *exitEnd;   // current exit code chunk
            NIns*       _nIns;                  // current instruction in current normal code chunk
            NIns*       _nExitIns;              // current instruction in current exit code chunk
                                                // note: _nExitIns == NULL until the first side exit is seen.
        #ifdef NJ_VERBOSE
            NIns*       _nInsAfter;             // next instruction (ascending) in current normal/exit code chunk (for verbose output)
            size_t      codeBytes;              // bytes allocated in normal code chunks
            size_t      exitBytes;              // bytes allocated in exit code chunks
        #endif

            #define     SWAP(t, a, b)   do { t tmp = a; a = b; b = tmp; } while (0)
            void        swapCodeChunks();

            NIns*       _epilogue;
            AssmError   _err;           // 0 = means assemble() appears ok, otherwise it failed
        #if PEDANTIC
            NIns*       pedanticTop;
        #endif

            // Holds the current instruction during gen().
            LIns*       currIns;

            AR          _activation;
            RegAlloc    _allocator;

            MetaDataWriter* _mdWriter;

            verbose_only( void asm_inc_m32(uint32_t*); )
            void        asm_mmq(Register rd, int dd, Register rs, int ds);
            void        asm_jmp(LIns* ins, InsList& pending_lives);
            void        asm_jcc(LIns* ins, InsList& pending_lives);
            void        asm_jov(LIns* ins, InsList& pending_lives);
            void        asm_x(LIns* ins);
            void        asm_xcc(LIns* ins);
            NIns*       asm_exit(LIns* guard);
            NIns*       asm_leave_trace(LIns* guard);
            void        asm_store32(LOpcode op, LIns *val, int d, LIns *base);
            void        asm_store64(LOpcode op, LIns *val, int d, LIns *base);

            // WARNING: the implementation of asm_restore() should emit fast code
            // to rematerialize instructions where canRemat() returns true.
            // Otherwise, register allocation decisions will be suboptimal.
            void        asm_restore(LIns*, Register);

            bool        asm_maybe_spill(LIns* ins, bool pop);

#ifdef NANOJIT_IA32
    #define ASM_SPILL_RESTARGS  bool pop, int8_t nWords
#else
    #define ASM_SPILL_RESTARGS  int8_t nWords
#endif
            void        asm_spill(Register rr, int d, ASM_SPILL_RESTARGS);

            void        asm_ret(LIns* ins);
#ifdef NANOJIT_64BIT
            void        asm_immq(LIns* ins);
#endif
            void        asm_immd(LIns* ins);
            void        asm_condd(LIns* ins);
            void        asm_cond(LIns* ins);
            void        asm_arith(LIns* ins);
            void        asm_neg_not(LIns* ins);
            void        asm_load32(LIns* ins);
            void        asm_load64(LIns* ins);
            void        asm_cmov(LIns* ins);
            void        asm_param(LIns* ins);
            void        asm_immi(LIns* ins);
#if NJ_SOFTFLOAT_SUPPORTED
            void        asm_qlo(LIns* ins);
            void        asm_qhi(LIns* ins);
            void        asm_qjoin(LIns *ins);
#endif
            void        asm_neg_abs(LIns* ins); // fpu neg, abs
            void        asm_recip_sqrt(LIns* ins);  // fpu recip, rsqrt, sqrt
            void        asm_fop(LIns* ins);     // fpu add, sub, mul, div
            void        asm_i2d(LIns* ins);
            void        asm_ui2d(LIns* ins);
            void        asm_d2i(LIns* ins);
#ifdef NANOJIT_64BIT
            void        asm_q2i(LIns* ins);
            void        asm_q2d(LIns* ins);
            void        asm_ui2uq(LIns *ins);
            void        asm_dasq(LIns *ins);
            void        asm_qasd(LIns *ins);
#endif
            void        asm_mmi(Register rd, int dd, Register rs, int ds);
            void        asm_store128(LOpcode op, LIns *val, int d, LIns *base);
            void        asm_load128(LIns* ins);
            void        asm_immf(LIns* ins);
            void        asm_immf4(LIns* ins);
            void        asm_condf4(LIns* ins);
            void        asm_i2f(LIns* ins);
            void        asm_ui2f(LIns* ins);
            void        asm_f2i(LIns* ins);
            void        asm_d2f(LIns* ins);
            void        asm_f2d(LIns* ins);
            void        asm_f2f4(LIns* ins);
            void        asm_ffff2f4(LIns* ins);
            void        asm_f4comp(LIns* ins);

            void        asm_nongp_copy(Register r, Register s);
            void        asm_call(LIns*);
            Register    asm_binop_rhs_reg(LIns* ins);
            Branches    asm_branch(bool branchOnFalse, LIns* cond, NIns* targ);
            NIns*       asm_branch_ov(LOpcode op, NIns* targ);
            void        asm_jtbl(NIns** table, Register indexreg);
            void        asm_insert_random_nop();
            void        asm_label();
            void        assignSavedRegs();
            void        reserveSavedRegs();
            void        assignParamRegs();
            void        handleLoopCarriedExprs(InsList& pending_lives, RegisterMask reserved);

            // platform specific implementation (see NativeXXX.cpp file)
            void        nBeginAssembly();
            void        nPatchBranch(NIns* branch, NIns* location);
            void        nFragExit(LIns* guard);

            // platform specific methods
        public:
            DECLARE_PLATFORM_ASSEMBLER()

        private:
            const Config& _config;
    };

    inline int32_t arDisp(LIns* ins)
    {
        // even on 64bit cpu's, we allocate stack area in 4byte chunks
        return -4 * int32_t(ins->getArIndex());
    }
    // XXX: deprecated, use arDisp() instead.  See bug 538924.
    inline int32_t deprecated_disp(LIns* ins)
    {
        // even on 64bit cpu's, we allocate stack area in 4byte chunks
        return -4 * int32_t(ins->deprecated_getArIndex());
    }

    /** Abstract class for writing metadata for deoptimization, debugging, etc. */
    class MetaDataWriter {
    public:
        // Report inital address, i.e., the last code byte.
        virtual void beginAssembly(Assembler* assm, uint8_t* address) = 0;

        // Report assembly of LIR_safe, providing the current code address.
        virtual void safepointStart(Assembler* assm, void* payload, uint8_t* address) = 0;

        // Report assembly of LIR_endsafe, providing the current code address.
        virtual void safepointEnd(Assembler* assm, void* payload, uint8_t* address) = 0;

        // Report a non-sequential change in the code address due to chaining
        // to a new code chunk.
        virtual void setNativePc(uint8_t* address) = 0;

        // Report final address, i.e., the first code byte.
        virtual void endAssembly(Assembler* assm, uint8_t* address) = 0;

        // Abandon metadata if assembly has failed.
        virtual void abandon() = 0;

        // We do not expect to invoke the destructor polymorphically,
        // but some compilers complain if a class with virtual methods
        // does not have a virtual destructor.
        virtual ~MetaDataWriter() {}
    };

}
#endif // __nanojit_Assembler__
