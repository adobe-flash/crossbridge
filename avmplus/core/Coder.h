/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Coder__
#define __avmplus_Coder__

namespace avmplus
{
    /**
     * CodegenDriver defines the interface by which CodeWriters may call back
     * to their top-level driver, e.g. CodegenLIR calling back to Verifier.
     */
    class CodegenDriver {
    public:
        virtual ~CodegenDriver() {}

        virtual const uint8_t* getTryFrom() const = 0;
        virtual const uint8_t* getTryTo() const = 0;
        virtual bool hasFrameState(const uint8_t* pc) const = 0;
        virtual int getBlockCount() const = 0;
        virtual bool hasReachableExceptions() const = 0;
        virtual FrameState* getFrameState(const uint8_t* pc) const = 0;
    };

    class CodeWriter {
    public:

        CodeWriter();
        virtual ~CodeWriter();

        virtual void write(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, Traits *type);
        virtual void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits *type);
        virtual void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type);
        virtual void writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo*, uintptr_t disp_id, uint32_t argc, Traits* type);
        virtual void writeNip(const FrameState* state, const uint8_t *pc, uint32_t count);
        virtual void writeCheckNull(const FrameState* state, uint32_t index);
        virtual void writeCoerce(const FrameState* state, uint32_t index, Traits* type);
        virtual void writeCoerceToNumeric(const FrameState* state, uint32_t index);
        virtual void writeCoerceToFloat4(const FrameState* state, uint32_t index);
        virtual void writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver*);
        virtual void writeEpilogue(const FrameState* state);
        virtual void writeBlockStart(const FrameState* state);
        virtual void writeOpcodeVerified(const FrameState* state, const uint8_t *pc, AbcOpcode opcode);
        virtual void writeFixExceptionsAndLabels(const FrameState* state, const uint8_t *pc);
        virtual void cleanup();

        // convenience functions
        void write(const FrameState* state, const uint8_t *pc, AbcOpcode opcode);
        void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1);
        void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2);
    };

    class NullWriter : public CodeWriter {
    public:
        CodeWriter* coder;       // the next leg of the pipeline

        NullWriter(CodeWriter* coder);
        ~NullWriter();
        void write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type);
        void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits *type);
        void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type);
        void writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo*, uintptr_t disp_id, uint32_t argc, Traits* type);
        /** Remove the count items under the stack top.  Equivalent to: while (count--) {swap;pop} */
        void writeNip(const FrameState* state, const uint8_t *pc, uint32_t count);
        void writeCheckNull(const FrameState* state, uint32_t index);
        void writeCoerceToNumeric(const FrameState* state, uint32_t index);
        void writeCoerceToFloat4(const FrameState* state, uint32_t index1);
        void writeCoerce(const FrameState* state, uint32_t index, Traits *type);
        void writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver*);
        void writeEpilogue(const FrameState* state);
        void writeBlockStart(const FrameState* state);
        void writeOpcodeVerified(const FrameState* state, const uint8_t *pc, AbcOpcode opcode);
        void writeFixExceptionsAndLabels(const FrameState* state, const uint8_t *pc);
        void cleanup();
    };
}
#endif  /* __avmplus_Coder__ */
