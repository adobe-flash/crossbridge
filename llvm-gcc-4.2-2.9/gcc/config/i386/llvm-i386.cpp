/* LLVM LOCAL begin (ENTIRE FILE!)  */
#ifdef ENABLE_LLVM
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
// This is a C++ source file that implements specific llvm IA-32 ABI.
//===----------------------------------------------------------------------===//

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm-i386-target.h"

extern "C" {
#include "toplev.h"
}

static LLVMContext &Context = getGlobalContext();

/* ConvertToX86_MMXTy - Convert a value from an "old" MMX type to the new x86mmx
 * type.
 */
static Value *ConvertToX86_MMXTy(Value *Val, LLVMBuilder &Builder) {
  static const Type *MMXTy = Type::getX86_MMXTy(Context);
  if (Val->getType() == MMXTy) return Val;
  return Builder.CreateBitCast(Val, MMXTy, "mmx_var");
}

namespace Encode {
  enum {
    Return = 0x01,
    Arg0   = 0x02,
    Arg1   = 0x04
  };
}

/* CreateMMXIntrinsicCall - Create an intrinsic call to an MMX intrinsic.
 */
static void CreateMMXIntrinsicCall(Intrinsic::ID IntID, Value *&Result,
                                   std::vector<Value*> &Ops,
                                   unsigned EncodePattern,
                                   LLVMBuilder &Builder) {
  unsigned NumOps = Ops.size();
  static const Type *MMXTy = Type::getX86_MMXTy(Context);
  Function *Func = Intrinsic::getDeclaration(TheModule, IntID);

  Value *Arg0 = 0, *Arg1 = 0;
  if (NumOps >= 1)
    Arg0 = EncodePattern & Encode::Arg0 ?
      ConvertToX86_MMXTy(Ops[0], Builder) : Ops[0];

  if (NumOps >= 2)
    Arg1 = EncodePattern & Encode::Arg1 ?
      ConvertToX86_MMXTy(Ops[1], Builder) : Ops[1];

  Value *CallOps[10] = { Arg0, Arg1 };

  for (unsigned I = 2; I < NumOps && I < 10; ++I)
    CallOps[I] = Ops[I];

  Result = Builder.CreateCall(Func, CallOps, CallOps + NumOps);

  if (EncodePattern & Encode::Return)
    Result = Builder.CreateBitCast(Result, MMXTy);
}

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
  default: break;

  /* MMX Builtins */
  case IX86_BUILTIN_PADDB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_padd_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PADDW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_padd_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PADDD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_padd_d, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PADDQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_padd_q, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PADDSB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_padds_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PADDSW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_padds_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PADDUSB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_paddus_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PADDUSW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_paddus_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psub_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psub_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psub_d, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psub_q, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBSB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psubs_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBSW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psubs_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBUSB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psubus_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSUBUSW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psubus_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMULHW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmulh_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMULLW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmull_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMULHUW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmulhu_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMULUDQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmulu_dq, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMADDWD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmadd_wd, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PAND:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pand, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PANDN:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pandn, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_POR:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_por, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PXOR:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pxor, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PAVGB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pavg_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PAVGW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pavg_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMAXUB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmaxu_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMAXSW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmaxs_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMINUB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pminu_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMINSW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmins_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSADBW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psad_bw, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSLLW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psll_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSLLD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psll_d, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSLLQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psll_q, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRLW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrl_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRLD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrl_d, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRLQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrl_q, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRAW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psra_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRAD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psra_d, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PSLLWI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pslli_w, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PSLLDI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pslli_d, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PSLLQI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pslli_q, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRLWI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrli_w, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRLDI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrli_d, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRLQI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrli_q, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRAWI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrai_w, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PSRADI:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_psrai_d, Result, Ops,
                           Encode::Return | Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_PACKSSWB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_packsswb, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PACKSSDW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_packssdw, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PACKUSWB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_packuswb, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PUNPCKHBW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_punpckhbw, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PUNPCKHWD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_punpckhwd, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PUNPCKHDQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_punpckhdq, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PUNPCKLBW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_punpcklbw, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PUNPCKLWD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_punpcklwd, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PUNPCKLDQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_punpckldq, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;

  case IX86_BUILTIN_PCMPEQB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pcmpeq_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PCMPEQW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pcmpeq_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PCMPEQD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pcmpeq_d, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PCMPGTB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pcmpgt_b, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PCMPGTW:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pcmpgt_w, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PCMPGTD:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pcmpgt_d, Result, Ops,
                           Encode::Return | Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_MASKMOVQ:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_maskmovq, Result, Ops,
                           Encode::Arg0 | Encode::Arg1,
                           Builder);
    return true;
  case IX86_BUILTIN_PMOVMSKB:
    CreateMMXIntrinsicCall(Intrinsic::x86_mmx_pmovmskb, Result, Ops,
                           Encode::Arg0,
                           Builder);
    return true;
  case IX86_BUILTIN_MOVNTQ: {
    static const Type *MMXTy = Type::getX86_MMXTy(Context);
    Function *Func = Intrinsic::getDeclaration(TheModule,
                                               Intrinsic::x86_mmx_movnt_dq);
    const PointerType *PTy = cast<PointerType>(Ops[0]->getType());
    Value *Arg0 = 0;
    if (PTy->getElementType() == MMXTy)
      Arg0 = Ops[0];
    else
      Arg0 = Builder.CreateBitCast(Ops[0],
                                   Type::getX86_MMXPtrTy(Context),
                                   "mmx_ptr_var");

    Value *Arg1 = ConvertToX86_MMXTy(Ops[1], Builder);
    Value *CallOps[2] = { Arg0, Arg1 };
    Result = Builder.CreateCall(Func, CallOps, CallOps + 2);
    return true;
  }
  case IX86_BUILTIN_PALIGNR: {
    static const Type *MMXTy = Type::getX86_MMXTy(Context);
    Function *Func = Intrinsic::getDeclaration(TheModule,
                                               Intrinsic::x86_mmx_palignr_b);
    Value *Arg0 = ConvertToX86_MMXTy(Ops[0], Builder);
    Value *Arg1 = ConvertToX86_MMXTy(Ops[1], Builder);
    Value *Arg2 = Builder.CreateTrunc(Ops[2], Type::getInt8Ty(Context),
                                      "i32_to_i8");

    Value *CallOps[3] = { Arg0, Arg1, Arg2 };
    Result = Builder.CreateCall(Func, CallOps, CallOps + 3);
    Result = Builder.CreateBitCast(Result, MMXTy);
    return true;
  }
  case IX86_BUILTIN_VEC_INIT_V8QI: {
    // The operands to this vector init are helpfully turned into i32. Truncate
    // them to i8, which is what's expected.
    for (unsigned I = 0, E = Ops.size(); I != E; ++I)
      Ops[I] = Builder.CreateTrunc(Ops[I], Type::getInt8Ty(Context),
                                   "i32_to_i8");

    Value *V = BuildVector(Ops);
    Result = Builder.CreateBitCast(V, Type::getX86_MMXTy(Context));
    return true;
  }
  case IX86_BUILTIN_VEC_INIT_V4HI: {
    // The operands to this vector init are helpfully turned into i32. Truncate
    // them to i16, which is what's expected.
    for (unsigned I = 0, E = Ops.size(); I != E; ++I)
      Ops[I] = Builder.CreateTrunc(Ops[I], Type::getInt16Ty(Context),
                                     "i32_to_i16");

    Value *V = BuildVector(Ops);
    Result = Builder.CreateBitCast(V, Type::getX86_MMXTy(Context));
    return true;
  }
  case IX86_BUILTIN_VEC_INIT_V2SI: {
    Value *V = BuildVector(Ops);
    Result = Builder.CreateBitCast(V, Type::getX86_MMXTy(Context));
    return true;
  }
  case IX86_BUILTIN_VEC_EXT_V2SI:
    Result = Builder.CreateBitCast(Ops[0], 
              VectorType::get(Type::getInt32Ty(Context), 2));
    Result = Builder.CreateExtractElement(Result,
              ConstantInt::get(Type::getInt32Ty(Context), 0));
    return true;
  case IX86_BUILTIN_ADDPS:
  case IX86_BUILTIN_ADDPD:
    Result = Builder.CreateFAdd(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_PADDB128:
  case IX86_BUILTIN_PADDW128:
  case IX86_BUILTIN_PADDD128:
  case IX86_BUILTIN_PADDQ128:
    Result = Builder.CreateAdd(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_SUBPS:
  case IX86_BUILTIN_SUBPD:
    Result = Builder.CreateFSub(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_PSUBB128:
  case IX86_BUILTIN_PSUBW128:
  case IX86_BUILTIN_PSUBD128:
  case IX86_BUILTIN_PSUBQ128:
    Result = Builder.CreateSub(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_MULPS:
  case IX86_BUILTIN_MULPD:
    Result = Builder.CreateFMul(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_PMULLW128:
  case IX86_BUILTIN_PMULLD128:
    Result = Builder.CreateMul(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_DIVPS:
  case IX86_BUILTIN_DIVPD:
    Result = Builder.CreateFDiv(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_PAND128:
    Result = Builder.CreateAnd(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_PANDN128:
    Ops[0] = Builder.CreateNot(Ops[0]);
    Result = Builder.CreateAnd(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_POR128:
    Result = Builder.CreateOr(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_PXOR128:
    Result = Builder.CreateXor(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_ANDPS:
  case IX86_BUILTIN_ORPS:
  case IX86_BUILTIN_XORPS:
  case IX86_BUILTIN_ANDNPS:
  case IX86_BUILTIN_ANDPD:
  case IX86_BUILTIN_ORPD:
  case IX86_BUILTIN_XORPD:
  case IX86_BUILTIN_ANDNPD:
    if (cast<VectorType>(ResultType)->getNumElements() == 4)  // v4f32
      Ops[0] = Builder.CreateBitCast(Ops[0], 
                                  VectorType::get(Type::getInt32Ty(Context), 4),
                                     "tmp");
    else                                                      // v2f64
      Ops[0] = Builder.CreateBitCast(Ops[0], 
                                 VectorType::get(Type::getInt64Ty(Context), 2),
                                     "tmp");
    
    Ops[1] = Builder.CreateBitCast(Ops[1], Ops[0]->getType());
    switch (FnCode) {
      case IX86_BUILTIN_ANDPS:
      case IX86_BUILTIN_ANDPD:
        Result = Builder.CreateAnd(Ops[0], Ops[1]);
        break;
      case IX86_BUILTIN_ORPS:
      case IX86_BUILTIN_ORPD:
        Result = Builder.CreateOr (Ops[0], Ops[1]);
         break;
      case IX86_BUILTIN_XORPS:
      case IX86_BUILTIN_XORPD:
        Result = Builder.CreateXor(Ops[0], Ops[1]);
        break;
      case IX86_BUILTIN_ANDNPS:
      case IX86_BUILTIN_ANDNPD:
        Ops[0] = Builder.CreateNot(Ops[0]);
        Result = Builder.CreateAnd(Ops[0], Ops[1]);
        break;
    }
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  case IX86_BUILTIN_SHUFPS:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[2])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[1],
                                  ((EV & 0x03) >> 0),   ((EV & 0x0c) >> 2),
                                  ((EV & 0x30) >> 4)+4, ((EV & 0xc0) >> 6)+4);
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case IX86_BUILTIN_SHUFPD:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[2])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[1],
                                  ((EV & 0x01) >> 0),   ((EV & 0x02) >> 1)+2);
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case IX86_BUILTIN_PSHUFD:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  ((EV & 0x03) >> 0),   ((EV & 0x0c) >> 2),
                                  ((EV & 0x30) >> 4),   ((EV & 0xc0) >> 6));
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    return true;
  case IX86_BUILTIN_PSHUFHW:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  0, 1, 2, 3,
                                  ((EV & 0x03) >> 0)+4, ((EV & 0x0c) >> 2)+4,
                                  ((EV & 0x30) >> 4)+4, ((EV & 0xc0) >> 6)+4);
      return true;
    }
    return false;
  case IX86_BUILTIN_PSHUFLW:
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[1])) {
      int EV = Elt->getZExtValue();
      Result = BuildVectorShuffle(Ops[0], Ops[0],
                                  ((EV & 0x03) >> 0),   ((EV & 0x0c) >> 2),
                                  ((EV & 0x30) >> 4),   ((EV & 0xc0) >> 6),
                                  4, 5, 6, 7);
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
    }
    
    return true;
  case IX86_BUILTIN_PUNPCKHBW128:
    Result = BuildVectorShuffle(Ops[0], Ops[1],  8, 24,  9, 25,
                                                10, 26, 11, 27,
                                                12, 28, 13, 29,
                                                14, 30, 15, 31);
    return true;
  case IX86_BUILTIN_PUNPCKHWD128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 12, 5, 13, 6, 14, 7, 15);
    return true;
  case IX86_BUILTIN_PUNPCKHDQ128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 6, 3, 7);
    return true;
  case IX86_BUILTIN_PUNPCKHQDQ128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 1, 3);
    return true;
  case IX86_BUILTIN_PUNPCKLBW128:
    Result = BuildVectorShuffle(Ops[0], Ops[1],  0, 16,  1, 17,
                                                 2, 18,  3, 19,
                                                 4, 20,  5, 21,
                                                 6, 22,  7, 23);
    return true;
  case IX86_BUILTIN_PUNPCKLWD128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 8, 1, 9, 2, 10, 3, 11);
    return true;
  case IX86_BUILTIN_PUNPCKLDQ128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 4, 1, 5);
    return true;
  case IX86_BUILTIN_PUNPCKLQDQ128:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 2);
    return true;
  case IX86_BUILTIN_UNPCKHPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 6, 3, 7);
    return true;
  case IX86_BUILTIN_UNPCKHPD:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 1, 3);
    return true;
  case IX86_BUILTIN_UNPCKLPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 4, 1, 5);
    return true;
  case IX86_BUILTIN_UNPCKLPD:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 2);
    return true;
  case IX86_BUILTIN_MOVHLPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 6, 7, 2, 3);
    return true;
  case IX86_BUILTIN_MOVLHPS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 1, 4, 5);
    return true;
  case IX86_BUILTIN_MOVSS:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 1, 2, 3);
    return true;
  case IX86_BUILTIN_MOVSD:
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 1);
    return true;
  case IX86_BUILTIN_MOVQ: {
    Value *Zero = ConstantInt::get(Type::getInt32Ty(Context), 0);
    Result = BuildVector(Zero, Zero, Zero, Zero, NULL);
    Result = BuildVectorShuffle(Result, Ops[0], 4, 5, 2, 3);
    return true;
  }
  case IX86_BUILTIN_LOADQ: {
    const PointerType *i64Ptr = Type::getInt64PtrTy(Context);
    Ops[0] = Builder.CreateBitCast(Ops[0], i64Ptr);
    Ops[0] = Builder.CreateLoad(Ops[0]);
    Value *Zero = ConstantInt::get(Type::getInt64Ty(Context), 0);
    Result = BuildVector(Zero, Zero, NULL);
    Value *Idx = ConstantInt::get(Type::getInt32Ty(Context), 0);
    Result = Builder.CreateInsertElement(Result, Ops[0], Idx);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_LOADUPS: {
    VectorType *v4f32 = VectorType::get(Type::getFloatTy(Context), 4);
    const PointerType *v4f32Ptr = v4f32->getPointerTo();
    Value *BC = Builder.CreateBitCast(Ops[0], v4f32Ptr);
    LoadInst *LI = Builder.CreateLoad(BC);
    LI->setAlignment(1);
    Result = LI;
    return true;
  }
  case IX86_BUILTIN_LOADUPD: {
    VectorType *v2f64 = VectorType::get(Type::getDoubleTy(Context), 2);
    const PointerType *v2f64Ptr = v2f64->getPointerTo();
    Value *BC = Builder.CreateBitCast(Ops[0], v2f64Ptr);
    LoadInst *LI = Builder.CreateLoad(BC);
    LI->setAlignment(1);
    Result = LI;
    return true;
  }
  case IX86_BUILTIN_LOADDQU: {
    VectorType *v16i8 = VectorType::get(Type::getInt8Ty(Context), 16);
    const PointerType *v16i8Ptr = v16i8->getPointerTo();
    Value *BC = Builder.CreateBitCast(Ops[0], v16i8Ptr);
    LoadInst *LI = Builder.CreateLoad(BC);
    LI->setAlignment(1);
    Result = LI;
    return true;
  }
  case IX86_BUILTIN_STOREUPS: {
    VectorType *v4f32 = VectorType::get(Type::getFloatTy(Context), 4);
    const PointerType *v4f32Ptr = v4f32->getPointerTo();
    Value *BC = Builder.CreateBitCast(Ops[0], v4f32Ptr);
    StoreInst *SI = Builder.CreateStore(Ops[1], BC);
    SI->setAlignment(1);
    return true;
  }
  case IX86_BUILTIN_STOREUPD: {
    VectorType *v2f64 = VectorType::get(Type::getDoubleTy(Context), 2);
    const PointerType *v2f64Ptr = v2f64->getPointerTo();
    Value *BC = Builder.CreateBitCast(Ops[0], v2f64Ptr);
    StoreInst *SI = Builder.CreateStore(Ops[1], BC);
    SI->setAlignment(1);
    return true;
  }
  case IX86_BUILTIN_STOREDQU: {
    VectorType *v16i8 = VectorType::get(Type::getInt8Ty(Context), 16);
    const PointerType *v16i8Ptr = v16i8->getPointerTo();
    Value *BC = Builder.CreateBitCast(Ops[0], v16i8Ptr);
    StoreInst *SI = Builder.CreateStore(Ops[1], BC);
    SI->setAlignment(1);
    return true;
  }
  case IX86_BUILTIN_LOADHPS: {
    const PointerType *f64Ptr = Type::getDoublePtrTy(Context);
    Ops[1] = Builder.CreateBitCast(Ops[1], f64Ptr);
    Value *Load = Builder.CreateLoad(Ops[1]);
    Ops[1] = BuildVector(Load, UndefValue::get(Type::getDoubleTy(Context)), NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType);
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 1, 4, 5);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_LOADLPS: {
    const PointerType *f64Ptr = Type::getDoublePtrTy(Context);
    Ops[1] = Builder.CreateBitCast(Ops[1], f64Ptr);
    Value *Load = Builder.CreateLoad(Ops[1]);
    Ops[1] = BuildVector(Load, UndefValue::get(Type::getDoubleTy(Context)), NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType);
    Result = BuildVectorShuffle(Ops[0], Ops[1], 4, 5, 2, 3);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_LOADHPD: {
    Value *Load = Builder.CreateLoad(Ops[1]);
    Ops[1] = BuildVector(Load, UndefValue::get(Type::getDoubleTy(Context)), NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType);
    Result = BuildVectorShuffle(Ops[0], Ops[1], 0, 2);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_LOADLPD: {
    Value *Load = Builder.CreateLoad(Ops[1]);
    Ops[1] = BuildVector(Load, UndefValue::get(Type::getDoubleTy(Context)), NULL);
    Ops[1] = Builder.CreateBitCast(Ops[1], ResultType);
    Result = BuildVectorShuffle(Ops[0], Ops[1], 2, 1);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_STOREHPS: {
    VectorType *v2f64 = VectorType::get(Type::getDoubleTy(Context), 2);
    const PointerType *f64Ptr = Type::getDoublePtrTy(Context);
    Ops[0] = Builder.CreateBitCast(Ops[0], f64Ptr);
    Value *Idx = ConstantInt::get(Type::getInt32Ty(Context), 1);
    Ops[1] = Builder.CreateBitCast(Ops[1], v2f64);
    Ops[1] = Builder.CreateExtractElement(Ops[1], Idx);
    Result = Builder.CreateStore(Ops[1], Ops[0]);
    return true;
  }
  case IX86_BUILTIN_STORELPS: {
    VectorType *v2f64 = VectorType::get(Type::getDoubleTy(Context), 2);
    const PointerType *f64Ptr = Type::getDoublePtrTy(Context);
    Ops[0] = Builder.CreateBitCast(Ops[0], f64Ptr);
    Value *Idx = ConstantInt::get(Type::getInt32Ty(Context), 0);
    Ops[1] = Builder.CreateBitCast(Ops[1], v2f64);
    Ops[1] = Builder.CreateExtractElement(Ops[1], Idx);
    Result = Builder.CreateStore(Ops[1], Ops[0]);
    return true;
  }
  case IX86_BUILTIN_MOVSHDUP:
    Result = BuildVectorShuffle(Ops[0], Ops[0], 1, 1, 3, 3);
    return true;
  case IX86_BUILTIN_MOVSLDUP:
    Result = BuildVectorShuffle(Ops[0], Ops[0], 0, 0, 2, 2);
    return true;
  case IX86_BUILTIN_VEC_EXT_V4HI:
  case IX86_BUILTIN_VEC_EXT_V2DF:
  case IX86_BUILTIN_VEC_EXT_V2DI:
  case IX86_BUILTIN_VEC_EXT_V4SI:
  case IX86_BUILTIN_VEC_EXT_V4SF:
  case IX86_BUILTIN_VEC_EXT_V8HI:
  case IX86_BUILTIN_VEC_EXT_V16QI:
    Result = Builder.CreateExtractElement(Ops[0], Ops[1]);
    return true;
  case IX86_BUILTIN_VEC_SET_V16QI:
    // Sometimes G++ promotes arguments to int.
    Ops[1] = Builder.CreateIntCast(Ops[1], Type::getInt8Ty(Context), false);
    Result = Builder.CreateInsertElement(Ops[0], Ops[1], Ops[2]);
    return true;
  case IX86_BUILTIN_VEC_SET_V4HI:
  case IX86_BUILTIN_VEC_SET_V8HI:
    // GCC sometimes doesn't produce the right element type.
    Ops[1] = Builder.CreateIntCast(Ops[1], Type::getInt16Ty(Context), false);
    Result = Builder.CreateInsertElement(Ops[0], Ops[1], Ops[2]);
    return true;
  case IX86_BUILTIN_VEC_SET_V4SI:
    Result = Builder.CreateInsertElement(Ops[0], Ops[1], Ops[2]);
    return true;
  case IX86_BUILTIN_VEC_SET_V2DI:
    Result = Builder.CreateInsertElement(Ops[0], Ops[1], Ops[2]);
    return true;
  case IX86_BUILTIN_CMPEQPS:
  case IX86_BUILTIN_CMPLTPS:
  case IX86_BUILTIN_CMPLEPS:
  case IX86_BUILTIN_CMPGTPS:
  case IX86_BUILTIN_CMPGEPS:
  case IX86_BUILTIN_CMPNEQPS:
  case IX86_BUILTIN_CMPNLTPS:
  case IX86_BUILTIN_CMPNLEPS:
  case IX86_BUILTIN_CMPNGTPS:
  case IX86_BUILTIN_CMPNGEPS:
  case IX86_BUILTIN_CMPORDPS:
  case IX86_BUILTIN_CMPUNORDPS: {
    Function *cmpps =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_cmp_ps);
    bool flip = false;
    unsigned PredCode;
    switch (FnCode) {
    default: assert(0 && "Unknown fncode!");
    case IX86_BUILTIN_CMPEQPS: PredCode = 0; break;
    case IX86_BUILTIN_CMPLTPS: PredCode = 1; break;
    case IX86_BUILTIN_CMPGTPS: PredCode = 1; flip = true; break;
    case IX86_BUILTIN_CMPLEPS: PredCode = 2; break;
    case IX86_BUILTIN_CMPGEPS: PredCode = 2; flip = true; break;
    case IX86_BUILTIN_CMPUNORDPS: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQPS: PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTPS: PredCode = 5; break;
    case IX86_BUILTIN_CMPNGTPS: PredCode = 5; flip = true; break;
    case IX86_BUILTIN_CMPNLEPS: PredCode = 6; break;
    case IX86_BUILTIN_CMPNGEPS: PredCode = 6; flip = true; break;
    case IX86_BUILTIN_CMPORDPS: PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::getInt8Ty(Context), PredCode);
    Value *Arg0 = Ops[0];
    Value *Arg1 = Ops[1];
    if (flip) std::swap(Arg0, Arg1);
    Value *CallOps[3] = { Arg0, Arg1, Pred };
    Result = Builder.CreateCall(cmpps, CallOps, CallOps+3);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_CMPEQSS:
  case IX86_BUILTIN_CMPLTSS:
  case IX86_BUILTIN_CMPLESS:
  case IX86_BUILTIN_CMPNEQSS:
  case IX86_BUILTIN_CMPNLTSS:
  case IX86_BUILTIN_CMPNLESS:
  case IX86_BUILTIN_CMPNGTSS:
  case IX86_BUILTIN_CMPNGESS:
  case IX86_BUILTIN_CMPORDSS:
  case IX86_BUILTIN_CMPUNORDSS: {
    Function *cmpss =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_cmp_ss);
    unsigned PredCode;
    switch (FnCode) {
    default: assert(0 && "Unknown fncode");
    case IX86_BUILTIN_CMPEQSS:    PredCode = 0; break;
    case IX86_BUILTIN_CMPLTSS:    PredCode = 1; break;
    case IX86_BUILTIN_CMPLESS:    PredCode = 2; break;
    case IX86_BUILTIN_CMPUNORDSS: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQSS:   PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTSS:   PredCode = 5; break;
    case IX86_BUILTIN_CMPNLESS:   PredCode = 6; break;
    case IX86_BUILTIN_CMPORDSS:   PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::getInt8Ty(Context), PredCode);
    Value *CallOps[3] = { Ops[0], Ops[1], Pred };
    Result = Builder.CreateCall(cmpss, CallOps, CallOps+3);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_CMPEQPD:
  case IX86_BUILTIN_CMPLTPD:
  case IX86_BUILTIN_CMPLEPD:
  case IX86_BUILTIN_CMPGTPD:
  case IX86_BUILTIN_CMPGEPD:
  case IX86_BUILTIN_CMPNEQPD:
  case IX86_BUILTIN_CMPNLTPD:
  case IX86_BUILTIN_CMPNLEPD:
  case IX86_BUILTIN_CMPNGTPD:
  case IX86_BUILTIN_CMPNGEPD:
  case IX86_BUILTIN_CMPORDPD:
  case IX86_BUILTIN_CMPUNORDPD: {
    Function *cmppd =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_cmp_pd);
    bool flip = false;
    unsigned PredCode;
    switch (FnCode) {
    default: assert(0 && "Unknown fncode!");
    case IX86_BUILTIN_CMPEQPD:    PredCode = 0; break;
    case IX86_BUILTIN_CMPLTPD:    PredCode = 1; break;
    case IX86_BUILTIN_CMPGTPD:    PredCode = 1; flip = true; break;
    case IX86_BUILTIN_CMPLEPD:    PredCode = 2; break;
    case IX86_BUILTIN_CMPGEPD:    PredCode = 2; flip = true; break;
    case IX86_BUILTIN_CMPUNORDPD: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQPD:   PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTPD:   PredCode = 5; break;
    case IX86_BUILTIN_CMPNGTPD:   PredCode = 5; flip = true; break;
    case IX86_BUILTIN_CMPNLEPD:   PredCode = 6; break;
    case IX86_BUILTIN_CMPNGEPD:   PredCode = 6; flip = true; break;
    case IX86_BUILTIN_CMPORDPD:   PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::getInt8Ty(Context), PredCode);
    Value *Arg0 = Ops[0];
    Value *Arg1 = Ops[1];
    if (flip) std::swap(Arg0, Arg1);

    Value *CallOps[3] = { Arg0, Arg1, Pred };
    Result = Builder.CreateCall(cmppd, CallOps, CallOps+3);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_CMPEQSD:
  case IX86_BUILTIN_CMPLTSD:
  case IX86_BUILTIN_CMPLESD:
  case IX86_BUILTIN_CMPNEQSD:
  case IX86_BUILTIN_CMPNLTSD:
  case IX86_BUILTIN_CMPNLESD:
  case IX86_BUILTIN_CMPORDSD:
  case IX86_BUILTIN_CMPUNORDSD: {
    Function *cmpsd =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse2_cmp_sd);
    unsigned PredCode;
    switch (FnCode) {
      default: assert(0 && "Unknown fncode");
    case IX86_BUILTIN_CMPEQSD:    PredCode = 0; break;
    case IX86_BUILTIN_CMPLTSD:    PredCode = 1; break;
    case IX86_BUILTIN_CMPLESD:    PredCode = 2; break;
    case IX86_BUILTIN_CMPUNORDSD: PredCode = 3; break;
    case IX86_BUILTIN_CMPNEQSD:   PredCode = 4; break;
    case IX86_BUILTIN_CMPNLTSD:   PredCode = 5; break;
    case IX86_BUILTIN_CMPNLESD:   PredCode = 6; break;
    case IX86_BUILTIN_CMPORDSD:   PredCode = 7; break;
    }
    Value *Pred = ConstantInt::get(Type::getInt8Ty(Context), PredCode);
    Value *CallOps[3] = { Ops[0], Ops[1], Pred };
    Result = Builder.CreateCall(cmpsd, CallOps, CallOps+3);
    Result = Builder.CreateBitCast(Result, ResultType);
    return true;
  }
  case IX86_BUILTIN_LDMXCSR: {
    Function *ldmxcsr =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_ldmxcsr);
    Value *Ptr = CreateTemporary(Type::getInt32Ty(Context));
    Builder.CreateStore(Ops[0], Ptr);
    Ptr = Builder.CreateBitCast(Ptr, Type::getInt8PtrTy(Context));
    Builder.CreateCall(ldmxcsr, Ptr);
    return true;
  }
  case IX86_BUILTIN_STMXCSR: {
    Function *stmxcsr =
      Intrinsic::getDeclaration(TheModule, Intrinsic::x86_sse_stmxcsr);
    Value *Ptr  = CreateTemporary(Type::getInt32Ty(Context));
    Value *BPtr = Builder.CreateBitCast(Ptr, Type::getInt8PtrTy(Context));
    Builder.CreateCall(stmxcsr, BPtr);
    
    Result = Builder.CreateLoad(Ptr);
    return true;
  }
  case IX86_BUILTIN_PALIGNR128: {
    if (ConstantInt *Elt = dyn_cast<ConstantInt>(Ops[2])) {
      
      // In the header we multiply by 8, correct that back now.
      unsigned shiftVal = (cast<ConstantInt>(Ops[2])->getZExtValue())/8;

      // If palignr is shifting the pair of input vectors less than 17 bytes,
      // emit a shuffle instruction.
      if (shiftVal <= 16) {
        const llvm::Type *IntTy = Type::getInt32Ty(Context);
        const llvm::Type *EltTy = Type::getInt8Ty(Context);
        const llvm::Type *VecTy = VectorType::get(EltTy, 16);
        
        Ops[1] = Builder.CreateBitCast(Ops[1], VecTy);
        Ops[0] = Builder.CreateBitCast(Ops[0], VecTy);

        llvm::SmallVector<Constant*, 16> Indices;
        for (unsigned i = 0; i != 16; ++i)
          Indices.push_back(ConstantInt::get(IntTy, shiftVal + i));

        Value* SV = ConstantVector::get(Indices);
        Result = Builder.CreateShuffleVector(Ops[1], Ops[0], SV, "palignr");
        return true;
      }

      // If palignr is shifting the pair of input vectors more than 16 but less
      // than 32 bytes, emit a logical right shift of the destination.
      if (shiftVal < 32) {
        const llvm::Type *EltTy = Type::getInt64Ty(Context);
        const llvm::Type *VecTy = VectorType::get(EltTy, 2);
        const llvm::Type *IntTy = Type::getInt32Ty(Context);

        Ops[0] = Builder.CreateBitCast(Ops[0], VecTy, "cast");
        Ops[1] = ConstantInt::get(IntTy, (shiftVal-16) * 8);

        // create i32 constant
        llvm::Function *F = Intrinsic::getDeclaration(TheModule,
                                                  Intrinsic::x86_sse2_psrl_dq);        
        Result = Builder.CreateCall(F, &Ops[0], &Ops[0] + 2, "palignr");
        return true;
      }

      // If palignr is shifting the pair of vectors more than 32 bytes, emit zero.
      Result = Constant::getNullValue(ResultType);
      return true;
    } else {
      error("%Hmask must be an immediate", &EXPR_LOCATION(exp));
      Result = Ops[0];
      return true;
    }
  }
  }

  return false;
}

