/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* LLVM interface to GCC
   Copyright (C) 2005 Free Software Foundation, Inc.
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

/* This file defines the public C interfaces to the LLVM backend. */
#ifndef LLVM_H
#define LLVM_H

/* This file is a noop unless GCC is configured with --enable-llvm. */
#ifdef ENABLE_LLVM

#include <stdio.h>
union tree_node;

/*
 *   Code defined in llvm-backend.cpp
 */
void llvm_initialize_backend(void);
void llvm_lang_dependent_init(const char *Name);

/* emit_llvm_code_for_current_function - Top level interface for emitting a
 * function to the .s file.
 */
void llvm_emit_code_for_current_function(union tree_node* fndecl);

/* make_decl_llvm - This is also defined in tree.h and used by macros there. */
void make_decl_llvm(union tree_node*);

/* reset_type_and_initializer_llvm - Change the initializer for a global 
 * variable.
 */
void reset_type_and_initializer_llvm(union tree_node*);

/* reset_initializer_llvm - Change the initializer for a global variable. */
void reset_initializer_llvm(union tree_node*);

/* emit_global_to_llvm - Emit the specified VAR_DECL to LLVM as a global
 * variable.
 */
void emit_global_to_llvm(union tree_node*);

/* emit_alias_to_llvm - Emit the specified alias to LLVM
 */
void emit_alias_to_llvm(union tree_node*, union tree_node*, union tree_node*);

/* llvm_get_decl_name - Used by varasm.c, returns the specified declaration's
 * name.
 */
const char *llvm_get_decl_name(void *LLVM);

/* llvm_mark_decl_weak - Used by varasm.c, called when a decl is found to be
 * weak, but it already had an llvm object created for it.  This marks the LLVM
 * object weak as well.
 */
void llvm_mark_decl_weak(union tree_node*);

/* llvm_emit_ctor_dtor - Called to emit static ctors/dtors to LLVM code.  fndecl
 * is a 'void()' FUNCTION_DECL for the code, initprio is the init priority, and
 * isInit indicates whether this is a ctor or dtor.
 */
void llvm_emit_ctor_dtor(union tree_node* fndecl, int initprio, int isInit);

/* llvm_emit_file_scope_asm - Emit the specified string as a file-scope inline
 * asm block.
 */
void llvm_emit_file_scope_asm(const char*);

/* llvm_emit_typedef - Emit the specified TYPE_DECL if desired.
 */
void llvm_emit_typedef(union tree_node*);

/* print_llvm - Print the specified LLVM chunk like an operand, called by
 * print-tree.c for tree dumps.
 */
void print_llvm(FILE *file, void *LLVM);

/* print_llvm_type - Print the specified LLVM type symbolically, called by
 * print-tree.c for tree dumps.
 */
void print_llvm_type(FILE *file, void *LLVM);

/* Init pch writing. */
void llvm_pch_write_init(void);

/* Read bytecodes from PCH file.  */
void llvm_pch_read(const unsigned char *, unsigned);

/* llvm_asm_file_start - Start the .s file. */
void llvm_asm_file_start(void);

/* llvm_asm_file_end - Finish the .s file. */
void llvm_asm_file_end(void);

/* llvm_call_llvm_shutdown - Release LLVM global state. */
void llvm_call_llvm_shutdown(void);

/* refine_type_to - Cause all users of the opaque type old_type to switch
 * to the more concrete type new_type.
 */
void refine_type_to (union tree_node*, union tree_node*);
#endif /* ENABLE_LLVM */

#endif
/* LLVM LOCAL end (ENTIRE FILE!)  */
