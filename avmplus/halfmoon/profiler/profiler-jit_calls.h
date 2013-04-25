/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/***
 * The equivalent of jit-calls.h in CodegenLIR but specific
 * methods ONLY for runtime profiling.
 */

#ifdef VMCFG_HALFMOON

namespace profiler {

using namespace halfmoon;
using namespace nanojit;

void recordArgumentTypes(MethodEnv* method_env, int argc, Atom* args) {
  MethodProfile* method_profile = JitManager::getProfile(method_env);
  int abc_pc = 0; // Because we're at the top of the method
  int output_count = 0;
  ProfiledState* profile_data = method_profile->getProfileState(abc_pc, argc, output_count);

  profile_data->setInputType(0, args[0]); // The "this" object, which can be the unsuedAtomTag
  for (int i = 1; i < argc; i++) {
    assert (atomKind(args[i] != kUnusedAtomTag));
    profile_data->setInputType(i, args[i]);
  }
}

void updateBranchCounters(MethodEnv* method_env, int branch_pc, int true_target_pc, int false_target_pc, bool condition_result, LOpcode opcode) {
  MethodProfile* method_profile = JitManager::getProfile(method_env);
  assert (true_target_pc != false_target_pc); // Means we have a fall through
  switch (opcode) {
  case LIR_j:
  {
    assert (condition_result == 1);
    assert (false_target_pc == 0);
    method_profile->incrementBranchCounters(condition_result, branch_pc, true_target_pc, false_target_pc);
    break;
  }
  case LIR_jt:
  {
    method_profile->incrementBranchCounters(condition_result, branch_pc, true_target_pc, false_target_pc);
    break;
  }
  case LIR_jf:
  {
    method_profile->incrementBranchCounters(condition_result, branch_pc, false_target_pc, true_target_pc);
    break;
  }
  default:
    assert (false && "unknown opcode");
  }
}

/***
 * All potential inputs other than method_env can be NULL
 * a NULL input type means that the verifier already has a precise
 * type for a given input and so we don't need to record it.
 */
void recordUnaryInputTypes(MethodEnv* method_env, Atom input) {
  (void) method_env;
  (void) input;

  MethodProfile* methodProfile = JitManager::getProfile(method_env);
  (void) methodProfile;
  Atom testAtom = (Atom)0x8;
  if (atomKind(testAtom) == kDoubleType) {
      printf("TESTING");
  }

  assert (false);
}

void recordBinaryInputTypes(MethodEnv* method_env, Atom left_operand, Atom right_operand, int abc_pc) {
  MethodProfile* method_profile = JitManager::getProfile(method_env);
  int input_count = 2;
  int output_count = 1;
  ProfiledState* profiled_state = method_profile->getProfileState(abc_pc, input_count, output_count);

  assert (atomKind(left_operand) != kUnusedAtomTag);
  assert (atomKind(right_operand) != kUnusedAtomTag);

  profiled_state->setInputType(0, left_operand);
  profiled_state->setInputType(1, right_operand);
}

void recordTernaryInputTypes(MethodEnv* method_env, Atom first_operand, Atom second_operand, Atom third_operand, Atom result_operand) {
  (void) method_env;
  (void) first_operand;
  (void) second_operand;
  (void) third_operand;
  (void) result_operand;
  assert (false);
}

void recordCallInputTypes(MethodEnv* method_env, Atom receiver_object, int argc, Atom* atom_args, Atom result_atom, MethodInfo* loaded_method, int abc_pc) {
  MethodProfile* method_profile = JitManager::getProfile(method_env);
  int output_count = 1;
  int input_count = 1 + argc; // + 1 for the receiver object
  ProfiledState* profiled_state = method_profile->getProfileState(abc_pc, input_count, output_count);

  profiled_state->setInputType(0, receiver_object);
  for (int i = 1; i < argc + 1; i++) {
    profiled_state->setInputType(i, atom_args[i]);
  }

  profiled_state->setOutputType(0, result_atom);
  (void) loaded_method;
}

FUNCTION(uintptr_t(profiler::updateBranchCounters), SIG6(V,P,I,I,I,I,I), updateBranchCounters)
//FUNCTION(uintptr_t(profiler::recordUnaryInputTypes), SIG2(V,P,A), recordUnaryInputTypes)
FUNCTION(uintptr_t(profiler::recordBinaryInputTypes), SIG4(V,P,A,A,I), recordBinaryInputTypes)
//FUNCTION(uintptr_t(profiler::recordTernaryInputTypes), SIG6(V,P,A,A,A,A,I), recordTernaryInputTypes)
FUNCTION(uintptr_t(profiler::recordCallInputTypes), SIG7(V,P,A,I,P,A,A,I), recordCallInputTypes)
FUNCTION(uintptr_t(profiler::recordArgumentTypes), SIG3(V,P,I,P), recordArgumentTypes);

/***
 * CallInfos ONLY for the first JIT compiler phase with light weight
 * profiling instrumentation. Heavy weight profiling CallInfos
 * should be seperate in profiler_jit_calls.h
 */
bool isHotMethod(int invocation_count) {
  return invocation_count == 10;
}

// Only run profiling code for 10 iterations
bool isRunningProfilingCode(int invocation_count) {
  return (invocation_count > 10) && (invocation_count < 15);
}

bool gatheredAllProfilingData(int invocation_count) {
  return invocation_count > 15;
}

void incrementMethodInvokeCounter(MethodEnv* method_env) {
  MethodProfile* method_profile = JitManager::getProfile(method_env);
  int invocation_count = method_profile->method_invocation_count_;
  method_profile->method_invocation_count_++;

  if (isRunningProfilingCode(invocation_count)) {
    return;
  }

  if (gatheredAllProfilingData(invocation_count)) {
    //AvmAssert(method_profile->current_profiler_state_ == PROFILING);
    method_profile->current_profiler_state_ = GATHERED;
    BaseExecMgr::setRecompileWithProfileData(method_env);
    return;
  }

  if (isHotMethod(invocation_count) && !method_env->method->isNative()) {
    AvmAssert(method_profile->current_profiler_state_ == NONE);
    method_profile->current_profiler_state_ = PROFILING;
    BaseExecMgr::setRecompileHotMethod(method_env);
    return;
  }
}
FUNCTION(uintptr_t(incrementMethodInvokeCounter), SIG1(V, P), incrementMethodInvokeCounter)

} // namespace profiler

#endif // VMCFG_HALFMOON
