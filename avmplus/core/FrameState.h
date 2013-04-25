/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_FrameState__
#define __avmplus_FrameState__

namespace avmplus
{
    /**
     * represents a value in the verifier
     */
    class FrameValue
    {
    public:
        Traits* traits;
        bool notNull;
        bool isWith;
#ifdef VMCFG_NANOJIT
        // One bit for each of 16 possible SlotStorageTypes for the native representation
        // of this value.  The JIT uses this mask to handle control-flow merges
        // of incompatible values (e.g. int and String*).  At merge points, masks
        // are OR-ed together.
        // If more than one bit is set, the type will be Object or *, and the JIT
        // will use a separate tag byte to convert the native representation to Atom.
        // See CodegenLIR::localGetp().
        uint16_t sst_mask;
#endif
    };

    /**
     * this object holds the stack frame state at any given block entry.
     * the frame state consists of the types of each local, each entry on the
     * scope chain, and each operand stack slot.
     */
    class FrameState
    {
        // Info about each local var in this frame.
        // Length is frameConsts->frameSize, one entry per local, scope, and stack operand.
    private:
        FrameValue *locals;
        MethodInfo* info;
    public:
        FrameState* wl_next; // next block in verifier->worklist.  ideally this is only accessed by Verifier.
        const uint8_t* abc_pc;  // pointer into abc bytecode
        int32_t scopeDepth;
        int32_t stackDepth;
        int32_t withBase;
    private:
        const int32_t frameSize;
        const int32_t scopeBase;
        const int32_t stackBase;
    public:
        bool targetOfBackwardsBranch; // true if this block is reachable from later code (in linear ABC order)
        bool targetOfExceptionBranch; // true if this block is reachable from exception dispatch
        bool wl_pending;    // true if this is in verifier->worklist.  Verifier::checkTarget() sets to true.

    public:
        FrameState(MethodSignaturep, MethodInfo*);
        ~FrameState();

        void init(const FrameState* other);
        FrameValue& value(int32_t i);
        const FrameValue& value(int32_t i) const;
        FrameValue& scopeValue(int32_t i);
        const FrameValue& scopeValue(int32_t i) const;
        FrameValue& stackValue(int32_t i);
        FrameValue& stackTop();
        int32_t sp() const;
        void setType(int32_t i, Traits* t);
		void setType(int32_t i, Traits* t, bool notNull, bool isWith = false);
        void pop(int32_t n=1);
        FrameValue& peek(int32_t n=1);
        const FrameValue& peek(int32_t n) const;
        void pop_push(int32_t n, Traits* t);
		void pop_push(int32_t n, Traits* t, bool notNull);
        void push(FrameValue& _value);
        void push(Traits* t);
        void push(Traits* traits, bool notNull);

    private:
        bool typeNotNull(Traits* t);
    };
}

#endif /* __avmplus_FrameState__ */