/* These are defined in i386.c */
#define MAX_CLASSES 4
extern "C" enum machine_mode ix86_getNaturalModeForType(tree);
extern "C" int ix86_HowToPassArgument(enum machine_mode, tree, int, int*, int*);
extern "C" int ix86_ClassifyArgument(enum machine_mode, tree,
                               enum x86_64_reg_class classes[MAX_CLASSES], int);

/* Target hook for llvm-abi.h. It returns true if an aggregate of the
   specified type should be passed in memory. This is only called for
   x86-64. */
static bool llvm_x86_64_should_pass_aggregate_in_memory(tree TreeType,
                                                        enum machine_mode Mode){
  int IntRegs, SSERegs;
  /* If ix86_HowToPassArgument return 0, then it's passed byval in memory.*/
  int ret = ix86_HowToPassArgument(Mode, TreeType, 0, &IntRegs, &SSERegs);
  if (ret==0)
    return true;
  if (ret==1 && IntRegs==0 && SSERegs==0)   // zero-sized struct
    return true;
  return false;
}

/* Returns true if all elements of the type are integer types. */
static bool llvm_x86_is_all_integer_types(const Type *Ty) {
  for (Type::subtype_iterator I = Ty->subtype_begin(), E = Ty->subtype_end();
       I != E; ++I) {
    const Type *STy = I->get();
    if (!STy->isIntOrIntVectorTy() && !STy->isPointerTy())
      return false;
  }
  return true;
}

