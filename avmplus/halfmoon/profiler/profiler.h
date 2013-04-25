/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// This is the public header file for the profiler module
//

#ifndef PROFILER_H_
#define PROFILER_H_

#ifndef assert
#  include <assert.h>
#endif

#include "avmplus.h"

#ifdef VMCFG_HALFMOON
#include "CodegenLIR.h"

namespace profiler {
using avmplus::CodegenLIR;
using avmplus::CodegenDriver;
using avmplus::MethodEnv;
using avmplus::MethodInfo;
using avmplus::Toplevel;
using avmplus::MethodSignaturep;
using avmplus::ActionBlockConstants::AbcOpcode;
using avmplus::Traits;
using avmplus::FrameState;
using avmplus::FrameValue;
using avmplus::CodegenLabel;

using nanojit::LIns;
using nanojit::LOpcode;
using nanojit::CallInfo;

// Forward declarations
class MethodProfile;

// Fascade api
void finish(MethodProfile*);
bool hasBailedOut(MethodProfile*);

class ProfileLirEmitter : public CodegenLIR {
public:
  ProfileLirEmitter(MethodInfo* info, MethodSignaturep ms, Toplevel* toplevel);
  ~ProfileLirEmitter();

  // CodeWriter methods
  void write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type);
  void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits* type);
  void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type);
  void writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo* methodInfo, uintptr_t disp_id, uint32_t argc, Traits* type);
  void writeNip(const FrameState* state, const uint8_t *pc, uint32_t count);
  void writeCheckNull(const FrameState* state, uint32_t index);
  void writeCoerce(const FrameState* state, uint32_t index, Traits *type);
  void writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver* driver);
  void writeEpilogue(const FrameState* state);
  void writeBlockStart(const FrameState* state);
  void writeOpcodeVerified(const FrameState* state, const uint8_t* pc, AbcOpcode opcode);
  void writeFixExceptionsAndLabels(const FrameState* state, const uint8_t* pc);
  void cleanup();

  // Overwritten CodegenLIR methods
  virtual void branchToAbcPos(LOpcode op, LIns *cond, const uint8_t* target);
  virtual void emitIf(AbcOpcode opcode, const uint8_t* target, int lhs, int rhs);
  virtual bool isProfilerLirEmitter() { return true; }
  virtual void emitRecordCallTypes(MethodSignaturep method_signature, LIns* obj, Traits* object_type, int argc, LIns* apAddr, Traits* result, LIns* call_result, LIns * method);

private:
  void emitRecordMethodReceiverType();
  void emitRecordBinaryTypes(LIns* left_operand, LIns* right_operand, const FrameValue& left_value, const FrameValue& right_value, const uint8_t* pc, AbcOpcode opcode);
  void emitRecordBinaryTypes(LIns* left_operand, LIns* right_operand, int sp, const uint8_t* pc, AbcOpcode opcode, Traits *type);
  void emitRecordCallTypes();
  void emitRecordGetPropertyTypes();
  void initializeBranchMaps(int abc_pc, int true_target, int false_target);
  void boxArguments(LIns* args);
  bool shouldEmitMethodCounters();
  bool isBinaryOpcode(AbcOpcode opcode);
  bool isBranchOpcode(AbcOpcode opcode);
  bool isAnyType(const FrameValue& value);
  bool isAnyType(const Traits* traits);
  bool allArgumentsTyped(MethodSignaturep method_signature);

  int getVerboseAbcPc(const uint8_t* state_pc) {
    return int(state_pc - code_pos);
  }
};

// functions called by profiling JIT code
extern const CallInfo ci_incrementMethodInvokeCounter;

}
#endif // VMCFG_HALFMOON
#endif // PROFILER_H_
