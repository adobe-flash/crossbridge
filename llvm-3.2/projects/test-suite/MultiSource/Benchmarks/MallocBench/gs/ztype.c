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

/* ztype.c */
/* Type, attribute, and conversion operators for GhostScript */
#include "math_.h"
#include "memory_.h"
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "dict.h"
#include "name.h"
#include "store.h"
#include "stream.h"

/* Imported values */
extern ref dstack[];			/* for systemdict */
#define systemdict (dstack[0])

/* Forward references */
private int access_check(P3(ref *, int, int));

/* Max and min integer values expressed as reals. */
/* Note that these are biased by 1 to correct for truncation. */
#define lb_real_int (-0x8000L * 1.0 * 0x10000L - 1)
#define ub_real_int (0x8000L * 1.0 * 0x10000L)

/* Get the pointer to the access flags for a ref. */
#define access_ref(opp)\
  (r_type(opp) == t_dictionary ? dict_access_ref(opp) : opp)

/* .typenumber */
int
ztypenumber(register ref *op)
{	check_op(1);
	make_int(op, r_btype(op));
	return 0;
}

/* cvlit */
int
zcvlit(register ref *op)
{	ref *aop;
	check_op(1);
	aop = access_ref(op);
	r_clear_attrs(aop, a_executable);
	return 0;
}

/* cvx */
int
zcvx(register ref *op)
{	ref *aop;
	check_op(1);
	aop = access_ref(op);
	r_set_attrs(aop, a_executable);
	return 0;
}

/* xcheck */
int
zxcheck(register ref *op)
{	check_op(1);
	make_bool(op, (r_attrs(access_ref(op)) & a_executable ? 1 : 0));
	return 0;
}

/* executeonly */
int
zexecuteonly(register ref *op)
{	check_op(1);
	if ( r_type(op) == t_dictionary ) return e_typecheck;
	return access_check(op, a_execute, 1);
}

/* noaccess */
int
znoaccess(register ref *op)
{	return access_check(op, 0, 1);
}

/* readonly */
int
zreadonly(register ref *op)
{	return access_check(op, a_read+a_execute, 1);
}

/* rcheck */
int
zrcheck(register ref *op)
{	int code = access_check(op, a_read, 0);
	if ( code >= 0 ) make_bool(op, code);
	return code;
}

/* wcheck */
int
zwcheck(register ref *op)
{	int code = access_check(op, a_write, 0);
	if ( code >= 0 ) make_bool(op, code);
	return code;
}

/* cvi */
int
zcvi(register ref *op)
{	float fval;
	switch ( r_type(op) )
	   {
	case t_integer: return 0;
	case t_real: fval = op->value.realval; break;
	default: return e_typecheck;
	case t_string:
	   {	stream st;
		ref nref;
		int code;
		check_read(*op);
		sread_string(&st, op->value.bytes, op->size);
		code = scan_number(&st, &nref);
		if ( code ) return code;	/* error condition */
		if ( sgetc(&st) != EOFC ) return e_syntaxerror;
		if ( r_type(&nref) == t_integer ) { *op = nref; return 0; }
		/* Otherwise, result was a real */
		fval = nref.value.realval;
	   }
	   }
	/* Check if a real will fit into an integer value */
	if ( fval <= lb_real_int || fval >= ub_real_int )
		return e_rangecheck;
	make_int(op, (long)fval);	/* truncates towards 0 */
	return 0;
}

/* cvn */
int
zcvn(register ref *op)
{	int exec;
	int code;
	check_read_type(*op, t_string);
	exec = r_attrs(op) & a_executable;
	code = name_ref(op->value.bytes, op->size, op, 1);
	if ( code ) return code;
	r_set_attrs(op, exec);
	return 0;
   }

/* cvr */
int
zcvr(register ref *op)
{	switch ( r_type(op) )
	   {
	case t_integer: make_real(op, op->value.intval);
	case t_real: return 0;
	default: return e_typecheck;
	case t_string:
	   {	stream st;
		ref nref;
		int code;
		check_read(*op);
		sread_string(&st, op->value.bytes, op->size);
		code = scan_number(&st, &nref);
		if ( code ) return code;	/* error condition */
		if ( sgetc(&st) != EOFC ) return e_syntaxerror;
		if ( r_type(&nref) == t_real ) { *op = nref; return 0; }
		/* Otherwise, result was an integer */
		make_real(op, nref.value.intval);
		return 0;
	   }
	   }
}

