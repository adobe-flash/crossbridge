/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_WORDCODE
#include "Interpreter.h"

namespace avmplus
{
    using namespace MMgc;

    WordcodeEmitter::WordcodeEmitter(MethodInfo* info, Toplevel* avm_toplevel)
        : WordcodeTranslator()
        , info(info)
        , core(info->pool()->core)
        , avm_toplevel(avm_toplevel)
        , backpatches(NULL)
        , labels(NULL)
        , exception_fixes(NULL)
        , buffers(NULL)
        , buffer_offset(0)
        , spare_buffer(NULL)
#ifdef VMCFG_DIRECT_THREADED
        , opcode_labels(interpGetOpcodeLabels())
#endif
        , pool(NULL)
        , code_start(NULL)
        , exceptions_consumed(false)
        , dest(NULL)
        , dest_limit(NULL)
    {
        AvmAssert(info != NULL);

        const uint8_t* pos = info->abc_body_pos();
        AvmCore::skipU32(pos, 5);  // max_stack, local_count, init_scope_depth, max_scope_depth, code_length
        code_start = pos;
        pool = info->pool();
        boot();
    }

#ifdef VMCFG_SELFTEST
    WordcodeEmitter::WordcodeEmitter(AvmCore* core, uint8_t* code_start)
        : WordcodeTranslator()
        , info(NULL)
        , core(core)
        , backpatches(NULL)
        , labels(NULL)
        , exception_fixes(NULL)
        , buffers(NULL)
        , buffer_offset(0)
        , spare_buffer(NULL)
#ifdef VMCFG_DIRECT_THREADED
        , opcode_labels(interpGetOpcodeLabels())
#endif
        , pool(NULL)
        , code_start(code_start)
        , exceptions_consumed(false)
        , dest(NULL)
        , dest_limit(NULL)
    {
        boot();
    }

#endif // VMCFG_SELFTEST

    void WordcodeEmitter::boot() {
        computeExceptionFixups();
        refill();
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peepInit();
#endif
    }

    WordcodeEmitter::~WordcodeEmitter()
    {
        cleanup();
    }

#define DELETE_LIST(T, v) \
    do { \
        T* tmp1 = v; \
        while (tmp1 != NULL) { \
            T* tmp2 = tmp1; \
            tmp1 = tmp1->next; \
            delete tmp2; \
        } \
        v = NULL; \
    } while (0)

    void WordcodeEmitter::cleanup()
    {
        DELETE_LIST(backpatch_info, backpatches);
        DELETE_LIST(label_info, labels);
        DELETE_LIST(catch_info, exception_fixes);
        DELETE_LIST(buffer_info, buffers);
        if (spare_buffer) {
            delete spare_buffer;
            spare_buffer = NULL;
        }
    }

    void WordcodeEmitter::refill()
    {
        if (buffers != NULL) {
            buffers->entries_used = int(dest - buffers->data);
            buffer_offset += buffers->entries_used;
        }
        buffer_info* b;
        if (spare_buffer != NULL) {
            b = spare_buffer;
            spare_buffer = NULL;
        }
        else
            b = new buffer_info;
        b->next = buffers;
        buffers = b;
        dest = b->data;
        dest_limit = dest + sizeof(b->data)/sizeof(b->data[0]);
    }

    void WordcodeEmitter::emitRelativeOffset(uintptr_t base_offset, const uint8_t *base_pc, intptr_t offset)
    {
        if (offset < 0) {
            // There must be a label for the target location
            uintptr_t old_offset = uintptr_t((base_pc - code_start) + offset);
            label_info* l = labels;
            while (l != NULL && l->old_offset != old_offset)
                l = l->next;
            // See https://bugzilla.mozilla.org/show_bug.cgi?id=481171.  Verifier should have caught the invalid target.
            AvmAssert(l != NULL);
            *dest++ = l->new_offset - base_offset;
        }
        else
            makeAndInsertBackpatch(base_pc + offset, base_offset);
    }

    void WordcodeEmitter::makeAndInsertBackpatch(const uint8_t* target_pc, uintptr_t patch_offset)
    {
        // Leave a backpatch for the target location.  Backpatches are sorted in
        // increasing address order always.
        backpatch_info* b = new backpatch_info;
        b->target_pc = target_pc;
        b->patch_loc = dest;
        b->patch_offset = patch_offset;
        backpatch_info* q = backpatches;
        backpatch_info* qq = NULL;
        while (q != NULL && q->target_pc < b->target_pc) {
            qq = q;
            q = q->next;
        }
        if (qq == NULL) {
            b->next = backpatches;
            backpatches = b;
        }
        else {
            b->next = q;
            qq->next = b;
        }
        *dest++ = 0x80000000U;
    }