/* Target hook for llvm-abi.h. It returns true if an aggregate of the
   specified type should be passed in a number of registers of mixed types.
   It also returns a vector of types that correspond to the registers used
   for parameter passing. This is only called for x86-32. */
bool
llvm_x86_32_should_pass_aggregate_in_mixed_regs(tree TreeType, const Type *Ty,
                                                std::vector<const Type*> &Elts){
  // If this is a small fixed size type, investigate it.
  HOST_WIDE_INT SrcSize = int_size_in_bytes(TreeType);
  if (SrcSize <= 0 || SrcSize > 16)
    return false;

  // X86-32 passes aggregates on the stack.  If this is an extremely simple
  // aggregate whose elements would be passed the same if passed as scalars,
  // pass them that way in order to promote SROA on the caller and callee side.
  // Note that we can't support passing all structs this way.  For example,
  // {i16, i16} should be passed in on 32-bit unit, which is not how "i16, i16"
  // would be passed as stand-alone arguments.
  const StructType *STy = dyn_cast<StructType>(Ty);
  if (!STy || STy->isPacked()) return false;

  for (unsigned i = 0, e = STy->getNumElements(); i != e; ++i) {
    const Type *EltTy = STy->getElementType(i);
    // 32 and 64-bit integers are fine, as are float and double.  Long double
    // (which can be picked as the type for a union of 16 bytes) is not fine, 
    // as loads and stores of it get only 10 bytes.
    if (EltTy->isIntegerTy(32) || EltTy->isIntegerTy(64) ||
        EltTy->isFloatTy() || EltTy->isDoubleTy() || EltTy->isPointerTy()) {
      Elts.push_back(EltTy);
      continue;
    }
    
    // TODO: Vectors are also ok to pass if they don't require extra alignment.
    // TODO: We can also pass structs like {i8, i32}.
    
    Elts.clear();
    return false;
  }
  
  return true;
}  

