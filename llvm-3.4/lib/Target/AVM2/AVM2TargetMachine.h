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
//===-- AVM2TargetMachine.h - Define TargetMachine for AVM2 ---*- C++ -*-===//
//
// This file declares the AVM2 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef AVM2TARGETMACHINE_H
#define AVM2TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "AVM2InstrInfo.h"
#include "AVM2Subtarget.h"
#include "AVM2ISelLowering.h"
#include "AVM2FrameLowering.h"
#include "AVM2SelectionDAGInfo.h"

namespace llvm
{

class Module;
class AVM2TargetLowering;

class AVM2TargetMachine : public LLVMTargetMachine
{
    const DataLayout DL;       // Calculates type size & alignment
    AVM2Subtarget Subtarget;
    AVM2InstrInfo InstrInfo;
    AVM2TargetLowering TLInfo;
    AVM2SelectionDAGInfo TSInfo;
    AVM2FrameLowering FrameLowering;

public:
    AVM2TargetMachine(const llvm::Target& T, StringRef TT, 
                      StringRef CPU, StringRef FS, 
                      const TargetOptions &Options, Reloc::Model RM,
                      CodeModel::Model CM, CodeGenOpt::Level OL);

    virtual const AVM2InstrInfo *getInstrInfo() const {
        return &InstrInfo;
    }
    virtual const TargetFrameLowering  *getFrameLowering() const {
        return &FrameLowering;
    }
    virtual const AVM2Subtarget  *getSubtargetImpl() const {
        return &Subtarget;
    }
    virtual AVM2TargetLowering* getTargetLowering() const {
        return const_cast<AVM2TargetLowering*>(&TLInfo);
    }
    virtual const AVM2SelectionDAGInfo* getSelectionDAGInfo() const {
        return &TSInfo;
    }
    virtual const TargetRegisterInfo *getRegisterInfo() const {
        return &InstrInfo.getRegisterInfo();
    }
    virtual const DataLayout       *getDataLayout() const {
        return &DL;
    }
    static unsigned getModuleMatchQuality(const Module &M);

    /*  virtual bool addPassesToEmitFile(PassManagerBase &PM, formatted_raw_ostream &Out, CodeGenFileType FileType, CodeGenOpt::Level OptLevel, bool DisableVerify)
      {
        if(FileType != CGFT_AssemblyFile)
          return true;
    fprintf(stderr, "hi!\n");
        return false;
      }
    */ // xxx

    // Pass Pipeline Configuration
    virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
    virtual bool addAssemblyEmitter(PassManagerBase &PM, llvm::CodeGenOpt::Level level,
                                    std::ostream &Out);
};

} // end namespace llvm

#endif
