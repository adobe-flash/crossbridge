/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* Some target-specific hooks for gcc->llvm conversion
Copyright (C) 2008 Free Software Foundation, Inc.
Contributed by Bruno Cardoso Lopes (bruno.cardoso@gmail.com)

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

#ifdef LLVM_ABI_H

extern bool llvm_mips_should_pass_aggregate_in_memory(tree, const Type *);

/* LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR - Return true if this aggregate
   value should be passed by value, i.e. passing its address with the byval
   attribute bit set. The default is false.  */
#define LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR(X, TY) \
  llvm_mips_should_pass_aggregate_in_memory(X, TY)

extern bool
llvm_mips_should_not_return_complex_in_memory(tree type);

/* LLVM_SHOULD_NOT_RETURN_COMPLEX_IN_MEMORY - A hook to allow
   special _Complex handling. Return true if X should be returned using
   multiple value return instruction.  */
#define LLVM_SHOULD_NOT_RETURN_COMPLEX_IN_MEMORY(X) \
  llvm_mips_should_not_return_complex_in_memory((X))

extern const Type *llvm_mips_aggr_type_for_struct_return(tree type);

/* LLVM_AGGR_TYPE_FOR_STRUCT_RETURN - Return LLVM Type if X can be 
   returned as an aggregate, otherwise return NULL. */
#define LLVM_AGGR_TYPE_FOR_STRUCT_RETURN(X, CC) \
  llvm_mips_aggr_type_for_struct_return(X)

#endif /* LLVM_ABI_H */

/* LLVM LOCAL end (ENTIRE FILE!)  */
