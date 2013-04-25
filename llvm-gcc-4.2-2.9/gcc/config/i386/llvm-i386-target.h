/* LLVM LOCAL begin (ENTIRE FILE!)  */
#ifdef ENABLE_LLVM
/* Some target-specific hooks for gcc->llvm conversion
Copyright (C) 2007 Free Software Foundation, Inc.
Contributed by Anton Korobeynikov (asl@math.spbu.ru)

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

#ifndef LLVM_I386_TARGET_H
#define LLVM_I386_TARGET_H

/* Detect MMX register variables.  We use this to generate an x86mmx LLVM type
   for these rather than <2 x i32>.  This is an utter hack, but
   there is no counterpart to x86mmx in the gcc type system, and trying to
   add it is a can of worms. */
#define LLVM_IS_DECL_MMX_REGISTER(decl)         \
  (TARGET_MMX &&                                                        \
   TREE_CODE(decl) == VAR_DECL && DECL_REGISTER(decl) && DECL_NAME(decl) && \
   strncmp("%mm", IDENTIFIER_POINTER(DECL_NAME(decl)), 3)==0)

/* Similarly, parameters of MMX vector type <8 x i8>, <4 x i16>, <2 x i32>
   must be changed to x86mmx at the last minute.  <2 x f32> is unsupported;
   it is filtered out here to avoid crashes in the gcc testsuite. */
#define LLVM_ADJUST_MMX_PARAMETER_TYPE(LLVMTy)  \
  ((TARGET_MMX &&                                                      \
    LLVMTy->isVectorTy() && LLVMTy->getPrimitiveSizeInBits()==64 &&    \
    cast<VectorType>(LLVMTy)->getElementType()->isIntegerTy() &&       \
    LLVMTy->getScalarSizeInBits() !=64) ?                              \
   Type::getX86_MMXTy(Context) : LLVMTy)

/* For parameters to an asm, check the constraint and use X86_mmx if an MMX
   register is called for.  "y" means an MMX register. */
#define LLVM_ADJUST_MMX_INLINE_PARAMETER_TYPE(Constraint, LLVMTy)      \
  ((TARGET_MMX &&                                                      \
    (StringRef(Constraint) == "y" || StringRef(Constraint) == "&y")) ? \
   Type::getX86_MMXTy(Context) : LLVMTy)

/* LLVM specific stuff for supporting calling convention output */
#define TARGET_ADJUST_LLVM_CC(CC, type)                         \
  {                                                             \
    tree type_attributes = TYPE_ATTRIBUTES (type);              \
    if (lookup_attribute ("stdcall", type_attributes)) {        \
      CC = CallingConv::X86_StdCall;                            \
    } else if (lookup_attribute("fastcall", type_attributes)) { \
      CC = CallingConv::X86_FastCall;                           \
    }                                                           \
  }

#define TARGET_ADJUST_LLVM_RETATTR(Rattributes, type)           \
  {                                                             \
    tree type_attributes = TYPE_ATTRIBUTES (type);              \
    if (!TARGET_64BIT && (TARGET_SSEREGPARM ||                  \
               lookup_attribute("sseregparm", type_attributes)))\
      RAttributes |= Attribute::InReg;                          \
  }

/* LLVM specific stuff for converting gcc's `regparm` attribute to LLVM's
   `inreg` parameter attribute */
#define LLVM_TARGET_ENABLE_REGPARM

extern int ix86_regparm;

#define LLVM_TARGET_INIT_REGPARM(local_regparm, local_fp_regparm, type) \
  {                                                             \
    tree attr;                                                  \
    local_regparm = ix86_regparm;                               \
    local_fp_regparm = TARGET_SSEREGPARM ? 3 : 0;               \
    attr = lookup_attribute ("regparm",                         \
                              TYPE_ATTRIBUTES (type));          \
    if (attr) {                                                 \
      local_regparm = TREE_INT_CST_LOW (TREE_VALUE              \
                                        (TREE_VALUE (attr)));   \
    }                                                           \
    attr = lookup_attribute("sseregparm",                       \
                              TYPE_ATTRIBUTES (type));          \
    if (attr)                                                   \
      local_fp_regparm = 3;                                     \
  }

