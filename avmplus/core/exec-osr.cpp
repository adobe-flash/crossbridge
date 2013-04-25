/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_NANOJIT
#include "CodegenLIR.h"
#include "FrameState.h"
#include "exec-osr.h"

/*

OSR Design notes

OSR adds a new compilation threshold parameter to the existing RM_mixed
runmode.  I chose not to add a new Runmode, just to keep the configuration
options simpler.  There is a ripple effect:  Runmode, AvmCore for default
values, ShellCoreSettings for commandline values, plus Flash settings
management.  A new osr_threshold setting is added; the default value is 0
which is the existing static heuristic.

Static heuristic means never interpret static initializers, and eagerly
compile all other functions on first invocation.  If the JIT fails, the
function reverts to the interpreter forever.

Generally, "invocation" means calls plus loop-backedges; we count both
with a single countdown counter.

New commandline arg -osr=count sets Config.osr_threshold to count.  Default
is count=0, which means use the static heuristic.

If osr_threshold is 1 or higher, we compile after that many interpreter
invocations.  The value 1 means we JIT compile on the second invocation,
because the first call triggers causes verification and the initial
interpreter invocation.

Limitations

We don't handle methods with exceptions because cloning and setting up
a new ExceptionFrame and jmp_buf seems too complicated.  Instead we JIT
these functions eagerly or not-at-all, according to the static heuristic.

The JIT can fail if a method's stack frame is too large.  We use a conservative
check for this case in OSR::isSupported(), and don't use OSR compilation if
the JIT could fail.  Instead we revert to the static heuristic for such
methods.

Future

The interaction between -Ojit, -Dinterp, and -osr is redundant.  If
-osr=1 meant jit-on-first-call, then -Ojit could be expressed in terms of
osr_threshold.  Similarly, -Dinterp could be emulated by
forcing OSR::isSupported() to return false for every function.

TODO
- (done) factor out patch to look up FrameState and CodegenLabel by byte* pc, not int pc
- (done) factor out patch to capture activation scopes separately
- (done) combine invocation count and loop count
- (done) commandline configurable thresholds
- (done) count down to zero instead of up to threshold
- (active) move the osr branch earlier in the prolog
- Performance testing
- handle methods with 2+ loop targets.  Currently we leak code if we OSR via
  one loop, then the other, which is possible if the method has >1 stack
  frame active in the interpreter.  A weird-but-simple test could repeatedly
  do this and cause OOM (TODO: write the test).
- ensure debugEnter/Exit pairing is correct
  https://bugzilla.mozilla.org/show_bug.cgi?id=539094#c10
- disable ScopeWriter for subsequent verify passes. (need a new bug)
- add default threshold for -osr with no given threshold.

From testing
- Why is osr repo slower than tr-tip with osr not in use?  See sunspider/math-cordic.as,math-partial-sums.as,math-spectral-norm.as,s3d-raytrace.as
- Not sure why osr repo seems faster with -osr not in use on regexp-dna.as?
- With -osr=50 jsbench/typed/FFT.as,LUFact.as,SOR.as are 5-15% slower than without -osr parameter set
- With -osr=50 jsbench/SparseMatmult.as is 20% faster than without -osr parameter set

*/

namespace avmplus
{
    int32_t OSR::parseConfig(const char *str)
    {
         // parse the commandline threshold
         int32_t threshold;
         if (VMPI_sscanf(str, "%d", &threshold) != 1 ||
             threshold < 0)
             return -1;
         return threshold;
    }

    OSR::OSR(const uint8_t *osr_pc, Atom* interp_frame)
        : osr_pc(osr_pc),
          interp_frame(interp_frame),
          jit_frame_state(NULL)
    {}

    void OSR::setFrameState(FrameState *fs)
    {
        jit_frame_state = fs;
    }

    const uint8_t* OSR::osrPc() const
    {
        return osr_pc;
    }