    void WordcodeEmitter::computeExceptionFixups()
    {
        if (info == NULL || info->abc_exceptions() == NULL)
            return;

        DELETE_LIST(catch_info, exception_fixes);

        const ExceptionHandlerTable* old_table = info->abc_exceptions();
        int exception_count = old_table->exception_count;
        ExceptionHandlerTable* new_table = ExceptionHandlerTable::create(core->GetGC(), exception_count);

        // Insert items in the exn list for from, to, and target, with the pc pointing
        // to the correct triggering instruction in the ABC and the update loc
        // pointing to the location to be patched; and a flag is_int_offset (if false
        // it's a intptr_t).

        for ( int i=0 ; i < exception_count ; i++ ) {

            new_table->exceptions[i].traits = old_table->exceptions[i].traits;
            new_table->exceptions[i].scopeTraits = old_table->exceptions[i].scopeTraits;

            catch_info* p[3];

            p[0] = new catch_info;
            p[0]->pc = code_start + old_table->exceptions[i].from;
            p[0]->fixup_loc = &(new_table->exceptions[i].from);

            p[1] = new catch_info;
            p[1]->pc = code_start + old_table->exceptions[i].to;
            p[1]->fixup_loc = &(new_table->exceptions[i].to);

            p[2] = new catch_info;
            p[2]->pc = code_start + old_table->exceptions[i].target;
            p[2]->fixup_loc = &(new_table->exceptions[i].target);

            // Verifier guarantees from <= to <= target.
            AvmAssert(p[0]->pc <= p[1]->pc);
            AvmAssert(p[1]->pc <= p[2]->pc);

            int j=0;
            catch_info* e = exception_fixes;
            catch_info* ee = NULL;
            while (j < 3 && e != NULL) {
                if (e->pc > p[j]->pc) {
                    if (ee == NULL)
                        exception_fixes = p[j];
                    else
                        ee->next = p[j];
                    p[j]->next = e;
                    e = p[j];
                    j++;
                }
                else {
                    ee = e;
                    e = e->next;
                }
            }
            while (j < 3) {
                if (ee == NULL)
                    exception_fixes = p[j];
                else
                    ee->next = p[j];
                p[j]->next = e;
                ee = p[j];
                j++;
            }
        }

        info->set_word_code_exceptions(core->GetGC(), new_table);

#ifdef _DEBUG
        if (exception_fixes != NULL) {
            catch_info* ee = exception_fixes;
            catch_info* e = ee->next;
            AvmAssert(ee->pc <= e->pc);
            ee = e;
            e = e->next;
        }
#endif
    }

    void WordcodeEmitter::fixExceptionsAndLabels(const uint8_t *pc)
    {
#ifdef VMCFG_WORDCODE_PEEPHOLE
        // Do not optimize across control flow targets, so flush the peephole window here
        if (((exception_fixes != NULL) && (exception_fixes->pc == pc)) || ((backpatches != NULL) && (backpatches->target_pc == pc)))
            peepFlush();
#endif

        while (exception_fixes != NULL && exception_fixes->pc <= pc) {
            // the Verifier can skip dead code.  if this happens and we had a catch_info
            // record on a dead location, then the fixup floats to the next valid pc,
            // which is the one passed into this call.
            //AvmAssert(exception_fixes->pc == pc);
            exceptions_consumed = true;
            *exception_fixes->fixup_loc = (int)(buffer_offset + (dest - buffers->data));
            catch_info* tmp = exception_fixes;
            exception_fixes = exception_fixes->next;
            delete tmp;
        }

        while (backpatches != NULL && backpatches->target_pc <= pc) {
            AvmAssert(backpatches->target_pc == pc);
            AvmAssert(*backpatches->patch_loc == 0x80000000U);
            *backpatches->patch_loc = (intptr_t)buffer_offset + (intptr_t)(dest - buffers->data) - (intptr_t)backpatches->patch_offset;
            backpatch_info* tmp = backpatches;
            backpatches = backpatches->next;
            delete tmp;
        }
    }

#define CHECK(n) \
        if (dest+n > dest_limit) refill();

    // These take no arguments
    void WordcodeEmitter::emitOp0(const uint8_t *pc, WordOpcode opcode) {
#ifdef _DEBUG
        AvmAssert(wopAttrs[opcode].width == 1);
#endif // _DEBUG
        (void)pc;
        CHECK(1);
        *dest++ = NEW_OPCODE(opcode);
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(opcode, dest-1);
#endif
    }

    inline WordOpcode wordCode(AbcOpcode opcode) {
        return (WordOpcode)opcodeInfo[opcode].wordCode;
    }

    void WordcodeEmitter::writePrologue(const FrameState*, const uint8_t *pc, CodegenDriver*)
    {
        #if defined DEBUGGER
        if (core->debugger()) emitOp0(pc, WOP_debugenter);
        #else
        (void)pc;
        #endif
        computeExceptionFixups();
    }

    void WordcodeEmitter::writeEpilogue(const FrameState* state)
    {
        // Process any labels patches for unreachable catch blocks.
        fixExceptionsAndLabels(state->abc_pc);
        epilogue();
    }

    void WordcodeEmitter::writeBlockStart(const FrameState* state)
    {
        emitLabel(state->abc_pc);
        if (state->targetOfBackwardsBranch && info->hasExceptions()) {
            // If this label is the target of a back edge, and if this
            // function has exception handlers, then emit a NOP to ensure
            // we have a unique position in the code from which interrupt
            // exceptions will appear to be thrown from.  The interrupt
            // check is in the branch, but the interrupt *position* is at
            // the label.
            // also see bug 554915 and test/acceptance/abcasm/bug_554915a.abs.
            emitOp0(state->abc_pc, WOP_nop);
        }
    }

    void WordcodeEmitter::writeOpcodeVerified(const FrameState*, const uint8_t*, AbcOpcode)
    {}

    void WordcodeEmitter::writeFixExceptionsAndLabels(const FrameState*, const uint8_t *pc)
    {
        fixExceptionsAndLabels(pc);
    }

    void WordcodeEmitter::writeOp1(const FrameState *state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits *type)
    {
        (void)type;
        switch (opcode) {
        case OP_iflt:
        case OP_ifle:
        case OP_ifnlt:
        case OP_ifnle:
        case OP_ifgt:
        case OP_ifge:
        case OP_ifngt:
        case OP_ifnge:
        case OP_ifeq:
        case OP_ifstricteq:
        case OP_ifne:
        case OP_ifstrictne:
        case OP_iftrue:
        case OP_iffalse:
        case OP_jump:
            emitRelativeJump(pc, wordCode(opcode));
            break;
        case OP_getslot:
        {
            const uint8_t* nextpc = pc;
            unsigned int imm30=0, imm30b=0;
            int imm8=0, imm24=0;
            AvmCore::readOperands(nextpc, imm30, imm24, imm30b, imm8);
            Traits* t = state->value(state->sp()).traits;
            AvmAssert(t->isResolved());
            if (t->pool->isBuiltin && !t->final)  // de-optimize
                emitOp1(WOP_getproperty, imm30);
            else
                emitOp1(WOP_getslot, opd1+1);
            break;
        }
        case OP_getglobalslot:
        case OP_setglobalslot:
            emitOp1(pc, wordCode(opcode));
            break;
        case OP_call:
        case OP_construct:
        case OP_getouterscope:
        case OP_newfunction:
        case OP_newclass:
            emitOp1(wordCode(opcode), opd1);
            break;
        case OP_getscopeobject:
            emitOp1(WOP_getscopeobject, opd1);
            break;
        case OP_findpropglobal:
        case OP_findpropglobalstrict:
            emitOp1(wordCode(opcode), opd1);
            break;
        case OP_pushscope:
        case OP_pushwith:
            emitOp0(pc, wordCode(opcode));
            break;
        case OP_convert_s:
        case OP_esc_xelem:
        case OP_esc_xattr:
            // do nothing, implemented by write()
            break;

        case OP_findpropstrict:
        case OP_findproperty:
        case OP_finddef:
            emitOp1(wordCode(opcode), opd1);
            break;

        default:
            AvmAssert (false);
            break;
        }
    }

