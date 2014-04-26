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

#ifndef AVM2TARGETMACHINE_H
#define AVM2TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "AVM2ShimSubtarget.h"
#include "AVM2ShimFrameLowering.h"

namespace llvm
{

class Module;

    
    class AVM2ShimTargetLowering : public TargetLowering
    {
        const AVM2ShimSubtarget *SubTarget;
    public:
        AVM2ShimTargetLowering(TargetMachine &TM);
        virtual unsigned int getFunctionAlignment(const llvm::Function*) const;
    };

class AVM2ShimTargetMachine : public LLVMTargetMachine
{
    const TargetData DataLayout;       // Calculates type size & alignment
    AVM2ShimSubtarget Subtarget;
    AVM2ShimTargetLowering TLInfo;
    AVM2ShimFrameLowering FrameLowering;

public:
    AVM2ShimTargetMachine(const llvm::Target& T, const std::string& TT, const std::string& FS);

    virtual const TargetFrameLowering  *getFrameLowering() const {
        return &FrameLowering;
    }
    virtual AVM2ShimTargetLowering* getTargetLowering() const {
        return const_cast<AVM2ShimTargetLowering*>(&TLInfo);
    }
    virtual const TargetSubtarget  *getSubtargetImpl() const {
        return &Subtarget;
    }
    virtual const TargetData       *getTargetData() const {
        return &DataLayout;
    }
};

} // end namespace llvm

#endif
