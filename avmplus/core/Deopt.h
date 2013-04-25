/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Deopt__
#define __avmplus_Deopt__

namespace avmplus
{
    /*
     * layout of vars array section of JIT compiled stack frame
     *
     *  var array is made up of three portions.
     *
     *                +---------------+
     * frame_size-1   |               |         ^  OPERAND STACK    max_stack
     *                +---------------+         |     (grows towards higher addresses)
     *                |               |         |
     *                +---------------+         | operand_stack_base
     *                |               |         v_______________
     *                +---------------+         ^
     *                |               |         |
     *                +---------------+         | LOCAL SCOPE STACK max_scope
     *                |               |         | scope_stack_base
     *                +---------------+         v_________________
     * local_count-1  |               |         ^
     *                +---------------+         |
     *                |               |         |
     *                +---------------+         |
     *                |               |         | LOCALS            local_count
     *                +---------------+         |
     *   0            |               |         |
     *                +---------------+         v
     */

    // Objects implementing the DeoptContext interface are created transiently
    // to interpret the (possibly compressed) metadata in a manner independent
    // of the code generator that created it.  In this way, we avoid burdening
    // the subclasses of Deoptimizer with permanently-allocated storage to meet
    // these transient needs.

    class DeoptContext {
    public:
        // TODO: At present, the context represents a single canonical frame.
        // To deoptimize functions containing inlined calls, the context must represent
        // a stack of canonical (interpreter) frames, and we will need methods to step
        // through this stack, beginning with the innermost invocation.

        // METHOD GLOBAL: These operations are always valid.

        // Given a pointer to the MethodFrame object for this activation, return the native
        // code frame base (FP).  This result is valid even if we have not yet set a safepoint location.
        virtual uint8_t* frameBase(MethodFrame* methodFrame) = 0;

        // Given a native code frame base (FP) for this activation, return a pointer to the
        // MethodFrame object.  This result is valid even if we have not yet set a safepoint location.
        virtual MethodFrame* methodFrame(uint8_t* frameBase) = 0;

        // Given a native code frame base (FP) for this activation, return the value of the
        // '_save_eip' slot in the frame.
        // TODO: Should do something sensible if _save_eip was not defined for this frame.
        virtual int32_t saveEip(uint8_t* frameBase) = 0;

        // Given the native code frame base for this activation, return the native
        // code framebase of our immediate callee.  Note that there is such a call
        // at *every* safepoint.  If the safepoint is not at an ABC call instruction,
        // there must have been a native call into the runtime associated with that
        // instruction, otherwise, we could never arrive here.
        virtual uint8_t* calleeFrameBase(uint8_t* frameBase) = 0;

        // Set the current safepoint from a virtual (ABC) pc.
        // The location provided must be exactly the vpc for the safepoint instruction.
        virtual void setSafepointFromVirtualPc(int32_t vpc) = 0;

        // Set the current safepoint from a native address.
        // The safepoint returned is the safepoint at the nearest prior address.
        // This convention accommodates the expansion of a single ABC instruction
        // into multiple machine-code instructions.
        virtual void setSafepointFromNativePc(uint8_t* pc) = 0;

        // SAFEPOINT RELATIVE: These operations refer to the current safepoint.

        // The virtual (ABC) pc of the current safepoint.
        virtual int32_t safepointVpc() = 0;

        // True iff the virtual pc is at an instruction that can throw.
        virtual bool isSafepointAtThrow() = 0;

        // True iff the virtual pc is at a call instruction.
        virtual bool isSafepointAtAbcCall() = 0;

        // If the current virtual pc is a call instruction, return the machine type of the
        // result of the call, else result is undefined.
        virtual SlotStorageType safepointReturnValueSST() = 0;

        // Restore the canonical interpreter frame from current context and interpret from
        // the current virtual pc to the end of the method.
        virtual Atom interpretFromSafepoint(uint8_t* fp, MethodEnv* env) = 0;
        
        // Valid only if the virtual pc of the current context is at a call instruction.
        // Like interpret, but adjust the operand stack to reflect the completion of the call.
        // Operands to the call are removed from the stack, and the result is pushed onto it.
        // Advances the virtual pc over the call before resuming execution.
        virtual Atom interpretFromCallSafepoint(uint8_t* fp, MethodEnv* env, Atom returnVal) = 0;

        // Valid only if the virtual pc of the current context is at an instruction that can
        // throw, including a call instruction.  Restore the locals, but clear the scope
        // stack, and push a single value on the operand stack.  Interpret from the provided
        // handler virtual pc to the end of the method.
        virtual Atom interpretFromThrowSafepoint(uint8_t* fp, MethodEnv* env, int32_t handlerVpc, Atom exnVal) = 0;

        virtual ~DeoptContext() {}
    };

    class Deoptimizer {
    public:

