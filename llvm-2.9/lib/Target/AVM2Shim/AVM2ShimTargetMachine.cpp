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

#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/CodeGen/LiveVariables.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "AVM2ShimTargetMachine.h"
#include "AVM2ShimTargetAsmInfo.h"
#include "AVM2Shim.h"
#include <set>

using namespace llvm;
extern Target llvm::TheAVM2ShimTarget;

extern "C" void LLVMInitializeAVM2ShimTarget()
{
    RegisterTargetMachine<AVM2ShimTargetMachine> X(TheAVM2ShimTarget);
    RegisterAsmInfo<AVM2ShimTargetAsmInfo> A(TheAVM2ShimTarget);
}

/// AVM2TargetMachine ctor
///
AVM2ShimTargetMachine::AVM2ShimTargetMachine(const llvm::Target& T, const std::string& TT, const std::string& FS)
    : LLVMTargetMachine( T, TT),
      DataLayout("e-p:32:32-f64:32:64-i64:32:64"),		// original 2.1 data layout string.
      Subtarget(T, TT, FS),
      TLInfo(*this),
      FrameLowering(*this, Subtarget)
{
}

static TargetLoweringObjectFile *createTLOF(TargetMachine &TM)
{
    return new TargetLoweringObjectFileELF();
}

AVM2ShimTargetLowering::AVM2ShimTargetLowering(TargetMachine &TM)
: TargetLowering(TM, createTLOF(TM))
{
}

unsigned int AVM2ShimTargetLowering::getFunctionAlignment(const llvm::Function*) const
{
    return 2;
}