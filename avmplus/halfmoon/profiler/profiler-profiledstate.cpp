/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "profiler-main.h"
#ifdef VMCFG_HALFMOON

namespace profiler {

ProfiledState::ProfiledState(Allocator& allocator, int input_count, int output_count)
  : allocator_(allocator)
{
  this->input_count_ = input_count;
  this->output_count_ = output_count;
  init();
}

RecordedType ProfiledState::getInputType(int input_index) {
  assert (hasData());
  assert (input_index < input_count_);
  return profiled_types_[input_index];
}

RecordedType ProfiledState::getOutputType(int output_index) {
  assert (hasData());
  assert (output_index < output_count_);
  return profiled_types_[output_index + input_count_];
}

void ProfiledState::setOutputType(int output_index, Atom value) {
  assert (hasData());
  assert (output_index < output_count_);
  profiled_types_[output_index + input_count_] = getRecordedType(value);
}

RecordedType ProfiledState::getRecordedType(Atom value) {
  Atom atom_kind = atomKind(value);
  switch (atom_kind) {
  case kDoubleType: return kDOUBLE;
  case kIntptrType: return kINTEGER;
  case kStringType: return kSTRING;
  case kBooleanType: return kBOOLEAN;
  case kObjectType: {
    ScriptObject* object = AvmCore::atomToScriptObject(value);
    if (object == NULL) return kOBJECT; // undefined object
    if (object->toArrayObject() != NULL) return kARRAY;
    return kOBJECT;
  }
  case kUnusedAtomTag: return kUNINITIALIZED;
  case kNamespaceType: return kUNSUPPORTED;
  case kSpecialBibopType: return kUNSUPPORTED;
  default:
    printf("Unknown atom type %d\n", (int) atom_kind);
    assert (false);
  }

  assert (false);
  return kUNINITIALIZED;
}

bool ProfiledState::isNumericType(RecordedType t) {
  return t == kINTEGER 
    || t == kDOUBLE
    || t == kNUMERIC;
}


void ProfiledState::setInputType(int input_index, Atom value) {
  RecordedType new_type = getRecordedType(value);
  setInputRecordedType(input_index, new_type);
}

void ProfiledState::setInputRecordedType(int input_index, RecordedType new_type) {
  assert (hasData());
  assert (input_index < input_count_);
  RecordedType previous_type = profiled_types_[input_index];

  if ((new_type != previous_type) &&
      (previous_type != kUNINITIALIZED)) {
      new_type = kVARIADIC;
  }

  profiled_types_[input_index] = new_type;
}

void ProfiledState::init() {
  allocateInputTypes();
}

/***
 * If we have no data, that means the verifier already refined
 * the types to something more than the any type.
 */
bool ProfiledState::hasData() {
  return profiled_types_ != NULL;
}

void ProfiledState::allocateInputTypes() {
  int total_size = input_count_ + output_count_;
  profiled_types_ = new (allocator_) RecordedType[total_size];
  for (int i = 0; i < total_size; i++)
    profiled_types_[i] = kUNINITIALIZED;
}

} // namespace profiler {

#endif // VMCFG_HALFMOON
