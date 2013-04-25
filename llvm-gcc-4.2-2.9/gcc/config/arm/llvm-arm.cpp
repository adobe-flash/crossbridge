/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* High-level LLVM backend interface
Copyright (C) 2008, 2009 Apple Inc.

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
// This is a C++ source file that implements specific llvm ARM ABI.
//===----------------------------------------------------------------------===//

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm-arm-target.h"

extern "C" {
#include "insn-codes.h"
#include "toplev.h"
#include "rtl.h"
#include "insn-config.h"
#include "recog.h"

static LLVMContext &Context = getGlobalContext();

enum neon_itype { neon_itype_dummy };
extern enum insn_code locate_neon_builtin_icode
  (int fcode, neon_itype *itype, enum neon_builtins *neon_code);
}

static ConstantInt *getInt32Const(int c) {
  return ConstantInt::get(Type::getInt32Ty(Context), c);
}

/// UnexpectedError - Report errors about unexpected uses of builtins.  The
/// msg argument should begin with a "%H" so that the location of the
/// expression is printed in the error message.
static bool UnexpectedError(const char *msg, tree exp, Value *&Result) {
  error(msg, &EXPR_LOCATION(exp));

  // Set the Result to an undefined value.
  const Type *ResTy = ConvertType(TREE_TYPE(exp));
  if (ResTy->isSingleValueType())
    Result = UndefValue::get(ResTy);

  // Return true, which can be propagated as the return value of
  // TargetIntrinsicLower, to indicate that no further error message
  // is needed.
  return true;
}

static bool NonImmediateError(tree exp, Value *&Result) {
  return UnexpectedError("%Hlast builtin argument must be an immediate",
                         exp, Result);
}

static bool BadImmediateError(tree exp, Value *&Result) {
  return UnexpectedError("%Hunexpected immediate argument for builtin",
                         exp, Result);
}

static bool BadModeError(tree exp, Value *&Result) {
  return UnexpectedError("%Hunexpected mode for builtin argument",
                         exp, Result);
}

enum neon_datatype {
  neon_datatype_unspecified,
  neon_datatype_signed,
  neon_datatype_unsigned,
  neon_datatype_float,
  neon_datatype_polynomial
};

/// GetBuiltinExtraInfo - Decipher the extra integer immediate argument
/// used with many of GCC's builtins for NEON to distinguish variants of an
/// operation.  The following values for that argument are used:
///   - bit0: For integer types (i.e., bit2 == 0), 0 = unsigned, 1 = signed;
///           otherwise, 0 = polynomial, 1 = float.
///   - bit1: The operation rounds its results.
///   - bit2: 0 = integer datatypes, 1 = floating-point or polynomial.
///   .
/// Returns false if the extra argument is not an integer immediate.
static bool GetBuiltinExtraInfo(const Value *extra_arg,
                                neon_datatype &datatype, bool &isRounded) {
  const ConstantInt *arg = dyn_cast<ConstantInt>(extra_arg);
  if (!arg)
    return false;

  int argval = arg->getZExtValue();
  isRounded = ((argval & 2) != 0);
  if ((argval & 4) == 0) {
    if ((argval & 1) == 0)
      datatype = neon_datatype_unsigned;
    else
      datatype = neon_datatype_signed;
  } else {
    if ((argval & 1) == 0)
      datatype = neon_datatype_polynomial;
    else
      datatype = neon_datatype_float;
  }
  return true;
}

/// BuildConstantSplatVector - Create a ConstantVector with the same value
/// replicated in each element.
static Value *BuildConstantSplatVector(unsigned NumElements, ConstantInt *Val) {
  std::vector<Constant*> CstOps;
  for (unsigned i = 0; i != NumElements; ++i)
    CstOps.push_back(Val);
  return ConstantVector::get(CstOps);
}

/// BuildDup - Build a splat operation to duplicate a value into every
/// element of a vector.
static Value *BuildDup(const Type *ResultType, Value *Val,
                       LLVMBuilder &Builder) {
  // GCC may promote the scalar argument; cast it back.
  const VectorType *VTy = dyn_cast<const VectorType>(ResultType);
  assert(VTy && "expected a vector type");
  const Type *ElTy = VTy->getElementType();
  if (Val->getType() != ElTy) {
    assert(!ElTy->isFloatingPointTy() &&
           "only integer types expected to be promoted");
    Val = Builder.CreateTrunc(Val, ElTy);
  }

  // Insert the value into lane 0 of an undef vector.
  Value *Undef = UndefValue::get(ResultType);
  Value *Result = Builder.CreateInsertElement(Undef, Val, getInt32Const(0));

  // Use a shuffle to move the value into the other lanes.
  unsigned NUnits = VTy->getNumElements();
  if (NUnits > 1) {
    std::vector<Constant*> Idxs;
    for (unsigned i = 0; i != NUnits; ++i)
      Idxs.push_back(getInt32Const(0));
    Result = Builder.CreateShuffleVector(Result, Undef,
                                         ConstantVector::get(Idxs));
  }
  return Result;
}

/// BuildDupLane - Build a splat operation to take a value from one element
/// of a vector and splat it into another vector.
static Value *BuildDupLane(Value *Vec, unsigned LaneVal, unsigned NUnits,
                           LLVMBuilder &Builder) {
  // Translate this to a vector shuffle.
  std::vector<Constant*> Idxs;
  for (unsigned i = 0; i != NUnits; ++i)
    Idxs.push_back(getInt32Const(LaneVal));
  return Builder.CreateShuffleVector(Vec, UndefValue::get(Vec->getType()),
                                     ConstantVector::get(Idxs));
}

// NEON vector shift counts must be in the range 0..ElemBits-1 for left shifts
// or 1..ElemBits for right shifts.  For narrowing shifts, compare against the
// destination element size.  For widening shifts, the upper bound can be
// equal to the element size.  Define separate functions to check these
// constraints, so that the rest of the code for handling vector shift counts
// can be shared.

typedef bool (*ShiftCountChecker)(int Cnt, int ElemBits);

static bool CheckLeftShiftCount(int Cnt, int ElemBits) {
  return (Cnt >= 0 && Cnt < ElemBits);
}

static bool CheckLongLeftShiftCount(int Cnt, int ElemBits) {
  return (Cnt >= 0 && Cnt <= ElemBits);
}

static bool CheckRightShiftCount(int Cnt, int ElemBits) {
  return (Cnt >= 1 && Cnt <= ElemBits);
}

static bool CheckNarrowRightShiftCount(int Cnt, int ElemBits) {
  return (Cnt >= 1 && Cnt <= ElemBits / 2);
}

/// BuildShiftCountVector - Check that the shift count argument to a constant
/// shift builtin is a constant in the appropriate range for the shift
/// operation.  It expands the shift count into a vector, optionally with the
/// count negated for right shifts.  Returns true on success.
static bool BuildShiftCountVector(Value *&Op, enum machine_mode Mode,
                                  ShiftCountChecker CheckCount,
                                  bool NegateRightShift) {
  ConstantInt *Cnt = dyn_cast<ConstantInt>(Op);
  if (!Cnt)
    return false;
  int CntVal = Cnt->getSExtValue();

  assert (VECTOR_MODE_P (Mode) && "expected vector mode for shift");
  unsigned ElemBits = GET_MODE_BITSIZE (GET_MODE_INNER (Mode));
  if (!CheckCount(CntVal, ElemBits))
    return false;

  // Right shifts are represented in NEON intrinsics by a negative shift count.
  Cnt = ConstantInt::get(IntegerType::get(Context, ElemBits),
                         NegateRightShift ? -CntVal : CntVal);
  Op = BuildConstantSplatVector(GET_MODE_NUNITS(Mode), Cnt);
  return true;
}

/// isValidLane - Check if the lane operand for a vector intrinsic is a
/// ConstantInt in the range 0..NUnits.  If pLaneVal is not null, store
/// the lane value to it.
static bool isValidLane(Value *LnOp, int NUnits, unsigned *pLaneVal = 0) {
  ConstantInt *Lane = dyn_cast<ConstantInt>(LnOp);
  if (!Lane)
    return false;

  int LaneVal = Lane->getSExtValue();
  if (LaneVal < 0 || LaneVal >= NUnits)
    return false;

  if (pLaneVal)
    *pLaneVal = LaneVal;
  return true;
}

