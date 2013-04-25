/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_LirHelper__
#define __avmplus_LirHelper__

using namespace MMgc;
#include "../nanojit/nanojit.h"

namespace halfmoon {
    class JitManager;
}

namespace avmplus
{
    using namespace nanojit;
    using halfmoon::JitManager;

    /**
     * LogControl adapter to print output to AvmCore.console.
     */
    class AvmLogControl : public LogControl
    {
    public:
        virtual ~AvmLogControl() {}
#ifdef NJ_VERBOSE
        void printf( const char* format, ... ) PRINTF_CHECK(2,3);

        AvmCore* core; // access console via core dynamically since core may modify it.
#endif
    };

    /**
     * Logger used to eat output.
     */
    class SinkLogControl : public AvmLogControl
    {
    public:
        SinkLogControl() { lcbits = 0; }
        virtual ~SinkLogControl() {}
#ifdef NJ_VERBOSE
        void printf( const char* /*format*/, ... ) PRINTF_CHECK(2,3) {}  // no-op
#endif
    };

    class BindingCache; // forward.

    /**
     * CodeMgr manages memory for compiled code, including the code itself
     * (in a nanojit::CodeAlloc), and any data with code lifetime
     * (in a nanojit::Allocator), such as debuging info, or inline caches.
     */
    class CodeMgr {
    public:
        CodeAlloc   codeAlloc;  // allocator for code memory
        AvmLogControl  log;        // controller for verbose output
        Allocator   allocator;  // data with same lifetime of this CodeMgr
        BindingCache* bindingCaches;    // head of linked list of all BindingCaches allocated by this codeMgr
                                        // (only for flushing... lifetime is still managed by codeAlloc)
        CodeMgr(nanojit::Config* conf);
        void flushBindingCaches();      // invalidate all binding caches for this codemgr... needed when AbcEnv is unloaded

        // DEOPT & PROFILER todo: provide some way to free code memory
        JitManager *jit_mgr;
    };

    // AccSet conventions
    // nanojit currently supports very coarse grained access-set tags
    // for loads, stores, and calls.  See the comments in LIR.h for a
    // detailed description.  Here we define a set of constants to use that
    // are more fine grained, and then map them to AccessSets that nanojit supports.
    //
    // If you aren't sure what to use, use ACCSET_LOAD_ANY or ACCSET_STORE_ANY.
    // Warning: if you annotate a load or store with something other
    // than ACC_*_ANY, and get it wrong, you will introduce subtle and
    // hard to find bugs: "if you lie to the compiler, it will get its revenge"
    //
    // we use ACCSET_OTHER for a catchall that does not overlap with any other
    // predefined alias set.  Future work should subdivide this set where
    // the improvements outweigh the cost of additional alias sets.

    const AccSet ACCSET_VARS  = (1 << 0);    // values of local variables
    const AccSet ACCSET_TAGS  = (1 << 1);    // BuiltinTraits tags for local variables
    const AccSet ACCSET_OTHER = (1 << 2);
    const uint8_t TR_NUM_USED_ACCS = 3;      // number of access regions used by Tamarin

    /** helper code to make LIR generation nice and tidy */
    class LirHelper {
    protected:
        LirHelper(PoolObject*);
        ~LirHelper();
        void cleanup();
        
        typedef enum {
            F4_X=0,
            F4_Y=1,
            F4_Z=2,
            F4_W=3
        } eFloat4Component;
        
