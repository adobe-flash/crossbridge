//===- Phi2Mem.cpp - Convert registers to allocas -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file demotes all phis to memory references.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "phi2mem"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CFG.h"
#include <list>
using namespace llvm;

STATISTIC(NumPhisDemoted, "Number of phi-nodes demoted");

namespace {
  struct PhiToMem : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    PhiToMem() : FunctionPass(ID) {
      initializePhiToMemPass(*PassRegistry::getPassRegistry());
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.addRequiredID(BreakCriticalEdgesID);
      AU.addPreservedID(BreakCriticalEdgesID);
    }

    virtual bool runOnFunction(Function &F);
  };
}
  
char PhiToMem::ID = 0;
INITIALIZE_PASS_BEGIN(PhiToMem, "phi2mem", "Demote all phi-nodes to stack slots",
                false, false)
INITIALIZE_PASS_DEPENDENCY(BreakCriticalEdges)
INITIALIZE_PASS_END(PhiToMem, "phi2mem", "Demote all phi-nodes to stack slots",
                false, false)

bool PhiToMem::runOnFunction(Function &F) {
  if (F.isDeclaration()) 
    return false;
  
  // Insert all new allocas into entry block.
  BasicBlock *BBEntry = &F.getEntryBlock();
  assert(pred_begin(BBEntry) == pred_end(BBEntry) &&
         "Entry block to function must not have predecessors!");
  
  // Find first non-alloca instruction and create insertion point. This is
  // safe if block is well-formed: it always have terminator, otherwise
  // we'll get and assertion.
  BasicBlock::iterator I = BBEntry->begin();
  while (isa<AllocaInst>(I)) ++I;
  
  CastInst *AllocaInsertionPoint =
    new BitCastInst(Constant::getNullValue(Type::getInt32Ty(F.getContext())),
                    Type::getInt32Ty(F.getContext()),
                    "phi2mem alloca point", I);
  
  // Find all phi's
  std::list<Instruction*> WorkList;
  
  for (Function::iterator ibb = F.begin(), ibe = F.end();
       ibb != ibe; ++ibb)
    for (BasicBlock::iterator iib = ibb->begin(), iie = ibb->end();
         iib != iie; ++iib)
      if (isa<PHINode>(iib))
        WorkList.push_front(&*iib);
  
  // Demote phi nodes
  NumPhisDemoted += WorkList.size();
  for (std::list<Instruction*>::iterator ilb = WorkList.begin(), 
       ile = WorkList.end(); ilb != ile; ++ilb)
    DemotePHIToStack(cast<PHINode>(*ilb), AllocaInsertionPoint);
  
  return true;
}


// createDemotePhiToMemory - Provide an entry point to create this pass.
//
char &llvm::DemotePhiToMemoryID = PhiToMem::ID;
FunctionPass *llvm::createDemotePhiToMemoryPass() {
  return new PhiToMem();
}
