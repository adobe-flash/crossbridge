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

//===- AVM2InstrInfo.cpp - AVM2 Instruction Information -------*- C++ -*-===//
//
// This file contains the AVM2 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_DESC
#include "AVM2InstrInfo.h"
#include "AVM2.h"
#include "AVM2TargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

#define GET_INSTRINFO_CTOR 
#include "AVM2GenInstrInfo.inc"

using namespace llvm;

AVM2InstrInfo::AVM2InstrInfo(AVM2Subtarget &ST)
    : AVM2GenInstrInfo(AVM2::ADJCALLSTACKDOWN, AVM2::ADJCALLSTACKUP),
      Subtarget(ST),
      RI(ST, *this)
{
    InitAVM2MCInstrInfo(this);
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned AVM2InstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
        int &FrameIndex) const
{
    if (MI->getOpcode() == AVM2::asLD32ri ||
            MI->getOpcode() == AVM2::asSLD32ri ||
            MI->getOpcode() == AVM2::asFLD32ri ||
            MI->getOpcode() == AVM2::asFLD64ri ||
            MI->getOpcode() == AVM2::inLD32ri ||
            MI->getOpcode() == AVM2::inSLD32ri ||
            MI->getOpcode() == AVM2::inFLD32ri ||
            MI->getOpcode() == AVM2::inFLD64ri
	) {
        if (MI->getOperand(1).isFI() && MI->getOperand(2).isImm() &&
                MI->getOperand(2).getImm() == 0) {
            FrameIndex = MI->getOperand(1).getIndex();
            return MI->getOperand(0).getReg();
        }
    }
    return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned AVM2InstrInfo::isStoreToStackSlot(const MachineInstr *MI,
        int &FrameIndex) const
{
    if (MI->getOpcode() == AVM2::asST32ri ||
            MI->getOpcode() == AVM2::asSST32ri ||
            MI->getOpcode() == AVM2::asFST32ri ||
            MI->getOpcode() == AVM2::asFST64ri ||
            MI->getOpcode() == AVM2::inST32ri ||
            MI->getOpcode() == AVM2::inSST32ri ||
            MI->getOpcode() == AVM2::inFST32ri ||
            MI->getOpcode() == AVM2::inFST64ri 
	) {
        if (MI->getOperand(0).isFI() && MI->getOperand(1).isImm() &&
                MI->getOperand(1).getImm() == 0) {
            FrameIndex = MI->getOperand(0).getIndex();
            return MI->getOperand(2).getReg();
        }
    }
    return 0;
}

unsigned AVM2InstrInfo::InsertBranch(MachineBasicBlock &MBB,MachineBasicBlock *TBB,
                                     MachineBasicBlock *FBB,
                                     const SmallVectorImpl<MachineOperand> &Cond,
                                     DebugLoc DL) const
{
    // Can only insert uncond branches so far.
    assert(Cond.empty() && !FBB && TBB && "Can only handle uncond branches!");
    BuildMI(&MBB, DL, get(Subtarget.useInlineAsm() ? AVM2::asBR : AVM2::inBR)).addMBB(TBB);
    return 1;
}

void AVM2InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI, DebugLoc DL,
                                unsigned DestReg, unsigned SrcReg,
                                bool KillSrc) const
{
    const MachineRegisterInfo *MRI = &(MBB.getParent()->getRegInfo());
    const TargetRegisterClass *SrcRC = AVM2RegisterInfo::getRegClass(MRI, SrcReg),
                               *DestRC = AVM2RegisterInfo::getRegClass(MRI, DestReg);


    if (DestRC != SrcRC) {
        // Not yet supported!
        assert(!"dest and src different register classes");
    }

    if (DestRC == &AVM2::IntRegsRegClass) {
        BuildMI(MBB, MI, DL, get(AVM2::MOVrr), DestReg).addReg(SrcReg);
    } else if (DestRC == &AVM2::FPRegsRegClass) {
        BuildMI(MBB, MI, DL, get(AVM2::FMOVrr), DestReg).addReg(SrcReg);
    } else if (DestRC == &AVM2::SPRegsRegClass) {
        BuildMI(MBB, MI, DL, get(AVM2::SMOVrr), DestReg).addReg(SrcReg);
    } else
        // Can't copy this register
    {
        assert("!can't copy this register");
    }
}

void AVM2InstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC,
                    const TargetRegisterInfo *TRI) const
{

    DebugLoc DL;
    if (I != MBB.end()) {
        DL = I->getDebugLoc();
    }

    // On the order of operands here: think "[FrameIdx + 0] = SrcReg".
    if (RC == &AVM2::IntRegsRegClass)
        BuildMI(MBB, I, DL, get(Subtarget.useInlineAsm() ? AVM2::asST32ri : AVM2::inST32ri)).addFrameIndex(FI).addImm(0)
        .addReg(SrcReg, getKillRegState(isKill));
    else if (RC == &AVM2::FPRegsRegClass)
        BuildMI(MBB, I, DL, get(Subtarget.useInlineAsm() ? AVM2::asFST64ri : AVM2::inFST64ri)).addFrameIndex(FI).addImm(0)
        .addReg(SrcReg,  getKillRegState(isKill));
    else if (RC == &AVM2::SPRegsRegClass)
        BuildMI(MBB, I, DL, get(Subtarget.useInlineAsm() ? AVM2::asSST32ri : AVM2::inSST32ri)).addFrameIndex(FI).addImm(0)
        .addReg(SrcReg,  getKillRegState(isKill));
    else {
        assert( 0 && "Can't store this register to stack slot");
    }
}

void AVM2InstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC,
                     const TargetRegisterInfo *TRI) const
{
    DebugLoc DL;
    if (I != MBB.end()) {
        DL = I->getDebugLoc();
    }

    if (RC == &AVM2::IntRegsRegClass) {
        BuildMI(MBB, I, DL, get(Subtarget.useInlineAsm() ? AVM2::asLD32ri : AVM2::inLD32ri), DestReg).addFrameIndex(FI).addImm(0);
    } else if (RC == &AVM2::FPRegsRegClass) {
        BuildMI(MBB, I, DL, get(Subtarget.useInlineAsm() ? AVM2::asFLD32ri : AVM2::inFLD32ri), DestReg).addFrameIndex(FI).addImm(0);
    } else if (RC == &AVM2::SPRegsRegClass) {
        BuildMI(MBB, I, DL, get(Subtarget.useInlineAsm() ? AVM2::asSLD32ri : AVM2::inSLD32ri), DestReg).addFrameIndex(FI).addImm(0);
    } else {
        assert( 0 && "Can't load this register from stack slot");
    }
}

