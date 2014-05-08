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

//====- AVM2FrameLowering.cpp - AVM2 Frame Information -------*- C++ -*-====//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#include "AVM2FrameLowering.h"
#include "AVM2InstrInfo.h"
#include "AVM2MachineFunctionInfo.h"
#include "AVM2TargetMachine.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

void AVM2FrameLowering::getInitialFrameState(std::vector<MachineMove> &Moves) const
{
  MachineLocation Dst(MachineLocation::VirtualFP);
  MachineLocation Src(AVM2::EDP, -20); // debug frame is 5 x 32bit values
  Moves.push_back(MachineMove(0, Dst, Src));

  {
    MachineLocation RSDst(AVM2::EDP, 0);
    MachineLocation RSSrc(AVM2::EIP);
    Moves.push_back(MachineMove(0, RSDst, RSSrc));
  }

  {
    MachineLocation RSDst(AVM2::EDP, 4);
    MachineLocation RSSrc(AVM2::EBP);
    Moves.push_back(MachineMove(0, RSDst, RSSrc));
  }

  {
    MachineLocation RSDst(AVM2::EDP, 8);
    MachineLocation RSSrc(AVM2::EI32);
    Moves.push_back(MachineMove(0, RSDst, RSSrc));
  }

  {
    MachineLocation RSDst(AVM2::EDP, 12);
    MachineLocation RSSrc(AVM2::EF32);
    Moves.push_back(MachineMove(0, RSDst, RSSrc));
  }

  {
    MachineLocation RSDst(AVM2::EDP, 16);
    MachineLocation RSSrc(AVM2::EF64);
    Moves.push_back(MachineMove(0, RSDst, RSSrc));
  }
}

int AVM2FrameLowering::getFrameIndexReference(const MachineFunction &MF, int FI,
                                     unsigned &FrameReg) const
{
	const MachineFrameInfo *MFI = MF.getFrameInfo();
	int Offset = MFI->getObjectOffset(FI);

	FrameReg = AVM2::EBP;
	return Offset;
}

void AVM2FrameLowering::emitPrologue(MachineFunction &MF) const
{
    MachineBasicBlock &MBB = MF.front();
    MachineFrameInfo *MFI = MF.getFrameInfo();
    const AVM2InstrInfo &TII =
        *static_cast<const AVM2InstrInfo*>(MF.getTarget().getInstrInfo());

    // Get the number of bytes to allocate from the FrameInfo
    int NumBytes = (int) MFI->getStackSize();

    MachineBasicBlock::iterator InsertPt = MBB.begin();
    DebugLoc DL;
    BuildMI(MBB, InsertPt, DL, TII.get(AVM2::MOVrr), AVM2::EBP).addReg(AVM2::ESP);
    if(NumBytes) {
        BuildMI(MBB, InsertPt, DL, TII.get(AVM2::DECri), AVM2::ESP).addImm(NumBytes);
    }
}

void AVM2FrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const
{
    const AVM2InstrInfo &TII =
        *static_cast<const AVM2InstrInfo*>(MF.getTarget().getInstrInfo());

    MachineBasicBlock::iterator MBBI = prior(MBB.end());
    DebugLoc DL;
    BuildMI(MBB, MBBI, DL, TII.get(AVM2::MOVrr), AVM2::ESP).addReg(AVM2::EBP);
}


