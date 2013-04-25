/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_HALFMOON

#include "CodegenLIR.h"
#include "Deopt.h"
#include "Interpreter.h"

namespace framelib {
    // Implementations of these functions are platform specific.
    void returnFromFrame(uint8_t* frame);  // void result
    void returnFromFrameWithInt32(uint8_t* frame, int32_t value);
    void returnFromFrameWithDouble(uint8_t* frame, double value);
    void returnFromFrameWithInt64(uint8_t* frame, int64_t value);
    uint8_t* getReturnAddress(uint8_t* frame);
    void setReturnAddress(uint8_t* frame, uint8_t* address);
    uint8_t* getPointerArgument0(uint8_t* frame);
    void resumeFrameAtLocation(uint8_t* state, uint8_t* address);
    void saveActivationState(uint8_t* state);
    // FIXME
    // FrameLib is intended to be generic, and to depend only on nanojit.
    // It was formerly located in the nanojit tree, which is where it belongs.
    // These functions have knowledge of the deoptimization scheme, and don't
    // belong here.  Also, moving them here messed up the resolution of the
    // calls to the the stubs in the trampolines (asm code), which need to
    // be in the same namespace.
    //uint8_t* getTrampForInt32();
    //uint8_t* getTrampForInt64();
    //uint8_t* getTrampForDouble();
#ifdef AVMPLUS_64BIT
    REALLY_INLINE void returnFromFrameWithPtr(uint8_t* frame, uintptr_t value) {
        framelib::returnFromFrameWithInt64(frame, value);
    }
#else
    REALLY_INLINE void returnFromFrameWithPtr(uint8_t* frame, uintptr_t value) {
        framelib::returnFromFrameWithInt32(frame, value);
    }
#endif
}