    // OSR is supported generally only in runmode RM_mixed.  We don't support
    // methods with try/catch blocks because of the complexity of establishing
    // a new ExceptionFrame and jmp_buf.  We also don't support methods for which
    // a previous compilation attempt failed, or for which failure can be predicted.
    //
    // We must only OSR methods that will execute with a BugCompatibility object
    // such that interpreter/compiler divergences are corrected.  Builtin methods
    // are invoked with bug compatibility inherited from the innermost non-builtin
    // function on the call chain, and thus may vary from call to call. Non-builtins
    // should always execute with bug compatibility taken from the AbcEnv to which
    // the method belongs, which will thus remain invariant. We can therefore only OSR
    // non-builtin methods.
    bool OSR::isSupported(const AbcEnv* abc_env, const MethodInfo* m, MethodSignaturep ms)
    {
        AvmAssert(abc_env->core() == m->pool()->core);
        AvmAssert(abc_env->pool() == m->pool());
        AvmAssert(abc_env->codeContext() != NULL);
        AvmAssert(abc_env->codeContext()->bugCompatibility() != NULL);

        return (m->osrEnabled() &&                    // OSR allowed by policy (global or ExecPolicy attribute)
                !m->hasExceptions() &&                // method does not have a try block
                !m->hasFailedJit() &&                 // no previous attempt to compile the method has failed
                !CodegenLIR::jitWillFail(ms) &&       // fast-fail predictor says JIT success is possible
                !m->pool()->isBuiltin &&              // the method is not a builtin (ABC baked into application)
                abc_env->codeContext()->bugCompatibility()->bugzilla539094);  // bug compatibility permits OSR
    }

    /**
     * Compile and finish executing a function.
     * We don't actually replace the interpreter frame.  Instead, this
     * routine JIT-compiles the current method and then calls the result
     * of that.  The JIT-ted version returns to here and the interpreter
     * then immediately exits, returning the result of the JIT-ted version,
     * which is returned here.
     */
    bool OSR::execute(MethodEnv *env, Atom* interp_frame,
                      MethodSignaturep ms, const uint8_t* osr_pc, Atom* result)
    {
        BaseExecMgr* exec = BaseExecMgr::exec(env);
        OSR osr(osr_pc, interp_frame);

#ifdef AVMPLUS_VERBOSE
        if (env->method->pool()->isVerbose(VB_execpolicy)) {
            env->core()->console << "execpolicy jit hot-loop " <<
                    env->method << " osr_pc=" <<
                    int(osr_pc - ms->abc_code_start()) <<
                    "\n";
        }
#endif
        // compile the method with an OSR entry point
        AvmAssert(!env->method->hasFailedJit());
        exec->verifyJit(env->method, ms, env->toplevel(), env->abcEnv(), &osr);
        env->_implGPR = env->method->_implGPR;

        if (env->method->hasFailedJit()) {
            // Clean up OSR object explicitly, as there is no destructor.
            mmfx_delete(osr.jit_frame_state);
            return false;
        }

        // Save current_osr.  It will be set to NULL in adjust_frame(),
        // once we have initialized the JIT frame from it.
        exec->current_osr = &osr;
        int fakeArgc = ms->requiredParamCount();
        *result = exec->endCoerce(env, fakeArgc, (uint32_t*)interp_frame, ms);
        return true;
    }

