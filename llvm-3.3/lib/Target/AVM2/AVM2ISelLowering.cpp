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

//===-- AVM2ISelLowering.cpp - AVM2 DAG Lowering Implementation ---------===//
//
// This file implements the interfaces that AVM2 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#include "AVM2.h"
#include "AVM2TargetMachine.h"
#include "AVM2MachineFunctionInfo.h"
#include "AVM2ISelLowering.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/VariadicFunction.h"

#include <stdio.h>

using namespace llvm;

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "AVM2GenCallingConv.inc"

static TargetLoweringObjectFile *createTLOF(TargetMachine &TM)
{
    return new TargetLoweringObjectFileELF();
}

//===----------------------------------------------------------------------===//
// TargetLowering Implementation
//===----------------------------------------------------------------------===//

AVM2TargetLowering::AVM2TargetLowering(TargetMachine &TM)
    : TargetLowering(TM, createTLOF(TM))
{

    SubTarget = &TM.getSubtarget<AVM2Subtarget>();

    // Set up the register classes.
    addRegisterClass(MVT::i32, &AVM2::IntRegsRegClass);
    addRegisterClass(MVT::f64, &AVM2::FPRegsRegClass);
    addRegisterClass(MVT::f32, &AVM2::SPRegsRegClass);

    setOperationAction(ISD::ADDC, MVT::i32, Expand);
    setOperationAction(ISD::ADDE, MVT::i32, Expand);
    setOperationAction(ISD::SUBC, MVT::i32, Expand);
    setOperationAction(ISD::SUBE, MVT::i32, Expand);

    // AVM2 doesn't have sext_inreg, replace them with shl/sra
    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i64, Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8 , Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);

    setOperationAction(ISD::MULHU, MVT::i64, Expand);
    setOperationAction(ISD::MULHS, MVT::i64, Expand);
    setOperationAction(ISD::MULHU, MVT::i32 , Expand);
    setOperationAction(ISD::MULHS, MVT::i32 , Expand);

    setOperationAction(ISD::BITCAST, MVT::f64, Expand);
    setOperationAction(ISD::BITCAST, MVT::f32, Expand);
    setOperationAction(ISD::BITCAST, MVT::i32, Expand);

    setSchedulingPreference(Sched::RegPressure);
    // Turn FP extload into load/fextend
    // setLoadExtAction(ISD::EXTLOAD, MVT::f32, Expand);
    // Sparc doesn't have i1 sign extending load
    // setLoadExtAction(ISD::SEXTLOAD, MVT::i1, Promote);
    // Turn FP truncstore into trunc + store.
    // setTruncStoreAction(MVT::f64, MVT::f32, Expand);

    setOperationAction(ISD::SELECT, MVT::f64, Expand);
    setOperationAction(ISD::SELECT, MVT::f32, Expand);
    setOperationAction(ISD::SELECT, MVT::i64, Expand);
    setOperationAction(ISD::SELECT, MVT::i32, Expand);
    setOperationAction(ISD::SELECT, MVT::i1, Expand);
    setOperationAction(ISD::SETCC, MVT::f64, Expand);
    setOperationAction(ISD::SETCC, MVT::f32, Expand);
    setOperationAction(ISD::SETCC, MVT::i64, Expand);
    setOperationAction(ISD::SETCC, MVT::i32, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::f64, Custom);
    setOperationAction(ISD::SELECT_CC, MVT::f32, Custom);
    // DO NOT INCLUDE, otherwise glob2() in glob.c will break:
    // setOperationAction(ISD::SELECT_CC, MVT::i64, Custom);
    setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);
    
    setOperationAction(ISD::BR_CC, MVT::f64, Custom);
    setOperationAction(ISD::BR_CC, MVT::f32, Custom);
    // DO NOT INCLUDE, otherwise mmap.c will break: setOperationAction(ISD::BR_CC, MVT::i64, Custom);
    setOperationAction(ISD::BR_CC, MVT::i32, Custom);
    setOperationAction(ISD::BRCOND, MVT::Other, Expand);
    setOperationAction(ISD::BRIND, MVT::Other, Expand);
    setOperationAction(ISD::BRIND, MVT::f64, Custom);
    setOperationAction(ISD::BR_JT, MVT::Other, Expand);

    /*
     http://llvm.org/viewvc/llvm-project?view=rev&revision=49572

     Drop ISD::MEMSET, ISD::MEMMOVE, and ISD::MEMCPY, which are not Legal
     on any current target and aren't optimized in DAGCombiner. Instead
     of using intermediate nodes, expand the operations, choosing between
     simple loads/stores, target-specific code, and library calls,
     immediately.

     Previously, the code to emit optimized code for these operations
     was only used at initial SelectionDAG construction time; now it is
     used at all times. This fixes some cases where rep;movs was being
     used for small copies where simple loads/stores would be better.

     This also cleans up code that checks for alignments less than 4;
     let the targets make that decision instead of doing it in
     target-independent code. This allows x86 to use rep;movs in
     low-alignment cases.

     Also, this fixes a bug that resulted in the use of rep;stos for
     memsets of 0 with non-constant memory size when the alignment was
     at least 4. It's better to use the library in this case, which
     can be significantly faster when the size is large.

     This also preserves more SourceValue information when memory
     intrinsics are lowered into simple loads/stores.

     setOperationAction(ISD::MEMMOVE, MVT::Other, Expand);
     setOperationAction(ISD::MEMSET, MVT::Other, Expand);
     setOperationAction(ISD::MEMCPY, MVT::Other, Expand);
    */

    setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);

    // TODO f32 support is very 2nd class
    setOperationAction(ISD::FADD, MVT::f32, Custom);
    setOperationAction(ISD::FSUB, MVT::f32, Custom);
    setOperationAction(ISD::FMUL, MVT::f32, Custom);
    setOperationAction(ISD::FDIV, MVT::f32, Custom);
    setOperationAction(ISD::FREM, MVT::f32, Custom);
    setOperationAction(ISD::FNEG, MVT::f32, Custom);

    setOperationAction(ISD::FSIN, MVT::f32, Custom);
    setOperationAction(ISD::FCOS, MVT::f32, Custom);
    setOperationAction(ISD::FABS, MVT::f32, Custom);
    setOperationAction(ISD::FPOW, MVT::f32, Custom);
    setOperationAction(ISD::FSQRT, MVT::f32, Custom);

    setOperationAction(ISD::FMA, MVT::f32, Expand);
    setOperationAction(ISD::FMA, MVT::f64, Expand);
    // TODO more?
    setOperationAction(ISD::FSIN, MVT::f64, Legal);
    setOperationAction(ISD::FCOS, MVT::f64, Legal);
    setOperationAction(ISD::FABS, MVT::f64, Legal);
    setOperationAction(ISD::FPOW, MVT::f64, Legal);
    setOperationAction(ISD::FSQRT, MVT::f64, Legal);

    setOperationAction(ISD::CTPOP, MVT::i32, Expand);
    setOperationAction(ISD::CTTZ , MVT::i32, Expand);
    setOperationAction(ISD::CTLZ , MVT::i32, Expand);
    setOperationAction(ISD::ROTL , MVT::i32, Expand);
    setOperationAction(ISD::ROTR , MVT::i32, Expand);
    setOperationAction(ISD::BSWAP, MVT::i32, Expand); //YYY
 
    setOperationAction(ISD::CTPOP, MVT::i64, Expand);
    setOperationAction(ISD::CTTZ , MVT::i64, Expand);
    setOperationAction(ISD::CTLZ , MVT::i64, Expand);
    setOperationAction(ISD::ROTL , MVT::i64, Expand);
    setOperationAction(ISD::ROTR , MVT::i64, Expand);
    setOperationAction(ISD::BSWAP, MVT::i64, Expand); //YYY

  
    setOperationAction(ISD::CTTZ_ZERO_UNDEF, MVT::i32, Expand);
    setOperationAction(ISD::CTLZ_ZERO_UNDEF, MVT::i32, Expand);
    setOperationAction(ISD::CTTZ_ZERO_UNDEF, MVT::i64, Expand);
    setOperationAction(ISD::CTLZ_ZERO_UNDEF, MVT::i64, Expand);
  
    setOperationAction(ISD::FCOPYSIGN, MVT::f64, Expand);
    setOperationAction(ISD::FCOPYSIGN, MVT::f32, Expand);

    setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SHL_PARTS, MVT::i64, Expand);
    setOperationAction(ISD::SRA_PARTS, MVT::i64, Expand);
    setOperationAction(ISD::SRL_PARTS, MVT::i64, Expand);

    for(int i=ISD::ATOMIC_LOAD; i <= ISD::ATOMIC_LOAD_UMAX; i++) {
      setOperationAction(i, MVT::i32, Expand);
      setOperationAction(i, MVT::i8, Promote);
      setOperationAction(i, MVT::i16, Promote);
      AddPromotedToType(i, MVT::i8, MVT::i32);
      AddPromotedToType(i, MVT::i16, MVT::i32);
    }
    setOperationAction(ISD::ATOMIC_FENCE, MVT::Other, Expand);

    /*
     http://llvm.org/viewvc/llvm-project?view=rev&revision=78142
     Major calling convention code refactoring.

     Instead of awkwardly encoding calling-convention information with ISD::CALL,
     ISD::FORMAL_ARGUMENTS, ISD::RET, and ISD::ARG_FLAGS nodes, TargetLowering
     provides three virtual functions for targets to override:
     LowerFormalArguments, LowerCall, and LowerRet, which replace the custom
     lowering done on the special nodes. They provide the same information, but
     in a more immediately usable format.

     This also reworks much of the target-independent tail call logic. The
     decision of whether or not to perform a tail call is now cleanly split
     between target-independent portions, and the target dependent portion
     in IsEligibleForTailCallOptimization.

     This also synchronizes all in-tree targets, to help enable future
     refactoring and feature work.

     setOperationAction(ISD::RET               , MVT::Other, Custom);
     */

    // VASTART needs to be custom lowered to use the VarArgsFrameIndex.
    setOperationAction(ISD::VASTART           , MVT::Other, Custom);
    setOperationAction(ISD::VAARG             , MVT::Other, Custom);

    // Use the default implementation.
    setOperationAction(ISD::VACOPY            , MVT::Other, Expand);
    setOperationAction(ISD::VAEND             , MVT::Other, Expand);
    setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32  , Custom); // TODO will "Expand" work for us?
    setOperationAction(ISD::STACKSAVE         , MVT::Other, Expand);
    setOperationAction(ISD::STACKRESTORE      , MVT::Other, Expand);
    setStackPointerRegisterToSaveRestore(AVM2::ESP);

    setOperationAction(ISD::ConstantFP, MVT::f32, Custom);
    setOperationAction(ISD::ConstantFP, MVT::f64, Custom);

    setOperationAction(ISD::BlockAddress, MVT::i32, Custom);
    setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
    setOperationAction(ISD::ExternalSymbol, MVT::i32, Custom);
    setOperationAction(ISD::ConstantPool, MVT::i32, Custom);

    setOperationAction(ISD::EHSELECTION,        MVT::i32,   Expand);
    setOperationAction(ISD::EXCEPTIONADDR,      MVT::i32,   Expand);
    setExceptionPointerRegister(AVM2::EAX);
    setExceptionSelectorRegister(AVM2::EDX);

    setOperationAction(ISD::INIT_TRAMPOLINE, MVT::Other, Custom);
    setOperationAction(ISD::ADJUST_TRAMPOLINE, MVT::Other, Custom);

    setOperationAction(ISD::INTRINSIC_WO_CHAIN, MVT::Other, Custom);
    setOperationAction(ISD::EH_SJLJ_SETJMP, MVT::i32, Custom);
    setOperationAction(ISD::EH_SJLJ_LONGJMP, MVT::Other, Custom);
    setOperationAction(ISD::EH_RETURN, MVT::Other, Custom);
    //TODO remove this and related junk if truly unneeded when except stuff
    // is working!
    //setOperationAction(ISD::EH_SJLJ_DISPATCHSETUP, MVT::Other, Custom);

    // TODO: Makes compiler error - VPMedia (MEMBARRIER is not in ISD anymore)
    //setOperationAction(ISD::MEMBARRIER,        MVT::Other,   Expand);

    for(unsigned int i=0; i<RTLIB::UNKNOWN_LIBCALL; i++) {
      const char *nm = getLibcallName((RTLIB::Libcall)i);
      if(nm && !strncmp(nm, "__sync", 6))
        setLibcallName((RTLIB::Libcall)i, nm+1);
    }
    setLibcallName(RTLIB::UNWIND_RESUME, "_Unwind_SjLj_Resume");
    computeRegisterProperties();
}