    protected:
        LIns* downcast_obj(LIns* atom, LIns* env, Traits* t); // atom -> typed scriptobj
        static BuiltinType bt(Traits *t);
        LIns* nativeToAtom(LIns* value, Traits* valType);
        LIns* atomToNative(BuiltinType, LIns* i);
        LIns* eqi0(LIns* i);             // eq(i, imm(0))
        LIns* eqp0(LIns* ptr);          // peq(ptr, immq(0))
        LIns* qlo(LIns* q);             // LIR_dlo2i(q)
        LIns* i2p(LIns* i);             // 32bit: nop, 64bit: l2q(i)
        LIns* ui2p(LIns* u);             // 32bit: nop, 64bit: ul2uq(i)
        LIns* p2i(LIns* ptr);           // 32bit: nop, 64bit: q2l(ptr)
        LIns* p2dIns(LIns* v);
        LIns* InsConst(int32_t c);
        LIns* InsConstPtr(const void *p);
        LIns* InsConstAtom(Atom c);
        LIns* InsConstDbl(double d);
        LIns* callIns(const CallInfo *, uint32_t argc, ...);
        LIns* vcallIns(const CallInfo *, uint32_t argc, va_list args);
        LIns* eqp(LIns* a, Atom b);
        LIns* eqp(LIns* a, LIns* b);
        LIns* ltup(LIns* a, LIns* b);
        LIns* choose(LIns* c, Atom t, LIns* f);
        LIns* addp(LIns* a, Atom imm);
        LIns* addi(LIns* a, int32_t imm);
        LIns* andp(LIns* a, Atom mask);
        LIns* orp(LIns* a, Atom mask);
        LIns* ori(LIns* a, int32_t mask);
        LIns* xorp(LIns* a, Atom mask);
        LIns* subp(LIns* a, int32_t offset);
        LIns* retp(LIns* a);
        LIns* label();
        LIns* jlti(LIns* a, int32_t b);
        LIns* jgti(LIns* a, int32_t b);
        LIns* jnei(LIns* a, int32_t b);
        LIns* lea(int32_t d, LIns *base);
        LIns* sti(LIns* val, LIns* p, int32_t d, AccSet);
        LIns* stp(LIns* val, LIns* p, int32_t d, AccSet);
        LIns* std(LIns* val, LIns* p, int32_t d, AccSet);
        LIns* stForTraits(Traits *t, LIns* val, LIns* p, int32_t d, AccSet);
        LIns* ldi(LIns* p, int32_t d, AccSet);
        LIns* ldd(LIns* p, int32_t d, AccSet);
        LIns* ldp(LIns* p, int32_t d, AccSet);
        LIns* ldForTraits(Traits *t, LIns* p, int32_t d, AccSet);
        LIns* livep(LIns*);
        LIns* param(int n, const char *name);
        LIns* lshi(LIns* a, int32_t b);
        LIns* rshi(LIns* a, int32_t b);
        LIns* lshp(LIns* a, int32_t b);
        LIns* rshp(LIns* a, int32_t b);
        LIns* rshup(LIns* a, int32_t b);
        LIns* binaryIns(LOpcode op, LIns *a, LIns *b);
#ifdef VMCFG_FLOAT
        LIns* InsConstFlt(float f);
        LIns* ldf(LIns* p, int32_t d, AccSet);
        LIns* ldf4(LIns* p,int32_t d, AccSet);
        LIns* stf(LIns* val, LIns* p, int32_t d, AccSet);
        LIns* stf4(LIns* val,LIns* p, int32_t d, AccSet);
        LIns* i2fIns(LIns* v);
        LIns* d2fIns(LIns* v);
        LIns* f2dIns(LIns* v);
        LIns* ui2fIns(LIns* v);
        LIns* f4tofIns(LIns* v, eFloat4Component comp);        
#endif        
        void  liveAlloc(LIns* expr);        // extend lifetime of LIR_allocp, otherwise no-op
        void  emitStart(Allocator&, LirBuffer*, LirWriter*&);

        /**
         * Compute number of bytes needed for the unboxed representation
         * of this argument value when passed on the stack.
         */
        static int32_t argSize(MethodSignaturep, int32_t i);

    protected: // data
        LirWriter *lirout;
        Fragment *frag;
        PoolObject* pool;
        AvmCore *core;
        LIns *coreAddr;
        Allocator* alloc1;    // allocator used in first pass, while writing LIR
        Allocator* lir_alloc; // allocator with LIR buffer lifetime
        JITObserver* jit_observer;
        bool const use_cmov;
        debug_only(ValidateWriter* validate1;)
        debug_only(ValidateWriter* validate2;)

#ifdef VMCFG_FLOAT
#include "LirHelper-Emitters.h"
#endif
    };

    /**
     * Returns true for functions that never return.
     */
    bool neverReturns(const CallInfo* call);


