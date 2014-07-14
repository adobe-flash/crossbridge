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

/* zcolor.c */
/* Color operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "store.h"
#include "gsmatrix.h"			/* for gs_state */
#include "gsstate.h"
#include "state.h"

/* Imported from util.c */
extern	int	num_params(P3(ref *, int, float *));
/* Imported from zgstate.c */
extern	void	tri_put(P2(ref *, float [3]));

/* Forward declarations */
private int make_color(P1(ref *));

/* currentgscolor */
int
zcurrentgscolor(register ref *op)
{	int code;
	push(1);
	if (	(code = make_color(op)) < 0 ||
		(code = gs_currentgscolor(igs, op->value.pcolor)) < 0
	   )
		pop(1);
	return code;
}

/* setgscolor */
int
zsetgscolor(register ref *op)
{	int code;
	check_type(*op, t_color);
	if ( (code = gs_setgscolor(igs, op->value.pcolor)) < 0 )
		return code;
	pop(1);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zcolor_op_init()
{	static op_def my_defs[] = {
		{"0currentgscolor", zcurrentgscolor},
		{"1setgscolor", zsetgscolor},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Internal routines ------ */

/* Make a color object */
private int
make_color(ref *op)
{	gs_color *cp = (gs_color *)alloc(1, gs_color_sizeof, "make_color");
	if ( cp == 0 ) return e_VMerror;
	make_tv(op, t_color, pcolor, cp);
	return 0;
}
