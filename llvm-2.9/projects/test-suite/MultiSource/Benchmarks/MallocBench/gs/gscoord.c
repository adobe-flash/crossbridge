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

/* gscoord.c */
/* Coordinate system operators for GhostScript library */
#include "math_.h"
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gzstate.h"
#include "gzdevice.h"			/* requires gsstate */
#include "gscoord.h"			/* requires gsmatrix, gsstate */

/* Forward declarations */
#ifdef DEBUG
private void trace_ctm(P1(gs_state *));
private void trace_matrix(P1(gs_matrix *));
#endif

/* Macro for ensuring ctm_inverse is valid */
#ifdef DEBUG
#define print_inverse(pgs)\
if ( gs_debug['x'] )\
	printf("[x]Inverting:\n"), trace_ctm(pgs), trace_matrix(&pgs->ctm_inverse)
#else
#define print_inverse(pgs) 0
#endif
#define ensure_inverse_valid(pgs)\
	if ( !pgs->inverse_valid )\
	   {	int code = gs_matrix_invert(&ctm_only(pgs), &pgs->ctm_inverse);\
		print_inverse(pgs);\
		if ( code < 0 ) return code;\
		pgs->inverse_valid = 1;\
	   }

/* Macro for updating fixed version of ctm */
#define update_ctm(pgs)\
	pgs->ctm.tx_fixed = float2fixed(pgs->ctm.tx),\
	pgs->ctm.ty_fixed = float2fixed(pgs->ctm.ty),\
	pgs->inverse_valid = 0

/* ------ Coordinate system definition ------ */

int
gs_initmatrix(gs_state *pgs)
{	gx_device *dev = pgs->device->info;
	(*dev->procs->get_initial_matrix)(dev, &ctm_only(pgs));
	update_ctm(pgs);
	pgs->char_tm_valid = 0;
#ifdef DEBUG
if ( gs_debug['x'] )
	printf("[x]initmatrix:\n"), trace_ctm(pgs);
#endif
	return 0;
}

int
gs_defaultmatrix(gs_state *pgs, gs_matrix *pmat)
{	gx_device *dev = pgs->device->info;
	(*dev->procs->get_initial_matrix)(dev, pmat);
	return 0;
}

int
gs_currentmatrix(gs_state *pgs, gs_matrix *pmat)
{	*pmat = ctm_only(pgs);
	return 0;
}

int
gs_setmatrix(gs_state *pgs, gs_matrix *pmat)
{	ctm_only(pgs) = *pmat;
	update_ctm(pgs);
	pgs->char_tm_valid = 0;
#ifdef DEBUG
if ( gs_debug['x'] )
	printf("[x]setmatrix:\n"), trace_ctm(pgs);
#endif
	return 0;
}

int
gs_translate(gs_state *pgs, floatp dx, floatp dy)
{	gs_point pt;
	int code;
	if ( (code = gs_distance_transform(dx, dy, &ctm_only(pgs), &pt)) < 0 )
		return code;
	pgs->ctm.tx += pt.x;
	pgs->ctm.ty += pt.y;
	update_ctm(pgs);		/* leaves char_tm valid */
#ifdef DEBUG
if ( gs_debug['x'] )
	printf("[x]translate: %f %f -> %f %f\n",
		dx, dy, pt.x, pt.y),
	trace_ctm(pgs);
#endif
	return 0;
}

int
gs_scale(gs_state *pgs, floatp sx, floatp sy)
{	pgs->ctm.xx *= sx;
	pgs->ctm.xy *= sx;
	pgs->ctm.yx *= sy;
	pgs->ctm.yy *= sy;
	pgs->inverse_valid = 0, pgs->char_tm_valid = 0;
#ifdef DEBUG
if ( gs_debug['x'] )
	printf("[x]scale: %f %f\n", sx, sy), trace_ctm(pgs);
#endif
	return 0;
}

int
gs_rotate(gs_state *pgs, floatp ang)
{	int code = gs_matrix_rotate(&ctm_only(pgs), ang, &ctm_only(pgs));
	pgs->inverse_valid = 0, pgs->char_tm_valid = 0;
#ifdef DEBUG
if ( gs_debug['x'] )
	printf("[x]rotate: %f\n", ang), trace_ctm(pgs);
#endif
	return code;
}

int
gs_concat(gs_state *pgs, gs_matrix *pmat)
{	int code = gs_matrix_multiply(pmat, &ctm_only(pgs), &ctm_only(pgs));
	update_ctm(pgs);
	pgs->char_tm_valid = 0;
#ifdef DEBUG
if ( gs_debug['x'] )
	printf("[x]concat:\n"), trace_matrix(pmat), trace_ctm(pgs);
#endif
	return code;
}

/* ------ Coordinate transformation ------ */

int
gs_transform(gs_state *pgs, floatp x, floatp y, gs_point *pt)
{	return gs_point_transform(x, y, &ctm_only(pgs), pt);
}

int
gs_dtransform(gs_state *pgs, floatp dx, floatp dy, gs_point *pt)
{	return gs_distance_transform(dx, dy, &ctm_only(pgs), pt);
}

int
gs_itransform(gs_state *pgs, floatp x, floatp y, gs_point *pt)
{	ensure_inverse_valid(pgs);
	return gs_point_transform((float)x, (float)y, &pgs->ctm_inverse, pt);
}

int
gs_idtransform(gs_state *pgs, floatp dx, floatp dy, gs_point *pt)
{	ensure_inverse_valid(pgs);
	return gs_distance_transform((float)dx, (float)dy, &pgs->ctm_inverse, pt);
}

/* ------ For internal use only ------ */

/* Set the translation to a given fixed value, */
/* and mark char_tm as valid. */
/* Used by gschar.c to prepare for a BuildChar procedure. */
int
gs_translate_to_fixed(register gs_state *pgs, fixed px, fixed py)
{	pgs->ctm.tx = fixed2float(pgs->ctm.tx_fixed = px);
	pgs->ctm.ty = fixed2float(pgs->ctm.ty_fixed = py);
	pgs->inverse_valid = 0;
	pgs->char_tm_valid = 1;
	return 0;
}

/* ------ Debugging printout ------ */

#ifdef DEBUG

/* Print a matrix */
private void
trace_ctm(gs_state *pgs)
{	gs_matrix_fixed *pmat = &pgs->ctm;
	trace_matrix((gs_matrix *)pmat);
	printf("\t\tt_fixed: [%6g %6g]\n",
		fixed2float(pmat->tx_fixed), fixed2float(pmat->ty_fixed));
}
private void
trace_matrix(register gs_matrix *pmat)
{	printf("\t[%6g %6g %6g %6g %6g %6g]\n",
		pmat->xx, pmat->xy, pmat->yx, pmat->yy, pmat->tx, pmat->ty);
}

#endif
