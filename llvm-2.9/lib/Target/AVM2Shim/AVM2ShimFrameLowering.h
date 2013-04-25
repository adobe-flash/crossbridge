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

#ifndef AVM2_FRAMEINFO_H
#define AVM2_FRAMEINFO_H

#include "AVM2Shim.h"
#include "AVM2ShimSubtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm
{
class AVM2ShimTargetMachine;
class AVM2ShimSubtarget;

class AVM2ShimFrameLowering : public TargetFrameLowering
{
    const AVM2ShimTargetMachine &TM;
    const AVM2ShimSubtarget &STI;
public:
    explicit AVM2ShimFrameLowering(const AVM2ShimTargetMachine &tm, const AVM2ShimSubtarget &sti)
        : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 16, 0), TM(tm), STI(sti) {
    }

    bool hasFP(const MachineFunction &MF) const {
        return true;
    }
    
    virtual void emitPrologue(llvm::MachineFunction&) const;
    virtual void emitEpilogue(llvm::MachineFunction&, llvm::MachineBasicBlock&) const;
};

} // End llvm namespace

#endif