/* It returns true if an aggregate of the specified type should be passed as a
   first class aggregate. */
bool llvm_x86_should_pass_aggregate_as_fca(tree type, const Type *Ty) {
  if (TREE_CODE(type) != COMPLEX_TYPE)
    return false;
  const StructType *STy = dyn_cast<StructType>(Ty);
  if (!STy || STy->isPacked()) return false;

  // FIXME: Currently codegen isn't lowering most _Complex types in a way that
  // makes it ABI compatible for x86-64. Same for _Complex char and _Complex
  // short in 32-bit.
  const Type *EltTy = STy->getElementType(0);
  return !((TARGET_64BIT && (EltTy->isIntegerTy() ||
                             EltTy->isFloatTy() ||
                             EltTy->isDoubleTy())) ||
           EltTy->isIntegerTy(16) || EltTy->isIntegerTy(8));
}

/* Target hook for llvm-abi.h. It returns true if an aggregate of the
   specified type should be passed in memory. */
bool llvm_x86_should_pass_aggregate_in_memory(tree TreeType, const Type *Ty) {
  if (llvm_x86_should_pass_aggregate_as_fca(TreeType, Ty))
    return false;

  enum machine_mode Mode = ix86_getNaturalModeForType(TreeType);
  HOST_WIDE_INT Bytes =
    (Mode == BLKmode) ? int_size_in_bytes(TreeType) : (int) GET_MODE_SIZE(Mode);

  // Zero sized array, struct, or class, not passed in memory.
  if (Bytes == 0)
    return false;

  if (!TARGET_64BIT) {
    std::vector<const Type*> Elts;
    return !llvm_x86_32_should_pass_aggregate_in_mixed_regs(TreeType, Ty, Elts);
  }
  return llvm_x86_64_should_pass_aggregate_in_memory(TreeType, Mode);
}

