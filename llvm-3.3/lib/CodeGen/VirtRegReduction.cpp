//===-- VirtRegReduction.cpp - Virtual register reduction -----------===//

#include "llvm/IR/Function.h"
#include "llvm/CodeGen/LiveIntervalAnalysis.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "RegisterCoalescer.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetMachine.h"

#include "ConflictGraph.h"

#define VRRPROF 0
#if VRRPROF
#include <time.h>
#include <stdarg.h>

static void llog(const char *fmt, ...)
{
  static FILE *sLog = NULL;

  if(!sLog)
    sLog = fopen("/tmp/vrr.log", "w");

  char buf[4096];
  va_list ap;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);
  fprintf(sLog, "%s", buf);
  fflush(sLog);
}
#endif

#include <memory>

// To avoid problem with non-clang compilers not having this macro.
#if defined(__has_include)
	#if __has_include(<unordered_map>)
		#include <unordered_map>
		#include <unordered_set>
	#else
		#include <tr1/unordered_map>
		#include <tr1/unordered_set>
		namespace std {
			using tr1::unordered_map;
			using tr1::unordered_set;
		}
	#endif
#else
	#include <tr1/unordered_map>
	#include <tr1/unordered_set>
	namespace std {
		using tr1::unordered_map;
		using tr1::unordered_set;
	}
#endif

using namespace llvm;

namespace {
  struct VirtRegReduction : public MachineFunctionPass {

    static char ID;

    VirtRegReduction () : MachineFunctionPass(ID) {
      initializeLiveIntervalsPass(*PassRegistry::getPassRegistry());
      initializeStrongPHIEliminationPass(*PassRegistry::getPassRegistry());
      initializeRegisterCoalescerPass(*PassRegistry::getPassRegistry());
    }

    virtual const char* getPassName() const {
      return "Virtual Register Reduction";
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesCFG();
      AU.addRequired<LiveIntervals>();
      AU.addRequiredID(StrongPHIEliminationID);
      // Make sure PassManager knows which analyses to make available
      // to coalescing and which analyses coalescing invalidates.
      //AU.addRequiredTransitive<RegisterCoalescer>();
      MachineFunctionPass::getAnalysisUsage(AU);
    }

    /// runOnMachineFunction - register reduce the whole function
    bool runOnMachineFunction(MachineFunction&);
  };
}

char VirtRegReduction::ID = 0;

INITIALIZE_PASS_BEGIN(VirtRegReduction, "virtregreduction",
                "Virtual Register Reduction", false, false)
INITIALIZE_PASS_DEPENDENCY(LiveIntervals)
INITIALIZE_PASS_DEPENDENCY(StrongPHIElimination)
//INITIALIZE_AG_DEPENDENCY(RegisterCoalescer)
INITIALIZE_PASS_END(VirtRegReduction, "virtregreduction",
                "Virtual Register Reduction", false, false)

