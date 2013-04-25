/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
  BailoutData::BailoutData(Allocator& alloc, MethodInfo* method, 
    int scope_base, int stack_base) 
    : alloc_(alloc) 
    , method_info_(method)
    , state_map_(alloc)
    , scope_base_(scope_base)
    , stack_base_(stack_base)
  {
    MethodSignaturep method_signature = method->getMethodSignature();
    frame_size_ = method_signature->frame_size();
    init();
  }

  BailoutData::~BailoutData() {
  }

  void BailoutData::setNativeType(int local_index, SlotStorageType sst_type) {
    current_state_->local_types[local_index] = sst_type;
  }

  void BailoutData::do_safepoint(int abc_pc, int abc_scope_ptr, 
      int abc_stack_ptr) {
    current_state_->scope_ptr = abc_scope_ptr;
    current_state_->stack_ptr = abc_stack_ptr;
    state_map_.put(abc_pc, current_state_);

    SafepointData* new_state = new (alloc_) SafepointData;
    new_state->local_types = new (alloc_) SlotStorageType[frame_size_];
    new_state->scope_ptr = abc_scope_ptr;
    new_state->stack_ptr = abc_stack_ptr;

    SlotStorageType* local_types = new_state->local_types;
    for (int i = 0; i < frame_size_; i++) {
      local_types[i] = current_state_->local_types[i];
    }

    current_state_ = new_state;
  }

  int BailoutData::getStackPointer(int abc_pc) {
    SafepointData* state = getSafepointData(abc_pc);
    return state->stack_ptr;
  }

  int BailoutData::getScopeStack(int abc_pc) {
    SafepointData* state = getSafepointData(abc_pc);
    return state->scope_ptr;
  }

  SafepointData* BailoutData::getSafepointData(int abc_pc) {
    assert (state_map_.containsKey(abc_pc));
    return state_map_.get(abc_pc);
  }

  void BailoutData::init() {
    current_state_ = new (alloc_) SafepointData;
    current_state_->local_types = new (alloc_) SlotStorageType[frame_size_];
    current_state_->stack_ptr = stack_base_;
    current_state_->scope_ptr = scope_base_;
  }

  void BailoutData::clean() {
    state_map_.clear();
  }

} // end namespace
#endif // VMCFG_HALFMOON