    /**
     * This function is called by JIT code if OSR has been requested
     * if (exec->current_osr != 0).  This function fills the JIT frame locals
     * to match the interpreter frame.  The JIT code looks like this:
     *
     *   if (*(&exec->current_osr)) {
     *     adjustFrame(...);
     *     goto loop_entry
     *   }
     */
    void OSR::adjustFrame(MethodFrame* jitMethodFrame, CallStackNode *callStack,
                             FramePtr jitFramePointer,  uint8_t *jitFrameTags)
    {
        MethodEnv* env = jitMethodFrame->env();
        BaseExecMgr* exec = BaseExecMgr::exec(env);
        OSR *osr = exec->current_osr;
        AvmAssert(osr && "should not have gotten here");
        Atom* interpFramePointer = osr->interp_frame;

        MethodSignaturep ms = env->method->getMethodSignature();
        int nLocals = ms->local_count();
        int stackBase = nLocals + ms->max_scope();
        FrameState* frameState = osr->jit_frame_state;
        int scopeTop = nLocals + frameState->scopeDepth;
        int stackTop = stackBase + frameState->stackDepth;

        // OSR has been requested.

#ifdef AVMPLUS_VERBOSE
        if (env->method->pool()->isVerbose(VB_interp)) {
            env->core()->console <<
                    "osr-adjust_frame " << env->method->method_id() <<
                    " " << env->method <<
                    " scopeTop=" << scopeTop <<
                    " stackTop=" << stackTop <<
                    "\n";
        }
#endif

        // Patch the JIT frame local variable slots and the scope slots in use to match the interpreter state.
        for (int i = 0; i < scopeTop; i++)
            unboxSlot(frameState, env, interpFramePointer, jitFramePointer, jitFrameTags, i);

        // zero out stack area for unused scopes:
        if (scopeTop < stackBase) {
            void* p = ((char*) jitFramePointer + (scopeTop << VARSHIFT(env->method)));
            size_t nbytes = (stackBase - scopeTop) << VARSHIFT(env->method);
            VMPI_memset(p, 0, nbytes);
        }

        // Patch operand stack slots:
        for (int i = stackBase; i < stackTop; i++)
            unboxSlot(frameState, env, interpFramePointer, jitFramePointer, jitFrameTags, i);

        MethodFrame *interpreterMethodFrame = jitMethodFrame->next;
        jitMethodFrame->dxns = interpreterMethodFrame->dxns;
        jitMethodFrame->next = interpreterMethodFrame->next;

        // Clean up non-local OSR parameterization data:
        mmfx_delete(frameState);
        exec->current_osr = NULL;

#ifdef DEBUGGER
        // Call debugEnter if necessary, since the jit code won't.
        // We can safely pass NULL for &eip here, because OSR is disabled
        // for methods with catch blocks.
        if (callStack)
            env->debugEnter(jitFrameTags, callStack, jitFramePointer, NULL);
#else
        (void) callStack;
#endif
    }

#ifdef DEBUG
    /**
     * Make sure the atom we have meets all the invariants we derived
     * at verify time.
     */
    static bool isValidAtom(const FrameValue& value, Atom atom)
    {
        Traits* type = value.traits;
        switch (Traits::getBuiltinType(type)) {
        case BUILTIN_void:
            return atom == undefinedAtom;
        case BUILTIN_null:
            return AvmCore::isNull(atom);
        case BUILTIN_int:
        case BUILTIN_uint:
        case BUILTIN_number:
        case BUILTIN_boolean:
            return AvmCore::istype(atom, type);
        case BUILTIN_any:
            return !value.notNull || !AvmCore::isNullOrUndefined(atom);
        default:
            return (!value.notNull && AvmCore::isNull(atom)) ||
                   AvmCore::istype(atom, type);
        }
    }
#endif

    void OSR::unboxSlot(FrameState* frameState,
            MethodEnv* env, Atom *interpFramePointer, FramePtr jitFramePointer,
            uint8_t* jitFrameTags, int index)
    {
        const FrameValue& value = frameState->value(index);
        Atom atom = interpFramePointer[index];
        AvmAssert(isValidAtom(value, atom));

        // Unbox the value and store in jit vars[].
        AvmAssert(isValidAtom(value, atom));
        Atom* addr = (Atom *) ((char *) jitFramePointer + (index << VARSHIFT(env->method)));
        Traits* type = value.traits;
        BaseExecMgr::unbox1(atom, type, addr);

        // Also update the jit's SlotStorageType byte in tags[].
        BuiltinType tag = Traits::getBuiltinType(type);
        SlotStorageType sst = valueStorageType(tag);
        jitFrameTags[index] = (uint8_t) sst;

#ifdef AVMPLUS_VERBOSE
        if (env->method->pool()->isVerbose(VB_interp)) {
            env->core()->console << "osr-unbox " << index << " type=" << type <<
                    " notnull=" << frameState->value(index).notNull <<
                    " value=" << asAtom(atom) <<
                    "\n";
        }
#endif
        (void)env;
    }

