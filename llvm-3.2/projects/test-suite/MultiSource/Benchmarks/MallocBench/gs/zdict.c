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

/* zdict.c */
/* Dictionary operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "dict.h"
#include "store.h"

/* Import the dictionary stack */
extern ref dstack[];
extern ref *dsp, *dstop;

/* dict */
int
zdict(register ref *op)
{	check_type(*op, t_integer);
	if ( op->value.intval < 0 || op->value.intval > dict_max_size )
		return e_rangecheck;
	return dict_create((uint)op->value.intval, op);
}

/* maxlength */
int
zmaxlength(register ref *op)
{	check_type(*op, t_dictionary);
	check_dict_read(*op);
	make_int(op, dict_maxlength(op));
	return 0;
}

/* setmaxlength */
int
zsetmaxlength(register ref *op)
{	uint new_size;
	int code;
	ref *op1 = op - 1;
	check_type(*op1, t_dictionary);
	check_dict_write(*op1);
	check_type(*op, t_integer);
	if ( op->value.intval < 0 || op->value.intval > dict_max_size )
		return e_rangecheck;
	new_size = (uint)op->value.intval;
	if ( dict_length(op - 1) > new_size )
		return e_dictfull;
	code = dict_resize(op - 1, new_size);
	if ( code >= 0 ) pop(2);
	return code;
}

/* begin */
int
zbegin(register ref *op)
{	check_type(*op, t_dictionary);
	check_dict_read(*op);
	if ( dsp == dstop ) return e_dictstackoverflow;
	*++dsp = *op;
	pop(1);
	return 0;
}

/* end */
int
zend(register ref *op)
{	if ( dsp == dstack + 1 ) return e_dictstackunderflow;
	dsp--;
	return 0;
}

/* def */
int
zdef(register ref *op)
{	int code;
	check_op(2);
	if ( r_type(op - 1) == t_null ) return e_typecheck;
	check_dict_write(*dsp);
	code = dict_put(dsp, op - 1, op);
	if ( !code ) pop(2);
	return code;
}

/* load */
int
zload(register ref *op)
{	ref *pvalue;
	check_op(1);
	if ( r_type(op) == t_null ) return e_typecheck;
	if ( dict_lookup(dstack, dsp, op, &pvalue) <= 0 )
		return e_undefined;
	*op = *pvalue;
	return 0;
}

/* store */
int
zstore(register ref *op)
{	ref *pvalue;
	int code;
	check_op(2);
	if ( r_type(op - 1) == t_null ) return e_typecheck;
	if ( dict_lookup(dstack, dsp, op - 1, &pvalue) <= 0 )
	   {	code = dict_put(dsp, op - 1, op);
		if ( code ) return code;
	   }
	else
		store_i(pvalue, op);
	pop(2);
	return 0;
}

/* get - implemented in zarray.c */

/* put - implemented in zarray.c */

/* known */
int
zknown(register ref *op)
{	ref *op1 = op - 1;
	ref *pvalue;
	check_type(*op1, t_dictionary);
	check_dict_read(*op1);
	make_bool(op1,
		  (r_type(op) == t_null ? 0 :
		   dict_find(op1, op, &pvalue) > 0 ? 1 : 0));
	pop(1);
	return 0;
}

/* where */
int
zwhere(register ref *op)
{	ref *pdref = dsp;
	ref *pvalue;
	check_op(1);
	if ( r_type(op) == t_null )
	   {	make_bool(op, 0);
		return 0;
	   }
	while ( 1 )
	   {	check_dict_read(*pdref);
		if ( dict_find(pdref, op, &pvalue) > 0 ) break;
		if ( --pdref < dstack )
		   {	make_bool(op, 0);
			return 0;
		   }
	   }
	*op = *pdref;
	push(1);
	make_bool(op, 1);
	return 0;
}

/* copy for dictionaries -- called from zcopy in zgeneric.c. */
/* Only the type of *op has been checked. */
int
zcopy_dict(register ref *op)
{	ref *op1 = op - 1;
	check_type(*op1, t_dictionary);
	check_dict_read(*op1);
	check_dict_write(*op);
	if ( dict_length(op) != 0 || dict_maxlength(op) < dict_maxlength(op1) )
		return e_rangecheck;
	dict_copy(op1, op);
	op[-1] = *op;
	pop(1);
	return 0;
}

/* currentdict */
int
zcurrentdict(register ref *op)
{	push(1);
	*op = *dsp;
	return 0;
}

/* countdictstack */
int
zcountdictstack(register ref *op)
{	push(1);
	make_int(op, dsp - dstack + 1);
	return 0;
}

/* dictstack */
int
zdictstack(register ref *op)
{	int depth = dsp - dstack + 1;
	check_write_type(*op, t_array);
	if ( depth > op->size ) return e_rangecheck;
	op->size = depth;
	r_set_attrs(op, a_subrange);
	refcpy(op->value.refs, dstack, depth);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zdict_op_init()
{	static op_def my_defs[] = {
		{"1begin", zbegin},
		{"0countdictstack", zcountdictstack},
		{"0currentdict", zcurrentdict},
		{"2def", zdef},
		{"1dict", zdict},
		{"0dictstack", zdictstack},
		{"0end", zend},
		{"2known", zknown},
		{"1load", zload},
		{"1maxlength", zmaxlength},
		{"2setmaxlength", zsetmaxlength},
		{"2store", zstore},
		{"1where", zwhere},
		op_def_end
	};
	z_op_init(my_defs);
}
