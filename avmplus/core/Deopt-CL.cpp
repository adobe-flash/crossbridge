/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

// TODO: I believe this code is now obsolete and should be removed.
//#ifdef VMCFG_HALFMOON
#if 0

#include "CodegenLIR.h"
#include "Deopt.h"
#include "Deopt-CL.h"
#include "Interpreter.h"

namespace avmplus
{
    // Return a generic pointer to local slot at the specified index.
    static void* addrOfJitSlot(void* vars, int slot_ix, int shift) {
        return (void*)(((uint8_t*) vars) + (slot_ix << shift));
    }

    static const uint8_t* INVALID_PC_MARKER = (uint8_t*)0xfafafafa;

    CLSafepoint::CLSafepoint(MethodInfo* mi, const FrameState* state, CLSafepoint* next)
        : pc_(INVALID_PC_MARKER),
          abc_pc_(state->abc_pc),
          scope_depth_(state->scopeDepth),
          stack_depth_(state->stackDepth),
          next_(next)
    {
        const MethodSignature *ms = mi->getMethodSignature();

        int frame_size  = ms->frame_size();  // NOTE: May be padded on 64-bit platforms!
        int local_count = ms->local_count();
        int scope_base  = local_count;
        int stack_base  = local_count + ms->max_scope();

        // Set method-relative ABC instruction pointer, used by debugger.
        if (abc_pc_ == 0) {
            // Still in prologue, which we charge to the first instruction.
            abc_ix_ = 0;
        } else {
            // Substract absolute address of first ABC instruction.
            const uint8_t* codeStart = ms->abc_code_start();
            abc_ix_ = uint32_t(abc_pc_ - codeStart);
        }

        // TODO:  Allocate this in the correct arena.
        tags_ = new uint8_t[frame_size];
        for (int i = 0; i < frame_size; i++) tags_[i] = 0;

        // Capture machine type for each frame slot.
        // TODO: Tags for locals need not be captured at each safepoint.
        // We'll need to do things differently in Halfmoon anyway, and
        // we *will* need to note changes in the register allocation for
        // locals from safepoint to safepoint.

        for (int i = 0; i < local_count; i++) {
            tags_[i] = uint8_t(slotType(state, i));
        }

        for (int i = 0; i < state->scopeDepth; i++) {
            int j = scope_base + i;
            tags_[j] = uint8_t(slotType(state, j));
        }

        for (int i = 0; i < state->stackDepth; i++) {
            int j = stack_base + i;
            tags_[j] = uint8_t(slotType(state, j));
        }

        // DIRTY HACK!
        // We need to pass the current stack depth to the deadvars optimizer
        // in the LIR_safe instruction.  We take advantage of the fact that we
        // pass the address of the pc_ slot as the payload of th LIR_safe,
        // to be filled in during assembly.  We stash the stack depth in the
        // pc_ slot temporarily, where it can be accessed from the LIR_safe insn
        // prior to assembly, e.g., during deadvars optimization.
        // NOTE: We don't need this for safepoints that will be used only for
        // exception handling, as the stack is dead.

        pc_ = (uint8_t*)state->stackDepth;
    }

    SlotStorageType CLSafepoint::slotType(const FrameState* state, int i)
    {
        const FrameValue& val = state->value(i);
        if (exactlyOneBit(val.sst_mask)) {
            // Slot has a unique machine type, inferable from its traits.
            BuiltinType bt = Traits::getBuiltinType(val.traits);
            return valueStorageType(bt);
        } else {
            // Slot may have multiple machine types.  Must consult runtime tag.
            return SlotStorageType(0xff); // Magic cookie.
        }
    }

    // Fill in additional safepoint information needed only for calls.
    // NOTE: This method violates the principle that we not assume that
    // safepoints are objects, permitting easy replacement with an opaque
    // handle, likely implemented as a scalar.  The clunky way that safepoint
    // generation is driven by the verifier will be revisited in the context
    // of Halfmoon, however, so I'm just doing something simple and easy here.

