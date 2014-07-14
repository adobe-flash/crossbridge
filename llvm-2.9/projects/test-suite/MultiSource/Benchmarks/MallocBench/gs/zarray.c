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

/* zarray.c */
/* Array operators for GhostScript */
#include "memory_.h"
#include "ghost.h"
#include "alloc.h"
#include "errors.h"
#include "oper.h"
#include "store.h"
#include "sstorei.h"
#include <stdint.h>

/* The generic operators (copy, get, put, getinterval, putinterval, */
/* length, and forall) are implemented in zgeneric.c. */

/* Forward references */
int make_array(P4(register ref *, int, int, char *));

/* array */
int
zarray(register ref *op)
{	int code = make_array(op, t_array, a_all, "array");
	if ( code < 0 ) return code;
	   {	/* Fill the array with nulls. */
		ushort size = op->size;
		ref *abody = op->value.refs;
		while ( size-- ) abody++->type_attrs = null_type_attrs;
	   }
	return 0;
}

/* aload */
int
zaload(register ref *op)
{	ref aref;
	check_array(*op);
	check_read(*op);
	aref = *op;
	if ( aref.size > ostop - op ) return e_rangecheck;
	memcpy((char *)op, (char *)aref.value.refs, aref.size * sizeof(ref));
	push(aref.size);
	*op = aref;
	return 0;
}

/* astore */
int
zastore(register ref *op)
{	uintptr_t size;
	check_type(*op, t_array);
	check_write(*op);
	size = op->size;
	if ( size > op - osbot ) return e_stackunderflow;
	refcpy(op->value.refs, op - size, size);
	op[-size] = *op;
	pop(size);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zarray_op_init()
{	static op_def my_defs[] = {
		{"1aload", zaload},
		{"1array", zarray},
		{"1astore", zastore},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Internal procedures ------ */

/* Make an array from the operand stack. */
/* Don't fill it in. */
/* This is also used, for the moment, for packed arrays. */
int
make_array(register ref *op, int type, int attrs, char *client_name)
{	ref *abody;
	uint size;
	check_type(*op, t_integer);
	if ( op->value.intval < 0 ||
	     op->value.intval > max_uint / sizeof(ref) - 1
	   )
		return e_rangecheck;
	size = op->value.intval;
	abody = (ref *)alloc(size, sizeof(ref), client_name);
	if ( abody == 0 ) return e_VMerror;
	make_tasv(op, type, attrs, size, refs, abody);
	return 0;
}
