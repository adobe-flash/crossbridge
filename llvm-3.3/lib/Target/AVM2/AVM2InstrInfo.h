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

//===- AVM2InstrInfo.h - AVM2 Instruction Information ---------*- C++ -*-===//
//
// This file contains the AVM2 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef AVM2INSTRUCTIONINFO_H
#define AVM2INSTRUCTIONINFO_H

#include "llvm/Target/TargetInstrInfo.h"
#include "AVM2RegisterInfo.h"

#define GET_INSTRINFO_HEADER
#include "AVM2GenInstrInfo.inc"

namespace llvm
{

/// AVM2II - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
namespace AVM2II
{
enum {
    Pseudo = (1<<0),
    Load = (1<<1),
    Store = (1<<2)
};
}

class AVM2InstrInfo : public AVM2GenInstrInfo
{
    const AVM2Subtarget &Subtarget;
    const AVM2RegisterInfo RI;
public:
    AVM2InstrInfo(AVM2Subtarget &ST);

    /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
    /// such, whenever a client has an instance of instruction info, it should
    /// always be able to get register info as well (through this method).
    ///
    virtual const TargetRegisterInfo &getRegisterInfo() const {
        return RI;
    }

    /// isLoadFromStackSlot - If the specified machine instruction is a direct
    /// load from a stack slot, return the virtual or physical register number of
    /// the destination along with the FrameIndex of the loaded stack slot.  If
    /// not, return 0.  This predicate must return 0 if the instruction has
    /// any side effects other than loading from the stack slot.
    virtual unsigned isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const;

    /// isStoreToStackSlot - If the specified machine instruction is a direct
    /// store to a stack slot, return the virtual or physical register number of
    /// the source reg along with the FrameIndex of the loaded stack slot.  If
    /// not, return 0.  This predicate must return 0 if the instruction has
    /// any side effects other than storing to the stack slot.
    virtual unsigned isStoreToStackSlot(const MachineInstr *MI, int &FrameIndex) const;


    virtual unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                                  MachineBasicBlock *FBB,
                                  const SmallVectorImpl<MachineOperand> &Cond, DebugLoc DL) const;

    virtual void copyPhysReg(MachineBasicBlock &MBB,
                             MachineBasicBlock::iterator MI, DebugLoc DL,
                             unsigned DestReg, unsigned SrcReg,
                             bool KillSrc) const;

    virtual void storeRegToStackSlot(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator MBBI,
                                     unsigned SrcReg, bool isKill, int FrameIndex,
                                     const TargetRegisterClass *RC, const llvm::TargetRegisterInfo *TRI) const;

    virtual void loadRegFromStackSlot(MachineBasicBlock &MBB,
                                      MachineBasicBlock::iterator MBBI,
                                      unsigned DestReg, int FrameIndex,
                                      const TargetRegisterClass *RC, const llvm::TargetRegisterInfo *TRI) const;
};

}

#endif
