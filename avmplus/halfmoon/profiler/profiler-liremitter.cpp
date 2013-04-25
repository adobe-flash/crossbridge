/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "../nanojit/nanojit.h"

#ifdef VMCFG_HALFMOON

#include "../halfmoon/hm-main.h"
#include "profiler-main.h"

using namespace nanojit;

/***
 * Macros defined for profiler_jit_calls, copied from CodegenLIR
 */
#include "profiler-jit_calls.h"

/***
 * The general idea is to have the verifier call ProfileLirEmitter as if
 * it was CodegenLIR. Instrument important opcodes with runtime profiling
 * code in this class, then call CodegenLIR methods to build the actual
 * functionality of the current AbcOpcode.
 */
namespace profiler {

using namespace avmplus;

ProfileLirEmitter::ProfileLirEmitter(MethodInfo* info, MethodSignaturep ms,
                                     Toplevel* toplevel)
: CodegenLIR(info, ms, toplevel, NULL) {
}

ProfileLirEmitter::~ProfileLirEmitter() {
  // Compiler should automatically call CodegenLIR's destructor
}

void ProfileLirEmitter::initializeBranchMaps(int abc_pc, int true_target, int false_target) {
  MethodProfile* profile = JitManager::getProfile(info);
  profile->initializeBranchCounters(abc_pc, true_target, false_target);
}

// catch all if statements
// There is some overlap between this and branchtoAbcPos
// However, CodegenLIR::emitIf sometimes optimizes the branch away, but we track
// it anyway because halfmoon doesn't optimize the branch away during
// abcbuilder time.
void ProfileLirEmitter::emitIf(AbcOpcode opcode, const uint8_t* target, int lhs, int rhs) {
  int fall_through_offset = 4; // ABC dictates fall through is current_pc + 4
  initializeBranchMaps(getVerboseAbcPc(state->abc_pc),
                       getVerboseAbcPc(target),
                       getVerboseAbcPc(state->abc_pc + fall_through_offset));
  CodegenLIR::emitIf(opcode, target, lhs, rhs);
}

/// This will catch jumps created because of ABC branch opcodes.
/// It will ignore jumps created due to CodegenLIR inlining
/// TODO: check for conditions where the opcode gets inversed in CodegenLIR
void ProfileLirEmitter::branchToAbcPos(LOpcode op, LIns *condition, const uint8_t* target) {
  LIns* current_pc = InsConst(getVerboseAbcPc(state->abc_pc));
  LIns* true_target_pc = InsConst(getVerboseAbcPc(target));

  int fall_through_offset = 4; // ABC dictates fall through is current_pc + 4
  LIns* fall_through_pc = InsConst(getVerboseAbcPc(state->abc_pc + fall_through_offset));

  /// CallIns can't have NULL arguments, but branches can.
  /// So call with a constant 0, but branch on the NULL
  LIns* recordCondition = condition;
  if (condition == NULL) {
    assert (op == LIR_j);
    recordCondition = InsConst(1); // Condition is true for OP_jump
    fall_through_pc = InsConst(0);  // Fall through PC is actually 0
  }

  initializeBranchMaps(getVerboseAbcPc(state->abc_pc),
                       getVerboseAbcPc(target),
                       getVerboseAbcPc(state->abc_pc + fall_through_offset));

  callIns(FUNCTIONID(updateBranchCounters), 6, env_param,
          current_pc, true_target_pc, fall_through_pc, recordCondition, InsConst(op));
  CodegenLIR::branchToAbcPos(op, condition, target);
}

void ProfileLirEmitter::emitRecordBinaryTypes(LIns* left_operand, LIns* right_operand, const FrameValue& left_value, const FrameValue& right_value, const uint8_t* pc, AbcOpcode opcode) {
  /***
   * We cannot record the output type of binary operations
   * because CodegenLIR uses the framestate types for various operations.
   * An opcode such as add stores the result value of a different type into a stack slot
   * eg add int + object = object. Stack slot in the framestate still says the type is an int
   * not an object, which messes up vartracker, localcopy, etc.
   */
  switch (opcode) {
    case OP_add:
    {
      LIns* left_atom = nativeToAtom(left_operand, left_value.traits);
      LIns* right_atom = nativeToAtom(right_operand, right_value.traits);
      callIns(FUNCTIONID(recordBinaryInputTypes), 4, env_param, left_atom, right_atom, InsConst(getVerboseAbcPc(pc)));
      break;
    }
    default:
      break;
  }
}

bool ProfileLirEmitter::isBranchOpcode(AbcOpcode opcode) {
  switch (opcode){
    case OP_iftrue:
    case OP_iffalse:
    case OP_iflt:
    case OP_ifnlt:
    case OP_ifle:
    case OP_ifnle:
    case OP_ifgt:
    case OP_ifngt:
    case OP_ifge:
    case OP_ifnge:
    case OP_ifeq:
    case OP_ifne:
    case OP_ifstricteq:
    case OP_ifstrictne:
      return true;
    default:
      return false;
  }

}

bool ProfileLirEmitter::isBinaryOpcode(AbcOpcode opcode) {
  switch (opcode) {
    case OP_modulo:
    case OP_subtract:
    case OP_divide:
    case OP_multiply:
    case OP_add:
    case OP_bitnot:
    case OP_lshift:
    case OP_rshift:
    case OP_bitand:
    case OP_bitor:
    case OP_bitxor:
    case OP_increment:
    case OP_decrement:
    case OP_increment_i:
    case OP_decrement_i:
    case OP_add_i:
    case OP_subtract_i:
    case OP_multiply_i:
      return true;
    default:
      return false;
  }
}

bool ProfileLirEmitter::isAnyType(const Traits* traits) {
  return traits == NULL;
}

bool ProfileLirEmitter::isAnyType(const FrameValue& value) {
  return isAnyType(value.traits);
}

void ProfileLirEmitter::emitRecordCallTypes(MethodSignaturep callee_ms,
                                            LIns* obj, Traits* objectType, int argc, LIns* apAddr,
                                            Traits* result, LIns* call_result, LIns * method)
{
  int abc_pc = getVerboseAbcPc(this->state->abc_pc);
  if (objectType != NULL) {
    obj = nativeToAtom(obj, objectType);
  }

  if (!allArgumentsTyped(callee_ms)) {
    callIns(FUNCTIONID(recordCallInputTypes), 7, env_param, obj,
            InsConst(argc), apAddr, method, call_result, InsConst(abc_pc));
  }
  (void) result;
}

// CodeWriter methods
void ProfileLirEmitter::write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type) {
  this->state = state;
  int sp = state->sp();
  const uint8_t* nextpc = pc;
  unsigned int imm30=0, imm30b=0;
  int imm8=0, imm24=0;
  AvmCore::readOperands(nextpc, imm30, imm24, imm30b, imm8);

  /// If Either the left or right values are boxed, box both values and record the type
  /// We could choose to record only the untyped value, but then we'd have
  /// a combinatorial explosion of profiler-jit-methods for each case.
  /// We could also write a "isNotAtomType" and feed that instead of the actual value
  /// However, we don't have the type tag for that in the atom tag (only 3 bits).
  /// Finally, no damage is done if we record extra information because halfmoon
  /// Should always ask the verifier first, what type is the value. Only in the untyped case
  /// do we actually need to speculate.
  if (isBinaryOpcode(opcode)) {
    const FrameValue& left_value = state->value(sp);
    const FrameValue& right_value = state->value(sp - 1);

    if (isAnyType(left_value) || (isAnyType(right_value))) {
      LIns* leftOperand = localCopy(sp);
      LIns* rightOperand = localCopy(sp - 1);
      CodegenLIR::write(state, pc, opcode, type);
      emitRecordBinaryTypes(leftOperand, rightOperand, left_value, right_value, pc, opcode);
      return;
    }
  } else if (opcode == OP_lookupswitch) {
    int case_count = imm30b + 1;  // +1 for default case
    uintptr_t default_case = uintptr_t(pc + imm24);
    // Don't do anything for switch cases now, but this is where we'd hook
    // into it.
    (void) case_count;
    (void) default_case;
  }

  CodegenLIR::write(state, pc, opcode, type);
}

