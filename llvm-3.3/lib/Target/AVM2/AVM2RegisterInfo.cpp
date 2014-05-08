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
//===- AVM2RegisterInfo.cpp - AVM2 Register Information -------*- C++ -*-===//
//
// This file contains the AVM2 implementation of the MRegisterInfo class.
//
//===----------------------------------------------------------------------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#define GET_REGINFO_TARGET_DESC
#define GET_REGINFO_MC_DESC
#include "AVM2.h"
#include "AVM2RegisterInfo.h"
#include "AVM2Subtarget.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/MachineLocation.h"
using namespace llvm;

AVM2RegisterInfo::AVM2RegisterInfo(AVM2Subtarget &st,
                                   const TargetInstrInfo &tii)
    : AVM2GenRegisterInfo(AVM2::EIP, 0, 0),
      Subtarget(st), TII(tii)
{
}

const uint16_t* AVM2RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
    static const uint16_t CalleeSavedRegs[] = { 0 };
    return CalleeSavedRegs;
}

BitVector AVM2RegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
    BitVector Reserved(getNumRegs());
    Reserved.set(AVM2::ESP);
    Reserved.set(AVM2::EBP);
// YYY put other regs here?
    return Reserved;
}


const TargetRegisterClass* const*
AVM2RegisterInfo::getCalleeSavedRegClasses(const MachineFunction *MF) const
{
    static const TargetRegisterClass * const CalleeSavedRegClasses[] = { 0 };
    return CalleeSavedRegClasses;
}

bool AVM2RegisterInfo::hasFP(const MachineFunction &MF) const
{
    return true;
}

void AVM2RegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const
{
    MachineInstr &MI = *I;
    int Size = MI.getOperand(0).getImm();
    if (Size) {
        DebugLoc DL;
        if (MI.getOpcode() == AVM2::ADJCALLSTACKDOWN) {
            BuildMI(MBB, I, DL, TII.get(AVM2::DECri), AVM2::ESP).addImm(Size);
        } else {
            BuildMI(MBB, I, DL, TII.get(AVM2::INCri), AVM2::ESP).addImm(Size);
        }
    }
    MBB.erase(I);
}

void AVM2RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
        int AVM2Adj, //int *Value,
        unsigned FIOperandNum,
        RegScavenger *RS) const
{
    assert(AVM2Adj == 0 && "Unexpected");

    unsigned i = 0;
    MachineInstr &MI = *II;
    while (!MI.getOperand(i).isFI()) {
        ++i;
        assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
    }

    int FrameIndex = MI.getOperand(i).getIndex();

    // Addressable stack objects are accessed using offsets from EBP
    MachineFunction &MF = *MI.getParent()->getParent();
    int Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex) +
                 MI.getOperand(i+1).getImm();

    // Replace frame index with a frame pointer reference.
    // If the offset is small enough to fit in the immediate field, directly
    // encode it.
    MI.getOperand(i).ChangeToRegister(AVM2::EBP, false);
    MI.getOperand(i+1).ChangeToImmediate(Offset);
}

void AVM2RegisterInfo::
processFunctionBeforeFrameFinalized(MachineFunction &MF) const {}

unsigned AVM2RegisterInfo::getRARegister() const
{
	return AVM2::EIP;
}

unsigned AVM2RegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
    return AVM2::EDP; // EDP for debugging...
}

unsigned AVM2RegisterInfo::getEHExceptionRegister() const
{
    assert(0 && "What is the exception register");
    return 0;
}

unsigned AVM2RegisterInfo::getEHHandlerRegister() const
{
    assert(0 && "What is the exception handler register");
    return 0;
}



