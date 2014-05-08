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

//===- AVM2FrameLowering.h - Define frame lowering for AVM2 --*- C++ -*--===//

#ifndef AVM2_FRAMEINFO_H
#define AVM2_FRAMEINFO_H

#include "AVM2.h"
#include "AVM2Subtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm
{
class AVM2TargetMachine;
class AVM2Subtarget;

class AVM2FrameLowering : public TargetFrameLowering
{
    const AVM2TargetMachine &TM;
    const AVM2Subtarget &STI;
public:
    explicit AVM2FrameLowering(const AVM2TargetMachine &tm, const AVM2Subtarget &sti)
        : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 16, 0), TM(tm), STI(sti) {
    }

    void getInitialFrameState(std::vector<MachineMove> &Moves) const;
	int getFrameIndexReference(const MachineFunction &MF, int FI,
                                     unsigned &FrameReg) const;
    /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
    /// the function.
    void emitPrologue(MachineFunction &MF) const;
    void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

    bool hasFP(const MachineFunction &MF) const {
        return true;
    }
};

} // End llvm namespace

#endif