const char *AVM2TargetLowering::getTargetNodeName(unsigned Opcode) const
{
    switch (Opcode) {
    default:
        return 0;
    case AVM2ISD::CBR:
        return "AVM2ISD::CBR - Conditional branch wrapper";
    case AVM2ISD::FCBR:
        return "AVM2ISD::FCBR - Float conditional branch wrapper";
    case AVM2ISD::INOP:
        return "AVM2ISD::INOP - Integer operation wrapper";
    case AVM2ISD::FNOP:
        return "AVM2ISD::FNOP - Float operation wrapper";
    case AVM2ISD::SL:
        return "AVM2ISD::SL - Select wrapper";
    case AVM2ISD::FSL:
        return "AVM2ISD::FSL - Float select wrapper";
    case AVM2ISD::CNOP:
        return "AVM2ISD::CNOP - Condition wrapper";
    case AVM2ISD::FCNOP:
        return "AVM2ISD::FCNOP - Float condition wrapper";
    case AVM2ISD::F64:
        return "AVM2ISD::F64 - Float immediate wrapper";
    case AVM2ISD::CALL:
        return "AVM2ISD::CALL - A call instruction";
    case AVM2ISD::RET_FLAG:
        return "AVM2ISD::RET_FLAG - Return with a flag operand";
    case AVM2ISD::EH_SJLJ_SETJMP:
        return "AVM2ISD::EH_SJLJ_SETJMP - SjLj exception handling setjmp";
    case AVM2ISD::EH_SJLJ_LONGJMP:
        return "AVM2ISD::EH_SJLJ_LONGJMP - SjLj exception handling longjmp";
    case AVM2ISD::EH_SJLJ_DISPATCHSETUP :
        return "AVM2ISD::EH_SJLJ_DISPATCHSETUP - SjLj exception handling dispatch setup";
    }
}