/* count_num_registers_uses - Return the number of GPRs and XMMs parameter
   register used so far.  Caller is responsible for initializing outputs. */
static void count_num_registers_uses(std::vector<const Type*> &ScalarElts,
                                     unsigned &NumGPRs, unsigned &NumXMMs) {
  for (unsigned i = 0, e = ScalarElts.size(); i != e; ++i) {
    const Type *Ty = ScalarElts[i];
    if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
      if (!TARGET_MACHO)
        continue;
      if (VTy->getNumElements() == 1)
        // v1i64 is passed in GPRs on Darwin.
        ++NumGPRs;
      else
        // All other vector scalar values are passed in XMM registers.
        ++NumXMMs;
    } else if (Ty->isIntegerTy() || Ty->isPointerTy()) {
      ++NumGPRs;
    } else if (Ty->isVoidTy()) {
      // Padding bytes that are not passed anywhere
      ;
    } else {
      // Floating point scalar argument.
      assert(Ty->isFloatingPointTy() && Ty->isPrimitiveType() &&
             "Expecting a floating point primitive type!");
      if (Ty->getTypeID() == Type::FloatTyID
          || Ty->getTypeID() == Type::DoubleTyID)
        ++NumXMMs;
    }
  }
}

/* Target hook for llvm-abi.h. This is called when an aggregate is being passed
   in registers. If there are only enough available parameter registers to pass
   part of the aggregate, return true. That means the aggregate should instead
   be passed in memory. */
bool
llvm_x86_64_aggregate_partially_passed_in_regs(std::vector<const Type*> &Elts,
                                         std::vector<const Type*> &ScalarElts) {
  // Counting number of GPRs and XMMs used so far. According to AMD64 ABI
  // document: "If there are no registers available for any eightbyte of an
  // argument, the whole  argument is passed on the stack." X86-64 uses 6
  // integer 
  // For example, if two GPRs are required but only one is available, then
  // both parts will be in memory.
  // FIXME: This is a temporary solution. To be removed when llvm has first
  // class aggregate values.
  unsigned NumGPRs = 0;
  unsigned NumXMMs = 0;
  count_num_registers_uses(ScalarElts, NumGPRs, NumXMMs);

  unsigned NumGPRsNeeded = 0;
  unsigned NumXMMsNeeded = 0;
  count_num_registers_uses(Elts, NumGPRsNeeded, NumXMMsNeeded);

  bool GPRsSatisfied = true;
  if (NumGPRsNeeded) {
    if (NumGPRs < 6) {
      if ((NumGPRs + NumGPRsNeeded) > 6)
        // Only partially satisfied.
        return true;
    } else
      GPRsSatisfied = false;
  }

  bool XMMsSatisfied = true;
  if (NumXMMsNeeded) {
    if (NumXMMs < 8) {
      if ((NumXMMs + NumXMMsNeeded) > 8)
        // Only partially satisfied.
        return true;
    } else
      XMMsSatisfied = false;
  }

  return !GPRsSatisfied || !XMMsSatisfied;
}

/* Target hook for llvm-abi.h. It returns true if an aggregate of the
   specified type should be passed in a number of registers of mixed types.
   It also returns a vector of types that correspond to the registers used
   for parameter passing. This is only called for x86-64. */
bool
llvm_x86_64_should_pass_aggregate_in_mixed_regs(tree TreeType, const Type *Ty,
                                                std::vector<const Type*> &Elts){
  if (llvm_x86_should_pass_aggregate_as_fca(TreeType, Ty))
    return false;

  enum x86_64_reg_class Class[MAX_CLASSES];
  enum machine_mode Mode = ix86_getNaturalModeForType(TreeType);
  bool totallyEmpty = true;
  HOST_WIDE_INT Bytes =
    (Mode == BLKmode) ? int_size_in_bytes(TreeType) : (int) GET_MODE_SIZE(Mode);
  int NumClasses = ix86_ClassifyArgument(Mode, TreeType, Class, 0);
  if (!NumClasses)
    return false;

  for (int i = 0; i < NumClasses; ++i) {
    switch (Class[i]) {
    case X86_64_INTEGER_CLASS:
    case X86_64_INTEGERSI_CLASS:
      Elts.push_back(Type::getInt64Ty(Context));
      totallyEmpty = false;
      Bytes -= 8;
      break;
    case X86_64_POINTER_CLASS:
      Elts.push_back(Type::getInt8PtrTy(Context));
      totallyEmpty = false;
      Bytes -= 8;
      break;
    case X86_64_SSE_CLASS:
      totallyEmpty = false;
      // If it's a SSE class argument, then one of the followings are possible:
      // 1. 1 x SSE, size is 8: 1 x Double.
      // 2. 1 x SSE, size is 4: 1 x Float.
      // 3. 1 x SSE + 1 x SSEUP, size is 16: 1 x <4 x i32>, <4 x f32>,
      //                                         <2 x i64>, or <2 x f64>.
      // 4. 1 x SSE + 1 x SSESF, size is 12: 1 x Double, 1 x Float.
      // 5. 2 x SSE, size is 16: 2 x Double.
      if ((NumClasses-i) == 1) {
        if (Bytes == 8) {
          Elts.push_back(Type::getDoubleTy(Context));
          Bytes -= 8;
        } else if (Bytes == 4) {
          Elts.push_back (Type::getFloatTy(Context));
          Bytes -= 4;
        } else
          assert(0 && "Not yet handled!");
      } else if ((NumClasses-i) == 2) {
        if (Class[i+1] == X86_64_SSEUP_CLASS) {
          const Type *Ty = ConvertType(TreeType);
          if (const StructType *STy = dyn_cast<StructType>(Ty))
            // Look pass the struct wrapper.
            if (STy->getNumElements() == 1)
              Ty = STy->getElementType(0);
          if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
            if (VTy->getNumElements() == 2) {
              if (VTy->getElementType()->isIntegerTy()) {
                Elts.push_back(VectorType::get(Type::getInt64Ty(Context), 2));
              } else {
                Elts.push_back(VectorType::get(Type::getDoubleTy(Context), 2));
              }
              Bytes -= 8;
            } else {
              assert(VTy->getNumElements() == 4);
              if (VTy->getElementType()->isIntegerTy()) {
                Elts.push_back(VectorType::get(Type::getInt32Ty(Context), 4));
              } else {
                Elts.push_back(VectorType::get(Type::getFloatTy(Context), 4));
              }
              Bytes -= 4;
            }
          } else if (llvm_x86_is_all_integer_types(Ty)) {
            Elts.push_back(VectorType::get(Type::getInt32Ty(Context), 4));
            Bytes -= 4;
          } else {
            Elts.push_back(VectorType::get(Type::getFloatTy(Context), 4));
            Bytes -= 4;
          }
        } else if (Class[i+1] == X86_64_SSESF_CLASS) {
          assert(Bytes == 12 && "Not yet handled!");
          Elts.push_back(Type::getDoubleTy(Context));
          Elts.push_back(Type::getFloatTy(Context));
          Bytes -= 12;
        } else if (Class[i+1] == X86_64_SSE_CLASS) {
          Elts.push_back(Type::getDoubleTy(Context));
          Elts.push_back(Type::getDoubleTy(Context));
          Bytes -= 16;
        } else if (Class[i+1] == X86_64_SSEDF_CLASS && Bytes == 16) {
          Elts.push_back(VectorType::get(Type::getFloatTy(Context), 2));
          Elts.push_back(Type::getDoubleTy(Context));
        } else if (Class[i+1] == X86_64_INTEGER_CLASS) {
          Elts.push_back(VectorType::get(Type::getFloatTy(Context), 2));
          Elts.push_back(Type::getInt64Ty(Context));
        } else if (Class[i+1] == X86_64_POINTER_CLASS) {
          Elts.push_back(VectorType::get(Type::getFloatTy(Context), 2));
          Elts.push_back(Type::getInt8PtrTy(Context));
        } else if (Class[i+1] == X86_64_NO_CLASS) {
          // padding bytes, don't pass
          Elts.push_back(Type::getDoubleTy(Context));
          Elts.push_back(Type::getVoidTy(Context));
          Bytes -= 16;
        } else
          assert(0 && "Not yet handled!");
        ++i; // Already handled the next one.
      } else
        assert(0 && "Not yet handled!");
      break;
    case X86_64_SSESF_CLASS:
      totallyEmpty = false;
      Elts.push_back(Type::getFloatTy(Context));
      Bytes -= 4;
      break;
    case X86_64_SSEDF_CLASS:
      totallyEmpty = false;
      Elts.push_back(Type::getDoubleTy(Context));
      Bytes -= 8;
      break;
    case X86_64_X87_CLASS:
    case X86_64_X87UP_CLASS:
    case X86_64_COMPLEX_X87_CLASS:
      return false;
    case X86_64_NO_CLASS:
      // Padding bytes that are not passed (unless the entire object consists
      // of padding)
      Elts.push_back(Type::getVoidTy(Context));
      Bytes -= 8;
      break;
    default: assert(0 && "Unexpected register class!");
    }
  }

  return !totallyEmpty;
}

