/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* High-level LLVM backend interface 
Copyright (C) 2005 Free Software Foundation, Inc.
Contributed by Evan Cheng (evan.cheng@apple.com)

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
// This is a C++ source file that implements specific llvm alpha ABI.
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

static LLVMContext &Context = getGlobalContext();

enum alpha_builtin
{
  ALPHA_BUILTIN_CMPBGE,
  ALPHA_BUILTIN_EXTBL,
  ALPHA_BUILTIN_EXTWL,
  ALPHA_BUILTIN_EXTLL,
  ALPHA_BUILTIN_EXTQL,
  ALPHA_BUILTIN_EXTWH,
  ALPHA_BUILTIN_EXTLH,
  ALPHA_BUILTIN_EXTQH,
  ALPHA_BUILTIN_INSBL,
  ALPHA_BUILTIN_INSWL,
  ALPHA_BUILTIN_INSLL,
  ALPHA_BUILTIN_INSQL,
  ALPHA_BUILTIN_INSWH,
  ALPHA_BUILTIN_INSLH,
  ALPHA_BUILTIN_INSQH,
  ALPHA_BUILTIN_MSKBL,
  ALPHA_BUILTIN_MSKWL,
  ALPHA_BUILTIN_MSKLL,
  ALPHA_BUILTIN_MSKQL,
  ALPHA_BUILTIN_MSKWH,
  ALPHA_BUILTIN_MSKLH,
  ALPHA_BUILTIN_MSKQH,
  ALPHA_BUILTIN_UMULH,
  ALPHA_BUILTIN_ZAP,
  ALPHA_BUILTIN_ZAPNOT,
  ALPHA_BUILTIN_AMASK,
  ALPHA_BUILTIN_IMPLVER,
  ALPHA_BUILTIN_RPCC,
  ALPHA_BUILTIN_THREAD_POINTER,
  ALPHA_BUILTIN_SET_THREAD_POINTER,

  /* TARGET_MAX */
  ALPHA_BUILTIN_MINUB8,
  ALPHA_BUILTIN_MINSB8,
  ALPHA_BUILTIN_MINUW4,
  ALPHA_BUILTIN_MINSW4,
  ALPHA_BUILTIN_MAXUB8,
  ALPHA_BUILTIN_MAXSB8,
  ALPHA_BUILTIN_MAXUW4,
  ALPHA_BUILTIN_MAXSW4,
  ALPHA_BUILTIN_PERR,
  ALPHA_BUILTIN_PKLB,
  ALPHA_BUILTIN_PKWB,
  ALPHA_BUILTIN_UNPKBL,
  ALPHA_BUILTIN_UNPKBW,

  /* TARGET_CIX */
  ALPHA_BUILTIN_CTTZ,
  ALPHA_BUILTIN_CTLZ,
  ALPHA_BUILTIN_CTPOP,

  ALPHA_BUILTIN_max
};

/* TargetIntrinsicLower - For builtins that we want to expand to normal LLVM
 * code, emit the code now.  If we can handle the code, this macro should emit
 * the code, return true.
 */
bool TreeToLLVM::TargetIntrinsicLower(tree exp,
                                      unsigned FnCode,
                                      const MemRef *DestLoc,
                                      Value *&Result,
                                      const Type *ResultType,
                                      std::vector<Value*> &Ops) {
  switch (FnCode) {
  case ALPHA_BUILTIN_UMULH: {
    Value *Arg0 = Ops[0];
    Value *Arg1 = Ops[1];
    Arg0 = Builder.CreateZExt(Arg0, IntegerType::get(Context, 128));
    Arg1 = Builder.CreateZExt(Arg1, IntegerType::get(Context, 128));
    Result = Builder.CreateMul(Arg0, Arg1);
    Result = Builder.CreateLShr(Result, ConstantInt::get(
                                               Type::getInt64Ty(Context), 64));
    Result = Builder.CreateTrunc(Result, Type::getInt64Ty(Context));
    return true;
  }
  case ALPHA_BUILTIN_CMPBGE: {
    Value *Arg0 = Ops[0];
    Value *Arg1 = Ops[1];
    Value* cmps[8];
    for (unsigned x = 0; x < 8; ++x) {
      Value* LHS = Builder.CreateLShr(Arg0, ConstantInt::get(
                                              Type::getInt64Ty(Context), x*8));
      LHS = Builder.CreateTrunc(LHS, Type::getInt8Ty(Context));
      Value* RHS = Builder.CreateLShr(Arg1, ConstantInt::get(
                                              Type::getInt64Ty(Context), x*8));
      RHS = Builder.CreateTrunc(RHS, Type::getInt8Ty(Context));
      Value* cmps = Builder.CreateICmpUGE(LHS, RHS);
      cmps = Builder.CreateIsNotNull(cmps);
      cmps = Builder.CreateZExt(cmps, Type::getInt64Ty(Context));
      cmps = Builder.CreateShl(cmps, ConstantInt::get(
                                                Type::getInt64Ty(Context), x));
      if (x == 0)
	Result = cmps;
      else
	Result = Builder.CreateOr(Result,cmps);
    }
    return true;
  }
  case ALPHA_BUILTIN_EXTBL:
  case ALPHA_BUILTIN_EXTWL:
  case ALPHA_BUILTIN_EXTLL:
  case ALPHA_BUILTIN_EXTQL: {
    unsigned long long mask = 0;
    switch (FnCode) {
    case ALPHA_BUILTIN_EXTBL: mask = 0x00000000000000FFULL; break;
    case ALPHA_BUILTIN_EXTWL: mask = 0x000000000000FFFFULL; break;
    case ALPHA_BUILTIN_EXTLL: mask = 0x00000000FFFFFFFFULL; break;
    case ALPHA_BUILTIN_EXTQL: mask = 0xFFFFFFFFFFFFFFFFULL; break;
    };
    Value *Arg0 = Ops[0];
    Value *Arg1 = Builder.CreateAnd(Ops[1], ConstantInt::get(
                                                Type::getInt64Ty(Context), 7));
    Arg0 = Builder.CreateLShr(Arg0, Arg1);
    Result = Builder.CreateAnd(Arg0, ConstantInt::get(
                                             Type::getInt64Ty(Context), mask));
    return true;
  }
  case ALPHA_BUILTIN_EXTWH:
  case ALPHA_BUILTIN_EXTLH:
  case ALPHA_BUILTIN_EXTQH: {
    unsigned long long mask = 0;
    switch (FnCode) {
    case ALPHA_BUILTIN_EXTWH: mask = 0x000000000000FFFFULL; break;
    case ALPHA_BUILTIN_EXTLH: mask = 0x00000000FFFFFFFFULL; break;
    case ALPHA_BUILTIN_EXTQH: mask = 0xFFFFFFFFFFFFFFFFULL; break;
    };
    Value *Arg0 = Ops[0];
    Value *Arg1 = Builder.CreateAnd(Ops[1], ConstantInt::get(
                                                Type::getInt64Ty(Context), 7));
    Arg1 = Builder.CreateSub(ConstantInt::get(
                                         Type::getInt64Ty(Context), 64), Arg1);
    Arg0 = Builder.CreateShl(Arg0, Arg1);
    Result = Builder.CreateAnd(Arg0, ConstantInt::get(
                                             Type::getInt64Ty(Context), mask));
    return true;
  }

  default: break;
  }

  return false;
}

bool llvm_alpha_should_return_scalar_as_shadow(const Type* Ty) {
  if (Ty && Ty->isIntegerTy(128))
    return true;
  return false;
}

/* LLVM LOCAL end (ENTIRE FILE!)  */
