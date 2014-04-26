/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* High-level LLVM backend interface 
Copyright (C) 2007 Free Software Foundation, Inc.
Contributed by Jim Laskey (jlaskey@apple.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

//===----------------------------------------------------------------------===//
// This is a C++ source file that implements specific llvm powerpc ABI.
//===----------------------------------------------------------------------===//

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"

extern "C" {
#include "toplev.h"
}

static bool isSVR4ABI() {
#if defined(POWERPC_LINUX) && (TARGET_64BIT == 0)
  return true;
#else
  return false;
#endif
}

static LLVMContext &Context = getGlobalContext();

// MergeIntPtrOperand - This merges the int and pointer operands of a GCC
// intrinsic into a single operand for the LLVM intrinsic.  For example, this
// turns LVX(4, p) -> llvm.lvx(gep P, 4).  OPNUM specifies the operand number
// of the integer to contract with its following pointer and NAME specifies the
// name of the resultant intrinsic.
static void MergeIntPtrOperand(TreeToLLVM *TTL,
                               unsigned OpNum, Intrinsic::ID IID,
                               const Type *ResultType,
                               std::vector<Value*> &Ops,
                               LLVMBuilder &Builder, Value *&Result) {
  const Type *VoidPtrTy = PointerType::getUnqual(Type::getInt8Ty(Context));
  
  Function *IntFn = Intrinsic::getDeclaration(TheModule, IID);
  
  Value *Offset = Ops[OpNum];
  Value *Ptr = Ops[OpNum + 1];
  Ptr = TTL->CastToType(Instruction::BitCast, Ptr, VoidPtrTy);
  
  if (!isa<Constant>(Offset) || !cast<Constant>(Offset)->isNullValue())
    Ptr = Builder.CreateGEP(Ptr, Offset, "tmp");
    
  Ops.erase(Ops.begin() + OpNum);
  Ops[OpNum] = Ptr;
  Value *V = Builder.CreateCall(IntFn, &Ops[0], &Ops[0]+Ops.size());
  
  if (V->getType() != Type::getVoidTy(Context)) {
    V->setName("tmp");
    Result = V;
  }
}

// GetAltivecTypeNumFromType - Given an LLVM type, return a unique ID for
// the type in the range 0-3.
static int GetAltivecTypeNumFromType(const Type *Ty) {
  return (Ty->isIntegerTy(32) ? 0 : \
          (Ty->isIntegerTy(16) ? 1 : \
           (Ty->isIntegerTy(8) ? 2 : \
            ((Ty == Type::getFloatTy(Context)) ? 3 : -1))));
}

// TargetIntrinsicLower - To handle builtins, we want to expand the
//invocation into normal LLVM code.  If the target can handle the builtin, this
//function should emit the expanded code and return true.
//
bool TreeToLLVM::TargetIntrinsicLower(tree exp,
                                      unsigned FnCode,
                                      const MemRef *DestLoc,
                                      Value *&Result,
                                      const Type *ResultType,
                                      std::vector<Value*> &Ops) {
  switch (FnCode) {
  default: break;
  case ALTIVEC_BUILTIN_VADDFP:
    Result = Builder.CreateFAdd(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VADDUBM:
  case ALTIVEC_BUILTIN_VADDUHM:
  case ALTIVEC_BUILTIN_VADDUWM:
    Result = Builder.CreateAdd(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VSUBFP:
    Result = Builder.CreateFSub(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VSUBUBM:
  case ALTIVEC_BUILTIN_VSUBUHM:
  case ALTIVEC_BUILTIN_VSUBUWM:
    Result = Builder.CreateSub(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VAND:
    Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VANDC:
    Ops[1] = Builder.CreateNot(Ops[1], "tmp");
    Result = Builder.CreateAnd(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VOR:
    Result = Builder.CreateOr(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_VNOR:
    Result = Builder.CreateOr(Ops[0], Ops[1], "tmp");
    Result = Builder.CreateNot(Result, "tmp");
    return true;
  case ALTIVEC_BUILTIN_VXOR:
    Result = Builder.CreateXor(Ops[0], Ops[1], "tmp");
    return true;
  case ALTIVEC_BUILTIN_LVSL:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvsl,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVSR:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvsr,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVXL:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvxl,
                         ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVEBX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvebx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVEHX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvehx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_LVEWX:
    MergeIntPtrOperand(this, 0, Intrinsic::ppc_altivec_lvewx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVEBX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvebx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVEHX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvehx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVEWX:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvewx,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_STVXL:
    MergeIntPtrOperand(this, 1, Intrinsic::ppc_altivec_stvxl,
                       ResultType, Ops, Builder, Result);
    return true;
  case ALTIVEC_BUILTIN_VSPLTISB:
    if (Constant *Elt = dyn_cast<ConstantInt>(Ops[0])) {
      Elt = ConstantExpr::getIntegerCast(Elt, Type::getInt8Ty(Context), true);
      Result = BuildVector(Elt, Elt, Elt, Elt,  Elt, Elt, Elt, Elt,
                           Elt, Elt, Elt, Elt,  Elt, Elt, Elt, Elt, NULL);
    } else {
      error("%Helement must be an immediate", &EXPR_LOCATION(exp));
      Result = UndefValue::get(VectorType::get(Type::getInt8Ty(Context), 16));
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTISH:
    if (Constant *Elt = dyn_cast<ConstantInt>(Ops[0])) {
      Elt = ConstantExpr::getIntegerCast(Elt, Type::getInt16Ty(Context), true);
      Result = BuildVector(Elt, Elt, Elt, Elt,  Elt, Elt, Elt, Elt, NULL);
    } else {
      error("%Helement must be an immediate", &EXPR_LOCATION(exp));
      Result = UndefValue::get(VectorType::get(Type::getInt16Ty(Context), 8));
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTISW:
    if (Constant *Elt = dyn_cast<ConstantInt>(Ops[0])) {
      Elt = ConstantExpr::getIntegerCast(Elt, Type::getInt32Ty(Context), true);
      Result = BuildVector(Elt, Elt, Elt, Elt, NULL);
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = UndefValue::get(VectorType::get(Type::getInt32Ty(Context), 4));
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTB:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  EV, EV, EV, EV, EV, EV, EV, EV,
                                  EV, EV, EV, EV, EV, EV, EV, EV);
    } else {
      error("%Helement number must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTH:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      // gcc accepts anything up to 31, and there is code that tests for it, 
      // although it doesn't seem to make sense.  Hardware behaves as if mod 8.
      if (EV>7 && EV<=31)
        EV = EV%8;
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  EV, EV, EV, EV, EV, EV, EV, EV);
    } else {
      error("%Helement number must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VSPLTW:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      // gcc accepts anything up to 31, and there is code that tests for it, 
      // although it doesn't seem to make sense.  Hardware behaves as if mod 4.
      if (EV>3 && EV<=31)
        EV = EV%4;
      Result = BuildVectorShuffle(Ops[0], Ops[0], EV, EV, EV, EV);
    } else {
      error("%Helement number must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VSLDOI_16QI:
  case ALTIVEC_BUILTIN_VSLDOI_8HI:
  case ALTIVEC_BUILTIN_VSLDOI_4SI:
  case ALTIVEC_BUILTIN_VSLDOI_4SF:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[2])) {
      /* Map all of these to a shuffle. */
      unsigned Amt = Elt->getZExtValue() & 15;
      VectorType *v16i8 = VectorType::get(Type::getInt8Ty(Context), 16);
      Ops[0] = Builder.CreateBitCast(Ops[0], v16i8, "tmp");
      Ops[1] = Builder.CreateBitCast(Ops[1], v16i8, "tmp");
      Result = BuildVectorShuffle(Ops[0], Ops[1],
                                  Amt, Amt+1, Amt+2, Amt+3,
                                  Amt+4, Amt+5, Amt+6, Amt+7,
                                  Amt+8, Amt+9, Amt+10, Amt+11,
                                  Amt+12, Amt+13, Amt+14, Amt+15);
      /* Make sure result type matches. */
      Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    } else {
      error("%Hshift amount must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case ALTIVEC_BUILTIN_VPKUHUM:
    Ops[0] = Builder.CreateBitCast(Ops[0], ResultType, "tmp");
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType, "tmp");
    Result = BuildVectorShuffle(Ops[0], Ops[1], 1, 3, 5, 7, 9, 11, 13, 15,
                                17, 19, 21, 23, 25, 27, 29, 31);
    return true;
  case ALTIVEC_BUILTIN_VPKUWUM:
    Ops[0] = Builder.CreateBitCast(Ops[0], ResultType, "tmp");
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType, "tmp");
    Result = BuildVectorShuffle(Ops[0], Ops[1], 1, 3, 5, 7, 9, 11, 13, 15);
    return true;
  case ALTIVEC_BUILTIN_VMRGHB:
    Result = BuildVectorShuffle(Ops[0], Ops[1],
                                0, 16, 1, 17, 2, 18, 3, 19,
                                4, 20, 5, 21, 6, 22, 7, 23);
    return true;
  case ALTIVEC_BUILTIN_VMRGHH:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 8, 1, 9, 2, 10, 3, 11);
    return true;
  case ALTIVEC_BUILTIN_VMRGHW:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 4, 1, 5);
    return true;
  case ALTIVEC_BUILTIN_VMRGLB:
    Result = BuildVectorShuffle(Ops[0], Ops[1],
                                 8, 24,  9, 25, 10, 26, 11, 27,
                                12, 28, 13, 29, 14, 30, 15, 31);
    return true;
  case ALTIVEC_BUILTIN_VMRGLH:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 12, 5, 13, 6, 14, 7, 15);
    return true;
  case ALTIVEC_BUILTIN_VMRGLW:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 6, 3, 7);
    return true;
  case ALTIVEC_BUILTIN_ABS_V4SF: {
    // and out sign bits
    VectorType *v4i32 = VectorType::get(Type::getInt32Ty(Context), 4);
    Ops[0] = Builder.CreateBitCast(Ops[0], v4i32, "tmp");
    Constant *C = ConstantInt::get(Type::getInt32Ty(Context), 0x7FFFFFFF);
    C = ConstantVector::get(std::vector<Constant*>(4, C));
    Result = Builder.CreateAnd(Ops[0], C, "tmp");
    Result = Builder.CreateBitCast(Result, ResultType, "tmp");
    return true;
  }
  case ALTIVEC_BUILTIN_ABS_V4SI:
  case ALTIVEC_BUILTIN_ABS_V8HI:
  case ALTIVEC_BUILTIN_ABS_V16QI: { // iabs(x) -> smax(x, 0-x)
    Result = Builder.CreateNeg(Ops[0], "tmp");
    // get the right smax intrinsic.
    static const Intrinsic::ID smax_iid[3] = {
      Intrinsic::ppc_altivec_vmaxsw,
      Intrinsic::ppc_altivec_vmaxsh,
      Intrinsic::ppc_altivec_vmaxsb
    };
    const VectorType *PTy = cast<VectorType>(ResultType);
    unsigned N = GetAltivecTypeNumFromType(PTy->getElementType());
    Function *smax = Intrinsic::getDeclaration(TheModule, smax_iid[N]);
    Value *ActualOps[] = { Ops[0], Result };
    Result = Builder.CreateCall(smax, ActualOps, ActualOps+2, "tmp");
    return true;
  }
  case ALTIVEC_BUILTIN_ABSS_V4SI:
  case ALTIVEC_BUILTIN_ABSS_V8HI:
  case ALTIVEC_BUILTIN_ABSS_V16QI: { // iabss(x) -> smax(x, satsub(0,x))
    // get the right smax/subs intrinsics.
    static const Intrinsic::ID smax_iid[3] = {
      Intrinsic::ppc_altivec_vmaxsw,
      Intrinsic::ppc_altivec_vmaxsh,
      Intrinsic::ppc_altivec_vmaxsb
    };
    static const Intrinsic::ID subss_iid[3] = {
      Intrinsic::ppc_altivec_vsubsws,
      Intrinsic::ppc_altivec_vsubshs,
      Intrinsic::ppc_altivec_vsubsbs
    };
    
    // get the right satsub intrinsic.
    const VectorType *PTy = cast<VectorType>(ResultType);
    unsigned N = GetAltivecTypeNumFromType(PTy->getElementType());
    Function *smax = Intrinsic::getDeclaration(TheModule, smax_iid[N]);
    Function *subss = Intrinsic::getDeclaration(TheModule, subss_iid[N]);

    Value *ActualOps[] = {Constant::getNullValue(ResultType), Ops[0] };
    Result = Builder.CreateCall(subss, ActualOps, ActualOps+2, "tmp");
    ActualOps[0] = Ops[0];
    ActualOps[1] = Result;
    Result = Builder.CreateCall(smax, ActualOps, ActualOps+2, "tmp");
    return true;
  }
  case ALTIVEC_BUILTIN_VPERM_4SI:
  case ALTIVEC_BUILTIN_VPERM_4SF:
  case ALTIVEC_BUILTIN_VPERM_8HI:
  case ALTIVEC_BUILTIN_VPERM_16QI: {
    // Operation is identical on all types; we have a single intrinsic.
    const Type *VecTy = VectorType::get(Type::getInt32Ty(Context), 4);
    Value *Op0 = CastToType(Instruction::BitCast, Ops[0], VecTy);
    Value *Op1 = CastToType(Instruction::BitCast, Ops[1], VecTy);
    Value *ActualOps[] = { Op0, Op1, Ops[2]};
    Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule, 
                                          Intrinsic::ppc_altivec_vperm), 
                                ActualOps, ActualOps+3, "tmp");
    Result = CastToType(Instruction::BitCast, Result, Ops[0]->getType());
    return true;
  }
  case ALTIVEC_BUILTIN_VSEL_4SI:
  case ALTIVEC_BUILTIN_VSEL_4SF:
  case ALTIVEC_BUILTIN_VSEL_8HI:
  case ALTIVEC_BUILTIN_VSEL_16QI: {
    // Operation is identical on all types; we have a single intrinsic.
    const Type *VecTy = VectorType::get(Type::getInt32Ty(Context), 4);
    Value *Op0 = CastToType(Instruction::BitCast, Ops[0], VecTy);
    Value *Op1 = CastToType(Instruction::BitCast, Ops[1], VecTy);
    Value *Op2 = CastToType(Instruction::BitCast, Ops[2], VecTy);
    Value *ActualOps[] = { Op0, Op1, Op2 };
    Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule, 
                                          Intrinsic::ppc_altivec_vsel), 
                                ActualOps, ActualOps+3, "tmp");
    Result = CastToType(Instruction::BitCast, Result, Ops[0]->getType());
    return true;
  }
  }

  return false;
}

static unsigned count_num_registers_uses(std::vector<const Type*> &ScalarElts) {
  unsigned NumGPRs = 0;
  for (unsigned i = 0, e = ScalarElts.size(); i != e; ++i) {
    if (NumGPRs >= 8)
      break;
    const Type *Ty = ScalarElts[i];
    if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
      abort();
    } else if (Ty->isPointerTy()) {
      NumGPRs++;
    } else if (Ty->isIntegerTy()) {
      unsigned TypeSize = Ty->getPrimitiveSizeInBits();
      unsigned NumRegs = (TypeSize + 31) / 32;

      NumGPRs += NumRegs;
    } else if (Ty->isVoidTy()) {
      // Padding bytes that are not passed anywhere
      ;
    } else {
      // Floating point scalar argument.
      assert(Ty->isFloatingPointTy() && Ty->isPrimitiveType() &&
             "Expecting a floating point primitive type!");
    }
  }
  return NumGPRs < 8 ? NumGPRs : 8;
}

/// _Complex arguments are never split, thus their two scalars are either
/// passed both in argument registers or both on the stack. Also _Complex
/// arguments are always passed in general purpose registers, never in
/// Floating-point registers or vector registers.
bool llvm_rs6000_try_pass_aggregate_custom(tree type,
					   std::vector<const Type*> &ScalarElts,
					   const CallingConv::ID &CC,
					   struct DefaultABIClient* C) {
  if (!isSVR4ABI())
    return false;

  // Eight GPR's are availabe for parameter passing.
  const unsigned NumArgRegs = 8;
  unsigned NumGPR = count_num_registers_uses(ScalarElts);
  const Type *Ty = ConvertType(type);
  const Type* Int32Ty = Type::getInt32Ty(getGlobalContext());
  if (Ty->isSingleValueType()) {
    if (Ty->isIntegerTy()) {
      unsigned TypeSize = Ty->getPrimitiveSizeInBits();

      // Determine how many general purpose registers are needed for the
      // argument.
      unsigned NumRegs = (TypeSize + 31) / 32;

      // Make sure argument registers are aligned. 64-bit arguments are put in
      // a register pair which starts with an odd register number.
      if (TypeSize == 64 && (NumGPR % 2) == 1) {
	NumGPR++;
	ScalarElts.push_back(Int32Ty);
	C->HandlePad(Int32Ty);
      }

      if (NumGPR > (NumArgRegs - NumRegs)) {
	for (unsigned int i = 0; i < NumArgRegs - NumGPR; ++i) {
	  ScalarElts.push_back(Int32Ty);
	  C->HandlePad(Int32Ty);
	}
      }
    } else if (!(Ty->isFloatingPointTy() ||
		 Ty->isVectorTy()   ||
		 Ty->isPointerTy())) {
      abort();
    }

    C->HandleScalarArgument(Ty, type);
    ScalarElts.push_back(Ty);
    return true;
  }
  if (TREE_CODE(type) == COMPLEX_TYPE) {
    unsigned SrcSize = int_size_in_bytes(type);
    unsigned NumRegs = (SrcSize + 3) / 4;
    std::vector<const Type*> Elts;

    // This looks very strange, but matches the old code.
    if (SrcSize == 8) {
      // Make sure argument registers are aligned. 64-bit arguments are put in
      // a register pair which starts with an odd register number.
      if (NumGPR % 2 == 1) {
	NumGPR++;
	ScalarElts.push_back(Int32Ty);
	C->HandlePad(Int32Ty);
      }
    }

    if (NumGPR > (NumArgRegs - NumRegs)) {
      for (unsigned int i = 0; i < NumArgRegs - NumGPR; ++i) {
        ScalarElts.push_back(Int32Ty);
        C->HandlePad(Int32Ty);
      }
    }
    for (unsigned int i = 0; i < NumRegs; ++i) {
      Elts.push_back(Int32Ty);
    }
    const StructType *STy = StructType::get(getGlobalContext(), Elts, false);
    for (unsigned int i = 0; i < NumRegs; ++i) {
      C->EnterField(i, STy);
      C->HandleScalarArgument(Int32Ty, 0);
      ScalarElts.push_back(Int32Ty);
      C->ExitField();
    }
    return true;
  }
  return false;
}

/* Target hook for llvm-abi.h. It returns true if an aggregate of the
   specified type should be passed using the byval mechanism. */
bool llvm_rs6000_should_pass_aggregate_byval(tree TreeType, const Type *Ty) {
  /* FIXME byval not implemented for ppc64. */
  if (TARGET_64BIT)
    return false;

  HOST_WIDE_INT Bytes = (TYPE_MODE(TreeType) == BLKmode) ? 
                        int_size_in_bytes(TreeType) : 
                        (int) GET_MODE_SIZE(TYPE_MODE(TreeType));
  
  // The SVR4 ABI always passes _Complex types in registers (or on the stack if
  // there are no free argument registers left).
  if (isSVR4ABI() && (TREE_CODE(TreeType) == COMPLEX_TYPE))
   return false;

  // Zero sized array, struct, or class, ignored.
  if (Bytes == 0)
    return false;
  
  // With the SVR4 ABI, pass all other aggregates with the byval mechanism.
  if (isSVR4ABI())
    return true;

  // Large types always use byval.  If this is a small fixed size type, 
  // investigate it.
  if (Bytes <= 0 || Bytes > 16)
    return true;

  // ppc32 passes aggregates by copying, either in int registers or on the 
  // stack.
  const StructType *STy = dyn_cast<StructType>(Ty);
  if (!STy) return true;

  // A struct containing only a float, double or vector field, possibly with
  // some zero-length fields as well, must be passed as the field type.
  // Note this does not apply to long double.
  // This is required for ABI correctness.  
  tree tType = isSingleElementStructOrArray(TreeType, true, false);
  if (tType && int_size_in_bytes(tType)==Bytes && TYPE_MODE(tType)!=TFmode &&
      (TREE_CODE(tType)!=VECTOR_TYPE || Bytes==16))
    return false;

  return true;
}

/* Target hook for llvm-abi.h. It returns true if an aggregate of the
   specified type should be passed in a number of registers of mixed types.
   It also returns a vector of types that correspond to the registers used
   for parameter passing. */
bool 
llvm_rs6000_should_pass_aggregate_in_mixed_regs(tree TreeType, const Type* Ty,
                                              std::vector<const Type*>&Elts) {
  // FIXME there are plenty of ppc64 cases that need this.
  if (TARGET_64BIT)
    return false;
  
  HOST_WIDE_INT SrcSize = int_size_in_bytes(TreeType);
  
  // With the SVR4 ABI, aggregates of type _Complex are the only aggregates
  // which are passed in registers.
  if (isSVR4ABI() && TREE_CODE(TreeType) == COMPLEX_TYPE) {
    switch (SrcSize) {
    default:
      abort();
    case 32:
      // Pass _Complex long double in eight registers.
      Elts.push_back(Type::getInt32Ty(Context));
      Elts.push_back(Type::getInt32Ty(Context));
      Elts.push_back(Type::getInt32Ty(Context));
      Elts.push_back(Type::getInt32Ty(Context));
      // FALLTRHOUGH
    case 16:
      // Pass _Complex long long/double in four registers.
      Elts.push_back(Type::getInt32Ty(Context));
      Elts.push_back(Type::getInt32Ty(Context));
      Elts.push_back(Type::getInt32Ty(Context));
      Elts.push_back(Type::getInt32Ty(Context));
      break;
    case 8:
      // Pass _Complex int/long/float in two registers.
      // They require 8-byte alignment, thus they are passed with i64,
      // which will be decomposed into two i32 elements. The first element will
      // have the split attribute set, which is used to trigger 8-byte alignment
      // in the backend.
      Elts.push_back(Type::getInt64Ty(Context));
      break;
    case 4:
    case 2:
      // Pass _Complex short/char in one register.
      Elts.push_back(Type::getInt32Ty(Context));
      break;
    }
    
    return true;
  }

  // With the SVR4 ABI, no other aggregates are passed in registers.
  if (isSVR4ABI())
    return false;

  // If this is a small fixed size type, investigate it.
  if (SrcSize <= 0 || SrcSize > 16)
    return false;

  const StructType *STy = dyn_cast<StructType>(Ty);
  if (!STy) return false;

  // A struct containing only a float, double or Altivec field, possibly with
  // some zero-length fields as well, must be passed as the field type.
  // Note this does not apply to long double, nor generic vectors.
  // Other single-element structs may be passed this way as well, but
  // only if the type size matches the element's type size (structs that
  // violate this can be created with __aligned__).
  tree tType = isSingleElementStructOrArray(TreeType, true, false);
  if (tType && int_size_in_bytes(tType)==SrcSize && TYPE_MODE(tType)!=TFmode &&
      (TREE_CODE(tType)!=VECTOR_TYPE || SrcSize==16)) {
    Elts.push_back(ConvertType(tType));
    return true;
  }

  Elts.clear();
  return false;
}

/* Non-Altivec vectors are passed in integer regs. */
bool llvm_rs6000_should_pass_vector_in_integer_regs(tree type) {
  if (!TARGET_64BIT &&
    TREE_CODE(type) == VECTOR_TYPE &&
    TYPE_SIZE(type) && TREE_CODE(TYPE_SIZE(type))==INTEGER_CST &&
    TREE_INT_CST_LOW(TYPE_SIZE(type)) != 128)
    return true;
  return false;
}

/* (Generic) vectors 4 bytes long are returned as an int.
   Vectors 8 bytes long are returned as 2 ints. */
tree llvm_rs6000_should_return_vector_as_scalar(tree type, 
                                    bool isBuiltin ATTRIBUTE_UNUSED) {
  if (!TARGET_64BIT &&
      TREE_CODE(type) == VECTOR_TYPE &&
      TYPE_SIZE(type) &&
      TREE_CODE(TYPE_SIZE(type))==INTEGER_CST) {
    if (TREE_INT_CST_LOW(TYPE_SIZE(type))==32)
      return uint32_type_node;
    else if (TREE_INT_CST_LOW(TYPE_SIZE(type))==64)
      return uint64_type_node;
  }
  return 0;
}

/* Non-altivec vectors bigger than 8 bytes are returned by sret. */
bool llvm_rs6000_should_return_vector_as_shadow(tree type, 
                                      bool isBuiltin ATTRIBUTE_UNUSED) {
  if (!TARGET_64BIT &&
      TREE_CODE(type) == VECTOR_TYPE &&
      TYPE_SIZE(type) &&
      TREE_CODE(TYPE_SIZE(type))==INTEGER_CST &&
      TREE_INT_CST_LOW(TYPE_SIZE(type))>64 &&
      TREE_INT_CST_LOW(TYPE_SIZE(type))!=128)
    return true;
  return false;
}

/* LLVM LOCAL end (ENTIRE FILE!)  */