        // Arm the topmost activation of the method to which this deoptimizer belongs
        // for deoptimization, and arrange for the deoptimization of all earlier activations
        // as the stack unwinds.  The caller is responsible for changing the execution
        // strategy for subsequent invcations of the method.  Normally, we should be called
        // only from subclasses of ExecMgr.
        void deoptimize(AvmCore* core);

        // Enter the interpreter at the specified virtual pc within the method to which
        // the deoptimizer belongs, and in the given activation.  The virtual pc must
        // correspond to a safepoint, and the activation must be at that safepoint.
        virtual void enterInterpreter(MethodFrame* frame, int32_t vpc) = 0;

        // Attempt to handle an exception within an activation of the method to which this
        // deoptimizer belongs.
        virtual void handleException(AvmCore* core, ExceptionFrame* ef, MethodFrame* handlerFrame) = 0;

        // Arm activation so that deoptTrameUponResult() will be invoked when it returns.
        virtual void armForLazyDeoptimization(MethodFrame* victim) = 0;

        // For debugging only.
        static bool deoptAncestor(AvmCore* core, uint32_t k);

        #ifdef DEOPT_TRACE
        static int frameDepth;
        static void showFrames(AvmCore* core);
        static void traceEnter(AvmCore* core);
        static void traceExit(AvmCore* core);
        void showArmed(AvmCore* core);
        #endif
        
        // This should be private, but must be accessible to the trampolines.
        virtual void deoptFrameUponReturnWithResult(uint8_t* fp, MethodEnv* env, void* resultPtr) = 0;

        virtual ~Deoptimizer() {}

    protected:

        Deoptimizer(MethodInfo* info)
            : methodInfo(info),
              nextArmed(NULL),
              armedFrame(NULL),
              resumptionPc(0)
        {}

        // Shared code invoked by the virtual functions above.
        // Overrides simply allocate a context of the appriate type and call the functions below.
        void enterInterpreter(DeoptContext* ctx, MethodFrame* frame, int32_t vpc);
        void handleException(DeoptContext* ctx, AvmCore* core, ExceptionFrame* ef, MethodFrame* handlerFrame);
        void armForLazyDeoptimization(DeoptContext* ctx, MethodFrame* victim);
        void deoptFrameUponReturnWithResult(DeoptContext* ctx, uint8_t* fp, MethodEnv* env, void* resultPtr);

    private:
        // Internal helper functions.
        MethodFrame* nextActivation(MethodFrame* startFrame);
        static void returnFromFrameWithUnboxedValue(uint8_t* frameBase, Atom atom, Traits* t);
        static MethodFrame* methodFrame(AvmCore*);

    private:

        MethodInfo*  methodInfo;
        Deoptimizer* nextArmed;
        MethodFrame* armedFrame;
        uint8_t*     resumptionPc;   // old value of return address clobbered by trampoline

    };

    // A concrete sublcass of Deoptimizer is based on an underlying type representing the raw deopt metadata,
    // and a concrete subclass of DeoptContext that inteprets metadata in that format.  The argument to the
    // constructor for the DeoptContext must match the type of the raw metadata.  The trampolines defined in
    // this template allow us to avoid any further specialization of Deoptimizer itself.

    template<typename DEOPTDATA, class DEOPTCONTEXT>
    class DeoptimizerInstance : public Deoptimizer {
    public:

        DeoptimizerInstance(MethodInfo* info, DEOPTDATA* dd)
            : Deoptimizer(info),
              metaData(dd)
        {}

        void enterInterpreter(MethodFrame* frame, int32_t vpc)
        {
            DEOPTCONTEXT ctx(metaData);
            Deoptimizer::enterInterpreter(&ctx, frame, vpc);
        }

        void handleException(AvmCore* core, ExceptionFrame* ef, MethodFrame* handlerFrame)
        {
            DEOPTCONTEXT ctx(metaData);
            Deoptimizer::handleException(&ctx, core, ef, handlerFrame);
        }

        void armForLazyDeoptimization(MethodFrame* victim)
        {
            DEOPTCONTEXT ctx(metaData);
            Deoptimizer::armForLazyDeoptimization(&ctx, victim);
        }

        void deoptFrameUponReturnWithResult(uint8_t* fp, MethodEnv* env, void* resultPtr)
        {
            DEOPTCONTEXT ctx(metaData);
            Deoptimizer::deoptFrameUponReturnWithResult(&ctx, fp, env, resultPtr);
        }

    private:
        DEOPTDATA* metaData;
    };

    /**
     * Utility function to return pointer to location of type T at the given
     * offset from fp.
     */
    template<class T> T* addrInFrame(void* fp, int offset) {
        return (T *)((uint8_t*) fp + offset);
    }
} //namespace

#endif // __avmplus_Deopt_
