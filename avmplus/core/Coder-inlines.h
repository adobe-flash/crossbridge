/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{
REALLY_INLINE CodeWriter::CodeWriter()
{ }

// convenience functions
REALLY_INLINE void CodeWriter::write(const FrameState* state, const uint8_t *pc, AbcOpcode opcode)
{
    write(state, pc, opcode, NULL);
}

REALLY_INLINE void CodeWriter::writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1)
{
    writeOp1(state, pc, opcode, opd1, NULL);
}

REALLY_INLINE void CodeWriter::writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2)
{
    writeOp2(state, pc, opcode, opd1, opd2, NULL);
}

} // namespace avmplus