    void WordcodeEmitter::writeNip(const FrameState* state, const uint8_t *pc, uint32_t count)
    {
        while (count--) {
            write(state, pc, OP_swap);
            write(state, pc, OP_pop);
        }
    }

    void WordcodeEmitter::writeCheckNull(const FrameState*, uint32_t)
    {}

    void WordcodeEmitter::writeMethodCall(const FrameState*, const uint8_t *pc, AbcOpcode opcode, MethodInfo* m, uintptr_t disp_id, uint32_t argc, Traits *)
    {
        (void)m;
        switch (opcode) {
        case OP_callproperty:
        case OP_callproplex:
        case OP_callpropvoid:
            AvmAssert(m->declaringTraits()->isInterface());
            // disp_id=m->iid(), opd2=argc
            emitOp2(pc, wordCode(opcode));
            break;
        case OP_callmethod:
            emitOp2(wordCode(opcode), uint32_t(disp_id+1), argc);
            break;
        default:
            AvmAssert(false);
            break;
        }
    }

    void WordcodeEmitter::writeOp2(const FrameState*, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits*)
    {
        (void)pc;
        switch (opcode) {
        case OP_setslot:
            emitOp1(WOP_setslot, opd1+1);
            break;
        case OP_callproperty:
        case OP_callproplex:
        case OP_callpropvoid:
        case OP_callstatic:
        case OP_callsuper:
        case OP_callsupervoid:
            emitOp2(wordCode(opcode), opd1, opd2);
            break;

        case OP_getproperty:
        case OP_setproperty:
        case OP_initproperty:
            emitOp1(pc, wordCode(opcode));
            break;

        case OP_constructprop:
        case OP_hasnext2:
            emitOp2(pc, wordCode(opcode));
            break;

        case OP_getsuper:
        case OP_setsuper:
        case OP_constructsuper:
            emitOp1(pc, wordCode(opcode));
            break;

        default:
            AvmAssert (false);
            break;
        }
    }

