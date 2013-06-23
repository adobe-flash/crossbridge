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

/* zht.c */
/* Halftone operator and rendering for GhostScript */
#include "ghost.h"
#include "memory_.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "estack.h"
#include "gsmatrix.h"
#include "gsstate.h"
#include "state.h"
#include "store.h"

/* Forward references */
private int screen_sample(P1(ref *));
private int set_screen_continue(P1(ref *));

/* Imported from util.c */
extern	int	num_params(P3(ref *, int, float *));

/* The setscreen operator is complex because it has to sample */
/* each pixel in the pattern cell, calling a procedure, and then */
/* sort the result into a whitening order. */

/* Layout of stuff pushed on estack: */
/*	Control mark, */
/*	spot procedure, */
/*	enumeration structure (as bytes). */
#define snumpush 3
#define sproc esp[-1]
#define senum (gs_screen_enum *)esp->value.bytes

/* setscreen */
int
zsetscreen(register ref *op)
{	float fa[2];
	int code = num_params(op - 1, 2, fa);
	gs_screen_enum *penum;
	if ( code < 0 ) return code;
	check_proc(*op);
	penum = (gs_screen_enum *)alloc(1, gs_screen_enum_sizeof, "setscreen");
	if ( penum == 0 ) return e_VMerror;
	code = gs_screen_init(penum, igs, fa[0], fa[1]);
	if ( code < 0 )
	   {	alloc_free((char *)penum, 1, gs_screen_enum_sizeof, "setscreen");
		return code;
	   }
	/* Push everything on the estack */
	check_estack(snumpush);
	mark_estack(es_other);
	*++esp = *op;			/* sproc = proc */
	++esp;
	make_tasv(esp, t_string, 0, gs_screen_enum_sizeof, bytes, (byte *)penum);
	pop(3);  op -= 3;
	return screen_sample(op);
}
/* Set up the next sample */
private int
screen_sample(register ref *op)
{	gs_screen_enum *penum = senum;
	gs_point pt;
	int code = gs_screen_currentpoint(penum, &pt);
	int set_screen_continue(P1(ref *));
	ref proc;
	if ( code < 0 ) return code;
	if ( code != 0 )
	   {	/* All done */
		istate.screen_proc = sproc;
		esp -= snumpush;
		return o_check_estack;
	   }
	push(2);
	make_real(op - 1, pt.x);
	make_real(op, pt.y);
	proc = sproc;
	push_op_estack(set_screen_continue);
	*++esp = proc;
	return o_check_estack;
}
/* Continuation procedure for processing sampled pixels. */
private int
set_screen_continue(register ref *op)
{	float value;
	int code = num_params(op, 1, &value);
	if ( code < 0 ) return code;
	code = gs_screen_next(senum, value);
	if ( code < 0 ) return code;
	pop(1);  op--;
	return screen_sample(op);
}

/* ------ Initialization procedure ------ */

void
zht_op_init()
{	static op_def my_defs[] = {
		{"3setscreen", zsetscreen},
		op_def_end
	};
	z_op_init(my_defs);
}
