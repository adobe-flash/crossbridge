/* Copyright (C) 1990 Aladdin Enterprises.  All rights reserved.
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

/* zpacked.c */
/* Packed array operators for Ghostscript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "store.h"
#include <stdint.h>

/* Import the array packing flag */
extern int array_packing;

/* Import a utility procedure from zarray.c. */
extern int make_array(P4(ref *, int, int, char *));

/*

The following is a new design for packed arrays, not implemented yet.

Use packed elements of 2 different sizes, 2 bytes or 8 bytes.  The top
bits of the first 16-bit subelement distinguish the 2 forms.  The 'size'
of a packed array is the number of bytes it occupies, not the number of
elements.  A plausible encoding:

	00iiiiii iiiiiiii	executable name
	01iiiiii iiiiiiii	literal name
	100jjjjj jjjjjjjj	(executable) operator (for bind)
	101svvvv vvvvvvvv	2's complement integer
	11s000wr ttttttxe	full 8-byte object

It would be very nice to have a code that says "embedded packed array
follows", but this would create a lot of extra complexity for both the
scanner (the only place where such a thing could be constructed) and the
accessing routines for packed arrays.

This encoding requires a table of all operators, and a table of all
names.  (The latter already exists.)  Both are easy to construct
dynamically as a 2-level array of (say) 256-element arrays.

 */

/* currentpacking */
int
zcurrentpacking(register ref *op)
{	push(1);
	make_bool(op, array_packing);
	return 0;
}

/* packedarray */
int
zpackedarray(register ref *op)
{	int code = make_array(op, t_packedarray, a_read+a_execute, "packedarray");
	if ( code < 0 ) return code;
	   {	/* Fill the array from the stack. */
		uintptr_t size = op->size;
		if ( size > op - osbot ) return e_stackunderflow;
		refcpy(op->value.refs, op - size, size);
		op[-size] = *op;
		pop(size);
	   }
	return 0;
}

/* setpacking */
int
zsetpacking(register ref *op)
{	check_type(*op, t_boolean);
	array_packing = op->value.index;
	pop(1);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zpacked_op_init()
{	static op_def my_defs[] = {
		{"0currentpacking", zcurrentpacking},
		{"1packedarray", zpackedarray},
		{"1setpacking", zsetpacking},
		op_def_end
	};
	z_op_init(my_defs);
}