namespace avmplus
{

#ifdef DEOPT_TRACE
#ifndef AVMPLUS_VERBOSE
#error "DEOPT_TRACE requires AVMPLUS_VERBOSE"
#endif
static bool deopt_trace = true;
static bool trace_verbose = false;
#endif

void Deoptimizer::returnFromFrameWithUnboxedValue(uint8_t* frame, Atom value,
                                                  Traits* t)
{
    // Atom must be correct type already, we're just unboxing it.
    AvmAssert(AvmCore::istype(value, t) || AvmCore::isNullOrUndefined(value));
    switch (Traits::getBuiltinType(t)) {
    case BUILTIN_boolean:
        framelib::returnFromFrameWithInt32(frame, ((value >> 3) != 0));
        break;
    case BUILTIN_int:
        framelib::returnFromFrameWithInt32(frame, AvmCore::integer_i(value));
        break;
    case BUILTIN_uint:
        framelib::returnFromFrameWithInt32(frame, AvmCore::integer_u(value));
        break;
    case BUILTIN_number:
        framelib::returnFromFrameWithDouble(frame, AvmCore::number_d(value));
        break;
    default:
        // String, Namespace, ScriptObject
        value = (Atom) atomPtr(value);
        /* FALLTHROUGH */
    case BUILTIN_void:
    case BUILTIN_any:
    case BUILTIN_object:
        framelib::returnFromFrameWithPtr(frame, value);
        break;
    }
}

#ifdef linux
// GCC on Linux apparently do not mangle C-linkage names.
// Add a leading underscore here so references from assembler code will resolve.
#define deoptFrameUponReturnWithInt32   _deoptFrameUponReturnWithInt32
#define deoptFrameUponReturnWithInt64   _deoptFrameUponReturnWithInt64
#define deoptFrameUponReturnWithDouble  _deoptFrameUponReturnWithDouble
#endif

// Static bridges from the assembly-language trampolines.
// The asm trampoline has captured the victim's fp.
// We use that to retrieve its first argument, the MethodEnv.
// From the MethodEnv, we can obtain the MethodInfo and the Deoptimizer.

extern "C"
void deoptFrameUponReturnWithInt32(uint8_t* fp, int32_t result)
{
    MethodEnv* env = (MethodEnv*)framelib::getPointerArgument0(fp);
    env->method->_armed_deoptimizers->deoptFrameUponReturnWithResult(fp, env, (void*)&result);
}

extern "C"
void deoptFrameUponReturnWithInt64(uint8_t* fp, int64_t result)
{
    MethodEnv* env = (MethodEnv*)framelib::getPointerArgument0(fp);
    env->method->_armed_deoptimizers->deoptFrameUponReturnWithResult(fp, env, (void*)&result);
}

extern "C"
void deoptFrameUponReturnWithDouble(uint8_t* fp, double result)
{
    MethodEnv* env = (MethodEnv*)framelib::getPointerArgument0(fp);
    env->method->_armed_deoptimizers->deoptFrameUponReturnWithResult(fp, env, (void*)&result);
}

#ifdef VMCFG_IA32

// TODO: These would be better off in a separate .asm file, but I don't think we
// have one for the i386 builds, and I'm not ready to get into that yet.

// Return a trampoline to capture the frame pointer and return value from a returning
// function and transfer into the appropriate static bridge above.  We overwrite the
// return address of the frame we wish to "hook" in order to take control when it returns.
// We maintain 16-byte stack alignment, which is required on some platforms.

// GCC will attempt to inline these functions, but emit an out-of-line version
// as well.  This results in duplicate symbol definitions due to the inline
// assembler code.  We could avoid this by using numeric local labels, but there
// is no need to waste the code space, and it's good to have a symbol for the
// trampoline.

NO_INLINE
uint8_t* getTrampForInt32()
{
    //using namespace framelib;
#if _MSC_VER
    _asm {
        jmp     done
      deopt_tramp_I32:
        sub     esp, 8
        push    eax
        push    ebp
        call    deoptFrameUponReturnWithInt32
        /* NOTREACHED */
        int     3
      done:
        lea     eax, deopt_tramp_I32
    }
#else
    uint8_t* result;
    asm volatile (
        "    jmp   0f                                 ;\
             .align 4                                 ;\
             .globl deopt_tramp_I32                   ;\
         deopt_tramp_I32:                             ;\
             subl  $8, %%esp                          ;\
             pushl %%eax                              ;\
             pushl %%ebp                              ;\
             call  _deoptFrameUponReturnWithInt32     ;\
             int3                                     ;\
         0:                                           ;\
             movl $deopt_tramp_I32, %[result]         "
         : : [result] "m"(result)
    );
    return result;
#endif
}

NO_INLINE
uint8_t* getTrampForInt64()
{
    return NULL;  //nyi
}

NO_INLINE
uint8_t* getTrampForDouble()
{
    //using namespace framelib;
#if _MSC_VER
    _asm {
        jmp     done
      deopt_tramp_double:
        sub     esp, 12
        fstp    qword ptr [esp]
        push    ebp
        call    deoptFrameUponReturnWithDouble
        /* NOTREACHED */
        int     3
      done:
        lea     eax, deopt_tramp_double
    }
#else
    uint8_t* result;
    asm volatile (
        "    jmp   0f                                 ;\
             .align 4                                 ;\
             .globl deopt_tramp_double                ;\
         deopt_tramp_double:                          ;\
             subl  $12, %%esp                         ;\
             fstpl (%%esp)                            ;\
             pushl %%ebp                              ;\
             call  _deoptFrameUponReturnWithDouble    ;\
             int3                                     ;\
         0:                                           ;\
             movl $deopt_tramp_double, %[result]      "
         : : [result] "m"(result)
    );
    return result;
#endif
}

#else

uint8_t* getTrampForInt32() {
    AvmAssert(false && "not implemented");
    return 0;
}

uint8_t* getTrampForInt64() {
   AvmAssert(false && "not implemented");
   return 0;
}

uint8_t* getTrampForDouble() {
    AvmAssert(false && "not implemented");
    return 0;
}

#endif // ifdef VMCFG_IA32

// Deoptimize the method associated with this Deoptimizer.
// We arrange for activations of the method to be deoptimized as the stack unwinds.
// Here, we do not actually revert the state of the method to interpreted, but only
// arrange for the deoptimization of all pending activations.
// TODO:  When unwinding for exceptions, we may skip over an armed frame, and should
// then re-arm a frame that is still active.  Otherwise, the deoptimize-and-rearm chain
// will be broken, and we will not deoptimize all activations.

void Deoptimizer::deoptimize(AvmCore* core)
{
    MethodFrame* frame = nextActivation(methodFrame(core));
    if (frame) {
        Deoptimizer* armed = methodInfo->_armed_deoptimizers;
        if (!armed || armed->armedFrame != frame) {
            // There is an activation not handled by a pending deoptimizer.
            armForLazyDeoptimization(frame);
        }
    }
    methodInfo->_deoptimizer = NULL;
}

void Deoptimizer::armForLazyDeoptimization(DeoptContext* ctx, MethodFrame* victim)
{
    // Requires fixed ESP!  We expect to locate the callee's native frame by
    // adding nj_frame_size_ to to the caller's native frame pointer.

    // It is illegal for a deoptimizer to have more than one armed frame.
    AvmAssert(resumptionPc == 0);

    // Calculate base of native frame.
    uint8_t* frameBase = ctx->frameBase(victim);

#ifdef DEOPT_TRACE
    if (deopt_trace) {
        AvmCore* core = victim->env()->core();
        core->console << "$$$ deopt armed " << hexAddr((uintptr_t)this) << " @ " << hexAddr((uintptr_t)victim) << "\n";
    }
#endif

    uint8_t* calleeFrameBase = ctx->calleeFrameBase(frameBase);

    // We assume that the victim frame is awaiting return of a
    // subsidiary call, and will be resumed at the return address
    // saved in the frame of its immediate callee.
    uint8_t* nativePc = framelib::getReturnAddress(calleeFrameBase);

    // Save the return address, because we are going to clobber it.
    resumptionPc = nativePc;

    // Find safepoint associated with the call.
    ctx->setSafepointFromNativePc(nativePc);

    // Get trampoline appropriate for the callee's return type.
    uint8_t* trampoline;
    switch (ctx->safepointReturnValueSST()) {
    case SST_int32:
    case SST_uint32:
    case SST_bool32:
        trampoline = getTrampForInt32();
        break;
    case SST_double:
        trampoline = getTrampForDouble();
        break;
    default:
      #ifdef AVMPLUS_64BIT
        trampoline = getTrampForInt64();
      #else
        trampoline = getTrampForInt32();
      #endif
        break;
    }

    // Install the trampoline as the callee's return address.
    framelib::setReturnAddress(calleeFrameBase, trampoline);

    this->armedFrame = victim;

    // Push ourself onto the queue of pending deoptimizers.
    this->nextArmed = methodInfo->_armed_deoptimizers;
    methodInfo->_armed_deoptimizers = this;
#ifdef DEOPT_TRACE
    if (deopt_trace) {
        AvmCore* core = victim->env()->core();
        core->console << "==> ";
        showArmed(core);
    }
#endif
}

// We should likely not export this in its raw form,
// exposing "bogus" frames used to hold CodeContexts.
// Perhaps we want a 'currentMethodEnv()' call instead?
MethodFrame* Deoptimizer::methodFrame(AvmCore* core)
{
    return core->currentMethodFrame;
}

MethodFrame* Deoptimizer::nextActivation(MethodFrame* startFrame)
{
    AvmAssert(startFrame != NULL);
    MethodFrame* frame = startFrame;
    while (frame != NULL) {
        MethodEnv *env = frame->env();
        if (!env) {
            // MethodFrame is holder for CodeContext, so skip it.
            continue;
        }
        // TODO: Don't waste an entire word on this flag!
        // We can probably repurpose another low-order address
        // bit following existing precedent in MethodFrame.
        if (frame->isInterp) {
            frame = frame->next;
            continue;
        }
        if (env->method == this->methodInfo) {
            // We found another activation.
            return frame;
        }
        frame = frame->next;
    }
    return NULL;
}

void Deoptimizer::deoptFrameUponReturnWithResult(DeoptContext* ctx, uint8_t* fp, MethodEnv* env, void* resultPtr)
{
    AvmCore* core = env->core();

#ifdef DEOPT_TRACE
    if (deopt_trace) {
        core->console << "$$$ deopt fired " << hexAddr((uintptr_t)this) << "\n";
    }
#endif

    // The frame to deoptimize must be an activation of the method to which we belong.
    AvmAssert(env->method == this->methodInfo);

    // We must have previously armed this deoptimizer.
    AvmAssert(resumptionPc != 0);

    ctx->setSafepointFromNativePc(resumptionPc);

    resumptionPc = 0;

    //### Not necessarily!
    AvmAssert(ctx->isSafepointAtAbcCall());

    // Note that the 'fp' argument is the native frame pointer.
    uint8_t* frameBase = fp;
    MethodFrame* frame = ctx->methodFrame(frameBase);

    // Verify that this is the correct deoptimizer for the code that
    // created the frame.  Note that frame->deopt is used only for
    // assertion checks, and need not be maintained in production.
    AvmAssert(frame->deopt == this);

    // Exit the victim frame, as the interpreter will re-establish it.
    frame->exit(core);

    const MethodSignature *ms = methodInfo->getMethodSignature();

    // Box the result returned from the callee.
    Atom returnVal = nativeLocalToAtom(core, resultPtr,  ctx->safepointReturnValueSST());

    // Pop this deoptimizer.
    armedFrame = NULL;
    AvmAssert(methodInfo->_armed_deoptimizers == this);
    methodInfo->_armed_deoptimizers = nextArmed;
    nextArmed = NULL;
#ifdef DEOPT_TRACE
    if (deopt_trace) {
        core->console << "<== ";
        showArmed(core);
    }
#endif

    // Search for the previous activation of the method, and arm for lazy deoptimization.
    MethodFrame* prevFrame = nextActivation(methodFrame(core));
    if (prevFrame) {
        Deoptimizer* armed = methodInfo->_armed_deoptimizers;
        // There is a pending activation of the method.
        if (!armed || armed->armedFrame != prevFrame) {
            // We are still the appropriate deoptimizer for this frame.
            armForLazyDeoptimization(prevFrame);
        }
    }

    // Interpreter frame will reconstruct state at call, with boxed result pushed on operand stack.
    Atom retval = ctx->interpretFromCallSafepoint(frameBase, env, returnVal);

    // Return from lazily-deoptimized frame with the unboxed value from the interpreter.
    returnFromFrameWithUnboxedValue(frameBase, retval, ms->returnTraits());
}


// Begin interpretive execution at the specfied safepoint in the given frame.
// The frame must be an activation of the AS3 method to which this member belongs.
// TODO: Given the intended use of enterHandler, we could trim this down to the
// specialized case of the currently executing frame only.

void Deoptimizer::enterInterpreter(DeoptContext* ctx, MethodFrame* frame, int32_t vpc)
{
    MethodEnv* env = frame->env();
    AvmCore* core = env->core();

    // The frame to deoptimize must be an activation of the method to which we belong.
    AvmAssert(env->method == this->methodInfo);

#ifdef DEOPT_TRACE
    if (deopt_trace) {
        core->console << "$$$ deopt interp " << hexAddr((uintptr_t)this) << " @ " << hexAddr((uintptr_t)frame) << "\n";
    }
#endif

    // Unlink the intervening frames, including the victim.
    frame->exit(core);

    // Continue at safepoint instruction.
    ctx->setSafepointFromVirtualPc(vpc);
    uint8_t* frameBase = ctx->frameBase(frame);
    Atom result = ctx->interpretFromSafepoint(frameBase, env);

    // Return from current frame with interpreter result as our result.
    const MethodSignature *ms = methodInfo->getMethodSignature();
    returnFromFrameWithUnboxedValue(frameBase, result, ms->returnTraits());
}

#ifdef  VMCFG_DEOPT_DIRECT_EXN
// TODO: Cribbed from jit-calls.h.  Should be shared.
union AnyVal {
    Atom atom;              // SST_atom
    const Namespace* ns;    // SST_namespace
    String* str;            // SST_string
    ScriptObject* obj;      // SST_scriptobject
    int32_t i;              // SST_int32
    uint32_t u;             // SST_uint32
    int32_t b;              // SST_bool32
    double d;               // SST_double
};

// defined in jit-calls.h
extern void set_slot_from_atom(Atom atom, Traits* traits, AnyVal* slotPtr, uint8_t* tagPtr);
#endif

// Attempt to handle an exception in the method activation to which 'ef' and 'handlerFrame'
// belong.  If no handler exists, we re-throw.  We do not simply search up the ExceptionFrame
// chain, as each frame may follow a different protocol (e.g., interpreter vs. JIT-ed code).
// See beginCatch() in jit-calls.h

void Deoptimizer::handleException(DeoptContext* ctx, AvmCore* core, ExceptionFrame* ef, MethodFrame* handlerFrame)
{
    // The ExceptionFrame 'ef' must belong to the handling method activation.
    AvmAssert(ef->savedMethodFrame == handlerFrame);

    // Get the exception being thrown.
    Exception* exception = core->exceptionAddr;

    // Restore enclosing exception frame.
    ef->beginCatch();

    // Calculate base of native frame.
    uint8_t* frameBase = ctx->frameBase(handlerFrame);

#ifdef VMCFG_DEOPT_SAFEPOINT_NATIVE
    // Reconstruct virtual ABC pc from safepoint metadata, given the native pc.
    uint8_t* nativePc = framelib::getReturnAddress(ctx->calleeFrameBase(frameBase));
    ctx->setSafepointFromNativePc(nativePc);
    int32_t exnVpc = ctx->safepointVpc();
#else
    // Extract the virtual ABC pc from the frame of the handling method activation.
    int32_t exnVpc = ctx->saveEip(frameBase);
#endif

    // Find handler for throw at location exnPc.  Rethrows if no applicable handler exists.
    ExceptionHandler* handler = core->findExceptionHandler(methodInfo, exnVpc, exception);

#ifdef VMCFG_DEOPT_DIRECT_EXN
    //### This depends on CodegenLIR!
    // Re-establish exception-handling context for possible exceptions raised in handler.
    ef->beginDeoptUnwindTry(core);

    Atom* vars = addrInFrame<Atom>(frameBase, vars_offset_);
    uint8_t* tags = addrInFrame<uint8_t>(frameBase, tags_offset_);

    const MethodSignature *ms = methodInfo->getMethodSignature();
    int stack_base = ms->stack_base();

    // Set operand_stack[0] to exception object.
    set_slot_from_atom(exception->atom, handler->traits, ((AnyVal*)((uint8_t*)vars + stack_base * 8)), (tags + stack_base));

    // Reset the stack and the callee-saved registers using explicitly-saved state.
    // A proper unwinder that understands callee-saved registers would be more efficient.
    framelib::resumeFrameAtLocation((uint8_t*)&ef->jmpbuf, handler->target_address);
#else
    MethodEnv* env = handlerFrame->env();

    // The frame to deoptimize must be an activation of the method to which we belong.
    AvmAssert(env->method == this->methodInfo);

#ifdef DEOPT_TRACE
    if (deopt_trace) {
        core->console << "$$$ deopt interp " << hexAddr((uintptr_t)this) << " @ " << hexAddr((uintptr_t)handlerFrame) << "\n";
    }
#endif
    // Unlink the intervening frames, including the victim.
    handlerFrame->exit(core);

    // Continue at handler target, with stacks reset and exception pushed.
    Atom result = ctx->interpretFromThrowSafepoint(frameBase, env, handler->target, exception->atom);

    // Return from current frame with interpreter result as our result.
    const MethodSignature *ms = methodInfo->getMethodSignature();
    returnFromFrameWithUnboxedValue(frameBase, result, ms->returnTraits());
#endif
}


#ifdef DEOPT_TRACE

//static
int Deoptimizer::frameDepth = 0;

//static
void Deoptimizer::showFrames(AvmCore* core)
{
    if (!deopt_trace) return;

    int limit = 5;
    for (MethodFrame* frame = core->methodFrame(); frame != NULL; frame = frame->next) {
        if (limit-- < 0) {
            core->console << "...";
            break;
        }
        MethodInfo* m = frame->env()->method;
        char mode = (frame->isInterp) ? 'I' : 'J';
        core->console << " [" << m << " " << mode << " " << hexAddr((uintptr_t)frame) << " " << hexAddr((uintptr_t)frame->deopt) << "]";
    }
    core->console << "\n";
}

//static
void Deoptimizer::traceEnter(AvmCore* core)
{
    if (!deopt_trace) return;

    core->console << "[" << frameDepth++ << "]>>>";
    if (trace_verbose) {
        showFrames(core);
    } else {
        core->console << " " << hexAddr((uintptr_t)core->methodFrame()) << "\n";
    }
}

//static
void Deoptimizer::traceExit(AvmCore* core)
{
    if (!deopt_trace) return;

    core->console << "[" << --frameDepth << "]<<<";
    if (trace_verbose) {
        showFrames(core);
    } else {
        core->console << " " << hexAddr((uintptr_t)core->methodFrame()) << "\n";
    }
}

void Deoptimizer::showArmed(AvmCore* core)
{
    if (!deopt_trace) return;

    int limit = 5;
    for (Deoptimizer* deopt = methodInfo->_armed_deoptimizers; deopt != NULL; deopt = deopt->nextArmed) {
        if (limit-- < 0) {
            core->console << "...";
            break;
        }
        core->console << " {" << hexAddr((uintptr_t)deopt) << " @ " << hexAddr((uintptr_t)deopt->armedFrame) << "}";
    }
    core->console << "\n";
}

#endif // ifdef DEOPT_TRACE

// Debugging scaffolding.

//static
bool Deoptimizer::deoptAncestor(AvmCore* core, uint32_t k)
{
    for (MethodFrame* frame = methodFrame(core); frame != NULL; frame = frame->next) {
        MethodEnv *env = frame->env();
        if (!env) continue;  // MethodFrame is holder for CodeContext, so skip it.
        if (k-- == 0) {
            if (frame->isInterp)
                return true;
            MethodInfo* info = env->method;
            if (info->_deoptimizer) {
                core->exec->deoptimize(env);
            }
            return true;
        }
    }
    return false;
}

} // namespace avmplus