/* On Darwin x86-32, vectors which are not MMX nor SSE should be passed as 
   integers.  On Darwin x86-64, such vectors bigger than 128 bits should be
   passed in memory (byval). */
bool llvm_x86_should_pass_vector_in_integer_regs(tree type) {
  if (!TARGET_MACHO)
    return false;
  if (TREE_CODE(type) == VECTOR_TYPE &&
      TYPE_SIZE(type) &&
      TREE_CODE(TYPE_SIZE(type))==INTEGER_CST) {
    if (TREE_INT_CST_LOW(TYPE_SIZE(type))==64 && TARGET_MMX)
      return false;
    if (TREE_INT_CST_LOW(TYPE_SIZE(type))==128 && TARGET_SSE)
      return false;
    if (TARGET_64BIT && TREE_INT_CST_LOW(TYPE_SIZE(type)) > 128)
      return false;
  }
  return true;
}

/* On Darwin x86-64, vectors which are bigger than 128 bits should be passed
   byval (in memory).  */
bool llvm_x86_should_pass_vector_using_byval_attr(tree type) {
  if (!TARGET_MACHO)
    return false;
  if (!TARGET_64BIT)
    return false;
  if (TREE_CODE(type) == VECTOR_TYPE &&
      TYPE_SIZE(type) &&
      TREE_CODE(TYPE_SIZE(type))==INTEGER_CST) {
    if (TREE_INT_CST_LOW(TYPE_SIZE(type))<=128)
      return false;
  }
  return true;
}

/* The MMX vector v1i64 is returned in EAX and EDX on Darwin.  Communicate
    this by returning i64 here.  Likewise, (generic) vectors such as v2i16
    are returned in EAX.  
   On Darwin x86-64, v1i64 is returned in RAX and other MMX vectors are 
    returned in XMM0.  Judging from comments, this would not be right for
    Win64.  Don't know about Linux.  */
tree llvm_x86_should_return_vector_as_scalar(tree type, bool isBuiltin) {
  if (TARGET_MACHO &&
      !isBuiltin &&
      TREE_CODE(type) == VECTOR_TYPE &&
      TYPE_SIZE(type) &&
      TREE_CODE(TYPE_SIZE(type))==INTEGER_CST) {
    if (TREE_INT_CST_LOW(TYPE_SIZE(type))==64 &&
        TYPE_VECTOR_SUBPARTS(type)==1)
      return uint64_type_node;
    if (TARGET_64BIT && TREE_INT_CST_LOW(TYPE_SIZE(type))==64)
      return double_type_node;
    if (TREE_INT_CST_LOW(TYPE_SIZE(type))==32)
      return uint32_type_node;
  }
  return 0;
}

/* MMX vectors are returned in XMM0 on x86-64 Darwin.  The easiest way to
   communicate this is pretend they're doubles.
   Judging from comments, this would not be right for Win64.  Don't know
   about Linux.  */
tree llvm_x86_should_return_selt_struct_as_scalar(tree type) {
  tree retType = isSingleElementStructOrArray(type, true, false);
  if (!retType || !TARGET_64BIT || !TARGET_MACHO)
    return retType;
  if (TREE_CODE(retType) == VECTOR_TYPE &&
      TYPE_SIZE(retType) &&
      TREE_CODE(TYPE_SIZE(retType))==INTEGER_CST &&
      TREE_INT_CST_LOW(TYPE_SIZE(retType))==64)
    return double_type_node;
  return retType;
}

/* MMX vectors v2i32, v4i16, v8i8, v2f32 are returned using sret on Darwin
   32-bit.  Vectors bigger than 128 are returned using sret.  */
bool llvm_x86_should_return_vector_as_shadow(tree type, bool isBuiltin) {
  if (TARGET_MACHO &&
    !isBuiltin &&
    !TARGET_64BIT &&
    TREE_CODE(type) == VECTOR_TYPE &&
    TYPE_SIZE(type) &&
    TREE_CODE(TYPE_SIZE(type))==INTEGER_CST) {
    if (TREE_INT_CST_LOW(TYPE_SIZE(type))==64 &&
       TYPE_VECTOR_SUBPARTS(type)>1)
      return true;
  }
  if (TREE_INT_CST_LOW(TYPE_SIZE(type))>128)
    return true;
  return false;
}

// llvm_x86_should_not_return_complex_in_memory -  Return true if TYPE 
// should be returned using multiple value return instruction.
bool llvm_x86_should_not_return_complex_in_memory(tree type) {

  if (!TARGET_64BIT)
    return false;

  if (TREE_CODE(type) == COMPLEX_TYPE &&
      TREE_INT_CST_LOW(TYPE_SIZE_UNIT(type)) == 32)
    return true;

  return false;
}

// llvm_suitable_multiple_ret_value_type - Return TRUE if return value 
// of type TY should be returned using multiple value return instruction.
static bool llvm_suitable_multiple_ret_value_type(const Type *Ty,
                                                  tree TreeType) {

  if (!TARGET_64BIT)
    return false;

  const StructType *STy = dyn_cast<StructType>(Ty);
  if (!STy)
    return false;

  if (llvm_x86_should_not_return_complex_in_memory(TreeType))
    return true;

  // Let gcc specific routine answer the question.
  enum x86_64_reg_class Class[MAX_CLASSES];
  enum machine_mode Mode = ix86_getNaturalModeForType(TreeType);
  int NumClasses = ix86_ClassifyArgument(Mode, TreeType, Class, 0);
  if (NumClasses == 0)
    return false;

  if (NumClasses == 1 && 
      (Class[0] == X86_64_INTEGERSI_CLASS || Class[0] == X86_64_INTEGER_CLASS ||
       Class[0] == X86_64_POINTER_CLASS))
    // This will fit in one i64 register.
    return false;

  if (NumClasses == 2 &&
      (Class[0] == X86_64_NO_CLASS || Class[1] == X86_64_NO_CLASS))
    // One word is padding which is not passed at all; treat this as returning
    // the scalar type of the other word.
    return false;

  // Otherwise, use of multiple value return is OK.
  return true;
}