    void WordcodeEmitter::write(const FrameState*, const uint8_t* pc, AbcOpcode opcode, Traits*)
    {
      //AvmLog("WordcodeEmitter::write %x\n", opcode);
        switch (opcode) {
        case OP_coerce_a:
        case OP_nop:
        case OP_bkpt:
        case OP_bkptline:
        case OP_timestamp:
            // do nothing, all values on stack are atoms
            break;
        case OP_label:
            // do nothing, we generate implicit and explicit labels in writeBlockStart()
            break;
        case OP_pushfalse:
        case OP_pushtrue:
        case OP_pushundefined:
        case OP_pushnull:
        case OP_pushnan:
        case OP_checkfilter:
        case OP_astypelate:
        case OP_coerce_b:
        case OP_convert_b:
        case OP_coerce_i:
        case OP_convert_i:
        case OP_coerce_u:
        case OP_convert_u:
        case OP_coerce_d:
        case OP_convert_d:
        case OP_coerce_s:
        case OP_coerce_o:
        case OP_convert_o:
#ifdef VMCFG_FLOAT
        case OP_convert_f:
        case OP_convert_f4:
#endif
        case OP_istypelate:
        case OP_newactivation:
        case OP_popscope:
        case OP_pop:
        case OP_dup:
        case OP_swap:
        case OP_lessthan:
        case OP_greaterthan:
        case OP_lessequals:
        case OP_greaterequals:
        case OP_equals:
        case OP_strictequals:
        case OP_instanceof:
        case OP_in:
        case OP_not:
        case OP_add:
        case OP_modulo:
        case OP_subtract:
        case OP_divide:
        case OP_multiply:
        case OP_increment_i:
        case OP_decrement_i:
        case OP_increment:
        case OP_decrement:
        case OP_add_i:
        case OP_subtract_i:
        case OP_multiply_i:
        case OP_negate:
        case OP_negate_i:
#ifdef VMCFG_FLOAT
        case OP_unplus:
#endif
        case OP_bitand:
        case OP_bitor:
        case OP_bitxor:
        case OP_bitnot:
        case OP_lshift:
        case OP_rshift:
        case OP_urshift:
        case OP_typeof:
        case OP_nextvalue:
        case OP_nextname:
        case OP_hasnext:
        case OP_sxi1:
        case OP_sxi8:
        case OP_sxi16:
        case OP_li8:
        case OP_li16:
        case OP_li32:
        case OP_lf32:
        case OP_lf64:
#ifdef VMCFG_FLOAT
        case OP_lf32x4:
#endif
        case OP_si8:
        case OP_si16:
        case OP_si32:
        case OP_sf32:
        case OP_sf64:
#ifdef VMCFG_FLOAT
        case OP_sf32x4:
#endif
        case OP_getglobalscope:
        case OP_convert_s:
        case OP_esc_xelem:
        case OP_esc_xattr:
        case OP_lix8:
        case OP_lix16:
            emitOp0(pc, wordCode(opcode));
            break;
        case OP_throw:
        case OP_returnvalue:
        case OP_returnvoid:
#if defined DEBUGGER
            if (core->debugger()) emitOp0(pc, WOP_debugexit);
#endif
            emitOp0(pc, wordCode(opcode));
            break;

        case OP_pushstring:
        case OP_pushdouble:
        case OP_pushnamespace:
#ifdef VMCFG_FLOAT
        case OP_pushfloat:
        case OP_pushfloat4:
#endif
        case OP_getlocal:
        case OP_setlocal:
        case OP_inclocal:
        case OP_declocal:
        case OP_inclocal_i:
        case OP_declocal_i:
        case OP_dxns:
        case OP_finddef:
        case OP_getdescendants:
        case OP_deleteproperty:
        case OP_astype:
        case OP_coerce:
        case OP_istype:
        case OP_applytype:
        case OP_newobject:
        case OP_newarray:
        case OP_newcatch:
        case OP_getslot:
        case OP_setslot:
            emitOp1(pc, wordCode(opcode));
            break;
        case OP_hasnext2:
            emitOp2(pc, wordCode(opcode));
            break;
        case OP_pushshort:
            emitPushshort(pc);
            break;
        case OP_pushbyte:
            emitPushbyte(pc);
            break;
        case OP_getlocal0:
        case OP_getlocal1:
        case OP_getlocal2:
        case OP_getlocal3:
#ifdef VMCFG_WORDCODE_PEEPHOLE
            emitOp1(WOP_getlocal, opcode-OP_getlocal0);
#else
            emitOp0(pc, wordCode(opcode));
#endif
            break;
        case OP_setlocal0:
        case OP_setlocal1:
        case OP_setlocal2:
        case OP_setlocal3:
#ifdef VMCFG_WORDCODE_PEEPHOLE
            emitOp1(WOP_setlocal, opcode-OP_setlocal0);
#else
            emitOp0(pc, wordCode(opcode));
#endif
            break;
        case OP_pushint:
            emitPushint(pc);
            break;
        case OP_pushuint:
            emitPushuint(pc);
            break;
        case OP_iflt:
        case OP_ifle:
        case OP_ifnlt:
        case OP_ifnle:
        case OP_ifgt:
        case OP_ifge:
        case OP_ifngt:
        case OP_ifnge:
        case OP_ifeq:
        case OP_ifstricteq:
        case OP_ifne:
        case OP_ifstrictne:
        case OP_iftrue:
        case OP_iffalse:
        case OP_jump:
            emitRelativeJump(pc, wordCode(opcode));
            break;
        case OP_lookupswitch:
            emitLookupswitch(pc);
            break;
        case OP_debugfile:
            #ifdef DEBUGGER
            if (core->debugger()) emitOp1(pc, WOP_debugfile);
            #endif
            break;
        case OP_debug:
            #ifdef DEBUGGER
            if (core->debugger()) emitDebug(pc);
            #endif
            break;
        case OP_debugline:
            #ifdef DEBUGGER
            if (core->debugger()) emitOp1(pc, WOP_debugline);
            #endif
            break;
        case OP_dxnslate:
            emitOp0(pc, WOP_dxnslate);
            break;
        case OP_kill:
            // We used to remove this but it has side effects, so we can't
            emitOp1(pc, WOP_kill);
            break;
        default:
            // FIXME need error handler here
            break;
        }

    }

    void WordcodeEmitter::writeCoerce(const FrameState*, uint32_t, Traits*)
    {}

    void WordcodeEmitter::writeCoerceToNumeric(const FrameState*, uint32_t)
    {
#ifdef VMCFG_FLOAT
        emitOp0(NULL, WOP_unplus);      // NULL is dodgy but happens to work
#else
        emitOp0(NULL, WOP_convert_d);   // NULL is dodgy but happens to work
#endif
    }

    void WordcodeEmitter::writeCoerceToFloat4(const FrameState*, uint32_t)
    {
#ifdef VMCFG_FLOAT
        emitOp0(NULL, WOP_float4);      // NULL is dodgy but happens to work
#else
        AvmAssert(!"Should not happen");
#endif
    }

    void WordcodeEmitter::emitOp1(const uint8_t *pc, WordOpcode opcode)
    {
#ifdef _DEBUG
        AvmAssert(wopAttrs[opcode].width == 2);
#endif // _DEBUG
        CHECK(2);
        pc++;
        *dest++ = NEW_OPCODE(opcode);
        *dest++ = (intptr_t)(int32_t)AvmCore::readU32(pc);
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(opcode, dest-2);
#endif
    }

    // These take one U30 argument, and the argument is explicitly passed here (result of optimization)
    void WordcodeEmitter::emitOp1(WordOpcode opcode, uint32_t operand)
    {
#ifdef _DEBUG
        AvmAssert(wopAttrs[opcode].width == 2);
#endif // _DEBUG
        CHECK(2);
        *dest++ = NEW_OPCODE(opcode);
        *dest++ = (intptr_t)(int32_t)operand;
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(opcode, dest-2);
#endif
    }

    // These take two U30 arguments
    void WordcodeEmitter::emitOp2(const uint8_t *pc, WordOpcode opcode)
    {
#ifdef _DEBUG
        AvmAssert(wopAttrs[opcode].width == 3);
#endif
        CHECK(3);
        pc++;
        *dest++ = NEW_OPCODE(opcode);
        *dest++ = (intptr_t)(int32_t)AvmCore::readU32(pc);
        *dest++ = (intptr_t)(int32_t)AvmCore::readU32(pc);
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(opcode, dest-3);
#endif
    }

    void WordcodeEmitter::emitOp2(WordOpcode opcode, uint32_t op1, uint32_t op2)
    {
#ifdef _DEBUG
        AvmAssert(wopAttrs[opcode].width == 3);
#endif
        CHECK(3);
        *dest++ = NEW_OPCODE(opcode);
        *dest++ = (intptr_t)(int32_t)op1;
        *dest++ = (intptr_t)(int32_t)op2;
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(opcode, dest-3);
#endif
    }


