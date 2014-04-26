/* LLVM LOCAL begin (ENTIRE FILE!)  */
#ifdef ENABLE_LLVM
/* Some target-specific hooks for gcc->llvm conversion
Copyright (C) 2009 Free Software Foundation, Inc.
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

/* LLVM specific code to select the calling conventions. The AAPCS
   specification says that varargs functions must use the base standard
   instead of the VFP hard float variant. We check for that with
   (isVoid || hasArgList). */

/* from  TARGET_AAPCS_BASED */
#define DEFAULT_TARGET_AAPCS_BASED \
  (ARM_DEFAULT_ABI != ARM_ABI_APCS && ARM_DEFAULT_ABI != ARM_ABI_ATPCS)

#define TARGET_ADJUST_LLVM_CC(CC, type)                       \
  {                                                           \
    if (TARGET_AAPCS_BASED) {                                 \
      if (TARGET_VFP && TARGET_HARD_FLOAT_ABI &&              \
             ((TYPE_ARG_TYPES(type) == 0) ||                  \
              (TREE_VALUE(tree_last(TYPE_ARG_TYPES(type))) == \
               void_type_node)))                              \
        CC = CallingConv::ARM_AAPCS_VFP;                      \
      if (!DEFAULT_TARGET_AAPCS_BASED)                        \
        CC = CallingConv::ARM_AAPCS;                          \
    } else if (DEFAULT_TARGET_AAPCS_BASED) {                  \
        CC = CallingConv::ARM_APCS;                           \
    }                                                         \
  }

#ifdef LLVM_ABI_H

extern bool
llvm_arm_should_pass_aggregate_in_mixed_regs(tree, const Type *Ty,
                                             CallingConv::ID&,
                                             std::vector<const Type*>&);

#define LLVM_SHOULD_PASS_AGGREGATE_IN_MIXED_REGS(T, TY, CC, E)    \
   llvm_arm_should_pass_aggregate_in_mixed_regs((T), (TY), (CC), (E))

struct DefaultABIClient;
extern bool
llvm_arm_try_pass_aggregate_custom(tree, std::vector<const Type*>&,
				   CallingConv::ID&,
				   struct DefaultABIClient*);

#define LLVM_TRY_PASS_AGGREGATE_CUSTOM(T, E, CC, C)	\
  llvm_arm_try_pass_aggregate_custom((T), (E), (CC), (C))

extern
bool llvm_arm_aggregate_partially_passed_in_regs(std::vector<const Type*>&,
                                                 std::vector<const Type*>&,
                                                 CallingConv::ID&);

#define LLVM_AGGREGATE_PARTIALLY_PASSED_IN_REGS(E, SE, CC)   \
   llvm_arm_aggregate_partially_passed_in_regs((E), (SE), (CC))

extern const Type *llvm_arm_aggr_type_for_struct_return(tree type,
                                                        CallingConv::ID &CC);

/* LLVM_AGGR_TYPE_FOR_STRUCT_RETURN - Return LLVM Type if X can be 
  returned as an aggregate, otherwise return NULL. */
#define LLVM_AGGR_TYPE_FOR_STRUCT_RETURN(X, CC) \
    llvm_arm_aggr_type_for_struct_return((X), (CC))

extern void llvm_arm_extract_multiple_return_value(Value *Src, Value *Dest,
                                                   bool isVolatile,
                                                   LLVMBuilder &B);

/* LLVM_EXTRACT_MULTIPLE_RETURN_VALUE - Extract multiple return value from
  SRC and assign it to DEST. */
#define LLVM_EXTRACT_MULTIPLE_RETURN_VALUE(Src,Dest,V,B)       \
    llvm_arm_extract_multiple_return_value((Src),(Dest),(V),(B))

extern
bool llvm_arm_should_pass_or_return_aggregate_in_regs(tree TreeType,
                                                      CallingConv::ID &CC);

/* LLVM_SHOULD_NOT_USE_SHADOW_RETURN = Return true is the given type should
  not be returned via a shadow parameter with the given calling conventions. */
#define LLVM_SHOULD_NOT_USE_SHADOW_RETURN(X, CC) \
    llvm_arm_should_pass_or_return_aggregate_in_regs((X), (CC))

/* Vectors bigger than 128 are returned using sret.  */
#define LLVM_SHOULD_RETURN_VECTOR_AS_SHADOW(X, isBuiltin) \
  (TREE_INT_CST_LOW(TYPE_SIZE(X)) > 128)

#endif /* LLVM_ABI_H */
#endif /* ENABLE_LLVM */
/* LLVM LOCAL end (ENTIRE FILE!)  */
