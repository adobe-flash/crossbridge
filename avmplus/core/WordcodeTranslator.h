/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
#ifdef VMCFG_WORDCODE

#  ifdef VMCFG_DIRECT_THREADED
#    define NEW_OPCODE(n)         ((uintptr_t)opcode_labels[n])
#  else
#    ifdef _DEBUG
#      define NEW_OPCODE(opcode)  ((opcode) | ((opcode) << 16))  // aids debugging
#    else
#      define NEW_OPCODE(opcode)  opcode
#    endif
#  endif

    // Abstract Base class
    class WordcodeTranslator : public CodeWriter
    {
    public:
        WordcodeTranslator() { };
        virtual ~WordcodeTranslator() { };

        // In all cases below, pc points to the opcode.

        virtual void computeExceptionFixups() = 0;

        // Call before every instruction to handle exception range translation and
        // fix up branches to this address
        virtual void fixExceptionsAndLabels(const uint8_t *pc) = 0;

        // Paste up the translated code and install it in info
        virtual uint32_t epilogue(uintptr_t** code_result = NULL) = 0;

        // Handle specific instructions or instruction classes
        virtual void emitOp0(const uint8_t *pc, WordOpcode opcode) = 0;
        void emitOp0(WordOpcode opcode) { emitOp0(NULL, opcode); }
        virtual void emitOp1(const uint8_t *pc, WordOpcode opcode) = 0;
        virtual void emitOp1(WordOpcode opcode, uint32_t operand) = 0;
        virtual void emitOp2(const uint8_t *pc, WordOpcode opcode) = 0;
        virtual void emitOp2(WordOpcode opcode, uint32_t op1, uint32_t op2) = 0;
#ifdef DEBUGGER
        virtual void emitDebug(const uint8_t *pc) = 0;
#endif
        virtual void emitRelativeJump(const uint8_t *pc, WordOpcode opcode) = 0;
        virtual void emitLookupswitch(const uint8_t *pc) = 0;
        virtual void emitLabel(const uint8_t *pc) = 0;
        virtual void emitPushbyte(const uint8_t *pc) = 0;
        virtual void emitPushshort(const uint8_t *pc) = 0;
        virtual void emitPushint(const uint8_t *pc) = 0;
        virtual void emitPushuint(const uint8_t *pc) = 0;
        virtual void emitGetscopeobject(const uint8_t *pc) = 0;

#ifdef SUPERWORD_PROFILING
        static void swprofStart();
        static void swprofStop();
        static void swprofCode(const uint32_t* start, const uint32_t* limit);
        static void swprofPC(const uint32_t* pc);
#endif
    };

#endif // AVMPUS_WORD_CODE
}