    // These take one S24 argument that is PC-relative.  If the offset is negative
    // then the target must be a LABEL instruction, and we can just look it up.
    // Otherwise, we enter the target offset into an ordered list with the current
    // transformed PC and the location to backpatch.
    void WordcodeEmitter::emitRelativeJump(const uint8_t *pc, WordOpcode opcode)
    {
#ifdef _DEBUG
        AvmAssert(wopAttrs[opcode].jumps);
#endif
        CHECK(2);
        pc++;
        intptr_t offset = (intptr_t)AvmCore::readS24(pc);
        pc += 3;
        *dest++ = NEW_OPCODE(opcode);
        uintptr_t base_offset = uintptr_t(buffer_offset + (dest - buffers->data) + 1);
        emitRelativeOffset(base_offset, pc, offset);
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(opcode, dest-2);
        AvmAssert(state == 0);      // Never allow a jump instruction to be in the middle of a match
#endif
    }

    void WordcodeEmitter::emitLabel(const uint8_t *pc)
    {
#ifdef VMCFG_WORDCODE_PEEPHOLE
        // Do not optimize across control control flow targets, so flush the peephole window here.
        peepFlush();
#endif
        label_info* l = new label_info;
        l->old_offset = uint32_t(pc - code_start);
        l->new_offset = uint32_t(buffer_offset + (dest - buffers->data));
        l->next = labels;
        labels = l;
    }

#ifdef DEBUGGER
    void WordcodeEmitter::emitDebug(const uint8_t *pc)
    {
        CHECK(5);
        pc++;
        uint8_t debug_type = *pc++;
        uint32_t index = AvmCore::readU32(pc);
        uint8_t reg = *pc++;
        uint32_t extra = AvmCore::readU32(pc);
        // 4 separate operands to match the value in the operand count table,
        // though obviously we could pack debug_type and reg into one word and
        // we could also omit extra.
        *dest++ = NEW_OPCODE(OP_debug);
        *dest++ = debug_type;
        *dest++ = (intptr_t)(int32_t)index;
        *dest++ = (intptr_t)(int32_t)reg;
        *dest++ = (intptr_t)(int32_t)extra;
    }
#endif

    void WordcodeEmitter::emitPushbyte(const uint8_t *pc)
    {
        CHECK(2);
        pc++;
        *dest++ = NEW_OPCODE(WOP_pushbits);
        *dest++ = (intptr_t)(((int8_t)*pc++) << 3) | kIntptrType;
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(WOP_pushbits, dest-2);
#endif
    }

    void WordcodeEmitter::emitPushshort(const uint8_t *pc)
    {
        CHECK(2);
        pc++;
        *dest++ = NEW_OPCODE(WOP_pushbits);
        *dest++ = (intptr_t)((int16_t)AvmCore::readU32(pc) << 3) | kIntptrType;
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(WOP_pushbits, dest-2);
#endif
    }

    void WordcodeEmitter::emitGetscopeobject(const uint8_t *pc)
    {
        CHECK(2);
        pc++;
        *dest++ = NEW_OPCODE(OP_getscopeobject);
        *dest++ = *pc++;
#ifdef VMCFG_WORDCODE_PEEPHOLE
        peep(OP_getscopeobject, dest-2);
#endif
    }

    void WordcodeEmitter::emitPushint(const uint8_t *pc)
    {
        // FIXME: wrong for 64-bit, we want 32 bits of payload
        pc++;
        int32_t value = pool->cpool_int[AvmCore::readU32(pc)];
        if (atomIsValidIntptrValue(value)) {
            CHECK(2);
            *dest++ = NEW_OPCODE(WOP_pushbits);
            *dest++ = (intptr_t(value) << 3) | kIntptrType;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            peep(WOP_pushbits, dest-2);
#endif
        }
        else {
            double_overlay v((double)value);
            CHECK(3);
            *dest++ = NEW_OPCODE(WOP_push_doublebits);
            *dest++ = v.bits32[0];
            *dest++ = v.bits32[1];
#ifdef VMCFG_WORDCODE_PEEPHOLE
            peep(WOP_push_doublebits, dest-3);
#endif
        }
    }

    void WordcodeEmitter::emitPushuint(const uint8_t *pc)
    {
        // FIXME: wrong for 64-bit, we want 32 bits of payload
        pc++;
        uint32_t value = pool->cpool_uint[AvmCore::readU32(pc)];
        if (atomIsValidIntptrValue_u(value)) {
            CHECK(2);
            *dest++ = NEW_OPCODE(WOP_pushbits);
            *dest++ = (intptr_t(value) << 3) | kIntptrType;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            peep(WOP_pushbits, dest-2);
#endif
        }
        else {
            double_overlay d((double)value);
            CHECK(3);
            *dest++ = NEW_OPCODE(WOP_push_doublebits);
            *dest++ = d.bits32[0];
            *dest++ = d.bits32[1];
#ifdef VMCFG_WORDCODE_PEEPHOLE
            peep(WOP_push_doublebits, dest-3);
#endif
        }
    }

    void WordcodeEmitter::emitLookupswitch(const uint8_t *pc)
    {
#ifdef VMCFG_WORDCODE_PEEPHOLE
        // Avoid a lot of hair by flushing before LOOKUPSWITCH and not peepholing after.
        peepFlush();
#endif
        const uint8_t* base_pc = pc;
        pc++;
        uint32_t base_offset = uint32_t(buffer_offset + (dest - buffers->data));
        intptr_t default_offset = AvmCore::readS24(pc);
        pc += 3;
        uint32_t case_count = AvmCore::readU32(pc);
        CHECK(3);
        *dest++ = NEW_OPCODE(OP_lookupswitch);
        emitRelativeOffset(base_offset, base_pc, default_offset);
        *dest++ = case_count;

        for ( uint32_t i=0 ; i <= case_count ; i++ ) {
            intptr_t offset = AvmCore::readS24(pc);
            pc += 3;
            CHECK(1);
            emitRelativeOffset(base_offset, base_pc, offset);
        }
#ifdef VMCFG_WORDCODE_PEEPHOLE
        // need a forward declaration for toplevel.
//      AvmAssert(toplevel[OP_lookupswitch] == 0);
#endif
    }

