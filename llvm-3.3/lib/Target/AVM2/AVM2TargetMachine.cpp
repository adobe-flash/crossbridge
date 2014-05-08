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
//===-- AVM2TargetMachine.cpp - Define TargetMachine for AVM2 -----------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#include "AVM2TargetAsmInfo.h"
#include "AVM2TargetMachine.h"
#include "AVM2.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/CodeGen/LiveVariables.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#include <set>

using namespace llvm;
extern Target llvm::TheAVM2Target;

namespace llvm
{
MCStreamer *createAVM2AsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
                                  bool isVerboseAsm, bool useLoc, bool useCFI,
                                  bool useDwarfDirectory,
                                  MCInstPrinter *InstPrint,
                                  MCCodeEmitter *CE,
                                  MCAsmBackend *TAB,
                                  bool ShowInst);

MCStreamer *createAVM2ABCStreamer(const Target &T, StringRef TT,
                                  MCContext &Context, MCAsmBackend &TAB,
                                  raw_ostream &OS,
                                  MCCodeEmitter *_Emitter,
                                  bool RelaxAll,
                                  bool NoExecStack);
}

static MCAsmInfo *createAVM2MCAsmInfo(const Target &T, StringRef TT) {
    MCAsmInfo *MAI = new AVM2TargetAsmInfo(T, TT);
    return MAI;
}

extern "C" void LLVMInitializeAVM2Target()
{
    // Register the target.
    RegisterTargetMachine<AVM2TargetMachine> X(TheAVM2Target);
}

extern "C" void LLVMInitializeAVM2TargetMC()
{
    // Register the target asm info.
    RegisterMCAsmInfoFn A(TheAVM2Target, createAVM2MCAsmInfo);
    TargetRegistry::RegisterMCCodeEmitter(TheAVM2Target, createAVM2MCCodeEmitter);
    TargetRegistry::RegisterMCAsmBackend(TheAVM2Target, createAVM2AsmBackend);
    TargetRegistry::RegisterAsmStreamer(TheAVM2Target, createAVM2AsmStreamer);
    TargetRegistry::RegisterMCObjectStreamer(TheAVM2Target, createAVM2ABCStreamer);
}

/// AVM2TargetMachine ctor
///
AVM2TargetMachine::AVM2TargetMachine(const llvm::Target& T, StringRef TT, 
                      StringRef CPU, StringRef FS, 
                      const TargetOptions &Options, Reloc::Model RM,
                      CodeModel::Model CM, CodeGenOpt::Level OL)
    : LLVMTargetMachine( T, TT, CPU, FS, Options, RM, CM, OL),
      DL("e-p:32:32-f64:32:64-i64:32:64-i32:32:32"),		// original 2.1 data layout string.
      Subtarget(TT, CPU, FS),
      InstrInfo(Subtarget),
      TLInfo(*this),
      TSInfo(*this),
      FrameLowering(*this, Subtarget)
{}

//===----------------------------------------------------------------------===//
// Pass Pipeline Configuration
//===----------------------------------------------------------------------===//

namespace {
/// AVM2 Code Generator Pass Configuration Options.
class AVM2PassConfig : public TargetPassConfig {
private:
    bool EnableRegAlloc;
public:
  AVM2PassConfig(AVM2TargetMachine *TM, PassManagerBase &PM, bool ERA)
    : TargetPassConfig(TM, PM), EnableRegAlloc(ERA) {
  
    if (!ERA) {
      disablePass(&PostRASchedulerID);
      disablePass(&BranchFolderPassID);
      disablePass(&TailDuplicateID);
      disablePass(&MachineBlockPlacementID);
      // TODO: Implement or Deprecated ? VPMedia
      //disablePass(&CodePlacementOptID);
      disablePass(&MachineCopyPropagationID);
    } 
  }

  AVM2TargetMachine &getAVM2TargetMachine() const {
    return getTM<AVM2TargetMachine>();
  }

  const AVM2Subtarget &getAVM2Subtarget() const {
    return *getAVM2TargetMachine().getSubtargetImpl();
  }

  virtual bool addInstSelector();
  virtual bool addPreRegAlloc();
  virtual bool addPostRegAlloc();
  virtual bool addPreEmitPass();

  virtual void addFastRegAlloc(FunctionPass *);
  virtual void addOptimizedRegAlloc(FunctionPass *);
};
} // namespace

