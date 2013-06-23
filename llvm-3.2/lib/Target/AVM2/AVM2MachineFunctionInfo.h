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
//===- AVM2MachineFunctionInfo.h - AVM2 Machine Function Info -*- C++ -*-===//
//
// This file declares  AVM2 specific per-machine-function information.
//
//===----------------------------------------------------------------------===//
#ifndef AVM2MACHINEFUNCTIONINFO_H
#define AVM2MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm
{

class AVM2MachineFunctionInfo : public MachineFunctionInfo
{
private:
    unsigned GlobalBaseReg;

    /// VarArgsFrameOffset - Frame offset to start of varargs area.
    int VarArgsFrameOffset;

    /// SRetReturnReg - Holds the virtual register into which the sret
    /// argument is passed.
    unsigned SRetReturnReg;
public:
    AVM2MachineFunctionInfo()
        : GlobalBaseReg(0), VarArgsFrameOffset(0), SRetReturnReg(0) {}
    explicit AVM2MachineFunctionInfo(MachineFunction &MF)
        : GlobalBaseReg(0), VarArgsFrameOffset(0), SRetReturnReg(0) {}

    unsigned getGlobalBaseReg() const {
        return GlobalBaseReg;
    }
    void setGlobalBaseReg(unsigned Reg) {
        GlobalBaseReg = Reg;
    }

    int getVarArgsFrameOffset() const {
        return VarArgsFrameOffset;
    }
    void setVarArgsFrameOffset(int Offset) {
        VarArgsFrameOffset = Offset;
    }

    unsigned getSRetReturnReg() const {
        return SRetReturnReg;
    }
    void setSRetReturnReg(unsigned Reg) {
        SRetReturnReg = Reg;
    }
};
}

#endif
