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

#include "AVM2TargetAsmInfo.h"
#include "AVM2TargetMachine.h"
#include "AVM2.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/CodeGen/LiveVariables.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#include <set>

using namespace llvm;
extern Target llvm::TheAVM2Target;

namespace llvm
{
MCStreamer *createAVM2AsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
                                  bool isVerboseAsm, bool useLoc,
                                  MCInstPrinter *InstPrint,
                                  MCCodeEmitter *CE,
                                  TargetAsmBackend *TAB,
                                  bool ShowInst);

MCStreamer *createAVM2ABCStreamer(const Target &T, const std::string &TT,
                                  MCContext &Context, TargetAsmBackend &TAB,
                                  raw_ostream &OS,
                                  MCCodeEmitter *_Emitter,
                                  bool RelaxAll,
                                  bool NoExecStack);
}

extern "C" void LLVMInitializeAVM2Target()
{
    // Register the target.
    RegisterTargetMachine<AVM2TargetMachine> X(TheAVM2Target);
    // Register the target asm info.
    RegisterAsmInfo<AVM2TargetAsmInfo> A(TheAVM2Target);
    TargetRegistry::RegisterCodeEmitter(TheAVM2Target, createAVM2MCCodeEmitter);
    TargetRegistry::RegisterAsmBackend(TheAVM2Target, createAVM2AsmBackend);
    TargetRegistry::RegisterAsmStreamer(TheAVM2Target, createAVM2AsmStreamer);
    TargetRegistry::RegisterObjectStreamer(TheAVM2Target, createAVM2ABCStreamer);
}

/// AVM2TargetMachine ctor
///
AVM2TargetMachine::AVM2TargetMachine(const llvm::Target& T, const std::string& TT, const std::string& FS)
    : LLVMTargetMachine( T, TT),
      DataLayout("e-p:32:32-f64:32:64-i64:32:64"),		// original 2.1 data layout string.
      Subtarget(T, TT, FS),
      InstrInfo(Subtarget),
      TLInfo(*this),
      TSInfo(*this),
      FrameLowering(*this, Subtarget)
{
    EnableRegAlloc = false;
    setRelocationModel(Reloc::Default); // TODO: do we need to handle other relocation modes?
}

bool AVM2TargetMachine::addInstSelector(llvm::PassManagerBase& PM, llvm::CodeGenOpt::Level OptLevel)
{
    PM.add(createAVM2ISelDag(*this, OptLevel)); // real instruction selector
    return false;
}

/// addPreEmitPass - This pass may be implemented by targets that want to run
/// passes immediately before machine code is emitted.  This should return
/// true if -print-machineinstrs should print out the code after the passes.
bool AVM2TargetMachine::addPreEmitPass(PassManagerBase &PM, llvm::CodeGenOpt::Level level)
{
    return true;
}

bool AVM2TargetMachine::addAssemblyEmitter(PassManagerBase &PM, llvm::CodeGenOpt::Level,
        std::ostream &Out)
{
    // Output assembly language.
    // Now done in LLVMInitializeAVM2AsmPrinter, AVM2AsmPrinter.cpp:
    // PM.add(createAVM2CodePrinterPass(formatted_raw_ostream(Out), *this));
    return false;
}