TargetPassConfig *AVM2TargetMachine::createPassConfig(PassManagerBase &PM) {
  AVM2PassConfig *PC = new AVM2PassConfig(this, PM, false);
  return PC;
}

bool AVM2PassConfig::addInstSelector() {
  // Install an instruction selector.
  addPass(createAVM2ISelDag(getAVM2TargetMachine(), getOptLevel()));
  return false;
}

bool AVM2PassConfig::addPreRegAlloc() {
  return false;  // -print-machineinstr shouldn't print after this.
}

bool AVM2PassConfig::addPostRegAlloc() {
  return true;  // -print-machineinstr should print after this.
}

void AVM2PassConfig::addFastRegAlloc(FunctionPass *FP) {
  if (EnableRegAlloc) {
    TargetPassConfig::addFastRegAlloc(FP);
  }
}

void AVM2PassConfig::addOptimizedRegAlloc(FunctionPass *RegAllocPass) {
  printAndVerify("beginning of addOptimizedRegAlloc");
  //if (EnableRegAlloc) {
    //TargetPassConfig::addOptimizedRegAlloc(FP);
  //}
  addPass(&ProcessImplicitDefsID);

  // LiveVariables currently requires pure SSA form.
  //
  // FIXME: Once TwoAddressInstruction pass no longer uses kill flags,
  // LiveVariables can be removed completely, and LiveIntervals can be directly
  // computed. (We still either need to regenerate kill flags after regalloc, or
  // preferably fix the scavenger to not depend on them).
  addPass(&LiveVariablesID);
  addPass(&LiveIntervalsID);

  // Add passes that move from transformed SSA into conventional SSA. This is a
  // "copy coalescing" problem.
  //
#if 0
  if (!EnableStrongPHIElim) {
    // Edge splitting is smarter with machine loop info.
    addPass(&MachineLoopInfoID);
    addPass(&PHIEliminationID);
  }
#endif

  // Eventually, we want to run LiveIntervals before PHI elimination.
#if 0
  if (EarlyLiveIntervals)
    addPass(&LiveIntervalsID);
#endif

  //addPass(&TwoAddressInstructionPassID);

#if 0
  if (EnableStrongPHIElim)
    addPass(&StrongPHIEliminationID);
#endif

  //addPass(&RegisterCoalescerID);

  // PreRA instruction scheduling.
  if (addPass(&MachineSchedulerID))
    printAndVerify("After Machine Scheduling");

  // Add the selected register allocation pass.
  printAndVerify("before Register Allocation");
  if (EnableRegAlloc) {
    addPass(RegAllocPass);
    printAndVerify("after Register Allocation");
  } else {
    addPass(createVirtRegReductionPass());
    printAndVerify("after Register reduction");
  }
  printAndVerify("After Register Allocation, before rewriter");

  // Allow targets to change the register assignments before rewriting.
  if (addPreRewrite())
    printAndVerify("After pre-rewrite passes");

  // Finally rewrite virtual registers.
  //addPass(&VirtRegRewriterID);
  printAndVerify("After Virtual Register Rewriter");

  // FinalizeRegAlloc is convenient until MachineInstrBundles is more mature,
  // but eventually, all users of it should probably be moved to addPostRA and
  // it can go away.  Currently, it's the intended place for targets to run
  // FinalizeMachineBundles, because passes other than MachineScheduling an
  // RegAlloc itself may not be aware of bundles.
  // TODO: Implement or Deprecated ? - VPmedia
  //if (addFinalizeRegAlloc())
  //  printAndVerify("After RegAlloc finalization");

  // Perform stack slot coloring and post-ra machine LICM.
  //
  // FIXME: Re-enable coloring with register when it's capable of adding
  // kill markers.
  if (EnableRegAlloc)
    addPass(&StackSlotColoringID);

  // Run post-ra machine LICM to hoist reloads / remats.
  //
  // FIXME: can this move into MachineLateOptimization?
  if (EnableRegAlloc)
    addPass(&PostRAMachineLICMID);

  printAndVerify("After StackSlotColoring and postra Machine LICM");
}

/// addPreEmitPass - This pass may be implemented by targets that want to run
/// passes immediately before machine code is emitted.  This should return
/// true if -print-machineinstrs should print out the code after the passes.
bool AVM2PassConfig::addPreEmitPass() {
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