#define LLVM_ADJUST_REGPARM_ATTRIBUTE(PAttribute, Type, Size,   \
                                      local_regparm,            \
                                      local_fp_regparm)         \
  {                                                             \
    if (!TARGET_64BIT) {                                        \
      if (TREE_CODE(Type) == REAL_TYPE &&                       \
          (TYPE_PRECISION(Type)==32 ||                          \
           TYPE_PRECISION(Type)==64)) {                         \
          local_fp_regparm -= 1;                                \
          if (local_fp_regparm >= 0)                            \
            PAttribute |= Attribute::InReg;                     \
          else                                                  \
            local_fp_regparm = 0;                               \
      } else if (INTEGRAL_TYPE_P(Type) ||                       \
                 POINTER_TYPE_P(Type)) {                        \
          int words =                                           \
                  (Size + BITS_PER_WORD - 1) / BITS_PER_WORD;   \
          local_regparm -= words;                               \
          if (local_regparm>=0)                                 \
            PAttribute |= Attribute::InReg;                     \
          else                                                  \
            local_regparm = 0;                                  \
      }                                                         \
    }                                                           \
  }

#define LLVM_SET_RED_ZONE_FLAG(disable_red_zone)                \
  if (TARGET_64BIT && TARGET_NO_RED_ZONE)                       \
    disable_red_zone = 1;

#ifdef LLVM_ABI_H

/* On x86-32 objects containing SSE vectors are 16 byte aligned, everything
   else 4.  On x86-64 vectors are 8-byte aligned, everything else can
   be figured out by the back end. */
extern "C" bool contains_128bit_aligned_vector_p(tree);
#define LLVM_BYVAL_ALIGNMENT(T) \
  (TARGET_64BIT ? (TREE_CODE(T)==VECTOR_TYPE ? 8 : 0) : \
   TARGET_SSE && contains_128bit_aligned_vector_p(T) ? 16 : 4)

extern tree llvm_x86_should_return_selt_struct_as_scalar(tree);

/* Structs containing a single data field plus zero-length fields are
   considered as if they were the type of the data field.  On x86-64,
   if the element type is an MMX vector, return it as double (which will
   get it into XMM0). */

#define LLVM_SHOULD_RETURN_SELT_STRUCT_AS_SCALAR(X) \
  llvm_x86_should_return_selt_struct_as_scalar((X))

extern bool llvm_x86_should_pass_aggregate_in_integer_regs(tree, 
                                                          unsigned*, bool*);

/* LLVM_SHOULD_PASS_AGGREGATE_IN_INTEGER_REGS - Return true if this aggregate
   value should be passed in integer registers.  This differs from the usual
   handling in that x86-64 passes 128-bit structs and unions which only
   contain data in the first 64 bits, as 64-bit objects.  (These can be
   created by abusing __attribute__((aligned)).  */
#define LLVM_SHOULD_PASS_AGGREGATE_IN_INTEGER_REGS(X, Y, Z)             \
  llvm_x86_should_pass_aggregate_in_integer_regs((X), (Y), (Z))

extern const Type *llvm_x86_scalar_type_for_struct_return(tree type, 
                                                          unsigned *Offset);

/* LLVM_SCALAR_TYPE_FOR_STRUCT_RETURN - Return LLVM Type if X can be 
   returned as a scalar, otherwise return NULL. */
#define LLVM_SCALAR_TYPE_FOR_STRUCT_RETURN(X, Y) \
  llvm_x86_scalar_type_for_struct_return((X), (Y))

extern const Type *llvm_x86_aggr_type_for_struct_return(tree type);

/* LLVM_AGGR_TYPE_FOR_STRUCT_RETURN - Return LLVM Type if X can be 
   returned as an aggregate, otherwise return NULL. */
#define LLVM_AGGR_TYPE_FOR_STRUCT_RETURN(X, CC) \
  llvm_x86_aggr_type_for_struct_return(X)

extern void llvm_x86_extract_multiple_return_value(Value *Src, Value *Dest,
                                                   bool isVolatile,
                                                   LLVMBuilder &B);

/* LLVM_EXTRACT_MULTIPLE_RETURN_VALUE - Extract multiple return value from
   SRC and assign it to DEST. */
