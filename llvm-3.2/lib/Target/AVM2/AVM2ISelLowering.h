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

//===-- AVM2ISelLowering.h - AVM2 DAG Lowering Interface ------*- C++ -*-===//
//
// This file defines the interfaces that AVM2 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef AVM2_ISELLOWERING_H
#define AVM2_ISELLOWERING_H

#include "llvm/Target/TargetLowering.h"
#include "AVM2.h"

namespace llvm
{
class AVM2Subtarget;

namespace AVM2ISD
{
enum {
    FIRST_NUMBER = ISD::BUILTIN_OP_END /*+AVM2::INSTRUCTION_LIST_END */,
    CBR, // Conditional branch wrapper
    FCBR, // Float conditional branch wrappers
    INOP, // Integer operation wrapper
    FNOP, // Float operation wrapper
    SL, // Select wrapper
    FSL, // Float select wrapper
    CNOP, // Condition wrapper
    FCNOP, // Float condition wrapper
    F64, // Float immediate wrapper
    CALL,        // A call instruction.
    RET_FLAG,     // Return with a flag operand.
    EH_SJLJ_SETJMP,         // SjLj exception handling setjmp.
    EH_SJLJ_LONGJMP,        // SjLj exception handling longjmp.
    EH_SJLJ_DISPATCHSETUP  // SjLj exception handling dispatch setup.

};
}


class AVM2TargetLowering : public TargetLowering
{
    /// Subtarget - Keep a pointer to the AVM2Subtarget around so that we can
    /// make the right decision when generating code for different targets.
    const AVM2Subtarget *SubTarget;
public:
    AVM2TargetLowering(TargetMachine &TM);
    virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const;

    virtual SDValue
    LowerFormalArguments(SDValue Chain,
                         CallingConv::ID CallConv, bool isVarArg,
                         const SmallVectorImpl<ISD::InputArg> &Ins,
                         DebugLoc dl, SelectionDAG &DAG,
                         SmallVectorImpl<SDValue> &InVals) const;
    virtual SDValue 
    LowerCall(CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const;

    SDValue LowerBRIND(SDValue Op, SelectionDAG &DAG) const;

    SDValue LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;

    SDValue LowerTRAMPOLINE(SDValue Op, SelectionDAG &DAG) const;

    virtual SDValue
    LowerReturn(SDValue Chain,
                CallingConv::ID CallConv, bool isVarArg,
                const SmallVectorImpl<ISD::OutputArg> &Outs,
                const SmallVectorImpl<SDValue> &OutVals,
                DebugLoc dl, SelectionDAG &DAG) const;

    SDValue LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerEH_SJLJ_SETJMP(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerEH_SJLJ_LONGJMP(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerEH_SJLJ_DISPATCHSETUP(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerINTRINSIC_WO_CHAIN(SDValue Op, SelectionDAG &DAG,
                                    const AVM2Subtarget *Subtarget) const;

    virtual const TargetSubtargetInfo* getSubtarget();

    virtual const char *getTargetNodeName(unsigned Opcode) const;

    ConstraintType getConstraintType(const std::string &Constraint) const;
    std::pair<unsigned, const TargetRegisterClass*> getRegForInlineAsmConstraint(const std::string &Constraint,	EVT VT) const;

    MachineBasicBlock *EmitInstrWithCustomInserter(MachineInstr *MI,	MachineBasicBlock *BB) const;

    /// getFunctionAlignment - Return the Log2 alignment of this function.
    virtual unsigned int getFunctionAlignment(const llvm::Function*) const;
    virtual bool allowsUnalignedMemoryAccesses(EVT VT) const;
    virtual bool ExpandInlineAsm(CallInst *CI) const;
};
} // end namespace llvm

#endif    // AVM2_ISELLOWERING_H
