/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "profiler-main.h"
#ifdef VMCFG_HALFMOON
#include "../halfmoon/halfmoon.h"

namespace profiler {

void finish(MethodProfile* p) {
  p->finish();
}

bool hasBailedOut(MethodProfile* p) {
  return p->hasBailedOut();
}

MethodProfileMgr::MethodProfileMgr(Allocator& alloc)
: method_profile_allocator_(alloc),
  input_types_allocator_(alloc),
  method_info_map_(method_profile_allocator_) {
}

MethodProfile* MethodProfileMgr::getMethodProfile(MethodInfo* methodInfo) {
  assert(methodInfo != NULL);
  MethodProfile* methodProfile = method_info_map_.get(methodInfo);
  if (methodProfile == NULL) {
    methodProfile = new (method_profile_allocator_) MethodProfile(
        method_profile_allocator_, input_types_allocator_);
    method_info_map_.put(methodInfo, methodProfile);
  }

  assert(methodProfile != NULL);
  return methodProfile;
}

/*** 
 * MethodProfile shares the same allocator as the MethodProfileMgr
 * so we can clean all the underlying MethodProfiles when we clean
 * MethodProfileMgr. Also, if each MethodProfile has its own allocator,
 * we instantly get GC Leaks, and I'm not sure why.
 */
MethodProfile::MethodProfile(Allocator& method_profile_mgr_allocator,
                             Allocator& input_types_allocator)
: branch_counter_allocator_(method_profile_mgr_allocator),
  input_types_allocator_(input_types_allocator),
  branch_counter_map_(branch_counter_allocator_),
  runtime_type_map_(input_types_allocator_) {
  this->loop_iteration_count_ = 0;
  this->method_invocation_count_ = 0;
  this->current_profiler_state_ = NONE;
}

MethodProfile::~MethodProfile() {
}

int32_t MethodProfile::getLoopIterationCount(int branch_pc, int target_pc) {
  int loop_count = getBranchCount(branch_pc, target_pc);
  assert(loop_count != 0);
  return loop_count;
}

double MethodProfile::getBranchProbability(int branch_abc_pc,
                                           int target_abc_pc) {
  HashMap<int, int>* branch_targets = branch_counter_map_.get(branch_abc_pc);
  assert(branch_targets != NULL && "Not a valid branch abc pc");

  HashMap<int, int>::Iter iter(*branch_targets);
  int total_branch_count = 0;
  while (iter.next()) {
    int branch_count = iter.value();
    total_branch_count += branch_count;
  }

  int target_branch_count = branch_targets->get(target_abc_pc);
  assert(target_branch_count <= total_branch_count);
  return (double) target_branch_count / (double) total_branch_count;
}

void MethodProfile::finish() {
  method_invocation_count_ = 0;
  current_profiler_state_ = INACCURATE;
  branch_counter_map_.clear();
  runtime_type_map_.clear();
}

int MethodProfile::getBranchCount(int branch_abc_pc, int target_abc_pc) {
  HashMap<int, int>* branch_targets = branch_counter_map_.get(branch_abc_pc);
  assert(branch_targets != NULL && "Not a valid branch abc pc");
  assert(branch_targets->containsKey(target_abc_pc) &&
         "Not a valid branch target pc");

  return branch_targets->get(target_abc_pc);
}

HashMap<int, int>* MethodProfile::getBranchTargetCounters(int abc_branch_pc) {
  HashMap<int, int>* branch_targets = branch_counter_map_.get(abc_branch_pc);
  if (branch_targets == NULL) {
    int default_hashmap_size = 2;
    branch_targets = new (branch_counter_allocator_) HashMap<int, int>(
        branch_counter_allocator_, default_hashmap_size);
    branch_counter_map_.put(abc_branch_pc, branch_targets);
  }

  return branch_targets;
}

void MethodProfile::initializeBranchCounters(int start_pc, int true_target_pc,
                                             int false_target_pc) {
  HashMap<int, int>* branch_target_count = getBranchTargetCounters(start_pc);
  branch_target_count->put(true_target_pc, 0);
  branch_target_count->put(false_target_pc, 0);
}

void MethodProfile::incrementBranchCounters(bool took_branch, int start_pc,
                                            int true_target_pc,
                                            int false_target_pc) {
  HashMap<int, int>* branch_target_count = getBranchTargetCounters(start_pc);
  int taken_branch = took_branch ? true_target_pc : false_target_pc;
  int updated_count = branch_target_count->get(taken_branch) + 1;
  branch_target_count->put(taken_branch, updated_count);
}

bool MethodProfile::hasGatheredAllData() {
  return current_profiler_state_ == GATHERED;
}

bool MethodProfile::hasBailedOut() {
  return current_profiler_state_ == INACCURATE;
}

ProfiledState* MethodProfile::getProfileState(int abc_pc, int input_count,
                                              int output_count) {
  assert(!hasBailedOut());
  ProfiledState* profiled_state = runtime_type_map_.get(abc_pc);
  if (profiled_state == NULL) {
    profiled_state = new (input_types_allocator_) ProfiledState(
        input_types_allocator_, input_count, output_count);
    runtime_type_map_.put(abc_pc, profiled_state);
  }

  assert(profiled_state != NULL);
  return profiled_state;
}

} // end namespace
#endif // VMCFG_HALFMOON
