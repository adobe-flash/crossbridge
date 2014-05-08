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

//===-- AVM2ISelDAGToDAG.cpp - A dag to dag inst selector for AVM2 ------===//
//
// This file defines an instruction selector for the AVM2 target.
//
//===----------------------------------------------------------------------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#include "AVM2.h"
#include "AVM2TargetMachine.h"
#include "AVM2ISelLowering.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <queue>
#include <set>
#include <stdio.h>
using namespace llvm;

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===--------------------------------------------------------------------===//
/// AVM2DAGToDAGISel - AVM2 specific code to select AVM2 machine
/// instructions for SelectionDAG operations.
///
namespace llvm
{
class AVM2DAGToDAGISel : public SelectionDAGISel
{
    AVM2TargetLowering Lowering;

    /// Subtarget - Keep a pointer to the AVM2 Subtarget around so that we can
    /// make the right decision when generating code for different targets.
    const AVM2Subtarget &Subtarget;
public:
    AVM2DAGToDAGISel(TargetMachine &TM, CodeGenOpt::Level OptLevel)
        : SelectionDAGISel(TM, OptLevel), Lowering(TM),
          Subtarget(TM.getSubtarget<AVM2Subtarget>()) {
    }

    SDNode *Select(SDNode *N);

    // Complex Pattern Selectors.
//  bool SelectADDRri(SDNode *Op, SDValue Addr, SDValue &Base, SDValue &Offset)
    bool SelectADDRri(SDValue Addr, SDValue &Base, SDValue &Offset) {
        if (Addr.getOpcode() == ISD::TargetExternalSymbol ||
                Addr.getOpcode() == ISD::TargetGlobalAddress) {
            return false;    // direct calls.
        }

        if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
            Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
            Offset = CurDAG->getTargetConstant(0, MVT::i32);
            return true;
        }

        if (Addr.getOpcode() == ISD::ADD) {
            if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) {
                if (FrameIndexSDNode *FIN =
                            dyn_cast<FrameIndexSDNode>(Addr.getOperand(0))) {
                    // Constant offset from frame ref.
                    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
                } else {
                    Base = Addr.getOperand(0);
                    if(Base.getOpcode() == AVM2ISD::INOP) {
                        Base = Base.getOperand(0);
                    }
                }
                Offset = CurDAG->getTargetConstant(*CN->getConstantIntValue(), MVT::i32);
                return true;
            }
        }

        Base = Addr;
        if(Base.getOpcode() == AVM2ISD::INOP) {
            Base = Base.getOperand(0);
        }
        Offset = CurDAG->getTargetConstant(0, MVT::i32);
        return true;
    }

    virtual bool SelectInlineAsmMemoryOperand(const SDValue &Op,
                                              char ConstraintCode,
                                              std::vector<SDValue> &OutOps);

    // DEAD
    bool SelectADDRrr(SDValue Addr, SDValue &Base, SDValue &Offset) {
        return false;
    }

    bool SelectFloatImm(SDValue N, SDValue &A, SDValue &B) {
        if(N.getOpcode() == AVM2ISD::F64) {
            A = N.getOperand(0);
            B = N.getOperand(1);
            return true;
        }
        return false;
    }

    bool SelectCond(SDValue N, SDValue &CC, SDValue &L, SDValue &R) {
        if(N.getOpcode() == AVM2ISD::CNOP) {
            CC = N.getOperand(0);
            L = N.getOperand(1);
            R = N.getOperand(2);
            return true;
        }
        return false;
    }

    bool SelectFloatCond(SDValue N, SDValue &CC, SDValue &L, SDValue &R) {
        if(N.getOpcode() == AVM2ISD::FCNOP) {
            CC = N.getOperand(0);
            L = N.getOperand(1);
            R = N.getOperand(2);
            return true;
        }
        return false;
    }

    virtual const char *getPassName() const {
        return "AVM2 DAG->DAG Pattern Instruction Selection";
    }

    // Include the pieces autogenerated from the target description.
#include "AVM2GenDAGISel.inc"
};
}  // end anonymous namespace

SDNode *AVM2DAGToDAGISel::Select(SDNode *N)
{
    if (N->getOpcode() >= ISD::BUILTIN_OP_END &&
            N->getOpcode() < AVM2ISD::FIRST_NUMBER) {
        return NULL;    // Already selected.
    }

    switch (N->getOpcode()) {
    default:
        break;
    }

    return SelectCode(N);
}

bool AVM2DAGToDAGISel::
SelectInlineAsmMemoryOperand(const SDValue &Op, char ConstraintCode,
                             std::vector<SDValue> &OutOps) {
  if(ConstraintCode != 'm') {
    report_fatal_error(std::string("Unsupported inline asm constraint ") + ConstraintCode);
  }
  OutOps.push_back(Op);
  return false;
}


/// createAVM2ISelDag - This pass converts a legalized DAG into a
/// AVM2-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createAVM2ISelDag(TargetMachine &TM, CodeGenOpt::Level OptLevel)
{
    return new AVM2DAGToDAGISel(TM, OptLevel);
}
