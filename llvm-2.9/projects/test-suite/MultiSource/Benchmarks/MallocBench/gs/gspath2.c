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

/* gspath2.c */
/* Non-constructor path routines for GhostScript library */
#include "gx.h"
#include "gserrors.h"
#include "gspath.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gzstate.h"
#include "gzpath.h"
#include "gzdevice.h"

/* Forward declarations */
private int common_clip(P2(gs_state *, int));
private int set_clip_path(P3(gs_state *, gx_path *, int));

/* Path enumeration structure */
struct gs_path_enum_s {
	segment *pseg;
	gs_state *pgs;
};

/* Size of path enumeration structure, so clients can allocate */
int gs_path_enum_sizeof = sizeof(gs_path_enum);

/* ------ Path transformers ------ */

int
gs_flattenpath(gs_state *pgs)
{	gx_path fpath;
	int code;
	if ( !pgs->path->curve_count ) return 0;	/* no curves */
	code = gx_path_flatten(pgs->path, &fpath, pgs->flatness);
	if ( code < 0 ) return code;
	gx_path_release(pgs->path);
	*pgs->path = fpath;
	return 0;
}

int
gs_reversepath(gs_state *pgs)
{	return_error(gs_error_undefined);		/* NYI */
}

/* ------ Accessors ------ */

int
gs_pathbbox(gs_state *pgs, gs_rect *pbox)
{	gs_fixed_rect fbox;		/* box in device coordinates */
	gs_rect dbox;
	int code = gx_path_bbox(pgs->path, &fbox);
	if ( code < 0 ) return code;
	/* Transform the result back to user coordinates. */
	dbox.p.x = fixed2float(fbox.p.x);
	dbox.p.y = fixed2float(fbox.p.y);
	dbox.q.x = fixed2float(fbox.q.x);
	dbox.q.y = fixed2float(fbox.q.y);
	return gs_bbox_transform_inverse(&dbox, &ctm_only(pgs), pbox);
}

/* ------ Enumerators ------ */

/* Start enumerating a path */
void
gs_path_enum_init(gs_path_enum *penum, gs_state *pgs)
{	penum->pseg = (segment *)pgs->path->first_subpath;
	penum->pgs = pgs;
}

/* Enumerate the next element of a path. */
/* If the path is finished, return 0; */
/* otherwise, return the element type. */
int
gs_path_enum_next(gs_path_enum *penum, gs_point ppts[3])
{	segment *pseg = penum->pseg;
	gs_state *pgs = penum->pgs;
	gs_point pt;
	int code;
	if ( pseg == 0 ) return 0;	/* finished */
	penum->pseg = pseg->next;
	if ( pseg->type == s_line_close )
	  return gs_pe_closepath;
	if ( (code = gs_itransform(pgs, fixed2float(pseg->pt.x),
				   fixed2float(pseg->pt.y), &pt)) < 0 )
	  return code;
	switch ( pseg->type )
	   {
	case s_start:
	     ppts[0] = pt;
	     return gs_pe_moveto;
	case s_line:
	     ppts[0] = pt;
	     return gs_pe_lineto;
	case s_curve:
#define pcurve ((curve_segment *)pseg)
	     if ( (code =
		   gs_itransform(pgs, fixed2float(pcurve->p1.x),
				 fixed2float(pcurve->p1.y), &ppts[0])) < 0 ||
		  (code =
		   gs_itransform(pgs, fixed2float(pcurve->p2.x),
				 fixed2float(pcurve->p2.y), &ppts[1])) < 0 )
	       return 0;
	     ppts[2] = pt;
	     return gs_pe_curveto;
#undef pcurve
	default:
	     dprintf1("bad type %x in gs_path_enum_next!\n", pseg->type);
	     exit(1);
	   }
}

/* ------ Clipping ------ */

int
gs_clippath(gs_state *pgs)
{	return gx_path_copy(pgs->clip_path, pgs->path);
}

int
gs_initclip(gs_state *pgs)
{	gx_device *dev = pgs->device->info;
	return gx_clip_to_rectangle(pgs, (fixed)0, (fixed)0, int2fixed(dev->width), int2fixed(dev->height));
}

int
gs_clip(gs_state *pgs)
{	return common_clip(pgs, gx_rule_winding_number);
}

int
gs_eoclip(gs_state *pgs)
{	return common_clip(pgs, gx_rule_even_odd);
}

/* ------ Internal routines for clipping ------ */

/* Establish a rectangle as the clipping path. */
/* Used by initclip and by the character cache logic. */
int
gx_clip_to_rectangle(gs_state *pgs, fixed x0, fixed y0, fixed x1, fixed y1)
{	gx_path cpath;
	gx_path *ppath = &cpath;
	int code;
	gx_path_init(ppath, &pgs->memory_procs);
	if ( (code = gx_path_add_rectangle(ppath, x0, y0, x1, y1)) < 0 )
	   {	gx_path_release(ppath);
		return code;
	   }
	gx_path_release(pgs->clip_path);
	return set_clip_path(pgs, ppath, gx_rule_winding_number);
}

/* Main clipping routine.  NOT CORRECT. */
private int
common_clip(gs_state *pgs, int rule)
{	gx_path cpath;
	int code = gx_path_flatten(pgs->path, &cpath, pgs->flatness);
	if ( !code ) code = set_clip_path(pgs, &cpath, rule);
	return code;
}
/* Set the clipping path.  If it is just a rectangle, */
/* set the parameters for the quick clipping check. */
private int
set_clip_path(gs_state *pgs, register gx_path *ppath, int rule)
{	if ( !gx_path_is_rectangle(ppath, &ppath->cbox) )
	   {	/* Not a rectangle, the quick check must fail */
		ppath->cbox.p.x = ppath->cbox.p.y = 0;
		ppath->cbox.q.x = ppath->cbox.q.y = 0;
	   }
	/* Update the outer bounding box as well. */
	gx_path_bbox(ppath, &ppath->bbox);
	*pgs->clip_path = *ppath;
	pgs->clip_rule = rule;
#ifdef DEBUG
if ( gs_debug['p'] )
	printf("[p]Clipping path:\n"), gx_path_print(stdout, pgs->clip_path);
#endif
	return 0;
}