// llvm_x86_scalar_type_for_struct_return - Return LLVM type if TYPE
// can be returned as a scalar, otherwise return NULL.
const Type *llvm_x86_scalar_type_for_struct_return(tree type, unsigned *Offset) {
  *Offset = 0;
  const Type *Ty = ConvertType(type);
  unsigned Size = getTargetData().getTypeAllocSize(Ty);
  if (Size == 0)
    return Type::getVoidTy(Context);
  else if (Size == 1)
    return Type::getInt8Ty(Context);
  else if (Size == 2)
    return Type::getInt16Ty(Context);
  else if (Size <= 4)
    return Type::getInt32Ty(Context);

  // Check if Ty should be returned using multiple value return instruction.
  if (llvm_suitable_multiple_ret_value_type(Ty, type))
    return NULL;

  if (TARGET_64BIT) {
    // This logic relies on llvm_suitable_multiple_ret_value_type to have
    // removed anything not expected here.
    enum x86_64_reg_class Class[MAX_CLASSES];
    enum machine_mode Mode = ix86_getNaturalModeForType(type);
    int NumClasses = ix86_ClassifyArgument(Mode, type, Class, 0);
    if (NumClasses == 0)
      return Type::getInt64Ty(Context);

    if (NumClasses == 1) {
      if (Class[0] == X86_64_INTEGERSI_CLASS ||
          Class[0] == X86_64_INTEGER_CLASS ||
          Class[0] == X86_64_POINTER_CLASS) {
        // one int register
        HOST_WIDE_INT Bytes =
          (Mode == BLKmode) ? int_size_in_bytes(type) : 
                              (int) GET_MODE_SIZE(Mode);
        if (Bytes==8 && Class[0] == X86_64_POINTER_CLASS)
          return Type::getInt8PtrTy(Context);
        if (Bytes>4)
          return Type::getInt64Ty(Context);
        else if (Bytes>2)
          return Type::getInt32Ty(Context);
        else if (Bytes>1)
          return Type::getInt16Ty(Context);
        else
          return Type::getInt8Ty(Context);
      }
      assert(0 && "Unexpected type!"); 
    }
    if (NumClasses == 2) {
      if (Class[1] == X86_64_NO_CLASS) {
        if (Class[0] == X86_64_INTEGER_CLASS || 
            Class[0] == X86_64_NO_CLASS ||
            Class[0] == X86_64_INTEGERSI_CLASS)
          return Type::getInt64Ty(Context);
        else if (Class[0] == X86_64_POINTER_CLASS)
          return Type::getInt8PtrTy(Context);
        else if (Class[0] == X86_64_SSE_CLASS || Class[0] == X86_64_SSEDF_CLASS)
          return Type::getDoubleTy(Context);
        else if (Class[0] == X86_64_SSESF_CLASS)
          return Type::getFloatTy(Context);
        assert(0 && "Unexpected type!");
      }
      if (Class[0] == X86_64_NO_CLASS) {
        *Offset = 8;
        if (Class[1] == X86_64_INTEGERSI_CLASS ||
            Class[1] == X86_64_INTEGER_CLASS)
          return Type::getInt64Ty(Context);
        else if (Class[1] == X86_64_POINTER_CLASS)
          return Type::getInt8PtrTy(Context);
        else if (Class[1] == X86_64_SSE_CLASS || Class[1] == X86_64_SSEDF_CLASS)
          return Type::getDoubleTy(Context);
        else if (Class[1] == X86_64_SSESF_CLASS)
          return Type::getFloatTy(Context);
        assert(0 && "Unexpected type!"); 
      }
      assert(0 && "Unexpected type!");
    }
    assert(0 && "Unexpected type!");
  } else {
    if (Size <= 8)
      return Type::getInt64Ty(Context);
    else if (Size <= 16)
      return IntegerType::get(Context, 128);
    else if (Size <= 32)
      return IntegerType::get(Context, 256);
  }
  return NULL;
}

/// llvm_x86_64_get_multiple_return_reg_classes - Find register classes used
/// to return Ty. It is expected that Ty requires multiple return values.
/// This routine uses GCC implementation to find required register classes.
/// The original implementation of this routine is based on 
/// llvm_x86_64_should_pass_aggregate_in_mixed_regs code.
void
llvm_x86_64_get_multiple_return_reg_classes(tree TreeType, const Type *Ty,
                                            std::vector<const Type*> &Elts){
  enum x86_64_reg_class Class[MAX_CLASSES];
  enum machine_mode Mode = ix86_getNaturalModeForType(TreeType);
  HOST_WIDE_INT Bytes =
    (Mode == BLKmode) ? int_size_in_bytes(TreeType) : (int) GET_MODE_SIZE(Mode);
  int NumClasses = ix86_ClassifyArgument(Mode, TreeType, Class, 0);
  if (!NumClasses)
     assert(0 && "This type does not need multiple return registers!");

  if (NumClasses == 1 && Class[0] == X86_64_INTEGERSI_CLASS)
    // This will fit in one i32 register.
     assert(0 && "This type does not need multiple return registers!");

  if (NumClasses == 1 && Class[0] == X86_64_INTEGER_CLASS)
     assert(0 && "This type does not need multiple return registers!");

  if (NumClasses == 1 && Class[0] == X86_64_POINTER_CLASS)
     assert(0 && "This type does not need multiple return registers!");

  // classify_argument uses a single X86_64_NO_CLASS as a special case for
  // empty structs. Recognize it and don't add any return values in that
  // case.
  if (NumClasses == 1 && Class[0] == X86_64_NO_CLASS)
     return;

  for (int i = 0; i < NumClasses; ++i) {
    switch (Class[i]) {
    case X86_64_INTEGER_CLASS:
    case X86_64_INTEGERSI_CLASS:
      Elts.push_back(Type::getInt64Ty(Context));
      Bytes -= 8;
      break;
    case X86_64_POINTER_CLASS:
      Elts.push_back(Type::getInt8PtrTy(Context));
      Bytes -= 8;
      break;
    case X86_64_SSE_CLASS:
      // If it's a SSE class argument, then one of the followings are possible:
      // 1. 1 x SSE, size is 8: 1 x Double.
      // 2. 1 x SSE, size is 4: 1 x Float.
      // 3. 1 x SSE + 1 x SSEUP, size is 16: 1 x <4 x i32>, <4 x f32>,
      //                                         <2 x i64>, or <2 x f64>.
      // 4. 1 x SSE + 1 x SSESF, size is 12: 1 x Double, 1 x Float.
      // 5. 2 x SSE, size is 16: 2 x Double.
      // 6. 1 x SSE, 1 x NO:  Second is padding, pass as double.
      if ((NumClasses-i) == 1) {
        if (Bytes == 8) {
          Elts.push_back(Type::getDoubleTy(Context));
          Bytes -= 8;
        } else if (Bytes == 4) {
          Elts.push_back(Type::getFloatTy(Context));
          Bytes -= 4;
        } else
          assert(0 && "Not yet handled!");
      } else if ((NumClasses-i) == 2) {
        if (Class[i+1] == X86_64_SSEUP_CLASS) {
          const Type *Ty = ConvertType(TreeType);
          if (const StructType *STy = dyn_cast<StructType>(Ty))
            // Look pass the struct wrapper.
            if (STy->getNumElements() == 1)
              Ty = STy->getElementType(0);
          if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
            if (VTy->getNumElements() == 2) {
              if (VTy->getElementType()->isIntegerTy())
                Elts.push_back(VectorType::get(Type::getInt64Ty(Context), 2));
              else
                Elts.push_back(VectorType::get(Type::getDoubleTy(Context), 2));
              Bytes -= 8;
            } else {
              assert(VTy->getNumElements() == 4);
              if (VTy->getElementType()->isIntegerTy())
                Elts.push_back(VectorType::get(Type::getInt32Ty(Context), 4));
              else
                Elts.push_back(VectorType::get(Type::getFloatTy(Context), 4));
              Bytes -= 4;
            }
          } else if (llvm_x86_is_all_integer_types(Ty)) {
            Elts.push_back(VectorType::get(Type::getInt32Ty(Context), 4));
            Bytes -= 4;
          } else {
            Elts.push_back(VectorType::get(Type::getFloatTy(Context), 4));
            Bytes -= 4;
          }
        } else if (Class[i+1] == X86_64_SSESF_CLASS) {
          assert(Bytes == 12 && "Not yet handled!");
          Elts.push_back(Type::getDoubleTy(Context));
          Elts.push_back(Type::getFloatTy(Context));
          Bytes -= 12;
        } else if (Class[i+1] == X86_64_SSE_CLASS) {
          Elts.push_back(Type::getDoubleTy(Context));
          Elts.push_back(Type::getDoubleTy(Context));
          Bytes -= 16;
        } else if (Class[i+1] == X86_64_SSEDF_CLASS && Bytes == 16) {
          Elts.push_back(VectorType::get(Type::getFloatTy(Context), 2));
          Elts.push_back(Type::getDoubleTy(Context));
        } else if (Class[i+1] == X86_64_INTEGER_CLASS) {
          Elts.push_back(VectorType::get(Type::getFloatTy(Context), 2));
          Elts.push_back(Type::getInt64Ty(Context));
        } else if (Class[i+1] == X86_64_POINTER_CLASS) {
          Elts.push_back(VectorType::get(Type::getFloatTy(Context), 2));
          Elts.push_back(Type::getInt8PtrTy(Context));
        } else if (Class[i+1] == X86_64_NO_CLASS) {
          Elts.push_back(Type::getDoubleTy(Context));
          Bytes -= 16;
        } else {
          assert(0 && "Not yet handled!");
        }
        ++i; // Already handled the next one.
      } else
        assert(0 && "Not yet handled!");
      break;
    case X86_64_SSESF_CLASS:
      Elts.push_back(Type::getFloatTy(Context));
      Bytes -= 4;
      break;
    case X86_64_SSEDF_CLASS:
      Elts.push_back(Type::getDoubleTy(Context));
      Bytes -= 8;
      break;
    case X86_64_X87_CLASS:
    case X86_64_X87UP_CLASS:
    case X86_64_COMPLEX_X87_CLASS:
      Elts.push_back(Type::getX86_FP80Ty(Context));
      break;
    case X86_64_NO_CLASS:
      // padding bytes.
      Elts.push_back(Type::getInt64Ty(Context));
      break;
    default: assert(0 && "Unexpected register class!");
    }
  }
}

// Return LLVM Type if TYPE can be returned as an aggregate, 
// otherwise return NULL.
const Type *llvm_x86_aggr_type_for_struct_return(tree type) {
  const Type *Ty = ConvertType(type);
  if (!llvm_suitable_multiple_ret_value_type(Ty, type))
    return NULL;

  const StructType *STy = cast<StructType>(Ty);
  std::vector<const Type *> ElementTypes;

  // Special handling for _Complex.
  if (llvm_x86_should_not_return_complex_in_memory(type)) {
    ElementTypes.push_back(Type::getX86_FP80Ty(Context));
    ElementTypes.push_back(Type::getX86_FP80Ty(Context));
    return StructType::get(Context, ElementTypes, STy->isPacked());
  } 

  std::vector<const Type*> GCCElts;
  llvm_x86_64_get_multiple_return_reg_classes(type, Ty, GCCElts);
  return StructType::get(Context, GCCElts, false);
}