    /**
     * Initialize the code manager the first time we jit any method for this
     * PoolObject.
     */
    CodeMgr* initCodeMgr(PoolObject *pool);

    // Forward declarations for CallInfos of shared helper functions.
    extern const CallInfo ci_argcError;
    extern const CallInfo ci_boolean;
    extern const CallInfo ci_doubleToAtom;
#ifdef VMCFG_FLOAT
    extern const CallInfo ci_floatToAtom;
    extern const CallInfo ci_float4ToAtom;
    extern const CallInfo ci_singlePrecisionFloat;
    extern const CallInfo ci_float4;
    extern const CallInfo ci_fcalli;
    extern const CallInfo ci_fcallimt;
    extern const CallInfo ci_vfcalli;
    extern const CallInfo ci_vfcallimt;
#endif
    extern const CallInfo ci_intToAtom;
    extern const CallInfo ci_uintToAtom;
    extern const CallInfo ci_number;
    extern const CallInfo ci_number_d;
    extern const CallInfo ci_integer;
    extern const CallInfo ci_integer_i;
    extern const CallInfo ci_integer_u;
    extern const CallInfo ci_throwAtom;
    extern const CallInfo ci_toUInt32;
    extern const CallInfo ci_npe;
    extern const CallInfo ci_upe;
    extern const CallInfo ci_mop_rangeCheckFailed;
#ifdef VMCFG_TELEMETRY_SAMPLER
    extern const CallInfo ci_takeSample;
#endif
    extern const CallInfo ci_handleInterruptMethodEnv;
    extern const CallInfo ci_coerce;
    extern const CallInfo ci_coerce_s;
    extern const CallInfo ci_coerceobj_atom;
#ifdef VMCFG_FLOAT
    extern const CallInfo ci_mod;
    extern const CallInfo ci_op_modulo;
    extern const CallInfo ci_op_add;
    extern const CallInfo ci_op_add_nofloat;
    extern const CallInfo ci_op_subtract;
    extern const CallInfo ci_op_multiply;
    extern const CallInfo ci_op_divide;
    extern const CallInfo ci_op_negate;
#endif

#ifdef VMCFG_SSE2
    extern const CallInfo ci_doubleToAtom_sse2;
#endif

    extern const CallInfo ci_acalli;
    extern const CallInfo ci_icalli;
    extern const CallInfo ci_dcalli;
    extern const CallInfo ci_acallimt;
    extern const CallInfo ci_icallimt;
    extern const CallInfo ci_dcallimt;
    extern const CallInfo ci_compare;
    extern const CallInfo ci_createRestHelper;
    extern const CallInfo ci_mod;
    extern const CallInfo ci_string;
    extern const CallInfo ci_atomWriteBarrier;
    extern const CallInfo ci_privateWriteBarrierRC;
    extern const CallInfo ci_createInstanceProc;
    extern const CallInfo ci_doubleToInt32;
    extern const CallInfo ci_handleStackOverflowMethodEnv;
    extern const CallInfo ci_get_cache_handler;
    extern const CallInfo ci_set_cache_handler;
    extern const CallInfo ci_call_cache_handler;
    extern const CallInfo ci_op_add_a_aa;
    extern const CallInfo ci_hasnextproto;

    const ArgType ARGTYPE_U = ARGTYPE_UI; // uint32_t
    const ArgType ARGTYPE_A = ARGTYPE_P;  // Atom
}

#include "LirHelper-inlines.h"

#ifdef DEBUGGER
#define DEBUGGER_ONLY(...) __VA_ARGS__
#else
#define DEBUGGER_ONLY(...)
#endif

#ifdef VMCFG_SSE2
#define SSE2_ONLY(...) __VA_ARGS__
#else
#define SSE2_ONLY(...)
#endif

/** Wrapper macro to generate the name of a CallInfo structure. */
#define FUNCTIONID(n) &ci_##n