MachineBasicBlock *
AVM2TargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
        MachineBasicBlock *BB) const
{
  const TargetMachine &TM = getTargetMachine();
  const TargetInstrInfo &TII = *TM.getInstrInfo();
  unsigned BROpcode;
  DebugLoc dl = MI->getDebugLoc();
  bool useIntrin = TM.getSubtarget<AVM2Subtarget>().useIntrinsics();
  // Figure out the conditional branch opcode to use for this select_cc.
  switch (MI->getOpcode()) {
  default: llvm_unreachable("Unknown SELECT_CC!");
  case AVM2::SL:
  case AVM2::SLF:
    BROpcode = useIntrin ? AVM2::inCBR : AVM2::asCBR;
    break;
  case AVM2::FSL:
  case AVM2::FSLF:
    BROpcode = useIntrin ? AVM2::inFCBR : AVM2::asFCBR;
    break;
  }
  // To "insert" a SELECT_CC instruction, we actually have to insert the diamond
  // control-flow pattern.  The incoming instruction knows the destination vreg
  // to set, the condition code register to branch on, the true/false values to
  // select between, and a branch opcode to use.
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator It = BB;
  ++It;

  //  thisMBB:
  //  ...
  //   TrueVal = ...
  //   [f]bCC copy1MBB
  //   fallthrough --> copy0MBB
  MachineBasicBlock *thisMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *sinkMBB = F->CreateMachineBasicBlock(LLVM_BB);
  F->insert(It, copy0MBB);
  F->insert(It, sinkMBB);

  // Transfer the remainder of BB and its successor edges to sinkMBB.
  sinkMBB->splice(sinkMBB->begin(), BB,
                  llvm::next(MachineBasicBlock::iterator(MI)),
                  BB->end());
  sinkMBB->transferSuccessorsAndUpdatePHIs(BB);

  // Add the true and fallthrough blocks as its successors.
  BB->addSuccessor(copy0MBB);
  BB->addSuccessor(sinkMBB);

  BuildMI(BB, dl, TII.get(BROpcode))
    .addMBB(sinkMBB)
    .addImm(MI->getOperand(1).getImm()) // CC
    .addReg(MI->getOperand(2).getReg())
    .addReg(MI->getOperand(3).getReg());

  //  copy0MBB:
  //   %FalseValue = ...
  //   # fallthrough to sinkMBB
  BB = copy0MBB;

  // Update machine-CFG edges
  BB->addSuccessor(sinkMBB);

  //  sinkMBB:
  //   %Result = phi [ %FalseValue, copy0MBB ], [ %TrueValue, thisMBB ]
  //  ...
  BB = sinkMBB;
  BuildMI(*BB, BB->begin(), dl, TII.get(AVM2::PHI), MI->getOperand(0).getReg())
    .addReg(MI->getOperand(5).getReg()).addMBB(copy0MBB)
    .addReg(MI->getOperand(4).getReg()).addMBB(thisMBB);

  MI->eraseFromParent();   // The pseudo instruction is gone now.
  return BB;

}

SDValue
AVM2TargetLowering::LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const
{
  const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();
  SDValue Result = DAG.getBlockAddress(BA, getPointerTy(),
                                       /*isTarget=*/true, 0);
  return Result;
}

SDValue
AVM2TargetLowering::LowerBRIND(SDValue Op, SelectionDAG &DAG) const
{
    report_fatal_error("LLVM indirect branch instructions are not supported in flascc.\n");
}

SDValue AVM2TargetLowering::LowerTRAMPOLINE(SDValue Op,
                                             SelectionDAG &DAG) const
{
    report_fatal_error("LLVM Trampoline intrinsics are not supported in flascc.\n");
}

