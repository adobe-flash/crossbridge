/* Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* zrelbit.c */
/* Relational, boolean, and bit operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "store.h"
#include "sstorei.h"
#include "dict.h"

/* eq */
int
zeq(register ref *op)
{	register ref *op1 = op - 1;
#define eq_check_read(opp)\
  switch ( r_type(opp) )\
   {	case t_string: case t_array: case t_packedarray:\
	  check_read(*opp); break;\
	case t_dictionary: check_dict_read(*opp);\
   }
	eq_check_read(op);
	eq_check_read(op1);
	make_bool(op1, (obj_eq(op1, op) ? 1 : 0));
	pop(1);
	return 0;
}

/* ne */
int
zne(register ref *op)
{	/* We'll just be lazy and use eq. */
	int code = zeq(op);
	if ( !code ) op[-1].value.index ^= 1;
	return code;
}

/* ge */
int
zge(register ref *op)
{	int code = obj_compare(op, 4+2);
	if ( code < 0 ) return code;
	make_bool(op - 1, code);
	pop(1);
	return 0;
}

/* gt */
int
zgt(register ref *op)
{	int code = obj_compare(op, 4);
	if ( code < 0 ) return code;
	make_bool(op - 1, code);
	pop(1);
	return 0;
}

/* le */
int
zle(register ref *op)
{	int code = obj_compare(op, 2+1);
	if ( code < 0 ) return code;
	make_bool(op - 1, code);
	pop(1);
	return 0;
}

/* lt */
int
zlt(register ref *op)
{	int code = obj_compare(op, 1);
	if ( code < 0 ) return code;
	make_bool(op - 1, code);
	pop(1);
	return 0;
}

/* max */
int
zmax(register ref *op)
{	int code = obj_compare(op, 1);
	if ( code < 0 ) return code;
	if ( code )
	   {	s_store_r(op, -1, 0);
	   }
	pop(1);
	return 0;
}

/* min */
int
zmin(register ref *op)
{	int code = obj_compare(op, 4);
	if ( code < 0 ) return code;
	if ( code )
	   {	s_store_r(op, -1, 0);
	   }
	pop(1);
	return 0;
}

/* and */
int
zand(register ref *op)
{	check_type(op[-1], r_type(op));
	switch ( r_type(op) )
	   {
	case t_boolean: op[-1].value.index &= op->value.index; break;
	case t_integer: op[-1].value.intval &= op->value.intval; break;
	default: return e_typecheck;
	   }
	pop(1);
	return 0;
}

/* not */
int
znot(register ref *op)
{	switch ( r_type(op) )
	   {
	case t_boolean: op->value.index = !op->value.index; break;
	case t_integer: op->value.intval = ~op->value.intval; break;
	default: return e_typecheck;
	   }
	return 0;
}

/* or */
int
zor(register ref *op)
{	check_type(op[-1], r_type(op));
	switch ( r_type(op) )
	   {
	case t_boolean: op[-1].value.index |= op->value.index; break;
	case t_integer: op[-1].value.intval |= op->value.intval; break;
	default: return e_typecheck;
	   }
	pop(1);
	return 0;
}

/* xor */
int
zxor(register ref *op)
{	check_type(op[-1], r_type(op));
	switch ( r_type(op) )
	   {
	case t_boolean: op[-1].value.index ^= op->value.index; break;
	case t_integer: op[-1].value.intval ^= op->value.intval; break;
	default: return e_typecheck;
	   }
	pop(1);
	return 0;
}

/* bitshift */
int
zbitshift(register ref *op)
{	int shift;
	check_type(op[-1], t_integer);
	check_type(*op, t_integer);
	if ( op->value.intval < -31 || op->value.intval > 31 )
		op[-1].value.intval = 0;
	else if ( (shift = op->value.intval) < 0 )
		op[-1].value.intval = ((ulong)(op[-1].value.intval)) >> -shift;
	else
		op[-1].value.intval <<= shift;
	pop(1);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zrelbit_op_init()
{	static op_def my_defs[] = {
		{"2and", zand},
		{"2bitshift", zbitshift},
		{"2eq", zeq},
		{"2ge", zge},
		{"2gt", zgt},
		{"2le", zle},
		{"2lt", zlt},
		{"2max", zmax},
		{"2min", zmin},
		{"2ne", zne},
		{"1not", znot},
		{"2or", zor},
		{"2xor", zxor},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Internal routines ------ */

/* Compare two operands (both numeric, or both strings). */
/* Compute a result: 4 if >, 2 if =, 1 if <. */
/* If the result matches a 1-bit in the mask, return 1, */
/* otherwise return 0. */
/* If the comparison fails, return a (negative) error code. */
int
obj_compare(register ref *op, int mask)
{
#define op1 (op-1)
	float real1, real2;
	switch ( r_type(op1) )
	   {
	case t_integer:
		switch ( r_type(op) )
		   {
		case t_integer:
			if ( op1->value.intval > op->value.intval )
				mask >>= 2;
			else if ( op1->value.intval == op->value.intval )
				mask >>= 1;
			return (mask & 1);
		case t_real:
			real1 = op1->value.intval;
			real2 = op->value.realval;
			break;
		default: return e_typecheck;
		   }
		break;
	case t_real:
		real1 = op1->value.realval;
		switch ( r_type(op) )
		   {
		case t_integer:
			real2 = op->value.intval;
			break;
		case t_real:
			real2 = op->value.realval;
			break;
		default: return e_typecheck;
		   }
		break;
	case t_string:
		check_read(*op1);
		check_read_type(*op, t_string);
		mask >>= (bytes_compare(op1->value.bytes, op1->size,
				op->value.bytes, op->size) + 1);
		return (mask & 1);
	default: return e_typecheck;
	   }
	if ( real1 > real2 )
		mask >>= 2;
	else if ( real1 == real2 )
		mask >>= 1;
	return (mask & 1);
#undef op1
}