/// TargetIntrinsicLower - To handle builtins, we want to expand the
/// invocation into normal LLVM code.  If the target can handle the builtin,
/// this function should emit the expanded code and return true.
bool TreeToLLVM::TargetIntrinsicLower(tree exp,
                                      unsigned FnCode,
                                      const MemRef *DestLoc,
                                      Value *&Result,
                                      const Type *ResultType,
                                      std::vector<Value*> &Ops) {
  neon_datatype datatype = neon_datatype_unspecified;
  bool isRounded = false;
  Intrinsic::ID intID = Intrinsic::not_intrinsic;
  Function *intFn;
  const Type* intOpTypes[2];

  if (FnCode < ARM_BUILTIN_NEON_BASE)
    return false;

  neon_builtins neon_code;
  enum insn_code icode = locate_neon_builtin_icode (FnCode, 0, &neon_code);

  // Read the extra immediate argument to the builtin.
  switch (neon_code) {
  default:
    return false;
  case NEON_BUILTIN_vpaddl:
  case NEON_BUILTIN_vneg:
  case NEON_BUILTIN_vqneg:
  case NEON_BUILTIN_vabs:
  case NEON_BUILTIN_vqabs:
  case NEON_BUILTIN_vcls:
  case NEON_BUILTIN_vclz:
  case NEON_BUILTIN_vcnt:
  case NEON_BUILTIN_vrecpe:
  case NEON_BUILTIN_vrsqrte:
  case NEON_BUILTIN_vmvn:
  case NEON_BUILTIN_vcvt:
  case NEON_BUILTIN_vmovn:
  case NEON_BUILTIN_vqmovn:
  case NEON_BUILTIN_vqmovun:
  case NEON_BUILTIN_vmovl:
  case NEON_BUILTIN_vrev64:
  case NEON_BUILTIN_vrev32:
  case NEON_BUILTIN_vrev16:
    if (!GetBuiltinExtraInfo(Ops[1], datatype, isRounded))
      return NonImmediateError(exp, Result);
    break;
  case NEON_BUILTIN_vadd:
  case NEON_BUILTIN_vaddl:
  case NEON_BUILTIN_vaddw:
  case NEON_BUILTIN_vhadd:
  case NEON_BUILTIN_vqadd:
  case NEON_BUILTIN_vaddhn:
  case NEON_BUILTIN_vmul:
  case NEON_BUILTIN_vqdmulh:
  case NEON_BUILTIN_vmull:
  case NEON_BUILTIN_vqdmull:
  case NEON_BUILTIN_vsub:
  case NEON_BUILTIN_vsubl:
  case NEON_BUILTIN_vsubw:
  case NEON_BUILTIN_vqsub:
  case NEON_BUILTIN_vhsub:
  case NEON_BUILTIN_vsubhn:
  case NEON_BUILTIN_vceq:
  case NEON_BUILTIN_vcge:
  case NEON_BUILTIN_vcgt:
  case NEON_BUILTIN_vcage:
  case NEON_BUILTIN_vcagt:
  case NEON_BUILTIN_vtst:
  case NEON_BUILTIN_vabd:
  case NEON_BUILTIN_vabdl:
  case NEON_BUILTIN_vmax:
  case NEON_BUILTIN_vmin:
  case NEON_BUILTIN_vpadd:
  case NEON_BUILTIN_vpadal:
  case NEON_BUILTIN_vpmax:
  case NEON_BUILTIN_vpmin:
  case NEON_BUILTIN_vrecps:
  case NEON_BUILTIN_vrsqrts:
  case NEON_BUILTIN_vshl:
  case NEON_BUILTIN_vqshl:
  case NEON_BUILTIN_vshr_n:
  case NEON_BUILTIN_vshrn_n:
  case NEON_BUILTIN_vqshrn_n:
  case NEON_BUILTIN_vqshrun_n:
  case NEON_BUILTIN_vshl_n:
  case NEON_BUILTIN_vqshl_n:
  case NEON_BUILTIN_vqshlu_n:
  case NEON_BUILTIN_vshll_n:
  case NEON_BUILTIN_vget_lane:
  case NEON_BUILTIN_vcvt_n:
  case NEON_BUILTIN_vmul_n:
  case NEON_BUILTIN_vmull_n:
  case NEON_BUILTIN_vqdmull_n:
  case NEON_BUILTIN_vqdmulh_n:
  case NEON_BUILTIN_vand:
  case NEON_BUILTIN_vorr:
  case NEON_BUILTIN_veor:
  case NEON_BUILTIN_vbic:
  case NEON_BUILTIN_vorn:
    if (!GetBuiltinExtraInfo(Ops[2], datatype, isRounded))
      return NonImmediateError(exp, Result);
    break;
  case NEON_BUILTIN_vmla:
  case NEON_BUILTIN_vmls:
  case NEON_BUILTIN_vmlal:
  case NEON_BUILTIN_vmlsl:
  case NEON_BUILTIN_vqdmlal:
  case NEON_BUILTIN_vqdmlsl:
  case NEON_BUILTIN_vaba:
  case NEON_BUILTIN_vabal:
  case NEON_BUILTIN_vsra_n:
  case NEON_BUILTIN_vmul_lane:
  case NEON_BUILTIN_vmull_lane:
  case NEON_BUILTIN_vqdmull_lane:
  case NEON_BUILTIN_vqdmulh_lane:
  case NEON_BUILTIN_vmla_n:
  case NEON_BUILTIN_vmlal_n:
  case NEON_BUILTIN_vqdmlal_n:
  case NEON_BUILTIN_vmls_n:
  case NEON_BUILTIN_vmlsl_n:
  case NEON_BUILTIN_vqdmlsl_n:
    if (!GetBuiltinExtraInfo(Ops[3], datatype, isRounded))
      return NonImmediateError(exp, Result);
    break;
  case NEON_BUILTIN_vmla_lane:
  case NEON_BUILTIN_vmlal_lane:
  case NEON_BUILTIN_vqdmlal_lane:
  case NEON_BUILTIN_vmls_lane:
  case NEON_BUILTIN_vmlsl_lane:
  case NEON_BUILTIN_vqdmlsl_lane:
    if (!GetBuiltinExtraInfo(Ops[4], datatype, isRounded))
      return NonImmediateError(exp, Result);
    break;
  case NEON_BUILTIN_vsri_n:
  case NEON_BUILTIN_vsli_n:
  case NEON_BUILTIN_vset_lane:
  case NEON_BUILTIN_vcreate:
  case NEON_BUILTIN_vdup_n:
  case NEON_BUILTIN_vdup_lane:
  case NEON_BUILTIN_vcombine:
  case NEON_BUILTIN_vget_high:
  case NEON_BUILTIN_vget_low:
  case NEON_BUILTIN_vtbl1:
  case NEON_BUILTIN_vtbl2:
  case NEON_BUILTIN_vtbl3:
  case NEON_BUILTIN_vtbl4:
  case NEON_BUILTIN_vtbx1:
  case NEON_BUILTIN_vtbx2:
  case NEON_BUILTIN_vtbx3:
  case NEON_BUILTIN_vtbx4:
  case NEON_BUILTIN_vext:
  case NEON_BUILTIN_vbsl:
  case NEON_BUILTIN_vtrn:
  case NEON_BUILTIN_vzip:
  case NEON_BUILTIN_vuzp:
  case NEON_BUILTIN_vld1:
  case NEON_BUILTIN_vld2:
  case NEON_BUILTIN_vld3:
  case NEON_BUILTIN_vld4:
  case NEON_BUILTIN_vld1_lane:
  case NEON_BUILTIN_vld2_lane:
  case NEON_BUILTIN_vld3_lane:
  case NEON_BUILTIN_vld4_lane:
  case NEON_BUILTIN_vld1_dup:
  case NEON_BUILTIN_vld2_dup:
  case NEON_BUILTIN_vld3_dup:
  case NEON_BUILTIN_vld4_dup:
  case NEON_BUILTIN_vst1:
  case NEON_BUILTIN_vst2:
  case NEON_BUILTIN_vst3:
  case NEON_BUILTIN_vst4:
  case NEON_BUILTIN_vst1_lane:
  case NEON_BUILTIN_vst2_lane:
  case NEON_BUILTIN_vst3_lane:
  case NEON_BUILTIN_vst4_lane:
  case NEON_BUILTIN_vreinterpretv8qi:
  case NEON_BUILTIN_vreinterpretv4hi:
  case NEON_BUILTIN_vreinterpretv2si:
  case NEON_BUILTIN_vreinterpretv2sf:
  case NEON_BUILTIN_vreinterpretv1di:
  case NEON_BUILTIN_vreinterpretv16qi:
  case NEON_BUILTIN_vreinterpretv8hi:
  case NEON_BUILTIN_vreinterpretv4si:
  case NEON_BUILTIN_vreinterpretv4sf:
  case NEON_BUILTIN_vreinterpretv2di:
    // No extra argument used here.
    break;
  }

  // Check that the isRounded flag is only set when it is supported.
  if (isRounded) {
    switch (neon_code) {
    case NEON_BUILTIN_vhadd:
    case NEON_BUILTIN_vaddhn:
    case NEON_BUILTIN_vqdmulh:
    case NEON_BUILTIN_vsubhn:
    case NEON_BUILTIN_vshl:
    case NEON_BUILTIN_vqshl:
    case NEON_BUILTIN_vshr_n:
    case NEON_BUILTIN_vshrn_n:
    case NEON_BUILTIN_vqshrn_n:
    case NEON_BUILTIN_vqshrun_n:
    case NEON_BUILTIN_vsra_n:
    case NEON_BUILTIN_vqdmulh_lane:
    case NEON_BUILTIN_vqdmulh_n:
      // These all support a rounded variant.
      break;
    default:
      return BadImmediateError(exp, Result);
    }
  }

  // Check for supported vector modes.

  // Set defaults for mode checking.
  int modeCheckOpnd = 1;
  bool allow_64bit_modes = true;
  bool allow_128bit_modes = true;
  bool allow_8bit_elements = true;
  bool allow_16bit_elements = true;
  bool allow_32bit_elements = true;
  bool allow_64bit_elements = false;
  bool allow_16bit_polynomials = false;

  switch (neon_code) {
  default:
    assert(0 && "unexpected builtin");
    break;

  case NEON_BUILTIN_vadd:
  case NEON_BUILTIN_vsub:
  case NEON_BUILTIN_vqadd:
  case NEON_BUILTIN_vqsub:
  case NEON_BUILTIN_vshl:
  case NEON_BUILTIN_vqshl:
  case NEON_BUILTIN_vshr_n:
  case NEON_BUILTIN_vshl_n:
  case NEON_BUILTIN_vqshl_n:
  case NEON_BUILTIN_vqshlu_n:
  case NEON_BUILTIN_vsra_n:
  case NEON_BUILTIN_vsri_n:
  case NEON_BUILTIN_vsli_n:
  case NEON_BUILTIN_vmvn:
  case NEON_BUILTIN_vext:
  case NEON_BUILTIN_vbsl:
  case NEON_BUILTIN_vand:
  case NEON_BUILTIN_vorr:
  case NEON_BUILTIN_veor:
  case NEON_BUILTIN_vbic:
  case NEON_BUILTIN_vorn:
  case NEON_BUILTIN_vdup_lane:
    allow_64bit_elements = true;
    break;

  case NEON_BUILTIN_vhadd:
  case NEON_BUILTIN_vhsub:
  case NEON_BUILTIN_vmul:
  case NEON_BUILTIN_vceq:
  case NEON_BUILTIN_vcge:
  case NEON_BUILTIN_vcgt:
  case NEON_BUILTIN_vcage:
  case NEON_BUILTIN_vcagt:
  case NEON_BUILTIN_vtst:
  case NEON_BUILTIN_vabd:
  case NEON_BUILTIN_vabdl:
  case NEON_BUILTIN_vaba:
  case NEON_BUILTIN_vmax:
  case NEON_BUILTIN_vmin:
  case NEON_BUILTIN_vpaddl:
  case NEON_BUILTIN_vrecps:
  case NEON_BUILTIN_vrsqrts:
  case NEON_BUILTIN_vneg:
  case NEON_BUILTIN_vqneg:
  case NEON_BUILTIN_vabs:
  case NEON_BUILTIN_vqabs:
  case NEON_BUILTIN_vcls:
  case NEON_BUILTIN_vclz:
  case NEON_BUILTIN_vtrn:
  case NEON_BUILTIN_vzip:
  case NEON_BUILTIN_vuzp:
    break;

  case NEON_BUILTIN_vabal:
  case NEON_BUILTIN_vmla:
  case NEON_BUILTIN_vmls:
  case NEON_BUILTIN_vpadal:
    modeCheckOpnd = 2;
    break;

  case NEON_BUILTIN_vaddhn:
  case NEON_BUILTIN_vsubhn:
  case NEON_BUILTIN_vshrn_n:
  case NEON_BUILTIN_vqshrn_n:
  case NEON_BUILTIN_vqshrun_n:
  case NEON_BUILTIN_vmovn:
  case NEON_BUILTIN_vqmovn:
  case NEON_BUILTIN_vqmovun:
    allow_64bit_modes = false;
    allow_8bit_elements = false;
    allow_64bit_elements = true;
    break;

  case NEON_BUILTIN_vqdmulh:
  case NEON_BUILTIN_vqdmulh_lane:
  case NEON_BUILTIN_vqdmulh_n:
  case NEON_BUILTIN_vmul_lane:
  case NEON_BUILTIN_vmul_n:
  case NEON_BUILTIN_vmla_lane:
  case NEON_BUILTIN_vmla_n:
  case NEON_BUILTIN_vmls_lane:
  case NEON_BUILTIN_vmls_n:
    allow_8bit_elements = false;
    break;

  case NEON_BUILTIN_vqdmull:
  case NEON_BUILTIN_vqdmull_lane:
  case NEON_BUILTIN_vqdmull_n:
  case NEON_BUILTIN_vmull_lane:
  case NEON_BUILTIN_vmull_n:
    allow_128bit_modes = false;
    allow_8bit_elements = false;
    break;

  case NEON_BUILTIN_vqdmlal:
  case NEON_BUILTIN_vqdmlal_lane:
  case NEON_BUILTIN_vqdmlal_n:
  case NEON_BUILTIN_vqdmlsl:
  case NEON_BUILTIN_vmlal_lane:
  case NEON_BUILTIN_vmlal_n:
  case NEON_BUILTIN_vmlsl_lane:
  case NEON_BUILTIN_vmlsl_n:
  case NEON_BUILTIN_vqdmlsl_lane:
  case NEON_BUILTIN_vqdmlsl_n:
    modeCheckOpnd = 2;
    allow_128bit_modes = false;
    allow_8bit_elements = false;
    break;

  case NEON_BUILTIN_vaddw:
  case NEON_BUILTIN_vmlal:
  case NEON_BUILTIN_vmlsl:
  case NEON_BUILTIN_vsubw:
    modeCheckOpnd = 2;
    allow_128bit_modes = false;
    break;

  case NEON_BUILTIN_vaddl:
  case NEON_BUILTIN_vmull:
  case NEON_BUILTIN_vsubl:
  case NEON_BUILTIN_vpadd:
  case NEON_BUILTIN_vpmax:
  case NEON_BUILTIN_vpmin:
  case NEON_BUILTIN_vshll_n:
  case NEON_BUILTIN_vmovl:
    allow_128bit_modes = false;
    break;

  case NEON_BUILTIN_vcnt:
    allow_16bit_elements = false;
    allow_32bit_elements = false;
    break;

  case NEON_BUILTIN_vtbl1:
  case NEON_BUILTIN_vtbl2:
  case NEON_BUILTIN_vtbl3:
  case NEON_BUILTIN_vtbl4:
  case NEON_BUILTIN_vtbx1:
  case NEON_BUILTIN_vtbx2:
  case NEON_BUILTIN_vtbx3:
  case NEON_BUILTIN_vtbx4:
    allow_16bit_elements = false;
    allow_32bit_elements = false;
    allow_128bit_modes = false;
    modeCheckOpnd = 0;
    break;

  case NEON_BUILTIN_vrecpe:
  case NEON_BUILTIN_vrsqrte:
  case NEON_BUILTIN_vcvt:
  case NEON_BUILTIN_vcvt_n:
    allow_8bit_elements = false;
    allow_16bit_elements = false;
    break;

  case NEON_BUILTIN_vget_lane:
    allow_64bit_elements = true;
    allow_16bit_polynomials = true;
    break;

  case NEON_BUILTIN_vset_lane:
    allow_64bit_elements = true;
    allow_16bit_polynomials = true;
    modeCheckOpnd = 2;
    break;

  case NEON_BUILTIN_vrev64:
    allow_16bit_polynomials = true;
    break;

  case NEON_BUILTIN_vrev32:
    allow_16bit_polynomials = true;
    allow_32bit_elements = false;
    break;

  case NEON_BUILTIN_vrev16:
    allow_16bit_elements = false;
    allow_32bit_elements = false;
    break;

  case NEON_BUILTIN_vcreate:
    modeCheckOpnd = 0;
    allow_128bit_modes = false;
    allow_64bit_elements = true;
    break;

  case NEON_BUILTIN_vdup_n:
    modeCheckOpnd = 0;
    allow_64bit_elements = true;
    break;

  case NEON_BUILTIN_vcombine:
  case NEON_BUILTIN_vreinterpretv8qi:
  case NEON_BUILTIN_vreinterpretv4hi:
  case NEON_BUILTIN_vreinterpretv2si:
  case NEON_BUILTIN_vreinterpretv2sf:
  case NEON_BUILTIN_vreinterpretv1di:
    allow_128bit_modes = false;
    allow_64bit_elements = true;
    break;

  case NEON_BUILTIN_vget_high:
  case NEON_BUILTIN_vget_low:
  case NEON_BUILTIN_vreinterpretv16qi:
  case NEON_BUILTIN_vreinterpretv8hi:
  case NEON_BUILTIN_vreinterpretv4si:
  case NEON_BUILTIN_vreinterpretv4sf:
  case NEON_BUILTIN_vreinterpretv2di:
    allow_64bit_modes = false;
    allow_64bit_elements = true;
    break;

  case NEON_BUILTIN_vld1:
  case NEON_BUILTIN_vld2:
  case NEON_BUILTIN_vld3:
  case NEON_BUILTIN_vld4:
  case NEON_BUILTIN_vld1_lane:
  case NEON_BUILTIN_vld2_lane:
  case NEON_BUILTIN_vld3_lane:
  case NEON_BUILTIN_vld4_lane:
  case NEON_BUILTIN_vld1_dup:
  case NEON_BUILTIN_vld2_dup:
  case NEON_BUILTIN_vld3_dup:
  case NEON_BUILTIN_vld4_dup:
  case NEON_BUILTIN_vst1:
  case NEON_BUILTIN_vst2:
  case NEON_BUILTIN_vst3:
  case NEON_BUILTIN_vst4:
  case NEON_BUILTIN_vst1_lane:
  case NEON_BUILTIN_vst2_lane:
  case NEON_BUILTIN_vst3_lane:
  case NEON_BUILTIN_vst4_lane:
    // Most of the load/store builtins do not have operands with the mode of
    // the operation.  Skip the mode check, since there is no extra operand
    // to check against the mode anyway.
    modeCheckOpnd = -1;
    break;
  }

  if (modeCheckOpnd >= 0) {

    switch (insn_data[icode].operand[modeCheckOpnd].mode) {
    case V8QImode: case V4HImode: case V2SImode: case V1DImode: case V2SFmode:
      if (!allow_64bit_modes)
        return BadModeError(exp, Result);
      break;
    case V16QImode: case V8HImode: case V4SImode: case V2DImode: case V4SFmode:
      if (!allow_128bit_modes)
        return BadModeError(exp, Result);
      break;
    default:
      return BadModeError(exp, Result);
    }

    if (datatype == neon_datatype_polynomial) {

      switch (insn_data[icode].operand[modeCheckOpnd].mode) {
      case V8QImode: case V16QImode:
        break;
      case V4HImode: case V8HImode:
        if (!allow_16bit_polynomials)
          return BadModeError(exp, Result);
        break;
      default:
        return BadModeError(exp, Result);
      }

    } else if (datatype == neon_datatype_float) {

      switch (insn_data[icode].operand[modeCheckOpnd].mode) {
      case V2SFmode: case V4SFmode:
        break;
      default:
        return BadModeError(exp, Result);
      }

    } else {

      switch (insn_data[icode].operand[modeCheckOpnd].mode) {
      case V8QImode: case V16QImode:
        if (!allow_8bit_elements)
          return BadModeError(exp, Result);
        break;
      case V4HImode: case V8HImode:
        if (!allow_16bit_elements)
          return BadModeError(exp, Result);
        break;
      case V2SImode: case V4SImode:
      case V2SFmode: case V4SFmode:
        if (!allow_32bit_elements)
          return BadModeError(exp, Result);
        break;
      case V1DImode: case V2DImode:
        if (!allow_64bit_elements)
          return BadModeError(exp, Result);
        break;
      default:
        return BadModeError(exp, Result);
      }
    }
  }

  // Now translate the builtin to LLVM.

  switch (neon_code) {
  default:
    assert(0 && "unimplemented builtin");
    break;

  case NEON_BUILTIN_vadd:
    if (datatype == neon_datatype_polynomial)
      return BadImmediateError(exp, Result);
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFAdd(Ops[0], Ops[1]);
    else
      Result = Builder.CreateAdd(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vaddl:
    if (datatype == neon_datatype_signed) {
      Ops[0] = Builder.CreateSExt(Ops[0], ResultType);
      Ops[1] = Builder.CreateSExt(Ops[1], ResultType);
    } else if (datatype == neon_datatype_unsigned) {
      Ops[0] = Builder.CreateZExt(Ops[0], ResultType);
      Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
    } else
      return BadImmediateError(exp, Result);

    Result = Builder.CreateAdd(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vaddw:
    if (datatype == neon_datatype_signed)
      Ops[1] = Builder.CreateSExt(Ops[1], ResultType);
    else if (datatype == neon_datatype_unsigned)
      Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
    else
      return BadImmediateError(exp, Result);

    Result = Builder.CreateAdd(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vhadd:
    if (datatype == neon_datatype_signed)
      intID = (isRounded ?
               Intrinsic::arm_neon_vrhadds :
               Intrinsic::arm_neon_vhadds);
    else if (datatype == neon_datatype_unsigned)
      intID = (isRounded ?
               Intrinsic::arm_neon_vrhaddu :
               Intrinsic::arm_neon_vhaddu);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqadd:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vqadds;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vqaddu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vaddhn:
    if (datatype == neon_datatype_signed ||
        datatype == neon_datatype_unsigned)
      intID = (isRounded ?
               Intrinsic::arm_neon_vraddhn :
               Intrinsic::arm_neon_vaddhn);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vmul_lane:
  case NEON_BUILTIN_vmul_n:
    if (datatype == neon_datatype_polynomial)
      return BadImmediateError(exp, Result);
    // fall through....
  case NEON_BUILTIN_vmul:
    if (neon_code == NEON_BUILTIN_vmul_n) {
      Ops[1] = BuildDup(Ops[0]->getType(), Ops[1], Builder);
    } else if (neon_code == NEON_BUILTIN_vmul_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
      if (!isValidLane(Ops[2], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[1] = BuildDupLane(Ops[1], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_polynomial) {
      intID = Intrinsic::arm_neon_vmulp;
      intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
      Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    } else if (datatype == neon_datatype_float)
      Result = Builder.CreateFMul(Ops[0], Ops[1]);
    else
      Result = Builder.CreateMul(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vmla_lane:
  case NEON_BUILTIN_vmla_n:
  case NEON_BUILTIN_vmla:
    if (neon_code == NEON_BUILTIN_vmla_n) {
      Ops[2] = BuildDup(Ops[1]->getType(), Ops[2], Builder);
    } else if (neon_code == NEON_BUILTIN_vmla_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[2].mode);
      if (!isValidLane(Ops[3], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[2] = BuildDupLane(Ops[2], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_polynomial)
      return BadImmediateError(exp, Result);
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFAdd(Ops[0], Builder.CreateFMul(Ops[1], Ops[2]));
    else
      Result = Builder.CreateAdd(Ops[0], Builder.CreateMul(Ops[1], Ops[2]));
    break;

  case NEON_BUILTIN_vmls_lane:
  case NEON_BUILTIN_vmls_n:
  case NEON_BUILTIN_vmls:
    if (neon_code == NEON_BUILTIN_vmls_n) {
      Ops[2] = BuildDup(Ops[1]->getType(), Ops[2], Builder);
    } else if (neon_code == NEON_BUILTIN_vmls_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[2].mode);
      if (!isValidLane(Ops[3], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[2] = BuildDupLane(Ops[2], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_polynomial)
      return BadImmediateError(exp, Result);
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFSub(Ops[0], Builder.CreateFMul(Ops[1], Ops[2]));
    else
      Result = Builder.CreateSub(Ops[0], Builder.CreateMul(Ops[1], Ops[2]));
    break;

  case NEON_BUILTIN_vmlal_lane:
  case NEON_BUILTIN_vmlal_n:
  case NEON_BUILTIN_vmlal:
    if (neon_code == NEON_BUILTIN_vmlal_n) {
      Ops[2] = BuildDup(Ops[1]->getType(), Ops[2], Builder);
    } else if (neon_code == NEON_BUILTIN_vmlal_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[2].mode);
      if (!isValidLane(Ops[3], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[2] = BuildDupLane(Ops[2], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_signed) {
      Ops[1] = Builder.CreateSExt(Ops[1], ResultType);
      Ops[2] = Builder.CreateSExt(Ops[2], ResultType);
    } else if (datatype == neon_datatype_unsigned) {
      Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
      Ops[2] = Builder.CreateZExt(Ops[2], ResultType);
    } else
      return BadImmediateError(exp, Result);

    Ops[1] = Builder.CreateMul(Ops[1], Ops[2]);
    Result = Builder.CreateAdd(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vmlsl_lane:
  case NEON_BUILTIN_vmlsl_n:
  case NEON_BUILTIN_vmlsl:
    if (neon_code == NEON_BUILTIN_vmlsl_n) {
      Ops[2] = BuildDup(Ops[1]->getType(), Ops[2], Builder);
    } else if (neon_code == NEON_BUILTIN_vmlsl_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[2].mode);
      if (!isValidLane(Ops[3], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[2] = BuildDupLane(Ops[2], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_signed) {
      Ops[1] = Builder.CreateSExt(Ops[1], ResultType);
      Ops[2] = Builder.CreateSExt(Ops[2], ResultType);
    } else if (datatype == neon_datatype_unsigned) {
      Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
      Ops[2] = Builder.CreateZExt(Ops[2], ResultType);
    } else
      return BadImmediateError(exp, Result);

    Ops[1] = Builder.CreateMul(Ops[1], Ops[2]);
    Result = Builder.CreateSub(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqdmulh_lane:
  case NEON_BUILTIN_vqdmulh_n:
  case NEON_BUILTIN_vqdmulh:
    if (neon_code == NEON_BUILTIN_vqdmulh_n) {
      Ops[1] = BuildDup(Ops[0]->getType(), Ops[1], Builder);
    } else if (neon_code == NEON_BUILTIN_vqdmulh_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
      if (!isValidLane(Ops[2], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[1] = BuildDupLane(Ops[1], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_signed)
      intID = (isRounded ?
               Intrinsic::arm_neon_vqrdmulh :
               Intrinsic::arm_neon_vqdmulh);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqdmlal_lane:
  case NEON_BUILTIN_vqdmlal_n:
  case NEON_BUILTIN_vqdmlal:
    if (neon_code == NEON_BUILTIN_vqdmlal_n) {
      Ops[2] = BuildDup(Ops[1]->getType(), Ops[2], Builder);
    } else if (neon_code == NEON_BUILTIN_vqdmlal_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[2].mode);
      if (!isValidLane(Ops[3], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[2] = BuildDupLane(Ops[2], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vqdmlal;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall3(intFn, Ops[0], Ops[1], Ops[2]);
    break;

  case NEON_BUILTIN_vqdmlsl_lane:
  case NEON_BUILTIN_vqdmlsl_n:
  case NEON_BUILTIN_vqdmlsl:
    if (neon_code == NEON_BUILTIN_vqdmlsl_n) {
      Ops[2] = BuildDup(Ops[1]->getType(), Ops[2], Builder);
    } else if (neon_code == NEON_BUILTIN_vqdmlsl_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[2].mode);
      if (!isValidLane(Ops[3], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[2] = BuildDupLane(Ops[2], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vqdmlsl;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall3(intFn, Ops[0], Ops[1], Ops[2]);
    break;

  case NEON_BUILTIN_vmull_lane:
  case NEON_BUILTIN_vmull_n:
  case NEON_BUILTIN_vmull:
    if (neon_code == NEON_BUILTIN_vmull_n) {
      Ops[1] = BuildDup(Ops[0]->getType(), Ops[1], Builder);
    } else if (neon_code == NEON_BUILTIN_vmull_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
      if (!isValidLane(Ops[2], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[1] = BuildDupLane(Ops[1], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_polynomial) {
      intID = Intrinsic::arm_neon_vmullp;
      intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
      Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
      break;
    }
    if (datatype == neon_datatype_signed) {
      Ops[0] = Builder.CreateSExt(Ops[0], ResultType);
      Ops[1] = Builder.CreateSExt(Ops[1], ResultType);
    } else if (datatype == neon_datatype_unsigned) {
      Ops[0] = Builder.CreateZExt(Ops[0], ResultType);
      Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
    } else
      return BadImmediateError(exp, Result);

    Result = Builder.CreateMul(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqdmull_n:
  case NEON_BUILTIN_vqdmull_lane:
  case NEON_BUILTIN_vqdmull:
    if (neon_code == NEON_BUILTIN_vqdmull_n) {
      Ops[1] = BuildDup(Ops[0]->getType(), Ops[1], Builder);
    } else if (neon_code == NEON_BUILTIN_vqdmull_lane) {
      unsigned LaneVal;
      unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
      if (!isValidLane(Ops[2], NUnits, &LaneVal))
        return UnexpectedError("%Hinvalid lane number", exp, Result);
      Ops[1] = BuildDupLane(Ops[1], LaneVal, NUnits, Builder);
    }
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vqdmull;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vshl_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckLeftShiftCount, false))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (datatype == neon_datatype_signed ||
        datatype == neon_datatype_unsigned)
      Result = Builder.CreateShl(Ops[0], Ops[1]);
    else
      return BadImmediateError(exp, Result);
    break;

  case NEON_BUILTIN_vshr_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckRightShiftCount, isRounded))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (!isRounded) {
      if (datatype == neon_datatype_signed)
        Result = Builder.CreateAShr(Ops[0], Ops[1]);
      else if (datatype == neon_datatype_unsigned)
        Result = Builder.CreateLShr(Ops[0], Ops[1]);
      else
        return BadImmediateError(exp, Result);
      break;
    }
    // fall through....
  case NEON_BUILTIN_vshl:
    if (datatype == neon_datatype_signed)
      intID = (isRounded ?
               Intrinsic::arm_neon_vrshifts :
               Intrinsic::arm_neon_vshifts);
    else if (datatype == neon_datatype_unsigned)
      intID = (isRounded ?
               Intrinsic::arm_neon_vrshiftu :
               Intrinsic::arm_neon_vshiftu);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vshrn_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckNarrowRightShiftCount, true))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (datatype == neon_datatype_signed ||
        datatype == neon_datatype_unsigned)
      intID = (isRounded ?
               Intrinsic::arm_neon_vrshiftn :
               Intrinsic::arm_neon_vshiftn);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqshl_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckLeftShiftCount, false))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    // fall through....
  case NEON_BUILTIN_vqshl:
    if (datatype == neon_datatype_signed)
      intID = (isRounded ?
               Intrinsic::arm_neon_vqrshifts :
               Intrinsic::arm_neon_vqshifts);
    else if (datatype == neon_datatype_unsigned)
      intID = (isRounded ?
               Intrinsic::arm_neon_vqrshiftu :
               Intrinsic::arm_neon_vqshiftu);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqshlu_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckLeftShiftCount, false))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (datatype != neon_datatype_signed)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vqshiftsu;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqshrn_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckNarrowRightShiftCount, true))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (datatype == neon_datatype_signed)
      intID = (isRounded ?
               Intrinsic::arm_neon_vqrshiftns :
               Intrinsic::arm_neon_vqshiftns);
    else if (datatype == neon_datatype_unsigned)
      intID = (isRounded ?
               Intrinsic::arm_neon_vqrshiftnu :
               Intrinsic::arm_neon_vqshiftnu);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqshrun_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckNarrowRightShiftCount, true))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (datatype == neon_datatype_signed)
      intID = (isRounded ?
               Intrinsic::arm_neon_vqrshiftnsu :
               Intrinsic::arm_neon_vqshiftnsu);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vshll_n:
    if (!BuildShiftCountVector(Ops[1], insn_data[icode].operand[1].mode,
                               CheckLongLeftShiftCount, false))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vshiftls;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vshiftlu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vsra_n:
    if (!BuildShiftCountVector(Ops[2], insn_data[icode].operand[1].mode,
                               CheckRightShiftCount, isRounded))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    if (!isRounded) {
      if (datatype == neon_datatype_signed)
        Result = Builder.CreateAShr(Ops[1], Ops[2]);
      else if (datatype == neon_datatype_unsigned)
        Result = Builder.CreateLShr(Ops[1], Ops[2]);
      else
        return BadImmediateError(exp, Result);
    } else {
      if (datatype == neon_datatype_signed)
        intID = (isRounded ?
                 Intrinsic::arm_neon_vrshifts :
                 Intrinsic::arm_neon_vshifts);
      else if (datatype == neon_datatype_unsigned)
        intID = (isRounded ?
                 Intrinsic::arm_neon_vrshiftu :
                 Intrinsic::arm_neon_vshiftu);
      else
        return BadImmediateError(exp, Result);

      intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
      Result = Builder.CreateCall2(intFn, Ops[1], Ops[2]);
    }
    Result = Builder.CreateAdd(Ops[0], Result);
    break;

  case NEON_BUILTIN_vsub:
    if (datatype == neon_datatype_polynomial)
      return BadImmediateError(exp, Result);
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFSub(Ops[0], Ops[1]);
    else
      Result = Builder.CreateSub(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vsubl:
    if (datatype == neon_datatype_signed) {
      Ops[0] = Builder.CreateSExt(Ops[0], ResultType);
      Ops[1] = Builder.CreateSExt(Ops[1], ResultType);
    } else if (datatype == neon_datatype_unsigned) {
      Ops[0] = Builder.CreateZExt(Ops[0], ResultType);
      Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
    } else
      return BadImmediateError(exp, Result);

    Result = Builder.CreateSub(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vsubw:
    if (datatype == neon_datatype_signed)
      Ops[1] = Builder.CreateSExt(Ops[1], ResultType);
    else if (datatype == neon_datatype_unsigned)
      Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
    else
      return BadImmediateError(exp, Result);

    Result = Builder.CreateSub(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vqsub:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vqsubs;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vqsubu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vhsub:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vhsubs;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vhsubu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vsubhn:
    if (datatype == neon_datatype_signed ||
        datatype == neon_datatype_unsigned)
      intID = (isRounded ?
               Intrinsic::arm_neon_vrsubhn :
               Intrinsic::arm_neon_vsubhn);
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vceq:
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFCmp(FCmpInst::FCMP_OEQ, Ops[0], Ops[1]);
    else
      Result = Builder.CreateICmp(ICmpInst::ICMP_EQ, Ops[0], Ops[1]);
    Result = Builder.CreateSExt(Result, ResultType);
    break;

  case NEON_BUILTIN_vcge:
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFCmp(FCmpInst::FCMP_OGE, Ops[0], Ops[1]);
    else if (datatype == neon_datatype_signed)
      Result = Builder.CreateICmp(ICmpInst::ICMP_SGE, Ops[0], Ops[1]);
    else if (datatype == neon_datatype_unsigned)
      Result = Builder.CreateICmp(ICmpInst::ICMP_UGE, Ops[0], Ops[1]);
    else
      return BadImmediateError(exp, Result);
    Result = Builder.CreateSExt(Result, ResultType);
    break;

  case NEON_BUILTIN_vcgt:
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFCmp(FCmpInst::FCMP_OGT, Ops[0], Ops[1]);
    else if (datatype == neon_datatype_signed)
      Result = Builder.CreateICmp(ICmpInst::ICMP_SGT, Ops[0], Ops[1]);
    else if (datatype == neon_datatype_unsigned)
      Result = Builder.CreateICmp(ICmpInst::ICMP_UGT, Ops[0], Ops[1]);
    else
      return BadImmediateError(exp, Result);
    Result = Builder.CreateSExt(Result, ResultType);
    break;

  case NEON_BUILTIN_vcage:
    if (datatype != neon_datatype_float)
      return BadImmediateError(exp, Result);

    switch (insn_data[icode].operand[1].mode) {
    case V2SFmode:
      intID = Intrinsic::arm_neon_vacged;
      break;
    case V4SFmode:
      intID = Intrinsic::arm_neon_vacgeq;
      break;
    default:
      return BadModeError(exp, Result);
    }

    intFn = Intrinsic::getDeclaration(TheModule, intID);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vcagt:
    if (datatype != neon_datatype_float)
      return BadImmediateError(exp, Result);

    switch (insn_data[icode].operand[1].mode) {
    case V2SFmode:
      intID = Intrinsic::arm_neon_vacgtd;
      break;
    case V4SFmode:
      intID = Intrinsic::arm_neon_vacgtq;
      break;
    default:
      return BadModeError(exp, Result);
    }

    intFn = Intrinsic::getDeclaration(TheModule, intID);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vtst:
    if (datatype == neon_datatype_float)
      return BadImmediateError(exp, Result);
    Result = Builder.CreateICmp(ICmpInst::ICMP_NE,
                                Builder.CreateAnd(Ops[0], Ops[1]),
                                ConstantAggregateZero::get(ResultType));
    Result = Builder.CreateSExt(Result, ResultType);
    break;

  case NEON_BUILTIN_vabd:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vabds;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vabdu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vabdl: {
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vabds;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vabdu;
    else
      return BadImmediateError(exp, Result);

    const VectorType *VTy = dyn_cast<const VectorType>(ResultType);
    assert(VTy && "expected a vector type for vabdl result");
    const llvm::Type *DTy = VectorType::getTruncatedElementVectorType(VTy);
    intFn = Intrinsic::getDeclaration(TheModule, intID, &DTy, 1);
    Ops[0] = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    Result = Builder.CreateZExt(Ops[0], ResultType);
    break;
  }

  case NEON_BUILTIN_vaba:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vabds;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vabdu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Ops[1] = Builder.CreateCall2(intFn, Ops[1], Ops[2]);
    Result = Builder.CreateAdd(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vabal: {
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vabds;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vabdu;
    else
      return BadImmediateError(exp, Result);

    const VectorType *VTy = dyn_cast<const VectorType>(ResultType);
    assert(VTy && "expected a vector type for vabal result");
    const llvm::Type *DTy = VectorType::getTruncatedElementVectorType(VTy);
    intFn = Intrinsic::getDeclaration(TheModule, intID, &DTy, 1);
    Ops[1] = Builder.CreateCall2(intFn, Ops[1], Ops[2]);
    Ops[1] = Builder.CreateZExt(Ops[1], ResultType);
    Result = Builder.CreateAdd(Ops[0], Ops[1]);
    break;
  }

  case NEON_BUILTIN_vmax:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vmaxs;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vmaxu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vmin:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vmins;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vminu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vpadd:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_signed ||
        datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vpadd;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vpaddl:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vpaddls;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vpaddlu;
    else
      return BadImmediateError(exp, Result);

    intOpTypes[0] = ResultType;
    intOpTypes[1] = Ops[0]->getType();
    intFn = Intrinsic::getDeclaration(TheModule, intID, intOpTypes, 2);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vpadal:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vpadals;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vpadalu;
    else
      return BadImmediateError(exp, Result);

    intOpTypes[0] = ResultType;
    intOpTypes[1] = Ops[1]->getType();
    intFn = Intrinsic::getDeclaration(TheModule, intID, intOpTypes, 2);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vpmax:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vpmaxs;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vpmaxu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vpmin:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vpmins;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vpminu;
    else
      return BadImmediateError(exp, Result);

    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vrecps:
    if (datatype != neon_datatype_float)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vrecps;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vrsqrts:
    if (datatype != neon_datatype_float)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vrsqrts;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vsri_n:
    if (!BuildShiftCountVector(Ops[2], insn_data[icode].operand[1].mode,
                               CheckRightShiftCount, true))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    intID = Intrinsic::arm_neon_vshiftins;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall3(intFn, Ops[0], Ops[1], Ops[2]);
    break;

  case NEON_BUILTIN_vsli_n:
    if (!BuildShiftCountVector(Ops[2], insn_data[icode].operand[1].mode,
                               CheckLeftShiftCount, false))
      return UnexpectedError("%Hinvalid shift count", exp, Result);
    intID = Intrinsic::arm_neon_vshiftins;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall3(intFn, Ops[0], Ops[1], Ops[2]);
    break;

  case NEON_BUILTIN_vabs:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vabs;
    else
      return BadImmediateError(exp, Result);
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vqabs:
    if (datatype != neon_datatype_signed)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vqabs;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vneg:
    if (datatype != neon_datatype_signed &&
        datatype != neon_datatype_float)
      return BadImmediateError(exp, Result);
    if (datatype == neon_datatype_float)
      Result = Builder.CreateFNeg(Ops[0]);
    else
      Result = Builder.CreateNeg(Ops[0]);
    break;

  case NEON_BUILTIN_vqneg:
    if (datatype != neon_datatype_signed)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vqneg;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vcls:
    if (datatype != neon_datatype_signed)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vcls;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vclz:
    if (datatype != neon_datatype_signed &&
        datatype != neon_datatype_unsigned)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vclz;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vcnt:
    if (datatype == neon_datatype_float)
      return BadImmediateError(exp, Result);
    intID = Intrinsic::arm_neon_vcnt;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vrecpe:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vrecpe;
    else
      return BadImmediateError(exp, Result);
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vrsqrte:
    if (datatype == neon_datatype_float ||
        datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vrsqrte;
    else
      return BadImmediateError(exp, Result);
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vmvn:
    if (datatype == neon_datatype_float)
      return BadImmediateError(exp, Result);
    Result = Builder.CreateNot(Ops[0]);
    break;

  case NEON_BUILTIN_vget_lane: {
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
    if (!isValidLane(Ops[1], NUnits))
      return UnexpectedError("%Hinvalid lane number", exp, Result);
    Result = Builder.CreateExtractElement(Ops[0], Ops[1]);
    break;
  }

  case NEON_BUILTIN_vset_lane: {
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[2].mode);
    if (!isValidLane(Ops[2], NUnits))
      return UnexpectedError("%Hinvalid lane number", exp, Result);
    // GCC may promote the scalar argument; cast it back.
    const VectorType *VTy = dyn_cast<const VectorType>(Ops[1]->getType());
    assert(VTy && "expected a vector type for vset_lane vector operand");
    const Type *ElTy = VTy->getElementType();
    if (Ops[0]->getType() != ElTy) {
      assert(!ElTy->isFloatingPointTy() &&
             "only integer types expected to be promoted");
      Ops[0] = Builder.CreateTrunc(Ops[0], ElTy);
    }
    Result = Builder.CreateInsertElement(Ops[1], Ops[0], Ops[2]);
    break;
  }

  case NEON_BUILTIN_vcreate:
    Result = Builder.CreateBitCast(Ops[0], ResultType);
    break;

  case NEON_BUILTIN_vdup_n:
    Result = BuildDup(ResultType, Ops[0], Builder);
    break;

  case NEON_BUILTIN_vdup_lane: {
    unsigned LaneVal;
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
    if (!isValidLane(Ops[1], NUnits, &LaneVal))
      return UnexpectedError("%Hinvalid lane number", exp, Result);
    unsigned DstUnits = GET_MODE_NUNITS(insn_data[icode].operand[0].mode);
    Result = BuildDupLane(Ops[0], LaneVal, DstUnits, Builder);
    break;
  }

  case NEON_BUILTIN_vcombine: {
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[0].mode);
    std::vector<Constant*> Idxs;
    for (unsigned i = 0; i != NUnits; ++i)
      Idxs.push_back(getInt32Const(i));
    Result = Builder.CreateShuffleVector(Ops[0], Ops[1],
                                         ConstantVector::get(Idxs));
    break;
  }

  case NEON_BUILTIN_vget_high:
  case NEON_BUILTIN_vget_low: {
    const Type *v2f64Ty = VectorType::get(Type::getDoubleTy(Context), 2);
    unsigned Idx = (neon_code == NEON_BUILTIN_vget_low ? 0 : 1);
    Result = Builder.CreateBitCast(Ops[0], v2f64Ty);
    Result = Builder.CreateExtractElement(Result, getInt32Const(Idx));
    Result = Builder.CreateBitCast(Result, ResultType);
    break;
  }

  case NEON_BUILTIN_vmovn:
    if (datatype == neon_datatype_signed ||
        datatype == neon_datatype_unsigned)
      Result = Builder.CreateTrunc(Ops[0], ResultType);
    else
      return BadImmediateError(exp, Result);
    break;

  case NEON_BUILTIN_vqmovn:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vqmovns;
    else if (datatype == neon_datatype_unsigned)
      intID = Intrinsic::arm_neon_vqmovnu;
    else
      return BadImmediateError(exp, Result);
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vqmovun:
    if (datatype == neon_datatype_signed)
      intID = Intrinsic::arm_neon_vqmovnsu;
    else
      return BadImmediateError(exp, Result);
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Result = Builder.CreateCall(intFn, Ops[0]);
    break;

  case NEON_BUILTIN_vmovl:
    if (datatype == neon_datatype_signed)
      Result = Builder.CreateSExt(Ops[0], ResultType);
    else if (datatype == neon_datatype_unsigned)
      Result = Builder.CreateZExt(Ops[0], ResultType);
    else
      return BadImmediateError(exp, Result);
    break;

  case NEON_BUILTIN_vext: {
    // Check if immediate operand is valid.
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
    ConstantInt *Imm = dyn_cast<ConstantInt>(Ops[2]);
    if (!Imm)
      return UnexpectedError("%Hinvalid immediate for vext", exp, Result);
    int ImmVal = Imm->getSExtValue();
    if (ImmVal < 0 || ImmVal >= (int)NUnits)
      return UnexpectedError("%Hout of range immediate for vext", exp, Result);
    if (ImmVal == 0) {
      Result = Ops[0];
      break;
    }
    // Translate to a vector shuffle.
    std::vector<Constant*> Idxs;
    for (unsigned i = 0; i != NUnits; ++i)
      Idxs.push_back(getInt32Const(i + ImmVal));
    Result = Builder.CreateShuffleVector(Ops[0], Ops[1],
                                         ConstantVector::get(Idxs));
    break;
  }

  case NEON_BUILTIN_vrev64:
  case NEON_BUILTIN_vrev32:
  case NEON_BUILTIN_vrev16: {
    unsigned ChunkBits = 0;
    switch (neon_code) {
    case NEON_BUILTIN_vrev64: ChunkBits = 64; break;
    case NEON_BUILTIN_vrev32: ChunkBits = 32; break;
    case NEON_BUILTIN_vrev16: ChunkBits = 16; break;
    default: assert(false);
    }
    const VectorType *VTy = dyn_cast<const VectorType>(ResultType);
    assert(VTy && "expected a vector type");
    const Type *ElTy = VTy->getElementType();
    unsigned ChunkElts = ChunkBits / ElTy->getPrimitiveSizeInBits();

    // Translate to a vector shuffle.
    std::vector<Constant*> Idxs;
    unsigned NUnits = VTy->getNumElements();
    for (unsigned c = ChunkElts; c <= NUnits; c += ChunkElts) {
      for (unsigned i = 0; i != ChunkElts; ++i) {
        Idxs.push_back(getInt32Const(c - i - 1));
      }
    }
    Result = Builder.CreateShuffleVector(Ops[0], UndefValue::get(ResultType),
                                         ConstantVector::get(Idxs));
    break;
  }

  case NEON_BUILTIN_vcvt:
    if (FLOAT_MODE_P(insn_data[icode].operand[1].mode)) {
      if (datatype == neon_datatype_unsigned)
        Result = Builder.CreateFPToUI(Ops[0], ResultType);
      else if (datatype == neon_datatype_signed)
        Result = Builder.CreateFPToSI(Ops[0], ResultType);
      else
        return BadImmediateError(exp, Result);
    } else {
      if (datatype == neon_datatype_unsigned)
        Result = Builder.CreateUIToFP(Ops[0], ResultType);
      else if (datatype == neon_datatype_signed)
        Result = Builder.CreateSIToFP(Ops[0], ResultType);
      else
        return BadImmediateError(exp, Result);
    }
    break;

  case NEON_BUILTIN_vcvt_n: {
    // Check if the fractional bits argument is between 1 and 32.
    ConstantInt *FBits = dyn_cast<ConstantInt>(Ops[1]);
    if (!FBits)
      return UnexpectedError("%Hinvalid fractional bit count", exp, Result);
    int FBitsVal = FBits->getSExtValue();
    if (FBitsVal < 1 || FBitsVal > 32)
      return UnexpectedError("%Hinvalid fractional bit count", exp, Result);
    if (FLOAT_MODE_P(insn_data[icode].operand[1].mode)) {
      if (datatype == neon_datatype_unsigned)
        intID = Intrinsic::arm_neon_vcvtfp2fxu;
      else if (datatype == neon_datatype_signed)
        intID = Intrinsic::arm_neon_vcvtfp2fxs;
      else
        return BadImmediateError(exp, Result);
    } else {
      if (datatype == neon_datatype_unsigned)
        intID = Intrinsic::arm_neon_vcvtfxu2fp;
      else if (datatype == neon_datatype_signed)
        intID = Intrinsic::arm_neon_vcvtfxs2fp;
      else
        return BadImmediateError(exp, Result);
    }
    intOpTypes[0] = ResultType;
    intOpTypes[1] = Ops[0]->getType();
    intFn = Intrinsic::getDeclaration(TheModule, intID, intOpTypes, 2);
    Result = Builder.CreateCall2(intFn, Ops[0], Ops[1]);
    break;
  }

  case NEON_BUILTIN_vbsl:
    Ops[1] = BitCastToType(Ops[1], Ops[0]->getType());
    Ops[2] = BitCastToType(Ops[2], Ops[0]->getType());
    Result = Builder.CreateOr(Builder.CreateAnd(Ops[1], Ops[0]),
                              Builder.CreateAnd(Ops[2],
                                                Builder.CreateNot(Ops[0])));
    break;

  case NEON_BUILTIN_vtbl1:
  case NEON_BUILTIN_vtbl2:
  case NEON_BUILTIN_vtbl3:
  case NEON_BUILTIN_vtbl4: {
    unsigned TblVecs = 0;
    switch (neon_code) {
    case NEON_BUILTIN_vtbl1:
      intID = Intrinsic::arm_neon_vtbl1;
      TblVecs = 1;
      break;
    case NEON_BUILTIN_vtbl2:
      intID = Intrinsic::arm_neon_vtbl2;
      TblVecs = 2;
      break;
    case NEON_BUILTIN_vtbl3:
      intID = Intrinsic::arm_neon_vtbl3;
      TblVecs = 3;
      break;
    case NEON_BUILTIN_vtbl4:
      intID = Intrinsic::arm_neon_vtbl4;
      TblVecs = 4;
      break;
    default:
      assert(false);
    }
    intFn = Intrinsic::getDeclaration(TheModule, intID);
    std::vector<Value*> Args;
    if (TblVecs == 1) {
      Args.push_back(Ops[0]);
    } else {
      for (unsigned n = 0; n < TblVecs; ++n) {
        Args.push_back(Builder.CreateExtractValue(Ops[0], n));
      }
    }
    Args.push_back(Ops[1]);
    Result = Builder.CreateCall(intFn, Args.begin(), Args.end());
    break;
  }

  case NEON_BUILTIN_vtbx1:
  case NEON_BUILTIN_vtbx2:
  case NEON_BUILTIN_vtbx3:
  case NEON_BUILTIN_vtbx4: {
    unsigned TblVecs = 0;
    switch (neon_code) {
    case NEON_BUILTIN_vtbx1:
      intID = Intrinsic::arm_neon_vtbx1;
      TblVecs = 1;
      break;
    case NEON_BUILTIN_vtbx2:
      intID = Intrinsic::arm_neon_vtbx2;
      TblVecs = 2;
      break;
    case NEON_BUILTIN_vtbx3:
      intID = Intrinsic::arm_neon_vtbx3;
      TblVecs = 3;
      break;
    case NEON_BUILTIN_vtbx4:
      intID = Intrinsic::arm_neon_vtbx4;
      TblVecs = 4;
      break;
    default:
      assert(false);
    }
    intFn = Intrinsic::getDeclaration(TheModule, intID);
    std::vector<Value*> Args;
    Args.push_back(Ops[0]);
    if (TblVecs == 1) {
      Args.push_back(Ops[1]);
    } else {
      for (unsigned n = 0; n < TblVecs; ++n) {
        Args.push_back(Builder.CreateExtractValue(Ops[1], n));
      }
    }
    Args.push_back(Ops[2]);
    Result = Builder.CreateCall(intFn, Args.begin(), Args.end());
    break;
  }

  case NEON_BUILTIN_vtrn: {
    // Translate this to a pair of vector shuffles.
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
    for (unsigned Elt = 0; Elt != 2; ++Elt) {
      std::vector<Constant*> Idxs;
      for (unsigned i = 0; i < NUnits; i += 2) {
        Idxs.push_back(getInt32Const(i + Elt));
        Idxs.push_back(getInt32Const(i + NUnits + Elt));
      }
      Result = Builder.CreateShuffleVector(Ops[0], Ops[1],
                                           ConstantVector::get(Idxs));
      Value *Addr = Builder.CreateConstInBoundsGEP2_32(DestLoc->Ptr, 0, Elt);
      Builder.CreateStore(Result, Addr);
    }
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vzip: {
    // Translate this to a pair of vector shuffles.
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
    unsigned Idx = 0;
    for (unsigned Elt = 0; Elt != 2; ++Elt) {
      std::vector<Constant*> Idxs;
      for (unsigned i = 0; i != NUnits; i += 2) {
        Idxs.push_back(getInt32Const(Idx));
        Idxs.push_back(getInt32Const(Idx + NUnits));
        Idx += 1;
      }
      Result = Builder.CreateShuffleVector(Ops[0], Ops[1],
                                           ConstantVector::get(Idxs));
      Value *Addr = Builder.CreateConstInBoundsGEP2_32(DestLoc->Ptr, 0, Elt);
      Builder.CreateStore(Result, Addr);
    }
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vuzp: {
    // Translate this to a pair of vector shuffles.
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
    for (unsigned Elt = 0; Elt != 2; ++Elt) {
      std::vector<Constant*> Idxs;
      for (unsigned i = 0; i != NUnits; ++i)
        Idxs.push_back(getInt32Const(2 * i + Elt));
      Result = Builder.CreateShuffleVector(Ops[0], Ops[1],
                                           ConstantVector::get(Idxs));
      Value *Addr = Builder.CreateConstInBoundsGEP2_32(DestLoc->Ptr, 0, Elt);
      Builder.CreateStore(Result, Addr);
    }
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vreinterpretv8qi:
  case NEON_BUILTIN_vreinterpretv4hi:
  case NEON_BUILTIN_vreinterpretv2si:
  case NEON_BUILTIN_vreinterpretv2sf:
  case NEON_BUILTIN_vreinterpretv1di:
  case NEON_BUILTIN_vreinterpretv16qi:
  case NEON_BUILTIN_vreinterpretv8hi:
  case NEON_BUILTIN_vreinterpretv4si:
  case NEON_BUILTIN_vreinterpretv4sf:
  case NEON_BUILTIN_vreinterpretv2di:
    Result = Builder.CreateBitCast(Ops[0], ResultType);
    break;

  case NEON_BUILTIN_vld1: {
    intID = Intrinsic::arm_neon_vld1;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &ResultType, 1);
    Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
    unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
    Result = Builder.CreateCall2(intFn, BitCastToType(Ops[0], VPTy),
                                 getInt32Const(Align));
    break;
  }

  case NEON_BUILTIN_vld2:
  case NEON_BUILTIN_vld3:
  case NEON_BUILTIN_vld4: {
    const StructType *STy = dyn_cast<const StructType>(ResultType);
    assert(STy && "expected a struct type");
    const Type *VTy = STy->getElementType(0);
    switch (neon_code) {
    case NEON_BUILTIN_vld2: intID = Intrinsic::arm_neon_vld2; break;
    case NEON_BUILTIN_vld3: intID = Intrinsic::arm_neon_vld3; break;
    case NEON_BUILTIN_vld4: intID = Intrinsic::arm_neon_vld4; break;
    default: assert(false);
    }
    intFn = Intrinsic::getDeclaration(TheModule, intID, &VTy, 1);
    Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
    unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
    Result = Builder.CreateCall2(intFn, BitCastToType(Ops[0], VPTy),
                                 getInt32Const(Align));
    Builder.CreateStore(Result, DestLoc->Ptr);
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vld1_lane: {
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[0].mode);
    if (!isValidLane(Ops[2], NUnits))
      return UnexpectedError("%Hinvalid lane number", exp, Result);
    Value *Elt = Builder.CreateLoad(Ops[0]);
    Result = Builder.CreateInsertElement(Ops[1], Elt, Ops[2]);
    break;
  }

  case NEON_BUILTIN_vld2_lane:
  case NEON_BUILTIN_vld3_lane:
  case NEON_BUILTIN_vld4_lane: {
    const StructType *STy = dyn_cast<const StructType>(ResultType);
    assert(STy && "expected a struct type");
    const VectorType *VTy = dyn_cast<const VectorType>(STy->getElementType(0));
    assert(VTy && "expected a vector type");
    if (!isValidLane(Ops[2], VTy->getNumElements()))
      return UnexpectedError("%Hinvalid lane number", exp, Result);
    switch (neon_code) {
    case NEON_BUILTIN_vld2_lane: intID = Intrinsic::arm_neon_vld2lane; break;
    case NEON_BUILTIN_vld3_lane: intID = Intrinsic::arm_neon_vld3lane; break;
    case NEON_BUILTIN_vld4_lane: intID = Intrinsic::arm_neon_vld4lane; break;
    default: assert(false);
    }
    intOpTypes[0] = VTy;
    intFn = Intrinsic::getDeclaration(TheModule, intID, intOpTypes, 1);
    unsigned NumVecs = 0;
    switch (neon_code) {
    case NEON_BUILTIN_vld2_lane: NumVecs = 2; break;
    case NEON_BUILTIN_vld3_lane: NumVecs = 3; break;
    case NEON_BUILTIN_vld4_lane: NumVecs = 4; break;
    default: assert(false);
    }
    std::vector<Value*> Args;
    Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
    Args.push_back(BitCastToType(Ops[0], VPTy));
    for (unsigned n = 0; n != NumVecs; ++n) {
      Args.push_back(Builder.CreateExtractValue(Ops[1], n));
    }
    Args.push_back(Ops[2]); // lane number
    unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
    Args.push_back(getInt32Const(Align));
    Result = Builder.CreateCall(intFn, Args.begin(), Args.end());
    Builder.CreateStore(Result, DestLoc->Ptr);
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vld1_dup:
    Result = BuildDup(ResultType, Builder.CreateLoad(Ops[0]), Builder);
    break;

  case NEON_BUILTIN_vld2_dup:
  case NEON_BUILTIN_vld3_dup:
  case NEON_BUILTIN_vld4_dup: {
    const StructType *STy = dyn_cast<const StructType>(ResultType);
    assert(STy && "expected a struct type");
    const VectorType *VTy = dyn_cast<const VectorType>(STy->getElementType(0));
    assert(VTy && "expected a vector type");
    intOpTypes[0] = VTy;

    // Handle 64-bit elements as a special-case.  There is no "dup" needed.
    if (VTy->getElementType()->getPrimitiveSizeInBits() == 64) {
      switch (neon_code) {
      case NEON_BUILTIN_vld2_dup: intID = Intrinsic::arm_neon_vld2; break;
      case NEON_BUILTIN_vld3_dup: intID = Intrinsic::arm_neon_vld3; break;
      case NEON_BUILTIN_vld4_dup: intID = Intrinsic::arm_neon_vld4; break;
      default: assert(false);
      }
      intFn = Intrinsic::getDeclaration(TheModule, intID, intOpTypes, 1);
      Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
      unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
      Result = Builder.CreateCall2(intFn, BitCastToType(Ops[0], VPTy),
                                   getInt32Const(Align));
      Builder.CreateStore(Result, DestLoc->Ptr);
      Result = 0;
      break;
    }

    // First use a vldN_lane intrinsic to load into lane 0 of undef vectors.
    switch (neon_code) {
    case NEON_BUILTIN_vld2_dup: intID = Intrinsic::arm_neon_vld2lane; break;
    case NEON_BUILTIN_vld3_dup: intID = Intrinsic::arm_neon_vld3lane; break;
    case NEON_BUILTIN_vld4_dup: intID = Intrinsic::arm_neon_vld4lane; break;
    default: assert(false);
    }
    intFn = Intrinsic::getDeclaration(TheModule, intID, intOpTypes, 1);
    unsigned NumVecs = 0;
    switch (neon_code) {
    case NEON_BUILTIN_vld2_dup: NumVecs = 2; break;
    case NEON_BUILTIN_vld3_dup: NumVecs = 3; break;
    case NEON_BUILTIN_vld4_dup: NumVecs = 4; break;
    default: assert(false);
    }
    std::vector<Value*> Args;
    Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
    Args.push_back(BitCastToType(Ops[0], VPTy));
    for (unsigned n = 0; n != NumVecs; ++n) {
      Args.push_back(UndefValue::get(VTy));
    }
    Args.push_back(getInt32Const(0));
    unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
    Args.push_back(getInt32Const(Align));
    Result = Builder.CreateCall(intFn, Args.begin(), Args.end());

    // Now splat the values in lane 0 to the rest of the elements.
    for (unsigned n = 0; n != NumVecs; ++n) {
      Value *Vec = Builder.CreateExtractValue(Result, n);
      Vec = BuildDupLane(Vec, 0, VTy->getNumElements(), Builder);
      Result = Builder.CreateInsertValue(Result, Vec, n);
    }

    Builder.CreateStore(Result, DestLoc->Ptr);
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vst1: {
    const Type *VTy = Ops[1]->getType();
    intID = Intrinsic::arm_neon_vst1;
    intFn = Intrinsic::getDeclaration(TheModule, intID, &VTy, 1);
    Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
    unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
    Builder.CreateCall3(intFn, BitCastToType(Ops[0], VPTy), Ops[1],
                        getInt32Const(Align));
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vst2:
  case NEON_BUILTIN_vst3:
  case NEON_BUILTIN_vst4: {
    const StructType *STy = dyn_cast<const StructType>(Ops[1]->getType());
    assert(STy && "expected a struct type");
    const Type *VTy = STy->getElementType(0);
    switch (neon_code) {
    case NEON_BUILTIN_vst2: intID = Intrinsic::arm_neon_vst2; break;
    case NEON_BUILTIN_vst3: intID = Intrinsic::arm_neon_vst3; break;
    case NEON_BUILTIN_vst4: intID = Intrinsic::arm_neon_vst4; break;
    default: assert(false);
    }
    intFn = Intrinsic::getDeclaration(TheModule, intID, &VTy, 1);
    unsigned NumVecs = 0;
    switch (neon_code) {
    case NEON_BUILTIN_vst2: NumVecs = 2; break;
    case NEON_BUILTIN_vst3: NumVecs = 3; break;
    case NEON_BUILTIN_vst4: NumVecs = 4; break;
    default: assert(false);
    }
    std::vector<Value*> Args;
    Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
    Args.push_back(BitCastToType(Ops[0], VPTy));
    for (unsigned n = 0; n != NumVecs; ++n) {
      Args.push_back(Builder.CreateExtractValue(Ops[1], n));
    }
    unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
    Args.push_back(getInt32Const(Align));
    Builder.CreateCall(intFn, Args.begin(), Args.end());
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vst1_lane: {
    unsigned NUnits = GET_MODE_NUNITS(insn_data[icode].operand[1].mode);
    if (!isValidLane(Ops[2], NUnits))
      return UnexpectedError("%Hinvalid lane number", exp, Result);
    Builder.CreateStore(Builder.CreateExtractElement(Ops[1], Ops[2]), Ops[0]);
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vst2_lane:
  case NEON_BUILTIN_vst3_lane:
  case NEON_BUILTIN_vst4_lane: {
    const StructType *STy = dyn_cast<const StructType>(Ops[1]->getType());
    assert(STy && "expected a struct type");
    const VectorType *VTy = dyn_cast<const VectorType>(STy->getElementType(0));
    assert(VTy && "expected a vector type");
    if (!isValidLane(Ops[2], VTy->getNumElements()))
      return UnexpectedError("%Hinvalid lane number", exp, Result);
    switch (neon_code) {
    case NEON_BUILTIN_vst2_lane: intID = Intrinsic::arm_neon_vst2lane; break;
    case NEON_BUILTIN_vst3_lane: intID = Intrinsic::arm_neon_vst3lane; break;
    case NEON_BUILTIN_vst4_lane: intID = Intrinsic::arm_neon_vst4lane; break;
    default: assert(false);
    }
    intOpTypes[0] = VTy;
    intFn = Intrinsic::getDeclaration(TheModule, intID, intOpTypes, 1);
    unsigned NumVecs = 0;
    switch (neon_code) {
    case NEON_BUILTIN_vst2_lane: NumVecs = 2; break;
    case NEON_BUILTIN_vst3_lane: NumVecs = 3; break;
    case NEON_BUILTIN_vst4_lane: NumVecs = 4; break;
    default: assert(false);
    }
    std::vector<Value*> Args;
    Type *VPTy = PointerType::getUnqual(Type::getInt8Ty(Context));
    Args.push_back(BitCastToType(Ops[0], VPTy));
    for (unsigned n = 0; n != NumVecs; ++n) {
      Args.push_back(Builder.CreateExtractValue(Ops[1], n));
    }
    Args.push_back(Ops[2]); // lane number
    unsigned Align = getPointerAlignment(TREE_VALUE(TREE_OPERAND(exp, 1)));
    Args.push_back(getInt32Const(Align));
    Builder.CreateCall(intFn, Args.begin(), Args.end());
    Result = 0;
    break;
  }

  case NEON_BUILTIN_vand:
    if (datatype != neon_datatype_signed &&
        datatype != neon_datatype_unsigned)
      return BadImmediateError(exp, Result);
    Result = Builder.CreateAnd(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vorr:
    if (datatype != neon_datatype_signed &&
        datatype != neon_datatype_unsigned)
      return BadImmediateError(exp, Result);
    Result = Builder.CreateOr(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_veor:
    if (datatype != neon_datatype_signed &&
        datatype != neon_datatype_unsigned)
      return BadImmediateError(exp, Result);
    Result = Builder.CreateXor(Ops[0], Ops[1]);
    break;

  case NEON_BUILTIN_vbic:
    if (datatype != neon_datatype_signed &&
        datatype != neon_datatype_unsigned)
      return BadImmediateError(exp, Result);
    Result = Builder.CreateAnd(Ops[0], Builder.CreateNot(Ops[1]));
    break;

  case NEON_BUILTIN_vorn:
    if (datatype != neon_datatype_signed &&
        datatype != neon_datatype_unsigned)
      return BadImmediateError(exp, Result);
    Result = Builder.CreateOr(Ops[0], Builder.CreateNot(Ops[1]));
    break;
  }

  return true;
}

// "Fundamental Data Types" according to the AAPCS spec.  These are used
// to check that a given aggregate meets the criteria for a "homogeneous
// aggregate."
enum arm_fdts {
  ARM_FDT_INVALID,

  ARM_FDT_HALF_FLOAT,
  ARM_FDT_FLOAT,
  ARM_FDT_DOUBLE,

  ARM_FDT_VECTOR_64,
  ARM_FDT_VECTOR_128,

  ARM_FDT_MAX
};

// Classify type according to the number of fundamental data types contained
// among its members.  Returns true if type is a homogeneous aggregate.
static bool
vfp_arg_homogeneous_aggregate_p(enum machine_mode mode, tree type,
                                int *fdt_counts)
{
  bool result = false;
  HOST_WIDE_INT bytes =
    (mode == BLKmode) ? int_size_in_bytes (type) : (int) GET_MODE_SIZE (mode);

  if (type && AGGREGATE_TYPE_P (type))
  {
    int i;
    int cnt = 0;
    tree field;

    // Zero sized arrays or structures are not homogeneous aggregates.
    if (!bytes)
      return 0;

    // Classify each field of records.
    switch (TREE_CODE (type))
    {
      case RECORD_TYPE:
      // For classes first merge in the field of the subclasses.
      if (TYPE_BINFO (type)) {
        tree binfo, base_binfo;
        int basenum;

        for (binfo = TYPE_BINFO (type), basenum = 0;
             BINFO_BASE_ITERATE (binfo, basenum, base_binfo); basenum++) {
          int offset = tree_low_cst (BINFO_OFFSET (base_binfo), 0) * 4;
          tree type = BINFO_TYPE (base_binfo);

          result = vfp_arg_homogeneous_aggregate_p(TYPE_MODE (type), type,
                                                   fdt_counts);
          if (!result)
            return false;
        }
      }
      // And now merge the fields of structure.
      for (field = TYPE_FIELDS (type); field; field = TREE_CHAIN (field)) {
        if (TREE_CODE (field) == FIELD_DECL) {
          if (TREE_TYPE (field) == error_mark_node)
            continue;

          result = vfp_arg_homogeneous_aggregate_p(TYPE_MODE(TREE_TYPE(field)),
                                                   TREE_TYPE(field),
                                                   fdt_counts);
          if (!result)
            return false;
        }
      }
      break;

      case ARRAY_TYPE:
        // Arrays are handled as small records.
        {
          int array_fdt_counts[ARM_FDT_MAX] = { 0 };

          result = vfp_arg_homogeneous_aggregate_p(TYPE_MODE(TREE_TYPE(type)),
                                                   TREE_TYPE(type),
                                                   array_fdt_counts);

          cnt = bytes / int_size_in_bytes(TREE_TYPE(type));
          for (i = 0; i < ARM_FDT_MAX; ++i)
            fdt_counts[i] += array_fdt_counts[i] * cnt;

          if (!result)
            return false;
        }
      break;

      case UNION_TYPE:
      case QUAL_UNION_TYPE:
        {
          // Unions are similar to RECORD_TYPE.
          int union_fdt_counts[ARM_FDT_MAX] = { 0 };

          // Unions are not derived.
          gcc_assert (!TYPE_BINFO (type)
                      || !BINFO_N_BASE_BINFOS (TYPE_BINFO (type)));
          for (field = TYPE_FIELDS (type); field; field = TREE_CHAIN (field)) {
            int union_field_fdt_counts[ARM_FDT_MAX] = { 0 };

            if (TREE_CODE (field) == FIELD_DECL) {
              if (TREE_TYPE (field) == error_mark_node)
                continue;

              result = vfp_arg_homogeneous_aggregate_p(
                          TYPE_MODE(TREE_TYPE(field)),
                          TREE_TYPE(field),
                          union_field_fdt_counts);
              if (!result)
                return false;

              // track largest union field
              for (i = 0; i < ARM_FDT_MAX; ++i) {
                if (union_field_fdt_counts[i] > 4)  // bail early if we can
                  return false;

                union_fdt_counts[i] = MAX(union_fdt_counts[i],
                                          union_field_fdt_counts[i]);
                union_field_fdt_counts[i] = 0;  // clear it out for next iter
              }
            }
          }

          // check for only one type across all union fields
          cnt = 0;
          for (i = 0; i < ARM_FDT_MAX; ++i) {
            if (union_fdt_counts[i])
              ++cnt;

            if (cnt > 1)
              return false;

            fdt_counts[i] += union_fdt_counts[i];
          }
        }
      break;

      default:
      assert(0 && "What type is this?");
    }

    // Walk through fdt_counts.  This is a homogeneous aggregate if
    // only one FDT is used.
    cnt = 0;
    for (i = 0; i < ARM_FDT_MAX; ++i) {
      if (fdt_counts[i]) {
        // Make sure that one FDT is 4 or less elements in size.
        if (fdt_counts[i] > 4)
          return false;
        ++cnt;
      }

      if (cnt > 1)
        return false;
    }

    if (cnt == 0)
      return false;

    return true;
  }

  if (type)
  {
    int idx = 0;
    int cnt = 0;

    switch (TREE_CODE(type))
    {
    case REAL_TYPE:
      idx = (TYPE_PRECISION(type) == 32) ?
              ARM_FDT_FLOAT :
              ((TYPE_PRECISION(type) == 64) ?
                ARM_FDT_DOUBLE :
                ARM_FDT_INVALID);
      cnt = 1;
      break;

    case COMPLEX_TYPE:
      {
        tree subtype = TREE_TYPE(type);
        idx = (TYPE_PRECISION(subtype) == 32) ?
                ARM_FDT_FLOAT :
                ((TYPE_PRECISION(subtype) == 64) ?
                  ARM_FDT_DOUBLE :
                  ARM_FDT_INVALID);
        cnt = 2;
      }
      break;

    case VECTOR_TYPE:
      idx = (bytes == 8) ?
              ARM_FDT_VECTOR_64 :
              (bytes == 16) ?
                ARM_FDT_VECTOR_128 :
                ARM_FDT_INVALID;
      cnt = 1;
      break;

    case INTEGER_TYPE:
    case POINTER_TYPE:
    case ENUMERAL_TYPE:
    case BOOLEAN_TYPE:
    case REFERENCE_TYPE:
    case FUNCTION_TYPE:
    case METHOD_TYPE:
    default:
      return false;     // All of these disqualify.
    }

    fdt_counts[idx] += cnt;
    return true;
  }
  else
    assert(0 && "what type was this?");

  return false;
}

// Walk over an LLVM Type that we know is a homogeneous aggregate and
// push the proper LLVM Types that represent the register types to pass
// that struct member in.
static void push_elts(const Type *Ty, std::vector<const Type*> &Elts)
{
  for (Type::subtype_iterator I = Ty->subtype_begin(), E = Ty->subtype_end();
       I != E; ++I) {
    const Type *STy = I->get();
    if (const VectorType *VTy = dyn_cast<VectorType>(STy)) {
      switch (VTy->getBitWidth())
      {
        case 64:  // v2f32
          Elts.push_back(VectorType::get(Type::getFloatTy(Context), 2));
          break;
        case 128: // v2f64
          Elts.push_back(VectorType::get(Type::getDoubleTy(Context), 2));
          break;
        default:
          assert (0 && "invalid vector type");
      }
    } else if (const ArrayType *ATy = dyn_cast<ArrayType>(STy)) {
      const Type *ETy = ATy->getElementType();

      for (uint64_t i = ATy->getNumElements(); i > 0; --i)
        Elts.push_back(ETy);
    } else if (STy->getNumContainedTypes())
      push_elts(STy, Elts);
    else
      Elts.push_back(STy);
  }
}

static unsigned count_num_words(std::vector<const Type*> &ScalarElts) {
  unsigned NumWords = 0;
  for (unsigned i = 0, e = ScalarElts.size(); i != e; ++i) {
    const Type *Ty = ScalarElts[i];
    if (Ty->isPointerTy()) {
      NumWords++;
    } else if (Ty->isIntegerTy()) {
      const unsigned TypeSize = Ty->getPrimitiveSizeInBits();
      const unsigned NumWordsForType = (TypeSize + 31) / 32;

      NumWords += NumWordsForType;
    } else {
      assert (0 && "Unexpected type.");
    }
  }
  return NumWords;
}

// This function is used only on AAPCS. The difference from the generic
// handling of arguments is that arguments larger than 32 bits are split
// and padding arguments are added as necessary for alignment. This makes
// the IL a bit more explicit about how arguments are handled.
extern bool
llvm_arm_try_pass_aggregate_custom(tree type,
                                   std::vector<const Type*>& ScalarElts,
				   CallingConv::ID& CC,
				   struct DefaultABIClient* C) {
  if (CC != CallingConv::ARM_AAPCS && CC != CallingConv::C)
    return false;

  if (CC == CallingConv::C && !TARGET_AAPCS_BASED)
    return false;

  if (TARGET_HARD_FLOAT_ABI)
    return false;
  const Type *Ty = ConvertType(type);
  if (Ty->isPointerTy())
    return false;

  const unsigned Size = TREE_INT_CST_LOW(TYPE_SIZE(type))/8;
  const unsigned Alignment = TYPE_ALIGN(type)/8;
  const unsigned NumWords = count_num_words(ScalarElts);
  const bool AddPad = Alignment >= 8 && (NumWords % 2);

  // First, build a type that will be bitcast to the original one and
  // from where elements will be extracted.
  std::vector<const Type*> Elts;
  const Type* Int32Ty = Type::getInt32Ty(getGlobalContext());
  const unsigned NumRegularArgs = Size / 4;
  for (unsigned i = 0; i < NumRegularArgs; ++i) {
    Elts.push_back(Int32Ty);
  }
  const unsigned RestSize = Size % 4;
  const llvm::Type *RestType = NULL;
  if (RestSize> 2) {
    RestType = Type::getInt32Ty(getGlobalContext());
  } else if (RestSize > 1) {
    RestType = Type::getInt16Ty(getGlobalContext());
  } else if (RestSize > 0) {
    RestType = Type::getInt8Ty(getGlobalContext());
  }
  if (RestType)
    Elts.push_back(RestType);
  const StructType *STy = StructType::get(getGlobalContext(), Elts, false);

  if (AddPad) {
    ScalarElts.push_back(Int32Ty);
    C->HandlePad(Int32Ty);
  }

  for (unsigned i = 0; i < NumRegularArgs; ++i) {
    C->EnterField(i, STy);
    C->HandleScalarArgument(Int32Ty, 0);
    ScalarElts.push_back(Int32Ty);
    C->ExitField();
  }
  if (RestType) {
    C->EnterField(NumRegularArgs, STy);
    C->HandleScalarArgument(RestType, 0, RestSize);
    ScalarElts.push_back(RestType);
    C->ExitField();
  }
  return true;
}

// Target hook for llvm-abi.h. It returns true if an aggregate of the
// specified type should be passed in a number of registers of mixed types.
// It also returns a vector of types that correspond to the registers used
// for parameter passing. This only applies to AAPCS-VFP "homogeneous
// aggregates" as specified in 4.3.5 of the AAPCS spec.
bool
llvm_arm_should_pass_aggregate_in_mixed_regs(tree TreeType, const Type *Ty,
                                             CallingConv::ID &CC,
                                             std::vector<const Type*> &Elts) {
  if (!llvm_arm_should_pass_or_return_aggregate_in_regs(TreeType, CC))
    return false;

  // Walk Ty and push LLVM types corresponding to register types onto
  // Elts.
  push_elts(Ty, Elts);

  return true;
}

static bool alloc_next_spr(bool *SPRs)
{
  for (int i = 0; i < 16; ++i)
    if (!SPRs[i]) {
      SPRs[i] = true;
      return true;
    }
  return false;
}

static bool alloc_next_dpr(bool *SPRs)
{
  for (int i = 0; i < 16; i += 2)
    if (!SPRs[i]) {
      SPRs[i] = SPRs[i+1] = true;
      return true;
    }
  return false;
}

static bool alloc_next_qpr(bool *SPRs) {
  for (int i = 0; i < 16; i += 4)
    if (!SPRs[i]) {
      SPRs[i] = SPRs[i+1] = SPRs[i+2] = SPRs[i+3] = true;
      return true;
    }
  return false;
}

// count_num_registers_uses - Simulate argument passing reg allocation in SPRs.
// Caller is expected to zero out SPRs.  Returns true if all of ScalarElts fit
// in registers.
static bool count_num_registers_uses(std::vector<const Type*> &ScalarElts,
                                     bool *SPRs) {
  for (unsigned i = 0, e = ScalarElts.size(); i != e; ++i) {
    const Type *Ty = ScalarElts[i];
    if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
      switch (VTy->getBitWidth())
      {
        case 64:
          if (!alloc_next_dpr(SPRs))
            return false;
          break;
        case 128:
          if (!alloc_next_qpr(SPRs))
            return false;
          break;
        default:
          assert(0);
      }
    } else if (Ty->isIntegerTy() || Ty->isPointerTy() ||
               Ty==Type::getVoidTy(Context)) {
      ;
    } else {
      // Floating point scalar argument.
      assert(Ty->isFloatingPointTy() && Ty->isPrimitiveType() &&
             "Expecting a floating point primitive type!");
      switch (Ty->getTypeID())
      {
        case Type::FloatTyID:
          if (!alloc_next_spr(SPRs))
            return false;
          break;
        case Type::DoubleTyID:
          if (!alloc_next_spr(SPRs))
            return false;
          break;
        default:
          assert(0);
      }
    }
  }
  return true;
}

// Target hook for llvm-abi.h. This is called when an aggregate is being passed
// in registers. If there are only enough available parameter registers to pass
// part of the aggregate, return true. That means the aggregate should instead
// be passed in memory.
bool
llvm_arm_aggregate_partially_passed_in_regs(std::vector<const Type*> &Elts,
                                            std::vector<const Type*> &ScalarElts,
                                            CallingConv::ID &CC) {
  // Homogeneous aggregates are an AAPCS-VFP feature.
  if ((CC != CallingConv::ARM_AAPCS_VFP) ||
      !(TARGET_AAPCS_BASED && TARGET_VFP && TARGET_HARD_FLOAT_ABI))
    return true;

  bool SPRs[16] = { 0 };                    // represents S0-S16

  // Figure out which SPRs are available.
  if (!count_num_registers_uses(ScalarElts, SPRs))
    return true;

  if (!count_num_registers_uses(Elts, SPRs))
    return true;

  return false;                            // it all fit in registers!
}

// Return LLVM Type if TYPE can be returned as an aggregate,
// otherwise return NULL.
const Type *llvm_arm_aggr_type_for_struct_return(tree TreeType,
                                                 CallingConv::ID &CC) {
  if (!llvm_arm_should_pass_or_return_aggregate_in_regs(TreeType, CC))
    return NULL;

  // Walk Ty and push LLVM types corresponding to register types onto
  // Elts.
  std::vector<const Type*> Elts;
  const Type *Ty = ConvertType(TreeType);
  push_elts(Ty, Elts);

  return StructType::get(Context, Elts, false);
}

// llvm_arm_extract_mrv_array_element - Helper function that helps extract
// an array element from multiple return value.
//
// Here, SRC is returning multiple values. DEST's DESTFIELDNO field is an array.
// Extract SRCFIELDNO's ELEMENO value and store it in DEST's FIELDNO field's
// ELEMENTNO.
//
static void llvm_arm_extract_mrv_array_element(Value *Src, Value *Dest,
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

// llvm_arm_extract_multiple_return_value - Extract multiple values returned
// by SRC and store them in DEST. It is expected that SRC and
// DEST types are StructType, but they may not match.
void llvm_arm_extract_multiple_return_value(Value *Src, Value *Dest,
                                            bool isVolatile,
                                            LLVMBuilder &Builder) {
  const StructType *STy = cast<StructType>(Src->getType());
  unsigned NumElements = STy->getNumElements();

  const PointerType *PTy = cast<PointerType>(Dest->getType());
  const StructType *DestTy = cast<StructType>(PTy->getElementType());

  unsigned SNO = 0;
  unsigned DNO = 0;

  while (SNO < NumElements) {

    const Type *DestElemType = DestTy->getElementType(DNO);

    // Directly access first class values.
    if (DestElemType->isSingleValueType()) {
      Value *GEP = Builder.CreateStructGEP(Dest, DNO, "mrv_gep");
      Value *EVI = Builder.CreateExtractValue(Src, SNO, "mrv_gr");
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
      }
      while (i < Size) {
        llvm_arm_extract_mrv_array_element(Src, Dest, SNO, i++,
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

// Target hook for llvm-abi.h for LLVM_SHOULD_NOT_USE_SHADOW_RETURN and is
// also a utility function used for other target hooks in this file. Returns
// true if the aggregate should be passed or returned in registers.
bool llvm_arm_should_pass_or_return_aggregate_in_regs(tree TreeType,
                                                      CallingConv::ID &CC) {
  // Homogeneous aggregates are an AAPCS-VFP feature.
  if ((CC != CallingConv::ARM_AAPCS_VFP) ||
      !(TARGET_AAPCS_BASED && TARGET_VFP && TARGET_HARD_FLOAT_ABI))
    return false;

  // Alas, we can't use LLVM Types to figure this out because we need to
  // examine unions closely.  We'll have to walk the GCC TreeType.
  int fdt_counts[ARM_FDT_MAX] = { 0 };
  bool result = false;
  result = vfp_arg_homogeneous_aggregate_p(TYPE_MODE(TreeType), TreeType,
                                           fdt_counts);
  return result && !TREE_ADDRESSABLE(TreeType);
}

/* LLVM LOCAL end (ENTIRE FILE!)  */
