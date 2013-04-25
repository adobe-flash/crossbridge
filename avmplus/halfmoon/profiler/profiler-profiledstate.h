/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/***
 * Represents the profile information gathered at one specific PC in
 * the ABC program. The information is specific to ABC information, not LIR.
 * We keep track of the ABC locals, scope stack, and operand stack type information.
 */

#ifndef PROFILER_PROFILEDSTATE_H_
#define PROFILER_PROFILEDSTATE_H_

/***
 * This represents the runtime profiled information
 * at a single ABC PC location. We only need to keep 
 * pertinent information about the current ABC opcode.
 * 
 * So we only have to track inputs -> output types.
 * Only need to track output types if we can have multiple
 * output types. eg bitand will always return an integer, so no
 * reason to track it.
 */
namespace profiler {
using avmplus::AvmCore;
using avmplus::Traits;
using avmplus::Atom;
using avmplus::ScriptObject;

class ProfiledState {
public:
  ProfiledState(Allocator& allocator, int input_count, int output_count);

  RecordedType getInputType(int inputIndex);
  RecordedType getOutputType(int outputIndex);
  bool isVariadicInputType(int inputIndex);
  bool isNumericType(RecordedType t);
  void setInputType(int inputIndex, Atom value);
  void setInputRecordedType(int input_index, RecordedType rt);
  void setOutputType(int outputIndex, Atom value);
  bool hasData();
  void init();
  RecordedType getRecordedType(Atom value);

private:
  void allocateInputTypes();
  RecordedType* profiled_types_;
  Allocator& allocator_;
  int input_count_;
  int output_count_;
};
}

#endif // PROFILER_PROFILEDSTATE_H_
