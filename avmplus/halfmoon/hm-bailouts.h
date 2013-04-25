/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_BAILOUTS_H
#define HM_BAILOUTS_H

namespace halfmoon {
  struct SafepointData {
    SlotStorageType*  local_types;
    int   stack_ptr;
    int   scope_ptr;
  };

  /// At a given savepoint, we have to store the state of the
  /// locals, scope stack, and operand stack for temporaries.
  /// At the moment, it's only limited to the native type represented by halfmoon
class BailoutData {
  typedef HashMap<int, SafepointData*> StateMap;

public:
  BailoutData(Allocator& alloc, MethodInfo* method, int scope_base, int stack_base);
  ~BailoutData();

  int getStackPointer(int abc_pc);
  int getScopeStack(int abc_pc);
  void setNativeType(int localIndex, SlotStorageType sst_type);
  void do_safepoint(int abc_pc, int abc_scope_ptr, int abc_stack_ptr);
  void init();
  void clean();

private:
  SafepointData* getSafepointData(int abc_pc);

private:
  // Data
  Allocator& alloc_;
  MethodInfo* method_info_;
  StateMap state_map_;
  SafepointData* current_state_;
  int frame_size_;
  int scope_base_;
  int stack_base_;
};

} // end namespace

#endif

