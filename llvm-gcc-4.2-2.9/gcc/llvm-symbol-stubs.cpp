/* llvm-symbol-stubs.cpp: define dummy symbols the RTL backend would provide
   Contributed by Chris Lattner (sabre@nondot.org)

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

#include <stdio.h>
#include <stdlib.h>

/* When LLVM is built in for BUILD_LLVM_INTO_A_DYLIB mode, we elide various
   chunks of the RTL backend from linking into CC1.  In cases where it is
   difficult (or wouldn't make sense) to ifdef out all uses of a symbol, it can
   be added to this file.
*/

#define SYMBOL(X) \
  extern "C" void X() { \
    printf("Error: llvm-gcc should not call '" #X "' function!\n"); \
    abort(); \
  }

// insn-recog.o  
SYMBOL(recog)

// simplify-rtx.o
SYMBOL(simplify_relational_operation)
SYMBOL(simplify_gen_unary)
SYMBOL(simplify_gen_binary)
SYMBOL(simplify_gen_subreg)
SYMBOL(simplify_binary_operation)
SYMBOL(simplify_subreg)
SYMBOL(simplify_unary_operation)
SYMBOL(simplify_replace_rtx)
SYMBOL(simplify_rtx)
SYMBOL(simplify_gen_relational)
SYMBOL(simplify_gen_ternary)
SYMBOL(avoid_constant_pool_reference)
SYMBOL(simplify_const_unary_operation)

// insn-attr-tab.o (i386)
SYMBOL(get_attr_fp_int_src)
SYMBOL(get_attr_length_address)
SYMBOL(get_attr_memory)
SYMBOL(get_attr_mode)
SYMBOL(get_attr_prefix_0f)
SYMBOL(get_attr_type)
SYMBOL(get_attr_unit)
SYMBOL(get_attr_i387_cw)
