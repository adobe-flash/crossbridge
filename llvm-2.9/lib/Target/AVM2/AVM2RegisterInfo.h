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

#ifndef AVM2REGISTERINFO_H
#define AVM2REGISTERINFO_H

#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "AVM2GenRegisterInfo.h.inc"

namespace llvm
{

class AVM2Subtarget;
class TargetInstrInfo;
class Type;

struct AVM2RegisterInfo : public AVM2GenRegisterInfo {
    AVM2Subtarget &Subtarget;
    const TargetInstrInfo &TII;

    AVM2RegisterInfo(AVM2Subtarget &st, const TargetInstrInfo &tii);


    // TODO this must already exist...
    static const TargetRegisterClass *getRegClass(const MachineRegisterInfo *MRI, unsigned Reg) {
        if(TargetRegisterInfo::isPhysicalRegister(Reg)) {
            if(AVM2::IntRegsRegClass.contains(Reg)) {
                return &AVM2::IntRegsRegClass;
            } else if(AVM2::FPRegsRegClass.contains(Reg)) {
                return &AVM2::FPRegsRegClass;
            } else if(AVM2::SPRegsRegClass.contains(Reg)) {
                return &AVM2::SPRegsRegClass;
            }
        } else {
            return MRI->getRegClass(Reg);
        }
        assert(!"couldn't gind reg class");
        return NULL;
    }

    const unsigned *getCalleeSavedRegs(const MachineFunction *MF = 0) const;

    const TargetRegisterClass* const* getCalleeSavedRegClasses(const MachineFunction *MF = 0) const;

    BitVector getReservedRegs(const MachineFunction &MF) const;

    bool hasFP(const MachineFunction &MF) const;

    void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                       MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator I) const;

    virtual void eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                     int AVM2Adj, //int *Value = NULL,
                                     RegScavenger *RS=NULL) const;

    void processFunctionBeforeFrameFinalized(MachineFunction &MF) const;

    // Debug information queries.
    unsigned getRARegister() const;
    unsigned getFrameRegister(const MachineFunction &MF) const;

    // Exception handling queries.
    unsigned getEHExceptionRegister() const;
    unsigned getEHHandlerRegister() const;
    virtual int getDwarfRegNum(unsigned int, bool) const;
};

} // end namespace llvm

#endif