    void CLSafepoint::finish(int argc, const uint8_t* nextpc, Traits* resultType)
    {
        // This needs to be the total number of slots that are popped off of the
        // operand stack by the call instruction, not including any results pushed.
        // Aside from the ABC call arguments, it must include the receiver, property
        // names, etc.
        pop_args_ = argc;

        // Capture the size of the call instruction.  This will be used to adjust
        // the captured abc_ix_ to reflect the return address of the call.
        call_size_ = nextpc - abc_pc_;

        return_sst_ =  valueStorageType(Traits::getBuiltinType(resultType));
    }

    CLSafepoint* CLDeoptData::addSafepoint(const FrameState* state)
    {
        // TODO: Allocate this in the correct arena.
        CLSafepoint* sp = new CLSafepoint(methodInfo, state, safepoints);
        safepoints = sp;
        return sp;
    }

    // Return the safepoint associated with the given virtual (ABC) pc.
    // Unlike lookups based on machine addresses, this must always be an exact match.

    CLSafepoint* CLDeoptData::findSafepointAtVirtualPc(int32_t vpc)
    {
        CLSafepoint* sp = safepoints;
        while (sp != NULL) {
            if (intptr_t(sp->abc_ix_) == vpc)
                return sp;
            sp = sp->next_;
        }
        return NULL;
    }

    // Return the safepoint associated with the given native pc.
    // The recorded address may precede the location at which we capture the pc at runtime,
    // so we search for the safepoint with the largest address preceding the given pc.

    CLSafepoint* CLDeoptData::findSafepointAtNativePc(uint8_t* pc)
    {
        CLSafepoint* sp = safepoints;
        intptr_t largest = 0;
        CLSafepoint* candidate = NULL;
        while (sp != NULL) {
            if ((intptr_t(sp->pc_) >= largest) && sp->pc_ <= pc) {
                largest = intptr_t(sp->pc_);
                candidate = sp;
            }
            sp = sp->next_;
        }
        return candidate;
    }

    // Frame populators for compiled methods generated by CodegenLIR.

    class CLFramePopulator : public FramePopulator {
    public:
        CLFramePopulator(MethodInfo* method, const MethodSignature* signature, CLSafepoint* safePoint, Atom* vars, uint8_t* tags);
        virtual void populate(Atom* framep, int *scopeDepth, int *stackDepth);
        virtual ~CLFramePopulator() {}  // Silence G++ warning.
    protected:
        SlotStorageType slotType(int i);

        void populateLocals(Atom* framep);
        void populateScopes(Atom* framep, int *scopeDepth);
        void populateStack(Atom* framep, int *stackDepth);
        AvmCore* core() const { return method_->pool()->core; }
        int varshift() const { return VARSHIFT(method_); }

        MethodInfo*             method_;
        const MethodSignature*  signature_;
        CLSafepoint*            safePoint_;
        Atom*                   vars_;
        uint8_t*                tags_;
    };

    CLFramePopulator::CLFramePopulator(MethodInfo* method,
                                       const MethodSignature* signature,
                                       CLSafepoint* safePoint, Atom* vars,
                                       uint8_t* tags)
        : method_(method),
          signature_(signature),
          safePoint_(safePoint),
          vars_(vars),
          tags_(tags)
    {}

    SlotStorageType CLFramePopulator::slotType(int i)
    {
        uint32_t tag = safePoint_->tags_[i];
        if (tag == 0xff) {
            // Fetch runtime tag value.
            return SlotStorageType(tags_[i]);
        }
        // Unique type is known at compile time.
        return SlotStorageType(tag);
    }

    void CLFramePopulator::populateLocals(Atom* framep)
    {
        int local_count = signature_->local_count();
        for (int i = 0; i < local_count; i++) {
            void *slotAddr = addrOfJitSlot(vars_, i, varshift());
            framep[i] = nativeLocalToAtom(core(), slotAddr, slotType(i));
        }
    }

    void CLFramePopulator::populateScopes(Atom* framep, int *scopeDepth)
    {
        int scope_base  = signature_->scope_base();
        int scope_depth = safePoint_->scope_depth_;

        for (int i = 0; i < scope_depth; i++) {
            int j = scope_base + i;
            void *slotAddr = addrOfJitSlot(vars_, j, varshift());
            framep[j] = nativeLocalToAtom(core(), slotAddr, slotType(j));
        }
        *scopeDepth = scope_depth;
    }
   