SDValue AVM2TargetLowering::
LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
    DebugLoc DL = Op.getDebugLoc();
    switch (Op.getOpcode()) {
    default: {
        assert(0 && "Shouldn't custom lower this");
    }
    // XXX TODO isn't this just Promote?
    case ISD::FADD:
    case ISD::FSUB:
    case ISD::FPOW:
    case ISD::FMUL:
    case ISD::FDIV:
    case ISD::FREM: {
        SDValue L = Op.getOperand(0);
        SDValue R = Op.getOperand(1);

        assert(L.getValueType() == R.getValueType() && L.getValueType() == MVT::f32);

        L = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, L);
        R = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, R);

        SDValue result = DAG.getNode(Op.getOpcode(), DL, MVT::f64, L, R);

        return DAG.getNode(ISD::FP_ROUND, DL, MVT::f32, result, DAG.getIntPtrConstant(0));
    }
    // XXX TODO isn't this just Promote?
    case ISD::FSIN:
    case ISD::FCOS:
    case ISD::FSQRT:
    case ISD::FNEG:
    case ISD::FABS: {
        SDValue L = Op.getOperand(0);

        assert(L.getValueType() == MVT::f32);

        L = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, L);

        SDValue result = DAG.getNode(Op.getOpcode(), DL, MVT::f64, L);

        return DAG.getNode(ISD::FP_ROUND, DL, MVT::f32, result, DAG.getIntPtrConstant(0));
    }
    case ISD::SELECT_CC: {
        ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
        SDValue L = Op.getOperand(0);
        SDValue R = Op.getOperand(1);
        SDValue A = Op.getOperand(2);
        SDValue B = Op.getOperand(3);

        // promote everything that is an f32
        if(L.getValueType() == MVT::f32) {
            L = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, L);
            R = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, R);
        }
        if(A.getValueType() == MVT::f32) {
            A = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, A);
            B = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, B);
        }

        bool IsFloat = L.getValueType() == MVT::f64;
        bool RIsFloat = A.getValueType() == MVT::f64;

        SDValue result = DAG.getNode(RIsFloat ? AVM2ISD::FSL : AVM2ISD::SL, DL, RIsFloat ? MVT::f64 : MVT::i32,
                                     DAG.getNode(IsFloat ? AVM2ISD::FCNOP: AVM2ISD::CNOP, DL, MVT::i32,
                                             DAG.getTargetConstant((int)CC, MVT::i32), L, R), A, B);
        if(Op.getValueType() == MVT::f32) {
            result = DAG.getNode(ISD::FP_ROUND, DL, MVT::f32, result, DAG.getIntPtrConstant(0));
        }
        return result;
    }

    // case ISD::BR_CC: { return LowerBR_CC(Op, DAG); }
    case ISD::BR_CC: {
        SDValue Chain = Op.getOperand(0);
        ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
        SDValue L = Op.getOperand(2);
        SDValue R = Op.getOperand(3);
        SDValue Dest = Op.getOperand(4);


        // If this is a br_cc of a "setcc", and if the setcc got lowered into
        // an CMP[IF]CC/SELECT_[IF]CC pair, find the original compared values.
        if(L.getValueType() == MVT::f32) {
            L = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, L);
            R = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, R);
        } else if(L.getValueType() == MVT::i32 /*|| L.getValueType() == MVT::i64*/) {
            if(ConstantSDNode *node = dyn_cast<ConstantSDNode>(L)) {
                L = DAG.getTargetConstant((int)node->getSExtValue(), MVT::i32);
            }
            if(ConstantSDNode *node = dyn_cast<ConstantSDNode>(R)) {
                R = DAG.getTargetConstant((int)node->getSExtValue(), MVT::i32);
            }
        }


        int Opc;
        int Cmp;
        bool IsFloat = L.getValueType() == MVT::f64;
        if( IsFloat ) {
            Opc = AVM2ISD::FCBR;
            Cmp = AVM2ISD::FCNOP;
        } else {
            Opc = AVM2ISD::CBR;
            Cmp = AVM2ISD::CNOP;
        }

        SDValue NOP = DAG.getNode( Cmp, DL, MVT::i32, DAG.getTargetConstant(CC, MVT::i32), L, R);
        return DAG.getNode(Opc, DL, MVT::Other, Chain, NOP, Dest);
    }


    // XXX TODO can't this just be Legal?
    case ISD::ConstantFP: {
        union {
            double d;
            struct {
                unsigned a, b;
            } i;
        } u;

        APFloat apf(cast<ConstantFPSDNode>(Op)->getValueAPF());

        bool losesInfo = false;
        apf.convert(APFloat::IEEEdouble, APFloat::rmNearestTiesToEven, &losesInfo);
        u.d = apf.convertToDouble();
        SDValue result = DAG.getNode(AVM2ISD::FNOP, DL, MVT::f64,
                                     DAG.getNode(AVM2ISD::F64, DL, MVT::f64,
                                             DAG.getTargetConstant(u.i.a, MVT::i32),
                                             DAG.getTargetConstant(u.i.b, MVT::i32)));
        if(Op.getValueType() == MVT::f32) {
            result = DAG.getNode(ISD::FP_ROUND, DL, MVT::f32, result,DAG.getIntPtrConstant(0));
        }
        return result;
    }
    case ISD::ConstantPool: {
        const Constant *C = cast<ConstantPoolSDNode>(Op)->getConstVal();
        SDValue CP = DAG.getTargetConstantPool(C, MVT::i32,
                                               cast<ConstantPoolSDNode>(Op)->getAlignment());
        return CP;
    }
    case ISD::BRIND:  {
        return LowerBRIND(Op, DAG);
    }
    case ISD::BlockAddress:  {
        return LowerBlockAddress(Op, DAG);
    }
    case ISD::INIT_TRAMPOLINE:
    case ISD::ADJUST_TRAMPOLINE:  {
        return LowerTRAMPOLINE(Op, DAG);
    }
    case ISD::GlobalAddress: {
        const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
        if(GV->isDeclaration()) {
            // TODO ok? probably a leak
            std::string Str = "\2" + GV->getName().str();
            const char *N = strdup(Str.c_str());
            return DAG.getNode(AVM2ISD::INOP, DL, MVT::i32, DAG.getTargetExternalSymbol(N, MVT::i32));
        }
        EVT VT(MVT::i32);
        return DAG.getNode(AVM2ISD::INOP, DL, MVT::i32, DAG.getTargetGlobalAddress(GV, DL, VT));
    }
    case ISD::ExternalSymbol : {
        const char *N = cast<ExternalSymbolSDNode>(Op)->getSymbol();
        const Module *M = DAG.getMachineFunction().getFunction()->getParent();
        const GlobalValue *GV = M->getGlobalVariable(N);
        if(GV && !GV->isDeclaration()) {
            EVT VT(MVT::i32);
            return DAG.getNode(AVM2ISD::INOP, DL, MVT::i32, DAG.getTargetGlobalAddress(GV, DL, VT));
        }
        return DAG.getNode(AVM2ISD::INOP, DL, MVT::i32, DAG.getTargetExternalSymbol(N, MVT::i32));
    }
    case ISD::VASTART: {
        // vastart just stores the address of the VarArgsFrameIndex slot into the
        // memory location argument.
        SDValue Offset = DAG.getNode(ISD::ADD, DL, MVT::i32,
                                     DAG.getRegister(AVM2::EBP, MVT::i32),
                                     DAG.getConstant(DAG.getMachineFunction().getInfo<AVM2MachineFunctionInfo>()->getVarArgsFrameOffset(), MVT::i32));

        /*
         http://llvm.org/viewvc/llvm-project/llvm/trunk/include/llvm/CodeGen/SelectionDAGNodes.h?revision=46585&view=markup

         Create a new class, MemOperand, for describing memory references
         in the backend. Introduce a new SDNode type, MemOperandSDNode, for
         holding a MemOperand in the SelectionDAG IR, and add a MemOperand
         list to MachineInstr, and code to manage them. Remove the offset
         field from SrcValueSDNode; uses of SrcValueSDNode that were using
         it are all all using MemOperandSDNode now.

         Also, begin updating some getLoad and getStore calls to use the
         PseudoSourceValue objects.

         Most of this was written by Florian Brander, some
         reorganization and updating to TOT by me.
         */


        const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
        return DAG.getStore(Op.getOperand(0), DL, Offset,
                            Op.getOperand(1), MachinePointerInfo(SV), false, false, 0);

    }
    case ISD::VAARG: {
        SDNode *Node = Op.getNode();
        MVT::SimpleValueType VT = Node->getValueType(0).getSimpleVT().SimpleTy;
        SDValue InChain = Node->getOperand(0);
        SDValue VAListPtr = Node->getOperand(1);
        const Value *SV = cast<SrcValueSDNode>(Node->getOperand(2))->getValue();
        SDValue VAList = DAG.getLoad(getPointerTy(), DL, InChain, VAListPtr,
                                     MachinePointerInfo(SV), false, false, false, 0);
        // Increment the pointer, VAList, to the next vaarg
        SDValue NextPtr = DAG.getNode(ISD::ADD, DL, getPointerTy(), VAList,
                                      DAG.getConstant(Node->getValueType(0).getSizeInBits()/8,
                                              getPointerTy()));
        // Store the incremented VAList to the legalized pointer
        InChain = DAG.getStore(VAList.getValue(1), DL, NextPtr,
                               VAListPtr, MachinePointerInfo(SV), false, false, 0);
        // Load the actual argument out of the pointer VAList
        return DAG.getLoad(VT, DL, InChain, VAList, MachinePointerInfo(), false, false, false, 0);
    }
    case ISD::DYNAMIC_STACKALLOC: {
        SDValue Chain = Op.getOperand(0);  // Legalize the chain.
        SDValue Size  = Op.getOperand(1);  // Legalize the size.

        unsigned SPReg = AVM2::ESP;
        SDValue SP = DAG.getCopyFromReg(Chain, DL, SPReg, MVT::i32);
        SDValue NewSP = DAG.getNode(ISD::SUB, DL, MVT::i32, SP, Size);    // Value
        unsigned Align = getTargetMachine().getFrameLowering()->getStackAlignment();

        assert(!(Align & (Align - 1))); // must be power of 2
        SDValue NewSPAligned = DAG.getNode(ISD::AND, DL, MVT::i32, NewSP, DAG.getIntPtrConstant(~(Align - 1)));
        Chain = DAG.getCopyToReg(SP.getValue(1), DL, SPReg, NewSPAligned);      // Output chain

        std::vector<EVT> Tys;
        Tys.push_back(MVT::i32);
        Tys.push_back(MVT::Other);
        SDValue Ops[2] = { NewSPAligned, Chain };
        return DAG.getNode(ISD::MERGE_VALUES, DL, Tys, Ops, 2);
    }

    /*
     http://llvm.org/viewvc/llvm-project?view=rev&revision=78142
     Major calling convention code refactoring.

     Instead of awkwardly encoding calling-convention information with ISD::CALL,
     ISD::FORMAL_ARGUMENTS, ISD::RET, and ISD::ARG_FLAGS nodes, TargetLowering
     provides three virtual functions for targets to override:
     LowerFormalArguments, LowerCall, and LowerRet, which replace the custom
     lowering done on the special nodes. They provide the same information, but
     in a more immediately usable format.

     This also reworks much of the target-independent tail call logic. The
     decision of whether or not to perform a tail call is now cleanly split
     between target-independent portions, and the target dependent portion
     in IsEligibleForTailCallOptimization.

     This also synchronizes all in-tree targets, to help enable future
     refactoring and feature work.

     setOperationAction(ISD::RET               , MVT::Other, Custom);

     case ISD::RET: {
     SDValue Copy;

     switch(Op.getNumOperands()) {
     default:
     assert(0 && "Do not know how to return this many arguments!");
     abort();
     case 1:
     return DAG.getNode(AVM2ISD::RET_FLAG, DL, MVT::Other, Op.getOperand(0), DAG.getConstant(0, MVT::i32));
     case 3: {
     unsigned ArgReg;
     switch(Op.getOperand(1).getValueType()) {
     default: assert(0 && "Unknown type to return!");
     case MVT::i32: ArgReg = AVM2::EAX; break;
     case MVT::f32: ArgReg = AVM2::SST0; break;
     case MVT::f64: ArgReg = AVM2::ST0; break;
     }
     Copy = DAG.getCopyToReg(Op.getOperand(0), ArgReg, Op.getOperand(1),
     SDValue());
     break;
     }
     case 5:
     Copy = DAG.getCopyToReg(Op.getOperand(0), AVM2::EDX, Op.getOperand(3),
     SDValue());
     Copy = DAG.getCopyToReg(Copy, AVM2::EAX, Op.getOperand(1), Copy.getValue(1));
     break;
     }
     return DAG.getNode(AVM2ISD::RET_FLAG, DL, MVT::Other, Copy, Copy.getValue(1));
     }
     */
    case ISD::FRAMEADDR:
        return LowerFRAMEADDR(Op, DAG);
    case ISD::EH_SJLJ_SETJMP:
        return LowerEH_SJLJ_SETJMP(Op, DAG);
    case ISD::EH_SJLJ_LONGJMP:
        return LowerEH_SJLJ_LONGJMP(Op, DAG);
    /*
    TODO MCANNIZZ
    case ISD::EH_RETURN:
        return LowerEH_RETURN(Op, DAG);
    */
    case ISD::INTRINSIC_WO_CHAIN:
        return LowerINTRINSIC_WO_CHAIN(Op, DAG, SubTarget);
        // Return address.  Currently unimplemented
    case ISD::RETURNADDR:
        break;
    }
    return SDValue();
}