bool VirtRegReduction::runOnMachineFunction(MachineFunction &MF)
{
  bool Changed = false;

#if VRRPROF
  const Function *F = MF.getFunction();
  std::string FN = F->getName().str();
  llog("starting vrr... %s (%d)\n", FN.c_str(), (int)time(NULL));
  llog("starting immRegs finder... (%d)\n", (int)time(NULL));
#endif
  std::auto_ptr<std::unordered_set<unsigned> > immRegsHolder;
  std::unordered_set<unsigned> *immRegs = NULL;
  
  // single-def regs defined by a MoveImm shouldn't coalesce as we may be
  // able to fold them later
  {
    std::unordered_map<unsigned, const MachineInstr *> singleDef;

    MachineFunction::const_iterator I = MF.begin(), E = MF.end();

    // find all registers w/ a single def
    for(; I != E; I++)
    {
      MachineBasicBlock::const_iterator BI = I->begin(), BE = I->end();

     for(; BI != BE; BI++)
     {
       MachineInstr::const_mop_iterator II, IE;
       II = BI->operands_begin();
       IE = BI->operands_end();
       for(; II != IE; II++)
         if(II->isReg() && II->isDef())
         {
           unsigned R = II->getReg();
           std::unordered_map<unsigned, const MachineInstr *>::iterator SI = singleDef.find(R);

           if(SI == singleDef.end())
             singleDef[R] = BI; // first seen! insert
           else
             SI->second = NULL; // second seen -- replace w/ NULL
         }
      }
    }

    std::unordered_map<unsigned, const MachineInstr *>::const_iterator SI = singleDef.begin(), SE = singleDef.end();

    for(; SI != SE; SI++)
    {
      if(SI->second && SI->second->getDesc().isMoveImmediate()) // single def imm?
      {
        if(!immRegs)
          immRegsHolder.reset(immRegs = new std::unordered_set<unsigned>);
        immRegs->insert(SI->first); // don't coalesce
      }
    }
  }

#if VRRPROF
  llog("starting tdkRegs finder... (%d)\n", (int)time(NULL));
#endif

  std::auto_ptr<std::unordered_set<unsigned> > tdkRegsHolder;
  std::unordered_set<unsigned> *tdkRegs = NULL;
  
  bool setjmpSafe = !MF.exposesReturnsTwice() && 
        MF.getFunction()->doesNotThrow();

  {
    tdkRegsHolder.reset(tdkRegs = new std::unordered_set<unsigned>);

    std::unordered_map<unsigned, unsigned> trivialDefKills;

    MachineFunction::const_iterator I = MF.begin(), E = MF.end();

    // find all registers defed and killed in the same block w/ no intervening
    // unsafe (due to setjmp) calls + side-effecty operations
    for(; I != E; I++)
    {
      std::unordered_set<unsigned> defs;

      MachineBasicBlock::const_iterator BI = I->begin(), BE = I->end();

     for(; BI != BE; BI++)
     {
       // TODO need to add || BI->getDesc().isInlineAsm() here to help stackification?
       if((!setjmpSafe && BI->getDesc().isCall()) || BI->getDesc().hasUnmodeledSideEffects()) { 
         // invalidate on a call instruction if setjmp present, or instr with side effects regardless
         defs.clear();
       }

       MachineInstr::const_mop_iterator II, IE;
     
       // uses when we're not tracking a reg it make it unsafe
       II = BI->operands_begin();
       IE = BI->operands_end();
       for(; II != IE; II++)
         if(II->isReg() && II->isUse())
         {
           unsigned R = II->getReg();
           std::unordered_set<unsigned>::const_iterator DI = defs.find(R);

           if(DI == defs.end())
             trivialDefKills[R] = 100;
         }
       // kills of tracked defs are trivial def/kills
       II = BI->operands_begin();
       IE = BI->operands_end();
       for(; II != IE; II++)
         if(II->isReg() && II->isKill())
         {
           unsigned R = II->getReg();
           std::unordered_set<unsigned>::const_iterator DI = defs.find(R);

           if(DI != defs.end())
           {
             defs.erase(DI);
             trivialDefKills[R]++;
           }
           else
             trivialDefKills[R] = 100; // don't use
         }
       // record all defs in this instruction
       II = BI->operands_begin();
       IE = BI->operands_end();
       for(; II != IE; II++)
         if(II->isReg() && II->isDef())
           defs.insert(II->getReg());
      }
    }

    std::unordered_map<unsigned, unsigned>::const_iterator DKI = trivialDefKills.begin(),
        DKE = trivialDefKills.end();

    for(; DKI != DKE; DKI++)
      if(DKI->second == 1)
        tdkRegs->insert(DKI->first);
  }

#if VRRPROF
  llog("starting conflict graph construction... (%d)\n", (int)time(NULL));
#endif

  std::unordered_set<unsigned>::const_iterator tdkE = tdkRegs->end();

  std::unordered_set<unsigned> *okRegs = NULL;

  if(!setjmpSafe)
    okRegs = tdkRegs;

  MachineRegisterInfo *RI = &(MF.getRegInfo());
  // will eventually hold a virt register coloring for this function
  ConflictGraph::Coloring coloring;

  {
    ConflictGraph cg;
    LiveIntervals &LIS = getAnalysis<LiveIntervals>();

    // check every possible LiveInterval, LiveInterval pair of the same
    // register class for overlap and add overlaps to the conflict graph
    // also, treat trivially def-kill-ed regs and not trivially def-kill-ed
    // regs as conflicting so they end up using different VRs -- this makes
    // stackification easier later in the toolchain
    for (unsigned i = 0, e = RI->getNumVirtRegs(); i != e; ++i) {
      unsigned R = TargetRegisterInfo::index2VirtReg(i);
      if (RI->reg_nodbg_empty(R))
        continue;
      const LiveInterval LI = LIS.getInterval(R); 

      if(TargetRegisterInfo::isPhysicalRegister(R))
        continue;
      if(okRegs && okRegs->find(R) == okRegs->end())
        continue;
      // leave singly-defined MoveImm regs for later coalescing
      if(immRegs && immRegs->find(R) != immRegs->end())
        continue;

//      const TargetRegisterClass *RC = RI->getRegClass(R);
      if(LI.empty())
        continue;

      cg.addVertex(R);

      bool notTDK = tdkRegs->find(R) == tdkE;

      for (unsigned j = 0; j != e; ++j) {
        unsigned R1 = TargetRegisterInfo::index2VirtReg(j);
        if (RI->reg_nodbg_empty(R1) || i == j)
          continue;

        if(TargetRegisterInfo::isPhysicalRegister(R1))
          continue;
        if(okRegs && okRegs->find(R1) == okRegs->end())
          continue;
        // leave singly-defined MoveImm regs for later coalescing
        if(immRegs && immRegs->find(R1) != immRegs->end())
          continue;

/* Don't bother checked RC -- even though it sounds like an opt, it doesn't speed us up in practice
        const TargetRegisterClass *RC1 = RI->getRegClass(R1);

        if(RC != RC1)
          continue; // different reg class... won't conflict
*/
        const LiveInterval LI1 = LIS.getInterval(R1); 

        // conflict if intervals overlap OR they're not both TDK or both NOT TDK
        if(LI.overlaps(LI1) || notTDK != (tdkRegs->find(R1) == tdkE))
          cg.addEdge(R, R1);
      }
    }

#if VRRPROF
  llog("starting coloring... (%d)\n", (int)time(NULL));
#endif

    cg.color(&coloring);

#if VRRPROF
  llog("starting vreg=>vreg construction... (%d)\n", (int)time(NULL));
#endif

	typedef std::unordered_map<unsigned, unsigned> VRegMap;
	VRegMap Regs;

	// build up map of vreg=>vreg
	{
		std::unordered_map<const TargetRegisterClass *, std::unordered_map<unsigned, unsigned> > RCColor2VReg;

		ConflictGraph::Coloring::const_iterator I = coloring.begin(), E = coloring.end();

		for(; I != E; I++)
		{
			unsigned R = I->first;
			unsigned Color = I->second;
			const TargetRegisterClass *RC = RI->getRegClass(R);
			std::unordered_map<unsigned, unsigned> &Color2VReg = RCColor2VReg[RC];

			VRegMap::const_iterator CI = Color2VReg.find(Color);

			if(CI != Color2VReg.end())
				Regs[R] = CI->second; // seen this color; map it
			else
				Regs[R] = Color2VReg[Color] = R; // first sighting of color; bind to this reg
		}
	}

#if VRRPROF
  llog("starting remap... (%d)\n", (int)time(NULL));
#endif


	// remap regs
	{
		VRegMap::const_iterator I = Regs.begin(), E = Regs.end();

		for(; I != E; I++)
			if(I->first != I->second)
			{
				RI->replaceRegWith(I->first, I->second);
				Changed = true;
			}
	}
  }

#if VRRPROF
  llog("done... (%d)\n", (int)time(NULL));
#endif

  return Changed;
}

FunctionPass *llvm::createVirtRegReductionPass()
{
  return new VirtRegReduction();
}