    void CLFramePopulator::populateStack(Atom* framep, int *stackDepth)
    {
        int stack_base  = signature_->stack_base();
        int stack_depth = safePoint_->stack_depth_;

        for (int i = 0; i < stack_depth; i++) {
            int j = stack_base + i;
            void *slotAddr = addrOfJitSlot(vars_, j, varshift());
            framep[j] = nativeLocalToAtom(core(), slotAddr, slotType(j));
        }
        *stackDepth = stack_depth;
    }

    void CLFramePopulator::populate(Atom* framep, int *scopeDepth, int *stackDepth)
    {
        populateLocals(framep);
        populateScopes(framep, scopeDepth);
        populateStack(framep, stackDepth);
    }

    class CLHandlerFramePopulator : public CLFramePopulator {
    public:
        CLHandlerFramePopulator(MethodInfo* method, const MethodSignature* signature, CLSafepoint* safePoint, Atom* vars, uint8_t* tags, Atom exnVal);
        virtual void populate(Atom* framep, int *scopeDepth, int *stackDepth);
        virtual ~CLHandlerFramePopulator()  {}  // Silence G++ warning.
    private:
        Atom             exnVal_;
    };
    
    CLHandlerFramePopulator::CLHandlerFramePopulator(MethodInfo* method,
                                                     const MethodSignature* signature,
                                                     CLSafepoint* safePoint,
                                                     Atom* vars,
                                                     uint8_t* tags,
                                                     Atom exnVal)
        : CLFramePopulator(method, signature, safePoint, vars, tags),
          exnVal_(exnVal)
    {}

    void CLHandlerFramePopulator::populate(Atom* framep, int *scopeDepth, int *stackDepth)
    {
        populateLocals(framep);

        *scopeDepth = 0;

        int stack_base  = signature_->stack_base();
        framep[stack_base] = exnVal_;
        *stackDepth = 1;
    }

    class CLReturnFramePopulator : public CLFramePopulator {
    public:
        CLReturnFramePopulator(MethodInfo* method, const MethodSignature* signature,
                               CLSafepoint* safePoint, Atom* vars, uint8_t* tags,
                               Atom returnVal);
        virtual void populate(Atom* framep, int *scopeDepth, int *stackDepth);
        virtual ~CLReturnFramePopulator()  {}  // Silence G++ warning.
    private:
        Atom             returnVal_;
    };

    CLReturnFramePopulator::CLReturnFramePopulator(MethodInfo* method,
                                                   const MethodSignature* signature,
                                                   CLSafepoint* safePoint,
                                                   Atom* vars, uint8_t* tags,
                                                   Atom returnVal)
        : CLFramePopulator(method, signature, safePoint, vars, tags),
          returnVal_(returnVal)
    {}

    void CLReturnFramePopulator::populate(Atom* framep, int *scopeDepth, int *stackDepth)
    {
        int stackIdx;

        populateLocals(framep);
        populateScopes(framep, scopeDepth);
        populateStack(framep, &stackIdx);

        stackIdx -= safePoint_->pop_args_;

        int stack_base  = signature_->stack_base();
        framep[stack_base + stackIdx] = returnVal_;
        *stackDepth = stackIdx + 1;
    }

    // Deoptimization context for compiled methods generated by CodegenLIR.

    class CLDeoptContext : public DeoptContext {
    public:

        CLDeoptContext(CLDeoptData* dd)
            : deoptData(dd),
              safePoint(NULL)
        {}

        uint8_t* frameBase(MethodFrame* methodFrame)
        {
            return (uint8_t*)methodFrame - deoptData->frame_offset_;
        }

        MethodFrame* methodFrame(uint8_t* frameBase)
        {
            return addrInFrame<MethodFrame>(frameBase, deoptData->frame_offset_);
        }

        int32_t saveEip(uint8_t* frameBase)
        {
            return *addrInFrame<int32_t>(frameBase, deoptData->eip_offset_);
        }