SDValue AVM2TargetLowering::LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const
{
    MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
    MFI->setFrameAddressIsTaken(true);

    EVT VT = Op.getValueType();
    DebugLoc dl = Op.getDebugLoc();  // FIXME probably not meaningful
    assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) && "only current frame addr supported");
    SDValue FrameAddr = DAG.getCopyFromReg(DAG.getEntryNode(), dl, AVM2::EBP, VT);
    return FrameAddr;
}

SDValue
AVM2TargetLowering::LowerEH_SJLJ_SETJMP(SDValue Op, SelectionDAG &DAG) const
{
    DebugLoc dl = Op.getDebugLoc();
    SDValue Val = DAG.getConstant(0, MVT::i32);
    return DAG.getNode(AVM2ISD::EH_SJLJ_SETJMP, dl, MVT::i32, Op.getOperand(0),
                       Op.getOperand(1), Val);
}

SDValue
AVM2TargetLowering::LowerEH_SJLJ_LONGJMP(SDValue Op, SelectionDAG &DAG) const
{
    DebugLoc dl = Op.getDebugLoc();
    return DAG.getNode(AVM2ISD::EH_SJLJ_LONGJMP, dl, MVT::Other, Op.getOperand(0),
                       Op.getOperand(1), DAG.getConstant(0, MVT::i32));
}

#if 0
// TODO MCANNIZZ
SDValue
AVM2TargetLowering::LowerEH_RETURN(SDValue Op, SelectionDAG &DAG) const
{

}
#endif

SDValue
AVM2TargetLowering::LowerINTRINSIC_WO_CHAIN(SDValue Op, SelectionDAG &DAG,
        const AVM2Subtarget *Subtarget) const
{
    unsigned IntNo = cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue();
    DebugLoc dl = Op.getDebugLoc();
    switch (IntNo) {
    default:
        return SDValue();    // Don't custom lower most intrinsics.
    case Intrinsic::eh_sjlj_lsda: {
        MachineFunction &MF = DAG.getMachineFunction();
        EVT VT(MVT::i32);
        SmallString<128> Str;
        raw_svector_ostream O(Str);
        O << getTargetMachine().getMCAsmInfo()->getPrivateGlobalPrefix() << "_LSDA_" << MF.getFunctionNumber();
        O.flush();
        // TODO strdup is probably a leak
        return DAG.getExternalSymbol(strdup(Str.str().str().c_str()), VT);
    }
    }
}