    uint32_t WordcodeEmitter::epilogue(uintptr_t** code_result)
    {
        AvmAssert(backpatches == NULL);
        AvmAssert(exception_fixes == NULL);

#ifdef VMCFG_WORDCODE_PEEPHOLE
        peepFlush();
#endif

        buffers->entries_used = uint32_t(dest - buffers->data);
        uint32_t total_size = buffer_offset + buffers->entries_used;

        TranslatedCode* code_anchor = TranslatedCode::New(core->GetGC(), total_size);
        uintptr_t* code = code_anchor->AsArray();

        // reverse the list of buffers
        buffer_info* first = buffers;
        buffer_info* next = first->next;
        first->next = NULL;
        while (next != NULL) {
            buffer_info* tmp = next->next;
            next->next = first;
            first = next;
            next = tmp;
        }
        buffers = first;

        // move the data
        uintptr_t* ptr = code;
        while (first != NULL) {
            VMPI_memcpy(ptr, first->data, first->entries_used*sizeof(uintptr_t));
            ptr += first->entries_used;
            first = first->next;
        }
        AvmAssert(ptr == code + total_size);

        if (info != NULL) {
            info->set_word_code(core->GetGC(), code_anchor);
#ifdef SUPERWORD_PROFILING
            WordcodeTranslator::swprofCode(code, code + total_size);
#endif
        }

        cleanup();
        if (code_result != NULL)
            *code_result = code;
        return total_size;
    }

#ifdef VMCFG_WORDCODE_PEEPHOLE

    // Peephole optimization.
    //
    //
    // DESCRIPTION.  This is a deterministic state machine driven peephole optimizer.  The
    // machine attempts to match a set of patterns against a prefix of the instruction
    // stream, replacing matched patterns by new strings of instructions (usually shorter
    // or faster ones) -- a "reduction".  A reduction is possible when the machine enters
    // a final state.  However, the machine is greedy and may leave the final state
    // looking for a longer match.  As the longer match may fail, the machine maintains
    // a stack of final states it may backtrack to.  A match may fail in two ways, either
    // because a state is reached from which there is no move to a final state on the
    // actual input, or if a final state is reached but the guard condition for the
    // state is not satisfied.  The guard is only tested when the machine is ready to
    // commit; for that reason, a stack of backtrack states is required (instead of a
    // single backtrack state).  The guard is mixed in with the commit code in order to
    // keep code size down, though it probably does not matter much.
    //
    // Instructions inserted by reductions are not subject to repeated matching (though
    // they could be).
    //
    // In order to handle patterns that are subpatterns but not prefixes of other patterns,
    // a special failure transition is encoded in each state and used if the matcher fails
    // at that state.
    //
    // The peephole optimizer function peep() /must/ be called every time an instruction
    // has been emitted to the instruction stream, as the state machine in the peephole
    // optimizer tracks the emitted instruction stream (it does not inspect it repeatedly).
    // The operands to peep() are the symbolic opcode that was just emitted and the address
    // at which that opcode was emitted.  The function peep() drives the state machine.
    //
    // If optimization must not cross some instruction boundary (for example it must not
    // merge the instruction at a control flow target with the previous instruction)
    // then peepFlush() must be called before instructions are emitted for the point beyond
    // the boundary.  This must also be called at the end of the instruction stream, and for
    // instructions that must not appear in the peephole window (currently only LOOKUPSWITCH).
    //
    // The tables 'states', 'transitions', and 'toplevel' are generated by the program
    // utils/peephole.as based on the patterns described in core/peephole.tbl, which are
    // in turn hand-selected with aid of the dynamic instruction profiling infrastructure
    // built into Tamarin - see comments in utils/superwordprof.c for help on how to use that.
    //
    // It is possible to optimize the entry to peep(), the in-line test is
    //
    //     state==0 && toplevel[toplevel_index] == 0
    //
    // where toplevel_index is computed from the opcode, maybe worth simplifying that in
    // order to make this test faster.  Anyhow, if the test is true then peep() need
    // not be called as there will not be a state transition.  This factoid may be useful
    // if emitOp0, emitOp1, and emitOp2 are in-lined into the verifier.
    //
    //
    // OPTIMALITY.  We wish to establish that if there is a stream of instructions s1, s2, ...
    // and patterns p1, p2, ... then (a) if some p matches a prefix of the stream then
    // the algorithm finds the match; (b) if several p match a prefix of the stream then
    // (one of) the longest p is chosen, subject to guard evaluation; (c) the
    // properties (a) and (b) hold for the remainder of the stream when the instructions
    // matching the pattern are removed from the stream; and (d) the properties (a) and (b)
    // hold for the remainder of the stream if no patterns match the prefix and the first
    // instruction is removed from the stream.
    //
    // Note that the optimality result - if established - does not imply that the "best"
    // match (the one that produces the fewest instructions, or the one that minimizes
    // stack pointer updates, or maximizes intra-instruction optimization in the C compiler,
    // or is best according to any criteria like that) is found, since a best match can
    // sometimes be obtained by choosing a shorter match against a prefix in order
    // to make a longer match possible subsequently.  It only implies that we do as well
    // as we can with a greedy matching strategy.
    //
    // Sketch of proof of optimality:
    //
    // To establish (a) and (b), note that the automaton tracks the prefix of the instruction
    // stream and stacks all accepting states.  If it reaches a state without a successor
    // (whether it's successor-less or the input has no transition from the current state)
    // then the stacked states are tried in longest-first (reverse) order, and the longest
    // match wins.  (The failure state is not visited if there are accepting stacked states,
    // and does not interfere.)
    //
    // To establish (c), note that the automaton consumes exactly the instructions that
    // were matched, then sets state=0 and re-emits the non-matched instructions, running
    // the automaton by calling peep() for each non-matched instruction.  If this succeeds
    // then (a) and (b) apply; if it fails, (d) applies.  Either way the automaton
    // eventually consumes all of the originally non-matching instructions, after which it
    // is once again driven by instructions pushed into the peephole window.
    //
    // To establish (d), note that the automaton makes a failure transition if no match
    // could be made, and that the failure transition by construction (see utils/peephole.as)
    // discards the fewest possible instructions from the prefix of the stream: those that
    // will not match any pattern, given that the present pattern failed.  Thus matching
    // will start at the earlies possible point, and (a), (b), (c), or (d) apply.
    //
    // Then, observe that peepFlush() commits to the longest match possible at the time
    // it is called, and then skips the remaining instructions.  By definition, the
    // skipped instructions cannot be matched (or they would have been part of the "longest
    // match possible").  Thus peepFlush() preserve (a) and (b).
    //
    // Finally, observe that peepFlush() is called following the last instruction emitted
    // in order to commit to the longest prefix of the instructions still in the peephole
    // window.

