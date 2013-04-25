/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

#ifdef VMCFG_NANOJIT

#include "CodegenLIR.h"
#include "exec-osr.h"

#if defined(WIN32) && defined(AVMPLUS_ARM)
#include <intrin.h>
#endif

// Workaround GCC 4.6 offsetof bug
#if (__GNUC__ == 4) && (__GNUC_MINOR__ == 6)
#undef offsetof
#define offsetof(a,b) ((int)(&(((a*)(0))->b)))
#endif

#ifdef VMCFG_VTUNE
namespace vtune {
    using namespace avmplus;
    void* vtuneInit(String*);
    void vtuneCleanup(void*);
}
using namespace vtune;
#endif // VMCFG_VTUNE

// Sparc, ARM and SH4 have buggy LIR_d2i support (bug 613189)
#if (defined(NANOJIT_SPARC) || defined(NANOJIT_SH4) || defined(NANOJIT_ARM))
# undef NJ_F2I_SUPPORTED
# define NJ_F2I_SUPPORTED 0
#endif

#ifdef _MSC_VER
    #if !defined (AVMPLUS_ARM) || defined(UNDER_RT)
    extern "C"
    {
        int __cdecl _setjmp3(jmp_buf jmpbuf, int arg);
    }
    #else
    #include <setjmp.h>
    #undef setjmp
    extern "C"
    {
        int __cdecl setjmp(jmp_buf jmpbuf);
    }
    #endif // AVMPLUS_ARM
#endif // _MSC_VER

#ifdef AVMPLUS_ARM
#ifdef _MSC_VER
#define RETURN_METHOD_PTR(_class, _method) \
return *((int*)&_method);
#else
#define RETURN_METHOD_PTR(_class, _method) \
union { \
    int (_class::*bar)(); \
    int foo[2]; \
}; \
bar = _method; \
return foo[0];
#endif

#elif defined __GNUC__
#define RETURN_METHOD_PTR(_class, _method) \
union { \
    int (_class::*bar)(); \
    intptr_t foo; \
}; \
bar = _method; \
return foo;
#else
#define RETURN_METHOD_PTR(_class, _method) \
return *((intptr_t*)&_method);
#endif

#ifdef PERFM
#define DOPROF
#endif /* PERFM */

//#define DOPROF
#include "../vprof/vprof.h"

// Profiling generated code.

#ifdef DOPROF
#define JIT_EVENT(id) \
    do { \
        static void* id; \
        _jnvprof_init(&id, #id, NULL); \
        callIns(FUNCTIONID(jitProfileEvent), 1, InsConstPtr(id)); \
    } while (0)
#define JIT_VALUE(id, val) \
    do { \
        static void* id; \
        _jnvprof_init(&id, #id, NULL); \
        callIns(FUNCTIONID(jitProfileValue32), 2, InsConstPtr(id), val); \
    } while (0)
#define JIT_TAGVAL(id, val) \
    do { \
        static void* id; \
        _jnhprof_init(&id, #id, 8, 1, 2, 3, 4, 5, 6, 7, 8); \
        LIns* jit_tagval_tag = p2i(andp((val), 7)); \
        callIns(FUNCTIONID(jitProfileHist32), 2, InsConstPtr(id), jit_tagval_tag); \
    } while (0)
#else
#define JIT_EVENT(id)       do { } while (0)
#define JIT_VALUE(id, val)  do { } while (0)
#define JIT_TAGVAL(id, val) do { } while (0)
#endif

#ifdef AVMPLUS_64BIT
#define PTR_SCALE 3
#else
#define PTR_SCALE 2
#endif

#define IS_ALIGNED(x, size) ((uintptr_t(x) & ((size)-1)) == 0)

namespace avmplus
{
        #define COREADDR(f) coreAddr((int (AvmCore::*)())(&f))
        #define GCADDR(f) gcAddr((int (MMgc::GC::*)())(&f))
        #define ENVADDR(f) envAddr((int (MethodEnv::*)())(&f))
        #define ARRAYADDR(f) arrayAddr((int (ArrayObject::*)())(&f))
        #define STRINGADDR(f) stringAddr((int (String::*)())(&f))
        #define VECTORINTADDR(f) vectorIntAddr((int (IntVectorObject::*)())(&f))
        #define VECTORUINTADDR(f) vectorUIntAddr((int (UIntVectorObject::*)())(&f))
        #define VECTORDOUBLEADDR(f) vectorDoubleAddr((int (DoubleVectorObject::*)())(&f))
        #define VECTORFLOATADDR(f) vectorFloatAddr((int (FloatVectorObject::*)())(&f))
        #define VECTORFLOAT4ADDR(f) vectorFloat4Addr((int (Float4VectorObject::*)())(&f))
        #define VECTOROBJADDR(f) vectorObjAddr((int (ObjectVectorObject::*)())(&f))
        #define EFADDR(f)   efAddr((int (ExceptionFrame::*)())(&f))
        #define DEBUGGERADDR(f)   debuggerAddr((int (Debugger::*)())(&f))
        #define FUNCADDR(addr) (uintptr_t)addr

        intptr_t coreAddr( int (AvmCore::*f)() )
        {
            RETURN_METHOD_PTR(AvmCore, f);
        }

        intptr_t  gcAddr( int (MMgc::GC::*f)() )
        {
            RETURN_METHOD_PTR(MMgc::GC, f);
        }

        intptr_t  envAddr( int (MethodEnv::*f)() )
        {
            RETURN_METHOD_PTR(MethodEnv, f);
        }

    #ifdef DEBUGGER
        intptr_t  debuggerAddr( int (Debugger::*f)() )
        {
            RETURN_METHOD_PTR(Debugger, f);
        }
    #endif /* DEBUGGER */

        intptr_t  arrayAddr(int (ArrayObject::*f)())
        {
            RETURN_METHOD_PTR(ArrayObject, f);
        }

        intptr_t  stringAddr(int (String::*f)())
        {
            RETURN_METHOD_PTR(String, f);
        }

        intptr_t vectorIntAddr(int (IntVectorObject::*f)())
        {
            RETURN_METHOD_PTR(IntVectorObject, f);
        }

        intptr_t vectorUIntAddr(int (UIntVectorObject::*f)())
        {
            RETURN_METHOD_PTR(UIntVectorObject, f);
        }

        intptr_t vectorDoubleAddr(int (DoubleVectorObject::*f)())
        {
            RETURN_METHOD_PTR(DoubleVectorObject, f);
        }

#ifdef VMCFG_FLOAT
        intptr_t vectorFloatAddr(int (FloatVectorObject::*f)())
        {
            RETURN_METHOD_PTR(FloatVectorObject, f);
        }

        intptr_t vectorFloat4Addr(int (Float4VectorObject::*f)())
        {
            RETURN_METHOD_PTR(Float4VectorObject, f);
        }
    #endif // VMCFG_FLOAT
        intptr_t vectorObjAddr(int (ObjectVectorObject::*f)())
        {
            RETURN_METHOD_PTR(ObjectVectorObject, f);
        }
        intptr_t efAddr( int (ExceptionFrame::*f)() )
        {
            RETURN_METHOD_PTR(ExceptionFrame, f);
        }

    using namespace MMgc;
    using namespace nanojit;

    #if defined _MSC_VER && !defined AVMPLUS_ARM
    #  define SETJMP ((uintptr_t)_setjmp3)
    #elif defined AVMPLUS_MAC_CARBON
    #  define SETJMP setjmpAddress
    #else
    #  define SETJMP ((uintptr_t)VMPI_setjmpNoUnwind)
    #endif // _MSC_VER
} // end namespace avmplus

// include this outside any namespace to appease Eclipse CDT indigo C++ indexer.
#include "../core/jit-calls.h"

namespace avmplus
{

#if NJ_EXPANDED_LOADSTORE_SUPPORTED && defined(VMCFG_UNALIGNED_INT_ACCESS) && defined(VMCFG_LITTLE_ENDIAN)
    #define VMCFG_MOPS_USE_EXPANDED_LOADSTORE_INT
#endif

#if NJ_EXPANDED_LOADSTORE_SUPPORTED && defined(VMCFG_UNALIGNED_FP_ACCESS) && defined(VMCFG_LITTLE_ENDIAN)
    #define VMCFG_MOPS_USE_EXPANDED_LOADSTORE_FP
#endif

    // source of entropy for Assembler
    JITNoise::JITNoise()
    {
        MathUtils::initRandom(&randomSeed);
    }

    // produce a random number from 0-maxValue for the JIT to use in attack mitigation
    uint32_t JITNoise::getValue(uint32_t maxValue)
    {
        int32_t v = MathUtils::Random(maxValue, &randomSeed);
        AvmAssert(v>=0);
        return (uint32_t)v;
    }

    struct MopsInfo
    {
        uint32_t size;
        LOpcode op;
        const CallInfo* call;
    };

    static const MopsInfo kMopsLoadInfo[7] = {
        { 1, LIR_ldc2i,   FUNCTIONID(mop_lix8) },
        { 2, LIR_lds2i,   FUNCTIONID(mop_lix16) },
        { 1, LIR_lduc2ui, FUNCTIONID(mop_liz8) },
        { 2, LIR_ldus2ui, FUNCTIONID(mop_liz16) },
        { 4, LIR_ldi,     FUNCTIONID(mop_li32) },
        { 4, LIR_ldf2d,   FUNCTIONID(mop_lf32) },
        { 8, LIR_ldd,     FUNCTIONID(mop_lf64) }
    };

    static const MopsInfo kMopsStoreInfo[5] = {
        { 1, LIR_sti2c, FUNCTIONID(mop_si8) },
        { 2, LIR_sti2s, FUNCTIONID(mop_si16) },
        { 4, LIR_sti,   FUNCTIONID(mop_si32) },
        { 4, LIR_std2f, FUNCTIONID(mop_sf32) },
        { 8, LIR_std,   FUNCTIONID(mop_sf64) }
     };

    class MopsRangeCheckFilter: public LirWriter
    {
    private:
        LirWriter* const prolog_out;
        LIns* const env_domainenv;
        LIns* curMemBase;
        LIns* curMemSize;
        LIns* curMopAddr;
        LIns* curRangeCheckLHS;
        LIns* curRangeCheckRHS;
        int32_t curRangeCheckMinValue;
        int32_t curRangeCheckMaxValue;

    private:
        void clearMemBaseAndSize();

        static void extractConstantDisp(LIns*& mopAddr, int32_t& curDisp);
        LIns* safeIns2(LOpcode op, LIns*, int32_t);
        void safeRewrite(LIns* ins, int32_t);

    public:
        MopsRangeCheckFilter(LirWriter* out, LirWriter* prolog_out, LIns* env_domainenv);

        LIns* emitRangeCheck(LIns*& mopAddr, int32_t const size, int32_t* disp, LIns*& br);
        void flushRangeChecks();

        // overrides from LirWriter
        LIns* ins0(LOpcode v);
        LIns* insCall(const CallInfo* call, LIns* args[]);
    };

    inline MopsRangeCheckFilter::MopsRangeCheckFilter(LirWriter* out, LirWriter* prolog_out, LIns* env_domainenv) :
        LirWriter(out),
        prolog_out(prolog_out),
        env_domainenv(env_domainenv),
        curMemBase(NULL),
        curMemSize(NULL),
        curMopAddr(NULL),
        curRangeCheckLHS(NULL),
        curRangeCheckRHS(NULL),
        curRangeCheckMinValue(int32_t(0x7fffffff)),
        curRangeCheckMaxValue(int32_t(0x80000000))
    {
        clearMemBaseAndSize();
    }

    void MopsRangeCheckFilter::clearMemBaseAndSize()
    {
        curMemBase = curMemSize = NULL;
    }

    void MopsRangeCheckFilter::flushRangeChecks()
    {
        AvmAssert((curRangeCheckLHS != NULL) == (curRangeCheckRHS != NULL));
        if (curRangeCheckLHS)
        {
            curRangeCheckLHS = curRangeCheckRHS = curMopAddr = NULL;
            curRangeCheckMinValue = int32_t(0x7fffffff);
            curRangeCheckMaxValue = int32_t(0x80000000);
            // but don't clearMemBaseAndSize()!
        }
        else
        {
            AvmAssert(curMopAddr == NULL);
            AvmAssert(curRangeCheckMinValue == int32_t(0x7fffffff));
            AvmAssert(curRangeCheckMaxValue == int32_t(0x80000000));
        }
    }

    static bool sumFitsInInt32(int32_t a, int32_t b)
    {
        return int64_t(a) + int64_t(b) == int64_t(a + b);
    }

    /*static*/ void MopsRangeCheckFilter::extractConstantDisp(LIns*& mopAddr, int32_t& curDisp)
    {
        // mopAddr is an int (an offset from globalMemoryBase) on all archs.
        // if mopAddr is an expression of the form
        //      expr+const
        //      const+expr
        //      expr-const
        //      (but not const-expr)
        // then try to pull the constant out and return it as a displacement to
        // be used in the instruction as an addressing-mode offset.
        // (but only if caller requests it that way.)
        for (;;)
        {
            LOpcode const op = mopAddr->opcode();
            if (op != LIR_addi && op != LIR_subi)
                break;

            int32_t imm;
            LIns* nonImm;
            if (mopAddr->oprnd2()->isImmI())
            {
                imm = mopAddr->oprnd2()->immI();
                nonImm = mopAddr->oprnd1();

                if (op == LIR_subi)
                    imm = -imm;
            }
            else if (mopAddr->oprnd1()->isImmI())
            {
                // don't try to optimize const-expr
                if (op == LIR_subi)
                    break;

                imm = mopAddr->oprnd1()->immI();
                nonImm = mopAddr->oprnd2();
            }
            else
            {
                break;
            }

            if (!sumFitsInInt32(curDisp, imm))
                break;

            curDisp += imm;
            mopAddr = nonImm;
        }
    }

    LIns* MopsRangeCheckFilter::emitRangeCheck(LIns*& mopAddr, int32_t const size, int32_t* disp, LIns*& br)
    {
        int32_t offsetMin = 0;
        if (disp != NULL)
        {
            *disp = 0;
            extractConstantDisp(mopAddr, *disp);
            offsetMin = *disp;
        }

        int32_t offsetMax = offsetMin + size;

        AvmAssert((curRangeCheckLHS != NULL) == (curRangeCheckRHS != NULL));

        AvmAssert(mopAddr != NULL);
        if (curRangeCheckLHS != NULL && curMopAddr == mopAddr)
        {
            int32_t n_curRangeCheckMin = curRangeCheckMinValue;
            if (n_curRangeCheckMin > offsetMin)
                n_curRangeCheckMin = offsetMin;
            int32_t n_curRangeCheckMax = curRangeCheckMaxValue;
            if (n_curRangeCheckMax < offsetMax)
                n_curRangeCheckMax = offsetMax;

            if ((n_curRangeCheckMax - n_curRangeCheckMin) <= DomainEnv::GLOBAL_MEMORY_MIN_SIZE)
            {
                if (curRangeCheckMinValue != n_curRangeCheckMin)
                    safeRewrite(curRangeCheckLHS, curRangeCheckMinValue);

                if ((n_curRangeCheckMax - n_curRangeCheckMin) != (curRangeCheckMaxValue - curRangeCheckMinValue))
                    safeRewrite(curRangeCheckRHS, curRangeCheckMaxValue - curRangeCheckMinValue);

                curRangeCheckMinValue = n_curRangeCheckMin;
                curRangeCheckMaxValue = n_curRangeCheckMax;
            }
            else
            {
                // if collapsed ranges get too large, pre-emptively flush, so that the
                // range-checking code can always assume the range is within minsize
                flushRangeChecks();
            }
        }
        else
        {
            flushRangeChecks();
        }

        if (!curMemBase)
        {
            //AvmAssert(curMemSize == NULL);
            curMemBase = out->insLoad(LIR_ldp, env_domainenv, offsetof(DomainEnv,m_globalMemoryBase), ACCSET_OTHER, LOAD_VOLATILE);
            curMemSize = out->insLoad(LIR_ldi, env_domainenv, offsetof(DomainEnv,m_globalMemorySize), ACCSET_OTHER, LOAD_VOLATILE);
        }

        AvmAssert((curRangeCheckLHS != NULL) == (curRangeCheckRHS != NULL));

        if (!curRangeCheckLHS)
        {
            AvmAssert(!curMopAddr);
            curMopAddr = mopAddr;
            curRangeCheckMinValue = offsetMin;
            curRangeCheckMaxValue = offsetMax;

            AvmAssert(env_domainenv != NULL);

            // we want to pass range-check if
            //
            //      (curMopAddr+curRangeCheckMin >= 0 && curMopAddr+curRangeCheckMax <= mopsMemorySize)
            //
            // which is the same as
            //
            //      (curMopAddr >= -curRangeCheckMin && curMopAddr <= mopsMemorySize - curRangeCheckMax)
            //
            // which is the same as
            //
            //      (curMopAddr >= -curRangeCheckMin && curMopAddr < mopsMemorySize - curRangeCheckMax + 1)
            //
            // and since (x >= min && x < max) is equivalent to (unsigned)(x-min) < (unsigned)(max-min)
            //
            //      (unsigned(curMopAddr + curRangeCheckMin) < unsigned(mopsMemorySize - curRangeCheckMax + 1 + curRangeCheckMin))
            //
            // from there, you'd think you could do
            //
            //      (curMopAddr < mopsMemorySize - curRangeCheckMax + 1))
            //
            // but that is only valid if you are certain that curMopAddr>0, due to the unsigned casting...
            // and curMopAddr could be anything, which is really the point of this whole exercise. Instead, settle for
            //
            //      (unsigned(curMopAddr + curRangeCheckMin) <= unsigned(mopsMemorySize - (curRangeCheckMax-curRangeCheckMin)))
            //

            AvmAssert(curRangeCheckMaxValue > curRangeCheckMinValue);
            AvmAssert(curRangeCheckMaxValue - curRangeCheckMinValue <= DomainEnv::GLOBAL_MEMORY_MIN_SIZE);

            curRangeCheckLHS = safeIns2(LIR_addi, curMopAddr, curRangeCheckMinValue);
            curRangeCheckRHS = safeIns2(LIR_subi, curMemSize, curRangeCheckMaxValue - curRangeCheckMinValue);

            LIns* cond = this->ins2(LIR_leui, curRangeCheckLHS, curRangeCheckRHS);
            br = this->insBranch(LIR_jf, cond, NULL);
        }

        return curMemBase;
    }

    // workaround for WE2569232: don't let these adds get specialized or CSE'd.
    LIns* MopsRangeCheckFilter::safeIns2(LOpcode op, LIns* lhs, int32_t rhsConst)
    {
        LIns* rhs = prolog_out->insImmI(rhsConst);
        LIns* ins = out->ins2(op, lhs, rhs);
        AvmAssert(ins->isop(op) && ins->oprnd1() == lhs && ins->oprnd2() == rhs);
        return ins;
    }

    // rewrite the instruction with a new rhs constant
    void MopsRangeCheckFilter::safeRewrite(LIns* ins, int32_t rhsConst)
    {
        LIns* rhs = prolog_out->insImmI(rhsConst);
        AvmAssert(ins->isop(LIR_addi) || ins->isop(LIR_subi));
        ins->initLInsOp2(ins->opcode(), ins->oprnd1(), rhs);
    }

    LIns* MopsRangeCheckFilter::ins0(LOpcode v)
    {
        if (v == LIR_label)
        {
            flushRangeChecks();
            clearMemBaseAndSize();
        }
        return LirWriter::ins0(v);
    }

    LIns* MopsRangeCheckFilter::insCall(const CallInfo *ci, LIns* args[])
    {
        // calls could potentially resize globalMemorySize, so we
        // can't collapse range checks across them
        if (!ci->_isPure)
        {
            flushRangeChecks();
            clearMemBaseAndSize();
        }
        return LirWriter::insCall(ci, args);
    }

    /**
     * ---------------------------------
     * Instruction convenience functions
     * ---------------------------------
     */


    LIns* CodegenLIR::localCopy(int i)
    {
        switch (bt(state->value(i).traits)) {
#ifdef VMCFG_FLOAT
        case BUILTIN_float: 
            return localGetf(i);
        case BUILTIN_float4: 
            return localGetf4(i);
#endif
        case BUILTIN_number:
            return localGetd(i);
        case BUILTIN_boolean:
        case BUILTIN_int:
        case BUILTIN_uint:
            return localGet(i);
        default:
            return localGetp(i);
        }
    }

    void CodegenLIR::localSet(int i, LIns* o, Traits* type)
    {
        BuiltinType tag = bt(type);
        SlotStorageType sst = valueStorageType(tag);
#ifdef DEBUG
        jit_sst[i] = uint16_t(1 << sst); 
#endif
        lirout->insStore(o, vars, i << VARSHIFT(info) , ACCSET_VARS);
        lirout->insStore(LIR_sti2c, InsConst(sst), tags, i, ACCSET_TAGS);
    }

    LIns* CodegenLIR::atomToNativeRep(int i, LIns* atom)
    {
        return atomToNativeRep(state->value(i).traits, atom);
    }

    LIns* CodegenLIR::ptrToNativeRep(Traits*t, LIns* ptr)
    {
        return t->isMachineType() ? addp(ptr, kObjectType) : ptr;
    }

#ifdef _DEBUG
    bool CodegenLIR::isPointer(int i)   {
        return !state->value(i).traits->isMachineType();
    }
#endif

    LIns* CodegenLIR::loadAtomRep(int i)
    {
        return nativeToAtom(localCopy(i), state->value(i).traits);
    }

    LIns* CodegenLIR::storeAtomArgs(int count, int index)
    {
        LIns* ap = insAlloc(sizeof(Atom)*count);
        for (int i=0; i < count; i++)
            stp(loadAtomRep(index++), ap, i * sizeof(Atom), ACCSET_OTHER);
        return ap;
    }

    LIns* CodegenLIR::storeAtomArgs(LIns* receiver, int count, int index)
    {
        #ifdef NJ_VERBOSE
        if (verbose())
            core->console << "          store args\n";
        #endif
        LIns* ap = insAlloc(sizeof(Atom)*(count+1));
        stp(receiver, ap, 0, ACCSET_OTHER);
        for (int i=1; i <= count; i++)
        {
            LIns* v = loadAtomRep(index++);
            stp(v, ap, sizeof(Atom)*i, ACCSET_OTHER);
        }
        return ap;
    }

    CodegenLIR::CodegenLIR(MethodInfo* i, MethodSignaturep ms, Toplevel* toplevel,
                           OSR* osr_state) :
        LirHelper(i->pool()),
        info(i),
        ms(ms),
        toplevel(toplevel),
        pool(i->pool()),
        osr(osr_state),
        driver(NULL),
        state(NULL),
        mopsRangeCheckFilter(NULL),
        restArgc(NULL),
        restLocal(-1),
        interruptable(true),
        npe_label("npe"),
        upe_label("upe"),
        interrupt_label("interrupt"),
        mop_rangeCheckFailed_label("mop_rangeCheckFailed"),
        catch_label("catch"),
        call_error_label("call_error"),
        inlineFastpath(false),
        call_cache_builder(*alloc1, *initCodeMgr(pool)),
        get_cache_builder(*alloc1, *pool->codeMgr),
        set_cache_builder(*alloc1, *pool->codeMgr),
        prolog(NULL),
        skip_ins(NULL),
        specializedCallHashMap(NULL),
        builtinFunctionOptimizerHashMap(NULL),
        blockLabels(NULL),
        cseFilter(NULL),
        noise(),
        jit_debug_info(NULL)
        DEBUGGER_ONLY(, haveDebugger(core->debugger() != NULL) )
    {
        #ifdef AVMPLUS_MAC_CARBON
        setjmpInit();
        #endif

        verbose_only(
            if (pool->isVerbose(VB_jit, i)) {
                core->console << "codegen " << i;
                core->console <<
                    " required=" << ms->requiredParamCount() <<
                    " optional=" << (ms->param_count() - ms->requiredParamCount()) << "\n";
            })
    }

    CodegenLIR::~CodegenLIR() {
        cleanup();
    }

    void CodegenLIR::cleanup()
    {
        LirHelper::cleanup();
    }

    #ifdef AVMPLUS_MAC_CARBON
    int CodegenLIR::setjmpAddress = 0;

    extern "C" int __setjmp();

    asm int CodegenLIR::setjmpDummy(jmp_buf buf)
    {
        b __setjmp;
    }

    void CodegenLIR::setjmpInit()
    {
        // CodeWarrior defies all reasonable efforts to get
        // the address of __vec_setjmp.  So, we resort to
        // a crude hack: We'll search the actual code
        // of setjmpDummy for the branch instruction.
        if (setjmpAddress == 0)
        {
            setjmpAddress = *((int*)&setjmpDummy);
        }
    }
    #endif

    void CodegenLIR::suspendCSE()
    {
        if (cseFilter) cseFilter->suspend();
    }

    void CodegenLIR::resumeCSE()
    {
        if (cseFilter) cseFilter->resume();
    }

    LIns* CodegenLIR::atomToNativeRep(Traits* t, LIns* atom)
    {
        return atomToNative(bt(t), atom);
    }

    bool isNullable(Traits* t) {
        BuiltinType bt = Traits::getBuiltinType(t);

        return bt != BUILTIN_int && bt != BUILTIN_uint && bt != BUILTIN_boolean && bt != BUILTIN_number
                      FLOAT_ONLY(&& bt != BUILTIN_float && bt != BUILTIN_float4); 
    }

    /**
     * Eliminates redundant loads within a block, and tracks the nullability of pointers
     * within blocks and across edges.  CodegenLIR will inform VarTracker that a
     * pointer is not null by calling setNotNull(ptr, type) either when the Verifier's
     * FrameState.FrameValue is not null, in localGetp(), or after a null check in emitNullCheck().
     *
     * Within a block, we track nullability of references to instructions; when references
     * are copied, we know the copies are not null.
     *
     * At block boundaries, different values may flow together, so we track nullability
     * in variable slots instead of specific instruction references.
     */
    class VarTracker: public LirWriter
    {
        Allocator &alloc;               // Allocator for the lifetime of this filter
        LIns** varTracker;              // remembers the last value stored in each var
        LIns** tagTracker;              // remembers the last tag stored in each var
        HashMap<LIns*, bool> *checked;  // pointers we know are not null.
        nanojit::BitSet *notnull;       // stack locations we know are not null
        LIns* vars;                     // LIns that defines the vars[] array
        LIns* tags;                     // LIns that defines the tags[] array
        const int nvar;                 // this method's frame size.
        const int varShift;              // the size of a frame variable in bytes (8 or 16)
        const int scopeBase;            // index of first local scope
        const int stackBase;            // index of first stack slot
        int restLocal;                  // -1 or, if it's lazily allocated, the local holding the rest array

        // false after an unconditional control flow instruction (jump, throw, return),
        // true from the start and after we start a block via trackLabel()
        bool reachable;

        // true if any backedges exist in LIR, false otherwise.
        bool has_backedges;

#ifdef DEBUGGER
        bool haveDebugger;              // true if debugger is currently enabled
#else
        static const bool haveDebugger = false;
#endif
#ifdef AVMPLUS_VERBOSE
        bool verbose;                   // true when generating verbose output
#else
        static const bool verbose = false;
#endif

    public:
        VarTracker(MethodInfo* info, Allocator& alloc, LirWriter *out,
                int nvar, int scopeBase, int stackBase, int restLocal,
                uint32_t code_len)
            : LirWriter(out), alloc(alloc),
              vars(NULL), tags(NULL), nvar(nvar), 
              varShift(VARSHIFT(info)), scopeBase(scopeBase), 
              stackBase(stackBase), restLocal(restLocal),
              reachable(true), has_backedges(false)
#ifdef DEBUGGER
            , haveDebugger(info->pool()->core->debugger() != NULL)
#endif
#ifdef AVMPLUS_VERBOSE
            , verbose(info->pool()->isVerbose(VB_jit, info))
#endif
        {
            (void) info; // suppress warning if !DEBUGGER && !AVMPLUS_VERBOSE
            varTracker = new (alloc) LIns*[nvar];
            tagTracker = new (alloc) LIns*[nvar];
            // allocate a large value until https://bugzilla.mozilla.org/show_bug.cgi?id=565489 is resolved
            checked = new (alloc) InsSet(alloc, code_len < 16700 ? code_len : 16700);
            notnull = new (alloc) nanojit::BitSet(alloc, nvar);
            clearState();
        }

        void init(LIns *vars, LIns* tags) {
            this->vars = vars;
            this->tags = tags;
        }

        bool hasBackedges() const {
            return has_backedges;
        }

        void setNotNull(LIns* ins, Traits* t) {
            if (isNullable(t))
                checked->put(ins, true);
        }

        bool isNotNull(LIns* ins) {
            return checked->containsKey(ins);
        }

        void initNotNull(const FrameState* state) {
            syncNotNull(notnull, state);
        }

        // We're at the start of an AS3 basic block; synchronize our
        // notnull bits for that block with ones from the driver.
        void syncNotNull(nanojit::BitSet* bits, const FrameState* state) {
            int scopeTop = scopeBase + state->scopeDepth;
            int stackTop = stackBase + state->stackDepth;
            if (state->targetOfBackwardsBranch || state->targetOfExceptionBranch) {
                // Clear any notNull bits that are not set in FrameState.
                // This is done at both targets of backward branches and at exception handlers,
                // as we cannot be assured of having seen all branches, thus the varTracker info
                // may not be complete.  This limitation arises from the one-pass algorithm.
                for (int i=0, n=nvar; i < n; i++) {
                    const FrameValue& v = state->value(i);
                    bool stateNotNull = v.notNull && isNullable(v.traits);
                    if (!stateNotNull || (i >= scopeTop && i < stackBase) || (i >= stackTop))
                        bits->clear(i);
                    else
                        bits->set(i);
                }
                printNotNull(bits, "loop label", state);
            } else {
                // Set any notNull bits that are set in FrameState.
                // If we are tracking an expression for a non-null variable,
                // add it to the set of checked expressions.
                for (int i=0, n=nvar; i < n; i++) {
                    const FrameValue& v = state->value(i);
                    bool stateNotNull = v.notNull && isNullable(v.traits);
                    if ((i >= scopeTop && i < stackBase) || (i >= stackTop)) {
                        bits->clear(i);
                    } else if (stateNotNull) {
                        bits->set(i);
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=773479
                        if (varTracker[i] && (i != restLocal))
#else
                        if (varTracker[i])
#endif
                            checked->put(varTracker[i], true);
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=773479
                    } else if (bits->get(i) && varTracker[i] && (i != restLocal))
#else
                    } else if (bits->get(i) && varTracker[i])
#endif
                        checked->put(varTracker[i], true);
                }
                printNotNull(bits, "forward label", state);
            }
        }

        // Model a control flow edge by merging our current state with the state
        // saved at the target.  Used for forward, non-exceptional branches only.
        void trackForwardEdge(CodegenLabel& target) {
            AvmAssert(target.labelIns == NULL);  // illegal to call trackForwardEdge on backedge

            // Merge varTracker/tagTracker state with state at target label.
            // Due to hidden internal control flow in exception dispatch, state may not be
            // propagated across exception edges.  Thus, if a label may be reached from such
            // an edge, we cannot determine accurate state at the label, and must clear it.
            // The state will remain cleared due to the monotonicity of the merge.
            if (!target.varTracker) {
                // Allocate state vectors for target label upon first encounter.
                target.varTracker = new (alloc) LIns*[nvar];
                target.tagTracker = new (alloc) LIns*[nvar];
                VMPI_memcpy(target.varTracker, varTracker, nvar*sizeof(LIns*));
                VMPI_memcpy(target.tagTracker, tagTracker, nvar*sizeof(LIns*));
            } else {
                for (int i=0, n=nvar; i < n; i++) {
                    if (varTracker[i] != target.varTracker[i])
                        target.varTracker[i] = NULL;
                    if (tagTracker[i] != target.tagTracker[i])
                        target.tagTracker[i] = NULL;
                }
            }

            for (int i=0, n=nvar; i < n; i++) {
                if (varTracker[i]) {
                    if (checked->containsKey(varTracker[i]))
                        notnull->set(i);
                    else
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=773479
                        AvmAssert((i == restLocal) || (!notnull->get(i)));
#else
                        AvmAssert(!notnull->get(i));
#endif
                }
            }
            if (!target.notnull) {
                //printf("save state\n");
                target.notnull = new (alloc) nanojit::BitSet(alloc, nvar);
                target.notnull->setFrom(*notnull);
            } else {
                // target.notnull &= notnull
                for (int i=0, n=nvar; i < n; i++)
                    if (!notnull->get(i))
                        target.notnull->clear(i);
            }
        }

#ifdef AVMPLUS_VERBOSE
        void printNotNull(nanojit::BitSet* bits, const char* title,
                          const FrameState* state) {
            if (0 && verbose) {
                if (bits) {
                    int max_scope = scopeBase + state->scopeDepth - 1;
                    int max_stack = stackBase + state->stackDepth;
                    printf("%s [0-%d,%d-%d] notnull = ", title, max_scope+1, stackBase, max_stack-1);
                    for (int i=0; i < max_stack; i = i != max_scope ? i + 1 : stackBase)
                        if (bits->get(i))
                            printf("%d ", i);
                    printf("\n");
                } else {
                    printf("%s notnull = null\n", title);
                }
            }
        }
#else
        void printNotNull(nanojit::BitSet*, const char*, const FrameState*)
        {}
#endif

        void checkBackEdge(CodegenLabel& target, const FrameState* state) {
            has_backedges = true;
#ifdef DEBUG
            AvmAssert(target.labelIns != NULL);
            if (target.notnull) {
                printNotNull(notnull, "current", state);
                printNotNull(target.notnull, "target", state);
                int scopeTop = scopeBase + state->scopeDepth;
                int stackTop = stackBase + state->stackDepth;
                // make sure our notnull bits at the target of the backedge were safe.
                for (int i=0, n=nvar; i < n; i++) {
                    if ((i >= scopeTop && i < stackBase) || i >= stackTop)
                        continue; // skip empty locations
                    if (!isNullable(state->value(i).traits))
                        continue; // skip non-nullable types in current state
                    //  current   target    assert(!target || current)
                    //  -------   ------    ------
                    //  false     false     true
                    //  false     true      false (assertion fires)
                    //  true      false     true
                    //  true      true      true
                    bool currentNotNull = (varTracker[i] ? isNotNull(varTracker[i]) : false) || notnull->get(i);
                    AvmAssert(!target.notnull->get(i) || currentNotNull);
                }
            }
#else
            (void) target;
            (void) state;
#endif // DEBUG
        }

        // starts a new block.  if the new label is reachable from here,
        // merge our state with it.  then initialize from the new merged state.
        void trackLabel(CodegenLabel& label, const FrameState* state) {
            if (reachable)
                trackForwardEdge(label); // model the fall-through path as an edge

            clearState();
            label.labelIns = out->ins0(LIR_label);

            // Load varTracker/tagTracker state accumulated from forward branches.
            // Do not load if there are any backward branches, as the tracker state may
            // not be accurate.  Just switch the pointers -- no need to copy the arrays.
            // Exception branches are assumed to be backward, as lowering to LIR may have
            // introduced backward control flow that was not present in the original ABC.
            if (!state->targetOfBackwardsBranch && label.varTracker && !state->targetOfExceptionBranch) {
                varTracker = label.varTracker;
                tagTracker = label.tagTracker;
            }

            // load state saved at label
            if (label.notnull) {
                syncNotNull(label.notnull, state);
                notnull->setFrom(*label.notnull);
                printNotNull(notnull, "merged label", state);
            } else {
                syncNotNull(notnull, state);
                printNotNull(notnull, "first-time label", state);
            }
            reachable = true;
        }

        // Clear the var and tag expression states, but do not clear the nullability
        // state.  Called around debugger safe points to ensure that we reload values
        // that are possibly modified by the debugger.  Clearing the nullability state
        // correctly must be done at the verifier level, and at that level, it must always
        // be done or never be done (can't be conditional on debugging).
        // FIXME: bug 544238: clearing only the var state has questionable validity
        void clearVarState() {
            VMPI_memset(varTracker, 0, nvar*sizeof(LIns*));
            VMPI_memset(tagTracker, 0, nvar*sizeof(LIns*));
        }

        // clear all nullability and var/tag tracking state at branch targets
        void clearState() {
            clearVarState();
            checked->clear();
            notnull->reset();
        }

        REALLY_INLINE int varOffsetToIndex(int offset) {
            AvmAssert(IS_ALIGNED(offset, 1 << varShift));
            return offset >> varShift;
        }

        // keep track of the value stored in var d and update notnull
        void trackVarStore(LIns *value, int i) {
            varTracker[i] = value;
            if (checked->containsKey(value))
                notnull->set(i);
            else
                notnull->clear(i);
        }

        // keep track of the tag stored in var i.
        void trackTagStore(LIns *value, int i) {
            tagTracker[i] = value;
        }

        // The first time we see a load from variable i, remember it,
        // and if we know that slot is nonnull, add the load instruction to the nonnull set.
        void trackVarLoad(LIns* value, int i) {
            varTracker[i] = value;
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=773479
            if (notnull->get(i) && (i != restLocal))
#else
            if (notnull->get(i))
#endif
                checked->put(value, true);
        }

        // first time we read a tag for variable i, remember it.
        void trackTagLoad(LIns* value, int i) {
            tagTracker[i] = value;
        }

        // monitor loads emitted by the LIR generator, track access to vars and tags
        LIns *insLoad(LOpcode op, LIns *base, int32_t d, AccSet accSet, LoadQual loadQual) {
            if (base == vars) {
                int i = varOffsetToIndex(d);
                LIns *val = varTracker[i];
                if (!val) {
                    val = out->insLoad(op, base, d, accSet, loadQual);
                    FLOAT_ONLY(AvmAssert(!val->isF4() || varShift == 4));
                    trackVarLoad(val, i);
                }
                return val;
            }
            if (base == tags) {
                int i = d; // 1 byte per tag
                LIns *tag = tagTracker[i];
                if (!tag) {
                    tag = out->insLoad(op, base, d, accSet, loadQual);
                    trackTagLoad(tag, i);
                }
                return tag;
            }
            return out->insLoad(op, base, d, accSet, loadQual);
        }

        // monitor all stores emitted by LIR generator, update our tracking state
        // when we see stores to vars or tags.
        LIns *insStore(LOpcode op, LIns *value, LIns *base, int32_t d, AccSet accSet) {
            if (base == vars)
            {
                FLOAT_ONLY(AvmAssert(!value->isF4() || varShift == 4));
                trackVarStore(value, varOffsetToIndex(d));
            }
            else if (base == tags)
                trackTagStore(value, d);
            return out->insStore(op, value, base, d, accSet);
        }

        // we expect the frontend to use CodegenLabels and call trackLabel for all
        // LIR label creation.  Assert to prevent unknown label generation.
        LIns *ins0(LOpcode op) {
            AvmAssert(op != LIR_label); // trackState must be called directly to generate a label.
            return out->ins0(op);
        }

        // set reachable = false after return instructions
        LIns* ins1(LOpcode op, LIns* a) {
            if (isRetOpcode(op))
                reachable = false;
            return out->ins1(op, a);
        }

        // set reachable = false after unconditional jumps
        LIns *insBranch(LOpcode v, LIns* cond, LIns* to) {
            if (v == LIR_j)
                reachable = false;
            return out->insBranch(v, cond, to);
        }

        // set reachable = false after LIR_jtbl which has explicit targets for all cases
        LIns *insJtbl(LIns* index, uint32_t size) {
            reachable = false;
            return out->insJtbl(index, size);
        }

        // assume any non-pure function can throw an exception, and that pure functions cannot.
        bool canThrow(const CallInfo* call)
        {
            return !call->_isPure;
        }

        // if debugging is attached, clear our tracking state when calling side-effect
        // fucntions, which are effectively debugger safe points.
        // also set reachable = false if the function is known to always throw, and never return.
        LIns *insCall(const CallInfo *call, LIns* args[]) {
            if (haveDebugger && canThrow(call))
                clearVarState(); // debugger might have modified locals, so make sure we reload after call.
            if (neverReturns(call))
                reachable = false;
            if (call->_address == (uintptr_t)&restargHelper) {
                // That helper has a by-reference argument which points into the vars array
                AvmAssert(restLocal != -1);
                varTracker[restLocal] = 0;
            }
            return out->insCall(call, args);
        }
    };

    LIns* CodegenLIR::localGet(int i) {
#ifdef DEBUG
        const FrameValue& v = state->value(i);
        AvmAssert((v.sst_mask == (1 << SST_int32) && v.traits == INT_TYPE) ||
                  (v.sst_mask == (1 << SST_uint32) && v.traits == UINT_TYPE) ||
                  (v.sst_mask == (1 << SST_bool32) && v.traits == BOOLEAN_TYPE));
#endif
        return lirout->insLoad(LIR_ldi, vars, i << VARSHIFT(info) , ACCSET_VARS);
    }

    LIns* CodegenLIR::localGetf(int i) {
#ifdef VMCFG_FLOAT
#ifdef DEBUG
        const FrameValue& v = state->value(i);
        AvmAssert(v.sst_mask == (1<<SST_float) && v.traits == FLOAT_TYPE);
#endif
        return lirout->insLoad(LIR_ldf, vars, i << VARSHIFT(info), ACCSET_VARS);
#else
        toplevel->throwError(kNotImplementedError); (void)i;
        return NULL;
#endif
    }

    LIns* CodegenLIR::localGetd(int i) {
#ifdef DEBUG
        const FrameValue& v = state->value(i);
        AvmAssert((v.sst_mask == (1<<SST_double) && v.traits == NUMBER_TYPE));
#endif
        return lirout->insLoad(LIR_ldd, vars, i << VARSHIFT(info) , ACCSET_VARS);
    }

#ifdef VMCFG_FLOAT
    LIns* CodegenLIR::localGetf4(int i) {
#ifdef DEBUG
        const FrameValue& v = state->value(i);
        AvmAssert(v.sst_mask == (1<<SST_float4) && v.traits == FLOAT4_TYPE);
        AvmAssert(VARSHIFT(info) == 4);
#endif
        return lirout->insLoad(LIR_ldf4, vars, i << 4, ACCSET_VARS);
    }

    LIns* CodegenLIR::localGetf4Addr(int i) {
#ifdef DEBUG
        const FrameValue& v = state->value(i);
        AvmAssert(v.sst_mask == (1<<SST_float4) && v.traits == FLOAT4_TYPE);
        AvmAssert(VARSHIFT(info) == 4);
#endif
        return lea(i << 4, vars);
    }
#endif

    // Load a pointer-sized var, and update null tracking state if the driver
    // informs us that it is not null via FrameState.value.
    LIns* CodegenLIR::localGetp(int i)
    {
        const FrameValue& v = state->value(i);
        LIns* ins;
        if (exactlyOneBit(v.sst_mask)) {
            // pointer or atom
            AvmAssert(!(v.sst_mask == (1 << SST_int32) && v.traits == INT_TYPE) &&
                      !(v.sst_mask == (1 << SST_uint32) && v.traits == UINT_TYPE) &&
                      !(v.sst_mask == (1 << SST_bool32) && v.traits == BOOLEAN_TYPE) &&
FLOAT_ONLY(           !(v.sst_mask == (1 << SST_float)  && v.traits == FLOAT_TYPE)   &&
                      !(v.sst_mask == (1 << SST_float4) && v.traits == FLOAT4_TYPE)  && )
                      !(v.sst_mask == (1 << SST_double) && v.traits == NUMBER_TYPE));
            ins = lirout->insLoad(LIR_ldp, vars, i << VARSHIFT(info) , ACCSET_VARS);
        } else {
            // more than one representation is possible: convert to atom using tag found at runtime.
            AvmAssert(bt(v.traits) == BUILTIN_any || bt(v.traits) == BUILTIN_object);
            LIns* tag = lirout->insLoad(LIR_lduc2ui, tags, i, ACCSET_TAGS);
            LIns* varAddr = lea( i << VARSHIFT(info) , vars);
            ins = callIns(FUNCTIONID(makeatom), 3, coreAddr, varAddr, tag);
        }
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=773479
        // A sentinel null value may be stored in the rest local slot even though the
        // verifier believes the slot is non-null.  This is valid from the ABC standpoint,
        // as the rest arg array will be created on demand if the slot is accessed, and
        // the null sentinel can never be observed.  We do not want to infer that 'ins' is
        // itself non-null in this case, however, as it may be used elsewhere.
        if (v.notNull && (i != restLocal))
#else
        if (v.notNull)
#endif
            varTracker->setNotNull(ins, v.traits);
        return ins;
    }

    LIns* CodegenLIR::callIns(const CallInfo *ci, uint32_t argc, ...)
    {
        // A LIR_call that throws an exception may not appear in the same position
        // with respect to the handler as did the ABC instruction that notionally
        // performed the throw.  As a result, the backwards-branch information
        // collected by the verifier is not reliable for exception edges.  We thus
        // do not attempt to model exceptions in the VarTracker, but conservatively
        // treat all exception handlers as if they were backward branch targets.
        va_list ap;
        va_start(ap, argc);
        LIns* ins = LirHelper::vcallIns(ci, argc, ap);
        va_end(ap);
        return ins;
    }

#if defined(DEBUGGER) && defined(_DEBUG)
    // The AS debugger requires type information for variables contained
    // in the AS frame regions (i.e. 'vars').  In the interpreter this
    // is not an issues, since the region contains box values (i.e. Atoms)
    // and so the type information is self-contained.  With the jit, this is
    // not the case, and thus 'tags' is used to track the type of each
    // variable in 'vars'.
    // This filter watches stores to 'vars' and 'tags' and upon encountering
    // debugline (i.e. place where debugger can halt), it ensures that the
    // tags entry is consistent with the value stored in 'vars'
    class DebuggerCheck : public LirWriter
    {
        AvmCore* core;
        LIns** varTracker;
        LIns** tagTracker;
        LIns *vars;
        LIns *tags;
        int nvar;
        int varShift;
    public:
        DebuggerCheck(AvmCore* core, Allocator& alloc, LirWriter *out, int nvar, int varShift)
            : LirWriter(out), core(core), vars(NULL), tags(NULL), nvar(nvar), varShift(varShift)
        {
            varTracker = new (alloc) LIns*[nvar];
            tagTracker = new (alloc) LIns*[nvar];
            clearState();
        }

        void init(LIns *vars, LIns *tags) {
            this->vars = vars;
            this->tags = tags;
        }

        void trackVarStore(LIns *value, int d) {
            AvmAssert(IS_ALIGNED(d, 1 << varShift));
            FLOAT_ONLY(AvmAssert(varShift == 4 || !value->isF4());)
            int i = d >> varShift;
            if (i >= nvar)
                return;
            varTracker[i] = value;
        }

        void trackTagStore(LIns *value, int d) {
            int i = d; // 1 byte per tag
            if (i >= nvar)
                return;
            tagTracker[i] = value;
            checkValid(i);
            tagTracker[i] = (LIns*)((intptr_t)value|1); // lower bit => validated;
        }

        void clearState() {
            VMPI_memset(varTracker, 0, nvar * sizeof(LIns*));
            VMPI_memset(tagTracker, 0, nvar * sizeof(LIns*));
        }

        void checkValid(int i) {
            // @pre tagTracker[i] has been previously filled
            LIns* val = varTracker[i];
            LIns* tra = tagTracker[i];
            NanoAssert(val && tra);

            switch ((SlotStorageType) tra->immI()) {
            case SST_double:
                AvmAssert(val->isQorD());
                break;
#ifdef VMCFG_FLOAT
            case SST_float:
                AvmAssert(val->isF()); 
                break;
            case SST_float4:
                AvmAssert(val->isF4()); 
                break;
#endif

            case SST_int32:
            case SST_uint32:
            case SST_bool32:
                AvmAssert(val->isI());
                break;
            default:
                AvmAssert(val->isP());
                break;
            }
        }

        void checkState() {
            for (int i=0; i < this->nvar; i++) {
                LIns* val = varTracker[i];
                LIns* tra = tagTracker[i];
                AvmAssert(val && tra);

                // isValid should have already been called on everything
                AvmAssert(((intptr_t)tra&0x1) == 1);
            }
        }

        LIns *insCall(const CallInfo *call, LIns* args[]) {
            if (call == FUNCTIONID(debugLine))
                checkState();
            return out->insCall(call,args);
        }

        LIns *insStore(LOpcode op, LIns *value, LIns *base, int32_t d, AccSet accSet) {
            if (base == vars)
                trackVarStore(value, d);
            else if (base == tags)
                trackTagStore(value, d);
            return out->insStore(op, value, base, d, accSet);
        }

    };
#endif

    // writer for the prolog.  instructions written here dominate code in the
    // body, and so are added to the body's CseFilter
    class PrologWriter : public LirWriter
    {
    public:
        LIns* lastIns;
        LIns* env_scope;
        LIns* env_vtable;
        LIns* env_abcenv;
        LIns* env_domainenv;
        LIns* env_toplevel;
        LIns* env_finddef_table;

        PrologWriter(LirWriter *out):
            LirWriter(out),
            lastIns(NULL),
            env_scope(NULL),
            env_vtable(NULL),
            env_abcenv(NULL),
            env_domainenv(NULL),
            env_toplevel(NULL),
            env_finddef_table(NULL)
        {}

        virtual LIns* ins0(LOpcode v) {
            return lastIns = out->ins0(v);
        }
        virtual LIns* ins1(LOpcode v, LIns* a) {
            return lastIns = out->ins1(v, a);
        }
        virtual LIns* ins2(LOpcode v, LIns* a, LIns* b) {
            return lastIns = out->ins2(v, a, b);
        }
        virtual LIns* ins3(LOpcode v, LIns* a, LIns* b, LIns* c) {
            return lastIns = out->ins3(v, a, b, c);
        }
        virtual LIns* ins4(LOpcode v, LIns* a, LIns* b, LIns* c, LIns* d) {
            return lastIns = out->ins4(v, a, b, c, d);
        }
        virtual LIns* insGuard(LOpcode v, LIns *c, GuardRecord *gr) {
            return lastIns = out->insGuard(v, c, gr);
        }
        virtual LIns* insBranch(LOpcode v, LIns* condition, LIns* to) {
            return lastIns = out->insBranch(v, condition, to);
        }
        // arg: 0=first, 1=second, ...
        // kind: 0=arg 1=saved-reg
        virtual LIns* insParam(int32_t arg, int32_t kind) {
            return lastIns = out->insParam(arg, kind);
        }
        virtual LIns* insImmI(int32_t imm) {
            return lastIns = out->insImmI(imm);
        }
#ifdef AVMPLUS_64BIT
        virtual LIns* insImmQ(uint64_t imm) {
            return lastIns = out->insImmQ(imm);
        }
#endif
        virtual LIns* insImmD(double d) {
            return lastIns = out->insImmD(d);
        }
        virtual LIns* insImmF(float f) {
            return lastIns = out->insImmF(f);
        }
        virtual LIns* insImmF4(const float4_t& f) {
            return lastIns = out->insImmF4(f);
        }
        virtual LIns* insLoad(LOpcode op, LIns* base, int32_t d, AccSet accSet, LoadQual loadQual) {
            return lastIns = out->insLoad(op, base, d, accSet, loadQual);
        }
        virtual LIns* insStore(LOpcode op, LIns* value, LIns* base, int32_t d, AccSet accSet) {
            return lastIns = out->insStore(op, value, base, d, accSet);
        }
        // args[] is in reverse order, ie. args[0] holds the rightmost arg.
        virtual LIns* insCall(const CallInfo *call, LIns* args[]) {
            return lastIns = out->insCall(call, args);
        }
        virtual LIns* insAlloc(int32_t size) {
            NanoAssert(size != 0);
            return lastIns = out->insAlloc(size);
        }
        virtual LIns* insJtbl(LIns* index, uint32_t size) {
            return lastIns = out->insJtbl(index, size);
        }
        virtual LIns* insSwz(LIns* a, uint8_t mask) {
            return lastIns = out->insSwz(a, mask);
        }
    };

    #ifdef DEBUG
    FUNCTION(FUNCADDR(OSR::checkBugCompatibility), SIG1(V, P), osr_check_bugcompatibility)
    #endif

    // Generate the prolog for a function with this C++ signature:
    //
    //    <return-type> f(MethodEnv* env, int argc, void* args)
    //
    // argc is the number of arguments, not counting the receiver
    // (aka "this"). args points to the arguments in memory:
    //
    //    [receiver] [arg1, arg2, ... ]
    //
    // the arguments in memory are typed according to the AS3 method
    // signature.  types * and Object are represented as Atom, and all
    // other types are native pointers or values.  return-type is whatever
    // the native type is for the AS3 return type; one of double, int32_t,
    // uint32_t, ScriptObject*, String*, Namespace*, Atom, or void.
    //
    // The stack frame layout of a jit-compiled function is determined by
    // the jit backend.  Stack-allocated structs are declared in LIR with
    // a LIR_allocp instruction.  Incoming parameters are declared with LIR_paramp
    // instructions, and any other local variables with function-body scope
    // and lifetime are declared with the expressions that compute them.
    // The backend will also allocate additional stack space for spilled values
    // and callee-saved registers.  The VM and LIR do not currently depend on how
    // the backend organizes the stack frame.
    //
    // Incoming parameters:
    //
    // env_param (LIR_paramp, MethodEnv*) is the incoming MethodEnv* parameter
    // that provides access to the environment for this function and all vm services.
    //
    // argc_param (LIR_paramp, int32_t) the # of arguments that follow.  Ignored
    // when the # of args is fixed, but otherwise used for optional arg processing
    // and/or creating the rest[] or arguments[] arrays for undeclared varargs.
    //
    // ap_param (LIR_paramp, uint32_t*) pointer to (argc+1) incoming arguments.
    // arguments are packed.  doubles are sizeof(double), everything else is sizeof(Atom).
    //
    // Distinguished locals:
    //
    // methodFrame (LIR_allocp, MethodFrame*) is the current MethodFrame.  in the prolog
    // we push this onto the call stack pointed to by AvmCore::currentMethodFrame, and
    // in the epilog we pop it back off.
    //
    // coreAddr (LIR_immi|LIR_immq) constant address of AvmCore*.  used in lots of places.
    // undefConst (LIR_immi|LIR_immq) constant value = undefinedAtom. used all over.
    //
    // vars (LIR_allocp) storage for ABC stack frame variables.  8 bytes per variable,
    // always, laid out according to ABC param/local var numbering.  The total number
    // is local_count + scope_depth + stack_depth, i.e. enough for the whole ABC frame.
    // values at any given point in the jit code are are represented according to the
    // statically known type of the variable at that point in the code.  (the type and
    // representation may change at different points.  CodegenDriver maintains
    // the known static types of variables and exposes them via FrameState.
    //
    // tags (LIR_allocp) SlotStorageType of each var in vars, one byte per variable.
    //
    // The contents of vars+tags are up-to-date at all labels and debugging safe points.
    // Inbetween those points, the contents are stale; the JIT optimizes away
    // stores and loads in straightline code.  Additional dead stores
    // are elided by deadvars_analyze() and deadvars_kill().
    //
    // Locals for Debugger use, only present when Debugger is in use:
    //
    // csn (LIR_allocp, CallStackNode).  extra information about this call frame
    // used by the debugger and also used for constructing human-readable stack traces.
    //
    // Locals for Exception-handling, only present when method has try/catch blocks:
    //
    // _save_eip (LIR_allocp, intptr_t) storage for the current ABC-based "pc", used by exception
    // handling to determine which catch blocks are in scope.  The value is an ABC
    // instruction offset, which is how catch handler records are indexed.
    //
    // _ef (LIR_allocp, ExceptionFrame) an instance of struct ExceptionFrame, including
    // a jmp_buf holding our setjmp() state, a pointer to the next outer ExceptionFrame,
    // and other junk.
    //
    // setjmpResult (LIR_call, int) result from calling setjmp; feeds a conditional branch
    // that surrounds the whole function body; logic to pick a catch handler and jump to it
    // is compiled after the function body.  if setjmp returns a nonzero result then we
    // jump forward, pick a catch block, then jump backwards to the catch block.
    //

    void CodegenLIR::writePrologue(const FrameState* state, const uint8_t* pc,
            CodegenDriver* driver)
    {
        this->state = state;
        this->driver = driver;
        this->code_pos = pc;
        this->try_from = driver->getTryFrom();
        this->try_to = driver->getTryTo();
        framesize = ms->frame_size();

        int32_t codeLength = info->parse_code_length();

        // Enable inline fastpath optimizations for sufficiently small methods.
        //
        // An overwhelming majority of methods in the Brightspot collection
        // are less than 50K bytes of ABC in length, and we see that JIT
        // times for methods of this length consistently fall in the 10ms
        // or less range with no outliers to suggest algorithmic pathologies.
        // The framesize limit is rather arbitrary, and is set to keep within
        // the boundaries of available brightspot data.  Note that large methods
        // appear more common in Alchemy-generated code, with a 500KB method
        // in Quake, for example.  We are admittedly conservative here.
        // We see only a modest increase in native code size with the present
        // suite of inlining optimizations and opcode mix in the code we examined.
        //
        // The issue we are attempting to guard against is non-linear blowup of
        // analysis algorithms such as deadvars() that are stressed by the introduction
        // of many new labels and branches.  Our heuristic seeks to confine the fastpath
        // optimization to an "envelope" for we have good empirical data, without
        // attempting to establish its true outer limits.  Additionally, from first
        // principles, we know that specific characteristics of the flow graph are
        // more relevant than length alone, and even a count of the labels and branches
        // would likely give a more precise result than overall code lenth.
        // The preferred long-term solution is to perform actual acconting for JIT
        // resource consumption (memory and time), and aborting when reasonable limits
        // are exceeded.
        //
        if  (core->config.jitconfig.opt_inline) {
            if (codeLength < 50000 && framesize < 1000) {
                inlineFastpath = true;
            } else {
                //core->console << "disabling inline fastpaths, frame size " << framesize << " code length " << codeLength << "\n";
            }
        }

#if defined(NANOJIT_ARM) && defined(VMCFG_FLOAT)
        // The LIR instructions for float and float4 are not supported by nanojit
        // for soft float, and we are planning to rip out soft float entirely.
        // While this is really a nanojit issue, we've already removed VMCFG_FLOAT
        // from nanojit, so we put the check here.
        AvmAssert(!core->config.njconfig.soft_float);
#endif

        if (info->needRestOrArguments() && info->lazyRest())
            restLocal = ms->param_count()+1;

        frag = new (*lir_alloc) Fragment(pc verbose_only(, 0));
        LirBuffer *prolog_buf = frag->lirbuf = new (*lir_alloc) LirBuffer(*lir_alloc);
        prolog_buf->abi = ABI_CDECL;

        lirout = new (*alloc1) LirBufWriter(prolog_buf, core->config.njconfig);

        verbose_only(
            vbNames = 0;
            if (verbose()) {
                vbNames = new (*lir_alloc) LInsPrinter(*lir_alloc, TR_NUM_USED_ACCS);
                vbNames->addrNameMap->addAddrRange(pool->core, sizeof(AvmCore), 0, "core");
                prolog_buf->printer = vbNames;
            }
        )
        debug_only(
            lirout = validate2 = new (*alloc1) ValidateWriter(lirout, prolog_buf->printer,
                                                  "writePrologue(prologue)");
        )
        verbose_only(
            vbWriter = 0;
            if (verbose())
                lirout = vbWriter = new (*alloc1) VerboseWriter(*alloc1, lirout, vbNames, &pool->codeMgr->log, "PROLOG");
        )
        prolog = new (*alloc1) PrologWriter(lirout);
        redirectWriter = lirout = new (*lir_alloc) LirWriter(prolog);
        if (core->config.njconfig.cseopt)
            lirout = cseFilter = new (*alloc1) CseFilter(lirout, TR_NUM_USED_ACCS, *alloc1);
#if defined(NANOJIT_ARM)
        if (core->config.njconfig.soft_float)
            lirout = new (*alloc1) SoftFloatFilter(lirout);
#endif
        lirout = new (*alloc1) ExprFilter(lirout);

        #ifdef DEBUGGER
        dbg_framesize = ms->local_count() + ms->max_scope();
        #ifdef DEBUG
        DebuggerCheck *checker = NULL;
        if (haveDebugger) {
            checker = new (*alloc1) DebuggerCheck(core, *alloc1, lirout, dbg_framesize, VARSHIFT(info));
            lirout = checker;
        }
        #endif // DEBUG
        #endif // DEBUGGER

        emitStart(*alloc1, prolog_buf, lirout);

        // add the VarTracker filter last because we want it to be first in line.
        lirout = varTracker = new (*alloc1) VarTracker(info, *alloc1, lirout,
                framesize, ms->scope_base(), ms->stack_base(), restLocal, codeLength);

        // last pc value that we generated a store for
        lastPcSave = NULL;

        //
        // generate lir to define incoming method arguments.  Stack
        // frame allocations follow.
        //

        env_param = lirout->insParam(0, 0);
        argc_param = lirout->insParam(1, 0);
    #ifdef AVMPLUS_64BIT
        argc_param = lirout->ins1(LIR_q2i, argc_param);
    #endif
        ap_param = lirout->insParam(2, 0);

        // allocate room for a MethodFrame structure
        methodFrame = insAlloc(sizeof(MethodFrame));
        verbose_only( if (vbNames) {
            vbNames->lirNameMap->addName(methodFrame, "methodFrame");
        })

        coreAddr = InsConstPtr(core);

        #ifdef VMCFG_TELEMETRY_SAMPLER
        // Emit the code to take a sample of the MethodFrame stack. This is only
        // emitted when the sampler is enabled so there is no performance impact
        // otherwise.
        if(core->samplerEnabled) {
            LIns* sampleTicks = loadIns(LIR_ldi, offsetof(AvmCore,sampleTicks), coreAddr, ACCSET_OTHER);
            CodegenLabel &afterTakeSample_label = createLabel("afterTakeSample");
            branchToLabel(LIR_jf, sampleTicks, afterTakeSample_label);
            callIns(FUNCTIONID(takeSampleWrapper), 1, coreAddr);
            emitLabel(afterTakeSample_label);
        }
        #endif

        // replicate MethodFrame ctor inline
        LIns* currentMethodFrame = loadIns(LIR_ldp, offsetof(AvmCore,currentMethodFrame), coreAddr, ACCSET_OTHER);
        // save env in MethodFrame.envOrCodeContext
        //     explicitly leave IS_EXPLICIT_CODECONTEXT clear
        //     explicitly leave DXNS_NOT_NULL clear, dxns is effectively null without doing the store here.
        stp(env_param, methodFrame, offsetof(MethodFrame,envOrCodeContext), ACCSET_OTHER);
        stp(currentMethodFrame, methodFrame, offsetof(MethodFrame,next), ACCSET_OTHER);
        stp(methodFrame, coreAddr, offsetof(AvmCore,currentMethodFrame), ACCSET_OTHER);
        #ifdef _DEBUG
        // poison MethodFrame.dxns since it's uninitialized by default
        stp(InsConstPtr((void*)(uintptr_t)0xdeadbeef), methodFrame, offsetof(MethodFrame,dxns), ACCSET_OTHER);
        #endif

        #ifdef DEBUG
        // Check that the BugCompatibility that would be used to OSR this function,
        // whether or not we actually did so, agrees with the value returned from
        // currentBugCompatibility() every time the function is executed.  Note that
        // it would be preferable to modify currentBugCompatibility() such that it is
        // structurally impossible for this test to fail.
        callIns(FUNCTIONID(osr_check_bugcompatibility), 1, env_param);
        #endif

        // allocate room for our local variables
        vars = insAlloc(framesize << VARSHIFT(info));   // room for double|Atom|int|pointer, and float4 if used 
        tags = insAlloc(framesize);             // one tag byte per var
        prolog_buf->sp = vars;
        varTracker->init(vars, tags);

        verbose_only( if (prolog_buf->printer) {
            prolog_buf->printer->lirNameMap->addName(env_param, "env");
            prolog_buf->printer->lirNameMap->addName(argc_param, "argc");
            prolog_buf->printer->lirNameMap->addName(ap_param, "ap");
            prolog_buf->printer->lirNameMap->addName(vars, "vars");
            prolog_buf->printer->lirNameMap->addName(tags, "tags");
        })

        debug_only(
            void** extras = new (*alloc1) void*[2];
            extras[0] = vars;
            extras[1] = tags;
            validate1->setCheckAccSetExtras(extras);
            validate2->setCheckAccSetExtras(extras);
        )

        // stack overflow check - use methodFrame address as comparison
        LIns *d = loadIns(LIR_ldp, offsetof(AvmCore, minstack), coreAddr, ACCSET_OTHER);
        LIns *c = binaryIns(LIR_ltup, methodFrame, d);
        CodegenLabel &begin_label = createLabel("begin");
        branchToLabel(LIR_jf, c, begin_label);
        callIns(FUNCTIONID(handleStackOverflowMethodEnv), 1, env_param);
        emitLabel(begin_label);

        // we emit the undefined constant here since we use it so often and
        // to ensure it dominates all uses.
        undefConst = InsConstAtom(undefinedAtom);

        // whether this sequence is interruptable or not.
        interruptable = ! info->isNonInterruptible();

        // then space for the exception frame, be safe if its an init stub
        if (driver->hasReachableExceptions()) {
            // [_save_eip][ExceptionFrame]
            // offsets of local vars, rel to current ESP
            _save_eip = insAlloc(sizeof(intptr_t));
            _ef       = insAlloc(sizeof(ExceptionFrame));
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(_save_eip, "_save_eip");
                vbNames->lirNameMap->addName(_ef, "_ef");
            })
        } else {
            _save_eip = NULL;
            _ef = NULL;
        }

        #ifdef DEBUGGER
        if (haveDebugger) {
            // tell the sanity checker about vars and tags
            debug_only( checker->init(vars, tags); )

            // Allocate space for the call stack
            csn = insAlloc(sizeof(CallStackNode));
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(csn, "csn");
            })
        }
        #ifdef VMCFG_STACK_METRICS
        callIns(FUNCTIONID(recordStackPointerWrapper), 1, coreAddr);        
        #endif

        #endif

#ifdef DEBUG
        jit_sst = new (*alloc1) uint16_t[framesize];
        memset(jit_sst, 0, framesize * sizeof(uint16_t));
#endif

        //
        // copy args to local frame
        //

        // copy required args, and initialize optional args.
        // this whole section only applies to functions that actually
        // have arguments.

        const int param_count = ms->param_count();
        const int optional_count = ms->optional_count();
        const int required_count = param_count - optional_count;

        LIns* apArg = ap_param;
        if (info->hasOptional())
        {
            // compute offset of first optional arg
            int offset = 0;
            for (int i=0, n=required_count; i <= n; i++)
                offset += argSize(ms, i);

            // now copy the default optional values
            LIns* argcarg = argc_param;
            for (int i=0, n=optional_count; i < n; i++)
            {
                // first set the local[p+1] = defaultvalue
                int param = i + required_count; // 0..N
                int loc = param+1;

                LIns* defaultVal = InsConstAtom(ms->getDefaultValue(i));
                defaultVal = atomToNativeRep(loc, defaultVal);
                localSet(loc, defaultVal, state->value(loc).traits);

                // then generate: if (argc > p) local[p+1] = arg[p+1]
                LIns* cmp = binaryIns(LIR_lei, argcarg, InsConst(param));
                CodegenLabel& optional_label = createLabel("param_", i);
                branchToLabel(LIR_jt, cmp, optional_label); // will patch
                copyParam(loc, offset);
                emitLabel(optional_label);
            }
        }
        else
        {
            // !info->hasOptional()
            AvmAssert(optional_count == 0);
        }

        // now set up the required args (we can ignore argc)
        // for (int i=0, n=param_count; i <= n; i++)
        //     framep[i] = argv[i];
        int offset = 0;
        for (int i=0, n=required_count; i <= n; i++)
            copyParam(i, offset);

        if (info->unboxThis())
        {
            localSet(0, atomToNativeRep(0, localGet(0)), state->value(0).traits);
        }

        int firstLocal = 1+param_count;

        // Capture remaining args.
        //
        // Optimized ...rest and 'arguments':
        //
        // We avoid constructing the rest array if possible.  An analysis in the
        // verifier sets the _lazyRest bit if access patterns to the rest argument
        // or the arguments array are only OBJ.length or OBJ[prop] for arbitrary
        // propery; see comments in Verifier::verify.  The former pattern
        // results in an OP_restargc instruction, while the latter results in an
        // OP_restarg instruction.  Those instructions will access an unconsed
        // rest array or arguments array when possible, and otherwise access a
        // constructed rest array.  (For example, if prop turns out to be "slice"
        // we must construct the array.  This will almost never happen.) They're
        // implemented via helper functions restargcHelper and restargHelper.
        //
        // The unconsed rest or arguments array, the argument count, the consed array,
        // and the flag that determines whether to use the unconsed or the consed array,
        // are represented as follows:
        //
        //  - The unconsed array restArg is represented indirectly via ap_param and
        //    rest_offset.
        //  - The argument count restArgc is a LIR expression of type uint32 computed from
        //    argc_param and param_count.
        //  - The rest parameter local is an array, it is either null (no rest array
        //    consed yet) or a raw array pointer, so it doubles as the flag.  The offset
        //    of this variable is 1+param_count.
        //
        // The rest parameter local is passed by reference to restargHelper, which
        // may update it.
        //
        // The difference between a ...rest argument and an arguments array are that in
        // the former case,
        //
        //    restArgc = MAX(argc_param - param_count, 0)
        //    restArg = is ap_param + rest_offset
        //
        // while in the latter case
        //
        //    restArgc = argc_param
        //    restArg = ap_param + 1
        //
        // restArg is computed in the code generation case for OP_restarg.

        if (info->needRest())
        {
            if (info->lazyRest())
            {
                LIns* x0 = binaryIns(LIR_subi, argc_param, InsConst(param_count));
                LIns* x1 = binaryIns(LIR_lti, x0, InsConst(0));
                restArgc = lirout->insChoose(x1, InsConst(0), x0, use_cmov);

                // Store a NULL array pointer
                localSet(firstLocal, InsConstPtr(0), ARRAY_TYPE);
            }
            else
            {
                //framep[info->param_count+1] = createRest(env, argv, argc);
                // use csop so if rest value never used, we don't bother creating array
                LIns* rest = callIns(FUNCTIONID(createRestHelper), 3,
                    env_param, argc_param, apArg);
                localSet(firstLocal, rest, ARRAY_TYPE);
            }
            firstLocal++;
        }
        else if (info->needArguments())
        {
            if (info->lazyRest())
            {
                restArgc = argc_param;

                // Store a NULL array pointer
                localSet(firstLocal, InsConstPtr(0), ARRAY_TYPE);
            }
            else {
                //framep[info->param_count+1] = createArguments(env, argv, argc);
                // use csop so if arguments never used, we don't create it
                LIns* arguments = callIns(FUNCTIONID(createArgumentsHelper), 3,
                    env_param, argc_param, apArg);
                localSet(firstLocal, arguments, ARRAY_TYPE);
            }
            firstLocal++;
        }

        // set remaining locals to undefined
        for (int i=firstLocal, n = ms->local_count(); i < n; i++) {
            AvmAssert(state->value(i).traits == NULL);
            localSet(i, undefConst, NULL); // void would be more precise
        }

        /// SWITCH PIPELINE FROM PROLOG TO BODY
        verbose_only( if (vbWriter) { vbWriter->flush();} )
        // we have written the prolog to prolog_buf, now create a new
        // LirBuffer to hold the body, and redirect further output to the body.
        LirBuffer *body_buf = new (*lir_alloc) LirBuffer(*lir_alloc);
        LirWriter *body = new (*alloc1) LirBufWriter(body_buf, core->config.njconfig);
        skip_ins = body->insSkip(prolog->lastIns);
        debug_only(
            body = validate3 = new (*alloc1) ValidateWriter(body, vbNames, "writePrologue(body)");
            validate3->setCheckAccSetExtras(extras);
        )
        verbose_only(
            if (verbose()) {
                AvmAssert(vbNames != NULL);
                body_buf->printer = vbNames;
                body = vbWriter = new (*alloc1) VerboseWriter(*alloc1, body, vbNames, &pool->codeMgr->log);
            }
        )
        redirectWriter->out = body;
        /// END SWITCH CODE

        varTracker->initNotNull(state);

        if (osr)
            emitOsrBranch();

        // Generate code to initialize the object, if we are compiling an initializer.
        // This is intentionally before debugEnter(), to match interpreter behavior.
        if (info->isConstructor())
            emitInitializers();

        if (haveDebugger)
            emitDebugEnter();

        if (driver->hasReachableExceptions()) {
            // _ef.beginTry(core);
            callIns(FUNCTIONID(beginTry), 2, _ef, coreAddr);

            // Exception* setjmpResult = setjmp(_ef.jmpBuf);
            // ISSUE this needs to be a cdecl call
            LIns* jmpbuf = lea(offsetof(ExceptionFrame, jmpbuf), _ef);
            setjmpResult = callIns(FUNCTIONID(fsetjmp), 2, jmpbuf, InsConst(0));

            // If (setjmp() != 0) goto catch dispatcher, which we generate in the epilog.
            // Note that register contents following setjmp return via longjmp are not predictable.
            branchToLabel(LIR_jf, eqi0(setjmpResult), catch_label);
        }
        verbose_only( if (vbWriter) { vbWriter->flush();} )
    }

    FUNCTION(FUNCADDR(OSR::adjustFrame), SIG4(B, P, P, P, P), osr_adjust_frame)

    // Emit code to call OSR::adjust_frame and conditionally enter loop.
    // Note that adjust_frame will call debugEnter if necessary, since
    // the loop-branch will skip the normal call to debugEnter.
    void CodegenLIR::emitOsrBranch()
    {
        // Compiling an OSR entry point; save FrameState at the OSR loop header,
        // for later use by adjust_frame().
        FrameState* osr_state = mmfx_new(FrameState(ms, info));
        const FrameState* loop_state = driver->getFrameState(osr->osrPc());
        AvmAssert(loop_state->targetOfBackwardsBranch);
        osr_state->init(loop_state);
        osr->setFrameState(osr_state);
        // if (exec->current_osr) {
        //    adjustFrame(...);
        //    goto osr_pc;
        // }
        BaseExecMgr* exec = (BaseExecMgr*) core->exec;
        // It is safe to emit &exec->current_osr as a constant here because
        // it never changes during runtime.
        LIns* current_osr = ldp(InsConstPtr(&exec->current_osr), 0, ACCSET_OTHER);
        CodegenLabel &no_osr_label = createLabel("no_osr");
        suspendCSE();
        branchToLabel(LIR_jt, eqp0(current_osr), no_osr_label);
        callIns(FUNCTIONID(osr_adjust_frame), 4,
                methodFrame, haveDebugger ? csn : InsConstPtr(0), vars, tags);
        branchToAbcPos(LIR_j, NULL, osr->osrPc());
        emitLabel(no_osr_label);
        resumeCSE();
    }

    void CodegenLIR::emitInitializers()
    {
        struct JitInitVisitor: public InitVisitor {
            CodegenLIR *jit;
            JitInitVisitor(CodegenLIR *jit) : jit(jit) {}
            virtual ~JitInitVisitor() {}
            void defaultVal(Atom value, uint32_t slot, Traits* slotType) {
#ifdef NJ_VERBOSE
                if (jit->verbose()) {
                    jit->vbWriter->flush();
                    jit->core->console << "init [" << slot << "] = " << asAtom(value) << "\n";
                }
#endif
                LIns* defaultVal = jit->InsConstAtom(value);
                defaultVal = jit->atomToNativeRep(slotType, defaultVal);
                jit->emitSetslot(OP_setslot, slot, 0, defaultVal);
            }
        };
        JitInitVisitor visitor(this);
        Traits* t = info->declaringTraits();
        const TraitsBindings *tb = t->getTraitsBindings();
        t->visitInitBody(&visitor, toplevel, tb);
    }

    void CodegenLIR::emitDebugEnter()
    {
        #ifdef DEBUGGER
        for (int i = ms->scope_base(), n = ms->stack_base(); i < n; ++i)
            localSet(i, undefConst, VOID_TYPE);

        callIns(FUNCTIONID(debugEnter), 5,
            env_param,
            tags,
            csn,
            vars,
            driver->hasReachableExceptions() ? _save_eip : InsConstPtr(0));
        #endif // DEBUGGER
    }

    void CodegenLIR::copyParam(int i, int& offset) {
        LIns* apArg = ap_param;
        Traits* type = ms->paramTraits(i);
        LIns *arg;
        switch (bt(type)) {
#ifdef VMCFG_FLOAT
        case BUILTIN_float: 
            arg = loadIns(LIR_ldf, offset, apArg, ACCSET_OTHER, LOAD_CONST);
            offset += sizeof(Atom); // float is 4 bytes; but on x64, it takes up 8 bytes too, just like any int.
            break;
        case BUILTIN_float4: 
            // float4 is passed inline [i.e. not as a pointer to the value !! ]
            arg = loadIns(LIR_ldf4, offset, apArg, ACCSET_OTHER, LOAD_CONST);
            offset += sizeof(float4_t);
            break;
#endif // VMCFG_FLOAT
        case BUILTIN_number:
            arg = loadIns(LIR_ldd, offset, apArg, ACCSET_OTHER, LOAD_CONST);
            offset += sizeof(double);
            break;
        case BUILTIN_int:
        case BUILTIN_uint:
        case BUILTIN_boolean:
            // in the args these are widened to intptr_t or uintptr_t, so truncate here.
            arg = p2i(loadIns(LIR_ldp, offset, apArg, ACCSET_OTHER, LOAD_CONST));
            offset += sizeof(Atom);
            break;
        default:
            arg = loadIns(LIR_ldp, offset, apArg, ACCSET_OTHER, LOAD_CONST);
            offset += sizeof(Atom);
            break;
        }
        localSet(i, arg, type);
    }

    void CodegenLIR::emitCopy(int src, int dest) {
        localSet(dest, localCopy(src), state->value(src).traits);
    }

    void CodegenLIR::emitGetscope(int scope_index, int dest)
    {
        Traits* t = info->declaringScope()->getScopeTraitsAt(scope_index);
        LIns* scope = loadEnvScope();
        LIns* scopeobj = loadIns(LIR_ldp, offsetof(ScopeChain,_scopes) + scope_index*sizeof(Atom), scope, ACCSET_OTHER, LOAD_CONST);
        localSet(dest, atomToNativeRep(t, scopeobj), t);
    }

    void CodegenLIR::emitSwap(int i, int j) {
        LIns* t = localCopy(i);
        localSet(i, localCopy(j), state->value(j).traits);
        localSet(j, t, state->value(i).traits);
    }

    void CodegenLIR::emitKill(int i)
    {
        localSet(i, undefConst, NULL);
    }

#ifdef VMCFG_FLOAT
    void CodegenLIR::emitNumericOp2(int32_t op1, int32_t op2, const LIREmitter& emitIns)
    {
        static_assert(kIntptrType == 6, "You can't change kIntptrType just like that");
        static_assert(kDoubleType == 7, "You can't change kDoubleType just like that");
        
        const FrameValue& v1 = state->value(op1); (void) v1;
        const FrameValue& v2 = state->value(op2); (void) v2;
        AvmAssert (v1.traits == OBJECT_TYPE && v2.traits == OBJECT_TYPE && v1.notNull && v2.notNull);

        CodegenLabel slow_path("_numeric2_slow_path_");
        CodegenLabel both_double("_numeric2_both_double_");
        CodegenLabel done("_numeric2_done_");
        CodegenLabel int_op_number("_numeric2_int_op_number_");
        CodegenLabel v1_is_int("_numeric2_v1_is_int_");
        
        LIns* val1 = loadAtomRep(op1);
        LIns* val2 = loadAtomRep(op2);
        LIns* result = insAlloc(sizeof(intptr_t));
        LIns* tag = andp(  binaryIns(LIR_andp, val1, val2), AtomConstants::kAtomTypeMask);
        // kIntptrType
        suspendCSE();
        /* NOTE: to make the fast path faster, we're playing with the bits to quickly determine if 
           "both are double", "at least one is not Number" and "both are int"
           We rely on the Atom TAG scheme, i.e. "kIntPtrType is 6" and "kDoubleType is 7",
           see static asserts above.
        */
        branchToLabel(LIR_jt, eqp(tag, AtomConstants::kDoubleType), both_double);
        branchToLabel(LIR_jf, eqp(tag, AtomConstants::kIntptrType), slow_path);
        LIns* xormask = andp( binaryIns(LIR_xorp,val1,val2), AtomConstants::kAtomTypeMask );

        branchToLabel(LIR_jf, eqp0(xormask), int_op_number);
    // both_integers:
        stp( nativeToAtom(
                    emitIns(BUILTIN_number, p2dIns(rshp(val1, AtomConstants::kAtomTypeSize)) 
                                          , p2dIns(rshp(val2, AtomConstants::kAtomTypeSize)) ),
                    NUMBER_TYPE ),
             result, 0, ACCSET_OTHER);
        JIT_EVENT(jit_numeric2_fast_int);
        branchToLabel(LIR_j, NULL, done);

    // int_op_number:
        emitLabel(int_op_number);
        tag = andp (val1, AtomConstants::kAtomTypeMask); 
        branchToLabel(LIR_jt, eqp(tag, AtomConstants::kIntptrType),v1_is_int );
    // v2_is_int: (v1 is number!)
        stp( nativeToAtom(
                emitIns(BUILTIN_number, ldd(subp(val1, AtomConstants::kDoubleType), 0, ACCSET_OTHER)
                                      , p2dIns(rshp(val2, AtomConstants::kAtomTypeSize)) ),
                NUMBER_TYPE ),
            result, 0, ACCSET_OTHER);
        JIT_EVENT(jit_numeric2_dbl_int);
        branchToLabel(LIR_j, NULL, done);
    // v1_is_int: (v2 is number!)
        emitLabel(v1_is_int);
        stp( nativeToAtom(
                    emitIns(BUILTIN_number, p2dIns(rshp(val1, AtomConstants::kAtomTypeSize))
                                          , ldd(subp(val2, AtomConstants::kDoubleType), 0, ACCSET_OTHER) ),
                NUMBER_TYPE ),
            result, 0, ACCSET_OTHER);
        JIT_EVENT(jit_numeric2_dbl_int);
        branchToLabel(LIR_j, NULL, done);
        
    // slow_path:        
        emitLabel(slow_path);
        stp( emitIns(BUILTIN_any,  val1, val2) , result, 0, ACCSET_OTHER);
        JIT_EVENT(jit_numeric2_slow);
        branchToLabel(LIR_j, NULL, done);

    // both_double:
        emitLabel(both_double);
        stp( nativeToAtom( 
                        emitIns(BUILTIN_number,  ldd(subp(val1, AtomConstants::kDoubleType), 0, ACCSET_OTHER)
                                              ,  ldd(subp(val2, AtomConstants::kDoubleType), 0, ACCSET_OTHER))
                        , NUMBER_TYPE),
             result, 0, ACCSET_OTHER);
        JIT_EVENT(jit_numeric2_fast_double);

   // done:
        emitLabel(done);
        resumeCSE();
        localSet( op1, ldp(result, 0, ACCSET_OTHER), OBJECT_TYPE);
    }

    void CodegenLIR::emitNumericOp1(int32_t op1, const LIREmitter &emitIns)
    {
        const FrameValue& v = state->value(op1); (void)v;
        AvmAssert(v.traits == OBJECT_TYPE && v.notNull);

        CodegenLabel not_intptr("_not_intptr_");
        CodegenLabel not_double("_not_double_");
        CodegenLabel done("_done_");
        LIns* val = loadAtomRep(op1); // load Atom.
        LIns* result = insAlloc(sizeof(intptr_t));
        LIns* tag = andp(val, AtomConstants::kAtomTypeMask);
        // kIntptrType
        suspendCSE();
        branchToLabel(LIR_jf, eqp(tag, AtomConstants::kIntptrType), not_intptr);
        stp( nativeToAtom(
            emitIns(BUILTIN_number, p2dIns(rshp(val, AtomConstants::kAtomTypeSize)) )
            , NUMBER_TYPE
            ) , result, 0, ACCSET_OTHER);

        JIT_EVENT(jit_numeric1_fast_int);
        branchToLabel(LIR_j, NULL, done);
        emitLabel(not_intptr);
        // kDoubleType
        branchToLabel(LIR_jf, eqp(tag, AtomConstants::kDoubleType), not_double);
        stp( nativeToAtom( 
            emitIns(BUILTIN_number,  ldd(val, -AtomConstants::kDoubleType, ACCSET_OTHER) )
            , NUMBER_TYPE
            ) , result, 0, ACCSET_OTHER);
        JIT_EVENT(jit_numeric1_fast_number);

        branchToLabel(LIR_j, NULL, done);
        emitLabel(not_double);

        stp( emitIns(BUILTIN_any,  val) , result, 0, ACCSET_OTHER);
        JIT_EVENT(jit_numeric1_slow);
        emitLabel(done);
        resumeCSE();
        localSet( op1, ldp(result, 0, ACCSET_OTHER), OBJECT_TYPE);
    }
#endif // VMCFG_FLOAT

#ifdef VMCFG_FASTPATH_ADD

#ifdef VMCFG_FASTPATH_ADD_INLINE

    // Emit code for the fastpath for int + intptr => intptr addition.
    // Branch to fallback label if rhs is not intptr, or result cannot be so represented due to overflow.
    //
    // 64-bit:
    //
    // FAST_ADD_INT_TO_ATOM(lhs, rhs, result, fallback)
    //    if ((rhs & kAtomTypeMask) != kIntptrType) goto fallback;   # punt if atom argument rhs is not intptr
    //    intptr_t lhsExtended = i;                                  # extend int argument lhs to atom size
    //    intptr_t lhsShifted = lhsExtended << (kAtomTypeSize+7);    # align lhs with 54-bit payload of rhs (see next step)
    //    intptr_t rhsShifted = rhs << 7;                            # left-justify 54-bit payload of rhs (note rhs is tagged)
    //    intptr_t sumShifted = lhsShifted + rhsShifted;             # add aligned values, producing 57-bit tagged sum, left-justified
    //    if (OVERFLOW) goto fallback;                               # punt on overflow, as sum will not fit in 57-bit field allotted
    //    intptr_t sum = sumShifted >> 7;                            # right-justify 57-bit tagged value (54 payload bits + 3 tag bits)
    //    result = sum;                                              # result is intptr atom, properly tagged
    //
    // 32-bit:
    //
    // FAST_ADD_INT_TO_ATOM(lhs, rhs, result, fallback)
    //    if ((rhs & kAtomTypeMask) != kIntptrType) goto fallback;   # punt if atom argument rhs is not intptr
    //    intptr_t lhsExtended = i;                                  # extend int argument lhs to atom size (a nop on 32-bit platforms)
    //    intptr_t lhsShifted = lhsExtended << kAtomTypeSize;        # left-justify 29-bit payload
    //    intptr_t lhsRestored = lhsShifted >> kAtomTypeSize;        # restore
    //    if (lhsRestored != lhsExtended) goto fallback;             # high-order bits were lost, value will not fit in 29-bit field allotted
    //    intptr_t sum = lhsShifted + rhs;                           # add left-justified lhs to tagged rhs, resulting in tagged result
    //    if (OVERFLOW) goto fallback;                               # punt on overflow, as sum will not fit in 29-bit field allotted
    //    result = sum;                                              # result is intptr atom, properly tagged
    //
    // Note: If lhs will not fit in 29 bits, the fastpath will fail even if the result may fit.  We expect this to be an
    // uncommon case.  Generating code as we do avoids extra tag manipulations that would be required if we did a full
    // 32-bit addition followed by a range check.

    void CodegenLIR::emitIntPlusAtomFastpath(int i, Traits* type, LIns* lhs, LIns* rhs, CodegenLabel &fallback)
    {
        LIns* tag = andp(rhs, AtomConstants::kAtomTypeMask);
        branchToLabel(LIR_jf, eqp(tag, AtomConstants::kIntptrType), fallback);
        LIns* lhsExtended = i2p(lhs);
        #ifdef AVMPLUS_64BIT
            // int argument is guaranteed to fit, but must restrict intptr result to 54 bit range
            // TODO: Consider maintaining 54-bit intptrs in pre-shifted form.
            LIns* lhsShifted = lshp(lhsExtended, atomSignExtendShift);
            LIns* rhsShifted = lshp(rhs, atomSignExtendShift-AtomConstants::kAtomTypeSize);
            LIns* sumShifted = branchJovToLabel(LIR_addjovp, lhsShifted, rhsShifted, fallback);
            LIns* sum = rshp(sumShifted, atomSignExtendShift-AtomConstants::kAtomTypeSize);
        #else
            // verify that int value will fit in intptr
            LIns* lhsShifted = lshp(lhsExtended, AtomConstants::kAtomTypeSize);
            LIns* lhsRestored = rshp(lhsShifted, AtomConstants::kAtomTypeSize);
            branchToLabel(LIR_jf, binaryIns(LIR_eqp, lhsRestored, lhsExtended), fallback);
            LIns* sum = branchJovToLabel(LIR_addjovp, lhsShifted, rhs, fallback);
        #endif
        localSet(i, sum, type);
    }
#endif /* VMCFG_FASTPATH_ADD_INLINE */

    // Emit code for int + atom => atom addition.
    // The usual helper function call may be bypassed with a fastpath for the int + intptr => intptr case:
    //
    //     intptr_t rhsa = CONVERT_TO_ATOM(rhs);                    # box atom if needed
    //     FAST_ADD_INT_TO_ATOM(lhs, rhsa, result, fallback);       # handle the fastpath, branching to fallback label on failure
    //     goto done;                                               # fastpath succeeded
    // fallback:
    //     result = op_add_a_ia(coreAddr, lhs, rhsa);               # fastpath failed, fall back to helper function
    // done:

    void CodegenLIR::emitAddIntToAtom(int i, int j, Traits* type)
    {
        LIns* rhs = loadAtomRep(j);
        LIns* lhs = localGet(i);
        FLOAT_ONLY(const bool floatSupport = pool->hasFloatSupport();)
        const CallInfo* addFunction = IFFLOAT(floatSupport ? FUNCTIONID(op_add_a_ia) : FUNCTIONID(op_add_a_ia_nofloat), FUNCTIONID(op_add_a_ia));

        #ifdef VMCFG_FASTPATH_ADD_INLINE
        if (inlineFastpath) {
            CodegenLabel fallback("fallback");
            CodegenLabel done("done");
            suspendCSE();
            emitIntPlusAtomFastpath(i, type, lhs, rhs, fallback);
            JIT_EVENT(jit_add_a_ia_fast_intptr);
            branchToLabel(LIR_j, NULL, done);
            emitLabel(fallback);
            LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
            localSet(i, out, type);
            JIT_EVENT(jit_add_a_ia_slow);
            emitLabel(done);
            resumeCSE();
            return;
        }
        #endif

        LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
        localSet(i, out, type);
        JIT_EVENT(jit_add_a_ia);
    }

    // Emit code for double + atom => atom addition.

    void CodegenLIR::emitAddDoubleToAtom(int i, int j, Traits* type)
    {
        LIns* rhs = loadAtomRep(j);
        LIns* lhs = localGetd(i);
#ifdef VMCFG_FLOAT
        const bool floatSupport = pool->hasFloatSupport();
        const CallInfo* addFunction = floatSupport ? FUNCTIONID(op_add_a_da) : FUNCTIONID(op_add_a_da_nofloat);
#else 
        const CallInfo* addFunction = FUNCTIONID(op_add_a_da);
#endif // VMCFG_FLOAT
        LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
        localSet(i, out, type);
        JIT_EVENT(jit_add_a_da);
    }

    // Emit code for atom + int => atom addition.
    // The usual helper function call may be bypassed with a fastpath for the intptr + int => intptr case:
    //
    //     intptr_t lhsa = CONVERT_TO_ATOM(lhs);                    # box atom if needed
    //     FAST_ADD_INT_TO_ATOM(rhs, lhsa, result, fallback);       # handle the fastpath, note it is OK to commute arguments here
    //     goto done;                                               # fastpath succeeded
    // fallback:
    //     result = op_add_a_ai(coreAddr, lhsa, rhs);               # fastpath failed, fall back to helper function
    // done:

    void CodegenLIR::emitAddAtomToInt(int i, int j, Traits* type)
    {
        LIns* lhs = loadAtomRep(i);
        LIns* rhs = localGet(j);
#ifdef VMCFG_FLOAT
        const bool floatSupport = pool->hasFloatSupport();
        const CallInfo* addFunction = floatSupport ? FUNCTIONID(op_add_a_ai) : FUNCTIONID(op_add_a_ai_nofloat);
#else
        const CallInfo* addFunction = FUNCTIONID(op_add_a_ai);
#endif // VMCFG_FLOAT

        #ifdef VMCFG_FASTPATH_ADD_INLINE
        if (inlineFastpath) {
            CodegenLabel fallback("fallback");
            CodegenLabel done("done");
            suspendCSE();
            emitIntPlusAtomFastpath(i, type, rhs, lhs, fallback);
            JIT_EVENT(jit_add_a_ai_fast_intptr);
            branchToLabel(LIR_j, NULL, done);
            emitLabel(fallback);
            LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
            localSet(i, out, type);
            JIT_EVENT(jit_add_a_ai_slow);
            emitLabel(done);
            resumeCSE();
            return;
        }
        #endif

        LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
        localSet(i, out, type);
        JIT_EVENT(jit_add_a_ai);
    }

    // Emit code for atom + double => atom addition.

    void CodegenLIR::emitAddAtomToDouble(int i, int j, Traits* type)
    {
        LIns* lhs = loadAtomRep(i);
        LIns* rhs = localGetd(j);
#ifdef VMCFG_FLOAT
        const bool floatSupport = pool->hasFloatSupport();
        const CallInfo* addFunction = floatSupport ? FUNCTIONID(op_add_a_ad) : FUNCTIONID(op_add_a_ad_nofloat);
#else
        const CallInfo* addFunction = FUNCTIONID(op_add_a_ad);
#endif // VMCFG_FLOAT

        LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
        localSet(i, out, type);
        JIT_EVENT(jit_add_a_ad);
    }

    // Emit code for atom + atom => atom addition.
    //
    // We implement a fastpath for the intptr + intptr => intptr case:
    //
    // 64-bit:
    //
    //     intptr_t lhsa = CONVERT_TO_ATOM(lhs);                    # box atom if needed
    //     intptr_t rhsa = CONVERT_TO_ATOM(rhs);                    # box atom if needed
    //     if (((lhsa ^ kIntptrType) | (rhsa ^ kIntptrType)) & kAtomTypeMask) goto fallback;
    //     # both arguments are intptr atoms
    //     intptr_t lhsStripped = lhsa - kIntptrType;               # zero out tag bits on lhs
    //     intptr_t lhsShifted = lhsStripped << 7;                  # left-justify 54-bit payload (followed by 0s in tag position)
    //     intptr_t rhsShifted = rhsa << 7;                         # align rhs payload and tag with left-justified lhs
    //     intptr_t sumShifted = lhsShifted + rhsShifted;           # add aligned values, producing 54-bit left-justified sum followed by tag
    //     if (OVERFLOW) goto fallback;
    //     result = sumShifted >> 7;                                # right-justify tagged sum (54-bit payload + 3-bit tag)
    //     goto done;
    // fallback:
    //     result = op_add_a_aa(coreAddr, lhsa, rhsa);              # handle the general case out-of-line
    // done:
    //
    // 32-bit:
    //
    //     intptr_t lhsa = CONVERT_TO_ATOM(lhs);                    # box atom if needed
    //     intptr_t rhsa = CONVERT_TO_ATOM(rhs);                    # box atom if needed
    //     if (((lhsa ^ kIntptrType) | (rhsa ^ kIntptrType)) & kAtomTypeMask) goto fallback;
    //     # both arguments are intptr atoms
    //     intptr_t lhsStripped = lhsa - kIntptrType;               # zero out tag bits on lhs (note rhs retains its tag)
    //     intptr_t sum = lhsStripped + rhs;                        # add, producing 29-bit sum followed by 3-bit tag
    //     if (OVERFLOW) goto fallback;
    //     result = sum;
    //     goto done;
    // fallback:
    //     result = op_add_a_aa(coreAddr, lhsa, rhsa);              # handle the general case out-of-line
    // done:

    void CodegenLIR::emitAddAtomToAtom(int i, int j, Traits* type)
    {
        LIns* lhs = loadAtomRep(i);
        LIns* rhs = loadAtomRep(j);
#ifdef VMCFG_FLOAT
        const bool floatSupport = pool->hasFloatSupport();
        const CallInfo* addFunction = floatSupport ? FUNCTIONID(op_add_a_aa) : FUNCTIONID(op_add_a_aa_nofloat);
#else
        const CallInfo* addFunction = FUNCTIONID(op_add_a_aa);
#endif // VMCFG_FLOAT

        #ifdef VMCFG_FASTPATH_ADD_INLINE
        if (inlineFastpath) {
            CodegenLabel fallback("fallback");
            CodegenLabel done("done");
            // intptr + intptr fastpath
            suspendCSE();
            LIns* t0 = xorp(lhs, AtomConstants::kIntptrType);
            LIns* t1 = xorp(rhs, AtomConstants::kIntptrType);
            LIns* t2 = binaryIns(LIR_orp, t0, t1);
            LIns* t3 = andp(t2, AtomConstants::kAtomTypeMask);
            branchToLabel(LIR_jf, eqp0(t3), fallback);
            LIns* lhsStripped = subp(lhs, AtomConstants::kIntptrType);
            #ifdef AVMPLUS_64BIT
                // restrict range of intptr result to 54 bits
                // since 64-bit int atoms expect exactly 54 bits of precision, shift bit 54+3 up into the sign bit
                LIns* lhsShifted = lshp(lhsStripped, atomSignExtendShift-AtomConstants::kAtomTypeSize);
                LIns* rhsShifted = lshp(rhs, atomSignExtendShift-AtomConstants::kAtomTypeSize);
                LIns* sumShifted = branchJovToLabel(LIR_addjovp, lhsShifted, rhsShifted, fallback);
                LIns* sum = rshp(sumShifted, atomSignExtendShift-AtomConstants::kAtomTypeSize);
            #else
                LIns* sum = branchJovToLabel(LIR_addjovp, lhsStripped, rhs, fallback);
            #endif
            localSet(i, sum, type);
            JIT_EVENT(jit_add_a_aa_fast_intptr);
            branchToLabel(LIR_j, NULL, done);
            emitLabel(fallback);
            LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
            localSet(i, out, type);
            JIT_EVENT(jit_add_a_aa_slow);
            emitLabel(done);
            resumeCSE();
            return;
        }
        #endif

        LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
        localSet(i, atomToNativeRep(type, out), type);
        JIT_EVENT(jit_add_a_aa);
    }

#else /* VMCFG_FASTPATH_ADD */

    void CodegenLIR::emitAddAtomToAtom(int i, int j, Traits* type)
    {
        LIns* lhs = loadAtomRep(i);
        LIns* rhs = loadAtomRep(j);
        
#ifdef VMCFG_FLOAT
        const bool floatSupport = pool->hasFloatSupport();
        const CallInfo* addFunction = floatSupport ? FUNCTIONID(op_add) : FUNCTIONID(op_add_nofloat);
#else
        const CallInfo* addFunction = FUNCTIONID(op_add);
#endif // VMCFG_FLOAT
        LIns* out = callIns(addFunction, 3, coreAddr, lhs, rhs);
        localSet(i, atomToNativeRep(type, out), type);
        JIT_EVENT(jit_add);
    }

#endif /* VMCFG_FASTPATH_ADD */

    void CodegenLIR::emitAdd(int i, int j, Traits* type)
    {
        const FrameValue& val1 = state->value(i);
        const FrameValue& val2 = state->value(j);
        if ((val1.traits == STRING_TYPE && val1.notNull) || (val2.traits == STRING_TYPE && val2.notNull)) {
            // string concatenation
            AvmAssert(type == STRING_TYPE);
            LIns* lhs = convertToString(i, true);
            LIns* rhs = convertToString(j, true);
            LIns* out = callIns(FUNCTIONID(concatStrings), 3, coreAddr, lhs, rhs);
            localSet(i,  out, type);
            JIT_EVENT(jit_add_a_ss);
        } else if (val1.traits && val2.traits && val1.traits->isNumeric() && val2.traits->isNumeric()) {
            // numeric + numeric
            // TODO: The tests for isNumeric() above could be isNumericOrBool(),
            // but a corresponding change would be needed in the verifier, resulting
            // in a slight change to the verifier's type inference algorithm.
            AvmAssert(type == NUMBER_TYPE FLOAT_ONLY(|| type==FLOAT_TYPE || type== FLOAT4_TYPE) );
#ifdef VMCFG_FLOAT
            if(!pool->hasFloatSupport()) 
                type = NUMBER_TYPE; 
            else if(val1.traits== FLOAT4_TYPE || val2.traits == FLOAT4_TYPE) 
                type = FLOAT4_TYPE;

            if (type == FLOAT4_TYPE) {
                LIns* lhs = coerceToFloat4(i);
                LIns* rhs = coerceToFloat4(j);
                localSet(i, binaryIns(LIR_addf4, lhs, rhs), type);
                JIT_EVENT(jit_add_a_f4f4);
            } else if(type==FLOAT_TYPE){
                AvmAssert(val1.traits == FLOAT_TYPE && val2.traits == FLOAT_TYPE);
                LIns* lhs = coerceToFloat(i);
                LIns* rhs = coerceToFloat(j);
                localSet(i, binaryIns(LIR_addf, lhs, rhs), type);
                JIT_EVENT(jit_add_a_ff);
            } else 
#endif // VMCFG_FLOAT
            {
                LIns* lhs = coerceToNumber(i);
                LIns* rhs = coerceToNumber(j);
                localSet(i, binaryIns(LIR_addd, lhs, rhs), type);
                JIT_EVENT(jit_add_a_nn);
            }
#ifdef VMCFG_FASTPATH_ADD
        // If we arrive here, at least one argument is not known to be of a numeric type.
        // Thus, having determined one argument to be of a known numeric type, we will coerce
        // the other to an atom.  We speculate that the other argument will already be an atom
        // of type kIntptrType or kDoubleType, checking for these cases first.
        } else if (val1.traits == INT_TYPE) {
            // integer + atom
            AvmAssert(type == OBJECT_TYPE);
            emitAddIntToAtom(i, j, type);
        } else if (val1.traits == NUMBER_TYPE) {
            // double + atom
            AvmAssert(type == OBJECT_TYPE);
            emitAddDoubleToAtom(i, j, type);
        } else if (val2.traits == INT_TYPE) {
            // atom + integer
            AvmAssert(type == OBJECT_TYPE);
            emitAddAtomToInt(i, j, type);
        } else if (val2.traits == NUMBER_TYPE) {
            // atom + double
            AvmAssert(type == OBJECT_TYPE);
            emitAddAtomToDouble(i, j, type);
#endif
        } else {
            // Neither argument is known to be of a numeric type, so coerce both to atoms.
            AvmAssert(type == OBJECT_TYPE);
            emitAddAtomToAtom(i, j, type);
        }
    }

    void CodegenLIR::writeBlockStart(const FrameState* state)
    {
        this->state = state;
        // get the saved label for our block start and tie it to this location
        CodegenLabel& label = getCodegenLabel(state->abc_pc);
        emitLabel(label);
        emitSetPc(state->abc_pc);

#ifdef DEBUG
        memset(jit_sst, 0, framesize * sizeof(uint16_t));
#endif

        // If this is the target of a backwards branch, generate an interrupt check.

#ifdef VMCFG_INTERRUPT_SAFEPOINT_POLL
        // Always poll for safepoints, regardless of config settings.
		if (state->targetOfBackwardsBranch) {
            Ins(LIR_savepc);
            LIns* interrupted = loadIns(LIR_ldi, offsetof(AvmCore,interrupted), coreAddr, ACCSET_OTHER, LOAD_VOLATILE);
            LIns* cond = binaryIns(LIR_eqi, interrupted, InsConst(AvmCore::NotInterrupted));
            branchToLabel(LIR_jf, cond, interrupt_label);

          /*
            CodegenLabel not_interrupt_label;
            branchToLabel(LIR_jt, cond, not_interrupt_label);
            branchToLabel(LIR_j, NULL, interrupt_label);
            emitLabel(not_interrupt_label);
          */
            Ins(LIR_discardpc);
        }
#else 
        if (interruptable && core->config.interrupts && state->targetOfBackwardsBranch) {
            LIns* interrupted = loadIns(LIR_ldi, offsetof(AvmCore,interrupted),
                    coreAddr, ACCSET_OTHER, LOAD_VOLATILE);
            LIns* cond = binaryIns(LIR_eqi, interrupted, InsConst(AvmCore::NotInterrupted));
            branchToLabel(LIR_jf, cond, interrupt_label);
        }
#endif            
    }

    void CodegenLIR::writeOpcodeVerified(const FrameState* state, const uint8_t*, AbcOpcode)
    {
        verbose_only( if (vbWriter) { vbWriter->flush();} )
#ifdef DEBUG
        this->state = NULL; // prevent access to stale state
        int scopeTop = ms->scope_base() + state->scopeDepth;
        for (int i=0, n=state->sp()+1; i < n; i++) {
            if (i >= scopeTop && i < ms->stack_base())
                continue;
            const FrameValue& v = state->value(i);
            AvmAssert(!jit_sst[i] || jit_sst[i] == v.sst_mask);
        }
#else
        (void)state;
#endif
    }

    // this is a no-op for the JIT because we do all label patching in emitLabel().
    void CodegenLIR::writeFixExceptionsAndLabels(const FrameState*, const uint8_t*)
    {}

    void CodegenLIR::write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type)
    {
      //AvmLog("CodegenLIR::write %x\n", opcode);
        this->state = state;
        emitSetPc(pc);
        const uint8_t* nextpc = pc;
        unsigned int imm30=0, imm30b=0;
        int imm8=0, imm24=0;
        AvmCore::readOperands(nextpc, imm30, imm24, imm30b, imm8);
        int sp = state->sp();

        switch (opcode) {
        case OP_nop:
        case OP_pop:
        case OP_label:
            // do nothing
            break;
        case OP_getlocal0:
        case OP_getlocal1:
        case OP_getlocal2:
        case OP_getlocal3:
            imm30 = opcode-OP_getlocal0;
            // hack imm30 and fall through
        case OP_getlocal:
            emitCopy(imm30, sp+1);
            break;
        case OP_setlocal0:
        case OP_setlocal1:
        case OP_setlocal2:
        case OP_setlocal3:
            imm30 = opcode-OP_setlocal0;
            // hack imm30 and fall through
        case OP_setlocal:
            emitCopy(sp, imm30);
            break;
        case OP_pushtrue:
            AvmAssert(type == BOOLEAN_TYPE);
            emitIntConst(sp+1, 1, type);
            break;
        case OP_pushfalse:
            AvmAssert(type == BOOLEAN_TYPE);
            emitIntConst(sp+1, 0, type);
            break;
        case OP_pushnull:
            AvmAssert(type == NULL_TYPE);
            emitPtrConst(sp+1, 0, type);
            break;
        case OP_pushundefined:
            AvmAssert(type == VOID_TYPE);
            emitPtrConst(sp+1, (void*)undefinedAtom, type);
            break;
        case OP_pushshort:
            AvmAssert(type == INT_TYPE);
            emitIntConst(sp+1, (signed short)imm30, type);
            break;
        case OP_pushbyte:
            AvmAssert(type == INT_TYPE);
            emitIntConst(sp+1, (signed char)imm8, type);
            break;
        case OP_pushstring:
            AvmAssert(type == STRING_TYPE);
            emitPtrConst(sp+1, pool->getString(imm30), type);
            break;
        case OP_pushnamespace:
            AvmAssert(type == NAMESPACE_TYPE);
            emitPtrConst(sp+1, pool->cpool_ns[imm30], type);
            break;
        case OP_pushint:
            AvmAssert(type == INT_TYPE);
            emitIntConst(sp+1, pool->cpool_int[imm30], type);
            break;
        case OP_pushuint:
            AvmAssert(type == UINT_TYPE);
            emitIntConst(sp+1, pool->cpool_uint[imm30], type);
            break;
        case OP_pushdouble:
            AvmAssert(type == NUMBER_TYPE);
            emitDoubleConst(sp+1, &pool->cpool_double[imm30]->value);
            break;
#ifdef VMCFG_FLOAT
        case OP_pushfloat:
            AvmAssert(type == FLOAT_TYPE);
            emitFloatConst(sp+1, pool->cpool_float[imm30]->value);
            break;
        case OP_pushfloat4:
            AvmAssert(type == FLOAT4_TYPE);
            emitFloat4Const(sp+1, (const float4_t*) pool->cpool_float4[imm30] );
            break;
#endif
        case OP_pushnan:
            AvmAssert(type == NUMBER_TYPE);
            emitDoubleConst(sp+1, (double*)atomPtr(core->kNaN));
            break;
        case OP_lookupswitch:
            emit(opcode, uintptr_t(pc + imm24), imm30b /*count*/);
            break;
        case OP_throw:
        case OP_returnvalue:
        case OP_returnvoid:
            emit(opcode, sp);
            break;
        case OP_debugfile: {
            if (haveDebugger || haveVTune || jit_observer) {
              String* filename = pool->getString(imm30); (void)filename;
#ifdef DEBUGGER
              if (haveDebugger) {
                  LIns* debugger = loadIns(LIR_ldp, offsetof(AvmCore, _debugger),
                                           coreAddr, ACCSET_OTHER, LOAD_CONST);
                  callIns(FUNCTIONID(debugFile), 2,
                          debugger,
                          InsConstPtr(filename));
              }
#endif // DEBUGGER
#ifdef VMCFG_VTUNE
              Ins(LIR_file, InsConstPtr(filename));
#endif /* VMCFG_VTUNE */
              if (jit_observer) {
                  JITDebugInfo* jdi = initJitDebugInfo();
                  JITDebugInfo::Info* info = new (*lir_alloc) JITDebugInfo::Info(JITDebugInfo::kFile);
                  info->file = imm30;
                  jdi->add(info);
                  Ins(LIR_pc, InsConstPtr(&info->pc));
              }
            }
            break;
        }
        case OP_debugline: {
            if (haveDebugger || haveVTune || jit_observer) {
                // we actually do generate code for these, in debugger mode
#ifdef DEBUGGER
                if (haveDebugger) {
                    LIns* debugger = loadIns(LIR_ldp, offsetof(AvmCore, _debugger),
                                             coreAddr, ACCSET_OTHER, LOAD_CONST);
                    callIns(FUNCTIONID(debugLine), 2, debugger, InsConst(imm30));
                }
#endif // DEBUGGER
#ifdef VMCFG_VTUNE
                Ins(LIR_line, InsConst(imm30));
#endif /* VMCFG_VTUNE */
                if (jit_observer) {
                    JITDebugInfo* jdi = initJitDebugInfo();
                    JITDebugInfo::Info* info = new (*lir_alloc) JITDebugInfo::Info(JITDebugInfo::kLine);
                    info->line = imm30; // filename pool index
                    jdi->add(info);
                    Ins(LIR_pc, InsConstPtr(&info->pc));
                }
            }
            break;
        }
        case OP_dxns:
        {
            Stringp str = pool->getString(imm30);  // assume been checked already
            emit(opcode, (uintptr_t)str);
            break;
        }
        case OP_dxnslate:
            // codgen will call intern on the input atom.
            emit(opcode, sp);
            break;
        case OP_kill:
            emitKill(imm30);
            break;
        case OP_inclocal:
        case OP_declocal:
            AvmAssert(type==NUMBER_TYPE FLOAT_ONLY(|| type == FLOAT_TYPE|| type == FLOAT4_TYPE|| type == OBJECT_TYPE) );
            emit(opcode, imm30, opcode==OP_inclocal ? 1 : -1, type);
            break;
        case OP_inclocal_i:
        case OP_declocal_i:
            emit(opcode, imm30, opcode==OP_inclocal_i ? 1 : -1, INT_TYPE);
            break;
        case OP_lessthan:
        case OP_greaterthan:
        case OP_lessequals:
        case OP_greaterequals:
            emit(opcode, 0, 0, BOOLEAN_TYPE);
            break;

        case OP_getdescendants:
        {
            const Multiname *name = pool->precomputedMultiname(imm30);
            emit(opcode, (uintptr_t)name, 0, NULL);
            break;
        }

        case OP_checkfilter:
            emit(opcode, sp, 0, NULL);
            break;

        case OP_deleteproperty:
        {
            const Multiname *name = pool->precomputedMultiname(imm30);
            emit(opcode, (uintptr_t)name, 0, BOOLEAN_TYPE);
            break;
        }

        case OP_astype:
        {
            emit(OP_astype, (uintptr_t)type, sp, type && type->isMachineType() ? OBJECT_TYPE : type);
            break;
        }
        case OP_astypelate:
        {
            emit(OP_astypelate, 0, 0, type);
            break;
        }

#ifdef VMCFG_FLOAT
        case OP_unplus:
            AvmAssert(type && (type == OBJECT_TYPE || type->isNumeric()));
            /* Don't do anything; the coercion already happened */
            break;
#endif
        case OP_coerce:
        case OP_coerce_b:
        case OP_convert_b:
        case OP_coerce_o:
        case OP_coerce_a:
        case OP_convert_i:
        case OP_coerce_i:
        case OP_convert_u:
        case OP_coerce_u:
        case OP_convert_d:
        case OP_coerce_d:
        case OP_coerce_s:
#ifdef VMCFG_FLOAT
        case OP_convert_f:
        case OP_convert_f4:
#endif

            AvmAssert( FLOAT_ONLY(
                    (opcode == OP_convert_f && type == FLOAT_TYPE)   ||  
                    (opcode == OP_convert_f4 && type == FLOAT4_TYPE) ||)
                    (opcode == OP_coerce    && type != NULL)         ||
                    (opcode == OP_coerce_b  && type == BOOLEAN_TYPE) ||
                    (opcode == OP_convert_b && type == BOOLEAN_TYPE) ||
                    (opcode == OP_coerce_o  && type == OBJECT_TYPE)  ||
                    (opcode == OP_coerce_a  && type == NULL)         ||
                    (opcode == OP_convert_i && type == INT_TYPE)     ||
                    (opcode == OP_coerce_i  && type == INT_TYPE)     ||
                    (opcode == OP_convert_u && type == UINT_TYPE)    ||
                    (opcode == OP_coerce_u  && type == UINT_TYPE)    ||
                    (opcode == OP_convert_d && type == NUMBER_TYPE)  ||
                    (opcode == OP_coerce_d  && type == NUMBER_TYPE)  ||
                    (opcode == OP_coerce_s  && type == STRING_TYPE)  );
            emitCoerce(sp, type);
            break;

        case OP_istype:
        {
            // used when operator "is" RHS is a compile-time type constant
            //sp[0] = istype(sp[0], itraits);
            LIns* obj = loadAtomRep(sp);
            LIns* out = callIns(FUNCTIONID(istype), 2, obj, InsConstPtr(type));
            localSet(sp, out, BOOLEAN_TYPE);
            break;
        }

        case OP_istypelate:
        {
            // null check for the type value T in (x is T).  This also preserves
            // any side effects from loading T, even if we end up inlining T.itraits() as a const.
            Traits* class_type = state->value(sp).traits;
            emitCheckNull(localCopy(sp), class_type);
            LIns* obj = loadAtomRep(sp-1);
            LIns* istype_result;
            if (class_type && class_type->base == CLASS_TYPE) {
                // (x is T) where T is a class object: get T.itraits as constant.
                istype_result = callIns(FUNCTIONID(istype), 2, obj, InsConstPtr(class_type->itraits));
            } else {
                // RHS is unknown, call general istype
                istype_result = callIns(FUNCTIONID(istypelate), 3, env_param, obj, loadAtomRep(sp));
            }
            localSet(sp-1, istype_result, BOOLEAN_TYPE);
            break;
        }

        case OP_convert_o:
            // NOTE check null has already been done
            break;

        case OP_applytype:
            // * is ok for the type, as Vector classes have no statics
            // when we implement type parameters fully, we should do something here.
            emit(opcode, imm30/*argc*/, 0, NULL);
            break;

        case OP_newobject:
            emit(opcode, imm30, 0, OBJECT_TYPE);
            break;

        case OP_newarray:
            emit(opcode, imm30, 0, ARRAY_TYPE);
            break;

        case OP_newactivation:
            emit(opcode, 0, 0, info->activationTraits());
            break;

        case OP_newcatch:
        {
            ExceptionHandler* handler = &info->abc_exceptions()->exceptions[imm30];
            emit(opcode, 0, 0, handler->scopeTraits);
            break;
        }

        case OP_popscope:
            if (haveDebugger)
                emitKill(ms->local_count()/*scopeBase*/ + state->scopeDepth);
            break;

        case OP_getslot:
        {
            const FrameValue& obj = state->peek(1);
            int index = imm30-1;
            Traits* slotTraits = obj.traits ? obj.traits->getTraitsBindings()->getSlotTraits(index) : NULL;
            emitGetslot(index, sp, slotTraits);
            break;
        }

        case OP_setslot:
            emitSetslot(OP_setslot, imm30-1, sp-1);
            break;

        case OP_dup:
            emitCopy(sp, sp+1);
            break;

        case OP_swap:
            emitSwap(sp, sp-1);
            break;

        case OP_add:
            emitAdd(sp-1, sp, type);
            break;

        case OP_equals:
        case OP_strictequals:
        case OP_instanceof:
        case OP_in:
            emit(opcode, 0, 0, BOOLEAN_TYPE);
            break;

        case OP_not:
            AvmAssert(type == BOOLEAN_TYPE);
            emit(opcode, sp, 0, type);
            break;

        case OP_modulo:
        case OP_subtract:
        case OP_divide:
        case OP_multiply:
            AvmAssert(type==NUMBER_TYPE FLOAT_ONLY(|| type == OBJECT_TYPE || type == FLOAT_TYPE || type == FLOAT4_TYPE));
            emit(opcode, 0, 0, type);
            break;

        case OP_increment:
        case OP_decrement:
            AvmAssert(type==NUMBER_TYPE FLOAT_ONLY(|| type == OBJECT_TYPE || type == FLOAT_TYPE || type == FLOAT4_TYPE));
            emit(opcode, sp, opcode == OP_increment ? 1 : -1, type);
            break;

        case OP_increment_i:
        case OP_decrement_i:
            emit(opcode, sp, opcode == OP_increment_i ? 1 : -1, INT_TYPE);
            break;

        case OP_add_i:
        case OP_subtract_i:
        case OP_multiply_i:
            emit(opcode, 0, 0, INT_TYPE);
            break;

        case OP_negate:
            AvmAssert(type==NUMBER_TYPE FLOAT_ONLY(|| type == OBJECT_TYPE || type == FLOAT_TYPE || type == FLOAT4_TYPE));
            emit(opcode, sp, 0, type);
            break;

        case OP_negate_i:
            emit(opcode, sp, 0, INT_TYPE);
            break;

        case OP_bitand:
        case OP_bitor:
        case OP_bitxor:
            emit(opcode, 0, 0, INT_TYPE);
            break;

        case OP_lshift:
        case OP_rshift:
            emit(opcode, 0, 0, INT_TYPE);
            break;

        case OP_urshift:
            emit(opcode, 0, 0, UINT_TYPE);
            break;

        case OP_bitnot:
            emit(opcode, sp, 0, INT_TYPE);
            break;

        case OP_typeof:
            emit(opcode, sp, 0, STRING_TYPE);
            break;

        case OP_nextvalue:
        case OP_nextname:
            emit(opcode, 0, 0, NULL);
            break;

        case OP_hasnext:
            emit(opcode, 0, 0, INT_TYPE);
            break;

        case OP_hasnext2:
            emit(opcode, imm30, imm30b, BOOLEAN_TYPE);
            break;

        // sign extends
        case OP_sxi1:
        case OP_sxi8:
        case OP_sxi16:
            emit(opcode, sp, 0, INT_TYPE);
            break;

        // loads
        case OP_lix8:
        case OP_lix16:
        case OP_li8:
        case OP_li16:
        case OP_li32:
        case OP_lf32:
        case OP_lf64:
        {
            Traits* result = (opcode == OP_lf32 || opcode == OP_lf64) ? NUMBER_TYPE : INT_TYPE;
            emit(opcode, sp, 0, result);
            break;
        }
#ifdef VMCFG_FLOAT
        case OP_lf32x4:
        {
            emit(opcode, sp, 0, FLOAT4_TYPE);
            break;
        }
#endif
                
        // stores
        case OP_si8:
        case OP_si16:
        case OP_si32:
        case OP_sf32:
        case OP_sf64:
        {
            emit(opcode, 0, 0, VOID_TYPE);
            break;
        }
#ifdef VMCFG_FLOAT
        case OP_sf32x4:
        {
            emit(opcode, 0, 0, FLOAT4_TYPE);
            break;
        }
#endif
                
        case OP_getglobalscope:
            emitGetGlobalScope(sp+1);
            break;

        case OP_convert_s:
            localSet(sp, convertToString(sp, false), STRING_TYPE);
            break;

        case OP_esc_xelem:
        case OP_esc_xattr:
            emit(opcode, sp, 0, STRING_TYPE);
            break;

        case OP_debug:
            // ignored
            break;

        case OP_restargc:
        {
            // See documentation in writePrologue regarding rest arguments
            AvmAssert(info->needRestOrArguments() && info->lazyRest());
            LIns* out = callIns(FUNCTIONID(restargcHelper),
                                2,
                                localGetp(restLocal),
                                restArgc);
            localSet(sp, out, UINT_TYPE);
            break;
        }

        default:
            AvmAssertMsg(false, "unhandled opcode in CodegenLIR::write()");
            break;
        }
    }

    // coerce parameter types, starting at firstArg.
    void CodegenLIR::coerceArgs(MethodSignaturep mms, int argc, int firstArg)
    {
        int sp = state->sp();
        for (int arg = argc, n = 1; arg >= firstArg; arg--, n++) {
            Traits* target = (arg <= mms->param_count()) ? mms->paramTraits(arg) : NULL;
            int index = sp - (n - 1);
            emitCoerce(index, target);
        }
    }

    // Coerce parameter types, but not receiver.  The object in the reciever
    // position is the class or function object and we haven't called newInstance yet.
    // In this case, emitCall() will generate a call to newInstance, producing the
    // new object, then call its init function with the coerced arguments.
    void CodegenLIR::emitConstructCall(intptr_t method_id, int argc, LIns* ctor, Traits* ctraits)
    {
        Traits* itraits = ctraits->itraits;
        MethodInfo* m = itraits->init;
        MethodSignaturep mms = m->getMethodSignature();
        AvmAssert(mms->argcOk(argc)); // caller must check this before early binding to ctor

        coerceArgs(mms, argc, 1);
        emitCall(OP_construct, method_id, argc, ctor, ctraits, itraits, mms);
    }

    /**
     * emitCoerceCall is used when the jit finds an opportunity to early bind that the
     * driver did not.  It does the coersions using the signature of the callee, and
     * does not mutate FrameState.
     */
    void CodegenLIR::emitCoerceCall(AbcOpcode opcode, intptr_t method_id, int argc, MethodSignaturep mms)
    {
        AvmAssert(state->value(state->sp() - argc).notNull);  // make sure null check happened
        AvmAssert(opcode != OP_construct);
        AvmAssert(mms->argcOk(argc));
        coerceArgs(mms, argc, 0);
        emitCall(opcode, method_id, argc, mms->returnTraits(), mms);
    }

    void CodegenLIR::emitGetGlobalScope(int dest)
    {
        const ScopeTypeChain* scope = info->declaringScope();
        int captured_depth = scope->size;
        if (captured_depth > 0)
        {
            // enclosing scope
            emitGetscope(0, dest);
        }
        else
        {
            // local scope
            AvmAssert(state->scopeDepth > 0); // verifier checked.
            emitCopy(ms->scope_base(), dest);
        }
    }

    void CodegenLIR::writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits *type)
    {
        this->state = state;
        emitSetPc(pc);
        switch (opcode) {
        case OP_iflt:
        case OP_ifle:
        case OP_ifnlt:
        case OP_ifnle:
        case OP_ifgt:
        case OP_ifge:
        case OP_ifngt:
        case OP_ifnge:
        case OP_ifeq:
        case OP_ifstricteq:
        case OP_ifne:
        case OP_ifstrictne:
        {
            int32_t offset = (int32_t) opd1;
            int lhs = state->sp()-1;
            emitIf(opcode, pc+4/*size*/+offset, lhs, lhs+1);
            break;
        }
        case OP_iftrue:
        case OP_iffalse:
        {
            int32_t offset = (int32_t) opd1;
            int sp = state->sp();
            emitIf(opcode, pc+4/*size*/+offset, sp, 0);
            break;
        }
        case OP_jump:
        {
            int32_t offset = (int32_t) opd1;
            emit(opcode, uintptr_t(pc+4/*size*/+offset));
            break;
        }
        case OP_getslot:
            emitGetslot(opd1, state->sp(), type);
            break;
        case OP_getglobalslot: {
            int32_t dest_index = state->sp(); // driver already incremented it
            uint32_t slot = opd1;
            emitGetGlobalScope(dest_index);
            emitGetslot(slot, dest_index, type /* slot type */);
            break;
        }
        case OP_setglobalslot:
            emitSetslot(OP_setglobalslot, opd1, 0 /* computed or ignored */);
            break;
        case OP_call:
            emit(opcode, opd1 /*argc*/, 0, NULL);
            break;

        case OP_construct:
        {
            const uint32_t argc = opd1;
            int ctor_index = state->sp() - argc;
            Traits* ctraits = state->value(ctor_index).traits;
            LIns* ctor = localCopy(ctor_index);
            emitConstruct(argc, ctor, ctraits);
            break;
        }
        case OP_getouterscope:
            emitGetscope(opd1, state->sp()+1);
            break;
        case OP_getscopeobject:
            emitCopy(opd1 + ms->scope_base(), state->sp()+1);
            break;
        case OP_newfunction:
            AvmAssert(pool->getMethodInfo(opd1)->declaringTraits() == type);
            emit(opcode, opd1, state->sp()+1, type);
            break;
        case OP_pushscope:
        case OP_pushwith:
            emitCopy(state->sp(), opd1);
            break;
        case OP_findpropstrict:
        case OP_findproperty:
        {
            const Multiname *name = pool->precomputedMultiname(opd1);
            emit(opcode, (uintptr_t)name, 0, OBJECT_TYPE);
            break;
        }
        case OP_findpropglobalstrict:
        {
            // NOTE opcode not supported, deoptimizing
            const Multiname *name = pool->precomputedMultiname(opd1);
            emit(OP_findpropstrict, (uintptr_t)name, 0, OBJECT_TYPE);
            break;
        }
        case OP_findpropglobal:
        {
            // NOTE opcode not supported, deoptimizing
            const Multiname *name = pool->precomputedMultiname(opd1);
            emit(OP_findproperty, (uintptr_t)name, 0, OBJECT_TYPE);
            break;
        }

        case OP_newclass:
        {
            Traits* ctraits = pool->getClassTraits(opd1);
            AvmAssert(ctraits == type);
            emit(opcode, (uintptr_t)(void*)ctraits, state->sp(), type);
            break;
        }

        case OP_finddef:
        {
            // opd1=name index
            // type=script->declaringTraits
            uint32_t name_id = opd1;
            AvmAssert(pool->precomputedMultiname(name_id)->isBinding());
            int32_t dest_index = state->sp() + 1;
            LIns* table = loadEnvFinddefTable();
            // We always call out here because inlining the fast path inhibits
            // dead code elimination when this finddef isn't used.
            LIns* obj_ptr = addp(table, offsetof(FinddefTable, elements) + sizeof(ScriptObject*) * name_id);
            LIns* obj = callIns(FUNCTIONID(finddef_cache), 2,
                                obj_ptr, methodFrame);
            localSet(dest_index, ptrToNativeRep(type, obj), type);
            break;
        }

        case OP_restarg:
        {
            // See documentation in writePrologue regarding rest arguments
            AvmAssert(info->needRestOrArguments() && info->lazyRest());
            const Multiname *multiname = pool->precomputedMultiname(opd1);
            // The by-reference parameter &restLocal is handled specially for this
            // helper function in VarTracker::insCall and in CodegenLIR::analyze_call.
            LIns* out = callIns(FUNCTIONID(restargHelper),
                                6,
                                loadEnvToplevel(),
                                InsConstPtr(multiname),
                                loadAtomRep(state->sp()),
                                lea(restLocal << VARSHIFT(info) , vars),
                                restArgc,
                                (info->needRest() ?
                                    binaryIns(LIR_addp, ap_param, InsConstPtr((void*)(ms->rest_offset()))) :
                                    binaryIns(LIR_addp, ap_param, InsConstPtr((void*)sizeof(Atom)))));
            localSet(state->sp()-1, out, type);
            break;
        }

        default:
            // writeOp1() called with an improper opcode.
            AvmAssert(false);
            break;
        }
    }

    LIns* CodegenLIR::coerceToString(int index)
    {
        const FrameValue& value = state->value(index);
        Traits* in = value.traits;

        switch (bt(in)) {
        case BUILTIN_null:
        case BUILTIN_string:
            // fine to just load the pointer
            return localGetp(index);
        case BUILTIN_int:
            return callIns(FUNCTIONID(intToString), 2, coreAddr, localGet(index));
        case BUILTIN_uint:
            return callIns(FUNCTIONID(uintToString), 2, coreAddr, localGet(index));
#ifdef VMCFG_FLOAT
        case BUILTIN_float: 
                return callIns(FUNCTIONID(floatToString),2,coreAddr,localGetf(index));
        case BUILTIN_float4: 
                return callIns(FUNCTIONID(float4ToString), 2, coreAddr, localGetf4Addr(index)); 
#endif
        case BUILTIN_number:
            return callIns(FUNCTIONID(doubleToString), 2, coreAddr, localGetd(index));
        case BUILTIN_boolean: {
            // load "true" or "false" string constant from AvmCore.booleanStrings[]
            LIns *offset = binaryIns(LIR_lshp, i2p(localGet(index)), InsConst(PTR_SCALE));
            LIns *arr = InsConstPtr(&core->booleanStrings);
            return loadIns(LIR_ldp, 0, binaryIns(LIR_addp, arr, offset), ACCSET_OTHER, LOAD_CONST);
        }
        default:
            if (value.notNull) {
                // not eligible for CSE, and we know it's not null/undefined
                return emitStringCall(index, FUNCTIONID(string), true); // call string
            }
            return emitStringCall(index, FUNCTIONID(coerce_s), true); // call coerce_s
        }
    }

    /** emit code for * -> Number conversion */
    LIns* CodegenLIR::coerceToNumber(int index)
    {
        const FrameValue& value = state->value(index);
        Traits* in = value.traits;

        /* can't promote "Numeric" to "Number", we don't know the exact type at compile time */
        if (in && (in->isNumeric() || in == BOOLEAN_TYPE)) {
            return promoteNumberIns(in, index);
        } else {
            // * -> Number
            // Note that another version of this code exists in InvokerCompiler::downcast_and_store, if we make
            // changes here they might need to be propagated there too.
            #ifdef VMCFG_FASTPATH_FROMATOM
            if (inlineFastpath) {
                //     double result;
                //     intptr_t val = CONVERT_TO_ATOM(arg);
                //     if ((val & kAtomTypeMask) != kIntptrType) goto not_intptr;   # test for kIntptrType tag
                //     # kIntptrType
                //     result = double(val >> kAtomTypeSize);                         # extract integer value and convert to double
                //     goto done;
                // not_intptr:
                //     if ((val & kAtomTypeMask) != kDoubleType) goto not_double;   # test for kDoubleType tag
                //     # kDoubleType
                //     result = *(val - kDoubleType);                                 # remove tag and dereference
                //     goto done;
                // not_double:
                //     result = number(val);                                          # slow path -- call helper
                // done:
                //     return result;
                CodegenLabel not_intptr;
                CodegenLabel not_double;
                CodegenLabel done;
                suspendCSE();
                LIns* val = loadAtomRep(index);
                LIns* result = insAlloc(sizeof(double));
                LIns* tag = andp(val, AtomConstants::kAtomTypeMask);
                // kIntptrType
                branchToLabel(LIR_jf, eqp(tag, AtomConstants::kIntptrType), not_intptr);
                // Note that this works on 64bit platforms only if we are careful
                // to restrict the range of intptr values to those that fit within
                // the integer range of the double type.
                std(p2dIns(rshp(val, AtomConstants::kAtomTypeSize)), result, 0, ACCSET_OTHER);
                JIT_EVENT(jit_atom2double_fast_intptr);
                branchToLabel(LIR_j, NULL, done);
                emitLabel(not_intptr);
                // kDoubleType
                branchToLabel(LIR_jf, eqp(tag, AtomConstants::kDoubleType), not_double);
                std(ldd(subp(val, AtomConstants::kDoubleType), 0, ACCSET_OTHER), result, 0, ACCSET_OTHER);
                JIT_EVENT(jit_atom2double_fast_double);
                branchToLabel(LIR_j, NULL, done);
                emitLabel(not_double);
                std(callIns(FUNCTIONID(number), 1, val), result, 0, ACCSET_OTHER);
                JIT_EVENT(jit_atom2double_slow);
                emitLabel(done);
                resumeCSE();
                return ldd(result, 0, ACCSET_OTHER);
            }
            #endif

            return callIns(FUNCTIONID(number), 1, loadAtomRep(index));
        }
    }

#ifdef VMCFG_FLOAT
    /** emit code for * -> Float conversion */
    LIns* CodegenLIR::coerceToFloat(int index)
    {
        const FrameValue& value = state->value(index);
        Traits* in = value.traits;
        
        if (in && (in->isNumeric() || in == BOOLEAN_TYPE)) {
            return promoteFloatIns(in, index);
        } else {
            // * -> Float
            return callIns(FUNCTIONID(singlePrecisionFloat), 1, loadAtomRep(index));
        }
    }

    /** emit code for * -> Float4 conversion */
    LIns* CodegenLIR::coerceToFloat4(int index)
    {
        const FrameValue& value = state->value(index);
        Traits* in = value.traits;
        return promoteFloat4Ins(in, index);
    }

    /** emit code for * -> Float|Float4|Number conversion */
    LIns* CodegenLIR::coerceToNumeric(int index)
    {
        const FrameValue& value = state->value(index);
        Traits* in = value.traits;

        if( in && (in->isNumeric() || in == BOOLEAN_TYPE))  
            return loadAtomRep(index); // already numeric, just load the atom.
        else{
            CodegenLabel already_numeric("_coercenum_already_numeric_");
            CodegenLabel done("_coercenum_done_");
            CodegenLabel non_numeric("_coercenum_non_numeric_");
            LIns* val = loadAtomRep(index);
            LIns* tag = andp(val, AtomConstants::kAtomTypeMask);
            LIns* result = insAlloc(sizeof(intptr_t));
            suspendCSE();

            LIns* tagmask = binaryIns(LIR_lshi, InsConst(1),p2i(tag));
            const int32_t numericMask = ( (1<<AtomConstants::kIntptrType) | (1<< AtomConstants::kDoubleType) | (1 << AtomConstants::kSpecialBibopType) );
            branchToLabel(LIR_jt, binaryIns(LIR_eqi, binaryIns(LIR_andi, tagmask, InsConst(numericMask)) , InsConst(0)  ), non_numeric);
            branchToLabel(LIR_jf, eqp(val, AtomConstants::undefinedAtom), already_numeric); // we still need to test for "undefined" before we conclude it's already a numeric type
            /* If we got here, the input was "undefined" - just return NaN */
            stp(InsConstAtom(core->kNaN),result,0,ACCSET_OTHER ); 
            branchToLabel(LIR_j, NULL, done);

            emitLabel(non_numeric);
            stp(callIns(FUNCTIONID(numericAtom), 2, coreAddr, val) , result,0,ACCSET_OTHER);
            JIT_EVENT(jit_atom2numeric_dbl);
            branchToLabel(LIR_j, NULL, done);

            emitLabel(already_numeric);
#ifdef DEBUG
            // Above we assume that every bibop type is numeric.
            // Add a DEBUG guard in case we introduce future non-numeric bibop types.
            CodegenLabel really_numeric("_really_numeric_");
            branchToLabel(LIR_jf, eqp(tag, AtomConstants::kSpecialBibopType), really_numeric);
            LIns* bibopTag = lirout->insLoad(LIR_lduc2ui, andp(val, (Atom)MMgc::GCHeap::kBlockMask), 0, ACCSET_OTHER);
            branchToLabel(LIR_jt, binaryIns(LIR_eqi, bibopTag, InsConst(AtomConstants::kBibopFloatType)), really_numeric);
            branchToLabel(LIR_jt, binaryIns(LIR_eqi, bibopTag, InsConst(AtomConstants::kBibopFloat4Type)), really_numeric);
            callIns(FUNCTIONID(upe), 1, env_param);
            emitLabel(really_numeric);
#endif
            stp(val,result,0, ACCSET_OTHER); 
            JIT_EVENT(jit_atom2numeric_flt);
            emitLabel(done);
            resumeCSE();
            return ldp(result,0,ACCSET_OTHER);
        }
    }
#endif // VMCFG_FLOAT

    LIns *CodegenLIR::emitStringCall(int index, const CallInfo *stringCall, bool preserveNull)
    {
        LIns* val = loadAtomRep(index);

        if (inlineFastpath) {
            // Inline fast path for string conversion.
            // if preserveNull == false:
            //   if ((input & kAtomTypeMask) != kStringType) || (input == kStringType))
            //     output = stringCall (input);
            //   else
            //     output = input ^ kStringType;
            //
            // if preserveNull == true:
            //   if (input & kAtomTypeMask) != kStringType)
            //     output = stringCall (input);
            //   else
            //     output = input ^ kStringType;
            CodegenLabel not_stringptr;
            CodegenLabel done;
            suspendCSE();
            LIns* result = insAlloc(sizeof(intptr_t));
            LIns* tag = andp(val, AtomConstants::kAtomTypeMask);
            // kStringType
            branchToLabel(LIR_jf, eqp(tag, AtomConstants::kStringType), not_stringptr);
            if (!preserveNull) {
                // If our value is equal to kStringType, we have a null String ptr
                branchToLabel(LIR_jt, eqp(val, AtomConstants::kStringType), not_stringptr);
            }
            stp(xorp(val, AtomConstants::kStringType), result, 0, ACCSET_OTHER);
            branchToLabel(LIR_j, NULL, done);

            emitLabel(not_stringptr);
            stp(callIns(stringCall, 2, coreAddr, val), result, 0, ACCSET_OTHER);
            emitLabel(done);
            resumeCSE();
            return ldp(result, 0, ACCSET_OTHER);
        }

        return callIns(stringCall, 2, coreAddr, val);
    }

    // OP_convert_s needs a null String ptr to be converted to "null"
    // while our other usage prior to concatStrings handles null ptrs
    // correctly in AvmCore::concatStrings.  This function is different
    // than coerceToString in how undefinedAtom is handled:
    // convert: undefinedAtom -> "undefined"
    // coerce:  undefinedAtom -> "null" (see coerce_s)
    LIns* CodegenLIR::convertToString(int index, bool preserveNull)
    {
        const FrameValue& value = state->value(index);
        Traits* in = value.traits;
        Traits* stringType = STRING_TYPE;

        if (in != stringType || (!preserveNull && !value.notNull)) {
            if (in && (value.notNull || in->isNumeric() || in == BOOLEAN_TYPE)) {
                // convert is the same as coerce
                return coerceToString(index);
            } else {
                 // explicitly convert to string
                return emitStringCall(index, FUNCTIONID(string), preserveNull);
            }
        }

        // already String*
        return localGetp(index);
     }

    void CodegenLIR::writeNip(const FrameState* state, const uint8_t *pc, uint32_t count)
    {
        this->state = state;
        emitSetPc(pc);
        emitCopy(state->sp(), state->sp() - count);
    }

    void CodegenLIR::writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo* m, uintptr_t disp_id, uint32_t argc, Traits *type)
    {
        this->state = state;
        emitSetPc(pc);
        switch (opcode) {
        case OP_callproperty:
        case OP_callproplex:
        case OP_callpropvoid:
            AvmAssert(m->declaringTraits()->isInterface());
            emitTypedCall(OP_callinterface, ImtHolder::getIID(m), argc, type, m);
            break;
        case OP_callmethod:
            emitTypedCall(OP_callmethod, disp_id, argc, type, m);
            break;
        default:
            AvmAssert(false);
            break;
        }
    }

    void CodegenLIR::writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits *type)
    {
        this->state = state;
        emitSetPc(pc);
        int sp = state->sp();
        switch (opcode) {

        case OP_constructsuper:
        {
            Traits* base = info->declaringTraits()->base;
            // opd1=unused, opd2=argc
            if (base == OBJECT_TYPE && base->init->isTrivial()) {
                AvmAssert(opd2 == 0);   // The verifier should have caught a non-zero argc
                break;
            }
            emitTypedCall(OP_constructsuper, 0, opd2, VOID_TYPE, base->init);
            break;
        }

        case OP_setsuper:
        {
            const uint32_t index = opd1;
            const uint32_t n = opd2;
            Traits* base = type;
            int32_t ptrIndex = sp-(n-1);

            const Multiname* name = pool->precomputedMultiname(index);

            Binding b = toplevel->getBinding(base, name);
            Traits* propType = Traits::readBinding(base, b);
            const TraitsBindingsp basetd = base->getTraitsBindings();

            if (AvmCore::isSlotBinding(b)) {
                if (!AvmCore::isVarBinding(b)) {
                    // else, ignore write to readonly accessor
                    break;
                }
                int slot_id = AvmCore::bindingToSlotId(b);
                LIns* value = coerceToType(sp, propType);
                emitSetslot(OP_setslot, slot_id, ptrIndex, value);
                break;
            }
            if (AvmCore::isAccessorBinding(b)) {
                if (!AvmCore::hasSetterBinding(b)) {
                    // ignore write to readonly accessor
                    break;
                }
                // Invoke the setter
                int disp_id = AvmCore::bindingToSetterId(b);
                MethodSignaturep mms = basetd->getMethod(disp_id)->getMethodSignature();
                if (mms->argcOk(1)) {
                    emitCoerceCall(OP_callsuperid, disp_id, 1, mms);
                    break;
                }
            }
            // generic late bound case
            emit(opcode, (uintptr_t)name);
            break;
        }
        case OP_getsuper:
        {
            const uint32_t index = opd1;
            const uint32_t n = opd2;
            Traits* base = type;

            const Multiname* name = pool->precomputedMultiname(index);

            Binding b = toplevel->getBinding(base, name);
            Traits* propType = Traits::readBinding(base, b);

            if (AvmCore::isSlotBinding(b)) {
                int slot_id = AvmCore::bindingToSlotId(b);
                emitGetslot(slot_id, state->sp()-(n-1), propType);
                break;
            }
            if (AvmCore::hasGetterBinding(b)) {
                // Invoke the getter
                int disp_id = AvmCore::bindingToGetterId(b);
                const TraitsBindingsp basetd = base->getTraitsBindings();
                MethodSignaturep mms = basetd->getMethod(disp_id)->getMethodSignature();
                if (mms->argcOk(0)) {
                    emitCoerceCall(OP_callsuperid, disp_id, 0, mms);
                    break;
                }
            }

            // generic late-bound case
            emit(opcode, (uintptr_t)name, 0, propType);
            break;
        }
        case OP_callsuper:
        case OP_callsupervoid:
        {
            const uint32_t index = opd1;
            const uint32_t argc = opd2;
            Traits* base = type;
            const TraitsBindingsp basetd = base->getTraitsBindings();

            const Multiname *name = pool->precomputedMultiname(index);

            Binding b = toplevel->getBinding(base, name);

            if (AvmCore::isMethodBinding(b)) {
                int disp_id = AvmCore::bindingToMethodId(b);
                MethodSignaturep mms = basetd->getMethod(disp_id)->getMethodSignature();
                if (mms->argcOk(argc)) {
                    emitCoerceCall(OP_callsuperid, disp_id, argc, mms);
                    break;
                }
            }
            // generic late bound case
            emit(opcode, (uintptr_t)name, argc, NULL);
            break;
        }

        case OP_constructprop:
        {
            const uint32_t argc = opd2;
            const Multiname* name = pool->precomputedMultiname(opd1);

            const FrameValue& obj = state->peek(argc+1); // object
            Binding b = toplevel->getBinding(obj.traits, name);

            if (AvmCore::isSlotBinding(b))
            {
                int slot_id = AvmCore::bindingToSlotId(b);
                int ctor_index = state->sp() - argc;
                LIns* ctor = loadFromSlot(ctor_index, slot_id, type);
                emitConstruct(argc, ctor, type);
            }
            else
            {
                emit(opcode, (uintptr_t)name, argc, NULL);
            }
            break;
        }

        case OP_getproperty:
        {
            // NOTE opd2 is the stack offset to the reciever
            const Multiname* name = pool->precomputedMultiname(opd1);
            const FrameValue& obj = state->peek(opd2); // object
            Binding b = toplevel->getBinding(obj.traits, name);

            // early bind accessor
            if (AvmCore::hasGetterBinding(b))
            {
                MethodInfo* getter_info = obj.traits->getTraitsBindings()->getMethod(AvmCore::bindingToGetterId(b));
                if (getter_info->pool() == core->builtinPool)
                {
                    // Inline certain blessed and possibly-hot accessors.
                    //
                    // -  Optimize access to vector's "length", which is final.
                    //
                    //    The verifier has computed that the return type of the "length" 
                    //    getter is uint, so we can just load "len" here: it is a uint32_t.
                    //
                    // -  Optimize access to float4's x, y, z, and w elements.
                    //
                    //    These are statically known to produce float, so a simple load
                    //    will suffice.
                    //
                    // -  Optimize access to float4 swizzlers.
                    //
                    //    These are statically known to produce float4, the swizzling compiles
                    //    down to a single LIR instruction.

                    int32_t arrayDataOffset = -1;
                    int32_t lenOffset = -1;

#ifdef VMCFG_FLOAT
                    uint8_t shuffle_mask = 0;
                    if (matchShuffler(getter_info, &shuffle_mask)) {
                        NanoAssert(bt(type) == BUILTIN_float4);
                        localSet(sp, lirout->insSwz(localGetf4(sp), shuffle_mask), type);
                        break;
                    }
                    LOpcode f4_getter = LIR_skip;
#endif

                    switch (getter_info->method_id())
                    {
                        case avmplus::NativeID::__AS3___vec_Vector_object_length_get:
                            arrayDataOffset = int32_t(offsetof(ObjectVectorObject, m_list.m_data));
                            lenOffset = int32_t(offsetof(AtomListHelper::LISTDATA, len));
                            goto vector_length;
                        case avmplus::NativeID::__AS3___vec_Vector_double_length_get:
                            arrayDataOffset = int32_t(offsetof(DoubleVectorObject, m_list.m_data));
                            lenOffset = int32_t(offsetof(DataListHelper<double>::LISTDATA, len));
                            goto vector_length;
                        case avmplus::NativeID::__AS3___vec_Vector_int_length_get:
                            arrayDataOffset = int32_t(offsetof(IntVectorObject, m_list.m_data));
                            lenOffset = int32_t(offsetof(DataListHelper<int32_t>::LISTDATA, len));
                            goto vector_length;
                        case avmplus::NativeID::__AS3___vec_Vector_uint_length_get:
                            arrayDataOffset = int32_t(offsetof(UIntVectorObject, m_list.m_data));
                            lenOffset = int32_t(offsetof(DataListHelper<uint32_t>::LISTDATA, len));
                            goto vector_length;
#ifdef VMCFG_FLOAT
                        case avmplus::NativeID::float4_x_get:
                            f4_getter = LIR_f4x;
                            goto float4_element;
                        case avmplus::NativeID::float4_y_get:
                            f4_getter = LIR_f4y;
                            goto float4_element;
                        case avmplus::NativeID::float4_z_get:
                            f4_getter = LIR_f4z;
                            goto float4_element;
                        case avmplus::NativeID::float4_w_get:
                            f4_getter = LIR_f4w;
                            goto float4_element;
#endif
                        default:
                            goto invoke_getter;
                    }
                    
#ifdef VMCFG_FLOAT
                float4_element: {
                    NanoAssert(bt(type) == BUILTIN_float);
                    localSet(sp, lirout->ins1(f4_getter, localGetf4(sp)), type);
                    break;
                }
#endif

                vector_length: {
                    LIns *arrayData = loadIns(LIR_ldp, arrayDataOffset, localGetp(sp), ACCSET_OTHER, LOAD_NORMAL);
                    LIns *arrayLen  = loadIns(LIR_ldi, lenOffset, arrayData, ACCSET_OTHER, LOAD_NORMAL);
                    localSet(sp, arrayLen, type);
                    break;
                }

                } // inlining blessed getters

            invoke_getter:
                // Default case: Invoke the getter
                int disp_id = AvmCore::bindingToGetterId(b);
                const TraitsBindingsp objtd = obj.traits->getTraitsBindings();
                MethodInfo *f = objtd->getMethod(disp_id);
                AvmAssert(f != NULL);

                if (!obj.traits->isInterface()) {
                    emitTypedCall(OP_callmethod, disp_id, 0, type, f);
                }
                else {
                    emitTypedCall(OP_callinterface, ImtHolder::getIID(f), 0, type, f);
                }
                AvmAssert(type == f->getMethodSignature()->returnTraits());
            }
            else {
                emit(OP_getproperty, opd1, 0, type);
            }
            break;
        }

        case OP_setproperty:
        case OP_initproperty:
        {
            // opd2=n the stack offset to the reciever
            const Multiname *name = pool->precomputedMultiname(opd1);
            const FrameValue& obj = state->peek(opd2); // object
            Binding b = toplevel->getBinding(obj.traits, name);

            // early bind accessor
            if (AvmCore::hasSetterBinding(b))
            {
                // invoke the setter
                int disp_id = AvmCore::bindingToSetterId(b);
                const TraitsBindingsp objtd = obj.traits->getTraitsBindings();
                MethodInfo *f = objtd->getMethod(disp_id);
                AvmAssert(f != NULL);

                if (!obj.traits->isInterface()) {
                    emitTypedCall(OP_callmethod, disp_id, 1, type, f);
                }
                else {
                    emitTypedCall(OP_callinterface, ImtHolder::getIID(f), 1, type, f);
                }
            }
            else {
                emit(opcode, (uintptr_t)name);
            }
            break;
        }

        case OP_setslot:
            emitSetslot(OP_setslot, opd1, opd2);
            break;

        case OP_callproperty:
        case OP_callproplex:
        case OP_callpropvoid:
        {
            emit(opcode, opd1, opd2, NULL);
            break;
        }

        case OP_callstatic: {
            uint32_t method_id = opd1;
            uint32_t argc = opd2;
            emitTypedCall(OP_callstatic, method_id, argc, type, pool->getMethodInfo(method_id));
            break;
        }

        default:
            AvmAssert(false);
            break;
        }
    }

    void CodegenLIR::emitIntConst(int index, int32_t c, Traits* type)
    {
        localSet(index, lirout->insImmI(c), type);
    }

    void CodegenLIR::emitPtrConst(int index, void* c, Traits* type)
    {
        localSet(index, lirout->insImmP(c), type);
    }

    void CodegenLIR::emitDoubleConst(int index, const double* pd)
    {
        localSet(index, lirout->insImmD(*pd), NUMBER_TYPE);
    }

#ifdef VMCFG_FLOAT
    void CodegenLIR::emitFloatConst(int index, const float f)
    {
        localSet(index, lirout->insImmF(f), FLOAT_TYPE);
    }

    void CodegenLIR::emitFloat4Const(int index, const float4_t* f4)
    {
        localSet(index, lirout->insImmF4(*(const float4_t*)f4), FLOAT4_TYPE);
    }

    /**
     * If m is a float4 shuffle getter, e.g. xyxy(), then return true and
     * the shuffle mask to use with the LIR_swzf4 instruction.
     */
    bool CodegenLIR::matchShuffler(MethodInfo* m, uint8_t* mask)
    {
        using namespace avmplus::NativeID;
        AvmAssert(m->pool() == core->builtinPool);

        uint32_t id = m->method_id();
        if (id < float4_xxxx_get || id > float4_wwww_get)
            return false;
        id -= float4_xxxx_get;
        int x = (id >> 6) & 3;
        int y = (id >> 4) & 3;
        int z = (id >> 2) & 3;
        int w = (id >> 0) & 3;
        *mask = uint8_t(w << 6 | z << 4 | y << 2 | x << 0);
        return true;

        // We are counting on xxxx, xxxy, ... wwww being in order.
        NanoStaticAssert(float4_xxxy_get - float4_xxxx_get == 1);
        // todo: put the rest of them here.
        NanoStaticAssert(float4_wwww_get - float4_xxxx_get == 255);
    }
#endif // VMCFG_FLOAT
    
    void CodegenLIR::writeCoerceToNumeric(const FrameState* state, uint32_t loc)
    {
        this->state = state;
        emitSetPc(state->abc_pc);
        localSet(loc, IFFLOAT(coerceToNumeric(loc), coerceToNumber(loc)), OBJECT_TYPE);
    }

    void CodegenLIR::writeCoerceToFloat4(const FrameState* state, uint32_t index)
    {
#ifdef VMCFG_FLOAT
        AvmAssert(index >= 3);
        this->state = state;
        emitSetPc(state->abc_pc);
        LIns* x = coerceToFloat(index-3);
        LIns* y = coerceToFloat(index-2);
        LIns* z = coerceToFloat(index-1);
        LIns* w = coerceToFloat(index);
        LIns* val = lirout->ins4(LIR_ffff2f4, x, y, z, w);
        localSet(index, val, FLOAT4_TYPE);
#else
        (void) index; (void) state;
        AvmAssert(!"coerceToFloat4 encountered in CodegenLIR, with VMCFG_FLOAT turned off!!!");
#endif
    }

    void CodegenLIR::writeCoerce(const FrameState* state, uint32_t loc, Traits* result)
    {
        this->state = state;
        emitSetPc(state->abc_pc);
        emitCoerce(loc, result);
    }

    void CodegenLIR::emitCoerce(uint32_t loc, Traits* result)
    {
        localSet(loc, coerceToType(loc, result), result);
    }

    // If we have already generated this specialized function, return our
    // prior entry instead of re-specializing the same function.
    LIns* CodegenLIR::getSpecializedCall(LIns* origCall)
    {
        if (!specializedCallHashMap)
            return NULL;

        return specializedCallHashMap->get(origCall);
    }

    // Track any specialized function so if we try to specialize the same function
    // again we can re-use the original one.
    LIns * CodegenLIR::addSpecializedCall(LIns* origCall, LIns* specializedCall)
    {
        if (!specializedCallHashMap)
            specializedCallHashMap = new (*alloc1) HashMap<LIns *, LIns *>(*alloc1);

        specializedCallHashMap->put(origCall, specializedCall);

        return specializedCall;
    }

    // Attempt to replace a call with a call to another function, using the
    // mapping given by 'specs'.  The intent is that the replacement function
    // yield equivalent semantics in the context in which it will appear, but
    // more efficiently.  A single replacement is generated for each call LIns*,
    // and subquent requests to perform the same specializatoin will return
    // the previously-constructed replacement LIns*.  If an attempt is made to
    // specialize a function call for which no replacement is given in 'specs', we
    // return NULL.  It is assumed that the replacement function will return
    // an int32 value.

    LIns* CodegenLIR::specializeIntCall(LIns* call, const Specialization* specs)
    {
        LIns *priorCall = getSpecializedCall(call);
        if (priorCall)
            return priorCall;

        const CallInfo *ci = call->callInfo();
        int i = 0;
        while (specs[i].oldFunc != NULL) {
            if (specs[i].oldFunc == ci) {
                const CallInfo* nci = specs[i].newFunc;
                AvmAssert(nci->returnType() == ARGTYPE_I);
                LIns* specialization = callIns(nci, 2, call->arg(1), call->arg(0), INT_TYPE);
                addSpecializedCall(call, specialization);
                return specialization;
            }
            i++;
        }
        return NULL;
    }

    // Return true if we are promoting an int or uint to a double
    bool CodegenLIR::isPromote(LOpcode op)
    {
        return op == LIR_ui2d || op == LIR_i2d;
    }

    // Return non-null LIns* if input is a constant that fits into a int32_t
    LIns* CodegenLIR::imm2Int(LIns* imm)
    {
        if (imm->isImmI())
            ; // just use imm
        else if (imm->isImmD()) {
            double val = imm->immD();
            double cvt = (int32_t)val;
            if (val == 0 || val == cvt)
                imm = InsConst((int32_t)cvt);
            else
                imm = 0; // can't convert
        } else {
            imm = 0; // non-imm
        }
        return imm;
    }

    static const Specialization coerceDoubleToInt[] = {
        { FUNCTIONID(String_charCodeAtDI),    FUNCTIONID(String_charCodeAtIU) },
        { FUNCTIONID(String_charCodeAtDU),    FUNCTIONID(String_charCodeAtIU) },
        { FUNCTIONID(String_charCodeAtDD),    FUNCTIONID(String_charCodeAtID) },
        { 0, 0 }
    };

    // Perform coercion from a double to integer.
    // Try various optimization to avoid double math if possible
    // and specialize charCodeAt to an faster integer version.
    LIns* CodegenLIR::coerceNumberToInt(int loc)
    {
        LIns *arg = localGetd(loc);
        LOpcode op = arg->opcode();
        switch (op) {
            case LIR_ui2d:
            case LIR_i2d:
                return arg->oprnd1();
            case LIR_addd:
            case LIR_subd:
            case LIR_muld: {
                LIns *a = arg->oprnd1();
                a = isPromote(a->opcode()) ? a->oprnd1() : imm2Int(a);
                if (a) {
                    LIns *b = arg->oprnd2();
                    b = isPromote(b->opcode()) ? b->oprnd1() : imm2Int(b);
                    if (b)
                        return lirout->ins2(arithOpcodeD2I(op), a, b);
                }
                break;
            }
            // Optimize integer division when divisor is non-zero constant integer.
            // We cannot use LIR_divi if divisor is non-constant, as we need to generate a NaN on division by zero.
            case LIR_divd: {
                LIns *a = arg->oprnd1();
                LOpcode aOpcode = a->opcode();
                // We should never arrive here at all if both operands are constants, as the LIR_divd should
                // have been folded previously.  Furthermore, Nanojit does not permit both arguments of LIR_divi
                // to be constant, and will assert if this occurs, so let's be absolutely sure it will not by
                // specifically not attempting to optimize that case here.
                AvmAssert(!imm2Int(a) || !imm2Int(arg->oprnd2()));
                if (isPromote(aOpcode)) {
                    a = a->oprnd1();
                    LIns *b = imm2Int(arg->oprnd2());
                    if (b) {
                        int32_t intConst = b->immI();
                        if (intConst) {
                            // use faster unsigned right shift if our arg is unsigned and
                            // we have just one bit set in the divisor.
                            if (aOpcode == LIR_ui2d && intConst >= 0 && exactlyOneBit(intConst)) {
                                return lirout->ins2(LIR_rshui, a, lirout->insImmI(msbSet32(intConst)));
                            }
#if NJ_DIVI_SUPPORTED
                            else if (aOpcode == LIR_i2d) {
                                return lirout->ins2(LIR_divi, a, b);
                            }
#endif // NJ_DIVI_SUPPORTED
                        }
                    }
                }
                break;
            }
#ifdef AVMPLUS_64BIT
            case LIR_immq:
#endif // AVMPLUS_64BIT
            case LIR_immd:
                // const fold
                return InsConst(AvmCore::integer_d(arg->immD()));
            // Try to replace a call returning a double, which will then be
            // coerced to an integer, with a call that will produce an equivalent
            // integer value directly.
            case LIR_calld: {
                LIns* specialized = specializeIntCall(arg, coerceDoubleToInt);
                if (specialized)
                    return specialized;
            }
        }

        // For SSE capable machines, inline our double to integer conversion
        // using the CVTTSD2SI instruction.  If we get a 0x80000000 return
        // value, our double is outside the valid integer range we fallback
        // to calling doubleToInt32.
        if (haveSSE2()) {
            suspendCSE();
            CodegenLabel skip_label("goodint");
            LIns* intResult = insAlloc(sizeof(int32_t));
            LIns* fastd2i = lirout->ins1(LIR_d2i, arg);
            sti(fastd2i, intResult, 0, ACCSET_STORE_ANY);      // int32_t index
            LIns *c = binaryIns(LIR_eqi, fastd2i, InsConst(0x80000000));
            branchToLabel(LIR_jf, c, skip_label);
            LIns *funcCall = callIns(FUNCTIONID(doubleToInt32), 1, arg);
            sti(funcCall, intResult, 0, ACCSET_STORE_ANY);      // int32_t index
            emitLabel(skip_label);
            LIns *result = loadIns(LIR_ldi, 0, intResult, ACCSET_LOAD_ANY);
            resumeCSE();
            return result;
        }
        return callIns(FUNCTIONID(integer_d), 1, arg);
    }

#ifdef VMCFG_FLOAT
    LIns* CodegenLIR::coerceFloatToInt(int loc)
    {
        LIns *arg = localGetf(loc);
        if (arg->isop(LIR_immf))
            return InsConst(AvmCore::integer_d((double)arg->immF()));

        // fixme: Bug 707644; emperically, calling the helper is fastest.
        const bool use_cvttss2si = false;

        // For SSE capable machines, inline our float to integer conversion
        // using the CVTTSS2SI instruction.  If we get a 0x80000000 return
        // value, our float is outside the valid integer range we fallback
        // to calling doubleToInt32.
        if (use_cvttss2si && haveSSE2()) {
            suspendCSE();
            CodegenLabel skip_label("goodint");
            LIns* intResult = insAlloc(sizeof(int32_t));
            LIns* fastf2i = lirout->ins1(LIR_f2i, arg);
            sti(fastf2i, intResult, 0, ACCSET_STORE_ANY);      // int32_t index
            LIns *c = binaryIns(LIR_eqi, fastf2i, InsConst(0x80000000));
            branchToLabel(LIR_jf, c, skip_label);
            LIns *funcCall = callIns(FUNCTIONID(doubleToInt32), 1, f2dIns(arg));
            sti(funcCall, intResult, 0, ACCSET_STORE_ANY);      // int32_t index
            emitLabel(skip_label);
            LIns *result = loadIns(LIR_ldi, 0, intResult, ACCSET_LOAD_ANY);
            resumeCSE();
            return result;
        }
        // fallback: promote float->double, then convert double->int
        return callIns(FUNCTIONID(integer_d), 1, f2dIns(arg));
    }
#endif

    LIns* CodegenLIR::coerceToType(int loc, Traits* result)
    {
        const FrameValue& value = state->value(loc);
        Traits* in = value.traits;
        LIns* expr;

        if (result == NULL)
        {
            // coerce to * is simple, we just save the atom rep.
            expr = loadAtomRep(loc);
        }
        else if (result == OBJECT_TYPE)
        {
            if (in == NULL || in == VOID_TYPE)
            {
                // value already boxed but we need to coerce undefined->null
                if (!value.notNull) {
                    // v == undefinedAtom ? nullObjectAtom : v;
                    LIns *v = localGetp(loc);
                    expr = choose(eqp(v, undefConst), nullObjectAtom, v);
                } else {
                    expr = loadAtomRep(loc);
                }
            }
            else
            {
                // value cannot be undefined so just box it
                expr = loadAtomRep(loc);
            }
        }
        else if (!result->isMachineType() && in == NULL_TYPE)
        {
            // it's fine to coerce null to a pointer type, just load the value
            expr = localGetp(loc);
        }
        else if (result == NUMBER_TYPE)
        {
            expr = coerceToNumber(loc);
        }
#ifdef VMCFG_FLOAT
        else if (result == FLOAT_TYPE)
        {
            expr = coerceToFloat(loc);
        }
        else if (result == FLOAT4_TYPE)
        {
            expr = coerceToFloat4(loc);
        }
#endif // VMCFG_FLOAT
        else if (result == INT_TYPE)
        {
            if (in == UINT_TYPE || in == BOOLEAN_TYPE || in == INT_TYPE)
            {
                // just load the value
                expr = localGet(loc);
            }
            else if (in == NUMBER_TYPE)
            {
                // narrowing conversion number->int
                expr = coerceNumberToInt(loc);
            }
#ifdef VMCFG_FLOAT
            else if (in == FLOAT_TYPE)
            {
                expr = coerceFloatToInt(loc);
            }
#endif
            else
            {
                // * -> int
                expr = callIns(FUNCTIONID(integer), 1, loadAtomRep(loc));
            }
        }
        else if (result == UINT_TYPE)
        {
            if (in == INT_TYPE || in == BOOLEAN_TYPE || in == UINT_TYPE)
            {
                // just load the value
                expr = localGet(loc);
            }
            else if (in == NUMBER_TYPE)
            {
                expr = coerceNumberToInt(loc);
            }
#ifdef VMCFG_FLOAT
            else if (in == FLOAT_TYPE)
            {
                expr = coerceFloatToInt(loc);
            }
#endif
            else
            {
                // * -> uint
                expr = callIns(FUNCTIONID(toUInt32), 1, loadAtomRep(loc));
            }
        }
        else if (result == BOOLEAN_TYPE)
        {
            if (in == BOOLEAN_TYPE)
            {
                expr = localGet(loc);
            }
            else if (in == NUMBER_TYPE FLOAT_ONLY(|| in ==FLOAT_TYPE) )
            {
                LIns* ins = FLOAT_ONLY( in == FLOAT_TYPE ? f2dIns(localGetf(loc)):)
                                                                  localGetd(loc);
                expr = callIns(FUNCTIONID(doubleToBool), 1, ins);
            }
            else if (in == INT_TYPE || in == UINT_TYPE)
            {
                // int to bool: b = (i==0) == 0
                expr = eqi0(eqi0(localGet(loc)));
            }
            else if (in && !in->hasComplexEqualityRules())
            {
                // ptr to bool: b = (p==0) == 0
                expr = eqi0(eqp0(localGetp(loc)));
            }
            else
            {
                // * -> Boolean
                expr = callIns(FUNCTIONID(boolean), 1, loadAtomRep(loc));
            }
        }
        else if (result == STRING_TYPE)
        {
            expr = coerceToString(loc);
        }
        else if (in && !in->isMachineType() && !result->isMachineType()
               && in != STRING_TYPE && in != NAMESPACE_TYPE)
        {
            if (!Traits::canAssign(result, in)) {
                // coerceobj_obj() is void, but we mustn't optimize it out; we only call it when required
                callIns(FUNCTIONID(coerceobj_obj), 3,
                    env_param, localGetp(loc), InsConstPtr(result));
            }
            // the input pointer has now been checked but it's still the same value.
            expr = localGetp(loc);
        }
        else if (!result->isMachineType() && result != NAMESPACE_TYPE)
        {
            // result is a ScriptObject based type.
            expr = downcast_obj(loadAtomRep(loc), env_param, result);
        }
        else if (result == NAMESPACE_TYPE && in == NAMESPACE_TYPE)
        {
            expr = localGetp(loc);
        }
        else
        {
            LIns* value = loadAtomRep(loc);
            // resultValue = coerce(caller_env, inputValue, traits)
            LIns* out = callIns(FUNCTIONID(coerce), 3,
                env_param, value, InsConstPtr(result));

            // store the result
            expr = atomToNativeRep(result, out);
        }
        return expr;
    }

    void CodegenLIR::writeCheckNull(const FrameState* state, uint32_t index)
    {
        this->state = state;
        emitSetPc(state->abc_pc);
        emitCheckNull(localCopy(index), state->value(index).traits);
    }

    void CodegenLIR::emitCheckNull(LIns* ptr, Traits* t)
    {
        // The result is either unchanged or an exception is thrown, so
        // we don't save the result.  This is the null pointer check.
        if (!isNullable(t) || varTracker->isNotNull(ptr))
            return;
        _nvprof("nullcheck",1);
        BuiltinType ty = bt(t);
        if (valueStorageType(ty) == SST_atom) {
            _nvprof("nullcheck atom", 1);
            if (ty != BUILTIN_object) {
                // If we know atom value is an object, it cannot be undefined.
                branchToLabel(LIR_jt, eqp(ptr, undefConst), upe_label); // if (p == undefined) throw undefined pointer error
            }
            branchToLabel(LIR_jt, ltup(ptr, undefConst), npe_label);    // if (p < undefined) throw null pointer error
        } else {
            _nvprof("nullcheck ptr", 1);
            branchToLabel(LIR_jt, eqp0(ptr), npe_label);
        }
        varTracker->setNotNull(ptr, t);
    }

    // Save our current PC location for the catch finder later.
    void CodegenLIR::emitSetPc(const uint8_t* pc)
    {
        AvmAssert(state->abc_pc == pc);
        // update bytecode ip if necessary
        if (_save_eip && lastPcSave != pc) {
            stp(InsConstPtr((void*)(pc - code_pos)), _save_eip, 0, ACCSET_OTHER);
            lastPcSave = pc;
        }
    }

    // This is for VTable->createInstanceProc which is called by OP_construct
    FASTFUNCTION(CALL_INDIRECT, SIG2(P,P,P), createInstanceProc)

    void CodegenLIR::emitIsNaN(Traits* result)
    {
        int op1 = state->sp();
        /* In practice, floats will be coerced to double before calling isNaN - so "singlePrecision" should be always false for now.
        But this is implemented as if isNaN was polymorphic - there's no reason why it won't eventually be polymorphic */
        bool singlePrecision = IFFLOAT(state->value(op1).traits == FLOAT_TYPE, false);
        LIns *f = singlePrecision? localGetf(op1):localGetd(op1);
        if (isPromote(f->opcode())) {
            // Promoting an integer to a double cannot result in a NaN.
            localSet(op1-1, InsConst(0), result);
        }
        else {
#ifdef VMCFG_FLOAT
            if (f->opcode()==LIR_f2d) {
                /* handle the case when isNan() was called on a float */
                f = f->oprnd1();
                singlePrecision = true;
            }
            LOpcode eqop = singlePrecision ? LIR_eqf : LIR_eqd;
#else
            LOpcode eqop = LIR_eqd;
#endif // VMCFG_FLOAT

            // LIR is required to follow IEEE floating point semantics, thus x is a NaN iff x != x.
            localSet(op1-1, binaryIns(LIR_eqi, binaryIns(eqop, f, f), InsConst(0)), result);
        }
    }

    void CodegenLIR::emitIntMathMin(Traits* result)
    {
        // if (x < y)
        //   return x
        // else
        //   return y
        int op1 = state->sp();
        int op2 = state->sp() - 1;
        LIns *x = getSpecializedArg(op1, BUILTIN_int);
        LIns *y = getSpecializedArg(op2, BUILTIN_int);
        LIns *s1 = binaryIns(LIR_lti, x, y);
        LIns *s2 = lirout->insChoose(s1, x, y, use_cmov);
        // coerceNumberToInt will remove a d2i/i2d roundtrip
        localSet(op1-2, i2dIns(s2), result);
    }

    void CodegenLIR::emitIntMathMax(Traits* result)
    {
        // if (x > y)
        //   return x
        // else
        //   return y
        int op1 = state->sp();
        int op2 = state->sp() - 1;
        LIns *x = getSpecializedArg(op1, BUILTIN_int);
        LIns *y = getSpecializedArg(op2, BUILTIN_int);
        LIns *s1 = binaryIns(LIR_gti, x, y);
        LIns *s2 = lirout->insChoose(s1, x, y, use_cmov);
        // coerceNumberToInt will remove a d2i/i2d roundtrip
        localSet(op1-2, i2dIns(s2), result);
    }

    void CodegenLIR::emitStringLength(Traits* result)
    {
        int op1 = state->sp();
        LIns * ptr = loadIns(LIR_ldi, offsetof(String, m_length), localGetp(op1), ACCSET_OTHER, LOAD_CONST);
        localSet(op1, ptr, result);
    }

#ifdef VMCFG_FLOAT
    void CodegenLIR::emitFloatUnary(Traits* result, LOpcode op) {
        int sp = state->sp();
        localSet(sp - 1, lirout->ins1(op, localGetf(sp)), result);
    }

    void CodegenLIR::emitFloatAbs(Traits* result)        { emitFloatUnary(result, LIR_absf);   }
    void CodegenLIR::emitFloatReciprocal(Traits* result) { emitFloatUnary(result, LIR_recipf); }
    void CodegenLIR::emitFloatRsqrt(Traits* result)      { emitFloatUnary(result, LIR_rsqrtf); }
    void CodegenLIR::emitFloatSqrt(Traits* result)       { emitFloatUnary(result, LIR_sqrtf);  }

    void CodegenLIR::emitFloat4unary(Traits* result, LOpcode op) {
        int sp = state->sp();
        localSet(sp - 1, lirout->ins1(op, localGetf4(sp)), result);
    }

    void CodegenLIR::emitFloat4abs(Traits* result)        { emitFloat4unary(result, LIR_absf4); }
    void CodegenLIR::emitFloat4reciprocal(Traits* result) { emitFloat4unary(result, LIR_recipf4); }
    void CodegenLIR::emitFloat4rsqrt(Traits* result)      { emitFloat4unary(result, LIR_rsqrtf4); }
    void CodegenLIR::emitFloat4sqrt(Traits* result)       { emitFloat4unary(result, LIR_sqrtf4); }

    // Helper to expand dotf4, dotf3, dotf2
    LIns* CodegenLIR::emitDot(LOpcode dot_op, LIns* a, LIns* b) {
        if (core->config.njconfig.i386_sse41)
            return lirout->ins2(dot_op, a, b);
        LIns* c = lirout->ins2(LIR_mulf4, a, b);
        LIns* sum = lirout->ins2(LIR_addf, lirout->ins1(LIR_f4x, c), lirout->ins1(LIR_f4y, c));
        if (dot_op == LIR_dotf3 || dot_op == LIR_dotf4)
            sum = lirout->ins2(LIR_addf, sum, lirout->ins1(LIR_f4z, c));
        if (dot_op == LIR_dotf4)
            sum = lirout->ins2(LIR_addf, sum, lirout->ins1(LIR_f4w, c));
        return sum;
    }

    // magnitudeK(x) = sqrt(dotK(x, x))
    LIns* CodegenLIR::magnitude(LOpcode dot_op, LIns* x) {
        return lirout->ins1(LIR_sqrtf, emitDot(dot_op, x, x));
    }

    /** normalize(x:float4) = x / magnitude(x) = x / sqrt(dot(x,x)) */
    void CodegenLIR::emitFloat4normalize(Traits* result) {
        int sp = state->sp();
        LIns* x = localGetf4(sp);
        LIns* mag4 = lirout->ins1(LIR_f2f4, magnitude(LIR_dotf4, x));
        LIns* normalize = lirout->ins2(LIR_divf4, x, mag4);
        localSet(sp - 1, normalize, result);
    }

    /** cross(a:float4, b:float4) = a.yzx0 * b.zxy0 - a.zxy0 * b.yzx0
        We implement the equivalent:a.yzxw * b.zxyw - a.zxyw * b.yzxw
    */
    void CodegenLIR::emitFloat4cross(Traits* result) {
        int sp = state->sp();
        LIns* x = localGetf4(sp - 1);
        LIns* y = localGetf4(sp);

        LIns* xs1 = lirout->insSwz(x,0xD8);
        LIns* ys1 = lirout->insSwz(y,0xE1);
        LIns* m1  = lirout->ins2(LIR_mulf4,xs1,ys1);

        LIns* xs2 = lirout->insSwz(x,0xE1);
        LIns* ys2 = lirout->insSwz(y,0xD8);
        LIns* m2  = lirout->ins2(LIR_mulf4,xs2,ys2);

        LIns* cross = lirout->ins2(LIR_subf4, m1,m2);
        localSet(sp - 2, cross , result);
    }

    void CodegenLIR::emitFloat4binary(Traits* result, LOpcode op) {
        int sp = state->sp();
        LIns* x = localGetf4(sp - 1);
        LIns* y = localGetf4(sp);
        localSet(sp - 2, lirout->ins2(op, x, y), result);
    }

    void CodegenLIR::emitFloat4max(Traits* result)            { emitFloat4binary(result, LIR_maxf4);   }
    void CodegenLIR::emitFloat4min(Traits* result)            { emitFloat4binary(result, LIR_minf4);   }
    void CodegenLIR::emitFloat4Greater(Traits* result)        { emitFloat4binary(result, LIR_cmpgtf4); }
    void CodegenLIR::emitFloat4GreaterOrEqual(Traits* result) { emitFloat4binary(result, LIR_cmpgef4); }
    void CodegenLIR::emitFloat4Less(Traits* result)           { emitFloat4binary(result, LIR_cmpltf4); }
    void CodegenLIR::emitFloat4LessOrEqual(Traits* result)    { emitFloat4binary(result, LIR_cmplef4); }
    void CodegenLIR::emitFloat4Equal(Traits* result)          { emitFloat4binary(result, LIR_cmpeqf4); }
    void CodegenLIR::emitFloat4NotEqual(Traits* result)       { emitFloat4binary(result, LIR_cmpnef4); }

    void CodegenLIR::emitFloat4dot(Traits* result, LOpcode op) {
        int sp = state->sp();
        LIns* x = localGetf4(sp - 1);
        LIns* y = localGetf4(sp);
        localSet(sp - 2, emitDot(op, x, y), result);
    }

    void CodegenLIR::emitFloat4dot(Traits* result)        { emitFloat4dot(result, LIR_dotf4); }
    void CodegenLIR::emitFloat4dot2(Traits* result)       { emitFloat4dot(result, LIR_dotf2); }
    void CodegenLIR::emitFloat4dot3(Traits* result)       { emitFloat4dot(result, LIR_dotf3); }

    void CodegenLIR::emitMagnitude(Traits* result, LOpcode dot_op) {
        int sp = state->sp();
        LIns* x = localGetf4(sp);
        localSet(sp - 1, magnitude(dot_op, x), result);
    }

    void CodegenLIR::emitFloat4magnitude(Traits* result)  { emitMagnitude(result, LIR_dotf4); }
    void CodegenLIR::emitFloat4magnitude2(Traits* result) { emitMagnitude(result, LIR_dotf2); }
    void CodegenLIR::emitFloat4magnitude3(Traits* result) { emitMagnitude(result, LIR_dotf3); }

    // distanceK(x,y) = magnitudeK(x - y)
    void CodegenLIR::emitDistance(Traits* result, LOpcode dot_op) {
        int sp = state->sp();
        LIns* x = localGetf4(sp - 1);
        LIns* y = localGetf4(sp);
        LIns* diff = lirout->ins2(LIR_subf4, x, y);
        localSet(sp - 2, magnitude(dot_op, diff), result);
    }

    void CodegenLIR::emitFloat4distance(Traits* result)  { emitDistance(result, LIR_dotf4); }
    void CodegenLIR::emitFloat4distance2(Traits* result) { emitDistance(result, LIR_dotf2); }
    void CodegenLIR::emitFloat4distance3(Traits* result) { emitDistance(result, LIR_dotf3); }
#endif

    // Determine a mask for our argument that indicates to which types it may be trivially converted
    // without insertion of additional instructions and without loss.  This information may allow us
    // to substitute a numeric operation on a more specific type than that inferred by the verifier.
    // For example:
    //    A number that has been promoted from an integer will be marked as a number|int.
    //    A constant number that is an unsigned integer will be marked as a number|uint|int
    int32_t CodegenLIR::determineBuiltinMaskForArg (int argOffset)
    {
        BuiltinType bt = this->bt(state->value(argOffset).traits);
        int32_t btMask = 1 << bt;
        if (bt == BUILTIN_number) {
            LIns *arg = localGetd(argOffset);
            if (arg->isImmD()) {
                int32_t intVal = (int32_t) arg->immD();
                if ((double) intVal == arg->immD() && !MathUtils::isNegZero(arg->immD())) {
                    if (intVal >= 0)
                        btMask |= 1 << BUILTIN_uint;
                    btMask |= 1 << BUILTIN_int;
                }
#ifdef VMCFG_FLOAT
                float fVal = (float) arg->immD();
                if((double)fVal == arg->immD() || MathUtils::isNaN(arg->immD()))
                    btMask |= 1 << BUILTIN_float;
#endif // VMCFG_FLOAT
            }
            else if (arg->opcode() == LIR_i2d)
                btMask |= 1 << BUILTIN_int;
            else if (arg->opcode() == LIR_ui2d)
                btMask |= 1 << BUILTIN_uint;
#ifdef VMCFG_FLOAT
            else if (arg->opcode() == LIR_f2d)
                btMask |= 1<< BUILTIN_float;
        }
        else if (bt == BUILTIN_float) {
            LIns *arg = localGetf(argOffset);
            btMask |= 1 << BUILTIN_number;
            if (arg->isImmF()) {
                int32_t intVal = (int32_t) arg->immF();
                if ((float) intVal == arg->immF() && !MathUtils::isNegZero(arg->immF())) {
                    if (intVal >= 0)
                        btMask |= 1 << BUILTIN_uint;
                    btMask |= 1 << BUILTIN_int;
                }
            }
            else if (arg->opcode() == LIR_i2f)
                btMask |= 1 << BUILTIN_int;
            else if (arg->opcode() == LIR_ui2f)
                btMask |= 1 << BUILTIN_uint;
#endif // VMCFG_FLOAT
        }
        else if (bt == BUILTIN_int) {
            LIns *arg = localGet(argOffset);
            if (arg->isImmI() && arg->immI() >= 0)
                btMask |= 1 << BUILTIN_uint;
        }

        return btMask;
    }

    // Given an argument and a builtin type to which it may be trivially converted,
    // return a LIns that represents the argument as a value of the correct machine type.
    LIns* CodegenLIR::getSpecializedArg (int argOffset, BuiltinType newBt)
    {
        BuiltinType oldBt = this->bt(state->value(argOffset).traits);

        if (oldBt == BUILTIN_number) {
            LIns *arg = localGetd(argOffset);
            if (newBt == BUILTIN_int) {
                if (arg->isImmD())
                    return InsConst((int32_t)arg->immD());
                else if (arg->opcode() == LIR_i2d)
                    return arg->oprnd1();
                else
                    AvmAssert(0);
            }
            else if (newBt == BUILTIN_uint) {
                if (arg->isImmD())
                    return InsConst((int32_t)arg->immD());
                else if (arg->opcode() == LIR_ui2d)
                    return arg->oprnd1();
                else
                    AvmAssert(0);
            }
#ifdef VMCFG_FLOAT
            else if (newBt == BUILTIN_float) {  // not used right now, but maybe soon...
                if (arg->isImmD())
                    return InsConstFlt((float)arg->immD());
                else if (arg->opcode() == LIR_f2d)
                    return arg->oprnd1();
                else
                    AvmAssert(0);
            }
            AvmAssert(newBt==BUILTIN_number);
            return arg;
        }
        else if (oldBt == BUILTIN_float) {
            LIns *arg = localGetf(argOffset);
            if (newBt == BUILTIN_int) {
                if (arg->isImmF())
                    return InsConst((int32_t)arg->immF());
                else if (arg->opcode() == LIR_i2f)
                    return arg->oprnd1();
                else
                    AvmAssert(0);
            }
            else if (newBt == BUILTIN_uint) {
                if (arg->isImmD())
                    return InsConst((int32_t)arg->immD());
                else if (arg->opcode() == LIR_ui2d)
                    return arg->oprnd1();
                else
                    AvmAssert(0);
            }
            else if (newBt == BUILTIN_number) {  // not used right now, but maybe soon...
                if (arg->isImmF())
                    return lirout->insImmD(arg->immF());
                else 
                    return f2dIns(arg);
            }
            AvmAssert(newBt==BUILTIN_float);
            return arg;
#endif // VMCFG_FLOAT
        }

        /* See determineBuiltinMaskForArg - so far we can have:
             - for Number/float - newBt may be any of float, int, uint, number
             - for int  - newBt may also be uint 
             - for others: newBt must be identical with oldBt
             That's why the switch below works.
        */

        switch (newBt) {
#ifdef VMCFG_FLOAT
        case BUILTIN_number:
            AvmAssertMsg(false, "Case should be already handled; this means oldBt is not number, but newBt is Number!");
            return localGetd(argOffset);
        case BUILTIN_float:
            AvmAssertMsg(false, "Case should be already handled; this means oldBt is not float, but newBt is float!");
            return localGetf(argOffset);
        case BUILTIN_float4:
            AvmAssertMsg(false, "Case should be already handled; this means oldBt is not float4, but newBt is float4!");
            return localGetf4(argOffset);
#else
        case BUILTIN_number:
            return localGetd(argOffset);
#endif // VMCFG_FLOAT
        case BUILTIN_boolean:
        case BUILTIN_int:
        case BUILTIN_uint:
            return localGet(argOffset);
        default:
            return localGetp(argOffset);
        }
    }

    // For a given builtin function id and argument count, matching of argument lists occurs in the order in which the
    // entries appear in the table below.  More specialized variants that are expected to execute faster should be listed
    // prior to the more general cases.  If no argument list matches, specialization is not performed.

    const CodegenLIR::FunctionMatch CodegenLIR::specializedFunctions[] =
    {
        { 0, /* dummy entry so 0 can be treated as HashMap miss*/ 1, {}, 0, 0},
        { avmplus::NativeID::native_script_function_isNaN, 1, {BUILTIN_number, BUILTIN_none},   0, &CodegenLIR::emitIsNaN },
        /*could theoretically also add { avmplus::NativeID::native_script_function_isNaN, 1, {BUILTIN_float, BUILTIN_none},   0, &CodegenLIR::emitIsNaN }, */

        { avmplus::NativeID::String_AS3_charCodeAt,        1, {BUILTIN_uint,   BUILTIN_none},   FUNCTIONID(String_charCodeAtDU), 0},
        { avmplus::NativeID::String_AS3_charCodeAt,        1, {BUILTIN_int,    BUILTIN_none},   FUNCTIONID(String_charCodeAtDI), 0},
        { avmplus::NativeID::String_AS3_charCodeAt,        1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(String_charCodeAtDD), 0},
        { avmplus::NativeID::String_AS3_charAt,            1, {BUILTIN_uint,   BUILTIN_none},   FUNCTIONID(String_charAtU), 0},
        { avmplus::NativeID::String_AS3_charAt,            1, {BUILTIN_int,    BUILTIN_none},   FUNCTIONID(String_charAtI), 0},
        { avmplus::NativeID::String_AS3_charAt,            1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(String_charAtD), 0},

        { avmplus::NativeID::String_length_get,            0, {BUILTIN_none,   BUILTIN_none},   0, &CodegenLIR::emitStringLength},

        { avmplus::NativeID::Math_acos,                    1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_acos), 0},
        { avmplus::NativeID::Math_asin,                    1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_asin), 0},
        { avmplus::NativeID::Math_atan,                    1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_atan), 0},
        { avmplus::NativeID::Math_ceil,                    1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_ceil), 0},
        { avmplus::NativeID::Math_cos,                     1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_cos), 0},
        { avmplus::NativeID::Math_exp,                     1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_exp), 0},
        { avmplus::NativeID::Math_floor,                   1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_floor), 0},
        { avmplus::NativeID::Math_log,                     1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_log), 0},
        { avmplus::NativeID::Math_round,                   1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_round), 0},
        { avmplus::NativeID::Math_sin,                     1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_sin), 0},
        { avmplus::NativeID::Math_sqrt,                    1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_sqrt), 0},
        { avmplus::NativeID::Math_tan,                     1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_tan), 0},

        { avmplus::NativeID::Math_atan2,                   2, {BUILTIN_number, BUILTIN_number}, FUNCTIONID(Math_atan2), 0},
        { avmplus::NativeID::Math_pow,                     2, {BUILTIN_number, BUILTIN_number}, FUNCTIONID(Math_pow), 0},

        // We would like to inline abs(), but the obvious implementation does not work, as both 0.0 and -0.0 compare as the same.
        { avmplus::NativeID::Math_abs,                     1, {BUILTIN_number, BUILTIN_none},   FUNCTIONID(Math_abs), 0},

#if defined VMCFG_FLOAT && defined VMCFG_IA32
        // float4
        { avmplus::NativeID::float4_abs,                   1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4abs},
        { avmplus::NativeID::float4_max,                   2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4max},
        { avmplus::NativeID::float4_min,                   2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4min},
        { avmplus::NativeID::float4_reciprocal,            1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4reciprocal},
        { avmplus::NativeID::float4_rsqrt,                 1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4rsqrt},
        { avmplus::NativeID::float4_sqrt,                  1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4sqrt},
        { avmplus::NativeID::float4_cross,                 2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4cross},
        { avmplus::NativeID::float4_normalize,             1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4normalize},
        { avmplus::NativeID::float4_dot,                   2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4dot},
        { avmplus::NativeID::float4_dot2,                  2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4dot2},
        { avmplus::NativeID::float4_dot3,                  2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4dot3},
        { avmplus::NativeID::float4_magnitude,             1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4magnitude},
        { avmplus::NativeID::float4_magnitude3,            1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4magnitude3},
        { avmplus::NativeID::float4_magnitude2,            1, {BUILTIN_float4, BUILTIN_none},   0, &CodegenLIR::emitFloat4magnitude2},
        { avmplus::NativeID::float4_distance,              2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4distance},
        { avmplus::NativeID::float4_distance2,             2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4distance2},
        { avmplus::NativeID::float4_distance3,             2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4distance3},
        { avmplus::NativeID::float4_isGreater,             2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4Greater},
        { avmplus::NativeID::float4_isLess,                2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4Less},
        { avmplus::NativeID::float4_isGreaterOrEqual,      2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4GreaterOrEqual},
        { avmplus::NativeID::float4_isLessOrEqual,         2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4LessOrEqual},
        { avmplus::NativeID::float4_isEqual,               2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4Equal},
        { avmplus::NativeID::float4_isNotEqual,            2, {BUILTIN_float4, BUILTIN_float4}, 0, &CodegenLIR::emitFloat4NotEqual},
#endif

        { avmplus::NativeID::Math_min,                     2, {BUILTIN_int,    BUILTIN_int},    0, &CodegenLIR::emitIntMathMin},
        { avmplus::NativeID::Math_max,                     2, {BUILTIN_int,    BUILTIN_int},    0, &CodegenLIR::emitIntMathMax},
        { avmplus::NativeID::Math_private__min,            2, {BUILTIN_int,    BUILTIN_int},    0, &CodegenLIR::emitIntMathMin},
        { avmplus::NativeID::Math_private__max,            2, {BUILTIN_int,    BUILTIN_int},    0, &CodegenLIR::emitIntMathMax},

        // We do not inline the non-integral cases of min and max due to the complexity of handling -0 correctly.
        // See MathClass::_min() and MathClass::_max().

        // Unsupported because it uses MathClass::seed
        //{ avmplus::NativeID::Math_random,                  0, {BUILTIN_none,   BUILTIN_none},   FUNCTIONID(Math_random), 0},

#ifdef VMCFG_FLOAT
        // float
        { avmplus::NativeID::float_acos,                    1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_acos), 0},
        { avmplus::NativeID::float_asin,                    1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_asin), 0},
        { avmplus::NativeID::float_atan,                    1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_atan), 0},
        { avmplus::NativeID::float_ceil,                    1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_ceil), 0},
        { avmplus::NativeID::float_cos,                     1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_cos), 0},
        { avmplus::NativeID::float_exp,                     1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_exp), 0},
        { avmplus::NativeID::float_floor,                   1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_floor), 0},
        { avmplus::NativeID::float_log,                     1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_log), 0},
        { avmplus::NativeID::float_sin,                     1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_sin), 0},
        { avmplus::NativeID::float_tan,                     1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_tan), 0},
        
        { avmplus::NativeID::float_atan2,                   2, {BUILTIN_float, BUILTIN_float}, FUNCTIONID(float_atan2), 0},
        { avmplus::NativeID::float_pow,                     2, {BUILTIN_float, BUILTIN_float}, FUNCTIONID(float_pow), 0},

        // We would like to inline this on x86, but need SSE 4.1 to use ROUNDSS. A CVTSS2SI; CVTSI2SS sequence should work for SSE2,
        // but only with the correct rounding mode, which we cannot guarantee.  It would be possible to inline the MathUtils::roundf
        // function, which seems the best we can do.
        { avmplus::NativeID::float_round,                   1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_round), 0},
        // We would like to inline these on x86, but the MINSS and MAXSS instructions do not handle signed zero as required.
        { avmplus::NativeID::float_max,                     2, {BUILTIN_float, BUILTIN_float}, FUNCTIONID(float_max2), 0},
        { avmplus::NativeID::float_min,                     2, {BUILTIN_float, BUILTIN_float}, FUNCTIONID(float_min2), 0},

    #ifdef VMCFG_IA32
        { avmplus::NativeID::float_abs,                     1, {BUILTIN_float, BUILTIN_none},  0, &CodegenLIR::emitFloatAbs},
        { avmplus::NativeID::float_reciprocal,              1, {BUILTIN_float, BUILTIN_none},  0, &CodegenLIR::emitFloatReciprocal},
        { avmplus::NativeID::float_rsqrt,                   1, {BUILTIN_float, BUILTIN_none},  0, &CodegenLIR::emitFloatRsqrt},
        { avmplus::NativeID::float_sqrt,                    1, {BUILTIN_float, BUILTIN_none},  0, &CodegenLIR::emitFloatSqrt},
    #else
        { avmplus::NativeID::float_abs,                     1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_abs), 0},
        { avmplus::NativeID::float_reciprocal,              1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_reciprocal), 0},
        { avmplus::NativeID::float_rsqrt,                   1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_rsqrt), 0},
        { avmplus::NativeID::float_sqrt,                    1, {BUILTIN_float, BUILTIN_none},  FUNCTIONID(float_sqrt), 0},
    #endif

#endif
    };

    static uint32_t genFunctionKey (int32_t methodId, int32_t argCount)
    {
        return uint32_t(methodId | (argCount << 16));
    }

    // Our builtinFunctionOptimizerHashMap maps from a [nativeId, argCount] key into the
    // starting index in our specializedFunctions table.  The inlineBuiltinFunction function
    // will then iterate over our specializedFunctions table for all matching [nativeId, argCount]
    // pairs looking for a match to argument types.
    void CodegenLIR::genBuiltinFunctionOptimizerHashMap()
    {
        builtinFunctionOptimizerHashMap = new (*alloc1) HashMap<uint32_t, uint32_t>(*alloc1, 100);

        uint32_t funcKey = 0;
        for (uint32_t i = 0; i < sizeof(specializedFunctions) / sizeof(FunctionMatch); i++) {
            uint32_t newFuncKey = genFunctionKey (specializedFunctions[i].methodId, specializedFunctions[i].argCount);
            if (newFuncKey != funcKey) {
                funcKey = newFuncKey;
                builtinFunctionOptimizerHashMap->put(funcKey, i);
            }
        }
    }

    bool CodegenLIR::inlineBuiltinFunction(AbcOpcode, intptr_t, int argc, Traits* result, MethodInfo* mi)
    {
        if (haveDebugger)
            return false;

        if (mi->pool() != core->builtinPool || !mi->isFinal())
            return false;

        if (!builtinFunctionOptimizerHashMap)
            genBuiltinFunctionOptimizerHashMap();

        uint32_t funcKey = genFunctionKey(mi->method_id(), argc);
        uint32_t startingIndex = builtinFunctionOptimizerHashMap->get(funcKey);
        if (!startingIndex)
            return false;

        int count = sizeof(specializedFunctions) / sizeof(FunctionMatch);
        for (int i = startingIndex; i < count; i++) {
            if (mi->method_id() != specializedFunctions[i].methodId)
                return false;

            if (argc != (int) specializedFunctions[i].argCount)
                return false;

            // matching identifier, matching arg count, try to match argument types
            bool bMatch = true;
            for (int32_t argindex = 0; argindex < argc; argindex++) {
                int32_t btMask = determineBuiltinMaskForArg(state->sp() - argindex);
                if (!(btMask & (1 << specializedFunctions[i].argType[argindex]))) {
                    bMatch = false;
                    break;
                }
            }

            if (!bMatch)
                continue;

            if (specializedFunctions[i].newFunction) {
                int32_t sp = state->sp();
                // No 'this' param
                if (specializedFunctions[i].newFunction->count_args() == (uint32_t) argc) {
                    if (argc == 1)
                        localSet(sp-1, callIns(specializedFunctions[i].newFunction, 1,
                                               getSpecializedArg(sp, specializedFunctions[i].argType[0])), result);
                    else if (argc == 2)
                        localSet(sp-2, callIns(specializedFunctions[i].newFunction, 2,
                                               getSpecializedArg(sp-1, specializedFunctions[i].argType[1]),
                                               getSpecializedArg(sp, specializedFunctions[i].argType[0])), result);
                }
                else {
                    if (argc == 1)
                        localSet(sp-1, callIns(specializedFunctions[i].newFunction, 2, localGetp(sp-1),
                                               getSpecializedArg(sp, specializedFunctions[i].argType[0])), result);
                    else if (argc == 2)
                        localSet(sp-2, callIns(specializedFunctions[i].newFunction, 3, localGetp(sp-2),
                                               getSpecializedArg(sp-1, specializedFunctions[i].argType[1]),
                                               getSpecializedArg(sp, specializedFunctions[i].argType[0])), result);
                }
            }
            else {
                // Invoke emitFunction as member function pointer.
                EmitMethod emitter = specializedFunctions[i].emitFunction;
                (this->*emitter)(result);
            }

            return true;
        }

        return false;
    }

#ifdef DEBUG
    /**
     * emitTypedCall is used when the Verifier has found an opportunity to early bind,
     * and has already coerced arguments from whatever native type is discovered, to
     * the required types.  emitTypedCall() then just double-checks (via assert) that
     * the arg types are already correct.
     */
    void CodegenLIR::emitTypedCall(AbcOpcode opcode, intptr_t method_id, int argc, Traits* result, MethodInfo* mi)
    {
        AvmAssert(opcode != OP_construct);
        AvmAssert(state->value(state->sp() - argc).notNull); // make sure null check happened

        MethodSignaturep ms = mi->getMethodSignature();
        AvmAssert(ms->argcOk(argc));
        int objDisp = state->sp() - argc;
        for (int arg = 0; arg <= argc && arg <= ms->param_count(); arg++)
            AvmAssert(Traits::canAssign(state->value(objDisp+arg).traits, ms->paramTraits(arg)));
        for (int arg = ms->param_count()+1; arg <= argc; arg++) {
            BuiltinType t = bt(state->value(objDisp+arg).traits);
            AvmAssert(valueStorageType(t) == SST_atom);
        }

        if (inlineBuiltinFunction(opcode, method_id, argc, result, mi))
            return;

        emitCall(opcode, method_id, argc, result, ms);
    }
#else
    REALLY_INLINE void CodegenLIR::emitTypedCall(AbcOpcode opcode, intptr_t method_id, int argc, Traits* result, MethodInfo* mi)
    {
        if (inlineBuiltinFunction(opcode, method_id, argc, result, mi))
            return;

        emitCall(opcode, method_id, argc, result, mi->getMethodSignature());
    }
#endif

    void CodegenLIR::emitCall(AbcOpcode opcode, intptr_t method_id, int argc, Traits* result, MethodSignaturep ms)
    {
        int objDisp = state->sp() - argc;
        LIns* obj = localCopy(objDisp);
        Traits* objType = state->value(objDisp).traits;
        emitCall(opcode, method_id, argc, obj, objType, result, ms);
    }

    void CodegenLIR::emitCall(AbcOpcode opcode, intptr_t method_id, int argc, LIns* obj, Traits* objType, Traits* result, MethodSignaturep ms)
    {
        int sp = state->sp();
        int dest = sp-argc;
        int objDisp = dest;

        LIns *method = NULL;
        LIns *iid = NULL;
        switch (opcode)
        {
        case OP_constructsuper:
        {
            // env->vtable->base->init->enter32v(argc, ...);
            LIns* vtable = loadEnvVTable();
            LIns* base = loadIns(LIR_ldp, offsetof(VTable,base), vtable, ACCSET_OTHER, LOAD_CONST);
            method = loadIns(LIR_ldp, offsetof(VTable,init), base, ACCSET_OTHER, LOAD_CONST);
            break;
        }
        case OP_callmethod:
        {
            // stack in:  obj arg1..N
            // stack out: result
            // sp[-argc] = callmethod(disp_id, argc, ...);
            // method_id is disp_id of virtual method
            LIns* vtable = loadVTable(obj, objType);
            method = loadIns(LIR_ldp, int32_t(offsetof(VTable,methods)+sizeof(MethodEnv*)*method_id), vtable, ACCSET_OTHER, LOAD_CONST);
            break;
        }
        case OP_callsuperid:
        {
            // stack in: obj arg1..N
            // stack out: result
            // method_id is disp_id of super method
            LIns* declvtable = loadEnvVTable();
            LIns* basevtable = loadIns(LIR_ldp, offsetof(VTable, base), declvtable, ACCSET_OTHER, LOAD_CONST);
            method = loadIns(LIR_ldp, int32_t(offsetof(VTable,methods)+sizeof(MethodEnv*)*method_id), basevtable, ACCSET_OTHER, LOAD_CONST);
            break;
        }
        case OP_callstatic:
        {
            // stack in: obj arg1..N
            // stack out: result
            LIns* abcenv = loadEnvAbcEnv();
            method = loadIns(LIR_ldp, int32_t(offsetof(AbcEnv,m_methods)+sizeof(MethodEnv*)*method_id), abcenv, ACCSET_OTHER, LOAD_CONST);
            break;
        }
        case OP_callinterface:
        {
            // method_id is pointer to interface method name (multiname)
            uint32_t index = ImtHolder::hashIID(method_id);
            LIns* vtable = loadVTable(obj, objType);
            // note, could be MethodEnv* or ImtThunkEnv*
            method = loadIns(LIR_ldp, offsetof(VTable, imt.entries) + index * sizeof(ImtThunkEnv*), vtable, ACCSET_OTHER, LOAD_CONST);
            iid = InsConstPtr((void*)method_id);
            break;
        }
        case OP_construct:
        {
            // stack in: ctor arg1..N
            // stack out: newinstance
            LIns* vtable = loadVTable(obj, objType);
            LIns* ivtable = loadIns(LIR_ldp, offsetof(VTable, ivtable), vtable, ACCSET_OTHER, LOAD_CONST);
            method = loadIns(LIR_ldp, offsetof(VTable, init), ivtable, ACCSET_OTHER, LOAD_CONST);
            LIns* createInstanceProc = loadIns(LIR_ldp, offsetof(VTable, createInstanceProc), ivtable, ACCSET_OTHER);
            obj = callIns(FUNCTIONID(createInstanceProc), 2, createInstanceProc, obj);
            objType = result;
            // the call below to the init function is void; the expression result we want
            // is the new object, not the result from the init function.  save it now.
            localSet(dest, obj, result);
            break;
        }
        default:
            AvmAssert(false);
        }

        // store args for the call
        LIns* ap = insAlloc(sizeof(Atom)); // we will update this size, below
        int disp = 0;
        int pad = 0;

        int param_count = ms->param_count();
        // LIR_allocp of any size >= 8 is always 8-aligned.
        // if the first double arg would be unaligned, add padding to align it.
    #if !defined AVMPLUS_64BIT
        for (int i=0; i <= argc && i <= param_count; i++) {
            if (ms->paramTraits(i) == NUMBER_TYPE) {
                if ((disp&7) != 0) {
                    // this double would be unaligned, so add some padding
                    pad = 8-(disp&7); // should be 4
                }
                break;
            }
            else {
                disp += sizeof(Atom);
            }
        }
    #endif

        disp = pad;
        for (int i=0, index=objDisp; i <= argc; i++, index++) {
            Traits* paramType = i <= param_count ? ms->paramTraits(i) : NULL;
            LIns* v;
            switch (bt(paramType)) {
#ifdef VMCFG_FLOAT
            case BUILTIN_float: 
                v = (i == 0) ? obj : lirout->insLoad(LIR_ldf, vars, index << VARSHIFT(info), ACCSET_VARS);
                stf(v, ap, disp, ACCSET_OTHER);
                disp += sizeof(intptr_t); // only 4 bytes needed, but on X64 we use at least 8 bytes anyway - same as "int" and "uint"
                break;
            case BUILTIN_float4: 
                v = (i == 0) ? obj : lirout->insLoad(LIR_ldf4, vars, index << VARSHIFT(info), ACCSET_VARS);
                stf4(v, ap, disp, ACCSET_OTHER);
                disp += sizeof(float4_t); 
                break;
#endif
            case BUILTIN_number:
                v = (i == 0) ? obj : lirout->insLoad(LIR_ldd, vars, index << VARSHIFT(info), ACCSET_VARS);
                std(v, ap, disp, ACCSET_OTHER);
                disp += sizeof(double);
                break;
            case BUILTIN_int:
                v = (i == 0) ? obj : lirout->insLoad(LIR_ldi, vars, index << VARSHIFT(info), ACCSET_VARS);
                stp(i2p(v), ap, disp, ACCSET_OTHER);
                disp += sizeof(intptr_t);
                break;
            case BUILTIN_uint:
            case BUILTIN_boolean:
                v = (i == 0) ? obj : lirout->insLoad(LIR_ldi, vars, index << VARSHIFT(info), ACCSET_VARS);
                stp(ui2p(v), ap, disp, ACCSET_OTHER);
                disp += sizeof(uintptr_t);
                break;
            default:
                v = (i == 0) ? obj : lirout->insLoad(LIR_ldp, vars, index << VARSHIFT(info), ACCSET_VARS);
                stp(v, ap, disp, ACCSET_OTHER);
                disp += sizeof(void*);
                break;
            }
        }

        // patch the size to what we actually need
        ap->setSize(disp);

        LIns* target = loadIns(LIR_ldp, offsetof(MethodEnvProcHolder,_implGPR), method, ACCSET_OTHER);
        LIns* apAddr = lea(pad, ap);

        LIns *out;
        BuiltinType rbt = bt(result);
        if (!iid) {
            const CallInfo *fid;
            switch (rbt) {
#ifdef VMCFG_FLOAT
            case BUILTIN_float: 
                fid = FUNCTIONID(fcalli);
                break;
            case BUILTIN_float4: 
                fid = FUNCTIONID(vfcalli);
                break;
#endif
            case BUILTIN_number:
                fid = FUNCTIONID(dcalli);
                break;
            case BUILTIN_int: case BUILTIN_uint: case BUILTIN_boolean:
                fid = FUNCTIONID(icalli);
                break;
            default:
                fid = FUNCTIONID(acalli);
                break;
            }
            out = callIns(fid, 4, target, method, InsConst(argc), apAddr);
        } else {
            const CallInfo *fid;
            switch (rbt) {
#ifdef VMCFG_FLOAT
            case BUILTIN_float: 
                fid = FUNCTIONID(fcallimt);
                break;
            case BUILTIN_float4: 
                fid = FUNCTIONID(vfcallimt);
                break;
#endif
            case BUILTIN_number:
                fid = FUNCTIONID(dcallimt);
                break;
            case BUILTIN_int: case BUILTIN_uint: case BUILTIN_boolean:
                fid = FUNCTIONID(icallimt);
                break;
            default:
                fid = FUNCTIONID(acallimt);
                break;
            }
            out = callIns(fid, 5, target, method, InsConst(argc), apAddr, iid);
        }

        // ensure the stack-allocated args are live until after the call
        liveAlloc(ap);

        if (opcode != OP_constructsuper && opcode != OP_construct)
            localSet(dest, out, result);
    }

    LIns* CodegenLIR::loadFromSlot(int ptr_index, int slot, Traits* slotType)
    {
        Traits *t = state->value(ptr_index).traits;
        LIns *ptr = localGetp(ptr_index);
        AvmAssert(state->value(ptr_index).notNull);
        AvmAssert(isPointer((int)ptr_index)); // obj

        AvmAssert(t->isResolved());
        const TraitsBindingsp tb = t->getTraitsBindings();
        int offset = tb->getSlotOffset(slot);

        // get
        LOpcode op;
        switch (bt(slotType)) {
#ifdef VMCFG_FLOAT
        case BUILTIN_float4:    op = LIR_ldf4;   break;
        case BUILTIN_float:     op = LIR_ldf;   break;
#endif
        case BUILTIN_number:    op = LIR_ldd;   break;
        case BUILTIN_int:
        case BUILTIN_uint:
        case BUILTIN_boolean:   op = LIR_ldi;    break;
        default:                op = LIR_ldp;   break;
        }
        return loadIns(op, offset, ptr, ACCSET_OTHER);
    }

    void CodegenLIR::emitGetslot(int slot, int ptr_index, Traits *slotType)
    {
        localSet(ptr_index, loadFromSlot(ptr_index, slot, slotType), slotType);
    }

    void CodegenLIR::emitSetslot(AbcOpcode opcode, int slot, int ptr_index)
    {
        emitSetslot(opcode, slot, ptr_index, localCopy(state->sp()));
    }

    void CodegenLIR::emitSetslot(AbcOpcode opcode, int slot, int ptr_index, LIns* value)
    {
        Traits* t;
        LIns* ptr;

        if (opcode == OP_setslot)
        {
            t = state->value(ptr_index).traits;
            ptr = localGetp(ptr_index);
            AvmAssert(state->value(ptr_index).notNull);
            AvmAssert(isPointer((int)ptr_index)); // obj
        }
        else
        {
            // setglobalslot
            const ScopeTypeChain* scopeTypes = info->declaringScope();
            if (scopeTypes->size == 0)
            {
                // no captured scopes, so global is local scope 0
                ptr_index = ms->scope_base();
                t = state->value(ptr_index).traits;
                ptr = localGetp(ptr_index);
                AvmAssert(state->value(ptr_index).notNull);
                AvmAssert(isPointer((int)ptr_index)); // obj
            }
            else
            {
                // global is outer scope 0
                t = scopeTypes->getScopeTraitsAt(0);
                LIns* scope = loadEnvScope();
                LIns* scopeobj = loadIns(LIR_ldp, offsetof(ScopeChain,_scopes) + 0*sizeof(Atom), scope, ACCSET_OTHER);
                ptr = atomToNativeRep(t, scopeobj);
            }
        }

        AvmAssert(t->isResolved());
        const TraitsBindingsp tb = t->getTraitsBindings();
        int offset = tb->getSlotOffset(slot);

        LIns *unoffsetPtr = ptr;

        // if storing to a pointer-typed slot, inline a WB
        Traits* slotType = tb->getSlotTraits(slot);
        if (!slotType || !slotType->isMachineType() || slotType == OBJECT_TYPE)
        {
            // slot type is Atom (for *, Object) or RCObject* (String, Namespace, or other user types)
            const CallInfo *wbAddr = FUNCTIONID(privateWriteBarrierRC);
            if (slotType == NULL ||  slotType == OBJECT_TYPE) {
                // use fast atom wb
                wbAddr = FUNCTIONID(atomWriteBarrier);
            }
            callIns(wbAddr, 4,
                    InsConstPtr(core->GetGC()),
                    unoffsetPtr,
                    lea(offset, ptr),
                    value);
        }
#ifdef VMCFG_FLOAT
        else if (slotType == FLOAT_TYPE) {
            // slot type is double or int
            stf(value, ptr, offset, ACCSET_OTHER);
        }
        else if (slotType == FLOAT4_TYPE) {
            // slot type is double or int
            stf4(value, ptr, offset, ACCSET_OTHER);
        }
#endif
        else if (slotType == NUMBER_TYPE) {
            // slot type is double or int
            std(value, ptr, offset, ACCSET_OTHER);
        } else {
            AvmAssert(slotType == INT_TYPE || slotType == UINT_TYPE || slotType == BOOLEAN_TYPE);
            sti(value, ptr, offset, ACCSET_OTHER);
        }
    }

    /**
     * Emit a constructor call, or a late bound constructor call.
     * Early binding is possible when we know the constructor (class) being
     * used, and we know it doesn't override ClassClosure::construct(),
     * as indicated by the itraits->hasCustomConstruct flag.
     */
    void CodegenLIR::emitConstruct(int argc, LIns* ctor, Traits* ctraits)
    {
        // Attempt to early bind to constructor method.
        Traits* itraits = NULL;
        if (ctraits && (itraits = ctraits->itraits) != NULL &&
                !ctraits->hasCustomConstruct) {
            // Inline the body of ClassClosure::construct() and early bind the call
            // to the constructor method, if it's resolved and argc is legal.
            // Cannot resolve signatures now because that could cause a premature verification failure,
            // one that should occur in the class's script-init.
            // If it's already resolved then we're good to go.
            if (itraits->init && itraits->init->isResolved() && itraits->init->getMethodSignature()->argcOk(argc)) {
                // The explicit null check will throw a different exception than
                // the generic call to op_construct below, or to similar paths through
                // interpreted code!
                emitCheckNull(ctor, ctraits);
                emitConstructCall(0, argc, ctor, ctraits);
                return;
            }
        }

        // Generic path: could not early bind to a constructor method.
        // stack in: ctor-object arg1..N
        // sp[-argc] = construct(env, sp[-argc], argc, null, arg1..N)
        int ctor_index = state->sp() - argc;
        LIns* func = nativeToAtom(ctor, ctraits);
        LIns* args = storeAtomArgs(InsConstAtom(nullObjectAtom), argc, ctor_index+1);
        LIns* newobj = callIns(FUNCTIONID(op_construct), 4, env_param, func, InsConst(argc), args);
        liveAlloc(args);
        localSet(ctor_index, atomToNativeRep(itraits, newobj), itraits);
    }

    static const CallInfo* getArrayHelpers[VI_SIZE] =
        { FUNCTIONID(ArrayObject_getUintProperty), FUNCTIONID(ArrayObject_getIntProperty), FUNCTIONID(ArrayObject_getDoubleProperty) };

    static const CallInfo* getObjectVectorHelpers[VI_SIZE] =
        { FUNCTIONID(ObjectVectorObject_getUintProperty), FUNCTIONID(ObjectVectorObject_getIntProperty), FUNCTIONID(ObjectVectorObject_getDoubleProperty) };

    static const CallInfo* getIntVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(IntVectorObject_getNativeUintProperty), FUNCTIONID(IntVectorObject_getNativeIntProperty), FUNCTIONID(IntVectorObject_getNativeDoubleProperty) };

    static const CallInfo* getIntVectorHelpers[VI_SIZE] =
        { FUNCTIONID(IntVectorObject_getUintProperty), FUNCTIONID(IntVectorObject_getIntProperty), FUNCTIONID(IntVectorObject_getDoubleProperty) };

    static const CallInfo* getUIntVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(UIntVectorObject_getNativeUintProperty), FUNCTIONID(UIntVectorObject_getNativeIntProperty), FUNCTIONID(UIntVectorObject_getNativeDoubleProperty) };

    static const CallInfo* getUIntVectorHelpers[VI_SIZE] =
        { FUNCTIONID(UIntVectorObject_getUintProperty), FUNCTIONID(UIntVectorObject_getIntProperty), FUNCTIONID(UIntVectorObject_getDoubleProperty) };

    static const CallInfo* getDoubleVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(DoubleVectorObject_getNativeUintProperty), FUNCTIONID(DoubleVectorObject_getNativeIntProperty), FUNCTIONID(DoubleVectorObject_getNativeDoubleProperty) };

    static const CallInfo* getDoubleVectorHelpers[VI_SIZE] =
        { FUNCTIONID(DoubleVectorObject_getUintProperty), FUNCTIONID(DoubleVectorObject_getIntProperty), FUNCTIONID(DoubleVectorObject_getDoubleProperty) };

#ifdef VMCFG_FLOAT
    static const CallInfo* getFloatVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(FloatVectorObject_getNativeUintProperty), FUNCTIONID(FloatVectorObject_getNativeIntProperty), FUNCTIONID(FloatVectorObject_getNativeDoubleProperty) };

    static const CallInfo* getFloatVectorHelpers[VI_SIZE] =
        { FUNCTIONID(FloatVectorObject_getUintProperty), FUNCTIONID(FloatVectorObject_getIntProperty), FUNCTIONID(FloatVectorObject_getDoubleProperty) };

    static const CallInfo* getFloat4VectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(Float4VectorObject_getNativeUintProperty), FUNCTIONID(Float4VectorObject_getNativeIntProperty), FUNCTIONID(Float4VectorObject_getNativeDoubleProperty) };
    
    static const CallInfo* getFloat4VectorHelpers[VI_SIZE] =
        { FUNCTIONID(Float4VectorObject_getUintProperty), FUNCTIONID(Float4VectorObject_getIntProperty), FUNCTIONID(Float4VectorObject_getDoubleProperty) };
#endif

    static const CallInfo* getGenericHelpers[VI_SIZE] =
        { FUNCTIONID(getpropertylate_u), FUNCTIONID(getpropertylate_i), FUNCTIONID(getpropertylate_d) };

    LIns* CodegenLIR::emitInlineVectorRead(int objIndexOnStack, 
                                           LIns* index,
                                           size_t arrayDataOffset, size_t lenOffset, size_t entriesOffset, int scale, LOpcode load_item,
                                           const CallInfo* helper)
    {
        CodegenLabel &begin_label = createLabel("arrayoutofbounds");

        // index is "int" or "uint".
        //
        // The basic code is this:
        //
        // arrayData = *(array+arrayDataOffset)                  ; array->data
        // arrayLen  = *(arrayData + lenOffset)                  ; array->data->len
        // if ((unsigned)index >= (unsigned)arrayLen)
        //   call(helper, array, index)
        // value = *(arrayData + (index<<scale) + entriesOffset) ; value = array->data->entries[index]
        //
        // In the case of float4 the (arrayData + entriesOffset) has to be rounded up to a 16-byte
        // boundary at run-time to conform to alignment restrictions.
        // It would be better to have a different vector representation and a guarantee about
        // alignment from the storage manager, so that we could avoid the adjustment, but that's
        // a future improvement.
        //
        // The "(unsigned)index" compare trick works because vectors are shorter than 2^31 elements,
        // that fact is verified statically in the vector code, see eg checkReadIndex_i.
        //
        // We also depend on vectors being shorter than 2^32 bytes in computing the scaled offset for
        // the vector.  The 4GB object limit is enforced by MMgc as of September 2011.

        LIns* arrayData = loadIns(LIR_ldp, int32_t(arrayDataOffset), localGetp(objIndexOnStack), ACCSET_OTHER, LOAD_NORMAL);
        LIns* arrayLen = loadIns(LIR_ldi, int32_t(lenOffset), arrayData, ACCSET_OTHER, LOAD_NORMAL);
        LIns* cmp = binaryIns(LIR_geui, index, arrayLen);
        suspendCSE();
        branchToLabel(LIR_jf, cmp, begin_label);
        if(load_item == LIR_ldf4)
            callIns(helper, 3, localGetp(objIndexOnStack), InsConstPtr(NULL), index);  // we don't really need the returned arg, hence we pass null.
        else
            callIns(helper, 2, localGetp(objIndexOnStack), index);
        emitLabel(begin_label);
        resumeCSE();
        switch (load_item)
        {
            case LIR_ldf4:
            {
                LIns* base = binaryIns(LIR_addp, arrayData, InsConstPtr((const void*)(uintptr_t(entriesOffset) + 15))); // arrayData + entries + 15
                LIns* baseRounded = binaryIns(LIR_andp, base, InsConstPtr((const void*)~uintptr_t(15)));                // & ~15
                LIns* element = ui2p(binaryIns(LIR_lshi, index, InsConst(scale)));
                LIns* effectiveAddress = binaryIns(LIR_addp, baseRounded, element);
                return loadIns(load_item, 0, effectiveAddress, ACCSET_OTHER, LOAD_NORMAL);
            }
#ifdef VMCFG_64BIT
            case LIR_ldp:
#endif
            case LIR_ldd:
            case LIR_ldi:
            case LIR_ldf:
            {
                LIns* valOffset = binaryIns(LIR_addp, arrayData, ui2p(binaryIns(LIR_lshi, index, InsConst(scale))));
                return loadIns(load_item, int32_t(entriesOffset), valOffset, ACCSET_OTHER, LOAD_NORMAL);
            }
            default:
                AvmAssert(!"Bad inline vector access");
                return NULL;
        }
    }

    LIns* CodegenLIR::emitInlineSpeculativeArrayRead(int objIndexOnStack,
                                                     LIns* index,
                                                     const CallInfo* helper)
    {
        CodegenLabel &nonsimpleBeginLabel = createLabel("nonsimplearray");
        CodegenLabel &joinLabel = createLabel("joinarraygetprop");

        // index is "int" or "uint"

        // Emitted speculative fast path for simple-marked array:
        //  ``return ((unsigned)index < m_lengthIfSimple) ?
        //            m_denseArray.get(index) : getUintProperty(index));''

        size_t lenIfSimpleOffset = offsetof(ArrayObject, m_lengthIfSimple);
        size_t denseArrayOffset = offsetof(ArrayObject, m_denseArray);
        typedef ListData<Atom, 0> LISTDATA;
        size_t entriesOffset = offsetof(LISTDATA, entries);
        int scale = (sizeof(void*) == 8) ? 3 : 2;

        LIns* arrayPtr = localGetp(objIndexOnStack);
        LIns* arraySimpleLen =
            loadIns(LIR_ldi, int32_t(lenIfSimpleOffset), arrayPtr,
                    ACCSET_OTHER, LOAD_NORMAL);
        LIns* cmp = binaryIns(LIR_geui, index, arraySimpleLen);

        suspendCSE();
        LIns* result = insAlloc(sizeof(Atom));
        branchToLabel(LIR_jt, cmp, nonsimpleBeginLabel);

        LIns* arrayData = loadIns(LIR_ldp, int32_t(denseArrayOffset), arrayPtr,
                                  ACCSET_OTHER, LOAD_NORMAL );
        LIns* idxScaled = ui2p(binaryIns(LIR_lshi, index, InsConst(scale)));
        LIns* valOffset = binaryIns(LIR_addp, arrayData, idxScaled);
        LIns* loadValue = loadIns(LIR_ldp, int32_t(entriesOffset), valOffset,
                                  ACCSET_OTHER, LOAD_NORMAL);
        stp(loadValue, result, 0, ACCSET_OTHER);
        branchToLabel(LIR_j, NULL, joinLabel);

        emitLabel(nonsimpleBeginLabel);
        LIns* callValue = callIns(helper, 2, localGetp(objIndexOnStack), index);
        stp(callValue, result, 0, ACCSET_OTHER);

        emitLabel(joinLabel);
        resumeCSE();

        return ldp(result, 0, ACCSET_OTHER);
    }

    // Generate code for get obj[index] where index is a signed or unsigned integer type, or double type.
    LIns* CodegenLIR::emitGetIndexedProperty(int objIndexOnStack, LIns* index, Traits* result, IndexKind idxKind)
    {
        Traits* objType = state->value(objIndexOnStack).traits;
        const CallInfo* getter = NULL;
        bool valIsAtom = true;

        if (objType == ARRAY_TYPE) {
            getter = getArrayHelpers[idxKind];
            if (idxKind == VI_INT || idxKind == VI_UINT) {
                LIns* value = emitInlineSpeculativeArrayRead(objIndexOnStack,
                                                             index,
                                                             getter);
                return atomToNativeRep(result, value);
            }
        }
        else if (objType != NULL && objType->subtypeof(VECTOROBJ_TYPE)) {
            if (idxKind == VI_INT || idxKind == VI_UINT) {
                typedef ListData<Atom, 0> STORAGE;
                return atomToNativeRep(result, emitInlineVectorRead(objIndexOnStack, 
                                                                    index,
                                                                    offsetof(ObjectVectorObject, m_list.m_data),
                                                                    offsetof(AtomListHelper::LISTDATA, len),
                                                                    offsetof(STORAGE, entries),
                                                                    sizeof(void*) == 8 ? 3 : 2,
                                                                    LIR_ldp,
                                                                    getObjectVectorHelpers[idxKind]));
            }
            getter = getObjectVectorHelpers[idxKind];
        }
        else if (objType == VECTORINT_TYPE) {
            if (result == INT_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<int32_t, 0> STORAGE;
                    return emitInlineVectorRead(objIndexOnStack, 
                                                index,
                                                offsetof(IntVectorObject, m_list.m_data),
                                                offsetof(DataListHelper<int32_t>::LISTDATA, len),
                                                offsetof(STORAGE, entries),
                                                2,
                                                LIR_ldi,
                                                getIntVectorNativeHelpers[idxKind]);
                }
                getter = getIntVectorNativeHelpers[idxKind];
                valIsAtom = false;
            }
            else {
                getter = getIntVectorHelpers[idxKind];
            }
        }
        else if (objType == VECTORUINT_TYPE) {
            if (result == UINT_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<uint32_t, 0> STORAGE;
                    return emitInlineVectorRead(objIndexOnStack, 
                                                index,
                                                offsetof(UIntVectorObject, m_list.m_data),
                                                offsetof(DataListHelper<uint32_t>::LISTDATA, len),
                                                offsetof(STORAGE, entries),
                                                2,
                                                LIR_ldi,
                                                getUIntVectorNativeHelpers[idxKind]);
                }
                getter = getUIntVectorNativeHelpers[idxKind];
                valIsAtom = false;
            }
            else {
                getter = getUIntVectorHelpers[idxKind];
            }
        }
        else if (objType == VECTORDOUBLE_TYPE) {
            if (result == NUMBER_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<double, 0> STORAGE;
                    return emitInlineVectorRead(objIndexOnStack, 
                                                index,
                                                offsetof(DoubleVectorObject, m_list.m_data),
                                                offsetof(DataListHelper<double>::LISTDATA, len),
                                                offsetof(STORAGE, entries),
                                                3,
                                                LIR_ldd,
                                                getDoubleVectorNativeHelpers[idxKind]);
                }
                getter = getDoubleVectorNativeHelpers[idxKind];
                valIsAtom = false;
            }
            else {
                getter = getDoubleVectorHelpers[idxKind];
            }
        }
#ifdef VMCFG_FLOAT
        else if (objType == VECTORFLOAT_TYPE) {
            if (result == FLOAT_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<float, 0> STORAGE;
                    return emitInlineVectorRead(objIndexOnStack, 
                                                index,
                                                offsetof(FloatVectorObject, m_list.m_data),
                                                offsetof(DataListHelper<float>::LISTDATA, len),
                                                offsetof(STORAGE, entries),
                                                2,
                                                LIR_ldf,
                                                getFloatVectorNativeHelpers[idxKind]);
                }
                getter = getFloatVectorNativeHelpers[idxKind];
                valIsAtom = false;
            }
            else {
                getter = getFloatVectorHelpers[idxKind];
            }
        }
        else if (objType == VECTORFLOAT4_TYPE) {
            if (result == FLOAT4_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<float4_t, 16> STORAGE;
                    typedef DataListHelper<float4_t, 16>::LISTDATA LISTDATA;
                    return emitInlineVectorRead(objIndexOnStack, 
                                                index,
                                                offsetof(Float4VectorObject, m_list.m_data),
                                                offsetof(LISTDATA, len),
                                                offsetof(STORAGE, entries),
                                                4,
                                                LIR_ldf4,
                                                getFloat4VectorNativeHelpers[idxKind]);
                }
                /* handle here the case of native helpers, it is different than all the others */
                LIns* retval = lirout->insAlloc(sizeof(float4_t));
                callIns(getFloat4VectorNativeHelpers[idxKind], 3, localGetp(objIndexOnStack), lea(0, retval), index);
                return ldf4(retval, 0, ACCSET_OTHER);
            }
            else {
                getter = getFloat4VectorHelpers[idxKind];
            }
        }
#endif

        if (getter) {
            LIns* value = callIns(getter, 2, localGetp(objIndexOnStack), index);
            return valIsAtom ? atomToNativeRep(result, value) : value;
        }
        else {
            LIns* value = callIns(getGenericHelpers[idxKind], 3, env_param, loadAtomRep(objIndexOnStack), index);
            return atomToNativeRep(result, value);
        }
    }

    static const CallInfo* setArrayHelpers[VI_SIZE] =
        { FUNCTIONID(ArrayObject_setUintProperty), FUNCTIONID(ArrayObject_setIntProperty), FUNCTIONID(ArrayObject_setDoubleProperty) };

    static const CallInfo* setObjectVectorHelpers[VI_SIZE] =
        { FUNCTIONID(ObjectVectorObject_setUintProperty), FUNCTIONID(ObjectVectorObject_setIntProperty), FUNCTIONID(ObjectVectorObject_setDoubleProperty) };

    static const CallInfo* setKnownTypeObjectVectorHelpers[VI_SIZE] =
        { FUNCTIONID(ObjectVectorObject_setKnownUintProperty), FUNCTIONID(ObjectVectorObject_setKnownIntProperty), FUNCTIONID(ObjectVectorObject_setKnownDoubleProperty) };
    
    static const CallInfo* setKnownPointerTypeObjectVectorHelpers[VI_SIZE] =
        { FUNCTIONID(ObjectVectorObject_setKnownUintPropertyWithPointer), FUNCTIONID(ObjectVectorObject_setKnownIntPropertyWithPointer), FUNCTIONID(ObjectVectorObject_setKnownDoublePropertyWithPointer) };
    
    static const CallInfo* setIntVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(IntVectorObject_setNativeUintProperty), FUNCTIONID(IntVectorObject_setNativeIntProperty), FUNCTIONID(IntVectorObject_setNativeDoubleProperty) };

    static const CallInfo* setIntVectorHelpers[VI_SIZE] =
        { FUNCTIONID(IntVectorObject_setUintProperty), FUNCTIONID(IntVectorObject_setIntProperty), FUNCTIONID(IntVectorObject_setDoubleProperty) };

    static const CallInfo* setUIntVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(UIntVectorObject_setNativeUintProperty), FUNCTIONID(UIntVectorObject_setNativeIntProperty), FUNCTIONID(UIntVectorObject_setNativeDoubleProperty) };

    static const CallInfo* setUIntVectorHelpers[VI_SIZE] =
        { FUNCTIONID(UIntVectorObject_setUintProperty), FUNCTIONID(UIntVectorObject_setIntProperty), FUNCTIONID(UIntVectorObject_setDoubleProperty) };

    static const CallInfo* setDoubleVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(DoubleVectorObject_setNativeUintProperty), FUNCTIONID(DoubleVectorObject_setNativeIntProperty), FUNCTIONID(DoubleVectorObject_setNativeDoubleProperty) };

    static const CallInfo* setDoubleVectorHelpers[VI_SIZE] =
        { FUNCTIONID(DoubleVectorObject_setUintProperty), FUNCTIONID(DoubleVectorObject_setIntProperty), FUNCTIONID(DoubleVectorObject_setDoubleProperty) };

#ifdef VMCFG_FLOAT
    static const CallInfo* setFloatVectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(FloatVectorObject_setNativeUintProperty), FUNCTIONID(FloatVectorObject_setNativeIntProperty), FUNCTIONID(FloatVectorObject_setNativeDoubleProperty) };

    static const CallInfo* setFloatVectorHelpers[VI_SIZE] =
        { FUNCTIONID(FloatVectorObject_setUintProperty), FUNCTIONID(FloatVectorObject_setIntProperty), FUNCTIONID(FloatVectorObject_setDoubleProperty) };

    static const CallInfo* setFloat4VectorNativeHelpers[VI_SIZE] =
        { FUNCTIONID(Float4VectorObject_setNativeUintProperty), FUNCTIONID(Float4VectorObject_setNativeIntProperty), FUNCTIONID(Float4VectorObject_setNativeDoubleProperty) };
    
    static const CallInfo* setFloat4VectorHelpers[VI_SIZE] =
        { FUNCTIONID(Float4VectorObject_setUintProperty), FUNCTIONID(Float4VectorObject_setIntProperty), FUNCTIONID(Float4VectorObject_setDoubleProperty) };
#endif

    static const CallInfo* setGenericHelpers[VI_SIZE] =
        { FUNCTIONID(setpropertylate_u), FUNCTIONID(setpropertylate_i), FUNCTIONID(setpropertylate_d) };


    // Not for values that require a write barrier!
    void CodegenLIR::emitInlineVectorWrite(int objIndexOnStack,
                                           LIns* index,
                                           LIns* value,
                                           size_t arrayDataOffset, size_t lenOffset, size_t entriesOffset, int scale, LOpcode store_item,
                                           const CallInfo* helper)
    {
        CodegenLabel &begin_label = createLabel("arrayoutofbounds");
        CodegenLabel &end_label = createLabel("traprecovered");
        
        // index is "int" or "uint".
        //
        // The basic  code is this:
        //
        // arrayData = *(array+arrayDataOffset)                    ; array->data
        // arrayLen  = *(arrayData + lenOffset)                    ; array->data->len
        // if ((unsigned)index >= (unsigned)arrayLen)
        //   call(helper, array, index, value)
        // else
        //   *(arrayData + (index<<scale) + entriesOffset) = value ; array->data->entries[index] = value
        //
        // In the case of float4 the (arrayData + entriesOffset) has to be rounded up to a 16-byte
        // boundary at run-time to conform to alignment restrictions.
        // It would be better to have a different vector representation and a guarantee about
        // alignment from the storage manager, so that we could avoid the adjustment, but that's
        // a future improvement.
        //
        // The "(unsigned)index" compare trick works because vectors are shorter than 2^31 elements,
        // that fact is verified statically in the vector code, see eg checkReadIndex_i.
        //
        // We also depend on vectors being shorter than 2^32 bytes in computing the scaled offset for
        // the vector.  The 4GB object limit is enforced by MMgc as of September 2011.

        LIns* arrayData = loadIns(LIR_ldp, int32_t(arrayDataOffset), localGetp(objIndexOnStack), ACCSET_OTHER, LOAD_NORMAL);
        LIns* arrayLen = loadIns(LIR_ldi, int32_t(lenOffset), arrayData, ACCSET_OTHER, LOAD_NORMAL);
        LIns* cmp = binaryIns(LIR_geui, index, arrayLen);
        suspendCSE();
        branchToLabel(LIR_jf, cmp, begin_label);
        callIns(helper, 3, localGetp(objIndexOnStack), index, value);
        branchToLabel(LIR_j, NULL, end_label);
        emitLabel(begin_label);
        switch (store_item)
        {
#ifdef VMCFG_FLOAT
            case LIR_stf4:
            {
                LIns* base = binaryIns(LIR_addp, arrayData, InsConstPtr((const void*)(uintptr_t(entriesOffset) + 15))); // arrayData + entries + 15
                LIns* baseRounded = binaryIns(LIR_andp, base, InsConstPtr((const void*)~uintptr_t(15)));                // & ~15
                LIns* element = ui2p(binaryIns(LIR_lshi, index, InsConst(scale)));
                LIns* effectiveAddress = binaryIns(LIR_addp, baseRounded, element);
                
                if( (value->opcode() == LIR_addp) && (value->oprnd1() == vars) ){
                    AvmAssert(value->oprnd2()->isImmP());
                    int32_t v_offset = (int32_t) (intptr_t) value->oprnd2()->immP();
                    value = ldf4(vars, v_offset, ACCSET_VARS);
                } else {
                    value = ldf4(value, 0 , ACCSET_OTHER);
                }

                storeIns(store_item, value, 0, effectiveAddress, ACCSET_OTHER);
                break;
            }
#endif
#ifdef VMCFG_64BIT
            case LIR_stp:
#endif
            case LIR_std:
            case LIR_sti:
            case LIR_stf:
            {
                LIns *valOffset = binaryIns(LIR_addp, arrayData, ui2p(binaryIns(LIR_lshi, index, InsConst(scale))));
                storeIns(store_item, value, int32_t(entriesOffset), valOffset, ACCSET_OTHER);
                break;
            }
            default:
                AvmAssert(!"Bad inline vector access");
        }
        emitLabel(end_label);
        resumeCSE();
    }

    // Generate code for 'obj[index] = value' where index is a signed or unsigned integer type, or a double.
    void CodegenLIR::emitSetIndexedProperty(int objIndexOnStack, int valIndexOnStack, LIns* index, IndexKind idxKind)
    {
        Traits* valueType = state->value(valIndexOnStack).traits;
        Traits* objType = state->value(objIndexOnStack).traits;
        const CallInfo* setter = NULL;
        LIns* value = NULL;

        if (objType == ARRAY_TYPE) {
            value = loadAtomRep(valIndexOnStack);
            setter = setArrayHelpers[idxKind];
        }
        else if (objType != NULL && objType->subtypeof(VECTOROBJ_TYPE)) {
            value = loadAtomRep(valIndexOnStack);
            // Optimization: avoid coercion if the stored value is known to be a subtype
            // of the vector parameter type.
            Traits* paramType = objType->m_paramTraits;
            if (paramType != NULL && valueType != NULL && valueType->subtypeof(paramType)) {
                // Optimization: avoid atomWriteBarrier if the vector parameter type is
                // known to be an RCObject; atomWriteBarrier has a bunch of run-time tests
                // that try to determine that fact.
                if (isRCObjectSlot(valueStorageType(Traits::getBuiltinType(paramType))))
                    setter = setKnownPointerTypeObjectVectorHelpers[idxKind];
                else
                    setter = setKnownTypeObjectVectorHelpers[idxKind];
            }
            else
                setter = setObjectVectorHelpers[idxKind];
        }
        else if (objType == VECTORINT_TYPE) {
            if (valueType == INT_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<int32_t, 0> STORAGE;
                    emitInlineVectorWrite(objIndexOnStack, 
                                          index,
                                          localGet(valIndexOnStack),
                                          offsetof(IntVectorObject, m_list.m_data),
                                          offsetof(DataListHelper<int32_t>::LISTDATA, len),
                                          offsetof(STORAGE, entries),
                                          2,
                                          LIR_sti,
                                          setIntVectorNativeHelpers[idxKind]);
                    return;
                }
                value = localGet(valIndexOnStack);
                setter = setIntVectorNativeHelpers[idxKind];
            }
            else {
                value = loadAtomRep(valIndexOnStack);
                setter = setIntVectorHelpers[idxKind];
            }
        }
        else if (objType == VECTORUINT_TYPE) {
            if (valueType == UINT_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<uint32_t, 0> STORAGE;
                    emitInlineVectorWrite(objIndexOnStack, 
                                          index,
                                          localGet(valIndexOnStack),
                                          offsetof(UIntVectorObject, m_list.m_data),
                                          offsetof(DataListHelper<uint32_t>::LISTDATA, len),
                                          offsetof(STORAGE, entries),
                                          2,
                                          LIR_sti,
                                          setUIntVectorNativeHelpers[idxKind]);
                    return;
                }
                value = localGet(valIndexOnStack);
                setter = setUIntVectorNativeHelpers[idxKind];
            }
            else {
                value = loadAtomRep(valIndexOnStack);
                setter = setUIntVectorHelpers[idxKind];
            }
        }
        else if (objType == VECTORDOUBLE_TYPE) {
            if (valueType == NUMBER_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<double, 0> STORAGE;
                    emitInlineVectorWrite(objIndexOnStack, 
                                          index,
                                          localGetd(valIndexOnStack),
                                          offsetof(DoubleVectorObject, m_list.m_data),
                                          offsetof(DataListHelper<double>::LISTDATA, len),
                                          offsetof(STORAGE, entries),
                                          3,
                                          LIR_std,
                                          setDoubleVectorNativeHelpers[idxKind]);
                    return;
                }
                value = localGetd(valIndexOnStack);
                setter = setDoubleVectorNativeHelpers[idxKind];
            }
            else {
                value = loadAtomRep(valIndexOnStack);
                setter = setDoubleVectorHelpers[idxKind];
            }
        }
#ifdef VMCFG_FLOAT
        else if (objType == VECTORFLOAT_TYPE) {
            if (valueType == FLOAT_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<float, 0> STORAGE;
                    emitInlineVectorWrite(objIndexOnStack, 
                                          index,
                                          localGetf(valIndexOnStack),
                                          offsetof(FloatVectorObject, m_list.m_data),
                                          offsetof(DataListHelper<float>::LISTDATA, len),
                                          offsetof(STORAGE, entries),
                                          2,
                                          LIR_stf,
                                          setFloatVectorNativeHelpers[idxKind]);
                    return;
                }
                value = localGetf(valIndexOnStack);
                setter = setFloatVectorNativeHelpers[idxKind];
            }
            else {
                value = loadAtomRep(valIndexOnStack);
                setter = setFloatVectorHelpers[idxKind];
            }
        }
        else if (objType == VECTORFLOAT4_TYPE) { 
                          
            if (valueType == FLOAT4_TYPE) {
                if (idxKind == VI_INT || idxKind == VI_UINT) {
                    typedef ListData<float4_t, 16> STORAGE;
                    typedef DataListHelper<float4_t, 16>::LISTDATA LISTDATA;
                    emitInlineVectorWrite(objIndexOnStack, 
                                          index,
                                          localGetf4Addr(valIndexOnStack),
                                          offsetof(Float4VectorObject, m_list.m_data),
                                          offsetof(LISTDATA, len),
                                          offsetof(STORAGE, entries),
                                          4,
                                          LIR_stf4,
                                          setFloat4VectorNativeHelpers[idxKind]);
                }
                value = localGetf4Addr(valIndexOnStack);
                setter = setFloat4VectorNativeHelpers[idxKind];
            }
            else {
                value = loadAtomRep(valIndexOnStack);
                setter = setFloat4VectorHelpers[idxKind];
            }
        }
#endif // VMCFG_FLOAT

        if (setter) {
            callIns(setter, 3, localGetp(objIndexOnStack), index, value);
        } else {
            value = loadAtomRep(valIndexOnStack);
            callIns(setGenericHelpers[idxKind], 4, env_param, loadAtomRep(objIndexOnStack), index, value);
        }
    }

    // Try to optimize our input argument to the fastest possible type.
    // Non-negative integer constants can be considered unsigned.
    // Promotions from int/uint to number can be used as integers.
    // Double constants that are actually integers can be used as integers.
    LIns *CodegenLIR::optimizeIndexArgumentType(int32_t sp, Traits** indexType)
    {
        LIns *index = NULL;
        if (*indexType == INT_TYPE) {
            index = localGet(sp);
            if (index->isImmI() && index->immI() > 0) {
                *indexType = UINT_TYPE;
            }

            return index;
        }
        else if (*indexType == UINT_TYPE) {
            return localGet(sp);
        }
        // Convert Number expression to int or uint if it is a promotion
        // from int or uint, or if it is a constant in range for int or uint.
        else if (*indexType == NUMBER_TYPE FLOAT_ONLY(|| *indexType == FLOAT_TYPE)) {
            bool singlePrecision = IFFLOAT(*indexType == FLOAT_TYPE,false);
            index = singlePrecision ? localGetf(sp) : localGetd(sp);
            if (index->opcode() == LIR_i2d FLOAT_ONLY(|| index->opcode()== LIR_i2f)) {
                FLOAT_ONLY(AvmAssert(singlePrecision == (index->opcode()==LIR_i2f)) );
                *indexType = INT_TYPE;
                return index->oprnd1();
            }
            else if (index->opcode() == LIR_ui2d FLOAT_ONLY(|| index->opcode()== LIR_ui2f)) {
                FLOAT_ONLY(AvmAssert(singlePrecision == (index->opcode()==LIR_ui2f)) );
                *indexType = UINT_TYPE;
                return index->oprnd1();
            }
            else if (index->isImmD() FLOAT_ONLY(|| index->isImmF()) )
            {
                double d = IFFLOAT( index->isImmD() ? index->immD(): (double)index->immF(), index->immD());
                // Convert to uint if possible.
                uint32_t u = uint32_t(d);
                if (double(u) == d) {
                    *indexType = UINT_TYPE;
                    return InsConst(u);
                }
                // Failing that, it may still be possible to
                // convert to some negative values to int.
                int32_t i = int32_t(d);
                if (double(i) == d) {
                    *indexType = INT_TYPE;
                    return InsConst(i);
                }
            }
        }

        return index;
    }

    void CodegenLIR::emit(AbcOpcode opcode, uintptr_t op1, uintptr_t op2, Traits* result)
    {
        int sp = state->sp();

        switch (opcode)
        {
            // sign extends
            case OP_sxi1:
            case OP_sxi8:
            case OP_sxi16:
            {
                // straightforward shift based sign extension
                static const uint8_t kShiftAmt[3] = { 31, 24, 16 };
                int32_t index = (int32_t) op1;
                LIns* val = localGet(index);
                if ((opcode == OP_sxi8 && val->opcode() == LIR_ldc2i) ||
                    (opcode == OP_sxi16 && val->opcode() == LIR_lds2i))
                {
                    // if we are sign-extending the result of a load-and-sign-extend
                    // instruction, no need to do anything.
                    break;
                }
                LIns* sh = InsConst(kShiftAmt[opcode - OP_sxi1]);
                LIns* shl = binaryIns(LIR_lshi, val, sh);
                LIns* res = binaryIns(LIR_rshi, shl, sh);
                localSet(index, res, result);
                break;
            }

            // loads
            case OP_lix8:
            case OP_lix16:
            case OP_li8:
            case OP_li16:
            case OP_li32:
            {
                int32_t index = (int32_t) op1;
                LIns* mopAddr = localGet(index);
                const MopsInfo& mi = kMopsLoadInfo[opcode-OP_lix8];
            #ifdef VMCFG_MOPS_USE_EXPANDED_LOADSTORE_INT
                int32_t disp = 0;
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, &disp);
                LIns* i2 = loadIns(mi.op, disp, realAddr, ACCSET_OTHER);
            #else
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, NULL);
                LIns* i2 = callIns(mi.call, 1, realAddr);
            #endif
                localSet(index, i2, result);
                break;
            }

            case OP_lf32:
            case OP_lf64:
            {
                int32_t index = (int32_t) op1;
                LIns* mopAddr = localGet(index);
                const MopsInfo& mi = kMopsLoadInfo[opcode-OP_lix8];
            #ifdef VMCFG_MOPS_USE_EXPANDED_LOADSTORE_FP
                int32_t disp = 0;
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, &disp);
                LIns* i2 = loadIns(mi.op, disp, realAddr, ACCSET_OTHER);
            #else
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, NULL);
                LIns* i2 = callIns(mi.call, 1, realAddr);
            #endif
                localSet(index, i2, result);
                break;
            }
#ifdef VMCFG_FLOAT
            case OP_lf32x4:
            {
                // TODO: inlining.  The appropriate condition is probably *not* VMCFG_MOPS_USE_EXPANDED_LOADSTORE_FP.
                int32_t index = (int32_t) op1;
                LIns* mopAddr = binaryIns(LIR_andi, localGet(index), InsConst(~15U));
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, sizeof(float4_t), NULL);
                LIns* retval = insAlloc(sizeof(float4_t));
                callIns(FUNCTIONID(mop_lf32x4), 2, retval, realAddr);
                localSet(index, ldf4(retval,0,ACCSET_OTHER), result);
                break;
            }
#endif

            // stores
            case OP_si8:
            case OP_si16:
            case OP_si32:
            {
                LIns* svalue = localGet(sp-1);
                LIns* mopAddr = localGet(sp);
                const MopsInfo& mi = kMopsStoreInfo[opcode-OP_si8];
            #ifdef VMCFG_MOPS_USE_EXPANDED_LOADSTORE_INT
                int32_t disp = 0;
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, &disp);
                lirout->insStore(mi.op, svalue, realAddr, disp, ACCSET_OTHER);
            #else
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, NULL);
                callIns(mi.call, 2, realAddr, svalue);
            #endif
                break;
            }

            case OP_sf32:
            case OP_sf64:
            {
                bool singlePrecision = IFFLOAT(state->value(sp-1).traits == FLOAT_TYPE, false);
                LIns* svalue = singlePrecision ? localGetf(sp-1) : localGetd(sp-1);
                LIns* mopAddr = localGet(sp);
                const MopsInfo& mi = kMopsStoreInfo[opcode-OP_si8];
            #ifdef VMCFG_MOPS_USE_EXPANDED_LOADSTORE_FP
                int32_t disp = 0;
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, &disp);
                lirout->insStore(mi.op, svalue, realAddr, disp, ACCSET_OTHER);
            #else
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, mi.size, NULL);
                callIns(mi.call, 2, realAddr, svalue);
            #endif
                break;
            }
#ifdef VMCFG_FLOAT
            case OP_sf32x4:
            {
                // TODO: inlining.  The appropriate condition is probably *not* VMCFG_MOPS_USE_EXPANDED_LOADSTORE_FP.
                LIns* svalue = localGetf4Addr(sp-1);
                LIns* mopAddr = binaryIns(LIR_andi, localGet(sp), InsConst(~15U));
                LIns* realAddr = mopAddrToRangeCheckedRealAddrAndDisp(mopAddr, sizeof(float4_t), NULL);
                callIns(FUNCTIONID(mop_sf32x4), 2, realAddr, svalue);
                break;
            }
#endif

            case OP_jump:
            {
                // spill everything first
                const uint8_t* target = (const uint8_t*) op1;

#ifdef DEBUGGER
                Sampler* s = core->get_sampler();
                if (s && s->sampling() && target < state->abc_pc)
                {
                    emitSampleCheck();
                }
#endif

                branchToAbcPos(LIR_j, 0, target);
                break;
            }

            case OP_lookupswitch:
            {
                //int index = integer(*(sp--));
                //pc += readS24(index < readU16(pc+4) ?
                //  (pc+6+3*index) :    // matched case
                //  (pc+1));            // default
                int count = int(1 + op2);
                const uint8_t* targetpc = (const uint8_t*) op1;

                AvmAssert(state->value(sp).traits == INT_TYPE);
                AvmAssert(count >= 0);

                // Compute address of jump table
                const uint8_t* pc = 4 + state->abc_pc;
                AvmCore::readU32(pc);  // skip count

                // Delete any trailing table entries that == default case (effective for asc output)
                while (count > 0 && targetpc == (state->abc_pc + AvmCore::readS24(pc+3*(count-1))))
                    count--;

                if (count > 0) {
                    LIns* index = localGet(sp);
                    LIns* cmp = binaryIns(LIR_ltui, index, InsConst(count));
                    branchToAbcPos(LIR_jf, cmp, targetpc);

                    // count == 1 is equivalent to if (case) else (default), so don't bother with jtbl
                    if (NJ_JTBL_SUPPORTED && count > 1) {
                        // Backend supports LIR_jtbl for jump tables
                        LIns* jtbl = lirout->insJtbl(index, count);
                        for (int i=0; i < count; i++) {
                            const uint8_t* target = state->abc_pc + AvmCore::readS24(pc+3*i);
                            patchLater(jtbl, target, i);
                        }
                    } else {
                        // Backend doesn't support jump tables, use cascading if's
                        for (int i=0; i < count; i++) {
                            const uint8_t* target = state->abc_pc + AvmCore::readS24(pc+3*i);
                            branchToAbcPos(LIR_jt, binaryIns(LIR_eqi, index, InsConst(i)), target);
                        }
                    }
                }
                else {
                    // switch collapses into a single target
                    branchToAbcPos(LIR_j, 0, targetpc);
                }
                break;
            }

            case OP_returnvoid:
            case OP_returnvalue:
            {
                // ISSUE if a method has multiple returns this causes some bloat

                #ifdef DEBUGGER
                if (haveDebugger) {
                    callIns(FUNCTIONID(debugExit), 2,
                        env_param, csn);
                    // now we toast the cse and restore contents in order to
                    // ensure that any variable modifications made by the debugger
                    // will be pulled in.
                    //firstCse = ip;
                }
                #endif // DEBUGGER

                if (driver->hasReachableExceptions())
                {
                    // _ef.endTry();
                    callIns(FUNCTIONID(endTry), 1, _ef);
                }

                #ifdef VMCFG_TELEMETRY_SAMPLER
                // Emit the code to take a sample of the MethodFrame stack. This is only
                // emitted when the sampler is enabled so there is no performance impact
                // otherwise.
                if(core->samplerEnabled) {
                    LIns* sampleTicks = loadIns(LIR_ldi, offsetof(AvmCore,sampleTicks), coreAddr, ACCSET_OTHER);
                    CodegenLabel &afterTakeSampleExit_label = createLabel("afterTakeSampleExit");
                    branchToLabel(LIR_jf, sampleTicks, afterTakeSampleExit_label);
                    callIns(FUNCTIONID(takeSampleWrapper), 1, coreAddr);
                    emitLabel(afterTakeSampleExit_label);
                }
                #endif

                // replicate MethodFrame dtor inline -- must come after endTry call (if any)
                LIns* nextMethodFrame = loadIns(LIR_ldp, offsetof(MethodFrame,next), methodFrame, ACCSET_OTHER);
                stp(nextMethodFrame, coreAddr, offsetof(AvmCore,currentMethodFrame), ACCSET_OTHER);

                Traits* t = ms->returnTraits();
                LIns* retvalue;
                if (opcode == OP_returnvalue)
                {
                    // already coerced to required native type
                    // use localCopy() to sniff type and use appropriate load instruction
                    int32_t index = (int32_t) op1;
                    retvalue = localCopy(index);
                }
                else
                {
                    retvalue = undefConst;
                    if (t && t != VOID_TYPE)
                    {
                        // implicitly coerce undefined to the return type
                        retvalue = callIns(FUNCTIONID(coerce), 3,
                            env_param, retvalue, InsConstPtr(t));
                        retvalue = atomToNativeRep(t, retvalue);
                    }
                }
                switch (bt(t)) {
#ifdef VMCFG_FLOAT
                case BUILTIN_float: 
                    Ins(LIR_retf, retvalue);
                    break;
                case BUILTIN_float4: 
                    Ins(LIR_retf4, retvalue);
                    break;
#endif
                case BUILTIN_number:
                    Ins(LIR_retd, retvalue);
                    break;
                case BUILTIN_int:
                    retp(i2p(retvalue));
                    break;
                case BUILTIN_uint:
                case BUILTIN_boolean:
                    retp(ui2p(retvalue));
                    break;
                default:
                    retp(retvalue);
                    break;
                }
                break;
            }

            case OP_typeof:
            {
                //sp[0] = typeof(sp[0]);
                int32_t index = (int32_t) op1;
                LIns* value = loadAtomRep(index);
                LIns* i3 = callIns(FUNCTIONID(typeof), 2,
                    coreAddr, value);
                AvmAssert(result == STRING_TYPE);
                localSet(index, i3, result);
                break;
            }

            case OP_not:
            {
                int32_t index = (int32_t) op1;
                AvmAssert(state->value(index).traits == BOOLEAN_TYPE && result == BOOLEAN_TYPE);
                LIns* value = localGet(index); // 0 or 1
                LIns* i3 = eqi0(value); // 1 or 0
                localSet(index, i3, result);
                break;
            }

            case OP_negate: {
                int32_t index = (int32_t) op1;
#ifdef VMCFG_FLOAT
                const Traits*  vt = state->value(index).traits;
                AvmAssert ( vt == result );
                if(vt == NUMBER_TYPE)
                    localSet(index, Ins(LIR_negd, localGetd(index)), result);
                else if(vt == FLOAT_TYPE)
                    localSet(index, Ins(LIR_negf, localGetf(index)), result);
                else if(vt == FLOAT4_TYPE)
                    localSet(index, Ins(LIR_negf4, localGetf4(index)), result);
                else {
                    NanoAssert( vt == OBJECT_TYPE && state->value(index).notNull );
                    emitNumericOp1( index, BasicLIREmitter(this, LIR_negd, FUNCTIONID(op_negate)) );
                }
#else
                localSet(index, Ins(LIR_negd, localGetd(index)),result);
#endif // VMCFG_FLOAT
                break;
            }

            case OP_negate_i: {
                //framep[op1] = -framep[op1]
                int32_t index = (int32_t) op1;
                AvmAssert(state->value(index).traits == INT_TYPE);
                localSet(index, Ins(LIR_negi, localGet(index)), result);
                break;
            }

            case OP_increment:
            case OP_decrement:
            case OP_inclocal:
            case OP_declocal: {
                int32_t index = (int32_t) op1;
                int32_t incr = (int32_t) op2; // 1 or -1
                LIns* addIns = NULL;
#ifdef VMCFG_FLOAT
                const Traits*  vt = state->value(index).traits;
                AvmAssert(result == vt);
                if(vt == NUMBER_TYPE)
                    addIns = binaryIns(LIR_addd, localGetd(index), i2dIns(InsConst(incr)));
                else if(vt == FLOAT_TYPE)
                    addIns = binaryIns(LIR_addf, localGetf(index), InsConstFlt((float)incr));
                else if(vt == FLOAT4_TYPE)
                    addIns = binaryIns(LIR_addf4, localGetf4(index), Ins(LIR_f2f4, InsConstFlt((float)incr)) );
                else 
                { /// NUMERIC - i.e., unknown
                    AvmAssert(vt == OBJECT_TYPE && state->value(index).notNull);
                    emitNumericOp1(index, IncrementLIREmitter(this, incr<0));
                    JIT_EVENT(jit_add);
                    break; // skip the localSet, emitNumericOp1 already does it
                }
                
#else
                addIns = binaryIns(LIR_addd, localGetd(index), i2dIns(InsConst(incr)));
#endif // VMCFG_FLOAT
                localSet(index, addIns,result);  
                break;
            }

            case OP_inclocal_i:
            case OP_declocal_i:
            case OP_increment_i:
            case OP_decrement_i: {
                int32_t index = (int32_t) op1;
                int32_t incr = (int32_t) op2;
                AvmAssert(state->value(index).traits == INT_TYPE);
                localSet(index, binaryIns(LIR_addi, localGet(index), InsConst(incr)), result);
                break;
            }

            case OP_bitnot: {
                // *sp = core->intToAtom(~integer(*sp));
                int32_t index = (int32_t) op1;
                AvmAssert(state->value(index).traits == INT_TYPE);
                localSet(index, lirout->ins1(LIR_noti, localGet(index)), result);
                break;
            }

            case OP_modulo: {
#ifdef VMCFG_FLOAT
                if(result ==FLOAT_TYPE){
                    AvmAssert(state->value(sp-1).traits == FLOAT_TYPE);
                    AvmAssert(state->value(sp).traits == FLOAT_TYPE);
                    LIns* get1 = f2dIns(localGetf(sp-1));
                    LIns* get2 = f2dIns(localGetf(sp));
                    LIns* out = callIns(FUNCTIONID(mod), 2,get1,get2);
                    localSet(sp-1,  d2fIns(out), result);
                } else if(result == FLOAT4_TYPE){
                    AvmAssert(state->value(sp-1).traits == FLOAT4_TYPE);
                    AvmAssert(state->value(sp).traits == FLOAT4_TYPE);
                    LIns* f4 = localGetf4(sp-1), *g4 = localGetf4(sp);
                    LIns* f4x = f2dIns(lirout->ins1(LIR_f4x, f4)), *g4x = f2dIns(lirout->ins1(LIR_f4x, g4));
                    LIns* ox = callIns(FUNCTIONID(mod), 2,f4x,g4x);
                    LIns* f4y = f2dIns(lirout->ins1(LIR_f4y, f4)), *g4y = f2dIns(lirout->ins1(LIR_f4y, g4));
                    LIns* oy = callIns(FUNCTIONID(mod), 2,f4y,g4y);
                    LIns* f4z = f2dIns(lirout->ins1(LIR_f4z, f4)), *g4z = f2dIns(lirout->ins1(LIR_f4z, g4));
                    LIns* oz = callIns(FUNCTIONID(mod), 2,f4z,g4z);
                    LIns* f4w = f2dIns(lirout->ins1(LIR_f4w, f4)), *g4w = f2dIns(lirout->ins1(LIR_f4w, g4));
                    LIns* ow = callIns(FUNCTIONID(mod), 2,f4w,g4w);
                    localSet(sp - 1, lirout->ins4(LIR_ffff2f4,d2fIns(ox), d2fIns(oy), d2fIns(oz), d2fIns(ow)), result);
                } else if(result == NUMBER_TYPE){
                    AvmAssert(state->value(sp-1).traits == NUMBER_TYPE);
                    AvmAssert(state->value(sp).traits == NUMBER_TYPE);
                    LIns* get1= localGetd(sp-1);
                    LIns* get2 = localGetd(sp);
                    LIns* out = callIns(FUNCTIONID(mod), 2,get1,get2);
                    localSet(sp-1,  out, result);
                } else {
                    AvmAssert(result == OBJECT_TYPE && state->value(sp).traits == OBJECT_TYPE && state->value(sp-1).traits == OBJECT_TYPE && state->value(sp).notNull && state->value(sp-1).notNull);
                    emitNumericOp2( sp-1, sp,  ModuloLIREmitter(this) );
                }
#else
                LIns* out = callIns(FUNCTIONID(mod), 2, localGetd(sp-1), localGetd(sp));
                localSet(sp-1,  out, result);
#endif // VMCFG_FLOAT
                break;
            }

            case OP_divide:
            case OP_multiply:
            case OP_subtract: {
#ifdef VMCFG_FLOAT
                LOpcode opf,opf4,opd;
                const CallInfo* interpCall = NULL;
                AvmAssert(state->value(sp-1).traits==result && state->value(sp-1).traits ==result);

                switch(opcode){
                    default: AvmAssertMsg(false,"Can only handle OP_divide/OP_multiply/OP_subtract");
                    case OP_divide:   opf= LIR_divf; opf4= LIR_divf4; opd = LIR_divd; interpCall= FUNCTIONID(op_divide); break;
                    case OP_multiply: opf= LIR_mulf; opf4= LIR_mulf4; opd = LIR_muld; interpCall= FUNCTIONID(op_multiply); break;
                    case OP_subtract: opf= LIR_subf; opf4= LIR_subf4; opd = LIR_subd; interpCall= FUNCTIONID(op_subtract); break;
                }

                if(result ==FLOAT_TYPE){
                    localSet(sp-1, binaryIns(opf, localGetf(sp-1), localGetf(sp)), result);
                } else if(result == NUMBER_TYPE){
                    localSet(sp-1, binaryIns(opd, localGetd(sp-1), localGetd(sp)), result);
                } else if(result == FLOAT4_TYPE){
                    localSet(sp-1, binaryIns(opf4, localGetf4(sp-1), localGetf4(sp)), result);
                } else {
                    AvmAssert(result == OBJECT_TYPE && state->value(sp).traits == OBJECT_TYPE && state->value(sp-1).traits == OBJECT_TYPE && state->value(sp).notNull && state->value(sp-1).notNull);
                    emitNumericOp2(sp-1, sp, BasicLIREmitter(this, opd, interpCall) );
                }
#else
                LOpcode op;
                switch (opcode) {
                    default:
                    case OP_divide:     op = LIR_divd; break;
                    case OP_multiply:   op = LIR_muld; break;
                    case OP_subtract:   op = LIR_subd; break;
                }
                localSet(sp-1, binaryIns(op, localGetd(sp-1), localGetd(sp)), result);
#endif // VMCFG_FLOAT
                break;
            }

            case OP_subtract_i:
            case OP_add_i:
            case OP_multiply_i:
            case OP_lshift:
            case OP_rshift:
            case OP_urshift:
            case OP_bitand:
            case OP_bitor:
            case OP_bitxor:
            {
                LOpcode op;
                switch (opcode) {
                    default:
                    case OP_bitxor:     op = LIR_xori;  break;
                    case OP_bitor:      op = LIR_ori;   break;
                    case OP_bitand:     op = LIR_andi;  break;
                    case OP_urshift:    op = LIR_rshui; break;
                    case OP_rshift:     op = LIR_rshi;  break;
                    case OP_lshift:     op = LIR_lshi;  break;
                    case OP_multiply_i: op = LIR_muli;  break;
                    case OP_add_i:      op = LIR_addi;  break;
                    case OP_subtract_i: op = LIR_subi;  break;
                }
                LIns* lhs = localGet(sp-1);
                LIns* rhs = localGet(sp);
                LIns* out = binaryIns(op, lhs, rhs);
                localSet(sp-1, out, result);
                break;
            }

            case OP_throw:
            {
                //throwAtom(*sp--);
                int32_t index = (int32_t) op1;
                callIns(FUNCTIONID(throwAtom), 2, coreAddr, loadAtomRep(index));
                break;
            }

            case OP_getsuper:
            {
                // stack in: obj [ns [name]]
                // stack out: value
                // sp[0] = env->getsuper(sp[0], multiname)
                int objDisp = sp;
                LIns* multi = initMultiname((Multiname*)op1, objDisp);
                AvmAssert(state->value(objDisp).notNull);

                LIns* obj = loadAtomRep(objDisp);

                LIns* i3 = callIns(FUNCTIONID(getsuper), 3,
                    env_param, obj, multi);
                liveAlloc(multi);

                i3 = atomToNativeRep(result, i3);
                localSet(objDisp, i3, result);
                break;
            }

            case OP_setsuper:
            {
                // stack in: obj [ns [name]] value
                // stack out: nothing
                // core->setsuper(sp[-1], multiname, sp[0], env->vtable->base)
                int objDisp = sp-1;
                LIns* multi = initMultiname((Multiname*)op1, objDisp);
                AvmAssert(state->value(objDisp).notNull);

                LIns* obj = loadAtomRep(objDisp);
                LIns* value = loadAtomRep(sp);

                callIns(FUNCTIONID(setsuper), 4,
                    env_param, obj, multi, value);
                liveAlloc(multi);
                break;
            }

            case OP_nextname:
            case OP_nextvalue:
            {
                // sp[-1] = next[name|value](sp[-1], sp[0]);
                LIns* obj = loadAtomRep(sp-1);
                AvmAssert(state->value(sp).traits == INT_TYPE);
                LIns* index = localGet(sp);
                LIns* i1 = callIns((opcode == OP_nextname) ? FUNCTIONID(nextname) : FUNCTIONID(nextvalue), 3,
                                   env_param, obj, index);
                localSet(sp-1, atomToNativeRep(result, i1), result);
                break;
            }

            case OP_hasnext:
            {
                // sp[-1] = hasnext(sp[-1], sp[0]);
                LIns* obj = loadAtomRep(sp-1);
                AvmAssert(state->value(sp).traits == INT_TYPE);
                LIns* index = localGet(sp);
                LIns* i1 = callIns(FUNCTIONID(hasnext), 3,
                    env_param, obj, index);
                AvmAssert(result == INT_TYPE);
                localSet(sp-1, i1, result);
                break;
            }

            case OP_hasnext2:
            {
                // fixme - if obj is already Atom, or index is already int,
                // easier to directly reference space in vars.
                int32_t obj_index = (int32_t) op1;
                int32_t index_index = (int32_t) op2;
                LIns* obj = insAlloc(sizeof(Atom));
                LIns* index = insAlloc(sizeof(int32_t));
                stp(loadAtomRep(obj_index), obj, 0, ACCSET_STORE_ANY);       // Atom obj
                sti(localGet(index_index), index, 0, ACCSET_STORE_ANY);      // int32_t index
                LIns* i1 = callIns(FUNCTIONID(hasnextproto), 3,
                                     env_param, obj, index);
                localSet(obj_index, loadIns(LIR_ldp, 0, obj, ACCSET_LOAD_ANY), OBJECT_TYPE);  // Atom obj
                localSet(index_index, loadIns(LIR_ldi, 0, index, ACCSET_LOAD_ANY), INT_TYPE); // int32_t index
                AvmAssert(result == BOOLEAN_TYPE);
                localSet(sp+1, i1, result);
                break;
            }

            case OP_newfunction:
            {
                uint32_t function_id = (uint32_t) op1;
                int32_t index = (int32_t) op2;
                //sp[0] = core->newfunction(env, body, _scopeBase, scopeDepth);
                MethodInfo* func = pool->getMethodInfo(function_id);
                int extraScopes = state->scopeDepth;

                // prepare scopechain args for call
                LIns* ap = storeAtomArgs(extraScopes, ms->scope_base());
                LIns* i3 = callIns(FUNCTIONID(newfunction), 3,
                                   env_param, InsConstPtr(func), ap);
                liveAlloc(ap);

                AvmAssert(!result->isMachineType());
                localSet(index, i3, result);
                break;
            }

            case OP_call:
            {
                // stack in: function obj arg1..N
                int argc = int(op1);
                int funcDisp = sp - argc - 1;
                int dest = funcDisp;

                // convert args to Atom[] for the call
                LIns *result_ins, *ap;
                switch (bt(state->value(funcDisp).traits)) {
                case BUILTIN_function:
                case BUILTIN_methodClosure: {
                    // call through pointer in object
                    // ptr = f->m_call_ptr
                    // if (!ptr) goto nfe_label
                    // sp[-argc-1] = (*ptr)(f, argc, obj, ...)
                    LIns* func = localGetp(funcDisp);
                    branchToLabel(LIR_jt, eqp0(func), call_error_label);
                    ap = storeAtomArgs(loadAtomRep(funcDisp+1), argc, funcDisp+2);
                    LIns* call_ptr = ldp(func, offsetof(FunctionObject, m_call_ptr),
                                         ACCSET_OTHER);
                    result_ins = callIns(FUNCTIONID(call_ptr), 4, call_ptr, func, InsConst(argc), ap);
                    break;
                }
                default:
                    // generic call
                    // sp[-argc-1] = op_call(env, sp[-argc], argc, ...)
                    LIns* func = loadAtomRep(funcDisp);
                    ap = storeAtomArgs(loadAtomRep(funcDisp+1), argc, funcDisp+2);
                    result_ins = callIns(FUNCTIONID(op_call_atom), 4, env_param, func, InsConst(argc), ap);
                    break;
                }
                liveAlloc(ap);
                localSet(dest, atomToNativeRep(result, result_ins), result);
                break;
            }

            case OP_callproperty:
            case OP_callproplex:
            case OP_callpropvoid:
            {
                // stack in: obj [ns [name]] arg1..N
                // stack out: result

                // obj = sp[-argc]
                //tempAtom = callproperty(env, name, toVTable(obj), argc, ...);
                //  *(sp -= argc) = tempAtom;
                int argc = int(op2);
                int argv = sp-argc+1;
                int baseDisp = sp-argc;
                const Multiname* name = pool->precomputedMultiname((int)op1);
                LIns* multi = initMultiname(name, baseDisp);
                AvmAssert(state->value(baseDisp).notNull);

                // convert args to Atom[] for the call
                LIns* base = loadAtomRep(baseDisp);
                LIns* receiver = opcode == OP_callproplex ? InsConstAtom(nullObjectAtom) : base;
                LIns* ap = storeAtomArgs(receiver, argc, argv);

                Traits* baseTraits = state->value(baseDisp).traits;
                Binding b = toplevel->getBinding(baseTraits, name);

                LIns* out;
                if (AvmCore::isSlotBinding(b)) {
                    // can early bind call to closure in slot
                    Traits* slotType = Traits::readBinding(baseTraits, b);
                    // todo if funcValue is already a ScriptObject then don't box it, use a different helper.
                    LIns* funcValue = loadFromSlot(baseDisp, AvmCore::bindingToSlotId(b), slotType);
                    LIns* funcAtom = nativeToAtom(funcValue, slotType);
                    out = callIns(FUNCTIONID(op_call_atom), 4, env_param, funcAtom, InsConst(argc), ap);
                }
                else if (!name->isRuntime()) {
                    // use inline cache for late bound call
                    // cache contains: [handler, vtable, [data], Multiname*]
                    // and we call (*cache->handler)(cache, obj, argc, args*, MethodEnv*)
                    CallCache* cache = call_cache_builder.allocateCacheSlot(name);
                    LIns* cacheAddr = InsConstPtr(cache);
                    LIns* handler = loadIns(LIR_ldp, offsetof(CallCache, call_handler), cacheAddr, ACCSET_OTHER);
                    out = callIns(FUNCTIONID(call_cache_handler), 6,
                        handler, cacheAddr, base, InsConst(argc), ap, env_param);
                }
                else {
                    // generic late bound call to anything
                    out = callIns(FUNCTIONID(callprop_late), 5, env_param, base, multi, InsConst(argc), ap);
                    liveAlloc(multi);
                }
                liveAlloc(ap);
                localSet(baseDisp, atomToNativeRep(result, out), result);
                break;
            }

            case OP_constructprop:
            {
                // stack in: obj [ns [name]] arg1..N
                // stack out: result

                int argc = int(op2);
                // obj = sp[-argc]
                //tempAtom = callproperty(env, name, toVTable(obj), argc, ...);
                //  *(sp -= argc) = tempAtom;
                int argv = sp-argc+1;

                int objDisp = sp-argc;
                LIns* multi = initMultiname((Multiname*)op1, objDisp);
                AvmAssert(state->value(objDisp).notNull);

                // convert args to Atom[] for the call
                LIns* ap = storeAtomArgs(loadAtomRep(objDisp), argc, argv);
                LIns* i3 = callIns(FUNCTIONID(construct_late), 4,
                    env_param, multi, InsConst(argc), ap);
                liveAlloc(multi);
                liveAlloc(ap);

                localSet(objDisp, atomToNativeRep(result, i3), result);
                break;
            }

            case OP_callsuper:
            case OP_callsupervoid:
            {
                // stack in: obj [ns [name]] arg1..N
                // stack out: result
                // null check must have already happened.
                //  tempAtom = callsuper(multiname, obj, sp-argc+1, argc, vtable->base);
                int argc = int(op2);
                int argv = sp - argc + 1;
                int objDisp = sp - argc;
                LIns* multi = initMultiname((Multiname*)op1, objDisp);
                AvmAssert(state->value(objDisp).notNull);

                // convert args to Atom[] for the call
                LIns* obj = loadAtomRep(objDisp);

                LIns* ap = storeAtomArgs(obj, argc, argv);

                LIns* i3 = callIns(FUNCTIONID(callsuper), 4,
                    env_param, multi, InsConst(argc), ap);
                liveAlloc(multi);
                liveAlloc(ap);

                localSet(objDisp, atomToNativeRep(result, i3), result);
                break;
            }

            case OP_applytype:
            {
                // stack in: method arg1..N
                // sp[-argc] = applytype(env, sp[-argc], argc, null, arg1..N)
                int argc = int(op1);
                int funcDisp = sp - argc;
                int dest = funcDisp;
                int arg0 = sp - argc + 1;

                LIns* func = loadAtomRep(funcDisp);

                // convert args to Atom[] for the call
                LIns* ap = storeAtomArgs(argc, arg0);

                LIns* i3 = callIns(FUNCTIONID(op_applytype), 4,
                    env_param, func, InsConst(argc), ap);
                liveAlloc(ap);

                localSet(dest, atomToNativeRep(result, i3), result);
                break;
            }

            case OP_newobject:
            {
                // result = env->op_newobject(sp, argc)
                int argc = int(op1);
                int dest = sp - (2*argc-1);
                int arg0 = dest;

                // convert args to Atom for the call[]
                LIns* ap = storeAtomArgs(2*argc, arg0);

                LIns* i3 = callIns(FUNCTIONID(op_newobject), 3,
                    env_param, lea(sizeof(Atom)*(2*argc-1), ap), InsConst(argc));
                liveAlloc(ap);

                localSet(dest, ptrToNativeRep(result, i3), result);
                break;
            }

            case OP_newactivation:
            {
                // result = env->newActivation()
                LIns* activation = callIns(FUNCTIONID(newActivation), 1, env_param);
                localSet(sp+1, ptrToNativeRep(result, activation), result);
                break;
            }

            case OP_newcatch:
            {
                // result = core->newObject(env->activation, NULL);
                int dest = sp+1;

                LIns* activation = callIns(FUNCTIONID(newcatch), 2,
                                         env_param, InsConstPtr(result));

                localSet(dest, ptrToNativeRep(result, activation), result);
                break;
            }

            case OP_newarray:
            {
                // sp[-argc+1] = env->toplevel()->arrayClass->newarray(sp-argc+1, argc)
                int argc = int(op1);
                int arg0 = sp - 1*argc+1;

                // convert array elements to Atom[]
                LIns* ap = storeAtomArgs(argc, arg0);
                LIns* i3 = callIns(FUNCTIONID(newarray), 3, env_param, InsConst(argc), ap);
                liveAlloc(ap);

                AvmAssert(!result->isMachineType());
                localSet(arg0, i3, result);
                break;
            }

            case OP_newclass:
            {
                // sp[0] = core->newclass(env, ctraits, scopeBase, scopeDepth, base)
                Traits* ctraits = (Traits*) op1;
                int localindex = int(op2);
                int extraScopes = state->scopeDepth;

                LIns* outer = loadEnvScope();
                LIns* base = localGetp(localindex);

                // prepare scopechain args for call
                LIns* ap = storeAtomArgs(extraScopes, ms->scope_base());

                LIns* i3 = callIns(FUNCTIONID(newclass), 5,
                    env_param, InsConstPtr(ctraits), base, outer, ap);
                liveAlloc(ap);

                AvmAssert(!result->isMachineType());
                localSet(localindex, i3, result);
                break;
            }

            case OP_getdescendants:
            {
                // stack in: obj [ns [name]]
                // stack out: value
                //sp[0] = core->getdescendants(sp[0], name);
                int objDisp = sp;
                Multiname* multiname = (Multiname*) op1;

                LIns* multi = initMultiname(multiname, objDisp);
                LIns* obj = loadAtomRep(objDisp);
                AvmAssert(state->value(objDisp).notNull);

                LIns* out = callIns(FUNCTIONID(getdescendants), 3,
                    env_param, obj, multi);
                liveAlloc(multi);

                localSet(objDisp, atomToNativeRep(result, out), result);
                break;
            }

            case OP_checkfilter: {
                int32_t index = (int32_t) op1;
                callIns(FUNCTIONID(checkfilter), 2, env_param, loadAtomRep(index));
                break;
            }

            case OP_findpropstrict:
            case OP_findproperty:
            {
                // stack in: [ns [name]]
                // stack out: obj
                // sp[1] = env->findproperty(scopeBase, scopedepth, name, strict)
                int dest = sp;
                LIns* multi = initMultiname((Multiname*)op1, dest);
                dest++;
                int extraScopes = state->scopeDepth;

                // prepare scopechain args for call
                LIns* ap = storeAtomArgs(extraScopes, ms->scope_base());

                LIns* outer = loadEnvScope();

                LIns* withBase;
                if (state->withBase == -1)
                {
                    withBase = InsConstPtr(0);
                }
                else
                {
                    withBase = lea(state->withBase*sizeof(Atom), ap);
                }

                //      return env->findproperty(outer, argv, extraScopes, name, strict);

                LIns* i3 = callIns(FUNCTIONID(findproperty), 7,
                    env_param, outer, ap, InsConst(extraScopes), multi,
                    InsConst((int32_t)(opcode == OP_findpropstrict)),
                    withBase);
                liveAlloc(multi);
                liveAlloc(ap);

                localSet(dest, atomToNativeRep(result, i3), result);
                break;
            }

            case OP_getproperty:
            {
                // stack in: obj [ns] [name]
                // stack out: value
                // obj=sp[0]
                //sp[0] = env->getproperty(obj, multiname);

                const Multiname* multiname = pool->precomputedMultiname((int)op1);
                bool attr = multiname->isAttr();
                Traits* indexType = state->value(sp).traits;
                LIns* index = NULL;
                bool maybeIntegerIndex = !attr && multiname->isRtname() && multiname->containsAnyPublicNamespace();

                if (maybeIntegerIndex)
                    index = optimizeIndexArgumentType(sp, &indexType);

                if (maybeIntegerIndex && indexType == INT_TYPE) {
                    LIns *value = emitGetIndexedProperty(sp-1, index, result, VI_INT);
                    localSet(sp-1, value, result);
                }
                else if (maybeIntegerIndex && indexType == UINT_TYPE) {
                    LIns *value = emitGetIndexedProperty(sp-1, index, result, VI_UINT);
                    localSet(sp-1, value, result);
                }
                else if (maybeIntegerIndex && indexType == NUMBER_TYPE) {
                    bool bGeneratedFastPath = false;
#ifdef VMCFG_FASTPATH_ADD_INLINE
                    if (inlineFastpath) {
                        // TODO: Since a helper must be called anyhow, I conjecture that fused add-and-index helpers
                        // may be more efficient than the inlining done here.  On the other hand, it is plausible that
                        // we would go the other way and completely inline the getNativeXXXProperty cases, in which
                        // case we'd want to retain this for the sake of more thorough inlining.
                        LOpcode op = index->opcode();
                        if (op == LIR_addd || op == LIR_subd) {
                            CodegenLabel slow_path("slow");
                            CodegenLabel done_path("done");
                            LIns* tempResult = insAllocForTraits(result);
                            suspendCSE();
                            LIns* a = index->oprnd1();
                            LIns* b = index->oprnd2();
                            // Our addjovi only works with signed integers so don't use isPromote.
                            a = (a->opcode() == LIR_i2d) ? a->oprnd1() : imm2Int(a);
                            b = (b->opcode() == LIR_i2d) ? b->oprnd1() : imm2Int(b);
                            if (a && b) {
                                // Inline fast path for index generated from integer add
                                // if (a+b) does not overflow && ((a+b) >= 0)
                                //   call getUintProperty(obj, a + b)
                                // else
                                //   call getprop_index_add (env, obj, multiname, a, b)
                                // (for subtraction, we emit (a-b) and call getprop_index_subtract)
                                LIns* addOp = branchJovToLabel((op == LIR_addd) ? LIR_addjovi : LIR_subjovi, a, b, slow_path);
                                branchToLabel(LIR_jt, binaryIns(LIR_lti, addOp, InsConst(0)), slow_path);
                                LIns* value0 = emitGetIndexedProperty(sp-1, addOp, result, VI_UINT);
                                stForTraits(result, value0, tempResult, 0, ACCSET_STORE_ANY);
                                branchToLabel(LIR_j, NULL, done_path);

                                emitLabel(slow_path);
                                LIns* value1 = emitGetIndexedProperty(sp-1, index, result, VI_DOUBLE);
                                stForTraits(result, value1, tempResult, 0, ACCSET_STORE_ANY);

                                emitLabel(done_path);
                                localSet(sp-1, ldForTraits(result, tempResult, 0, ACCSET_LOAD_ANY), result);
                                bGeneratedFastPath = true;
                            }
                            resumeCSE();
                        }
                    }
#endif // VMCFG_FASTPATH_ADD_INLINE
                    if (!bGeneratedFastPath) {
                        LIns *value = emitGetIndexedProperty(sp-1, index, result, VI_DOUBLE);
                        localSet(sp-1, value, result);
                    }
                }
                else if (maybeIntegerIndex && indexType != STRING_TYPE) {
                    LIns* multi = InsConstPtr(multiname); // inline ptr to precomputed name
                    LIns* index = loadAtomRep(sp);
                    AvmAssert(state->value(sp-1).notNull);
                    LIns* obj = loadAtomRep(sp-1);
                    LIns* value = callIns(FUNCTIONID(getprop_index), 4,
                                        env_param, obj, multi, index);

                    localSet(sp-1, atomToNativeRep(result, value), result);
                }
                else {
                    int objDisp = sp;
                    LIns* multi = initMultiname(multiname, objDisp);
                    AvmAssert(state->value(objDisp).notNull);

                    LIns* value;
                    LIns* obj = loadAtomRep(objDisp);
                    if (multiname->isRuntime()) {
                        //return getprop_late(obj, name);
                        value = callIns(FUNCTIONID(getprop_late), 3, env_param, obj, multi);
                        liveAlloc(multi);
                    } else {
                        // static name, use property cache
                        GetCache* cache = get_cache_builder.allocateCacheSlot(multiname);
                        LIns* cacheAddr = InsConstPtr(cache);
                        LIns* handler = loadIns(LIR_ldp, offsetof(GetCache, get_handler), cacheAddr, ACCSET_OTHER);
                        value = callIns(FUNCTIONID(get_cache_handler), 4, handler, cacheAddr, env_param, obj);
                    }

                    localSet(objDisp, atomToNativeRep(result, value), result);
                }
                break;
            }
            case OP_initproperty:
            case OP_setproperty:
            {
                // stack in: obj [ns] [name] value
                // stack out:
                // obj = sp[-1]
                //env->setproperty(obj, multiname, sp[0], toVTable(obj));

                const Multiname* multiname = (const Multiname*)op1;
                bool attr = multiname->isAttr();
                Traits* indexType = state->value(sp-1).traits;
                LIns* index = NULL;
                bool maybeIntegerIndex = !attr && multiname->isRtname() && multiname->containsAnyPublicNamespace();

                if (maybeIntegerIndex)
                    index = optimizeIndexArgumentType(sp-1, &indexType);

                if (maybeIntegerIndex && indexType == INT_TYPE) {
                    emitSetIndexedProperty(sp-2, sp, index, VI_INT);
                }
                else if (maybeIntegerIndex && indexType == UINT_TYPE) {
                    emitSetIndexedProperty(sp-2, sp, index, VI_UINT);
                }
                else if (maybeIntegerIndex && indexType == NUMBER_TYPE) {
                    bool bGeneratedFastPath = false;
#ifdef VMCFG_FASTPATH_ADD_INLINE
                    if (inlineFastpath) {
                        LOpcode op = index->opcode();
                        if (op == LIR_addd || op == LIR_subd) {
                            CodegenLabel slow_path("slow");
                            CodegenLabel done_path("done");
                            suspendCSE();
                            LIns *a = index->oprnd1();
                            LIns *b = index->oprnd2();
                            // Our addjovi only works with signed integers.
                            a = (a->opcode() == LIR_i2d) ? a->oprnd1() : imm2Int(a);
                            b = (b->opcode() == LIR_i2d) ? b->oprnd1() : imm2Int(b);
                            if (a && b) {
                                // Inline fast path for index generated from integer add
                                // if (a + b) does not overflow && ((a+b) >= 0)
                                //   call setProperty(obj, value, a + b)
                                // else
                                //   call set/initprop_index_add(env, obj, name, value, a, b)
                                // (for subtraction, we emit (a-b) and call set/initprop_index_subtract)
                                LIns* addOp = branchJovToLabel((op == LIR_addd) ? LIR_addjovi : LIR_subjovi, a, b, slow_path);
                                branchToLabel(LIR_jt, binaryIns(LIR_lti, addOp, InsConst(0)), slow_path);
                                emitSetIndexedProperty(sp-2, sp, addOp, VI_UINT);
                                branchToLabel(LIR_j, NULL, done_path);

                                emitLabel(slow_path);
                                emitSetIndexedProperty(sp-2, sp, index, VI_DOUBLE);

                                emitLabel(done_path);
                                bGeneratedFastPath = true;
                            }
                            resumeCSE();
                        }
                    }
#endif // VMCFG_FASTPATH_ADD_INLINE
                    if (!bGeneratedFastPath) {
                        emitSetIndexedProperty(sp-2, sp, index, VI_DOUBLE);
                    }
                }
                else if (maybeIntegerIndex) {
                    LIns* name = InsConstPtr(multiname); // precomputed multiname
                    LIns* value = loadAtomRep(sp);
                    LIns* index = loadAtomRep(sp-1);
                    AvmAssert(state->value(sp-2).notNull);
                    LIns* obj = loadAtomRep(sp-2);
                    const CallInfo* func = (opcode == OP_setproperty) ? FUNCTIONID(setprop_index) : FUNCTIONID(initprop_index);
                    callIns(func, 5, env_param, obj, name, value, index);
                }
                else {
                    int objDisp = sp-1;
                    LIns* value = loadAtomRep(sp);
                    LIns* multi = initMultiname(multiname, objDisp);
                    AvmAssert(state->value(objDisp).notNull);

                    LIns* obj = loadAtomRep(objDisp);
                    if (opcode == OP_setproperty) {
                        if (!multiname->isRuntime()) {
                            // use inline cache for dynamic setproperty access
                            SetCache* cache = set_cache_builder.allocateCacheSlot(multiname);
                            LIns* cacheAddr = InsConstPtr(cache);
                            LIns* handler = loadIns(LIR_ldp, offsetof(SetCache, set_handler), cacheAddr, ACCSET_OTHER);
                            callIns(FUNCTIONID(set_cache_handler), 5, handler, cacheAddr, obj, value, env_param);
                        } else {
                            // last resort slow path for OP_setproperty
                            callIns(FUNCTIONID(setprop_late), 4, env_param, obj, multi, value);
                            liveAlloc(multi);
                        }
                    }
                    else {
                        // initproplate is rare in jit code because we typically interpret static
                        // initializers, and constructor initializers tend to early-bind successfully.
                        callIns(FUNCTIONID(initprop_late), 4, env_param, obj, multi, value);
                        liveAlloc(multi);
                    }
                }
                break;
            }

            case OP_deleteproperty:
            {
                // stack in: obj [ns] [name]
                // stack out: Boolean
                //sp[0] = delproperty(sp[0], multiname);
                int objDisp = sp;
                Multiname *multiname = (Multiname*)op1;
                if(!multiname->isRtname()) {
                    LIns* multi = initMultiname(multiname, objDisp, true);

                    LIns* obj = loadAtomRep(objDisp);

                    LIns* i3 = callIns(FUNCTIONID(delproperty), 3,
                        env_param, obj, multi);
                    liveAlloc(multi);

                    localSet(objDisp, atomToNativeRep(result, i3), result);
                } else {
                    LIns* _tempname = copyMultiname(multiname);
                    LIns* index = loadAtomRep(objDisp--);

                    if( !multiname->isRtns() )
                    {
                        // copy the compile-time namespace to the temp multiname
                        LIns* mSpace = InsConstPtr(multiname->ns);
                        stp(mSpace, _tempname, offsetof(Multiname, ns), ACCSET_OTHER);
                    }
                    else
                    {
                        // intern the runtime namespace and copy to the temp multiname
                        LIns* nsAtom = loadAtomRep(objDisp--);
                        LIns* internNs = callIns(FUNCTIONID(internRtns), 2,
                            env_param, nsAtom);

                        stp(internNs, _tempname, offsetof(Multiname,ns), ACCSET_OTHER);
                    }
                    liveAlloc(_tempname);

                    AvmAssert(state->value(objDisp).notNull);
                    LIns* obj = loadAtomRep(objDisp);

                    LIns* value = callIns(FUNCTIONID(delpropertyHelper), 4,
                                        env_param, obj, _tempname, index);

                    localSet(objDisp, atomToNativeRep(result, value), result);
                }
                break;
            }

            case OP_esc_xelem: // ToXMLString will call EscapeElementValue
            {
                //sp[0] = core->ToXMLString(sp[0]);
                int32_t index = (int32_t) op1;
                LIns* value = loadAtomRep(index);
                LIns* i3 = callIns(FUNCTIONID(ToXMLString), 2,
                    coreAddr, value);
                AvmAssert(result == STRING_TYPE);
                localSet(index, i3, result);
                break;
            }

            case OP_esc_xattr:
            {
                //sp[0] = core->EscapeAttributeValue(sp[0]);
                int32_t index = (int32_t) op1;
                LIns* value = loadAtomRep(index);
                LIns* i3 = callIns(FUNCTIONID(EscapeAttributeValue), 2,
                    coreAddr, value);
                AvmAssert(result == STRING_TYPE);
                localSet(index, i3, result);
                break;
            }

            case OP_astype:
            {
                // sp[0] = AvmCore::astype(sp[0], traits)
                Traits *type = (Traits*) op1;
                int32_t index = (int32_t) op2;
                LIns* obj = loadAtomRep(index);
                LIns* i1 = callIns(FUNCTIONID(astype), 2, obj, InsConstPtr(type));
                i1 = atomToNativeRep(result, i1);
                localSet(index, i1, result);
                break;
            }

            case OP_astypelate:
            {
                //sp[-1] = astype_late(env, sp[-1], sp[0]);
                LIns* type = loadAtomRep(sp);
                LIns* obj = loadAtomRep(sp-1);
                LIns* i3 = callIns(FUNCTIONID(astype_late), 3, env_param, obj, type);
                i3 = atomToNativeRep(result, i3);
                localSet(sp-1, i3, result);
                break;
            }

            case OP_strictequals:
            {
                AvmAssert(result == BOOLEAN_TYPE);
                localSet(sp-1, cmpEq(FUNCTIONID(stricteq), sp-1, sp), result);
                break;
            }

            case OP_equals:
            {
                AvmAssert(result == BOOLEAN_TYPE);
                localSet(sp-1, cmpEq(FUNCTIONID(equals), sp-1, sp), result);
                break;
            }

            case OP_lessthan:
            {
                AvmAssert(result == BOOLEAN_TYPE);
                localSet(sp-1, cmpLt(sp-1, sp), result);
                break;
            }

            case OP_lessequals:
            {
                AvmAssert(result == BOOLEAN_TYPE);
                localSet(sp-1, cmpLe(sp-1, sp), result);
                break;
            }

            case OP_greaterthan:
            {
                AvmAssert(result == BOOLEAN_TYPE);
                localSet(sp-1, cmpLt(sp, sp-1), result);
                break;
            }

            case OP_greaterequals:
            {
                AvmAssert(result == BOOLEAN_TYPE);
                localSet(sp-1, cmpLe(sp, sp-1), result);
                break;
            }

            case OP_instanceof:
            {
                LIns* lhs = loadAtomRep(sp-1);
                LIns* rhs = loadAtomRep(sp);
                LIns* out = callIns(FUNCTIONID(instanceof), 3, env_param, lhs, rhs);
                out = atomToNativeRep(result, out);
                localSet(sp-1,  out, result);
                break;
            }

            case OP_in:
            {
                // sp[-1] = env->in_operator(sp[-1], sp[0])
                int lhsDisp = sp-1;
                LIns *lhs;
                LIns *rhs = loadAtomRep(sp);
                LIns *out;
                Traits *lhsType = state->value(lhsDisp).traits;
                switch (bt(lhsType)) {
                case BUILTIN_uint:
                    lhs = localGet(lhsDisp);
                    out = callIns(FUNCTIONID(haspropertylate_u), 3, env_param, rhs, lhs);
                    break;
                case BUILTIN_int:
                    lhs = localGet(lhsDisp);
                    out = callIns(FUNCTIONID(haspropertylate_i), 3, env_param, rhs, lhs);
                    break;
                default:
                    lhs = loadAtomRep(lhsDisp);
                    out = callIns(FUNCTIONID(op_in), 3, env_param, lhs, rhs);
                    out = atomToNativeRep(result, out);
                    break;
                }
                localSet(sp-1, out, result);
                break;
            }

            case OP_dxns:
            {
                LIns* uri = InsConstPtr((String*)op1); // namespace uri from string pool
                callIns(FUNCTIONID(setDxns), 3, coreAddr, methodFrame, uri);
                break;
            }

            case OP_dxnslate:
            {
                int32_t index = (int32_t) op1;
                LIns* atom = loadAtomRep(index);
                callIns(FUNCTIONID(setDxnsLate), 3, coreAddr, methodFrame, atom);
                break;
            }

            default:
            {
                AvmAssert(false); // unsupported
            }
        }

    } // emit()

    void CodegenLIR::emitIf(AbcOpcode opcode, const uint8_t* target, int a, int b)
    {
#ifdef DEBUGGER
        Sampler* s = core->get_sampler();
        if (s && s->sampling() && target < state->abc_pc)
        {
            emitSampleCheck();
        }
#endif

        //
        // compile instructions that cannot throw exceptions before we add exception handling logic
        //

        // op1 = abc opcode target
        // op2 = what local var contains condition

        LIns* cond;
        LOpcode br;

        switch (opcode)
        {
        case OP_iftrue:
            NanoAssert(state->value(a).traits == BOOLEAN_TYPE);
            br = LIR_jf;
            cond = eqi0(localGet(a));
            break;
        case OP_iffalse:
            NanoAssert(state->value(a).traits == BOOLEAN_TYPE);
            br = LIR_jt;
            cond = eqi0(localGet(a));
            break;
        case OP_iflt:
            br = LIR_jt;
            cond = cmpLt(a, b);
            break;
        case OP_ifnlt:
            br = LIR_jf;
            cond = cmpLt(a, b);
            break;
        case OP_ifle:
            br = LIR_jt;
            cond = cmpLe(a, b);
            break;
        case OP_ifnle:
            br = LIR_jf;
            cond = cmpLe(a, b);
            break;
        case OP_ifgt:  // a>b === b<a
            br = LIR_jt;
            cond = cmpLt(b, a);
            break;
        case OP_ifngt: // !(a>b) === !(b<a)
            br = LIR_jf;
            cond = cmpLt(b, a);
            break;
        case OP_ifge:  // a>=b === b<=a
            br = LIR_jt;
            cond = cmpLe(b, a);
            break;
        case OP_ifnge: // !(a>=b) === !(a<=b)
            br = LIR_jf;
            cond = cmpLe(b, a);
            break;
        case OP_ifeq:
            br = LIR_jt;
            cond = cmpEq(FUNCTIONID(equals), a, b);
            break;
        case OP_ifne:
            br = LIR_jf;
            cond = cmpEq(FUNCTIONID(equals), a, b);
            break;
        case OP_ifstricteq:
            br = LIR_jt;
            cond = cmpEq(FUNCTIONID(stricteq), a, b);
            break;
        case OP_ifstrictne:
            br = LIR_jf;
            cond = cmpEq(FUNCTIONID(stricteq), a, b);
            break;
        default:
            AvmAssert(false);
            return;
        }

        if (cond->isImmI()) {
            if ((br == LIR_jt && cond->immI()) || (br == LIR_jf && !cond->immI())) {
                // taken
                br = LIR_j;
                cond = 0;
            }
            else {
                // not taken = no-op
                return;
            }
        }

        branchToAbcPos(br, cond, target);
    } // emitIf()

    // Try to optimize comparison of a function call yielding NUMBER_TYPE with another expression
    // of INT_TYPE.  We may be able to simplify the function and/or the comparison based on the
    // type or value, if constant, of the other argument.  The function call is normally presumed
    // to be the left-hand argument.  The swap parameter, if true, reverses this convention.

    static const Specialization intCmpWithNumber[] = {
        { FUNCTIONID(String_charCodeAtDI),    FUNCTIONID(String_charCodeAtIU) },
        { FUNCTIONID(String_charCodeAtDU),    FUNCTIONID(String_charCodeAtIU) },
        { FUNCTIONID(String_charCodeAtDD),    FUNCTIONID(String_charCodeAtID) },
        { 0, 0 }
    };

    LIns *CodegenLIR::optimizeIntCmpWithNumberCall(int callIndex, int otherIndex, LOpcode icmp, bool swap)
    {
        LIns* numSide = localGetd(callIndex);
        const CallInfo *ci = numSide->callInfo();

        // Try to optimize charCodeAt to return an integer if possible. Because it can return NaN for
        // out of bounds access, we need to limit our support to constant integer values that generate
        // the same results for both NaN (floating point result) and zero (NaN cast to integer result).
        // These are the six possibilities:
        // String.CharCodeAt == int - any constant integer but zero
        // String.CharCodeAt < int  - zero or any negative integer constant
        // String.CharCodeAt <= int - any negative integer constant
        // int == String.CharCodeAt - any constant integer but zero
        // int < String.CharCodeAt  - zero or any positive integer constant
        // int <= String.CharCodeAt - any positive integer constant

        if (ci == FUNCTIONID(String_charCodeAtDI) || ci == FUNCTIONID(String_charCodeAtDU) || ci == FUNCTIONID(String_charCodeAtDD)) {

            AvmAssert(numSide->opcode() == LIR_calld);

            LIns* intSide = localGet(otherIndex);
            if (!intSide->isImmI())
                return NULL;
            int32_t intVal = intSide->immI();

            if ((icmp == LIR_eqi && intVal != 0) ||
                (icmp == LIR_lti && (swap ? intVal >= 0 : intVal <= 0)) ||
                (icmp == LIR_lei && (swap ? intVal > 0 : intVal < 0))) {

                numSide = specializeIntCall(numSide, intCmpWithNumber);
                AvmAssert(numSide != NULL);
                if (swap) {
                    return binaryIns(icmp, intSide, numSide);
                } else {
                    return binaryIns(icmp, numSide, intSide);
                }
            }
        }

        return NULL;
    }

    static const Specialization stringCmpWithString[] = {
        { FUNCTIONID(String_charAtI),    FUNCTIONID(String_charCodeAtII) },
        { FUNCTIONID(String_charAtU),    FUNCTIONID(String_charCodeAtIU) },
        { FUNCTIONID(String_charAtD),    FUNCTIONID(String_charCodeAtID) },
        { 0, 0 }
    };

    LIns *CodegenLIR::optimizeStringCmpWithStringCall(int callIndex, int otherIndex, LOpcode icmp, bool swap)
    {
        LIns* callSide = localGetp(callIndex);
        const CallInfo *ci = callSide->callInfo();
        if (ci == FUNCTIONID(String_charAtI) || ci == FUNCTIONID(String_charAtU) || ci == FUNCTIONID(String_charAtD)) {
            LIns*  strSide = localGetp(otherIndex);
            if (!strSide->isImmP())
                return NULL;

            String *s = (String *) strSide->immP();
            // If we have a single character constant string that is not the null
            // character, we can use charCodeAt which is about 2x faster.
            if (s->length() == 1 && s->charAt(0) != 0) {
                int32_t firstChar = s->charAt(0);
                strSide = InsConst(firstChar);
                callSide = specializeIntCall(callSide, stringCmpWithString);
                AvmAssert(callSide != NULL);
                if (swap) {
                    return binaryIns(icmp, strSide, callSide);
                } else {
                    return binaryIns(icmp, callSide, strSide);
                }
            }
        }

        return NULL;
    }

    // Faster compares for int, uint, double, boolean
    LIns* CodegenLIR::cmpOptimization( int lhsi, int rhsi, LOpcode icmp, LOpcode ucmp, LOpcode fcmp, bool strictOperation /*=false*/ )
    {
        Traits* lht = state->value(lhsi).traits;
        Traits* rht = state->value(rhsi).traits;

        if (lht == rht && (lht == INT_TYPE || lht == BOOLEAN_TYPE))
        {
            LIns* lhs = localGet(lhsi);
            LIns* rhs = localGet(rhsi);
            return binaryIns(icmp, lhs, rhs);
        }
        else if (lht == rht && lht == UINT_TYPE)
        {
            LIns* lhs = localGet(lhsi);
            LIns* rhs = localGet(rhsi);
            return binaryIns(ucmp, lhs, rhs);
        }
#ifdef VMCFG_FLOAT
        else if (lht == rht && lht == FLOAT_TYPE)
        {
            LIns* lhs = localGetf(lhsi);
            LIns* rhs = localGetf(rhsi);
            return binaryIns(getCmpFOpcode(fcmp), lhs, rhs);
        }
        else if (lht == FLOAT4_TYPE || rht == FLOAT4_TYPE)
        {
            if (fcmp == LIR_eqd) {
                // TODO: This is not the best way to optimize the mixed-mode cases
                // if SIMD instructions are not available on the target platform.
                if (lht == rht) {
                    // float4 ==/=== float4
                    LIns* lhs = localGetf4(lhsi);
                    LIns* rhs = localGetf4(rhsi);
                    return binaryIns(LIR_eqf4, lhs, rhs);
                } else if (lht == FLOAT_TYPE) {
                    // float ==/=== float4
                    if (strictOperation)
                        return InsConst(0);
                    LIns* lhs = Ins(LIR_f2f4, localGetf(lhsi));
                    LIns* rhs = localGetf4(rhsi);
                    return binaryIns(LIR_eqf4, lhs, rhs);
                } else if (rht == FLOAT_TYPE) {
                    // float4 ==/=== float
                    if (strictOperation)
                        return InsConst(0);
                    LIns* lhs = localGetf4(lhsi);
                    LIns* rhs = Ins(LIR_f2f4, localGetf(rhsi));
                return binaryIns(LIR_eqf4, lhs, rhs);
                } else
                    // Punt to out-of-line handler.
                    return NULL;
            } else
                // Comparison with NaN, always false.
                return InsConst(0);
        }
#endif // VMCFG_FLOAT
        else if (lht && lht->isNumeric() && rht && rht->isNumeric())
        {
            // Comparing the result of a call returning a Number to another int value.
            if (lht == NUMBER_TYPE && rht == INT_TYPE && localGetd(lhsi)->opcode() == LIR_calld) {
                LIns* result = optimizeIntCmpWithNumberCall(lhsi, rhsi, icmp, false);
                if (result)
                    return result;
            }
            if (rht == NUMBER_TYPE && lht == INT_TYPE && localGetd(rhsi)->opcode() == LIR_calld) {
                LIns* result = optimizeIntCmpWithNumberCall(rhsi, lhsi, icmp, true);
                if (result)
                    return result;
            }
            // Note: no optimizations needed above for "float" or "numeric".
            // Those optimize String.charCodeAt(), which returns a Number by language definition.

            // If we're comparing a uint to an int and the int is a non-negative
            // integer constant, don't promote to doubles for the compare
            if ((lht == UINT_TYPE) && (rht == INT_TYPE))
            {
                LIns* lhs = localGet(lhsi);
                LIns* rhs = localGet(rhsi);
            #ifdef AVMPLUS_64BIT
                // 32-bit signed and unsigned values fit in 64-bit registers
                // so we can promote and simply do a signed 64bit compare
                LOpcode qcmp = cmpOpcodeI2Q(icmp);
                NanoAssert((icmp == LIR_eqi && qcmp == LIR_eqq) ||
                           (icmp == LIR_lti && qcmp == LIR_ltq) ||
                           (icmp == LIR_lei && qcmp == LIR_leq));
                return binaryIns(qcmp, ui2p(lhs), i2p(rhs));
            #else
                if (rhs->isImmI() && rhs->immI() >= 0)
                    return binaryIns(ucmp, lhs, rhs);
            #endif
            }
            else if ((lht == INT_TYPE) && (rht == UINT_TYPE))
            {
                LIns* lhs = localGet(lhsi);
                LIns* rhs = localGet(rhsi);
            #ifdef AVMPLUS_64BIT
                // 32-bit signed and unsigned values fit in 64-bit registers
                // so we can promote and simply do a signed 64bit compare
                LOpcode qcmp = cmpOpcodeI2Q(icmp);
                NanoAssert((icmp == LIR_eqi && qcmp == LIR_eqq) ||
                           (icmp == LIR_lti && qcmp == LIR_ltq) ||
                           (icmp == LIR_lei && qcmp == LIR_leq));
                return binaryIns(qcmp, i2p(lhs), ui2p(rhs));
            #else
                if (lhs->isImmI() && lhs->immI() >= 0)
                    return binaryIns(ucmp, lhs, rhs);
            #endif
            }
            if(!strictOperation && (lht->isNumberType() || rht->isNumberType()) ){
                // NOTE: This assumes that comparisons between two floats works just as
                // well performed on doubles.  For float vs. number, we can't do strict
                // equality tests this way, since the types are assumed to be different.
                LIns* lhs = promoteNumberIns(lht, lhsi);
                LIns* rhs = promoteNumberIns(rht, rhsi);
                return binaryIns(fcmp, lhs, rhs);
            }
        }

        if (lht == STRING_TYPE && rht == STRING_TYPE) {
            if (localGetp(lhsi)->opcode() == LIR_calli) {
                LIns* result = optimizeStringCmpWithStringCall(lhsi, rhsi, icmp, false);
                if (result)
                    return result;
            }
            else if (localGetp(rhsi)->opcode() == LIR_calli) {
                LIns* result = optimizeStringCmpWithStringCall(rhsi, lhsi, icmp, true);
                if (result)
                    return result;
            }
        }
        return NULL;
    }

    // set cc's for < operator
    LIns* CodegenLIR::cmpLt(int lhsi, int rhsi)
    {
        LIns *result = cmpOptimization(lhsi, rhsi, LIR_lti, LIR_ltui, LIR_ltd);
        if (result)
            return result;
        
        // TODO numeric comparisons can be optimized by inlining the fastpath (int/int, number/number)

        AvmAssert(trueAtom == 13);
        AvmAssert(falseAtom == 5);
        AvmAssert(undefinedAtom == 4);
        LIns* lhs = loadAtomRep(lhsi);
        LIns* rhs = loadAtomRep(rhsi);
        LIns* atom = callIns(FUNCTIONID(compare), 2, lhs, rhs);

        // caller will use jt for (a<b) and jf for !(a<b)
        // compare          ^8    <8
        // true       1101  0101   y
        // false      0101  1101   n
        // undefined  0100  1100   n

        LIns* c = InsConst(8);
        return binaryIns(LIR_lti, binaryIns(LIR_xori, p2i(atom), c), c);
    }

    LIns* CodegenLIR::cmpLe(int lhsi, int rhsi)
    {
        LIns *result = cmpOptimization(lhsi, rhsi, LIR_lei, LIR_leui, LIR_led);
        if (result)
            return result;

        // TODO numeric comparisons can be optimized by inlining the fastpath (int/int, number/number)
        LIns* lhs = loadAtomRep(lhsi);
        LIns* rhs = loadAtomRep(rhsi);
        LIns* atom = callIns(FUNCTIONID(compare), 2, rhs, lhs);

        // assume caller will use jt for (a<=b) and jf for !(a<=b)
        // compare          ^1    <=4
        // true       1101  1100  n
        // false      0101  0100  y
        // undefined  0100  0101  n

        LIns* c2 = InsConst(1);
        LIns* c4 = InsConst(4);
        return binaryIns(LIR_lei, binaryIns(LIR_xori, p2i(atom), c2), c4);
    }

    LIns* CodegenLIR::cmpEq(const CallInfo *fid, int lhsi, int rhsi)
    {
        bool isStrict = fid == FUNCTIONID(stricteq);

        LIns *result = cmpOptimization(lhsi, rhsi, LIR_eqi, LIR_eqi, LIR_eqd, isStrict);
        
        if (result )
            return result;

        Traits* lht = state->value(lhsi).traits;
        Traits* rht = state->value(rhsi).traits;

        // There are various conditions we can check for that simplify our equality check down
        // to a ptr comparison:
        //  - null and a type that does not require complex equality checks
        //  - null and a string - no string comparison is performed, just ptrs
        //  - both types do not have complex equality checks (non builtin derived Object types)
        // This does not work for various other types (not a complete list) such as:
        //    string vs string - performs string comparison
        //    number vs string - type conversion is performed
        //    XML types - complex equality checks
        //    OBJECT_TYPE - this can mean even Number and String
        if (((lht == NULL_TYPE) && (rht && (!rht->hasComplexEqualityRules() || rht == STRING_TYPE))) ||
            ((rht == NULL_TYPE) && (lht && (!lht->hasComplexEqualityRules() || lht == STRING_TYPE))) ||
            ((rht && !rht->hasComplexEqualityRules()) && (lht && !lht->hasComplexEqualityRules()))) {
            LIns* lhs = localGetp(lhsi);
            LIns* rhs = localGetp(rhsi);
            return binaryIns(LIR_eqp, lhs, rhs);
        }
        
        // TODO numeric comparisons can be optimized by inlining the fastpath (int/int, number/number)

        if ((lht == rht) && (lht == STRING_TYPE)) {
            LIns* lhs = localGetp(lhsi);
            LIns* rhs = localGetp(rhsi);
            return callIns(FUNCTIONID(String_equals), 2, lhs, rhs, result);
        }

        LIns* lhs = loadAtomRep(lhsi);
        LIns* rhs = loadAtomRep(rhsi);
        LIns* out;
        if (isStrict)
            out = callIns(fid, 2, lhs, rhs);
        else
            out = callIns(fid, 3, coreAddr, lhs, rhs);
        result = binaryIns(LIR_eqp, out, InsConstAtom(trueAtom));
        return result;
    }

    void CodegenLIR::writeEpilogue(const FrameState *state)
    {
        this->state = state;
        this->labelCount = driver->getBlockCount();

        // LIR_regfence instructions are used below to reduce register pressure.
        // The cost of reloading is not an issue for these exceptional cases.
        // A notable exception is the 'catch_label' block, where correctness
        // requires a regfence for an unrelated reason.
        if (mop_rangeCheckFailed_label.unpatchedEdges) {
            emitLabel(mop_rangeCheckFailed_label);
            Ins(LIR_regfence);
            callIns(FUNCTIONID(mop_rangeCheckFailed), 1, env_param);
        }

        if (npe_label.unpatchedEdges) {
            emitLabel(npe_label);
            Ins(LIR_regfence);
            callIns(FUNCTIONID(npe), 1, env_param);
        }

        if (upe_label.unpatchedEdges) {
            emitLabel(upe_label);
            Ins(LIR_regfence);
            callIns(FUNCTIONID(upe), 1, env_param);
        }

        if (call_error_label.unpatchedEdges) {
            emitLabel(call_error_label);
            Ins(LIR_regfence);
            callIns(FUNCTIONID(op_call_error), 1, env_param);
        }

        if (interrupt_label.unpatchedEdges) {
            emitLabel(interrupt_label);
#ifdef VMCFG_INTERRUPT_SAFEPOINT_POLL
            Ins(LIR_pushstate);
#endif 
            Ins(LIR_regfence);
            callIns(FUNCTIONID(handleInterruptMethodEnv), 1, env_param);
#ifdef VMCFG_INTERRUPT_SAFEPOINT_POLL
            Ins(LIR_popstate);
            Ins(LIR_restorepc);
#endif
        }

        if (driver->hasReachableExceptions()) {
            // exception case
            emitLabel(catch_label);

            // This regfence is necessary for correctness,
            // as register contents after a longjmp are unpredictable.
            Ins(LIR_regfence);

            // _ef.beginCatch()
            int stackBase = ms->stack_base();
            LIns* pc = loadIns(LIR_ldp, 0, _save_eip, ACCSET_OTHER);
            LIns* slotAddr = lea(stackBase << VARSHIFT(info) , vars);
            LIns* tagAddr = lea(stackBase, tags);
            LIns* handler_ordinal = callIns(FUNCTIONID(beginCatch), 6, coreAddr, _ef, InsConstPtr(info), pc, slotAddr, tagAddr);

            int handler_count = info->abc_exceptions()->exception_count;
            // Jump to catch handler
            // Find last handler, to optimize branches generated below.
            int i;
            for (i = handler_count-1; i >= 0; i--) {
                ExceptionHandler* h = &info->abc_exceptions()->exceptions[i];
                const uint8_t* handler_pc = code_pos + h->target;
                if (driver->hasFrameState(handler_pc)) break;
            }
            int last_ordinal = i;
            // There should be at least one reachable handler.
            AvmAssert(last_ordinal >= 0);
            // Do a compare & branch to each possible target.
            for (int j = 0; j <= last_ordinal; j++) {
                ExceptionHandler* h = &info->abc_exceptions()->exceptions[j];
                const uint8_t* handler_pc = code_pos + h->target;
                if (driver->hasFrameState(handler_pc)) {
                    CodegenLabel& label = getCodegenLabel(handler_pc);
                    AvmAssert(label.labelIns != NULL);
                    if (j == last_ordinal) {
                        lirout->insBranch(LIR_j, NULL, label.labelIns);
                    } else {
                        LIns* cond = binaryIns(LIR_eqi, handler_ordinal, InsConst(j));
                        branchToLabel(LIR_jt, cond, label);
                    }
                }
            }
            livep(_ef);
            livep(_save_eip);
        }

        if (prolog->env_scope)      livep(prolog->env_scope);
        if (prolog->env_vtable)     livep(prolog->env_vtable);
        if (prolog->env_abcenv)     livep(prolog->env_abcenv);
        if (prolog->env_domainenv)  livep(prolog->env_domainenv);
        if (prolog->env_toplevel)   livep(prolog->env_toplevel);
        if (prolog->env_finddef_table) livep(prolog->env_finddef_table);
        if (restArgc) {
            lirout->ins1(LIR_livei, restArgc);
            livep(ap_param);
        }

        #ifdef DEBUGGER
        if (haveDebugger)
            livep(csn);
        #endif

        // extend live range of critical stuff
        // fixme -- this should be automatic based on live analysis
        livep(methodFrame);
        livep(env_param);
        frag->lastIns = livep(coreAddr);
        skip_ins->overwriteWithSkip(prolog->lastIns);

        // After CodeWriter::writeEpilogue() is called, driver is invalid
        // and could be destructed.  Null out our pointer as a precaution.
        this->driver = NULL;
    }

    // emit code to create a stack-allocated copy of the given multiname.
    // this helper only initializes Multiname.flags and Multiname.next_index
    LIns* CodegenLIR::copyMultiname(const Multiname* multiname)
    {
        LIns* name = insAlloc(sizeof(Multiname));
        sti(InsConst(multiname->ctFlags()), name, offsetof(Multiname, flags), ACCSET_OTHER);
        sti(InsConst(multiname->next_index), name, offsetof(Multiname, next_index), ACCSET_OTHER);
        return name;
    }

    LIns* CodegenLIR::initMultiname(const Multiname* multiname, int& csp, bool isDelete /*=false*/)
    {
        if (!multiname->isRuntime()) {
            // use the precomputed multiname
            return InsConstPtr(multiname);
        }

        // create an initialize a copy of the given multiname
        LIns* _tempname = copyMultiname(multiname);

        // then initialize its name and ns|nsset fields.
        LIns* nameAtom = NULL;
        if (multiname->isRtname())
        {
            nameAtom = loadAtomRep(csp--);
        }
        else
        {
            // copy the compile-time name to the temp name
            LIns* mName = InsConstPtr(multiname->name);
            stp(mName, _tempname, offsetof(Multiname,name), ACCSET_OTHER);
        }

        if (multiname->isRtns())
        {
            // intern the runtime namespace and copy to the temp multiname
            LIns* nsAtom = loadAtomRep(csp--);
            LIns* internNs = callIns(FUNCTIONID(internRtns), 2,
                env_param, nsAtom);

            stp(internNs, _tempname, offsetof(Multiname,ns), ACCSET_OTHER);
        }
        else
        {
            // copy the compile-time namespace to the temp multiname
            LIns* mSpace = InsConstPtr(multiname->ns);
            stp(mSpace, _tempname, offsetof(Multiname, ns), ACCSET_OTHER);
        }

        // Call initMultinameLate as the last step, since if a runtime
        // namespace is present, initMultinameLate may clobber it if a
        // QName is provided as index.
        if (nameAtom)
        {
            if (isDelete)
            {
                callIns(FUNCTIONID(initMultinameLateForDelete), 3,
                        env_param, _tempname, nameAtom);
            }
            else
            {
                callIns(FUNCTIONID(initMultinameLate), 3,
                        coreAddr, _tempname, nameAtom);
            }
        }

        return _tempname;
    }

    LIns* CodegenLIR::mopAddrToRangeCheckedRealAddrAndDisp(LIns* mopAddr, int32_t const size, int32_t* disp)
    {
        AvmAssert(size > 0);    // it's signed to help make the int promotion correct

        if (!mopsRangeCheckFilter) {
            // add a MopsRangeCheckFilter to the back end of the lirout pipeline, just after CseFilter.
            // fixme bug Bug 554030: We must put this after CseFilter and ExprFilter so that
            // the range-check expression using LIR_addi/LIR_subi are not modified (by ExprFilter)
            // and no not become referenced by other unrelated code (by CseFilter).
            AvmAssert(lirout == varTracker);
            mopsRangeCheckFilter = new (*alloc1) MopsRangeCheckFilter(redirectWriter->out, prolog, loadEnvDomainEnv());
            redirectWriter->out = mopsRangeCheckFilter;
        }

        // note, mopAddr and disp are both in/out parameters
        LIns* br = NULL;
        LIns* mopsMemoryBase = mopsRangeCheckFilter->emitRangeCheck(mopAddr, size, disp, br);
        if (br)
            patchLater(br, mop_rangeCheckFailed_label);


        // if mopAddr is a compiletime constant, we still have to do the range-check above
        // (since globalMemorySize can vary at runtime), but we might be able to encode
        // the entire address into the displacement (if any)...
        if (mopAddr->isImmI() && disp != NULL && sumFitsInInt32(*disp, mopAddr->immI()))
        {
            *disp += mopAddr->immI();
            return mopsMemoryBase;
        }

        // (yes, i2p, not u2p... it might legitimately be negative due to the
        // displacement optimization in emitCheck().)
        return binaryIns(LIR_addp, mopsMemoryBase, i2p(mopAddr));
    }

    LIns* CodegenLIR::loadEnvScope()
    {
        LIns* scope = prolog->env_scope;
        if (!scope)
        {
            prolog->env_scope = scope = prolog->insLoad(LIR_ldp, env_param, offsetof(MethodEnv, _scope), ACCSET_OTHER, LOAD_CONST);
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(scope, "env_scope");
            })
            verbose_only( if (vbWriter) { vbWriter->flush(); } )
        }
        return scope;
    }

    LIns* CodegenLIR::loadEnvVTable()
    {
        LIns* vtable = prolog->env_vtable;
        if (!vtable)
        {
            LIns* scope = loadEnvScope();
            prolog->env_vtable = vtable = prolog->insLoad(LIR_ldp, scope, offsetof(ScopeChain, _vtable), ACCSET_OTHER, LOAD_CONST);
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(vtable, "env_vtable");
            })
            verbose_only( if (vbWriter) { vbWriter->flush(); } )
        }
        return vtable;
    }

    LIns* CodegenLIR::loadEnvAbcEnv()
    {
        LIns* abcenv = prolog->env_abcenv;
        if (!abcenv)
        {
            LIns* scope = loadEnvScope();
            prolog->env_abcenv = abcenv = prolog->insLoad(LIR_ldp, scope, offsetof(ScopeChain, _abcEnv), ACCSET_OTHER, LOAD_CONST);
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(abcenv, "env_abcenv");
            })
            verbose_only( if (vbWriter) { vbWriter->flush(); } )
        }
        return abcenv;
    }

    LIns* CodegenLIR::loadEnvDomainEnv()
    {
        LIns* domainenv = prolog->env_domainenv;
        if (!domainenv)
        {
            LIns* abcenv = loadEnvAbcEnv();
            prolog->env_domainenv = domainenv = prolog->insLoad(LIR_ldp, abcenv, offsetof(AbcEnv, m_domainEnv), ACCSET_OTHER, LOAD_CONST);
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(domainenv, "env_domainenv");
            })
            verbose_only( if (vbWriter) { vbWriter->flush(); } )
        }
        return domainenv;
    }

    LIns* CodegenLIR::loadEnvToplevel()
    {
        LIns* toplevel = prolog->env_toplevel;
        if (!toplevel)
        {
            LIns* vtable = loadEnvVTable();
            prolog->env_toplevel = toplevel = prolog->insLoad(LIR_ldp, vtable, offsetof(VTable, _toplevel), ACCSET_OTHER, LOAD_CONST);
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(toplevel, "env_toplevel");
            })
            verbose_only( if (vbWriter) { vbWriter->flush(); } )
        }
        return toplevel;
    }

    LIns* CodegenLIR::loadEnvFinddefTable()
    {
        LIns* table = prolog->env_finddef_table;
        if (!table) {
            LIns* abc = loadEnvAbcEnv();
            prolog->env_finddef_table = table = prolog->insLoad(LIR_ldp, abc, offsetof(AbcEnv, m_finddef_table), ACCSET_OTHER, LOAD_CONST);
            verbose_only( if (vbNames) {
                vbNames->lirNameMap->addName(table, "env_finddef_table");
            })
            verbose_only( if (vbWriter) { vbWriter->flush(); } )
        }
        return table;
    }

    /**
     * given an object and type, produce code that loads the VTable for
     * the object.  Handles all types: primitive vables get loaded from
     * Toplevel, Object and * vtables get loaded by calling the toVTable() helper.
     * ScriptObject* vtables are loaded from the ScriptObject.
     */
    LIns* CodegenLIR::loadVTable(LIns* obj, Traits* t)
    {
        if (t && !t->isMachineType() && t != STRING_TYPE && t != NAMESPACE_TYPE && t != NULL_TYPE)
        {
            // must be a pointer to a scriptobject, and we've done the n
            // all other types are ScriptObject, and we've done the null check
            return loadIns(LIR_ldp, offsetof(ScriptObject, vtable), obj, ACCSET_OTHER, LOAD_CONST);
        }

        LIns* toplevel = loadEnvToplevel();
        int offset;
        if (t == NAMESPACE_TYPE)    offset = offsetof(Toplevel, _namespaceClass);
        else if (t == STRING_TYPE)  offset = offsetof(Toplevel, _stringClass);
        else if (t == BOOLEAN_TYPE) offset = offsetof(Toplevel, _booleanClass);
        else if (t == NUMBER_TYPE)  offset = offsetof(Toplevel, _numberClass);
#ifdef VMCFG_FLOAT
        else if (t == FLOAT_TYPE)   offset = offsetof(Toplevel, _floatClass);
        else if (t == FLOAT4_TYPE)  offset = offsetof(Toplevel, _float4Class );
#endif        
        else if (t == INT_TYPE)     offset = offsetof(Toplevel, _intClass);
        else if (t == UINT_TYPE)    offset = offsetof(Toplevel, _uintClass);
        else
        {
            // *, Object or Void
            LIns* atom = nativeToAtom(obj, t);
            return callIns(FUNCTIONID(toVTable), 2, toplevel, atom);
        }

        // now offset != -1 and we are returning a primitive vtable

        LIns* cc = loadIns(LIR_ldp, offset, toplevel, ACCSET_OTHER, LOAD_CONST);
        LIns* cvtable = loadIns(LIR_ldp, offsetof(ClassClosure, vtable), cc, ACCSET_OTHER, LOAD_CONST);
        return loadIns(LIR_ldp, offsetof(VTable, ivtable), cvtable, ACCSET_OTHER, LOAD_CONST);
    }

    LIns* CodegenLIR::promoteNumberIns(Traits* t, int i)
    {
        if (t == NUMBER_TYPE)
            return localGetd(i);
        if (t == INT_TYPE || t == BOOLEAN_TYPE)
            return i2dIns(localGet(i));
        if (t == UINT_TYPE)
            return ui2dIns(localGet(i));
#ifdef VMCFG_FLOAT
        if(t == FLOAT_TYPE)
            return f2dIns(localGetf(i));
        AvmAssert(t == FLOAT4_TYPE);
#else
        AvmAssert(!"Unhandled type in promoteNumberIns");
#endif
        return InsConstDbl(MathUtils::kNaN);
    }

#ifdef VMCFG_FLOAT
    LIns* CodegenLIR::promoteFloatIns(Traits* t, int i)
    {
        if (t == FLOAT_TYPE)
        {
            return localGetf(i);
        }
        if (t == FLOAT4_TYPE)
        {
            return InsConstFlt(AvmCore::atomToFloat(core->kFltNaN));
        }
        if(t == NUMBER_TYPE)
            return d2fIns(localGetd(i));
        if (t == INT_TYPE || t == BOOLEAN_TYPE)
        {
            return i2fIns(localGet(i));
        }
        AvmAssert(t == UINT_TYPE);
        return ui2fIns(localGet(i));
    }

    LIns* CodegenLIR::promoteFloat4Ins(Traits* t, int i)
    {
        // This function hadles * -> Float4, too.
        // That is because if any operand is float4, the other needs to be coerced to float4
        switch(bt(t)){
            case BUILTIN_float:
            case BUILTIN_number:
            case BUILTIN_int:
            case BUILTIN_uint:
            case BUILTIN_boolean:
                return Ins(LIR_f2f4, promoteFloatIns(t,i));
            case BUILTIN_float4: 
                return localGetf4(i);
            default:
            {
                LIns* retval = lirout->insAlloc(sizeof(float4_t));
                callIns(FUNCTIONID(float4), 2, lea(0, retval), loadAtomRep(i));
                return ldf4(retval,0,ACCSET_OTHER);
            }
        }
    }
#endif // VMCFG_FLOAT

    /// set position of a label and patch all pending jumps to point here.
    void CodegenLIR::emitLabel(CodegenLabel& label) 
    {
        
        // specialized calls shouldn't be re-used across labels,
        // otherwise we might use one that isn't valid for a given
        // code path. simplest fix is to just clear the map when
        // we see a label.
        if (specializedCallHashMap)
            specializedCallHashMap->clear();
    
        varTracker->trackLabel(label, state);

        // patch all unpatched branches to this label
        LIns* labelIns = label.labelIns;
        bool jtbl_forward_target = false;
        for (Seq<InEdge>* p = label.unpatchedEdges; p != NULL; p = p->tail) {
            InEdge& patch = p->head;
            LIns* branchIns = patch.branchIns;
            if (branchIns->isop(LIR_jtbl)) {
                jtbl_forward_target = true;
                branchIns->setTarget(patch.index, labelIns);
            } else {
                AvmAssert(branchIns->isBranch() && patch.index == 0);
                branchIns->setTarget(labelIns);
            }
        }
        if (jtbl_forward_target) {
            // A jtbl (switch) jumps forward to here, creating a situation our
            // register allocator cannot handle; force regs to be loaded at the
            // start of this block.
            Ins(LIR_regfence);
        }

#ifdef NJ_VERBOSE
        if (vbNames && label.name)
            vbNames->lirNameMap->addName(label.labelIns, label.name);
#endif
    }

#ifdef DEBUGGER
    void CodegenLIR::emitSampleCheck()
    {
        /* @todo inline the sample check code, help!  */
        callIns(FUNCTIONID(sampleCheck), 1, coreAddr);
    }
#endif

#ifdef NJ_VERBOSE
    bool CodegenLIR::verbose()
    {
        return pool->isVerbose(VB_jit, info);
    }
#endif

    // emit a conditional branch to the given label.  If we have already emitted
    // code for that label then the branch is complete.  If not then add a patch
    // record to the label, which will patch the branch when the label position
    // is reached.  cond == NULL for unconditional branches (LIR_j).
    void CodegenLIR::branchToLabel(LOpcode op, LIns *cond, CodegenLabel& label) {
        if (cond && !cond->isCmp()) {
            // branching on a non-condition expression, so test (v==0)
            // and invert the sense of the branch.
            cond = eqi0(cond);
            op = invertCondJmpOpcode(op);
        }
        LIns* labelIns = label.labelIns;
        LIns* br = lirout->insBranch(op, cond, labelIns);
        if (br != NULL) {
            if (labelIns != NULL) {
                varTracker->checkBackEdge(label, state);
            } else {
                label.unpatchedEdges = new (*alloc1) Seq<InEdge>(InEdge(br), label.unpatchedEdges);
                varTracker->trackForwardEdge(label);
            }
        } else {
            // branch was optimized away.  do nothing.
        }
    }

    LIns* CodegenLIR::branchJovToLabel(LOpcode op, LIns *a, LIns *b, CodegenLabel& label) {
        LIns* labelIns = label.labelIns;
        LIns* result = lirout->insBranchJov(op, a, b, labelIns);
        NanoAssert(result);
        if (result->isop(op)) {
            if (labelIns != NULL) {
                varTracker->checkBackEdge(label, state);
            } else {
                label.unpatchedEdges = new (*alloc1) Seq<InEdge>(InEdge(result), label.unpatchedEdges);
                varTracker->trackForwardEdge(label);
            }
        } else {
            // The root operator of the expression has been eliminated via
            // constant folding or other simplification.  This is only valid
            // if no overflow is possible, in which case the branch is not needed.
            // Do nothing.
        }
        return result;
    }

    // emit a relative branch to the given ABC pc-offset by mapping pc
    // to a corresponding CodegenLabel, and creating a new one if necessary
    void CodegenLIR::branchToAbcPos(LOpcode op, LIns *cond, const uint8_t* pc) {
        CodegenLabel& label = getCodegenLabel(pc);
        branchToLabel(op, cond, label);
    }

    CodegenLabel& CodegenLIR::createLabel(const char* name) {
        return *(new (*alloc1) CodegenLabel(name));
    }

    CodegenLabel& CodegenLIR::createLabel(const char* prefix, int id) {
        CodegenLabel* label = new (*alloc1) CodegenLabel();
#ifdef NJ_VERBOSE
        if (vbNames) {
            char *name = new (*lir_alloc) char[VMPI_strlen(prefix)+16];
            VMPI_sprintf(name, "%s%d", prefix, id);
            label->name = name;
        }
#else
        (void) prefix;
        (void) id;
#endif
        return *label;
    }

    CodegenLabel& CodegenLIR::getCodegenLabel(const uint8_t* pc) {
        AvmAssert(driver->hasFrameState(pc));
        if (!blockLabels)
            blockLabels = new (*alloc1) HashMap<const uint8_t*,CodegenLabel*>(*alloc1, driver->getBlockCount());
        CodegenLabel* label = blockLabels->get(pc);
        if (!label) {
            label = new (*alloc1) CodegenLabel();
            blockLabels->put(pc, label);
        }
#ifdef NJ_VERBOSE
        if (!label->name && vbNames) {
            char *name = new (*lir_alloc) char[16];
            VMPI_sprintf(name, "B%d", int(pc - code_pos));
            label->name = name;
        }
#endif
        return *label;
    }

    /// connect to a label for one entry of a switch
    void CodegenLIR::patchLater(LIns* jtbl, const uint8_t* pc, uint32_t index) {
        CodegenLabel& target = getCodegenLabel(pc);
        if (target.labelIns != 0) {
            jtbl->setTarget(index, target.labelIns);           // backward edge
            varTracker->checkBackEdge(target, state);
        } else {
            target.unpatchedEdges = new (*alloc1) Seq<InEdge>(InEdge(jtbl, index), target.unpatchedEdges);
            varTracker->trackForwardEdge(target);
        }
    }

    void CodegenLIR::patchLater(LIns *br, CodegenLabel &target) {
        if (!br) return; // occurs if branch was unconditional and thus never emitted.
        if (target.labelIns != 0) {
            br->setTarget(target.labelIns); // backwards edge
            varTracker->checkBackEdge(target, state);
        } else {
            target.unpatchedEdges = new (*alloc1) Seq<InEdge>(InEdge(br), target.unpatchedEdges);
            varTracker->trackForwardEdge(target);
        }
    }

    LIns* CodegenLIR::insAlloc(int32_t size) {
        return lirout->insAlloc(size >= 4 ? size : 4);
    }

    LIns* CodegenLIR::insAllocForTraits(Traits *t)
    {
        switch (bt(t)) {
#ifdef VMCFG_FLOAT
            case BUILTIN_float: 
                return insAlloc(sizeof(float));
            case BUILTIN_float4: 
                return insAlloc(sizeof(float4_t));
#endif // VMCFG_FLOAT
            case BUILTIN_number:
                return insAlloc(sizeof(double));
            case BUILTIN_int:
            case BUILTIN_uint:
            case BUILTIN_boolean:
                return insAlloc(sizeof(int32_t));
            default:
                return insAlloc(sizeof(intptr_t));
        }
    }

    CodeMgr::CodeMgr(nanojit::Config* config) : codeAlloc(config), bindingCaches(NULL), jit_mgr(NULL)
    {
        verbose_only( log.lcbits = 0; )
    }

    void CodeMgr::flushBindingCaches()
    {
        // this clears vtable so all kObjectType receivers are invalidated.
        // of course, this field is also "tag" for primitive receivers,
        // but 0 is never a legal value there (and this is asserted when the tag is set)
        // so this should safely invalidate those as well (though we don't really need to invalidate them)
        for (BindingCache* b = bindingCaches; b != NULL; b = b->next)
            b->vtable = NULL;
    }

    void analyze_edge(LIns* label, nanojit::BitSet &livein,
                      LabelBitSet& labels, InsList* looplabels)
    {
        nanojit::BitSet *lset = labels.get(label);
        if (lset) {
            livein.setFrom(*lset);
        } else {
            AvmAssertMsg(looplabels != NULL, "Unexpected back-edge");
            looplabels->add(label);
        }
    }

    REALLY_INLINE void update_var_liveness(nanojit::BitSet* varlivein, nanojit::BitSet* taglivein, int varIdx, const bool isVarStore)
    {
        if(isVarStore) {
            varlivein->clear(varIdx);
            if(taglivein) taglivein->clear(varIdx);
        } else {
            varlivein->set(varIdx);
            if(taglivein) taglivein->set(varIdx);
        }
        
    }
    
    // Treat the calculated address of addp(vars, const) as the target
    // of a load(or store) from/to the variable pointed to, as well as its associated tag (if a tag bitset is passed).
    bool analyze_addp(LIns* ins, LIns* vars, nanojit::BitSet* varlivein, nanojit::BitSet* taglivein, const size_t varShift, const bool isStore)
    {
        AvmAssert(ins->isop(LIR_addp));
        if (ins->oprnd1() == vars && ins->oprnd2()->isImmP()) {
            AvmAssert(IS_ALIGNED(ins->oprnd2()->immP(), 1 << varShift));
            int d = int(uintptr_t(ins->oprnd2()->immP()) >> varShift);
            update_var_liveness(varlivein, taglivein, d, isStore);
            return true;
        }
        return false;
    }

    void analyze_call(LIns* ins, LIns* catcher, LIns* vars, DEBUGGER_ONLY(bool haveDebugger, int dbg_framesize,)
            const size_t varShift,
            nanojit::BitSet& varlivein, LabelBitSet& varlabels,
            nanojit::BitSet& taglivein, LabelBitSet& taglabels)
    {
        typedef struct FuncLiveInfo{
            const CallInfo* callinfo;
            int32_t var_argument; /* NOTE! arguments are counted from the end! */
            bool isStore;
            bool tagAccess;
            bool mustBeVarAccess;
        } FuncLiveInfo;

        static const FuncLiveInfo varPtrFunctions[] = {
            // beginCatch(core, ef, info, pc, &vars[i], &tags[i]) => store to &vars[i] and &tag[i]
            { FUNCTIONID(beginCatch),                                1, true,  true,  true  }, 
            // makeatom(core, &vars[index], tag[index]) => treat as load from &vars[index]
            { FUNCTIONID(makeatom),                                  1, false, true, true  }, 
            // restargHelper(Toplevel*, Multiname*, Atom, ArrayObject**, uint32_t, Atom*) => arrayObject** might be reference to a var
            { FUNCTIONID(restargHelper),                             2, false, false, false }, 
#ifdef VMCFG_FLOAT
            // float4(float4_t* result, Atom val) => result might point to a local var.
            { FUNCTIONID(float4),                                    1, true, false, false  },  
            // Float4VectorObject::_getNativeUintProperty(this, result, index) => result might point to a local var.
            { FUNCTIONID(Float4VectorObject_getNativeUintProperty),  1, true, false, false  },  
            // Float4VectorObject::_getNativeIntProperty(this, result, index) => result might point to a local var.
            { FUNCTIONID(Float4VectorObject_getNativeIntProperty),   1, true, false, false  },  
            // Float4VectorObject::_getNativeDoubleProperty(this, result, index) => result might point to a local var.
            { FUNCTIONID(Float4VectorObject_getNativeDoubleProperty),1, true, false, false  },  
            // Float4VectorObject::_setFloat4UintProperty(this, index, value) => value might point to a local var.            
            { FUNCTIONID(Float4VectorObject_setNativeUintProperty),  0, false, false, false },  
            // Float4VectorObject::_setFloat4IntProperty(this, index, value) => value might point to a local var.            
            { FUNCTIONID(Float4VectorObject_setNativeIntProperty),   0, false, false, false },  
            // Float4VectorObject::_setFloat4DoubleProperty(this, index, value) => value might point to a local var.            
            { FUNCTIONID(Float4VectorObject_setNativeDoubleProperty),0, false, false, false },
#endif
        };
        const int varPtrFunctionsNum = (int) (sizeof(varPtrFunctions) / sizeof(FuncLiveInfo));
        
        const CallInfo* ci = ins->callInfo();
        for(int i=0;i < varPtrFunctionsNum; i++ ) {
            if(ci == varPtrFunctions[i].callinfo) {
                uint32_t argIdx = varPtrFunctions[i].var_argument;
                bool updated = false;
                LIns* varPtrArg = ins->arg(argIdx);  // varPtrArg == vars, OR addp(vars, index)
                if (varPtrArg == vars) {
                    update_var_liveness(&varlivein, varPtrFunctions[i].tagAccess? &taglivein : NULL, 0, varPtrFunctions[i].isStore);
                    updated = true;
                } else 
                if (varPtrArg->isop(LIR_addp)) {
                    updated = analyze_addp(varPtrArg, vars, &varlivein, varPtrFunctions[i].tagAccess? &taglivein : NULL, varShift, varPtrFunctions[i].isStore);
                } 
                AvmAssert(updated || !varPtrFunctions[i].mustBeVarAccess);
                return; // don't continue, we handled the call. 
            }
        }
        
        if (!ins->callInfo()->_isPure) {
            if (catcher) {
                // non-cse call is like a conditional forward branch to the catcher label.
                // this could be made more precise by checking whether this call
                // can really throw, and only processing edges to the subset of
                // reachable catch blocks.  If we haven't seen the catch label yet then
                // the call is to an exception handling helper (eg beginCatch())
                // that won't throw.
                nanojit::BitSet *varlset = varlabels.get(catcher);
                if (varlset)
                    varlivein.setFrom(*varlset);
                nanojit::BitSet *taglset = taglabels.get(catcher);
                if (taglset)
                    taglivein.setFrom(*taglset);
            }
#ifdef DEBUGGER
            if (haveDebugger) {
                // all vars and scopes must be considered "read" by any call
                // the debugger can stop in.  The debugger also will access tags[].
                for (int i = 0, n = dbg_framesize; i < n; i++) {
                    varlivein.set(i);
                    taglivein.set(i);
                }
            }
#endif
        }
    }

    // Analyze a LIR_label.  We're at the top of a block, save livein for
    // this block so it can be propagated to predecessors.
    bool analyze_label(LIns* i, Allocator& alloc, bool again,
            int framesize, InsList* looplabels,
            nanojit::BitSet& varlivein, LabelBitSet& varlabels,
            nanojit::BitSet& taglivein, LabelBitSet& taglabels)
    {
        nanojit::BitSet *var_lset = varlabels.get(i);
        if (!var_lset) {
            var_lset = new (alloc) nanojit::BitSet(alloc, framesize);
            varlabels.put(i, var_lset);
        }
        if (var_lset->setFrom(varlivein) && !again) {
            for (Seq<LIns*>* p = looplabels->get(); p != NULL; p = p->tail) {
                if (p->head == i) {
                    again = true;
                    break;
                }
            }
        }
        nanojit::BitSet *tag_lset = taglabels.get(i);
        if (!tag_lset) {
            tag_lset = new (alloc) nanojit::BitSet(alloc, framesize);
            taglabels.put(i, tag_lset);
        }
        if (tag_lset->setFrom(taglivein) && !again) {
            for (Seq<LIns*>* p = looplabels->get(); p != NULL; p = p->tail) {
                if (p->head == i) {
                    again = true;
                    break;
                }
            }
        }
        return again;
    }

    void CodegenLIR::deadvars_analyze(Allocator& alloc,
            nanojit::BitSet& varlivein, LabelBitSet& varlabels,
            nanojit::BitSet& taglivein, LabelBitSet& taglabels)
    {
        LIns *catcher = this->catch_label.labelIns;
        InsList looplabels(alloc);

        verbose_only(int iter = 0;)
        bool again;
        do {
            again = false;
            varlivein.reset();
            taglivein.reset();
            LirReader in(frag->lastIns);
            for (LIns *i = in.read(); !i->isop(LIR_start); i = in.read()) {
                LOpcode op = i->opcode();
                switch (op) {
                case LIR_reti:
                CASE64(LIR_retq:)
                case LIR_retd:
                case LIR_retf:
                case LIR_retf4:
                    varlivein.reset();
                    taglivein.reset();
                    break;
                CASE64(LIR_stq:)
                case LIR_sti:
                case LIR_std:
                case LIR_std2f:
                case LIR_stf:
                case LIR_stf4:
                case LIR_sti2c:
                    if (i->oprnd2() == vars) {
                        AvmAssert( IS_ALIGNED(i->disp(), (1 << VARSHIFT(info))) );
                        int d = i->disp() >> VARSHIFT(info);
                        varlivein.clear(d);
                    } else if (i->oprnd2() == tags) {
                        int d = i->disp(); // 1 byte per tag
                        taglivein.clear(d);
                    }
                    break;
                case LIR_addp:
                    // treat pointer calculations into vars as a read from vars
                    // FIXME: Bug 569677
                    // This is extremely fragile.  There is no reason to suppose
                    // that the address will be used for a read rather than a store,
                    // other than that to do so would break the deadvars analysis
                    // because of the dodgy assumption made here.
                    analyze_addp(i, vars, &varlivein, NULL, VARSHIFT(info), false);
                    break;
                CASE64(LIR_ldq:)
                case LIR_ldi:
                case LIR_ldd:
                case LIR_ldf2d:
                case LIR_ldf:
                case LIR_ldf4:
                case LIR_lduc2ui: case LIR_ldc2i:
                    if (i->oprnd1() == vars) {
                        AvmAssert( IS_ALIGNED(i->disp(), (1 << VARSHIFT(info))) );
                        int d = i->disp() >> VARSHIFT(info);
                        varlivein.set(d);
                    }
                    else if (i->oprnd1() == tags) {
                        int d = i->disp(); // 1 byte per tag
                        taglivein.set(d);
                    }
                    break;
                case LIR_label:
                    again |= analyze_label(i, alloc, again, framesize, &looplabels,
                            varlivein, varlabels, taglivein, taglabels);
                    break;
                case LIR_j:
                    // the fallthrough path is unreachable, clear it.
                    varlivein.reset();
                    taglivein.reset();
                    // fall through to other branch cases
                case LIR_jt:
                case LIR_jf:
                    // merge the LiveIn sets from each successor:  the fall
                    // through case (livein) and the branch case (lset).
                    analyze_edge(i->getTarget(), varlivein, varlabels, &looplabels);
                    analyze_edge(i->getTarget(), taglivein, taglabels, &looplabels);
                    break;
                case LIR_jtbl:
                    varlivein.reset(); // fallthrough path is unreachable, clear it.
                    taglivein.reset(); // fallthrough path is unreachable, clear it.
                    for (uint32_t j=0, n=i->getTableSize(); j < n; j++) {
                        analyze_edge(i->getTarget(j), varlivein, varlabels, &looplabels);
                        analyze_edge(i->getTarget(j), taglivein, taglabels, &looplabels);
                    }
                    break;
                CASE64(LIR_callq:)
                case LIR_calli:
                case LIR_calld:
                case LIR_callf:
                case LIR_callf4:
                case LIR_callv:
                    analyze_call(i, catcher, vars, DEBUGGER_ONLY(haveDebugger, dbg_framesize,) VARSHIFT(info),
                            varlivein, varlabels, taglivein, taglabels);
                    break;
                }
            }
            verbose_only(iter++;)
        }
        while (again);

        // now make a final pass, modifying LIR to delete dead stores (make them LIR_neartramps)
        verbose_only( if (pool->isVerbose(LC_Liveness, info))
            AvmLog("killing dead stores after %d LA iterations.\n",iter);
        )
    }

    // Erase the instruction by rewriting it as a skip.
    // TODO this can go away if we turn this kill pass into a LirReader
    // and do the work inline with the assembly pass.
    void CodegenLIR::eraseIns(LIns* ins, LIns* prevIns)
    {
#ifdef NJ_VERBOSE
      LInsPrinter *printer = frag->lirbuf->printer;
      bool verbose = printer && pool->isVerbose(LC_AfterDCE, info);
      if (verbose) {
          InsBuf b;
          AvmLog("- %s\n", printer->formatIns(&b, ins));
      }
#endif
      ins->overwriteWithSkip(prevIns);
    }

    void CodegenLIR::deadvars_kill(Allocator& alloc,
            nanojit::BitSet& varlivein, LabelBitSet& varlabels,
            nanojit::BitSet& taglivein, LabelBitSet& taglabels)
    {
#ifdef NJ_VERBOSE
        LInsPrinter *printer = frag->lirbuf->printer;
        bool verbose = printer && pool->isVerbose(LC_AfterDCE, info);
        InsBuf b;
#endif
        LIns *catcher = this->catch_label.labelIns;
        varlivein.reset();
        taglivein.reset();
        bool tags_touched = false;
        bool vars_touched = false;
        LirReader in(frag->lastIns);
        for (LIns *i = in.read(); !i->isop(LIR_start); i = in.read()) {
            LOpcode op = i->opcode();
            switch (op) {
                case LIR_reti:
                CASE64(LIR_retq:)
                case LIR_retd:
                case LIR_retf:
                case LIR_retf4:
                    varlivein.reset();
                    taglivein.reset();
                    break;
                CASE64(LIR_stq:)
                case LIR_sti:
                case LIR_std:
                case LIR_std2f:
                case LIR_stf:
                case LIR_stf4:
                case LIR_sti2c:
                    if (i->oprnd2() == vars) {
                        AvmAssert( IS_ALIGNED(i->disp(), (1 << VARSHIFT(info))) );
                        int d = i->disp() >> VARSHIFT(info);
                        if (!varlivein.get(d)) {
                            eraseIns(i, in.peek());
                            continue;
                        } else {
                            varlivein.clear(d);
                            vars_touched = true;
                        }
                    }
                    else if (i->oprnd2() == tags) {
                        int d = i->disp(); // 1 byte per tag
                        if (!taglivein.get(d)) {
                            eraseIns(i, in.peek());
                            continue;
                        } else {
                            // keep the store
                            taglivein.clear(d);
                            tags_touched = true;
                        }
                    }
                    break;
                case LIR_addp:
                    // treat pointer calculations into vars as a read from vars
                    analyze_addp(i, vars, &varlivein, NULL, VARSHIFT(info), false);
                    break;
                CASE64(LIR_ldq:)
                case LIR_ldi:
                case LIR_ldd:
                case LIR_ldf2d:
                case LIR_ldf:
                case LIR_ldf4:
                case LIR_lduc2ui: case LIR_ldc2i:
                    if (i->oprnd1() == vars) {
                        AvmAssert( IS_ALIGNED(i->disp(), (1 << VARSHIFT(info))) );
                        int d = i->disp() >> VARSHIFT(info);
                        varlivein.set(d);
                    }
                    else if (i->oprnd1() == tags) {
                        int d = i->disp(); // 1 byte per tag
                        taglivein.set(d);
                    }
                    break;
                case LIR_label:
                    analyze_label(i, alloc, true, framesize, 0,
                            varlivein, varlabels, taglivein, taglabels);
                    break;
                case LIR_j:
                    // the fallthrough path is unreachable, clear it.
                    varlivein.reset();
                    taglivein.reset();
                    // fall through to other branch cases
                case LIR_jt:
                case LIR_jf:
                    // merge the LiveIn sets from each successor:  the fall
                    // through case (live) and the branch case (lset).
                    analyze_edge(i->getTarget(), varlivein, varlabels, 0);
                    analyze_edge(i->getTarget(), taglivein, taglabels, 0);
                    break;
                case LIR_jtbl:
                    varlivein.reset();
                    taglivein.reset();
                    for (uint32_t j = 0, n = i->getTableSize(); j < n; j++) {
                        analyze_edge(i->getTarget(j), varlivein, varlabels, 0);
                        analyze_edge(i->getTarget(j), taglivein, taglabels, 0);
                    }
                    break;
                CASE64(LIR_callq:)
                case LIR_calli:
                case LIR_calld:
                case LIR_callf:
                case LIR_callf4:
                case LIR_callv:
                    analyze_call(i, catcher, vars, DEBUGGER_ONLY(haveDebugger, dbg_framesize,) VARSHIFT(info),
                            varlivein, varlabels, taglivein, taglabels);
                    break;
            }
            verbose_only(if (verbose) {
                AvmLog("  %s\n", printer->formatIns(&b, i));
            })
        }
        // if we have not removed all stores to the tags array, mark it live
        // so its live range will span loops.
        if (tags_touched)
            livep(tags);
        if (vars_touched)
            livep(vars);
    }

    /*
     * this is iterative live variable analysis.  We walk backwards through
     * the code.  when we see a load, we mark the variable live, and when
     * we see a store, we mark it dead.  Dead stores are dropped, not returned
     * by read().
     *
     * at labels, we save the liveIn set associated with that label.
     *
     * at branches, we merge the liveIn sets from the fall through case (which
     * is the current set) and the branch case (which was saved with the label).
     * this filter can be run multiple times, which is required to pick up
     * loop-carried live variables.
     *
     * once the live sets are stable, the DeadVars.kill flag is set to cause the filter
     * to not only drop dead stores, but overwrite them as tramps so they'll be
     * ignored by any later passes even without using this filter.
     */

    void CodegenLIR::deadvars()
    {
        // allocator used only for duration of this phase.  no exceptions are
        // thrown while this phase runs, hence no try/catch is necessary.
        Allocator dv_alloc;

        // map of label -> bitset, tracking what is livein at each label.
        // populated by deadvars_analyze, then used by deadvars_kill.
        // Estimated number of required buckets is based on the driver's block count,
        // which is slightly below the actual # of labels in LIR.  Being slightly low
        // is okay for a bucket hashtable.  note: labelCount is 0 for simple 1-block
        // methods, so use labelCount+1 as the estimate to ensure we have >0 buckets.
        LabelBitSet varlabels(dv_alloc, labelCount + 1);
        LabelBitSet taglabels(dv_alloc, labelCount + 1);

        // scratch bitset used by both dv_analyze and dv_kill.  Each resets
        // the bitset before using it.  creating it here saves one allocation.
        nanojit::BitSet varlivein(dv_alloc, framesize);
        nanojit::BitSet taglivein(dv_alloc, framesize);

        // If catch_label.labelIns is non-null, we emitted an exception
        // handler dispatcher, which generates back-edges into the method.
        if (varTracker->hasBackedges() || catch_label.labelIns)
            deadvars_analyze(dv_alloc, varlivein, varlabels, taglivein, taglabels);
        deadvars_kill(dv_alloc, varlivein, varlabels, taglivein, taglabels);
    }

#ifdef NJ_VERBOSE
    void listing(const char* title, AvmLogControl &log, Fragment* frag)
    {
        LirReader reader(frag->lastIns);
        Allocator lister_alloc;
        ReverseLister lister(&reader, lister_alloc, frag->lirbuf->printer, &log, title);
        for (LIns* ins = lister.read(); !ins->isop(LIR_start); ins = lister.read())
        {}
        lister.finish();
    }

    // build a filename based on path/title
    static const char* filenameFor(const char* path, const char* name, const char* ext, Allocator& alloc)
    {
        char* filename = new (alloc) char[1+VMPI_strlen(name)+VMPI_strlen(path)+VMPI_strlen(ext)];
        VMPI_strcpy(filename, path);
        VMPI_strcat(filename, "/");  // sorry windows
        char* dst = &filename[VMPI_strlen(filename)];
        for(const char* s = name; *s; s++) {
            char c = *s;
            c = VMPI_isalnum(*s) ? c : '.';  // convert non-alpha to dots
            c = ( c == '.' && dst[-1] == '.' ) ? 0 : c;  // dont print multiple dots
            if (c)
                *dst++ = c;
        }
        dst = dst[-1] == '.' ? dst-1 : dst; // rm trailing dot
        VMPI_strcpy(dst, ext);
        return filename;
    }

    // write out a control flow graph of LIR instructions
    static void lircfg(FILE* f, const Fragment* frag, InsSet* ignore, Allocator& alloc, CfgLister::CfgMode mode)
    {
        LirReader  reader(frag->lastIns);
        CfgLister  cfg(&reader, alloc, mode);

        for (LIns* ins = cfg.read(); !ins->isop(LIR_start); ins = cfg.read())
        {}

        cfg.printGmlCfg(f, frag->lirbuf->printer, ignore);
        fclose(f);
    }

#endif

    JITDebugInfo* CodegenLIR::initJitDebugInfo()
    {
        // allocate JITDebugInfo for any JITObserver
        if (!jit_debug_info) {
            jit_debug_info = new (*lir_alloc) JITDebugInfo();
            jit_debug_info->info = 0;
        }
        return jit_debug_info;
    }

    // return pointer to generated code on success, NULL on failure (frame size too large)
    GprMethodProc CodegenLIR::emitMD()
    {
        deadvars();  // deadvars_kill() will add livep(vars) or livep(tags) if necessary

        // do this very last so it's after livep(vars)
        frag->lastIns = livep(undefConst);

        PERFM_NTPROF_BEGIN("compile");
        mmfx_delete( alloc1 );
        alloc1 = NULL;

        CodeMgr *mgr = pool->codeMgr;
        #ifdef NJ_VERBOSE
        if (pool->isVerbose(LC_ReadLIR, info)) {
            StringBuffer sb(core);
            sb << info;
            core->console << "Final LIR " << info;
            listing(sb.c_str(), mgr->log, frag);
        }
        if (pool->isVerbose(LC_Liveness, info)) {
            Allocator live_alloc;
            LirReader in(frag->lastIns);
            nanojit::live(&in, live_alloc, frag, &mgr->log);
        }
        if (pool->isVerbose(LC_AfterDCE | LC_Native, info)) {
            StringBuffer sb(core);
            sb << info;
            mgr->log.printf("jit-assembler %s\n", sb.c_str());
        }
        if (pool->isVerbose(VB_lircfg, info)) {
            // For the control-flow graph :
            //   The list of instructions that we don't want to show explicit
            //   edges for are added to the 'ignore' set.
            Allocator alloc;
            bool hideCommonEdges = true;
            InsSet ignore(alloc);
            if (hideCommonEdges)
            {
                ignore.put(npe_label.labelIns, true);
                ignore.put(upe_label.labelIns, true);
                ignore.put(interrupt_label.labelIns, true);
                ignore.put(mop_rangeCheckFailed_label.labelIns, true);
                ignore.put(catch_label.labelIns, true);
                ignore.put(call_error_label.labelIns, true);
            }

            // type of cfg graph to produce
            CfgLister::CfgMode mode = pool->isVerbose(VB_lircfg_ins, info) ? CfgLister::CFG_INS
                                    : pool->isVerbose(VB_lircfg_bb, info)  ? CfgLister::CFG_BB
                                    : CfgLister::CFG_EBB;

            StringBuffer sb(core);
            sb << info;  // method name
            const char* filename = filenameFor(".", sb.c_str(), ".gml", alloc);
            FILE* f = fopen(filename, "w");
            lircfg(f, frag, &ignore, alloc, mode);
            fclose(f);
        }
        #endif

        // Use the 'active' log if we are in verbose output mode otherwise sink the output
        LogControl* log = &(mgr->log);
        verbose_only(
            SinkLogControl sink;
            log = pool->isVerbose(VB_jit,info) ? log : &sink;
        )

        Assembler *assm = new (*lir_alloc) Assembler(mgr->codeAlloc, mgr->allocator, *lir_alloc, log, core->config.njconfig);
        #ifdef VMCFG_VTUNE
        assm->vtuneHandle = vtuneInit(info->getMethodName());
        #endif /* VMCFG_VTUNE */

        assm->setNoiseGenerator( &noise );

        verbose_only(
            StringList asmOutput(*lir_alloc);
            if (!pool->isVerbose(VB_raw, info))
                assm->_outputCache = &asmOutput;
        );

        assm->beginAssembly(frag);
        LirReader reader(frag->lastIns);
        assm->assemble(frag, &reader);
        assm->endAssembly(frag);
        PERFM_NTPROF_END("compile");

        verbose_only(
            assm->_outputCache = 0;
            for (Seq<char*>* p = asmOutput.get(); p != NULL; p = p->tail) {
                assm->outputf("%s", p->head);
            }
        );

        PERFM_NVPROF("IR-bytes", frag->lirbuf->byteCount());
        PERFM_NVPROF("IR", frag->lirbuf->insCount());

        GprMethodProc code;
        bool keep = !assm->error();
        if (keep) {
            // save pointer to generated code
            code = (GprMethodProc) frag->code();
            PERFM_NVPROF("JIT method bytes", CodeAlloc::size(assm->codeList));
            if (jit_observer)
                jit_observer->notifyMethodJITed(info, assm->codeList, jit_debug_info);
        } else {
            verbose_only (if (pool->isVerbose(VB_execpolicy))
                AvmLog("execpolicy revert to interp (%d) compiler error %d \n", info->unique_method_id(), assm->error());
            )
            // assm puked, or we did something untested, so interpret.
            code = NULL;
            PERFM_NVPROF("lir-error",1);
        }

        #ifdef VMCFG_VTUNE
        if (assm->vtuneHandle) {
            vtuneCleanup(assm->vtuneHandle);
        }
        #endif /* VMCFG_VTUNE */
        return code;
    }

    BindingCache::BindingCache(const Multiname* name, BindingCache* next)
        : name(name), next(next)
    {}

    CallCache::CallCache(const Multiname* name, BindingCache* next)
        : BindingCache(name, next), call_handler(callprop_miss)
    {}

    GetCache::GetCache(const Multiname* name, BindingCache* next)
        : BindingCache(name, next), get_handler(getprop_miss)
    {}

    SetCache::SetCache(const Multiname* name, BindingCache* next)
        : BindingCache(name, next), set_handler(setprop_miss)
    {}

    template class CacheBuilder<CallCache>;
    template class CacheBuilder<GetCache>;
    template class CacheBuilder<SetCache>;
}

#endif // VMCFG_NANOJIT