/// TODO: branch counters for lookup switch
void ProfileLirEmitter::writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits* type) {
  CodegenLIR::writeOp1(state, pc, opcode, opd1, type);
}

void ProfileLirEmitter::writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type) {
  CodegenLIR::writeOp2(state, pc, opcode, opd1, opd2, type);
}

void ProfileLirEmitter::writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo* methodInfo, uintptr_t disp_id, uint32_t argc, Traits* type) {
  CodegenLIR::writeMethodCall(state, pc, opcode, methodInfo, disp_id, argc, type);
}

void ProfileLirEmitter::writeNip(const FrameState* state, const uint8_t *pc, uint32_t count) {
  CodegenLIR::writeNip(state, pc, count);
}

void ProfileLirEmitter::writeCheckNull(const FrameState* state, uint32_t index) {
  CodegenLIR::writeCheckNull(state, index);
}

void ProfileLirEmitter::writeCoerce(const FrameState* state, uint32_t index, Traits *type) {
  CodegenLIR::writeCoerce(state, index, type);
}

bool ProfileLirEmitter::allArgumentsTyped(MethodSignaturep method_signature) {
  for (int i = 0; i <= method_signature->param_count(); i++) {
    if (isAnyType(method_signature->paramTraits(i))) {
      return false;
    }
  }

  return true;
}