    void WordcodeEmitter::peepInit()
    {
        state = 0;
    }

    // Replace old instructions with new words of code.  This is tail called from the
    // generated commit() function (see peephole.cpp).
    //
    // Invariants here:
    //
    //   - Lookupswitch never appears in the peephole window (reduces complexity
    //     and guarantees we won't ever have more than one buffer boundary crossing)
    //
    //   - Relative branch instructions only ever appear as the last instruction in
    //     the window.  At that point, if it is a forward branch, then the backpatch
    //     may not be the first backpatch in the list, but it will usually be near
    //     the beginning (most branches are short).  Backpatches are uniquely
    //     identified by the patch location they point to so it's always safe to
    //     remove one if we're squashing a branch instruction.
    //
    //     That means that if the peephole optimizer processes a branch instruction
    //     then it /must/ reduce at that point, it can't wait until the next
    //     instruction even if the current state is a final state.
    //
    //   - If the optimizer inserts a branch then the address in the branch must
    //     be absolute.  If the branch is backward it must be the negative of the
    //     absolute word offset of the target.  If the branch is forward it must
    //     be the positive absolute ABC uint8_t offset of the branch target; a backpatch
    //     structure will be created in the latter case.

    bool WordcodeEmitter::replace(uint32_t old_instr, uint32_t new_words, bool jump_has_been_translated)
    {
        // Undo any relative offsets in the last instruction, if that wasn't done by
        // the commit code.

        if (isJumpInstruction(O[nextI - 1]) && !jump_has_been_translated)
            undoRelativeOffsetInJump();

        // Catenate unconsumed instructions onto R (it's easier than struggling with
        // moving instructions across buffer boundaries)

        uint32_t k = new_words;
        for ( uint32_t n=old_instr ; n < nextI ; n++ ) {
            uint32_t len = calculateInstructionWidth(O[n]);
            S[k] = O[n];
            for ( uint32_t j=0 ; j < len ; j++ )
                R[k++] = I[n][j];
        }

        // Unlink the last buffer segment if we took everything from it, push it onto
        // a reserve (there can only ever be one free).  We know I[nextI-1] points into the
        // current buffer, so check if I[0] is between the start of the buffer and
        // the last instruction.

        if (!(buffers->data <= I[0] && I[0] <= I[nextI-1])) {
            spare_buffer = buffers;
            buffers = buffers->next;
            spare_buffer->next = NULL;
            dest_limit = buffers->data + sizeof(buffers->data)/sizeof(buffers->data[0]);
            buffer_offset -= buffers->entries_used;
        }
        dest = I[0];

        // Emit the various instructions from new_data, handling branches specially.
        //
        // At this point the instance variables state, I, O, nextI, backtrack_stack,
        // and backtrack_idx are dead, and all the data we need for emitting the
        // instructions are in S and R.  In addition, dest has been rolled back and
        // points to the address of the first instruction in the peephole window, and
        // nothing is live in the code buffer beyond that point.  It's as if we are
        // in a context where we're just emitting instructions.
        //
        // Consequently, we set state to 0 and start emitting instructions from S/R
        // normally, calling peep() after each instruction that was not replaced by
        // the current action.  This works without having local copies of S and R
        // because peephole optimization cannot insert a replacement sequence that is
        // longer than the matched sequence; so the segments of S and R used by any
        // recursive match will not affect what we're doing here.  Furthermore, 'dest'
        // is shared between this match and recursive matches, so if a recursive match
        // shortens the instruction sequence the correct value of dest will be used
        // when we get back to the present invocation of replace().

        // Reset the machine.

        state = 0;

        uint32_t i=0;
        while (i < k) {
            uintptr_t op = S[i];
            uintptr_t width = calculateInstructionWidth(op);
            CHECK(width);
            if (isJumpInstruction(op)) {
                *dest++ = R[i++];
                int32_t offset = int32_t(R[i++]);
                if (offset >= 0) {
                    // Forward jump
                    // Install a new backpatch structure
                    makeAndInsertBackpatch(code_start + offset, uint32_t(buffer_offset + (dest + (width - 1) - buffers->data)));
                }
                else {
                    // Backward jump
                    // Compute new jump offset
                    *dest = -int32_t(buffer_offset + (dest + (width - 1) - buffers->data) + offset);
                    dest++;
                }
                if (width >= 3)
                    *dest++ = R[i++];
                if (width >= 4)
                    *dest++ = R[i++];
                AvmAssert(width <= 4);
            }
            else {
                switch (width) {
                    default:
                        AvmAssert(!"Can't happen");
                    case 1:
                        *dest++ = R[i++];
                        break;
                    case 2:
                        *dest++ = R[i++];
                        *dest++ = R[i++];
                        break;
                    case 3:
                        *dest++ = R[i++];
                        *dest++ = R[i++];
                        *dest++ = R[i++];
                        break;
                    case 5:  // OP_debug
                        *dest++ = R[i++];
                        *dest++ = R[i++];
                        *dest++ = R[i++];
                        *dest++ = R[i++];
                        *dest++ = R[i++];
                        break;
                }
            }
            if (i-width >= new_words)
                peep((uint32_t)op, dest-width);
        }

        return true;  // always
    }

