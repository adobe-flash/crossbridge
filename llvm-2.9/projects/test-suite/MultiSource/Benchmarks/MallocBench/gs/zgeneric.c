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

/* zgeneric.c */
/* Array/string/dictionary generic operators for PostScript */
#include "memory_.h"
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "dict.h"
#include "estack.h"	/* for forall */
#include "store.h"

/* This file implements copy, get, put, getinterval, putinterval, */
/* length, and forall, which apply generically to */
/* arrays, strings, and dictionaries.  (Copy also has a special */
/* meaning for copying the top N elements of the stack.) */

/* Forward references */
private int copy_interval(P3(ref *, uint, ref*));

/* copy */
/* Note that this implements copy for arrays and strings, */
/* but not for dictionaries (see zcopy_dict in zdict.c). */
int
zcopy(register ref *op)
{	int code;
	switch ( r_type(op) )
	   {
	case t_integer:
	   {	int count;
		if ( (ulong)op->value.intval > op - osbot )
			return e_rangecheck;
		count = op->value.intval;
		if ( op - 1 + count > ostop )
			return e_stackoverflow;
		memcpy((char *)op, (char *)(op - count), count * sizeof(ref));
		push(count - 1);
		return 0;
	   }
	case t_array:
	case t_packedarray:
	case t_string:
		code = copy_interval(op, 0, op - 1);
		break;
	case t_dictionary:
		return zcopy_dict(op);
	default:
		return e_typecheck;
	   }
	if ( code ) return code;	/* error */
	op->size = op[-1].size;
	op[-1] = *op;
	r_set_attrs(op - 1, a_subrange);
	pop(1);
	return 0;
}

/* length */
int
zlength(register ref *op)
{	switch ( r_type(op) )
	   {
	case t_array:
	case t_packedarray:
	case t_string:
		check_read(*op);
		make_int(op, op->size);
		return 0;
	case t_dictionary:
		check_dict_read(*op);
		make_int(op, dict_length(op));
		return 0;
	case t_name:
	   {	/* The PostScript manual doesn't allow length on names, */
		/* but the implementations apparently do. */
		ref str;
		name_string_ref(op, &str);
		make_int(op, str.size);
	   }
		return 0;
	default:
		return e_typecheck;
	   }
}

/* get */
int
zget(register ref *op)
{	ref *op1 = op - 1;
	ref *pvalue;
	switch ( r_type(op1) )
	   {
	case t_dictionary:
		check_dict_read(*op1);
		if ( dict_find(op1, op, &pvalue) <= 0 ) return e_undefined;
		break;
	case t_array:
	case t_packedarray:
		check_type(*op, t_integer);
		check_read(*op1);
		if ( (ulong)(op->value.intval) >= op1->size )
			return e_rangecheck;
		pvalue = op1->value.refs + (uint)op->value.intval;
		break;
	case t_string:
		check_type(*op, t_integer);
		check_read(*op1);
		if ( (ulong)(op->value.intval) >= op1->size )
			return e_rangecheck;
	   {	int element = op1->value.bytes[(uint)op->value.intval];
		make_int(op1, element);
		pop(1);
	   }
		return 0;
	default:
		return e_typecheck;
	   }
	op[-1] = *pvalue;
	pop(1);
	return 0;
}

/* put */
int
zput(register ref *op)
{	ref *op1 = op - 1;
	ref *op2 = op1 - 1;
	switch ( r_type(op2) )
	   {
	case t_dictionary:
		check_dict_write(*op2);
	   {	int code = dict_put(op2, op1, op);
		if ( code ) return code;	/* error */
	   }
		break;
	case t_array:
		check_type(*op1, t_integer);
		check_write(*op2);
		if ( op1->value.intval < 0 || op1->value.intval >= op2->size )
			return e_rangecheck;
		store_i(op2->value.refs + (uint)op1->value.intval, op);
		break;
	case t_packedarray:		/* packed arrays are read-only */
		return e_invalidaccess;
	case t_string:
		check_type(*op1, t_integer);
		check_write(*op2);
		if ( op1->value.intval < 0 || op1->value.intval >= op2->size )
			return e_rangecheck;
		check_type(*op, t_integer);
		if ( (ulong)op->value.intval > 0xff )
			return e_rangecheck;
		op2->value.bytes[(uint)op1->value.intval] = (byte)op->value.intval;
		break;
	default:
		return e_typecheck;
	   }
	pop(3);
	return 0;
}

