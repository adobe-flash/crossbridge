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
#include "llvm-avm2-target.h"

extern "C" {
#include "toplev.h"
}

static LLVMContext &Context = getGlobalContext();


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

  case AVM2_BUILTIN_MULPS:
    Result = Builder.CreateFMul(Ops[0], Ops[1]);
    return true;
  }

  return false;
}

#endif /* ENABLE_LLVM */
/* LLVM LOCAL end (ENTIRE FILE!)  */
