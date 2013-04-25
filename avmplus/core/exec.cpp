/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include "../vprof/vprof.h"
#include "Interpreter.h"

namespace avmplus {

// Computes the size in bytes of an argument of type t, when passed
// using the VM-wide AS3 calling convention.
int argSize(Traits* t)
{
    return Traits::getBuiltinType(t) == BUILTIN_number
            ? (int)sizeof(double)
#ifdef VMCFG_FLOAT
            : Traits::getBuiltinType(t) == BUILTIN_float4
            ? (int) sizeof(float4_t)
#endif
            : (int)sizeof(Atom);
}

BaseExecMgr* BaseExecMgr::exec(VTable* vtable)
{
    return (BaseExecMgr*) vtable->core()->exec;
}

BaseExecMgr* BaseExecMgr::exec(MethodEnv* env)
{
    return (BaseExecMgr*) env->core()->exec;
}

BaseExecMgr::BaseExecMgr(AvmCore* core)
    : core(core)
    , config(core->config)
#ifdef VMCFG_COMPILEPOLICY
    , _ruleSet(NULL)
#endif
#ifdef VMCFG_VERIFYALL
    , verifyFunctionQueue(core->gc, 0)
    , verifyTraitsQueue(core->gc, 0)
#endif
#ifdef VMCFG_NANOJIT
    , current_osr(NULL)
    , jit_observer(NULL)
#endif
{
#ifdef SUPERWORD_PROFILING
    WordcodeTranslator::swprofStart();
#endif
#ifdef VMCFG_COMPILEPOLICY
    prepPolicyRules();
#endif
#ifdef VMCFG_NANOJIT
    setupJit(core);
#endif
}

BaseExecMgr::~BaseExecMgr()
{
#ifdef SUPERWORD_PROFILING
    WordcodeTranslator::swprofStop();
#endif
#ifdef VMCFG_NANOJIT
    delete jit_observer;
    jit_observer = NULL;
#endif
}

// Called when MethodInfo is constructed.
void BaseExecMgr::init(MethodInfo* m, const NativeMethodInfo* native_info)
{
#ifndef MEMORY_INFO
//    MMGC_STATIC_ASSERT(offsetof(MethodInfo, _implGPR) == 0);
#endif

    if (native_info) {
        m->_apply_fastpath = 1;
        m->_native.thunker = native_info->thunker;
#if defined(VMCFG_TELEMETRY_SAMPLER) && !defined(VMCFG_AOT)
		m->_native.samplerThunker = native_info->samplerThunker;
#endif
#ifdef VMCFG_AOT
        if(!m->isAotCompiled()) {
            m->_native.handler = native_info->handler;
        }
#endif
    } else {
        #ifdef VMCFG_VERIFYALL
        if (config.verifyonly && m->isNative())
            m->_hasMethodBody = 0;
        #endif
    }

    // Initially, OSR will be allowed if we are in mixed run mode
    // (interpreter + JIT) and OSR has not been globally disabled.
    // Note that OSR may still be unspported for a given function
    // even if enabled here -- see OSR::isSupported().
    // This setting may be overridden by an explicit ExecPolicy
    // attribute on the method.  See AbcParser::parseTraits().
    if (!m->isNative())
        m->setOSR((config.runmode == RM_mixed && config.osr_enabled) ? config.osr_threshold : 0);

    m->_implGPR = NULL;
    m->_invoker = NULL;
    if (m->isResolved() && !config.verifyall) {
        // notifyMethodResolved() won't be called; this method is
        // a synthetic init method that returns void.  So install
        // verify trampolines now.
        m->_implGPR = verifyEnterGPR;
        m->_invoker = verifyInvoke;
    }
}

// Stub that is invoked when a methodenv is called the first time.
// Copy the invoker pointer from the underlying method, then call there.
uintptr_t BaseExecMgr::delegateInvoke(MethodEnv* env, int32_t argc, uint32_t *ap)
{
    env->_implGPR = env->method->_implGPR;
    return (*env->_implGPR)(env, argc, ap);
}

// Called when MethodInfo is constructed.
void BaseExecMgr::init(MethodEnv* env)
{
    env->_implGPR = delegateInvoke;
}

#ifdef VMCFG_FLOAT
typedef enum _eRetTypeKinds {
     kIntegerRetType = 0,         // return type for ints & co; also used for pointers, typically
     kFloatingPointRetType = 1,   // used by functions that return float or double
     kSIMDRetType = 2,            // used by functions that return float4
} eRetTypeKinds;

static REALLY_INLINE eRetTypeKinds ReturnType(MethodSignaturep ms){
    BuiltinType rt = ms->returnTraitsBT();
    if(rt==BUILTIN_number || rt==BUILTIN_float)
        return kFloatingPointRetType;
    if(rt==BUILTIN_float4)
        return kSIMDRetType;
    return kIntegerRetType;
}
#endif

// Called after MethodInfo is resolved.
void BaseExecMgr::notifyMethodResolved(MethodInfo* m, MethodSignaturep ms)
{
    if (!config.verifyall) {
        m->_invoker = verifyInvoke;
#ifdef VMCFG_FLOAT
        switch(ReturnType(ms)){
        case kIntegerRetType:
            m->_implGPR = verifyEnterGPR;
            break;
        case kFloatingPointRetType:
            m->_implFPR = verifyEnterFPR;
            break;
        case kSIMDRetType:
            m->_implVECR = verifyEnterVECR_adapter;
            break;
        }
#else
        if (ms->returnTraitsBT() == BUILTIN_number)
            m->_implFPR = verifyEnterFPR;
        else
            m->_implGPR = verifyEnterGPR;

#endif // VMCFG_FLOAT
    }
}

// True if at least one argument is typed and therefore must
// be coerced on invocation.
static bool hasTypedArgs(MethodSignaturep ms)
{
    int32_t param_count = ms->param_count();
    for (int32_t i = 1; i <= param_count; i++) {
        if (ms->paramTraits(i) != NULL) {
            // at least one parameter is typed; need full coerceEnter
            return true;
        }
    }
    return false;
}

// Initialize the declared slots of the given object by iterating
// through the ABC traits record and assigning to nonzero slots.
void BaseExecMgr::initObj(MethodEnv* env, ScriptObject* obj)
{
    struct InterpInitVisitor: public InitVisitor {
        ScriptObject* obj;
        InterpInitVisitor(ScriptObject* obj) : obj(obj) {}
        virtual ~InterpInitVisitor() {}
        void defaultVal(Atom val, uint32_t slot, Traits*) {
            // Assign the default value.
            // Keep in sync with interpreter INSTR(setslot).
            obj->coerceAndSetSlotAtom(slot, val);
        }
    };
    InterpInitVisitor visitor(obj);
    Traits* t = env->method->declaringTraits();
    const TraitsBindings *tb = t->getTraitsBindings();
    t->visitInitBody(&visitor, env->toplevel(), tb);
}

Atom BaseExecMgr::initInvokeInterp(MethodEnv* env, int argc, Atom* args)
{
    initObj(env, (ScriptObject*) atomPtr(args[0]));
    return invokeInterp(env, argc, args);
}

Atom BaseExecMgr::initInvokeInterpNoCoerce(MethodEnv* env, int argc, Atom* args)
{
    initObj(env, (ScriptObject*) atomPtr(args[0]));
    return invokeInterpNoCoerce(env, argc, args);
}

void BaseExecMgr::setInterp(MethodInfo* m, MethodSignaturep ms, bool isOsr)
{
    // Choose an appropriate set of interpreter invocation stubs.
    // * if OSR is enabled, choose a stub that counts invocations to trigger JIT.
    // * if the method is a constructor, choose a stub that initializes
    //   the object's fields before executing (init_).
    // * if the method has no typed args, choose an invoker stub that skips
    //   arg type checking entirely (_nocoerce).
    static const AtomMethodProc invoke_stubs[2][2][2] = {{{
        BaseExecMgr::invokeInterpNoCoerce,        // osr=0, ctor=0, typedargs=0
        BaseExecMgr::invokeInterp                 // osr=0, ctor=0, typedargs=1
    }, {
        BaseExecMgr::initInvokeInterpNoCoerce,    // osr=0, ctor=1, typedargs=0
        BaseExecMgr::initInvokeInterp             // osr=0, ctor=1, typedargs=1
    }}, {{
#ifdef VMCFG_NANOJIT
        OSR::osrInvokeInterp,                     // osr=1, ctor=0, typedargs=0
        OSR::osrInvokeInterp                      // osr=1, ctor=0, typedargs=1
    }, {
        OSR::osrInitInvokeInterp,                 // osr=1, ctor=1, typedargs=0
        OSR::osrInitInvokeInterp                  // osr=1, ctor=1, typedargs=1
#endif
    }}};
    int osr = isOsr ? 1 : 0;
    int ctor = m->isConstructor() ? 1 : 0;
    int typedargs = hasTypedArgs(ms) ? 1 : 0;
    m->_implGPR = NULL;
    m->_invoker = invoke_stubs[osr][ctor][typedargs];
    m->_isInterpImpl = 1;

    AvmAssert(!isOsr || isJitEnabled());

#ifdef VMCFG_NANOJIT
    if (isJitEnabled()) {
        // Choose an appropriate set of jit->interp stubs.
        // * if OSR is enabled, choose a stub that counts invocations to trigger JIT.
        // * if the method is a constructor, choose a stub that initializes
        //   the object's fields before executing (init_).
        // * if the return type is double, the stub must have a signature that
        //   returns double. (FPR)
        // * if the return type is float, the stub reuses the double (FPR)
        //   signature

        static const GprMethodProc impl_stubs[2][2][IFFLOAT(3,2)] = {{{
            BaseExecMgr::interpGPR                       // osr=0, ctor=0, fpr=0
            , (GprMethodProc)BaseExecMgr::interpFPR      // osr=0, ctor=0, fpr=1
#ifdef VMCFG_FLOAT
            , (GprMethodProc)BaseExecMgr::interpVECR     // osr=0, ctor=0, fpr=2
#endif
        }, {
            BaseExecMgr::initInterpGPR                   // osr=0, ctor=1, fpr=0
            , (GprMethodProc)BaseExecMgr::initInterpFPR  // osr=0, ctor=1, fpr=1
#ifdef VMCFG_FLOAT
            , (GprMethodProc)BaseExecMgr::initInterpVECR // osr=0, ctor=1, fpr=2
#endif
        }}, {{
            OSR::osrInterpGPR                            // osr=1, ctor=0, fpr=0
            , (GprMethodProc)OSR::osrInterpFPR           // osr=1, ctor=0, fpr=1
#ifdef VMCFG_FLOAT
            , (GprMethodProc)OSR::osrInterpVECR          // osr=1, ctor=0, fpr=2
#endif
        }, {
            OSR::osrInitInterpGPR                        // osr=1, ctor=1, fpr=0
            , (GprMethodProc)OSR::osrInitInterpFPR       // osr=1, ctor=1, fpr=1
#ifdef VMCFG_FLOAT
            , (GprMethodProc)OSR::osrInitInterpVECR      // osr=1, ctor=1, fpr=2
#endif
        }}};
        int rtype = IFFLOAT( ReturnType(ms),
                             ms->returnTraitsBT() == BUILTIN_number ? 1 : 0);
        m->_implGPR = impl_stubs[osr][ctor][rtype];

        // The countdown was previously set to config.osr_threshold, the
        // global default, and then possibly overridden by an explicit
        // ExecPolicy attribute.  If in fact OSR is not supported, zero
        // out the countdown here.  This accounts for cases in which OSR
        // is permitted by policy, but still cannot be supported.
        // Strictly speaking, this code is not required, but it may avoid
        // unnecessary invocations of OSR:isSupported() in OSR:countEdge().
        if (!isOsr)
            m->_abc.countdown = 0;
    }
#endif
}

uintptr_t BaseExecMgr::verifyEnterGPR(MethodEnv* env, int32_t argc, uint32_t* ap)
{
    verifyOnCall(env);
    STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
    uintptr_t ret = (*env->method->_implGPR)(env, argc, ap);
    STACKRESTORE();
    return ret;
}

double BaseExecMgr::verifyEnterFPR(MethodEnv* env, int32_t argc, uint32_t* ap)
{
    verifyOnCall(env);
    STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
    double d = (*env->method->_implFPR)(env, argc, ap);
    STACKRESTORE();
    return d;
}

#ifdef VMCFG_FLOAT
float4_t BaseExecMgr::verifyEnterVECR(MethodEnv* env, int32_t argc, uint32_t* ap)
{
    verifyOnCall(env);
    STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
    float4_t f4 = thunkEnterVECR_adapter((void*)env->method->_implVECR, env, argc, ap);
    STACKRESTORE();
    return f4;
}
#endif // VMCFG_FLOAT

// Entry point when the first call to the method is late bound.
Atom BaseExecMgr::verifyInvoke(MethodEnv* env, int argc, Atom* args)
{
    verifyOnCall(env);
    return (*env->method->_invoker)(env, argc, args);
}

void BaseExecMgr::verifyOnCall(MethodEnv* env)
{
    BaseExecMgr *exec = BaseExecMgr::exec(env);
    AvmAssert(!exec->config.verifyall);  // never verify late in verifyall mode

    #ifdef DEBUGGER
    // Install a fake CallStackNode here, so that if we throw a verify error,
    // we get a stack trace with the method being verified as its top entry.
    CallStackNode callStackNode(env->method);
    #endif

    exec->verifyMethod(env->method, env->toplevel(), env->abcEnv());

    // We got here by calling env->_implGPR, which was pointing to verifyEnterGPR/FPR,
    // but next time we want to call the real code, not verifyEnter again.
    // All other MethodEnv's in their default state will call the target method
    // directly and never go through verifyEnter().  Update the copy in MethodEnv.
    env->_implGPR = env->method->_implGPR;
}

// Verify the given method according to its type, with a CodeWriter
// pipeline appropriate to the current execution mode.
void BaseExecMgr::verifyMethod(MethodInfo* m, Toplevel *toplevel, AbcEnv* abc_env)
{
    AvmAssert(m->declaringTraits()->isResolved());
    m->resolveSignature(toplevel);
    PERFM_NTPROF_BEGIN("verify-ticks");
    MethodSignaturep ms = m->getMethodSignature();
    if (m->isNative())
        verifyNative(m, ms);
#ifdef VMCFG_NANOJIT
    else if (shouldJitFirst(abc_env, m, ms)) {
        verifyJit(m, ms, toplevel, abc_env, NULL);
    }
#endif
    else
        verifyInterp(m, ms, toplevel, abc_env);
    PERFM_NTPROF_END("verify-ticks");
}

/**
 * If we are in a pure-interpreter mode, this simply verifies code and then
 * installs a trampoline to run the interpreter.  If we are in a JIT mode,
 * then a previous decision has decided not to JIT-compile during verification,
 * and we call OSR::isSupported() to decide whether to simply install the
 * interpreter trampoline, or a countdown trampoline to trigger OSR.
 */
void BaseExecMgr::verifyInterp(MethodInfo* m, MethodSignaturep ms, Toplevel *toplevel, AbcEnv* abc_env)
{
#ifdef VMCFG_WORDCODE
    WordcodeEmitter coder(m, toplevel);
#else
    CodeWriter coder;
#endif
    verifyCommon(m, ms, toplevel, abc_env, &coder);

#ifdef VMCFG_NANOJIT
# ifdef AVMPLUS_VERBOSE
    if (m->pool()->isVerbose(VB_execpolicy))
        core->console << "execpolicy interp (" << m->unique_method_id() << ") " << m << " jit-available\n";
# endif
    setInterp(m, ms, OSR::isSupported(abc_env, m, ms));
#else
# ifdef AVMPLUS_VERBOSE
    if (m->pool()->isVerbose(VB_execpolicy))
        core->console << "execpolicy interp " << m << "\n";
# endif
    setInterp(m, ms, false);
#endif
}

// run the verifier, and if an exception is thrown,
// clean up the CodeWriter chain passed in by calling coder->cleanup().
// On normal return the CodeWriters declared here get cleaned via their
// destructors, and passed-in CodeWriters are still valid.
void BaseExecMgr::verifyCommon(MethodInfo* m, MethodSignaturep ms,
        Toplevel* toplevel, AbcEnv* abc_env, CodeWriter* const coder)
{
    CodeWriter* volatile vcoder = coder; // Volatile for setjmp safety.

#ifdef VMCFG_VERIFYALL
    VerifyallWriter verifyall(m, this, vcoder);
    if (config.verifyall)
        vcoder = &verifyall;
#endif

    Verifier verifier(m, ms, toplevel, abc_env); // Does not throw.
    TRY(core, kCatchAction_Rethrow) {
        verifier.verify(vcoder);  // Verify and fill vcoder pipeline.
    }
    CATCH (Exception *exception) {
        verifier.~Verifier();   // Clean up verifier.
        vcoder->cleanup();      // Cleans up all coders.
        core->throwException(exception);
    }
    END_CATCH
    END_TRY
}

#ifdef DEBUGGER

template<typename T, typename CALLT> T debugEnterExitWrapper(MethodEnv* env, GprMethodProc thunker, int32_t argc, uint32_t* argv){
    CallStackNode csn(CallStackNode::kEmpty);
    env->debugEnter(/*frame_sst*/NULL, &csn, /*framep*/NULL, /*eip*/NULL);
    CALLT thunk = (CALLT) thunker;
    const T result = thunk(env, argc, argv);
    env->debugExit(&csn);
    return result;
}
/*static*/
uintptr_t BaseExecMgr::debugEnterExitWrapper32(MethodEnv* env, int32_t argc, uint32_t* argv)
{
#if defined(VMCFG_TELEMETRY_SAMPLER) && !defined(VMCFG_AOT)
    return debugEnterExitWrapper<uintptr_t,GprMethodProc>(env,env->core()->samplerEnabled ? env->method->_native.samplerThunker : env->method->_native.thunker,argc,argv);
#else
    return debugEnterExitWrapper<uintptr_t,GprMethodProc>(env,env->method->_native.thunker,argc,argv);
#endif
}

/*static*/
double BaseExecMgr::debugEnterExitWrapperN(MethodEnv* env, int32_t argc, uint32_t* argv)
{
#if defined(VMCFG_TELEMETRY_SAMPLER) && !defined(VMCFG_AOT)
    return debugEnterExitWrapper<double,FprMethodProc>(env,env->core()->samplerEnabled ? env->method->_native.samplerThunker : env->method->_native.thunker, argc,argv);
#else
    return debugEnterExitWrapper<double,FprMethodProc>(env,env->method->_native.thunker, argc,argv);
#endif
}
#ifdef VMCFG_FLOAT
/*static*/
float4_t BaseExecMgr::debugEnterExitWrapperV(MethodEnv* env, int32_t argc, uint32_t* argv)
{
    CallStackNode csn(CallStackNode::kEmpty);
    env->debugEnter(/*frame_sst*/NULL, &csn, /*framep*/NULL, /*eip*/NULL);
#if defined(VMCFG_TELEMETRY_SAMPLER) && !defined(VMCFG_AOT)
    const float4_t result = thunkEnterVECR_adapter((void*) env->core()->samplerEnabled ? env->method->_native.samplerThunker : env->method->_native.thunker, env, argc, argv);
#else
    const float4_t result = thunkEnterVECR_adapter((void*) env->method->_native.thunker, env, argc, argv);
#endif
    env->debugExit(&csn);
    return result;
}
#endif // VMCFG_FLOAT
#endif

void BaseExecMgr::verifyNative(MethodInfo* m, MethodSignaturep ms)
{
#ifdef DEBUGGER
    if (core->debugger())
    {
#ifdef VMCFG_FLOAT
       switch( ReturnType(ms) ){
       case kIntegerRetType:
            setNative(m, debugEnterExitWrapper32);
            break;
       case kFloatingPointRetType:
            setNative(m, (GprMethodProc) debugEnterExitWrapperN);
            break;
       case kSIMDRetType:
            setNative(m, (GprMethodProc) debugEnterVECR_adapter);
            break;
       }
#else
        if (ms->returnTraitsBT() == BUILTIN_number)
            setNative(m, (GprMethodProc) debugEnterExitWrapperN);
        else
            setNative(m, debugEnterExitWrapper32);
#endif // VMCFG_FLOAT
    }
    else
#endif
    {
        (void)ms;
#if defined(VMCFG_TELEMETRY_SAMPLER) && !defined(VMCFG_AOT)
		setNative(m, core->samplerEnabled ? (GprMethodProc) m->_native.samplerThunker
				  : (GprMethodProc) m->_native.thunker);
#else
        setNative(m, (GprMethodProc) m->_native.thunker);
#endif
    }
}

#ifndef VMCFG_NANOJIT

// Install the generic, interpretive invoker for a native method.
// FIXME: Bug 529832 - We could specialize several common cases without JIT compiling.
void BaseExecMgr::setNative(MethodInfo* m, GprMethodProc p)
{
    m->_implGPR = p;
    m->_invoker = invokeGeneric;
}

// Without a JIT, we don't need to build any IMTs.
void BaseExecMgr::notifyVTableResolved(VTable*)
{}

bool BaseExecMgr::isJitEnabled() const
{
    return false;
}

#endif

// Only unbox the value (convert atom to native representation), coerce
// must have already happened.
Atom* FASTCALL BaseExecMgr::unbox1(Atom atom, Traits* t, Atom* arg0)
{
    // Atom must be correct type already, we're just unboxing it.
    AvmAssert(AvmCore::istype(atom, t) || AvmCore::isNullOrUndefined(atom));
    switch (Traits::getBuiltinType(t))
    {
        case BUILTIN_any:
        case BUILTIN_object:
        case BUILTIN_void:
            // My, that was easy.
            break;

        case BUILTIN_boolean:
            atom = (Atom) ((atom>>3) != 0);
            break;

        case BUILTIN_int:
            atom = AvmCore::integer_i(atom);
            break;

        case BUILTIN_uint:
            atom = AvmCore::integer_u(atom);
            break;

        case BUILTIN_number:
        {
            #ifdef AVMPLUS_64BIT
                AvmAssert(sizeof(Atom) == sizeof(double));
                union
                {
                    double d;
                    Atom a;
                };
                d = AvmCore::number_d(atom);
                atom = a;
            #else
                AvmAssert(sizeof(Atom)*2 == sizeof(double));
                union
                {
                    double d;
                    Atom a[2];
                };
                d = AvmCore::number_d(atom);
                arg0[0] = a[0];
                arg0 += 1;
                atom = a[1];    // Fall through, will be handled at end.
            #endif
            break;
        }
#ifdef VMCFG_FLOAT
        case BUILTIN_float:
        {
            union
            {
                float f;
                Atom a;
            };
            f = AvmCore::atomToFloat(atom);
            atom = a;
            break;
        }
        case BUILTIN_float4:
        {
            union{
                float4_t f4;
                Atom a[4];
            };
            const int nAtoms = sizeof(float4_t)/sizeof(Atom);
            AvmAssert(sizeof(float4_t)%sizeof(Atom)==0);
            f4 = AvmCore::atomToFloat4(atom);
            int i;
            for(i=0;i<nAtoms-1;i++)
                *arg0++ = a[i];
            atom = a[i]; // this will be handled below
            break;
        }
#endif

        default:
            atom = (Atom)atomPtr(atom);
            break;
    }
    // Every case increments by at least 1.
    arg0[0] = atom;
    return arg0+1;
}

// Coerce and unbox one argument.
// Note that some of these have (partial) guts of avmplus::coerce replicated here, for efficiency.
// If you find bugs here, you might need to update avmplus::coerce as well (and vice versa).
Atom* FASTCALL BaseExecMgr::coerceUnbox1(MethodEnv* env, Atom atom, Traits* t, Atom* args)
{
    // Using computed-gotos here doesn't move the needle appreciably in testing.
    switch (Traits::getBuiltinType(t))
    {
        case BUILTIN_any:
            // My, that was easy.
            break;

        case BUILTIN_boolean:
            atom = AvmCore::boolean(atom);
            break;

        case BUILTIN_int:
            atom = AvmCore::integer(atom);
            break;

        case BUILTIN_uint:
            atom = AvmCore::toUInt32(atom);
            break;

        case BUILTIN_namespace:
            // Coerce undefined -> Namespace should yield null.
            if (AvmCore::isNullOrUndefined(atom))
            {
                atom = 0;
                break;
            }
            if (atomKind(atom) != kNamespaceType)
                goto failure;
            atom = (Atom)atomPtr(atom);
            break;

#ifdef VMCFG_FLOAT
        case BUILTIN_float:
        {
            union
            {
                float f;
                Atom a;
            };
            f = (float) AvmCore::number(atom);
            atom = a;
            break;
        }
        case BUILTIN_float4:
        {
            const int nAtoms = sizeof(float4_t)/sizeof(Atom);
            union{
                float4_t f4;
                Atom a[nAtoms];
            };
            AvmAssert(sizeof(float4_t)%sizeof(Atom)==0);
            AvmCore::float4(&f4, atom);
            int i;
            for(i=0;i<nAtoms-1;i++)
                *args++ = a[i];
            atom = a[i]; // this will be handled below
            break;
        }
#endif // VMCFG_FLOAT
        case BUILTIN_number:
        {
            #ifdef AVMPLUS_64BIT
                AvmAssert(sizeof(Atom) == sizeof(double));
                union
                {
                    double d;
                    Atom a;
                };
                d = AvmCore::number(atom);
                atom = a;
            #else
                AvmAssert(sizeof(Atom)*2 == sizeof(double));
                union
                {
                    double d;
                    Atom a[2];
                };
                d = AvmCore::number(atom);
                args[0] = a[0];
                args += 1;
                atom = a[1];    // Fall thru, will be handled at end.
            #endif
            break;
        }
        case BUILTIN_object:
            if (atom == undefinedAtom)
                atom = nullObjectAtom;
            break;

        case BUILTIN_string:
            atom = AvmCore::isNullOrUndefined(atom) ? NULL : (Atom)env->core()->string(atom);
            break;

        case BUILTIN_null:
        case BUILTIN_void:
            AvmAssert(!"illegal, should not happen");
            atom = 0;
            break;

        case BUILTIN_math:
        case BUILTIN_methodClosure:
        case BUILTIN_qName:
        case BUILTIN_vector:
        case BUILTIN_vectordouble:
        case BUILTIN_vectorint:
        case BUILTIN_vectoruint:
#ifdef VMCFG_FLOAT
        case BUILTIN_vectorfloat:
        case BUILTIN_vectorfloat4:
#endif
        case BUILTIN_xml:
        case BUILTIN_xmlList:
            // A few intrinsic final classes can skip subtypeof calls.
            if (AvmCore::isNullOrUndefined(atom))
            {
                atom = 0;
                break;
            }
            else if (atomKind(atom) == kObjectType)
            {
                Traits* actual = AvmCore::atomToScriptObject(atom)->traits();
                AvmAssert(actual->final);
                if (actual == t)
                {
                    atom = (Atom)atomPtr(atom);
                    break;
                }
            }
            // Didn't break? that's a failure.
            goto failure;

        case BUILTIN_date:
        case BUILTIN_array:
        case BUILTIN_class:
        case BUILTIN_error:
        case BUILTIN_function:
        case BUILTIN_none:
        case BUILTIN_regexp:
        case BUILTIN_vectorobj: // Unlike other vector types, vectorobj is NOT final.
            if (AvmCore::isNullOrUndefined(atom))
            {
                atom = 0;
                break;
            }
            else if (atomKind(atom) == kObjectType)
            {
                Traits* actual = AvmCore::atomToScriptObject(atom)->traits();
                if (actual->subtypeof(t))
                {
                    atom = (Atom)atomPtr(atom);
                    break;
                }
            }
            // Didn't break? that's a failure.
            goto failure;
    }
    // Every case increments by at least 1.
    args[0] = atom;
    return args+1;

failure:
    AvmCore* core = env->core();
    env->toplevel()->throwTypeError(kCheckTypeFailedError, core->atomToErrorString(atom), core->toErrorString(t));
    return unreachableAtom;
}

// Coerce an argument to an expected type, but keep it represented as Atom.
// Note that this function is (currently) only used for interpreted functions.
inline Atom coerceAtom(AvmCore* core, Atom atom, Traits* t, Toplevel* toplevel)
{
    switch (Traits::getBuiltinType(t))
    {
    case BUILTIN_number:
        return (atomKind(atom) == kDoubleType) ? atom : core->numberAtom(atom);
    case BUILTIN_int:
        return (atomKind(atom) == kIntptrType) ? atom : core->intAtom(atom);
    case BUILTIN_uint:
        return (atomKind(atom) == kIntptrType && atom >= 0) ? atom : core->uintAtom(atom);
    case BUILTIN_boolean:
        return (atomKind(atom) == kBooleanType) ? atom : AvmCore::booleanAtom(atom);
    case BUILTIN_object:
        return (atom == undefinedAtom) ? nullObjectAtom : atom;
    case BUILTIN_any:
        return atom;
#ifdef VMCFG_FLOAT
    case BUILTIN_float:
        return AvmCore::isFloat(atom) ? atom : core->floatAtom(atom);
    case BUILTIN_float4:
        return AvmCore::isFloat4(atom) ? atom : core->float4Atom(atom);
#endif // VMCFG_FLOAT
    default:
        return toplevel->coerce(atom, t);
    }
}

Atom BaseExecMgr::endCoerce(MethodEnv* env, int32_t argc, uint32_t *ap, MethodSignaturep ms)
{
    // We know we have verified the method, so we can go right into it.
    AvmCore* core = env->core();
    const int32_t bt = ms->returnTraitsBT();
    
    switch(bt){
    case BUILTIN_number:
    {
        STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
        double d = (*env->method->_implFPR)(env, argc, ap);
        STACKRESTORE();
        return core->doubleToAtom(d);
    }
#ifdef VMCFG_FLOAT
    case BUILTIN_float:
    {
        STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
        // WARNING: This assumes that the calling conventions of SinglePrecisionFprMethodProc and FprMethodProc
        // are identical, or at least compatible. I.e. the register used to return the "double" value is identical with
        // (or a superset of) the register used to return a float value. This assumption is true for ARM (softfloat, hardfloat ABIs)
        // x86 and x64 (Mac, Windows, Linux) at least. Must double-check for other platforms!! (most notably, TODO: MIPS)
        typedef float (*SinglePrecisionFprMethodProc)(MethodEnv*, int32_t, uint32_t *);
        float f = reinterpret_cast<SinglePrecisionFprMethodProc> (*env->method->_implFPR)(env, argc, ap);
        STACKRESTORE();
        return core->floatToAtom(f);
    }
    case BUILTIN_float4:
        {
            STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
            float4_t f4 = thunkEnterVECR_adapter((void*)env->method->_implVECR, env, argc, ap);
            STACKRESTORE();
            return core->float4ToAtom(f4);
        }
#endif // VMCFG_FLOAT
    default:
    {
        STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
        const Atom i = (*env->method->_implGPR)(env, argc, ap);
        STACKRESTORE();

        switch (bt)
        {
        case BUILTIN_int:
            return core->intToAtom((int32_t)i);
        case BUILTIN_uint:
            return core->uintToAtom((uint32_t)i);
        case BUILTIN_boolean:
            return i ? trueAtom : falseAtom;
        case BUILTIN_any:
        case BUILTIN_object:
        case BUILTIN_void:
            return (Atom)i;
        case BUILTIN_string:
            return ((Stringp)i)->atom();
        case BUILTIN_namespace:
            return ((Namespace*)i)->atom();
        default:
            return ((ScriptObject*)i)->atom();
        }
    }
    }
}

inline void checkArgc(MethodEnv *env, int32_t argc, MethodSignaturep ms)
{
    // Getting toplevel() is slightly more expensive than it used to be (more indirection)...
    // so only extract in the (rare) event of an exception.

    if (!ms->argcOk(argc))
        env->argcError(argc);

    // Should no longer be possible to have mismatched scopes; we should reject any such
    // functions in makeIntoPrototypeFunction().
    AvmAssert(env->method->declaringScope()->equals(env->scope()->scopeTraits()));
}

REALLY_INLINE size_t calcAtomAllocSize(int32_t argc, int32_t extra = 0)
{
    // We need to check for integer overflow here; it's possible for someone to use Function.apply()
    // to pass an argument Array with a huge value for length (but sparsely populated).
    // But since "extra" is int32 and sizeof(Atom) is known here, we can do an efficient check
    // without a full 64-bit multiply, as CheckForCallocSizeOverflow() does. (Note: yes, I know
    // that a good compiler should optimize unsigned-divide-by-power-of-2 into a shift, but this is
    // a hot path, so let's be skeptical and force the issue.)
#ifdef VMCFG_64BIT
    uint32_t const kAtomSizeLog2 = 3;
#else
    uint32_t const kAtomSizeLog2 = 2;
#endif
    MMGC_STATIC_ASSERT(sizeof(Atom) == (1<<kAtomSizeLog2));
    if (uint32_t(argc) > ((MMgc::GCHeap::kMaxObjectSize - size_t(extra)) >> kAtomSizeLog2))
        MMgc::GCHeap::SignalObjectTooLarge();
    return (argc << kAtomSizeLog2) + extra;
}

// static
inline size_t BaseExecMgr::startCoerce(MethodEnv *env, int32_t argc, MethodSignaturep ms)
{
    checkArgc(env, argc, ms);

    // Compute the number of rest arguments present.
    const int32_t param_count = ms->param_count();
    const int32_t extra = argc > param_count ? argc - param_count : 0;
    AvmAssert(ms->rest_offset() > 0 && extra >= 0);
    const int32_t rest_offset = ms->rest_offset();
    return calcAtomAllocSize(extra, rest_offset);
}

// Specialized to be called from Function.apply().
Atom BaseExecMgr::apply(MethodEnv* env, Atom thisArg, ArrayObject *a)
{
    int32_t argc = a->getLength();
    if (argc == 0)
        return env->coerceEnter(thisArg);

    if (env->method->_apply_fastpath) {
        // JIT or native method.  We specialize this path to avoid
        // calling alloca twice; once to unpack atoms from a[], then
        // again to unpack from Atom[] to native values.
        MethodSignaturep ms = env->get_ms();
        const size_t extra_sz = startCoerce(env, argc, ms);
        MMgc::GC::AllocaAutoPtr _ap;
        uint32_t *ap = (uint32_t *)avmStackAlloc(core, _ap, extra_sz);
        unboxCoerceArgs(env, thisArg, a, ap, ms);
        return endCoerce(env, argc, ap, ms);
    }

    // Caller will coerce instance if necessary, so make sure it was done.
    AvmAssertMsgCanThrow(thisArg == coerce(env, thisArg, env->get_ms()->paramTraits(0)),"",env->core());

    // Tail call inhibited by local allocation/deallocation.
    MMgc::GC::AllocaAutoPtr _atomv;
    Atom* atomv = (Atom*)avmStackAllocArray(core, _atomv, (argc+1), sizeof(Atom));
    atomv[0] = thisArg;
    for (int32_t i=0 ; i < argc ; i++ )
        atomv[i+1] = a->getUintProperty(i);
    return env->coerceEnter(argc, atomv);
}

// Specialized to be called from Function.call().
Atom BaseExecMgr::call(MethodEnv* env, Atom thisArg, int argc, Atom *argv)
{
    if (argc == 0)
        return env->coerceEnter(thisArg);

    if (env->method->_apply_fastpath) {
        // JIT or native method.  We specialize this path to avoid
        // calling alloca twice; once to unpack atoms from a[], then
        // again to unpack from Atom[] to native values.
        MethodSignaturep ms = env->get_ms();
        const size_t extra_sz = startCoerce(env, argc, ms);
        MMgc::GC::AllocaAutoPtr _ap;
        uint32_t *ap = (uint32_t *)avmStackAlloc(core, _ap, extra_sz);
        unboxCoerceArgs(env, thisArg, argc, argv, ap, ms);
        return endCoerce(env, argc, ap, ms);
    }

    // Caller will coerce instance if necessary, so make sure it was done.
    AvmAssertMsgCanThrow(thisArg == coerce(env, thisArg, env->get_ms()->paramTraits(0)),"",env->core());

    // Tail call inhibited by local allocation/deallocation.
    MMgc::GC::AllocaAutoPtr _atomv;
    Atom* atomv = (Atom*)avmStackAllocArray(core, _atomv, (argc+1), sizeof(Atom));
    atomv[0] = thisArg;
    VMPI_memcpy(atomv+1, argv, sizeof(Atom)*argc);
    return env->coerceEnter(argc, atomv);
}

// Optimization opportunities: since we call interpBoxed() directly, it is
// probably possible to allocate its stack frame here and pass it in.
// If we do so then interpBoxed() should deallocate it.  This affords us
// the optimization of getting rid of alloca() allocation here,
// which means improved tail calls for one.  For another, if the argv
// pointer points into the stack segment s.t. argv+argc+1 equals the
// current stack pointer then the stack may be extended in place
// provided there's space.  But that optimization may equally well
// be performed inside interpBoxed(), and in fact if we alloc temp
// space on the alloca stack here then interpBoxed() would always perform
// that optimization.  So we'd just be moving the decision into interpBoxed().

// note that GCC typically restricts tailcalls to functions with similar signatures
// ("sibcalls") -- see http://www.ddj.com/architect/184401756 for a useful explanation.
// anyway, since we really want interpBoxed to be a tailcall from
// here, be sure to keep it using a compatible signature...
Atom BaseExecMgr::invokeInterp(MethodEnv* env, int32_t argc, Atom* atomv)
{
    // The tail call to interpBoxed is important in order to keep stack consumption down in an
    // interpreter-only configuration, but it's good always.

    AvmAssert(isInterpreted(env));
    AvmCore* core = env->core();
    Toplevel* toplevel = env->toplevel();

    MethodSignaturep ms = env->get_ms();
    checkArgc(env, argc, ms);

    // Caller will coerce instance if necessary, so make sure it was done.
    AvmAssertMsgCanThrow(atomv[0] == coerce(env, atomv[0], ms->paramTraits(0)),"",env->core());

    const int32_t param_count = ms->param_count();
    const int32_t end = argc >= param_count ? param_count : argc;
    for (int32_t i=1 ; i <= end ; i++ )
        atomv[i] = coerceAtom(core, atomv[i], ms->paramTraits(i), toplevel);
    return interpBoxed(env, argc, atomv);
}

// Specialized copy of invoke_interp() when there are no typed args.
Atom BaseExecMgr::invokeInterpNoCoerce(MethodEnv* env, int32_t argc, Atom* atomv)
{
    // The tail call to interpBoxed is important in order to keep stack consumption down in an
    // interpreter-only configuration, but it's good always.

    MethodSignaturep ms = env->get_ms();
    checkArgc(env, argc, ms);

#ifdef DEBUG
    AvmAssert(isInterpreted(env));
    // caller will coerce instance if necessary, so make sure it was done.
    AvmAssertMsgCanThrow(atomv[0] == coerce(env, atomv[0], ms->paramTraits(0)),"",env->core());

    const int32_t param_count = ms->param_count();
    const int32_t end = argc >= param_count ? param_count : argc;
    for (int32_t i=1 ; i <= end ; i++ )
        AvmAssert(ms->paramTraits(i) == NULL);
#endif

    return interpBoxed(env, argc, atomv);
}

// Invoker for native or jit code used before we have jit-compiled,
// or after JIT compilation of the invoker has failed.
Atom BaseExecMgr::invokeGeneric(MethodEnv *env, int32_t argc, Atom* atomv)
{
    MethodSignaturep ms = env->get_ms();
    const size_t extra_sz = startCoerce(env, argc, ms);
    MMgc::GC::AllocaAutoPtr _ap;
    uint32_t *ap = (uint32_t *)avmStackAlloc(env->core(), _ap, extra_sz);
    unboxCoerceArgs(env, argc, atomv, ap, ms);
    return endCoerce(env, argc, ap, ms);
}

// Convert atoms to native args.  argc is the number of
// args, not counting the instance which is arg[0].  the
// layout is [instance][arg1..argN]
void BaseExecMgr::unboxCoerceArgs(MethodEnv* env, int32_t argc, Atom* in, uint32_t *argv, MethodSignaturep ms)
{
    Atom* args = (Atom*)argv;

    const int32_t param_count = ms->param_count();
    int32_t end = argc >= param_count ? param_count : argc;
    args = unbox1(in[0], ms->paramTraits(0), args); // no need to coerce
    for (int32_t i=1; i <= end; i++)
        args = coerceUnbox1(env, in[i], ms->paramTraits(i), args);
    while (end < argc)
        *args++ = in[++end];
}

// Specialized for Function.apply().
void BaseExecMgr::unboxCoerceArgs(MethodEnv* env, Atom thisArg, ArrayObject *a, uint32_t *argv, MethodSignaturep ms)
{
    int32_t argc = a->getLength();

    Atom *args = unbox1(thisArg, ms->paramTraits(0), (Atom *) argv);

    const int32_t param_count = ms->param_count();
    int32_t end = argc >= param_count ? param_count : argc;
    for (int32_t i=0; i < end; i++)
        args = coerceUnbox1(env, a->getUintProperty(i), ms->paramTraits(i+1), args);
    while (end < argc)
        *args++ = a->getUintProperty(end++);
}

// Specialized for Function.call().
void BaseExecMgr::unboxCoerceArgs(MethodEnv* env, Atom thisArg, int32_t argc, Atom* in, uint32_t *argv, MethodSignaturep ms)
{
    Atom *args = unbox1(thisArg, ms->paramTraits(0), (Atom *) argv);

    const int32_t param_count = ms->param_count();
    int32_t end = argc >= param_count ? param_count : argc;
    for (int32_t i=0; i < end; i++)
        args = coerceUnbox1(env, in[i], ms->paramTraits(i+1), args);
    while (end < argc)
        *args++ = in[end++];
}

} // namespace avmplus