#define LLVM_EXTRACT_MULTIPLE_RETURN_VALUE(Src,Dest,V,B)       \
  llvm_x86_extract_multiple_return_value((Src),(Dest),(V),(B))

extern bool llvm_x86_should_pass_vector_using_byval_attr(tree);

/* On x86-64, vectors which are not MMX nor SSE should be passed byval. */
#define LLVM_SHOULD_PASS_VECTOR_USING_BYVAL_ATTR(X)      \
  llvm_x86_should_pass_vector_using_byval_attr((X))

extern bool llvm_x86_should_pass_vector_in_integer_regs(tree);

/* On x86-32, vectors which are not MMX nor SSE should be passed as integers. */
#define LLVM_SHOULD_PASS_VECTOR_IN_INTEGER_REGS(X)      \
  llvm_x86_should_pass_vector_in_integer_regs((X))

extern tree llvm_x86_should_return_vector_as_scalar(tree, bool);

/* The MMX vector v1i64 is returned in EAX and EDX on Darwin.  Communicate
    this by returning i64 here.  Likewise, (generic) vectors such as v2i16
    are returned in EAX.  
    On Darwin x86-64, MMX vectors are returned in XMM0.  Communicate this by
    returning f64.  */
#define LLVM_SHOULD_RETURN_VECTOR_AS_SCALAR(X,isBuiltin)\
  llvm_x86_should_return_vector_as_scalar((X), (isBuiltin))

extern bool llvm_x86_should_return_vector_as_shadow(tree, bool);

/* MMX vectors v2i32, v4i16, v8i8, v2f32 are returned using sret on Darwin
   32-bit.  Vectors bigger than 128 are returned using sret.  */
#define LLVM_SHOULD_RETURN_VECTOR_AS_SHADOW(X,isBuiltin)\
  llvm_x86_should_return_vector_as_shadow((X),(isBuiltin))

extern bool
llvm_x86_should_not_return_complex_in_memory(tree type);

/* LLVM_SHOULD_NOT_RETURN_COMPLEX_IN_MEMORY - A hook to allow
   special _Complex handling. Return true if X should be returned using
   multiple value return instruction.  */
#define LLVM_SHOULD_NOT_RETURN_COMPLEX_IN_MEMORY(X) \
  llvm_x86_should_not_return_complex_in_memory((X))

extern bool
llvm_x86_should_pass_aggregate_as_fca(tree type, const Type *);

/* LLVM_SHOULD_PASS_AGGREGATE_AS_FCA - Return true if an aggregate of the
   specified type should be passed as a first-class aggregate. */
#ifndef LLVM_SHOULD_PASS_AGGREGATE_AS_FCA
#define LLVM_SHOULD_PASS_AGGREGATE_AS_FCA(X, TY) \
  llvm_x86_should_pass_aggregate_as_fca(X, TY)
#endif

extern bool llvm_x86_should_pass_aggregate_in_memory(tree, const Type *);

#define LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR(X, TY)      \
  llvm_x86_should_pass_aggregate_in_memory(X, TY)


extern bool
llvm_x86_64_should_pass_aggregate_in_mixed_regs(tree, const Type *Ty,
                                                std::vector<const Type*>&);
extern bool
llvm_x86_32_should_pass_aggregate_in_mixed_regs(tree, const Type *Ty,
                                                std::vector<const Type*>&);

#define LLVM_SHOULD_PASS_AGGREGATE_IN_MIXED_REGS(T, TY, CC, E)       \
  (TARGET_64BIT ?                                                    \
   llvm_x86_64_should_pass_aggregate_in_mixed_regs((T), (TY), (E)) : \
   llvm_x86_32_should_pass_aggregate_in_mixed_regs((T), (TY), (E)))

extern
bool llvm_x86_64_aggregate_partially_passed_in_regs(std::vector<const Type*>&,
                                                    std::vector<const Type*>&);

#define LLVM_AGGREGATE_PARTIALLY_PASSED_IN_REGS(E, SE, CC)       \
  (TARGET_64BIT ?                                                     \
   llvm_x86_64_aggregate_partially_passed_in_regs((E), (SE)) : \
   false)

#endif /* LLVM_ABI_H */
#endif /* ENABLE_LLVM */

#endif /* LLVM_I386_TARGET_H */
/* LLVM LOCAL end (ENTIRE FILE!)  */
