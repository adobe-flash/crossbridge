// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//===- AVM2Subtarget.cpp - AVM2 Subtarget Information -------------------===//
//
// This file implements the AVM2 specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#include "AVM2Subtarget.h"
#include "AVM2.h"

#define GET_SUBTARGETINFO_ENUM
#define GET_SUBTARGETINFO_MC_DESC
#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "AVM2GenSubtargetInfo.inc"

using namespace llvm;

//#include "llvm/Support/CommandLine.h"

static cl::opt<bool> UseAS3Asm(
  "use-legacy-as3-asm",
  cl::init(false),
  cl::desc("Use AS3 Assembly instead of intrinsics"),
  cl::Hidden);

AVM2Subtarget::AVM2Subtarget(const std::string &TT, const std::string &CPU,
                             const std::string &FS)
  :AVM2GenSubtargetInfo(TT, CPU, FS)
{
    // Determine default and user specified characteristics
    //std::string CPU = "generic";
    UseActivations = false;
    DisableDebugLines = true;
    ForceOrderedCompares = false;
    // Parse features string.
    ParseSubtargetFeatures(CPU, FS);
}

bool AVM2Subtarget::useInlineAsm() const { return UseAS3Asm; }
bool AVM2Subtarget::useIntrinsics() const { return !UseAS3Asm; }