    void WordcodeEmitter::undoRelativeOffsetInJump()
    {
        AvmAssert(isJumpInstruction(O[nextI - 1]));
        AvmAssert(I[nextI - 1] + 2 == dest);

        uintptr_t offset = I[nextI - 1][1];
        if (offset == 0x80000000U) {
            // Forward branch, must find and nuke the backpatch
            backpatch_info *b = backpatches;
            backpatch_info *b2 = NULL;
            while (b != NULL && b->patch_loc != &I[nextI - 1][1])
                b2 = b, b = b->next;
            AvmAssert(b != NULL);
            if (b2 == NULL)
                backpatches = b->next;
            else
                b2->next = b->next;
            // b is unlinked
            // Install the ABC uint8_t offset from the backpatch structure (will be positive)
            I[nextI - 1][1] = uint32_t(b->target_pc - code_start);
            delete b;
        }
        else {
            // Backward branch
            AvmAssert((int32_t)I[nextI - 1][1] < 0);
            // Install the negative of the absolute word offset of the target
            I[nextI - 1][1] = -int32_t(buffer_offset + (dest - buffers->data) + (int32_t)I[nextI - 1][1]);
        }
    }

    void WordcodeEmitter::peep(uint32_t opcode, uintptr_t* loc)
    {
        const peep_state_t *s;
        uint32_t limit, next_state;

        AvmAssert(opcode != OP_lookupswitch);

        if (state == 0)
            goto initial_state;

        if (opcode == 0) {
            s = &states[state];
            goto accept;
        }

        // Search for a transition from the current state to a next
        // state on input 'opcode'.

        O[nextI] = opcode;
        I[nextI] = loc;
        nextI++;
        s = &states[state];
        limit = s->numTransitions;

        // The transition lists can get quite long for popular instructions like GETLOCAL;
        // binary search if it that might be profitable.

        if (limit > 4) {
            int32_t lo = s->transitionPtr;
            int32_t hi = lo + limit - 1;
            while (lo <= hi) {
                uint32_t mid = (unsigned)(lo + hi) / 2;
                uint32_t probe = transitions[mid].opcode;
                if (probe == opcode) {
                    next_state = transitions[mid].next_state;
                    goto found;
                }
                if (opcode < probe)
                    hi = mid-1;
                else
                    lo = mid+1;
            }
            next_state = 0;
        }
        else {
            const peep_transition_t* t = &transitions[s->transitionPtr];
            uint32_t i = 0;
            while (i < limit && t->opcode != opcode)
                i++, t++;

            next_state = (i == limit) ? 0 : t->next_state;
        }
    found:

        if (next_state != 0) {

            // Advance
            //
            // There is a next state, so push the current state on the backtrack
            // stack if it is final, and move to the next state.  If that state has
            // successor states then return, as the search continues.  Otherwise, the
            // next state must be final and we try to accept.
            //
            // (The shortcut of checking the successors is necessary for correctness,
            // as otherwise the peephole window could contain a branch in the non-final
            // position.)

            if (s->guardAndAction != 0)
                backtrack_stack[backtrack_idx++] = state;

        advance:
            state = next_state;
            s = &states[state];
            if (s->numTransitions > 0)
                return;

            next_state = 0;
            AvmAssert(s->guardAndAction != 0);
        }

        // Accept
        //
        // The next state is 0.  Commit to 'state' if it is final; otherwise to
        // successive backtrack states.  Committing means checking the guard
        // (which may fail, forcing further backtracking) and if the guard passes
        // then performing the transformation.  The commit function is generated,
        // see above; the replace logic is in the function replace() above.

    accept:
        if (s->guardAndAction && commit(s->guardAndAction))
            return;

        for ( int bi=backtrack_idx-1 ; bi >= 0 ; bi-- ) {
            const peep_state_t *b = &states[backtrack_stack[bi]];
            AvmAssert(b->guardAndAction != 0);
            if (commit(b->guardAndAction))
                return;
        }

        // If we could not accept or backtrack because of failing guards then
        // try the failure state, if defined.  We discard anything not relevant
        // to the failure state by shifting the window, so relevant instructions
        // always begin in offset 0 of the window.

        if (s->fail != 0) {
            shiftBuffers(s->failShift);
            next_state = s->fail;
            goto advance;
        }

        // If we failed to find an accepting state then fall through to initial_state
        // to reset the machine.  Resetting discards the first instruction only,
        // other cases - where larger shifts are possible - are handled above, because
        // in that case s->fail will be nonzero.
        //
        // After shifting, rerun the optimizer on the input buffer, since there may
        // be optimization opportunities there.

        shiftBuffers(1);
        if (nextI > 0) {
            replace(0, 0);
            peepFlush();
        }
        return;

    initial_state:
        AvmAssert(opcode < WOP_LAST+1);

        state = toplevel[opcode];  // may remain 0
        nextI = 0;
        backtrack_idx = 0;
        if (state != 0) {
            O[nextI] = opcode;
            I[nextI] = loc;
            nextI++;
        }
    }

    void WordcodeEmitter::shiftBuffers(uint32_t shift)
    {
        for ( uintptr_t i=0, limit=nextI-shift ; i < limit ; i++ ) {
            I[i] = I[i+shift];
            O[i] = O[i+shift];
        }
        nextI -= shift;
        backtrack_idx = 0;  // We could do better if we knew how many final states to discard
    }

    void WordcodeEmitter::peepFlush()
    {
        peep(0, NULL);      // commits, but may start another match
        state = 0;          // ignore any partial match
    }
#endif  // VMCFG_WORDCODE_PEEPHOLE

}
#endif // VMCFG_WORDCODE