void ProfileLirEmitter::boxArguments(LIns* args) {
  for (int i = 1; i < ms->param_count() + 1; i++) {
    // params are local vars starting from 1.
    // 0 is the this object.
    LIns* argument = localCopy(i);
    Traits* param_traits = ms->paramTraits(i);
    if (!isAnyType(param_traits)) {
      argument = nativeToAtom(argument, param_traits);
    } 

    lirout->insStore(LIR_stp, argument, args, i * sizeof(Atom), ACCSET_STORE_ANY);
  }
}

void ProfileLirEmitter::writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver* driver) {
  CodegenLIR::writePrologue(state, pc, driver);
  const int param_count = ms->param_count();
  const int optional_count = ms->optional_count();
  (void) optional_count;
  assert ((optional_count == 0) && "Don't support optional arguments yet");

  if (shouldEmitMethodCounters())
    callIns(FUNCTIONID(incrementMethodInvokeCounter), 1, env_param);

  if (!allArgumentsTyped(ms)) {
    LIns* args = insAlloc((param_count + 1) * sizeof(Atom));
    LIns* this_obj = lirout->insLoad(LIR_ldp, ap_param, 0, ACCSET_LOAD_ANY);
    //this_obj = addp(this_obj, kObjectType);
    lirout->insStore(LIR_stp, this_obj, args, 0, ACCSET_STORE_ANY);
    boxArguments(args);
    callIns(FUNCTIONID(recordArgumentTypes), 3, env_param,
            InsConst(param_count + 1), args);
  }
}

void ProfileLirEmitter::writeEpilogue(const FrameState* state) {
  CodegenLIR::writeEpilogue(state);
}

void ProfileLirEmitter::writeBlockStart(const FrameState* state) {
  CodegenLIR::writeBlockStart(state);
}

void ProfileLirEmitter::writeOpcodeVerified(const FrameState* state, const uint8_t* pc, AbcOpcode opcode) {
  CodegenLIR::writeOpcodeVerified(state, pc, opcode);
}

void ProfileLirEmitter::writeFixExceptionsAndLabels(const FrameState* state, const uint8_t* pc) {
  CodegenLIR::writeFixExceptionsAndLabels(state, pc);
}

void ProfileLirEmitter::cleanup() {
  CodegenLIR::cleanup();
}

bool ProfileLirEmitter::shouldEmitMethodCounters() {
  if (!halfmoon::isProfilerEnabled())
    return false;
  MethodProfile* methodProfile = JitManager::getProfile(info);
  return !info->isNative() &&    // Don't do native methods
    !info->pool()->isBuiltin &&  // or builtin.
    // Means the halfmoon jit failed
    // So just stick with LIR
    !hasBailedOut(methodProfile) &&
    ms->optional_count() == 0 && // Don't profile methods with optional args
    !info->hasExceptions();      // or exceptions.
}

}

#endif // VMCFG_HALFMOON
