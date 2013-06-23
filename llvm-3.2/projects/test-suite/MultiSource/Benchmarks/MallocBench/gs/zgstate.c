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

/* zgstate.c */
/* Graphics state operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "gsmatrix.h"
#include "gsstate.h"
#include "state.h"
#include "store.h"

/* Forward references */
private int num_param(P2(ref *, int (*)(P2(gs_state *, floatp))));
private int line_param(P2(ref *, int *));

/* Imported from util.c */
extern	int	real_param(P3(ref *, float *, int));
extern	int	num_params(P3(ref *, int, float *));

/* Forward declarations */
void	tri_put(P2(ref *, float [3]));

/* The current graphics state */
gs_state *igs;
int_state istate;

/* Initialize the graphics stack. */
void
gs_init()
{	igs = gs_state_alloc(alloc, alloc_free);
	istate.saved = 0;
	make_null(&istate.screen_proc);
	make_null(&istate.transfer);
}

/* gsave */
int
zgsave(register ref *op)
{	int_state *pis = (int_state *)alloc(1, sizeof(int_state), "gsave");
	if ( gs_gsave(igs) < 0 || pis == 0 )
		return e_limitcheck;
	*pis = istate;
	istate.saved = pis;
	return 0;
}

/* grestore */
int
zgrestore(register ref *op)
{	if ( gs_grestore(igs) >= 0 )
	   {	int_state *pis = istate.saved;
		istate = *pis;
		alloc_free((char *)pis, 1, sizeof(int_state), "grestore");
	   }
	return 0;
}

/* grestoreall */
int
zgrestoreall(register ref *op)
{	gs_grestoreall(igs);
	while ( istate.saved != 0 )
	   {	int_state *pis = istate.saved;
		istate = *pis;
		alloc_free((char *)pis, 1, sizeof(int_state), "restoreall");
	   }
	return 0;
}

/* initgraphics */
int
zinitgraphics(register ref *op)
{	return gs_initgraphics(igs);
}

/* setlinewidth */
int
zsetlinewidth(register ref *op)
{	return num_param(op, gs_setlinewidth);
}

/* currentlinewidth */
int
zcurrentlinewidth(register ref *op)
{	push(1);
	make_real(op, gs_currentlinewidth(igs));
	return 0;
}

/* setlinecap */
int
zsetlinecap(register ref *op)
{	int param;
	int code = line_param(op, &param);
	if ( !code ) code = gs_setlinecap(igs, (gs_line_cap)param);
	return code;
}

/* currentlinecap */
int
zcurrentlinecap(register ref *op)
{	push(1);
	make_int(op, (int)gs_currentlinecap(igs));
	return 0;
}

/* setlinejoin */
int
zsetlinejoin(register ref *op)
{	int param;
	int code = line_param(op, &param);
	if ( !code ) code = gs_setlinejoin(igs, (gs_line_join)param);
	return code;
}

/* currentlinejoin */
int
zcurrentlinejoin(register ref *op)
{	push(1);
	make_int(op, (int)gs_currentlinejoin(igs));
	return 0;
}

/* setmiterlimit */
int
zsetmiterlimit(register ref *op)
{	return num_param(op, gs_setmiterlimit);
}

/* currentmiterlimit */
int
zcurrentmiterlimit(register ref *op)
{	push(1);
	make_real(op, gs_currentmiterlimit(igs));
	return 0;
}

/* setdash */
int
zsetdash(register ref *op)
{	float offset;
	uint n, i;
	ref *dfrom;
	float *pattern, *dto;
	int code = real_param(op, &offset, 0);
	if ( code ) return code;
	check_array(op[-1]);
	check_read(op[-1]);
	/* Unpack the dash pattern and check it */
	dfrom = op[-1].value.refs;
	i = n = op[-1].size;
	pattern = dto = (float *)alloc(n, sizeof(float), "setdash");
	while ( i-- )
	   {	switch ( r_type(dfrom) )
		   {
		case t_integer:
			*dto++ = dfrom->value.intval;
			break;
		case t_real:
			*dto++ = dfrom->value.realval;
			break;
		default:
			alloc_free((char *)dto, n, sizeof(float), "setdash");
			return e_typecheck;
		   }
		dfrom++;
	   }
	code = gs_setdash(igs, pattern, n, offset);
	if ( !code ) pop(2);
	return code;
}

/* currentdash */
int
zcurrentdash(register ref *op)
{	int n = gs_currentdash_length(igs);
	int i = n;
	ref *pattern = (ref *)alloc(n, sizeof(ref), "currentdash");
	ref *dto = pattern;
	float *dfrom = (float *)((char *)pattern + n * (sizeof(ref) - sizeof(float)));
	gs_currentdash_pattern(igs, dfrom);
	while ( i-- )
	   {	make_real(dto, *dfrom);
		dto++, dfrom++;
	   }
	push(2);
	make_tasv(op - 1, t_array, a_all, n, refs, pattern);
	make_real(op, gs_currentdash_offset(igs));
	return 0;
}