SDValue AVM2TargetLowering::
LowerFormalArguments(SDValue Chain,
                     CallingConv::ID CallConv, bool isVarArg,
                     const SmallVectorImpl<ISD::InputArg> &Ins,
                     DebugLoc DL, SelectionDAG &DAG,
                     SmallVectorImpl<SDValue> &InVals) const
{
    MachineFunction &MF = DAG.getMachineFunction();
    AVM2MachineFunctionInfo *FuncInfo = MF.getInfo<AVM2MachineFunctionInfo>();


    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, isVarArg, MF, getTargetMachine(), ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeFormalArguments(Ins, CC_AVM2_32);

    SDValue Root = DAG.getRoot();
    unsigned ArgOffset = 0;

    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
        SDValue ArgValue;
        CCValAssign &VA = ArgLocs[i];
        // FIXME: We ignore the register assignments of AnalyzeFormalArguments
        // because it doesn't know how to split a double into two i32 registers.
        EVT ObjectVT = VA.getValVT();

        switch (ObjectVT.getSimpleVT().SimpleTy) {
        default:
            assert(0 && "Unhandled argument type!");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16:
        case MVT::i32:
            if (!Ins[i].Used) { // Argument is dead.
                InVals.push_back(DAG.getNode(ISD::UNDEF, DL, ObjectVT));
            } else {
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(4, ArgOffset, true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
                SDValue Load;
                if (ObjectVT == MVT::i32) {
                    Load = DAG.getLoad(MVT::i32, DL, Root, FIPtr, MachinePointerInfo(), false, false, false, 0);
                } else {
                    ISD::LoadExtType LoadOp = ISD::SEXTLOAD;

                    unsigned Offset = 0; // LE
                    FIPtr = DAG.getNode(ISD::ADD, DL, MVT::i32, FIPtr,
                                        DAG.getConstant(Offset, MVT::i32));
                    Load = DAG.getExtLoad(LoadOp, DL, MVT::i32, Root, FIPtr,
                                          MachinePointerInfo(), ObjectVT, false, false, 0);
                    Load = DAG.getNode(ISD::TRUNCATE, DL, ObjectVT, Load);
                }
                InVals.push_back(Load);
            }
            ArgOffset += 4;
            break;
        case MVT::f32:
            if (!Ins[i].Used) { // Argument is dead.
                InVals.push_back(DAG.getNode(ISD::UNDEF, DL, ObjectVT));
            } else {
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(4, ArgOffset, true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
                InVals.push_back(DAG.getLoad(ObjectVT, DL, Root, FIPtr, MachinePointerInfo(), false, false, false, 0));
            }
            ArgOffset += 4;
            break;

        case MVT::i64:
            if (!Ins[i].Used) { // Argument is dead.
                InVals.push_back(DAG.getNode(ISD::UNDEF, DL, ObjectVT));
            } else {
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(8, ArgOffset, true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
                SDValue FIPtr4 = DAG.getNode(ISD::ADD, DL, MVT::i32, FIPtr, DAG.getConstant(4, MVT::i32));
                SDValue Lo = DAG.getLoad(MVT::i32, DL, Root, FIPtr, MachinePointerInfo(), false, false, false, 0);
                SDValue Hi = DAG.getLoad(MVT::i32, DL, Root, FIPtr4, MachinePointerInfo(), false, false, false, 0);
                InVals.push_back(DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i64, Lo, Hi));
            }
            ArgOffset += 8;
            break;
        case MVT::f64:
            if (!Ins[i].Used) { // Argument is dead.
                InVals.push_back(DAG.getNode(ISD::UNDEF, DL, ObjectVT));
            } else {
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(8, ArgOffset, true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
                InVals.push_back(DAG.getLoad(ObjectVT, DL, Root, FIPtr, MachinePointerInfo(), false, false, false, 0));
            }
            ArgOffset += 8;
            break;
        }
    }

    if (isVarArg) {
        // Remember the vararg offset for the va_start implementation.
        FuncInfo->setVarArgsFrameOffset(ArgOffset);
    }

    return Chain;
}

// TODO refactor?
SDValue AVM2TargetLowering::
LowerCall(CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const
{
    SDValue Chain = CLI.Chain;
    SDValue Callee = CLI.Callee;
    CallingConv::ID CallConv = CLI.CallConv;
    bool isVarArg = CLI.IsVarArg;
    // AVM2 target does not yet support tail call optimization.
    bool isTailCall = false;
    const SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
    const SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
    const SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
    DebugLoc DL = CLI.DL;
    SelectionDAG &DAG = CLI.DAG;

    // Count the size of the outgoing arguments.
    unsigned ArgsSize = 0;

    for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
        switch (OutVals[i].getValueType().getSimpleVT().SimpleTy) {
        default:
            assert(0 && "Unknown value type!");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16:
        case MVT::i32:
        case MVT::f32:
            ArgsSize += 4;
            break;
        case MVT::i64:
        case MVT::f64:
            ArgsSize += 8;
            break;
        }
    }

    unsigned Align = getTargetMachine().getFrameLowering()->getStackAlignment();
    unsigned AlignAdjust = (Align - (ArgsSize % Align)) % Align;
    ArgsSize += AlignAdjust;
    unsigned ArgOffset = 0;

    Chain = DAG.getCALLSEQ_START(Chain,DAG.getIntPtrConstant(ArgsSize, true));
    // Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(ArgsSize, true));

    // Analyze operands of the call, assigning locations to each operand.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), DAG.getTarget(), ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeCallOperands(Outs, CC_AVM2_32);

    SDValue StackPtr;
    SmallVector<SDValue, 8> MemOpChains;
    for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
        SDValue Val = OutVals[i];
        EVT ObjectVT = Val.getValueType();
        SDValue ValToStore(0, 0);
        unsigned ObjSize;
        switch (ObjectVT.getSimpleVT().SimpleTy) {
        default:
            assert(0 && "Unhandled argument type!");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16: {
            CCValAssign &VA = ArgLocs[i];
            // Promote the integer to 32-bits.  If the input type is signed, use a
            // sign extend, otherwise use a zero extend.
            ISD::NodeType ExtendKind = ISD::ANY_EXTEND;
            if (VA.getLocInfo() == CCValAssign::SExt) {
                ExtendKind = ISD::SIGN_EXTEND;
            } else if (VA.getLocInfo() == CCValAssign::ZExt) {
                ExtendKind = ISD::ZERO_EXTEND;
            } else if (VA.getLocInfo() == CCValAssign::AExt) {
                ExtendKind = ISD::ANY_EXTEND;
            }
            Val = DAG.getNode(ExtendKind, DL, MVT::i32, Val);
            // FALL THROUGH
        }
        case MVT::i32: // TODO unify
            ObjSize = 4;
            ValToStore = Val;
            break;
        case MVT::f32:
            ObjSize = 4;
            ValToStore = Val;
            break;
        case MVT::f64:
            ObjSize = 8;
            ValToStore = Val;
            break;
        case MVT::i64:
            ObjSize = 8;
            ValToStore = Val; // Whole thing is passed in memory.
            break;
        }


        if (ValToStore.getNode()) {
            SDValue StackPtr = DAG.getRegister(AVM2::ESP, MVT::i32);
            SDValue PtrOff = DAG.getConstant(ArgOffset, MVT::i32);
            PtrOff = DAG.getNode(ISD::ADD, DL, MVT::i32, StackPtr, PtrOff);
            if(ObjectVT == MVT::i64) { // 2 stores for 64 bit
                SDValue Lo = DAG.getNode(ISD::EXTRACT_ELEMENT, DL, MVT::i32, ValToStore, DAG.getConstant(0, MVT::i32));
                SDValue Hi = DAG.getNode(ISD::EXTRACT_ELEMENT, DL, MVT::i32, ValToStore, DAG.getConstant(1, MVT::i32));
                SDValue PtrOff4 = DAG.getNode(ISD::ADD, DL, MVT::i32, PtrOff, DAG.getConstant(4, MVT::i32));

                MemOpChains.push_back(DAG.getStore(Chain, DL, Lo, PtrOff, MachinePointerInfo(), false, false, 0));
                MemOpChains.push_back(DAG.getStore(Chain, DL, Hi, PtrOff4, MachinePointerInfo(), false, false, 0));
            } else
                MemOpChains.push_back(DAG.getStore(Chain, DL, ValToStore,
                                                   PtrOff, MachinePointerInfo(),
                                                   false, false, 0));
        }

        ArgOffset += ObjSize;
    }

    // Emit all stores, make sure the occur before any copies into physregs.
    if (!MemOpChains.empty()) {
        Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, &MemOpChains[0], MemOpChains.size());
    }

    std::vector<EVT> NodeTys;
    NodeTys.push_back(MVT::Other); // Returns a chain
    NodeTys.push_back(MVT::Glue);  // Returns a flag for retval copy to use.
    SDValue Ops[] = { Chain, Callee };
    Chain = DAG.getNode(AVM2ISD::CALL, DL, NodeTys, Ops, 2);
    SDValue InFlag = Chain.getValue(1);
    // Chain = DAG.getNode(ISD::CALLSEQ_END, DL, MVT::Other, Chain, DAG.getConstant(ArgsSize, getPointerTy()));
    Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(ArgsSize, true), DAG.getIntPtrConstant(0, true), InFlag);

    // If the function returns void, just return the chain.
    if (Ins.empty()) {
        return Chain;
    }

    InFlag = Chain.getValue(1);

    /*
    	// Assign locations to each value returned by this call.
    	SmallVector<CCValAssign, 16> RVLocs;
    	CCState RVInfo(CallConv, isVarArg, DAG.getTarget(),
    				   RVLocs, *DAG.getContext());

    	RVInfo.AnalyzeCallResult(Ins, RetCC_AVM2_32);
    	const MVT::SimpleValueType RetTyVT = RVLocs.size() == 0 ? MVT::isVoid : RVLocs[0].getValVT().SimpleTy;

    	// Copy all of the result registers out of their specified physreg.
    	for (unsigned i = 0; i != RVLocs.size(); ++i) {
    		unsigned Reg = RVLocs[i].getLocReg();
    		Chain = DAG.getCopyFromReg(Chain, DL, Reg,
    								   RVLocs[i].getValVT(), InFlag).getValue(1);
    		InFlag = Chain.getValue(2);
    		InVals.push_back(Chain.getValue(0));
    	}
    */

    SmallVector<CCValAssign, 16> RVLocs;
    CCState RVInfo(CallConv, isVarArg, DAG.getMachineFunction(), 
                   DAG.getTarget(), RVLocs, *DAG.getContext());
    if(!RVInfo.CheckReturn(Ins, RetCC_AVM2_32)) {
      report_fatal_error("Flascc does not yet support LLVM SIMD intrinsics.\n");
    }

    for (unsigned i = 0, e = Ins.size(); i != e; ++i) {

        const MVT::SimpleValueType RetTyVT = Ins[i].VT.SimpleTy;

        SDValue RetVal;
        if (RetTyVT != MVT::isVoid) {
            // SDVTList Tys = DAG.getVTList(AVM2::EAX, MVT::i32, MVT::Glue);

            switch (RetTyVT) {
            default:
                assert(0 && "Unknown value type to return!");
            case MVT::i1:
            case MVT::i8:
            case MVT::i16: {
                assert( i <= 1 && "More than 2 return values for i1/i8/i16." );
                if( i == 0 ) {
                    RetVal = DAG.getCopyFromReg(Chain, DL, AVM2::EAX, MVT::i32, InFlag);
                } else {
                    RetVal = DAG.getCopyFromReg(Chain, DL, AVM2::EDX, MVT::i32, InFlag);
                }

                Chain = RetVal.getValue(1);
                InFlag = Chain.getValue(2);

                // Add a note to keep track of whether it is sign or zero extended.
                CCValAssign &VA = RVLocs[i];
                ISD::NodeType AssertKind = ISD::AssertZext;
                if (VA.getLocInfo() == CCValAssign::SExt) {
                    AssertKind = ISD::AssertSext;
                }
                RetVal = DAG.getNode(AssertKind, DL, MVT::i32, RetVal,
                                     DAG.getValueType(RetTyVT));
                RetVal = DAG.getNode(ISD::TRUNCATE, DL, RetTyVT, RetVal);
                break;
            }
            case MVT::i32:
                assert( i <= 1 && "More than 2 return values for i32." );
                if( i == 0 ) {
                    RetVal = DAG.getCopyFromReg(Chain, DL, AVM2::EAX, MVT::i32, InFlag);
                } else {
                    RetVal = DAG.getCopyFromReg(Chain, DL, AVM2::EDX, MVT::i32, InFlag);
                }

                Chain = RetVal.getValue(1);
                InFlag = Chain.getValue(2);
                // Chain = DAG.getCopyFromReg(Chain, DL, AVM2::EAX, MVT::i32, InFlag);
                break;
            case MVT::f32:
                assert( i == 0 && "More than 1 return value for f32." );
                RetVal = DAG.getCopyFromReg(Chain, DL, AVM2::SST0, MVT::f32, InFlag);
                Chain = RetVal.getValue(1);
                InFlag = Chain.getValue(2);
                break;
            case MVT::f64:
                assert( i == 0 && "More than 1 return value for f64." );
                RetVal = DAG.getCopyFromReg(Chain, DL, AVM2::ST0, MVT::f64, InFlag);
                Chain = RetVal.getValue(1);
                InFlag = Chain.getValue(2);
                break;
            case MVT::i64: {
                assert( i == 0 && "More than 1 return value for i64." );
                SDValue Lo = DAG.getCopyFromReg(Chain, DL, AVM2::EAX, MVT::i32, InFlag);
                SDValue Hi = DAG.getCopyFromReg(Lo.getValue(1), DL, AVM2::EDX, MVT::i32,
                                                Lo.getValue(2));
                RetVal = DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i64, Lo, Hi);
                Chain = Hi.getValue(1);
                InFlag = Chain.getValue(2);
            }
            break;
            }
        }

        InVals.push_back(Chain.getValue(0));
    }