    // increment the invocation counter, return true once method is compiled
    REALLY_INLINE bool OSR::countInvoke(MethodEnv* env)
    {
        MethodInfo* m = env->method;
        if (--m->_abc.countdown)
            return false;
        if (m->isInterpreted()) {
#ifdef AVMPLUS_VERBOSE
            if (m->pool()->isVerbose(VB_execpolicy))
                env->core()->console <<
                    "execpolicy jit hot-call " << env->method << "\n";
#endif
            AvmAssert(!m->hasFailedJit());
            BaseExecMgr* exec = BaseExecMgr::exec(env);
            exec->verifyJit(m, m->getMethodSignature(),
                            env->toplevel(), env->abcEnv(), NULL);
            if (m->hasFailedJit())
                return false;
        }
        // Method was already compiled; we got here because env->_implGPR
        // was not updated.  Update it now.
        env->_implGPR = env->method->_implGPR;
        return true;
    }

    // intercept call to interpreter increment invocation counter
    uintptr_t OSR::osrInterpGPR(MethodEnv* env, int argc, uint32_t *ap)
    {
        if (countInvoke(env))
            return (*env->_implGPR)(env, argc, ap);
        return BaseExecMgr::interpGPR(env, argc, ap);
    }

    // intercept call to interpreter increment invocation counter
    double OSR::osrInterpFPR(MethodEnv* env, int argc, uint32_t *ap)
    {
        if (countInvoke(env))
            return (*env->_implFPR)(env, argc, ap);
        return BaseExecMgr::interpFPR(env, argc, ap);
    }

#ifdef VMCFG_FLOAT
    // intercept call to interpreter increment invocation counter
    float4_t OSR::osrInterpVECR(MethodEnv* env, int argc, uint32_t *ap)
    {
        if (countInvoke(env))
            return (*env->_implVECR)(env, argc, ap);
        return BaseExecMgr::interpVECR(env, argc, ap);
    }
#endif

    // intercept call to interpreter increment invocation counter
    Atom OSR::osrInvokeInterp(MethodEnv* env, int argc, Atom* argv)
    {
        if (countInvoke(env))
            return env->coerceEnter(argc, argv);
        return BaseExecMgr::invokeInterp(env, argc, argv);
    }

    // OSR tramp for invoking a constructor that returns a GPR
    uintptr_t OSR::osrInitInterpGPR(MethodEnv* env, int argc, uint32_t *ap)
    {
        if (countInvoke(env))
            return (*env->_implGPR)(env, argc, ap);
        return BaseExecMgr::initInterpGPR(env, argc, ap);
    }

    // OSR tramp for invoking a constructor that returns a FPR
    double OSR::osrInitInterpFPR(MethodEnv* env, int argc, uint32_t *ap)
    {
        if (countInvoke(env))
            return (*env->_implFPR)(env, argc, ap);
        return BaseExecMgr::initInterpFPR(env, argc, ap);
    }

#ifdef VMCFG_FLOAT
    // OSR tramp for invoking a constructor that returns a FPR
    float4_t OSR::osrInitInterpVECR(MethodEnv* env, int argc, uint32_t *ap)
    {
        if (countInvoke(env))
            return (*env->_implVECR)(env, argc, ap);
        return BaseExecMgr::initInterpVECR(env, argc, ap);
    }
#endif

    // intercept call to interpreter increment invocation counter
    Atom OSR::osrInitInvokeInterp(MethodEnv* env, int argc, Atom* argv)
    {
        if (countInvoke(env))
            return env->coerceEnter(argc, argv);
        return BaseExecMgr::initInvokeInterp(env, argc, argv);
    }

    #ifdef DEBUG
    // Verify that currentBugCompatibility() is as OSR expected when method is called.
    void OSR::checkBugCompatibility(MethodEnv* env)
    {
        // A method can be OSR'd if it is not a builtin.  We ignore builtins here.
        if (!env->method->pool()->isBuiltin) {
            const BugCompatibility* abcBugCompatibility = env->abcEnv()->codeContext()->bugCompatibility();
            const BugCompatibility* dynamicBugCompatibility = env->core()->currentBugCompatibility();
            // Verify that currentBugCompatibility() agrees with what the JIT would have assumed.
            AvmAssert(dynamicBugCompatibility == abcBugCompatibility);
        }
    }
    #endif
}
#endif // VMCFG_NANOJIT