/* getinterval */
int
zgetinterval(register ref *op)
{	ref *op1 = op - 1;
	ref *op2 = op1 - 1;
	uint index;
	uint count;
	check_type(*op1, t_integer);
	check_type(*op, t_integer);
	switch ( r_type(op2) )
	   {
	default: return e_typecheck;
	case t_array: case t_packedarray: case t_string: ;
	   }
	check_read(*op2);
	if ( (ulong)op1->value.intval > op2->size ) return e_rangecheck;
	index = op1->value.intval;
	if ( (ulong)op->value.intval > op2->size - index ) return e_rangecheck;
	count = op->value.intval;
	switch ( r_type(op2) )
	   {
	case t_array:
	case t_packedarray: op2->value.refs += index; break;
	case t_string: op2->value.bytes += index; break;
	   }
	op2->size = count;
	r_set_attrs(op2, a_subrange);
	pop(2);
	return 0;
}

/* putinterval */
int
zputinterval(register ref *op)
{	ref *opindex = op - 1;
	ref *opto = opindex - 1;
	int code;
	check_type(*opindex, t_integer);
	switch ( r_type(opto) )
	   {
	default: return e_typecheck;
	case t_packedarray: return e_invalidaccess;
	case t_array: case t_string: ;
	   }
	check_write(*opto);
	if ( (ulong)opindex->value.intval > opto->size ) return e_rangecheck;
	code = copy_interval(opto, (uint)(opindex->value.intval), op);
	if ( code >= 0 ) pop(3);
	return 0;
}

/* forall */
private int
  array_continue(P1(ref *)),
  string_continue(P1(ref *)),
  dict_continue(P1(ref *));
int
zforall(register ref *op)
{	int (*cproc)(P1(ref *));
	ref *obj = op - 1;
	uint index;
	switch ( r_type(obj) )
	   {
	default:
		return e_typecheck;
	case t_array:
	case t_packedarray:
		check_read(*obj);
		cproc = array_continue;
		index = 0;		/* not used */
		break;
	case t_string:
		check_read(*obj);
		cproc = string_continue;
		index = 0;		/* not used */
		break;
	case t_dictionary:
		check_dict_read(*obj);
		cproc = dict_continue;
		index = dict_first(obj);
		break;
	   }
	/* Push a mark, the composite object, the iteration index, */
	/* and the procedure, and invoke the continuation operator. */
	check_estack(6);
	mark_estack(es_for);
	*++esp = *obj;
	++esp;
	make_int(esp, index);
	*++esp = *op;
	pop(2);  op -= 2;
	return (*cproc)(op);
}
/* Continuation operator for arrays */
private int
array_continue(register ref *op)
{	ref *obj = esp - 2;
	if ( obj->size-- )		/* continue */
	   {	push(1);
		*op = *obj->value.refs;
		obj->value.refs++;
		push_op_estack(array_continue);	/* push continuation */
		*++esp = obj[2];
	   }
	else				/* done */
	   {	esp -= 4;		/* pop mark, object, index, proc */
	   }
	return o_check_estack;
}
/* Continuation operator for strings */
private int
string_continue(register ref *op)
{	ref *obj = esp - 2;
	if ( obj->size-- )		/* continue */
	   {	push(1);
		make_int(op, *obj->value.bytes);
		obj->value.bytes++;
		push_op_estack(string_continue);	/* push continuation */
		*++esp = obj[2];
	   }
	else				/* done */
	   {	esp -= 4;		/* pop mark, object, index, proc */
	   }
	return o_check_estack;
}
/* Continuation operator for dictionaries */
private int
dict_continue(register ref *op)
{	ref *obj = esp - 2;
	int index = (int)esp[-1].value.intval;
	push(2);			/* make room for key and value */
	if ( (index = dict_next(obj, index, op - 1)) >= 0 )	/* continue */
	   {	esp[-1].value.intval = index;
		push_op_estack(dict_continue);	/* push continuation */
		*++esp = obj[2];
	   }
	else				/* done */
	   {	pop(2);			/* undo push */
		esp -= 4;		/* pop mark, object, index, proc */
	   }
	return o_check_estack;
}

/* ------ Initialization procedure ------ */

void
zgeneric_op_init()
{	static op_def my_defs[] = {
		{"1copy", zcopy},
		{"2forall", zforall},
		{"2get", zget},
		{"3getinterval", zgetinterval},
		{"1length", zlength},
		{"3put", zput},
		{"3putinterval", zputinterval},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Shared routines ------ */

/* Copy an interval from one operand to another. */
/* This is used by both putinterval and string/array copy. */
/* One operand is known to be an array or string, */
/* and the starting index is known to be less than or equal to */
/* its length; nothing else has been checked. */
private int
copy_interval(ref *prto, uint index, ref *prfrom)
{	if ( r_type(prfrom) != r_type(prto) ) return e_typecheck;
	check_read(*prfrom);
	check_write(*prto);
	if ( prfrom->size > prto->size - index ) return e_rangecheck;
	switch ( r_type(prto) )
	   {
	case t_array:
	case t_packedarray:
		refcpy(prto->value.refs + index, prfrom->value.refs, prfrom->size);
		break;
	case t_string:
		memcpy(prto->value.bytes + index, prfrom->value.bytes, prfrom->size);
	   }
	return 0;
}