#ifdef _DEBUG
    int InVals_size = InVals.size();
    int Ins_size = Ins.size();
    const char* err;

    // pre-check conditions that will assert in TargetLowering::LowerCallTo()
    if( InVals_size != Ins_size ) {
        // "LowerCall didn't emit the correct number of values!"
        Ins_size = InVals_size;
    }

    for (unsigned i = 0, e = Ins.size(); i != e; ++i) {
        if( !InVals[i].getNode()  ) {
            err = "LowerCall emitted a null value!";
        }

        EVT vt = Ins[i].VT;
        if( vt != InVals[i].getValueType() ) {
            err = "LowerCall emitted a value with the wrong type!";
            EVT vt1 = Ins[i].VT;
            EVT vt2 = InVals[i].getValueType();
            vt1 = vt2;
        }
    }

#endif

    return Chain;
}

SDValue AVM2TargetLowering::
LowerReturn(SDValue Chain,
            CallingConv::ID CallConv, bool isVarArg,
            const SmallVectorImpl<ISD::OutputArg> &Outs,
            const SmallVectorImpl<SDValue> &OutVals,
            DebugLoc dl, SelectionDAG &DAG) const
{
    /*
    	SDValue Op = Outs[0].Val;
    	const int numOperands = Op.getNumOperands();

    	SDValue Copy;

    	switch(numOperands) {
    		default:
    			assert(0 && "Do not know how to return this many arguments!");
    			abort();
    		case 1:
    			return DAG.getNode(AVM2ISD::RET_FLAG, dl, MVT::Other, Op.getOperand(0), DAG.getConstant(0, MVT::i32));
    		case 3: {
    			unsigned ArgReg;
    			const int vt = Op.getOperand(1).getValueType().SimpleTy;
    			switch(vt) {
    				default: assert(0 && "Unknown type to return!");
    				case MVT::i32: ArgReg = AVM2::EAX; break;
    				case MVT::f32: ArgReg = AVM2::SST0; break;
    				case MVT::f64: ArgReg = AVM2::ST0; break;
    			}

    			Copy = DAG.getCopyToReg(Op.getOperand(0), dl, ArgReg, Op.getOperand(1),
    									SDValue());
    			break;
    		}
    		case 5:
    			Copy = DAG.getCopyToReg(Op.getOperand(0), dl, AVM2::EDX, Op.getOperand(3),
    									SDValue());
    			Copy = DAG.getCopyToReg(Copy, dl, AVM2::EAX, Op.getOperand(1), Copy.getValue(1));
    			break;
    	}
    	return DAG.getNode(AVM2ISD::RET_FLAG, dl, MVT::Other, Copy, Copy.getValue(1));
    */

    // CCValAssign - represent the assignment of the return value to locations.
    SmallVector<CCValAssign, 16> RVLocs;

    // CCState - Info about the registers and stack slot.
    CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), 
                   DAG.getTarget(), RVLocs, *DAG.getContext());

    // Analyze return values.
    if(!(CCInfo.CheckReturn(Outs, RetCC_AVM2_32))) {
        report_fatal_error("FlasCC does not yet support LLVM SIMD intrinsics.\n");
    }   

    // TODO: Implement - VPMedia
    // If this is the first return lowered for this function, add the regs to the
    // liveout set for the function.
    /*if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
        for (unsigned i = 0; i != RVLocs.size(); ++i)
            if (RVLocs[i].isRegLoc()) {
                DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
            }
    }*/

    SDValue Flag;

    // Copy the result values into the output registers.
    for (unsigned i = 0; i != RVLocs.size(); ++i) {
        CCValAssign &VA = RVLocs[i];
        assert(VA.isRegLoc() && "Can only return in registers!");

        Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(),
                                 OutVals[i], Flag);

        // Guarantee that all emitted copies are stuck together with flags.
        Flag = Chain.getValue(1);
    }

    if (Flag.getNode()) {
        return DAG.getNode(AVM2ISD::RET_FLAG, dl, MVT::Other, Chain, Flag);
    }

    return DAG.getNode(AVM2ISD::RET_FLAG, dl, MVT::Other, Chain);
}

