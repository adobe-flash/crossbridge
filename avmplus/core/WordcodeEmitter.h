/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
#ifdef VMCFG_WORDCODE

    typedef MMgc::LeafVector<uintptr_t,MMgc::GC::kZero> TranslatedCode;

    class WordcodeEmitter : public WordcodeTranslator {
    public:
        WordcodeEmitter(MethodInfo* info, Toplevel* toplevel);
#    ifdef VMCFG_SELFTEST
        WordcodeEmitter(AvmCore* core, uint8_t* code_start);
#    endif
        virtual ~WordcodeEmitter();

        // In all cases below, pc points to the opcode.

        virtual void computeExceptionFixups();

        // Call before every instruction to handle exception range translation and
        // fix up branches to this address
        virtual void fixExceptionsAndLabels(const uint8_t *pc);

        // Paste up the translated code and install it in info.  Return the number
        // of words and install a pointer to the first word in 'code' if not NULL
        virtual uint32_t epilogue(uintptr_t** code_result = NULL);

        // Handle specific instructions or instruction classes
        virtual void emitOp0(const uint8_t *pc, WordOpcode opcode);
        virtual void emitOp1(const uint8_t *pc, WordOpcode opcode);
        virtual void emitOp1(WordOpcode opcode, uint32_t operand);
        virtual void emitOp2(const uint8_t *pc, WordOpcode opcode);
        virtual void emitOp2(WordOpcode opcode, uint32_t op1, uint32_t op2);
#ifdef DEBUGGER
        virtual void emitDebug(const uint8_t *pc);
#endif
        virtual void emitRelativeJump(const uint8_t *pc, WordOpcode opcode);
        virtual void emitLookupswitch(const uint8_t *pc);
        virtual void emitLabel(const uint8_t *pc);
        virtual void emitPushbyte(const uint8_t *pc);
        virtual void emitPushshort(const uint8_t *pc);
        virtual void emitPushint(const uint8_t *pc);
        virtual void emitPushuint(const uint8_t *pc);
        virtual void emitGetscopeobject(const uint8_t *pc);

        // CodeWriter
        void write(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, Traits *type = NULL);
        void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits *type = NULL);
        void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type = NULL);
        void writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo*, uintptr_t disp_id, uint32_t argc, Traits* type = NULL);
        void writeNip(const FrameState* state, const uint8_t *pc, uint32_t count);
        void writeCheckNull(const FrameState* state, uint32_t index);
        void writeCoerce(const FrameState* state, uint32_t index, Traits *type);
        void writeCoerceToNumeric(const FrameState* state, uint32_t index);
        void writeCoerceToFloat4(const FrameState* state, uint32_t index);
        void writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver*);
        void writeEpilogue(const FrameState* state);
        void writeBlockStart(const FrameState* state);
        void writeOpcodeVerified(const FrameState* state, const uint8_t *pc, AbcOpcode opcode);
        void writeFixExceptionsAndLabels(const FrameState* state, const uint8_t *pc);
        void cleanup();

    private:
        // 'backpatches' represent target addresses of forward jumps in the original code,
        // along with locations in the translated code that must be patched when the target
        // address in the new code is known.  There can be multiple backpatch structures
        // per target, one for each location that must be patched.  The backpatches are
        // sorted in address order: lowest address first.  Once a backpatch has been
        // consumed it can be deleted; when we're done, the list of backpatches should
        // be empty or there's an error.  Backpatch objects are managed by new/delete.
        //
        // 'labels' are sorted in address order: highest address first.  The list of
        // labels can be freed only at the end.  The labels are searched for every
        // backward branch, but the assumption is that most targets sought are
        // close by and that few probes are needed on the average.  That needs to be
        // verified.  Label objects are managed by new/delete.
        //
        // 'exception_fixes' represent addresses in the original code, along with locations
        // to be updated with corresponding offsets in the translated code.
        // The exception fixes are sorted in address order: lowest address first.  Once an
        // exception fix has been consumed it can be deleted; when we're done, the list of
        // exception fixes should be empty.  Exception fix objects are managed by new/delete.
        //
        // 'buffers' are sorted in reverse creation order: current buffer segment first.
        // The list of buffers is merged into a collectable object at the end of
        // translation, at which point buffers can be freed.  Buffer objects are managed
        // by new/delete.

        struct backpatch_info
        {
            const uint8_t* target_pc;       // the instruction in the old code that is the target of a forward control transfer
            uintptr_t* patch_loc;       // location in the new code into which to write the new offset
            uintptr_t patch_offset;     // value to subtract from offset of translated pc
            backpatch_info* next;
        };

        struct label_info
        {
            uintptr_t old_offset;
            uintptr_t new_offset;
            label_info* next;
        };

        struct catch_info
        {
            const uint8_t* pc;          // address in ABC code to trigger use of this structure
            int32_t *fixup_loc;         // points to a location to update in an ExceptionHandler
            catch_info* next;
        };

        struct buffer_info
        {
            uintptr_t data[100];
            int entries_used;
            buffer_info* next;
        };

        MethodInfo* info;
        AvmCore* core;
        Toplevel* avm_toplevel;             // for error classes; may be NULL
        backpatch_info* backpatches;    // in address order
        label_info* labels;             // in reverse offset order
        catch_info* exception_fixes;    // in address order
        buffer_info* buffers;           // newest buffer first
        uint32_t buffer_offset;         // offset of first word of current buffer
        buffer_info* spare_buffer;      // may be populated during peephole optimization; reused by refill
