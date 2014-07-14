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

/* gsstate.c */
/* Miscellaneous graphics state operators for GhostScript library */
#include "gx.h"
#include "memory_.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxmatrix.h"			/* for gzstate */
#include "gzstate.h"
#include "gzdevice.h"
#include "gzcolor.h"			/* requires gxdevice.h */
#include "gzht.h"
#include "gzline.h"
#include "gzpath.h"

/* Size of graphics state object (so clients can copy the top level) */
unsigned gs_state_sizeof = sizeof(gs_state);

/* Forward references */
private int alloc_state_contents(P2(gs_state *, proc_alloc_t));
private void free_state_contents(P1(gs_state *));

/* Allocate and initialize a graphics state */
gs_state *
gs_state_alloc(proc_alloc_t palloc, proc_free_t pfree)
{	register gs_state *pgs = (gs_state *)(*palloc)(1, sizeof(gs_state), "gs_state_alloc");
	if ( pgs == 0 ) return 0;
	if ( alloc_state_contents(pgs, palloc) < 0 ) return 0;
	pgs->saved = 0;
	pgs->memory_procs.alloc = palloc;
	pgs->memory_procs.free = pfree;
	/* Initialize things not covered by initgraphics */
	pgs->path->first_subpath = 0;
	pgs->clip_path->first_subpath = 0;
	pgs->halftone->width = pgs->halftone->height =
		pgs->halftone->order_size = 0;
	gs_nulldevice(pgs);
	gs_setflat(pgs, 1.0);
	/****** What about the font and transfer function ? ******/
	pgs->in_cachedevice = pgs->in_charpath = 0;
	if ( gs_initgraphics(pgs) < 0 )
	   {	/* Something went very wrong */
		return 0;
	   }
	return pgs;
}

/* Free a graphics state */
int
gs_state_free(gs_state *pgs)
{	free_state_contents(pgs);
	(*pgs->memory_procs.free)((char *)pgs, 1, sizeof(gs_state), "gs_state_free");
	return 0;
}

/* Save the graphics state */
int
gs_gsave(gs_state *pgs)
{	gs_state *pnew = (gs_state *)(*pgs->memory_procs.alloc)(1, sizeof(gs_state), "gs_gsave");
	if ( pnew == 0 ) return_error(gs_error_VMerror);
	*pnew = *pgs;
	if ( alloc_state_contents(pgs, pgs->memory_procs.alloc) < 0 ) return_error(gs_error_VMerror);
#define gcopy(element,type)\
    *pgs->element = *pnew->element
	gcopy(path, gx_path);
	gcopy(clip_path, gx_path);
	gcopy(line_params, line_params);
	gcopy(halftone, halftone);
	gcopy(color, gs_color);
	gcopy(dev_color, gx_device_color);
	gcopy(device, device);
#undef gcopy
	gx_path_share(pgs->path);
	gx_path_share(pgs->clip_path);
	pgs->saved = pnew;
	return 0;
}

/* Restore the graphics state */
int
gs_grestore(gs_state *pgs)
{	gs_state *saved = pgs->saved;
	if ( saved == 0 ) return_error(gs_error_undefinedresult);	/* adhoc */
	free_state_contents(pgs);
	*pgs = *saved;
	(*pgs->memory_procs.free)((char *)saved, 1, sizeof(gs_state), "gs_grestore");
	return 0;
}

/* Restore to the bottommost graphics state */
int
gs_grestoreall(gs_state *pgs)
{	while ( gs_grestore(pgs) >= 0 ) ;
	return 0;
}

/* Swap the saved pointer of the graphics state. */
/* This is provided only for save/restore. */
gs_state *
gs_state_swap_saved(gs_state *pgs, gs_state *new_saved)
{	gs_state *saved = pgs->saved;
	pgs->saved = new_saved;
	return saved;
}

/* Reset most of the graphics state */
int
gs_initgraphics(register gs_state *pgs)
{	int code;
	gs_initmatrix(pgs);
	if (	(code = gs_newpath(pgs)) < 0 ||
		(code = gs_initclip(pgs)) < 0 ||
		(code = gs_setlinewidth(pgs, 1.0)) < 0 ||
		(code = gs_setlinecap(pgs, gs_cap_butt)) < 0 ||
		(code = gs_setlinejoin(pgs, gs_join_miter)) < 0 ||
		(code = gs_setdash(pgs, (float *)0, 0, 0.0)) < 0 ||
		(code = gs_setgray(pgs, 0.0)) < 0 ||
		(code = gs_setmiterlimit(pgs, 10.0)) < 0
	   ) return code;
	return 0;
}

/* setflat */
int
gs_setflat(gs_state *pgs, floatp flat)
{	if ( flat <= 0 ) return_error(gs_error_rangecheck);
	pgs->flatness = flat;
	return 0;
}

/* currentflat */
float
gs_currentflat(gs_state *pgs)
{	return pgs->flatness;
}

/* ------ Internal routines ------ */

/* Allocate the contents of a graphics state object. */
/* Return -1 if the allocation fails. */
private int
alloc_state_contents(gs_state *pgs, proc_alloc_t palloc)
{	char *elt;
	static char *cname = "(gs)alloc_state_contents";
#define galloc(element,type)\
    if ( (elt = (*palloc)(1, sizeof(type), cname)) == 0 ) return -1;\
    pgs->element = (type *)elt
	galloc(path, gx_path);
	galloc(clip_path, gx_path);
	galloc(line_params, line_params);
	galloc(halftone, halftone);
	galloc(color, gs_color);
	galloc(dev_color, gx_device_color);
	galloc(device, device);
#undef galloc
	pgs->device_is_shared = 0;
	return 0;
}

/* Free the contents of a graphics state, but not the state itself. */
private void
free_state_contents(gs_state *pgs)
{	proc_free_t pfree = pgs->memory_procs.free;
	static char *cname = "(gs)free_state_contents";
	gx_path_release(pgs->clip_path);
	gx_path_release(pgs->path);
#define gfree(element,type)\
    (*pfree)((char *)pgs->element, 1, sizeof(type), cname)
	if ( !pgs->device_is_shared )
		gfree(device, device);
	gfree(dev_color, gx_device_color);
	gfree(color, gs_color);
	gfree(halftone, halftone);
	gfree(line_params, line_params);
	gfree(clip_path, gx_path);
	gfree(path, gx_path);
#undef gfree
}