#define SIG0(r)\
    nanojit::CallInfo::typeSig0(ARGTYPE_##r)
#define SIG1(r,a1)\
    nanojit::CallInfo::typeSig1(ARGTYPE_##r, ARGTYPE_##a1)
#define SIG2(r,a1,a2)\
    nanojit::CallInfo::typeSig2(ARGTYPE_##r, ARGTYPE_##a1, ARGTYPE_##a2)
#define SIG3(r,a1,a2,a3)\
    nanojit::CallInfo::typeSig3(ARGTYPE_##r, ARGTYPE_##a1, ARGTYPE_##a2, ARGTYPE_##a3)
#define SIG4(r,a1,a2,a3,a4)\
    nanojit::CallInfo::typeSig4(ARGTYPE_##r, ARGTYPE_##a1, ARGTYPE_##a2, ARGTYPE_##a3,\
                                ARGTYPE_##a4)
#define SIG5(r,a1,a2,a3,a4,a5)\
    nanojit::CallInfo::typeSig5(ARGTYPE_##r, ARGTYPE_##a1, ARGTYPE_##a2, ARGTYPE_##a3,\
                                ARGTYPE_##a4, ARGTYPE_##a5)
#define SIG6(r,a1,a2,a3,a4,a5,a6)\
    nanojit::CallInfo::typeSig6(ARGTYPE_##r, ARGTYPE_##a1, ARGTYPE_##a2, ARGTYPE_##a3,\
                                ARGTYPE_##a4, ARGTYPE_##a5, ARGTYPE_##a6)
#define SIG7(r,a1,a2,a3,a4,a5,a6,a7)\
    nanojit::CallInfo::typeSig7(ARGTYPE_##r, ARGTYPE_##a1, ARGTYPE_##a2, ARGTYPE_##a3,\
                                ARGTYPE_##a4, ARGTYPE_##a5, ARGTYPE_##a6, ARGTYPE_##a7)
#define SIG8(r,a1,a2,a3,a4,a5,a6,a7,a8)\
    nanojit::CallInfo::typeSig8(ARGTYPE_##r, ARGTYPE_##a1, ARGTYPE_##a2, ARGTYPE_##a3,\
                        ARGTYPE_##a4, ARGTYPE_##a5, ARGTYPE_##a6, ARGTYPE_##a7, ARGTYPE_##a8)

#if _MSC_VER
    #define ABI_FUNCTION ABI_CDECL
    #define ABI_FAST     ABI_FASTCALL
    #define ABI_METHOD   ABI_THISCALL
#elif defined(__SUNPRO_CC)
    #define ABI_FUNCTION ABI_CDECL
    #define ABI_FAST     ABI_CDECL
    #define ABI_METHOD   ABI_CDECL
#else
    // gcc, probably
    #define ABI_FUNCTION ABI_CDECL
    #define ABI_FAST     ABI_FASTCALL
    #define ABI_METHOD   ABI_CDECL
#endif

#ifdef NJ_VERBOSE
    #define DEFINE_CALLINFO(f, sig, abi, isPure, storeAccSet, name) \
        const CallInfo ci_##name = { f, sig, abi, isPure, storeAccSet, #name };
#else
    #define DEFINE_CALLINFO(f, sig, abi, isPure, storeAccSet, name) \
        const CallInfo ci_##name = { f, sig, abi, isPure, storeAccSet };
#endif

#define FUNCTION(f, sig, name) \
    DEFINE_CALLINFO(f, sig, ABI_FUNCTION, 0, ACCSET_STORE_ANY, name)
#define FASTFUNCTION(f, sig, name) \
    DEFINE_CALLINFO(f, sig, ABI_FAST, 0, ACCSET_STORE_ANY, name)
#define PUREFUNCTION(f, sig, name) \
    DEFINE_CALLINFO(f, sig, ABI_FUNCTION, 1, ACCSET_NONE, name)
#define PUREFASTFUNCTION(f, sig, name) \
    DEFINE_CALLINFO(f, sig, ABI_FAST, 1, ACCSET_NONE, name)

#define METHOD(f, sig, name) \
    DEFINE_CALLINFO(f, sig, ABI_METHOD, 0, ACCSET_STORE_ANY, name)
#define PUREMETHOD(f, sig, name) \
    DEFINE_CALLINFO(f, sig, ABI_METHOD, 1, ACCSET_NONE, name)

#endif /* __avmplus_LirHelper__ */
