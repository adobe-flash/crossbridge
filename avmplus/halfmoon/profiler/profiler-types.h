/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef PROFILER_TYPES_
#define PROFILER_TYPES_

namespace profiler {

enum RecordedType {
  kUNINITIALIZED,
  kVARIADIC,
  kUNSUPPORTED,
  kINTEGER,
  kDOUBLE,
  kNUMERIC, // Seen as both a double and int
  kOBJECT,
  kARRAY,
  kVECTOR,
  kBOOLEAN,
  kSTRING,
  kNAMESPACE,
};

};

#endif // end DEFINE PROFILER_TYPES_