        uint8_t* calleeFrameBase(uint8_t* frameBase)
        {
            return frameBase + deoptData->nj_frame_size_;
        }

        void setSafepointFromVirtualPc(int32_t vpc)
        {
            safePoint = deoptData->findSafepointAtVirtualPc(vpc);
        }

        void setSafepointFromNativePc(uint8_t* pc)
        {
            safePoint = deoptData->findSafepointAtNativePc(pc);
        }

        int32_t safepointVpc()
        {
            AvmAssert(safePoint != NULL);
            return safePoint->abc_ix_;
        }

        bool isSafepointAtThrow()
        {
            AvmAssert(safePoint != NULL);
            return true; // NYI
        }

        bool isSafepointAtAbcCall()
        {
            AvmAssert(safePoint != NULL);
            return (safePoint->call_size_ != 0);
        }

        SlotStorageType safepointReturnValueSST()
        {
            AvmAssert(isSafepointAtAbcCall());
            return safePoint->return_sst_;
        }

        Atom interpretFromSafepoint(uint8_t* fp, MethodEnv* env) ;
        Atom interpretFromCallSafepoint(uint8_t* fp, MethodEnv* env, Atom returnVal);
        Atom interpretFromThrowSafepoint(uint8_t* fp, MethodEnv* env, int32_t handlerVpc, Atom exnVal);

    private:

        CLDeoptData* deoptData;
        CLSafepoint* safePoint;
    };

    Atom CLDeoptContext::interpretFromSafepoint(uint8_t* fp, MethodEnv* env)
    {
        // Continue at safepoint instruction.
        AvmAssert(safePoint != NULL);
        Atom* vars = addrInFrame<Atom>(fp, deoptData->vars_offset_);
        uint8_t* tags = addrInFrame<uint8_t>(fp, deoptData->tags_offset_);
        const MethodSignature* ms = env->method->getMethodSignature();
        CLFramePopulator populator(env->method, ms, safePoint, vars, tags);
        return interpBoxedAtLocation(env, safePoint->abc_ix_, populator);
    }

    Atom CLDeoptContext::interpretFromCallSafepoint(uint8_t* fp, MethodEnv* env, Atom returnVal)
    {
        AvmAssert(safePoint != NULL);
        AvmAssert(isSafepointAtAbcCall());
        Atom* vars = addrInFrame<Atom>(fp, deoptData->vars_offset_);
        uint8_t* tags = addrInFrame<uint8_t>(fp, deoptData->tags_offset_);
        // Interpreter frame will reconstruct state at call, with boxed result pushed on operand stack.
        const MethodSignature* ms = env->method->getMethodSignature();
        CLReturnFramePopulator populator(env->method, ms, safePoint, vars, tags, returnVal);
        return interpBoxedAtLocation(env, (safePoint->abc_ix_ + safePoint->call_size_), populator);
    }

    Atom CLDeoptContext::interpretFromThrowSafepoint(uint8_t* fp, MethodEnv* env, int32_t handlerVpc, Atom exnVal)
    {
        // Continue at handler target, with stacks reset and exception pushed.
        AvmAssert(safePoint != NULL);
        AvmAssert(isSafepointAtThrow());
        Atom* vars = addrInFrame<Atom>(fp, deoptData->vars_offset_);
        uint8_t* tags = addrInFrame<uint8_t>(fp, deoptData->tags_offset_);
        const MethodSignature* ms = env->method->getMethodSignature();
        CLHandlerFramePopulator populator(env->method, ms, safePoint, vars, tags, exnVal);
        return interpBoxedAtLocation(env, handlerVpc, populator);
    }

    // Deoptimizer for compiled methods generated by CodegenLIR.

    typedef DeoptimizerInstance<CLDeoptData, CLDeoptContext> CLDeoptimizer;

    Deoptimizer* CLDeoptData::createDeoptimizer()
    {
        // TODO: Allocate this in the correct arena.
        return new CLDeoptimizer(methodInfo, this);
    }

} // namespace avmplus {

#endif // VMCFG_HALFMOON