AVM2TargetLowering::ConstraintType AVM2TargetLowering::getConstraintType(const std::string &Constraint) const
{
    if(Constraint == "L" || Constraint == "H") {
        return C_Register;
    }
    return TargetLowering::getConstraintType(Constraint);
}

std::pair<unsigned, const TargetRegisterClass*> AVM2TargetLowering::getRegForInlineAsmConstraint(const std::string &Constraint,
        EVT VT) const
{
    if (Constraint == "H" && VT == MVT::i32) {
        return std::make_pair((unsigned)AVM2::EDX, &AVM2::IntRegsRegClass);
    } else if (Constraint == "L" && VT == MVT::i32) {
        return std::make_pair((unsigned)AVM2::EAX, &AVM2::IntRegsRegClass);
    } else if (Constraint == "r" && (VT == MVT::i32 || VT == MVT::i16 || VT == MVT::i8 || VT == MVT::i1)) {
        return std::make_pair((unsigned)AVM2::NoRegister, &AVM2::IntRegsRegClass);
    } else if (Constraint == "r" && VT == MVT::f64) {
        return std::make_pair((unsigned)AVM2::NoRegister, &AVM2::FPRegsRegClass);
    } else if (Constraint == "r" && VT == MVT::f32) {
        return std::make_pair((unsigned)AVM2::NoRegister, &AVM2::SPRegsRegClass);
    }
    return TargetLowering::getRegForInlineAsmConstraint(Constraint, VT);
}

/// getFunctionAlignment - Return the Log2 alignment of this function.
unsigned int AVM2TargetLowering::getFunctionAlignment(const llvm::Function*) const
{
    return 2;
}

const TargetSubtargetInfo* AVM2TargetLowering::getSubtarget()
{
    return SubTarget;
}

bool AVM2TargetLowering::allowsUnalignedMemoryAccesses(EVT VT) const
{
    return true;
}

namespace {
  // Helper to match a string separated by whitespace.
  bool matchAsmImpl(StringRef s, ArrayRef<const StringRef *> args) {
    s = s.substr(s.find_first_not_of(" \t")); // Skip leading whitespace.
    
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      StringRef piece(*args[i]);
      if (!s.startswith(piece)) // Check if the piece matches.
        return false;
      
      s = s.substr(piece.size());
      StringRef::size_type pos = s.find_first_not_of(" \t");
      if (pos == 0) // We matched a prefix.
        return false;
      
      s = s.substr(pos);
    }
    
    return s.empty();
  }
  const VariadicFunction1<bool, StringRef, StringRef, matchAsmImpl> matchAsm={};
}

bool AVM2TargetLowering::ExpandInlineAsm(CallInst *CI) const {
  InlineAsm *IA = cast<InlineAsm>(CI->getCalledValue());
  
  std::string AsmStr = IA->getAsmString();
  
  IntegerType *Ty = dyn_cast<IntegerType>(CI->getType());
  if (!Ty || Ty->getBitWidth() % 16 != 0)
    return false;
  
  // TODO: should remove alternatives from the asmstring: "foo {a|b}" -> "foo a"
  SmallVector<StringRef, 4> AsmPieces;
  SplitString(AsmStr, AsmPieces, ";\n");
  
  switch (AsmPieces.size()) {
    default: return false;
    case 1:
      // FIXME: this should verify that we are targeting a 486 or better.  If not,
      // we will turn this bswap into something that will be lowered to logical
      // ops instead of emitting the bswap asm.  For now, we don't support 486 or
      // lower so don't worry about this.
      // bswap $0
      if (matchAsm(AsmPieces[0], "bswap", "$0") ||
          matchAsm(AsmPieces[0], "bswapl", "$0") ||
          matchAsm(AsmPieces[0], "bswapq", "$0") ||
          matchAsm(AsmPieces[0], "bswap", "${0:q}") ||
          matchAsm(AsmPieces[0], "bswapl", "${0:q}") ||
          matchAsm(AsmPieces[0], "bswapq", "${0:q}")) {
        // No need to check constraints, nothing other than the equivalent of
        // "=r,0" would be valid here.
        return IntrinsicLowering::LowerToByteSwap(CI);
      }
      
      // rorw $$8, ${0:w}  -->  llvm.bswap.i16
      if (CI->getType()->isIntegerTy(16) &&
          IA->getConstraintString().compare(0, 5, "=r,0,") == 0 &&
          (matchAsm(AsmPieces[0], "rorw", "$$8,", "${0:w}") ||
           matchAsm(AsmPieces[0], "rolw", "$$8,", "${0:w}"))) {
            AsmPieces.clear();
            const std::string &ConstraintsStr = IA->getConstraintString();
            SplitString(StringRef(ConstraintsStr).substr(5), AsmPieces, ",");
            std::sort(AsmPieces.begin(), AsmPieces.end());
            if (AsmPieces.size() == 4 &&
                AsmPieces[0] == "~{cc}" &&
                AsmPieces[1] == "~{dirflag}" &&
                AsmPieces[2] == "~{flags}" &&
                AsmPieces[3] == "~{fpsr}")
              return IntrinsicLowering::LowerToByteSwap(CI);
          }
      break;
    case 3:
      if (CI->getType()->isIntegerTy(32) &&
          IA->getConstraintString().compare(0, 5, "=r,0,") == 0 &&
          matchAsm(AsmPieces[0], "rorw", "$$8,", "${0:w}") &&
          matchAsm(AsmPieces[1], "rorl", "$$16,", "$0") &&
          matchAsm(AsmPieces[2], "rorw", "$$8,", "${0:w}")) {
        AsmPieces.clear();
        const std::string &ConstraintsStr = IA->getConstraintString();
        SplitString(StringRef(ConstraintsStr).substr(5), AsmPieces, ",");
        std::sort(AsmPieces.begin(), AsmPieces.end());
        if (AsmPieces.size() == 4 &&
            AsmPieces[0] == "~{cc}" &&
            AsmPieces[1] == "~{dirflag}" &&
            AsmPieces[2] == "~{flags}" &&
            AsmPieces[3] == "~{fpsr}")
          return IntrinsicLowering::LowerToByteSwap(CI);
      }
      
      if (CI->getType()->isIntegerTy(64)) {
        InlineAsm::ConstraintInfoVector Constraints = IA->ParseConstraints();
        if (Constraints.size() >= 2 &&
            Constraints[0].Codes.size() == 1 && Constraints[0].Codes[0] == "A" &&
            Constraints[1].Codes.size() == 1 && Constraints[1].Codes[0] == "0") {
          // bswap %eax / bswap %edx / xchgl %eax, %edx  -> llvm.bswap.i64
          if (matchAsm(AsmPieces[0], "bswap", "%eax") &&
              matchAsm(AsmPieces[1], "bswap", "%edx") &&
              matchAsm(AsmPieces[2], "xchgl", "%eax,", "%edx"))
            return IntrinsicLowering::LowerToByteSwap(CI);
        }
      }
      break;
  }
  return false;
}