#ifdef VMCFG_DIRECT_THREADED
        void** opcode_labels;
#endif
        PoolObject *pool;
        const uint8_t* code_start;

        bool exceptions_consumed;
        uintptr_t *dest;
        uintptr_t *dest_limit;

        void refill();
        void emitRelativeOffset(uintptr_t base_offset, const uint8_t *pc, intptr_t offset);
        void makeAndInsertBackpatch(const uint8_t* target_pc, uintptr_t patch_offset);
        void boot();

#ifdef VMCFG_WORDCODE_PEEPHOLE

        // The structures are laid out so as to improve packing and conserve space.  The
        // included initialization code below knows the order of fields.

        struct peep_state_t
        {
            uint8_t  numTransitions;            // Number of consecutive in the transitions[] array starting at transitionPtr
            uint8_t  failShift;                 // Initial tokens to discard on a failure transition
            uint16_t transitionPtr;             // Location in transitions[] for our transitions, sorted in increasing token order
            uint16_t guardAndAction;            // 0 if this is not a final state, otherwise an identifier for a case in 'commit()'
            uint16_t fail;                      // 0 if there is no failure transition, otherwise a state number
        };

        struct peep_transition_t
        {
            uint16_t opcode;                    // on this opcode
            uint16_t next_state;                //   move to this state (never 0)
        };

        static const uint16_t toplevel[];             // Transition table for initial state
        static const peep_state_t states[];           // State 0 is not used
        static const peep_transition_t transitions[]; // Compact transition representation

        uint32_t  state;                        // current state in the matcher, or 0
        uint32_t  backtrack_stack[10];          // commit candidates (state numbers)
        uint32_t  backtrack_idx;                // next slot in backtrack_state
        uintptr_t* I[10];                       // longest window 10 instructions, not a problem now, generator can generate constant later
        uintptr_t  O[10];                       // symbolic opcodes for each I entry
        uintptr_t  nextI;                       // next slot in I and O
        uintptr_t  R[30];                       // replacement data
        uintptr_t  S[30];                       // symbolic opcode for some R entries

        void peepInit();
        void peep(uint32_t opcode, uintptr_t* loc);
        void peepFlush();
        bool commit(uint32_t action);
        bool replace(uint32_t old_instr, uint32_t new_words, bool jump_has_been_translated=false);
        void undoRelativeOffsetInJump();
        void shiftBuffers(uint32_t shift);

        bool isJumpInstruction(uintptr_t opcode) {
            return wopAttrs[opcode].jumps;
        }

        uint32_t calculateInstructionWidth(uintptr_t opcode) {
            return wopAttrs[opcode].width;
        }
#endif  // VMCFG_WORDCODE_PEEPHOLE
    };
#endif // AVMPUS_WORD_CODE
}
