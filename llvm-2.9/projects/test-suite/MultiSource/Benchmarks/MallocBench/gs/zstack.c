/* Copyright (C) 1989 Aladdin Enterprises.  All rights reserved.
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

/* zstack.c */
/* Operand stack operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "store.h"
#include "sstorei.h"

/* pop */
int
zpop(register ref *op)
{	check_op(1);
	pop(1);
	return 0;
}

/* exch */
int
zexch(register ref *op)
{	ref next;
	ref *pnext = &next;
	check_op(2);
	s_store_b(pnext, 0, op, -1);
	s_store_r(op, -1, 0);
	s_store_i(op, pnext);
	return 0;
}

/* dup */
int
zdup(register ref *op)
{	check_op(1);
	push(1);
	s_store_r(op, 0, -1);
	return 0;
}

/* index */
int
zindex(register ref *op)
{	ref *opn;
	check_type(*op, t_integer);
	if ( (ulong)op->value.intval >= op - osbot ) return e_rangecheck;
	opn = op + ~(int)op->value.intval;
	s_store_i(op, opn);
	return 0;
}

/* roll */
int
zroll(register ref *op)
{	ref *op1 = op - 1;
	int count, mod;
	int istart, n;
	ref *base;
	check_type(*op1, t_integer);
	check_type(*op, t_integer);
	if ( (ulong)op1->value.intval > op1 - osbot )
		return e_rangecheck;
	count = op1->value.intval;
	if ( count == 0 ) { pop(2); return 0; }	/* no action */
	mod = op->value.intval % count;
	pop(2);				/* we're OK now */
	op -= 2;
	if ( mod < 0 )			/* can't assume % means mod! */
		mod += count;
	else if ( mod == 0 ) return 0;	/* no action */
	/* Rotate the elements in chains separated by mod elements. */
	/* The number of chains is gcd(count,mod): rather than compute */
	/* this, we simply proceed until we have moved count elements. */
	/* Since the loop interprets mod backwards from the way PostScript */
	/* specifies it, we have to complement mod first. */
	mod = count - mod;
	istart = 0;			/* first element of chain */
	n = count;			/* # of elements left to move */
	base = op - count + 1;
	while ( n )
	   {	ref save;
		int i = istart;
		int inext;
		save = base[istart];	/* no auto init for structures! */
		while ( n--, (inext = (i + mod) % count) != istart )
		   {	base[i] = base[inext];
			i = inext;
		   }
		base[i] = save;
		istart++;
	   }
	return 0;
}

/* clear */
/* The function name is changed, because the IRIS library has */
/* a function called zclear. */
int
zclear_stack(ref *op)
{	osp = osbot - 1;
	return 0;
}

/* count */
int
zcount(register ref *op)
{	push(1);
	make_int(op, op - osbot);
	return 0;
}

/* cleartomark */
int
zcleartomark(register ref *op)
{	while ( op >= osbot )
	   {	if ( r_type(op) == t_mark )
		   {	osp = op - 1;
			return 0;
		   }
		op--;
	   }
	return e_unmatchedmark;
}

/* counttomark */
int
zcounttomark(register ref *op)
{	register ref *mp = op;
	while ( mp >= osbot )
	   {	if ( r_type(mp) == t_mark )
		   {	push(1);
			make_int(op, op - mp - 1);
			return 0;
		   }
		mp--;
	   }
	return e_unmatchedmark;
}

/* ------ Initialization procedure ------ */

void
zstack_op_init()
{	static op_def my_defs[] = {
		{"0clear", zclear_stack},
		{"0cleartomark", zcleartomark},
		{"0count", zcount},
		{"0counttomark", zcounttomark},
		{"1dup", zdup},
		{"2exch", zexch},
		{"2index", zindex},
		{"1pop", zpop},
		{"2roll", zroll},
		op_def_end
	};
	z_op_init(my_defs);
}