/* cvrs */
int
zcvrs(register ref *op)
{	int radix;
	long ival;
	ulong val;
	byte digits[31];
	byte *endp = &digits[31];
	byte *dp = endp;
	check_type(op[-1], t_integer);
	if ( op[-1].value.intval < 2 || op[-1].value.intval > 36 )
		return e_rangecheck;
	radix = op[-1].value.intval;
	check_write_type(*op, t_string);
	switch ( r_type(op - 2) )
	   {
	case t_integer: ival = op[-2].value.intval; break;
	case t_real:	/****** SHOULD USE cvi HERE ******/
	default:
		return e_typecheck;
	   }
	val = (ival < 0 ? -ival : ival);
	do
	   {	int dit = val % radix;
		*--dp = dit + (dit < 10 ? '0' : ('A' - 10));
		val /= radix;
	   }
	while ( val );
	if ( ival < 0 ) *--dp = '-';
	if ( endp - dp > op->size ) return e_rangecheck;
	memcpy(op->value.bytes, dp, (uint)(endp - dp));
	op->size = endp - dp;
	r_set_attrs(op, a_subrange);
	op[-2] = *op;
	pop(2);
	return 0;
}

/* cvs */
int
zcvs(register ref *op)
{	ref *op1 = op - 1;
	char buf[25];			/* big enough for any float */
	ref stref;
	check_write_type(*op, t_string);
	stref.value.bytes = (byte *)buf;
	switch ( r_btype(op1) )
	   {
	case t_boolean:
		stref.value.bytes =
			(byte *)(op1->value.index ? "true" : "false");
		break;
	case t_integer:
		sprintf(buf, "%ld", op1->value.intval);
		break;
	case t_name:
		name_string_ref(op1, &stref);	/* name string */
		goto nl;
	case t_operator:
	   {	/* Get the name by enumerating systemdict. */
		int pos = dict_first(&systemdict);
		ref entry[2];
		while ( (pos = dict_next(&systemdict, pos, entry)) >= 0 )
		   {	if ( (long)op1->value.opproc ==
			       (long)entry[1].value.opproc &&
			     r_type(&entry[0]) == t_name
			   )
			   {	/* Found it. */
				name_string_ref(&entry[0], &stref);
				goto nl;
			   }
		   }
	   }
		/* Can't find it (shouldn't happen...). */
		sprintf(buf, "operator %lx", (ulong)op1->value.opproc);
		break;
	case t_real:
		sprintf(buf, "%g", op1->value.realval);
		break;
	case t_string:
		check_read(*op1);
		stref = *op1;
		goto nl;
	default:
		check_op(1);
		stref.value.bytes = (byte *)"--nostringval--";
	   }
	stref.size = strlen((char *)stref.value.bytes);
nl:	if ( stref.size > op->size ) return e_rangecheck;
	memcpy(op->value.bytes, stref.value.bytes, stref.size);
	op[-1] = *op;
	op[-1].size = stref.size;
	r_set_attrs(op - 1, a_subrange);
	pop(1);
	return 0;
}

/* ------ Initialization procedure ------ */

void
ztype_op_init()
{	static op_def my_defs[] = {
		{"1cvi", zcvi},
		{"1cvlit", zcvlit},
		{"1cvn", zcvn},
		{"1cvr", zcvr},
		{"3cvrs", zcvrs},
		{"2cvs", zcvs},
		{"1cvx", zcvx},
		{"1executeonly", zexecuteonly},
		{"1noaccess", znoaccess},
		{"1rcheck", zrcheck},
		{"1readonly", zreadonly},
		{"1.typenumber", ztypenumber},
		{"1wcheck", zwcheck},
		{"1xcheck", zxcheck},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Internal routines ------ */

/* Test or modify the access of an object. */
/* If modify = 1, restrict to the selected access and return 0; */
/* if modify = 0, return 1 if the object had the access, 0 if not. */
/* Return an error code if the object is not of appropriate type, */
/* or if the object did not have the access already when modify=1. */
private int
access_check(ref *op,
    int access,				/* mask for attrs */
    int modify)				/* if true, reduce access */
{	ref *aop = op;
	switch ( r_type(op) )
	   {
	default: return e_typecheck;
	case t_dictionary:
		aop = dict_access_ref(op);
	case t_array: case t_file:
	case t_packedarray: case t_string: ;
	   }
	if ( modify )
	   {	if ( ~r_attrs(aop) & access )
			return e_invalidaccess;
		r_clear_attrs(aop, a_all);
		r_set_attrs(aop, access);
		return 0;
	   }
	else
	   {	return (r_attrs(aop) & access) == access;
	   }
}