// llvm_x86_extract_mrv_array_element - Helper function that help extract 
// an array element from multiple return value.
//
// Here, SRC is returning multiple values. DEST's DESTFIELNO field is an array.
// Extract SRCFIELDNO's ELEMENO value and store it in DEST's FIELDNO field's 
// ELEMENTNO.
//
static void llvm_x86_extract_mrv_array_element(Value *Src, Value *Dest,
                                               unsigned SrcFieldNo, 
                                               unsigned SrcElemNo,
                                               unsigned DestFieldNo, 
                                               unsigned DestElemNo,
                                               LLVMBuilder &Builder,
                                               bool isVolatile) {
  Value *EVI = Builder.CreateExtractValue(Src, SrcFieldNo, "mrv_gr");
  const StructType *STy = cast<StructType>(Src->getType());
  llvm::Value *Idxs[3];
  Idxs[0] = ConstantInt::get(llvm::Type::getInt32Ty(Context), 0);
  Idxs[1] = ConstantInt::get(llvm::Type::getInt32Ty(Context), DestFieldNo);
  Idxs[2] = ConstantInt::get(llvm::Type::getInt32Ty(Context), DestElemNo);
  Value *GEP = Builder.CreateGEP(Dest, Idxs, Idxs+3, "mrv_gep");
  if (STy->getElementType(SrcFieldNo)->isVectorTy()) {
    Value *ElemIndex = ConstantInt::get(Type::getInt32Ty(Context), SrcElemNo);
    Value *EVIElem = Builder.CreateExtractElement(EVI, ElemIndex, "mrv");
    Builder.CreateStore(EVIElem, GEP, isVolatile);
  } else {
    Builder.CreateStore(EVI, GEP, isVolatile);
  }
}

// llvm_x86_extract_multiple_return_value - Extract multiple values returned
// by SRC and store them in DEST. It is expected thaty SRC and
// DEST types are StructType, but they may not match.
void llvm_x86_extract_multiple_return_value(Value *Src, Value *Dest,
                                            bool isVolatile,
                                            LLVMBuilder &Builder) {
  
  const StructType *STy = cast<StructType>(Src->getType());
  unsigned NumElements = STy->getNumElements();

  const PointerType *PTy = cast<PointerType>(Dest->getType());
  const StructType *DestTy = cast<StructType>(PTy->getElementType());

  unsigned SNO = 0;
  unsigned DNO = 0;

  if (DestTy->getNumElements() == 3
      && DestTy->getElementType(0)->getTypeID() == Type::FloatTyID
      && DestTy->getElementType(1)->getTypeID() == Type::FloatTyID
      && DestTy->getElementType(2)->getTypeID() == Type::FloatTyID) {
    // DestTy is { float, float, float }
    // STy is { <4 x float>, float > }

    Value *EVI = Builder.CreateExtractValue(Src, 0, "mrv_gr");

    Value *E0Index = ConstantInt::get(Type::getInt32Ty(Context), 0);
    Value *EVI0 = Builder.CreateExtractElement(EVI, E0Index, "mrv.v");
    Value *GEP0 = Builder.CreateStructGEP(Dest, 0, "mrv_gep");
    Builder.CreateStore(EVI0, GEP0, isVolatile);

    Value *E1Index = ConstantInt::get(Type::getInt32Ty(Context), 1);
    Value *EVI1 = Builder.CreateExtractElement(EVI, E1Index, "mrv.v");
    Value *GEP1 = Builder.CreateStructGEP(Dest, 1, "mrv_gep");
    Builder.CreateStore(EVI1, GEP1, isVolatile);

    Value *GEP2 = Builder.CreateStructGEP(Dest, 2, "mrv_gep");
    Value *EVI2 = Builder.CreateExtractValue(Src, 1, "mrv_gr");
    Builder.CreateStore(EVI2, GEP2, isVolatile);
    return;
  }

  while (SNO < NumElements) {

    const Type *DestElemType = DestTy->getElementType(DNO);

    // Directly access first class values using getresult.
    if (DestElemType->isSingleValueType()) {
      Value *GEP = Builder.CreateStructGEP(Dest, DNO, "mrv_gep");
      Value *EVI = Builder.CreateExtractValue(Src, SNO, "mrv_gr");
      Builder.CreateStore(EVI, GEP, isVolatile);
      ++DNO; ++SNO;
      continue;
    } 

    // Special treatement for _Complex.
    if (DestElemType->isStructTy()) {
      llvm::Value *Idxs[3];
      Idxs[0] = ConstantInt::get(llvm::Type::getInt32Ty(Context), 0);
      Idxs[1] = ConstantInt::get(llvm::Type::getInt32Ty(Context), DNO);

      Idxs[2] = ConstantInt::get(llvm::Type::getInt32Ty(Context), 0);
      Value *GEP = Builder.CreateGEP(Dest, Idxs, Idxs+3, "mrv_gep");
      Value *EVI = Builder.CreateExtractValue(Src, 0, "mrv_gr");
      Builder.CreateStore(EVI, GEP, isVolatile);
      ++SNO;

      Idxs[2] = ConstantInt::get(llvm::Type::getInt32Ty(Context), 1);
      GEP = Builder.CreateGEP(Dest, Idxs, Idxs+3, "mrv_gep");
      EVI = Builder.CreateExtractValue(Src, 1, "mrv_gr");
      Builder.CreateStore(EVI, GEP, isVolatile);
      ++DNO; ++SNO;
      continue;
    }
    
    // Access array elements individually. Note, Src and Dest type may
    // not match. For example { <2 x float>, float } and { float[3]; }
    const ArrayType *ATy = cast<ArrayType>(DestElemType);
    unsigned ArraySize = ATy->getNumElements();
    unsigned DElemNo = 0; // DestTy's DNO field's element number
    while (DElemNo < ArraySize) {
      unsigned i = 0;
      unsigned Size = 1;
      
      if (const VectorType *SElemTy = 
          dyn_cast<VectorType>(STy->getElementType(SNO))) {
        Size = SElemTy->getNumElements();
        if (SElemTy->getElementType()->getTypeID() == Type::FloatTyID
            && Size == 4)
          // Ignore last two <4 x float> elements.
          Size = 2;
      }
      while (i < Size) {
        llvm_x86_extract_mrv_array_element(Src, Dest, SNO, i++, 
                                           DNO, DElemNo++, 
                                           Builder, isVolatile);
      }
      // Consumed this src field. Try next one.
      ++SNO;
    }
    // Finished building current dest field. 
    ++DNO;
  }
}

/// llvm_x86_should_pass_aggregate_in_integer_regs - x86-32 is same as the
/// default.  x86-64 detects the case where a type is 16 bytes long but
/// only 8 of them are passed, the rest being padding (*size is set to 8
/// to identify this case).  It also pads out the size to that of a full
/// register.  This means we'll be loading bytes off the end of the object
/// in some cases.  That's what gcc does, so it must be OK, right?  Right?
bool llvm_x86_should_pass_aggregate_in_integer_regs(tree type, unsigned *size,
                                                    bool *DontCheckAlignment) {
  *size = 0;
  if (TARGET_64BIT) {
    enum x86_64_reg_class Class[MAX_CLASSES];
    enum machine_mode Mode = ix86_getNaturalModeForType(type);
    int NumClasses = ix86_ClassifyArgument(Mode, type, Class, 0);
    *DontCheckAlignment= true;
    if (NumClasses == 1 && (Class[0] == X86_64_INTEGER_CLASS ||
                            Class[0] == X86_64_INTEGERSI_CLASS ||
                            Class[0] == X86_64_POINTER_CLASS)) {
      // one int register
      HOST_WIDE_INT Bytes =
        (Mode == BLKmode) ? int_size_in_bytes(type) : (int) GET_MODE_SIZE(Mode);
      if (Bytes>4)
        *size = 8;
      else if (Bytes>2)
        *size = 4;
      else
        *size = Bytes;
      return true;
    }
    if (NumClasses == 2 && (Class[0] == X86_64_INTEGERSI_CLASS ||
                            Class[0] == X86_64_INTEGER_CLASS ||
                            Class[0] == X86_64_POINTER_CLASS)) {
      if (Class[1] == X86_64_INTEGER_CLASS ||
          Class[1] == X86_64_POINTER_CLASS) {
        // 16 byte object, 2 int registers
        *size = 16;
        return true;
      }
      // IntegerSI can occur only as element 0.
      if (Class[1] == X86_64_NO_CLASS) {
        // 16 byte object, only 1st register has information
        *size = 8;
        return true;
      }
    }
    return false;    
  }
  else 
    return !isSingleElementStructOrArray(type, false, true);
}
#endif /* ENABLE_LLVM */
/* LLVM LOCAL end (ENTIRE FILE!)  */