namespace framelib {
#ifdef VMCFG_IA32

// Target architecture and backend-dependent library routines for manipulating stack frames.
// Currently aimed at supporting deoptimization in Tamarin, though debugging suggests other
// extensions to the API.
//
// TODO: This stuff doesn't belong in the code generator source files, and probably shouldn't
// be in the nanojit namespace either.  Most likely, we should have a separate support library
// in a directory alongside 'nanojit'.

#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4731 )    // Don't complain about changing FP inside function.
#endif

// We assume eager saving of the callee-saved registers, i.e., all are spilled to consecutive
// slots at method entry regardless of whether they are actually used.

#ifndef NANOJIT_EAGER_REGSAVE
  #error "NANOJIT_FRAMELIB on i386 requires NANOJIT_EAGER_REGSAVE"
#endif

// In the following, we need control over register usage to satisfy
// ABI requirements.  The simplest way to do this is to hardcode the
// register assignments in the inline assembly snippets, but, on GCC,
// hiding register usage from the compiler's register allocator can be
// problematic.  We are relying on the fact that references to the C++
// parameters will be translated as an offset from EBP, without the need
// to allocate additional temporary registers.  Note that static
// variable references in position-independent code do not translate to
// a simple absolute address, and require such a temporary register.

// Given a frame handle representing a function activation, prune the stack and
// return from the activation with the callee-saved registers properly restored.

void returnFromFrame(uint8_t* frame)
{
#if _MSC_VER
    _asm {
        mov eax, [frame]
        mov ebx, [eax-4]      // Restore EBX
        mov esi, [eax-8]      // Restore ESI
        mov edi, [eax-12]     // Restore EDI
        mov esp, [frame]      // Prune stack, including allocated slots of victim frame
        pop ebp               // Restore frame pointer of victim's caller
        ret                   // Pop return address and return to victim's caller
    }
#else
    asm volatile (
        "movl %[frame],   %%eax  \n\t"
        "movl -4(%%eax),  %%ebx  \n\t"
        "movl -8(%%eax),  %%esi  \n\t"
        "movl -12(%%eax), %%edi  \n\t"
        "movl %[frame],   %%esp  \n\t"
        "popl  %%ebp             \n\t"
        "ret                     \n\t"
        : : [frame] "m"(frame)
    );
#endif
    /*NOTREACHED*/
}

// Same as 'returnFromFrame()', but return an 32-bit integer result.

void returnFromFrameWithInt32(uint8_t* frame, int32_t value)
{
#if _MSC_VER
    _asm {
        mov eax, [frame]
        mov ebx, [eax-4]      // Restore EBX
        mov esi, [eax-8]      // Restore ESI
        mov edi, [eax-12]     // Restore EDI
        mov eax, [value]      // Integer result will be returned in EAX
        mov esp, [frame]      // Prune stack, including allocated slots of victim frame
        pop ebp               // Restore frame pointer of victim's caller
        ret                   // Pop return address and return to victim's caller
    }
#else
    asm volatile (
         "movl %[frame],   %%eax  \n\t"
         "movl -4(%%eax),  %%ebx  \n\t"
         "movl -8(%%eax),  %%esi  \n\t"
         "movl -12(%%eax), %%edi  \n\t"
         "movl %[value],   %%eax  \n\t"
         "movl %[frame],   %%esp  \n\t"
         "popl  %%ebp             \n\t"
         "ret                     \n\t"
         : : [frame] "m"(frame), [value] "m"(value)
    );
#endif
    /*NOTREACHED*/
}

// Same as 'returnFromFrame()', but return a double result.

void returnFromFrameWithDouble(uint8_t* frame, double value)
{
#if _MSC_VER
    _asm {
        mov eax, [frame]
        mov ebx, [eax-4]      // Restore EBX
        mov esi, [eax-8]      // Restore ESI
        mov edi, [eax-12]     // Restore EDI
        fld      [value]      // Double result will be returned in FST0
        mov esp, [frame]      // Prune stack, including allocated slots of victim frame
        pop ebp               // Restore frame pointer of victim's caller
        ret                   // Pop return address and return to victim's caller
    }
#else
    asm volatile (
        "movl %[frame],   %%eax  \n\t"
        "movl -4(%%eax),  %%ebx  \n\t"
        "movl -8(%%eax),  %%esi  \n\t"
        "movl -12(%%eax), %%edi  \n\t"
        "fldl %[value]           \n\t"
        "movl %[frame],   %%esp  \n\t"
        "popl %%ebp              \n\t"
        "ret                     \n\t"
        : : [frame] "m"(frame), [value] "m"(value)
    );
#endif
    /*NOTREACHED*/
}

#ifdef NANOJIT_64BIT
//void returnFromFrameWithInt64(uint8_t* frame, int64_t value)
#error "unimplemented"
#endif

// Get the return address from a frame.

uint8_t* getReturnAddress(uint8_t* frame)
{
    return *((uint8_t**)(frame+4));
}

// Install a new return address in a frame, e.g., to hook returns.

void setReturnAddress(uint8_t* frame, uint8_t* address)
{
    *((uint8_t**)(frame+4)) = address;
}

// Get the first argument for a frame, assumed to be a pointer.

uint8_t* getPointerArgument0(uint8_t* frame)
{
    return *((uint8_t**)(frame+8));
}

// Capture the frame pointer, stack pointer, and callee-saved registers.

void saveActivationState(uint8_t* state)
{
#if _MSC_VER
    _asm {
        mov eax,      [state]
        mov [eax],    ebx       // Save EBX
        mov [eax+4],  esi       // Save ESI
        mov [eax+8],  edi       // Save EDI
        lea edx,      [ebp+8]   // Get stack pointer of our caller
        mov [eax+12], edx       // Save ESP of caller
        mov edx,      [ebp]     // Get frame pointer of caller
        mov [eax+16], edx       // Save EBP of caller
    }
#else
    asm volatile (
        "movl %[state], %%eax      \n\t"
        "movl %%ebx,    (%%eax)    \n\t"
        "movl %%esi,    4(%%eax)   \n\t"
        "movl %%edi,    8(%%eax)   \n\t"
        "leal 8(%%ebp), %%edx      \n\t"
        "movl %%edx,    12(%%eax)  \n\t"
        "movl (%%ebp),  %%edx      \n\t"
        "movl %%edx,    16(%%eax)  \n\t"
        : : [state] "m"(state)
    );
#endif
}

// Restore saved frame pointer, stack pointer, and callee-saved
// registers, and resume execution at the specified address.

void resumeFrameAtLocation(uint8_t* state, uint8_t* address)
{
#if _MSC_VER
    _asm {
        mov eax, [address]
        mov edx, [state]
        mov ebx, [edx]
        mov esi, [edx+4]
        mov edi, [edx+8]
        mov esp, [edx+12]
        mov ebp, [edx+16]
        jmp eax
    }
#else
    asm volatile (
        "movl %[address], %%eax  \n\t"
        "movl %[state],   %%edx  \n\t"
        "movl (%%edx),    %%ebx  \n\t"
        "movl 4(%%edx),   %%esi  \n\t"
        "movl 8(%%edx),   %%edi  \n\t"
        "movl 12(%%edx),  %%esp  \n\t"
        "movl 16(%%edx),  %%ebp  \n\t"
        "jmp *%%eax              \n\t"
        : : [state] "m"(state), [address] "m"(address)
    );
#endif
}

#if _MSC_VER
#pragma warning( pop )
#endif

#else

void returnFromFrame(uint8_t* /*frame*/) {
    AvmAssert(false && "not implemented");
}

void returnFromFrameWithInt32(uint8_t* /*frame*/, int32_t /*value*/) {
    AvmAssert(false && "not implemented");
}

void returnFromFrameWithDouble(uint8_t* /*frame*/, double /*value*/) {
    AvmAssert(false && "not implemented");
}

void returnFromFrameWithInt64(uint8_t* /*frame*/, int64_t /*value*/) {
    AvmAssert(false && "not implemented");
}

uint8_t* getReturnAddress(uint8_t* /*frame*/) {
    AvmAssert(false && "not implemented");
    return 0;
}

void setReturnAddress(uint8_t* /*frame*/, uint8_t* /*address*/) {
    AvmAssert(false && "not implemented");
}

uint8_t* getPointerArgument0(uint8_t* /*frame*/) {
    AvmAssert(false && "not implemented");
    return 0;
}

void resumeFrameAtLocation(uint8_t* /*state*/, uint8_t* /*address*/) {
    AvmAssert(false && "not implemented");
}

void saveActivationState(uint8_t* /*state*/) {
    AvmAssert(false && "not implemented");
}

#endif // ifdef VMCFG_IA32, etc
} // end namespace framelib

#endif // VMCFG_HALFMOON
