/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
    NullWriter::NullWriter(CodeWriter* coder)
        : coder(coder) {
    }

    NullWriter::~NullWriter()
    {}

    void NullWriter::write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type)
    {
        coder->write(state, pc, opcode, type);
    }

    void NullWriter::writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd, Traits *type)
    {
        coder->writeOp1(state, pc, opcode, opd, type);
    }

    void NullWriter::writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type)
    {
        coder->writeOp2(state, pc, opcode, opd1, opd2, type);
    }

    void NullWriter::writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo* m, uintptr_t disp_id, uint32_t argc, Traits* type)
    {
        coder->writeMethodCall(state, pc, opcode, m, disp_id, argc, type);
    }

    void NullWriter::writeNip(const FrameState* state, const uint8_t *pc, uint32_t count)
    {
        coder->writeNip(state, pc, count);
    }

    void NullWriter::writeCheckNull(const FrameState* state, uint32_t index)
    {
        coder->writeCheckNull(state, index);
    }

    void NullWriter::writeCoerceToNumeric(const FrameState* state, uint32_t index)
    {
        coder->writeCoerceToNumeric(state, index);
    }

    void NullWriter::writeCoerceToFloat4(const FrameState* state, uint32_t index)
    {
        coder->writeCoerceToFloat4(state, index);
    }
    
    void NullWriter::writeCoerce(const FrameState* state, uint32_t index, Traits *type)
    {
        coder->writeCoerce(state, index, type);
    }

    void NullWriter::writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver* dr)
    {
        coder->writePrologue(state, pc, dr);
    }

    void NullWriter::writeEpilogue(const FrameState* state)
    {
        coder->writeEpilogue(state);
    }

    void NullWriter::writeBlockStart(const FrameState* state)
    {
        coder->writeBlockStart(state);
    }

    void NullWriter::writeOpcodeVerified(const FrameState* state, const uint8_t* pc, AbcOpcode opcode)
    {
        coder->writeOpcodeVerified(state, pc, opcode);
    }

    void NullWriter::writeFixExceptionsAndLabels(const FrameState* state, const uint8_t* pc)
    {
        coder->writeFixExceptionsAndLabels(state, pc);
    }

    void NullWriter::cleanup()
    {
        coder->cleanup();
    }

    CodeWriter::~CodeWriter()
    { }

    void CodeWriter::write(const FrameState*, const uint8_t *, AbcOpcode, Traits*)
    { }

    void CodeWriter::writeOp1(const FrameState*, const uint8_t *, AbcOpcode, uint32_t, Traits*)
    { }

    void CodeWriter::writeOp2(const FrameState*, const uint8_t *, AbcOpcode, uint32_t, uint32_t, Traits*)
    { }

    void CodeWriter::writeMethodCall(const FrameState*, const uint8_t *, AbcOpcode, MethodInfo*, uintptr_t, uint32_t, Traits*)
    { }

    void CodeWriter::writeNip(const FrameState*, const uint8_t *, uint32_t)
    { }

    void CodeWriter::writeCheckNull(const FrameState*, uint32_t)
    { }

    void CodeWriter::writeCoerceToNumeric(const FrameState*, uint32_t)
    { }

    void CodeWriter::writeCoerceToFloat4(const FrameState*, uint32_t)
    { }

    void CodeWriter::writeCoerce(const FrameState*, uint32_t, Traits*)
    { }

    void CodeWriter::writePrologue(const FrameState*, const uint8_t *, CodegenDriver*)
    { }

    void CodeWriter::writeEpilogue(const FrameState*)
    { }

    void CodeWriter::writeBlockStart(const FrameState*)
    { }

    void CodeWriter::writeOpcodeVerified(const FrameState*, const uint8_t *, AbcOpcode)
    { }

    void CodeWriter::writeFixExceptionsAndLabels(const FrameState*, const uint8_t *)
    { }

    void CodeWriter::cleanup()
    { }
}