/* setflat */
int
zsetflat(register ref *op)
{	return num_param(op, gs_setflat);
}

/* currentflat */
int
zcurrentflat(register ref *op)
{	push(1);
	make_real(op, gs_currentflat(igs));
	return 0;
}

/* .setgray */
int
zsetgray(register ref *op)
{	return num_param(op, gs_setgray);
}

/* currentgray */
int
zcurrentgray(register ref *op)
{	push(1);
	make_real(op, gs_currentgray(igs));
	return 0;
}

/* sethsbcolor */
int
zsethsbcolor(register ref *op)
{	float par[3];
	int code;
	if (	(code = num_params(op, 3, par)) >= 0 &&
		(code = gs_sethsbcolor(igs, par[0], par[1], par[2])) >= 0
	   )
		pop(3);
	return code;
}

/* currenthsbcolor */
int
zcurrenthsbcolor(register ref *op)
{	float par[3];
	gs_currenthsbcolor(igs, par);
	push(3);
	tri_put(op, par);
	return 0;
}

/* setrgbcolor */
int
zsetrgbcolor(register ref *op)
{	float par[3];
	int code;
	if (	(code = num_params(op, 3, par)) >= 0 &&
		(code = gs_setrgbcolor(igs, par[0], par[1], par[2])) >= 0
	   )
		pop(3);
	return code;
}

/* currentrgbcolor */
int
zcurrentrgbcolor(register ref *op)
{	float par[3];
	gs_currentrgbcolor(igs, par);
	push(3);
	tri_put(op, par);
	return 0;
}

/* settransfer */
int
zsettransfer(register ref *op)
{	check_proc(*op);
	istate.transfer = *op;
	pop(1);
	return 0;
}

/* currenttransfer */
int
zcurrenttransfer(register ref *op)
{	push(1);
	*op = istate.transfer;
	return 0;
}

/* currentscreen */
int
zcurrentscreen(register ref *op)
{	float freq, angle;
	float (*proc)(P2(floatp, floatp));
	gs_currentscreen(igs, &freq, &angle, &proc);
	push(3);
	make_real(op - 2, freq);
	make_real(op - 1, angle);
	*op = istate.screen_proc;
	return 0;
}

/* ------ Initialization procedure ------ */

void
zgstate_op_init()
{	static op_def my_defs[] = {
		{"0currentdash", zcurrentdash},
		{"0currentflat", zcurrentflat},
		{"0currentgray", zcurrentgray},
		{"0currenthsbcolor", zcurrenthsbcolor},
		{"0currentlinecap", zcurrentlinecap},
		{"0currentlinejoin", zcurrentlinejoin},
		{"0currentlinewidth", zcurrentlinewidth},
		{"0currentmiterlimit", zcurrentmiterlimit},
		{"0currentrgbcolor", zcurrentrgbcolor},
		{"0currentscreen", zcurrentscreen},
		{"0currenttransfer", zcurrenttransfer},
		{"0grestore", zgrestore},
		{"0grestoreall", zgrestoreall},
		{"0gsave", zgsave},
		{"0initgraphics", zinitgraphics},
		{"2setdash", zsetdash},
		{"1setflat", zsetflat},
		{"1.setgray", zsetgray},
		{"3sethsbcolor", zsethsbcolor},
		{"1setlinecap", zsetlinecap},
		{"1setlinejoin", zsetlinejoin},
		{"1setlinewidth", zsetlinewidth},
		{"1setmiterlimit", zsetmiterlimit},
		{"3setrgbcolor", zsetrgbcolor},
		{"1settransfer", zsettransfer},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Internal routines ------ */

/* Get a numeric parameter */
private int
num_param(ref *op, int (*pproc)(P2(gs_state *, floatp)))
{	float param;
	int code = real_param(op, &param, 0);
	if ( !code ) code = (*pproc)(igs, param);
	if ( !code ) pop(1);
	return code;
}

/* Get an integer parameter 0-2. */
private int
line_param(register ref *op, int *pparam)
{	check_type(*op, t_integer);
	if ( op->value.intval < 0 || op->value.intval > 2 )
		return e_rangecheck;
	*pparam = (int)op->value.intval;
	pop(1);
	return 0;
}

/* Put 3 reals on the operand stack. */
/* This routine is also used by zcolor.c. */
void
tri_put(register ref *op, float pf3[3])
{	make_real(op - 2, pf3[0]);
	make_real(op - 1, pf3[1]);
	make_real(op, pf3[2]);
}
