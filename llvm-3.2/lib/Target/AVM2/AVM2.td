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

//===- AVM2.td - Describe the AVM2 Target Machine -------------*- C++ -*-===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// AVM2 Subtarget features.
//
 
def FeatureUseActivations : SubtargetFeature<"use-activations", "UseActivations", "true",
                                     "Use activations for virtual registers">;
def FeatureForceOrderedCompares : SubtargetFeature<"force-ordered-compares", "ForceOrderedCompares", "true",
                                     "Force ordered comparisons of floating point values">;

def FeatureDisableDebugLines : SubtargetFeature<"disable-debuglines", "DisableDebugLines", "true",
                                      "Disable explicit debugline emission">;

//===----------------------------------------------------------------------===//
// Register File Description
//===----------------------------------------------------------------------===//

include "AVM2RegisterInfo.td"
include "AVM2CallingConv.td"

//===----------------------------------------------------------------------===//
// Instruction Descriptions
//===----------------------------------------------------------------------===//

include "AVM2InstrInfo.td"

def AVM2InstrInfo : InstrInfo {
  // Define how we want to layout our target-specific information field.
}

//===----------------------------------------------------------------------===//
// AVM2 processors supported.
//===----------------------------------------------------------------------===//

def : Processor<"avm2", NoItineraries, []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def AVM2 : Target {
  // Pull in Instruction Info:
  let InstructionSet = AVM2InstrInfo;
}
