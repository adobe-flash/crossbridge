/* LLVM LOCAL file */
/* Stub functions for C language routines that are referenced
   from the back-end.
   Copyright (C) 2007 Free Software Foundation, Inc.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "c-common.h"

#define ATTRIBUTE_WEAK	__attribute__ ((__weak__))

enum iasm_states iasm_state ATTRIBUTE_WEAK;
bool iasm_in_operands ATTRIBUTE_WEAK;
int flag_iasm_blocks ATTRIBUTE_WEAK;
int parse_in ATTRIBUTE_WEAK;
tree invoke_impl_ptr_type ATTRIBUTE_WEAK;


tree iasm_addr (tree) ATTRIBUTE_WEAK;

tree iasm_addr (tree e ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}


tree iasm_build_bracket (tree, tree) ATTRIBUTE_WEAK;

tree iasm_build_bracket (tree v1 ATTRIBUTE_UNUSED,
			 tree v2 ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}


void iasm_force_constraint (const char *, iasm_md_extra_info *) ATTRIBUTE_WEAK;

void iasm_force_constraint (const char *c ATTRIBUTE_UNUSED,
			    iasm_md_extra_info *e ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
}


void iasm_get_register_var (tree, const char *, char *, unsigned, bool,
			    iasm_md_extra_info *) ATTRIBUTE_WEAK;

void iasm_get_register_var (tree var ATTRIBUTE_UNUSED,
			    const char *modifier ATTRIBUTE_UNUSED,
			    char *buf ATTRIBUTE_UNUSED,
			    unsigned argnum ATTRIBUTE_UNUSED,
			    bool must_be_reg ATTRIBUTE_UNUSED,
			    iasm_md_extra_info *e ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
}


bool iasm_is_pseudo (const char *) ATTRIBUTE_WEAK;

bool iasm_is_pseudo (const char *opcode ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return false;
}


void iasm_print_operand (char *, tree, unsigned, tree *, bool, bool,
			 iasm_md_extra_info *) ATTRIBUTE_WEAK;

void iasm_print_operand (char *buf ATTRIBUTE_UNUSED,
			 tree arg ATTRIBUTE_UNUSED,
			 unsigned argnum ATTRIBUTE_UNUSED,
			 tree *uses ATTRIBUTE_UNUSED,
			 bool must_be_reg ATTRIBUTE_UNUSED,
			 bool must_not_be_reg ATTRIBUTE_UNUSED,
			 iasm_md_extra_info *e ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
}


tree build_modify_expr (tree, enum tree_code, tree) ATTRIBUTE_WEAK;

tree build_modify_expr (tree lhs ATTRIBUTE_UNUSED,
			enum tree_code modifycode ATTRIBUTE_UNUSED,
			tree rhs ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}


tree decl_constant_value (tree) ATTRIBUTE_WEAK;

tree decl_constant_value (tree decl ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}


tree build_function_call (tree, tree) ATTRIBUTE_WEAK;

tree build_function_call (tree function ATTRIBUTE_UNUSED,
			  tree params ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}


tree build_stmt (enum tree_code, ...) ATTRIBUTE_WEAK;

tree build_stmt (enum tree_code code ATTRIBUTE_UNUSED, ...)
{
  gcc_assert(0);
  return NULL;
}


tree lookup_name (tree) ATTRIBUTE_WEAK;

tree lookup_name (tree name ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}


tree pointer_int_sum (enum tree_code, tree, tree) ATTRIBUTE_WEAK;

tree pointer_int_sum (enum tree_code resultcode ATTRIBUTE_UNUSED,
		      tree ptrop ATTRIBUTE_UNUSED,
		      tree intop ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}


void store_init_value (tree, tree) ATTRIBUTE_WEAK;

void store_init_value (tree decl ATTRIBUTE_UNUSED, tree init ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
}

bool cvt_utf8_utf16 (const unsigned char *, size_t, unsigned char **,
                     size_t *) ATTRIBUTE_WEAK;

bool
cvt_utf8_utf16 (const unsigned char * inbuf ATTRIBUTE_UNUSED,
                size_t length ATTRIBUTE_UNUSED,
                unsigned char ** uniCharBuf  ATTRIBUTE_UNUSED,
                size_t * numUniChars ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return false;
}

tree create_init_utf16_var (const unsigned char *, size_t,
                            size_t *) ATTRIBUTE_WEAK;

tree
create_init_utf16_var (const unsigned char * inbuf  ATTRIBUTE_UNUSED,
                       size_t length  ATTRIBUTE_UNUSED,
                       size_t * numUniChars ATTRIBUTE_UNUSED)
{
  gcc_assert(0);
  return NULL;
}

tree pushdecl_top_level(tree) ATTRIBUTE_WEAK;

tree
pushdecl_top_level(tree x ATTRIBUTE_UNUSED) {
  gcc_assert(0);
  return NULL;
}
