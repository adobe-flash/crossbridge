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

/* gxpath2.c */
/* Path tracing procedures for GhostScript library */
#include "math_.h"
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxarith.h"
#include "gzpath.h"

/* Forward declarations */
private int copy_path(P3(gx_path *, gx_path *,
  int (*)(P7(gx_path *, fixed, fixed, fixed, fixed, fixed, fixed))));
private int flatten_curve(P7(gx_path *,
  fixed, fixed, fixed, fixed, fixed, fixed));

/* Read the current point of a path. */
int
gx_path_current_point(gx_path *ppath, gs_fixed_point *ppt)
{	if ( !ppath->position_valid )
	  return_error(gs_error_nocurrentpoint);
	/* Copying the coordinates individually */
	/* is much faster on a PC, and almost as fast on other machines.... */
	ppt->x = ppath->position.x, ppt->y = ppath->position.y;
	return 0;
}

/* Read the bounding box of a path. */
int
gx_path_bbox(gx_path *ppath, gs_fixed_rect *pbox)
{	if ( ppath->first_subpath == 0 )
	   {	/* The path is empty, use the current point if any. */
		gx_path_current_point(ppath, &pbox->p);
		return gx_path_current_point(ppath, &pbox->q);
	   }
	/* The stored bounding box may not be up to date. */
	/* Correct it now if necessary. */
	if ( ppath->box_last == ppath->current_subpath->last )
	   {	/* Box is up to date */
		*pbox = ppath->bbox;
	   }
	else
	   {	gs_fixed_rect box;
		register segment *pseg = ppath->box_last;
		if ( pseg == 0 )	/* box is uninitialized */
		   {	pseg = (segment *)ppath->first_subpath;
			box.p.x = box.q.x = pseg->pt.x;
			box.p.y = box.q.y = pseg->pt.y;
		   }
		else
		   {	box = ppath->bbox;
			pseg = pseg->next;
		   }
/* Macro for adjusting the bounding box when adding a point */
#define adjust_bbox(pt)\
  if ( (pt).x < box.p.x ) box.p.x = (pt).x;\
  else if ( (pt).x > box.q.x ) box.q.x = (pt).x;\
  if ( (pt).y < box.p.y ) box.p.y = (pt).y;\
  else if ( (pt).y > box.q.y ) box.q.y = (pt).y
		while ( pseg )
		   {	switch ( pseg->type )
			   {
			case s_curve:
#define pcurve ((curve_segment *)pseg)
				adjust_bbox(pcurve->p1);
				adjust_bbox(pcurve->p2);
#undef pcurve
				/* falls through */
			default:
				adjust_bbox(pseg->pt);
			   }
			pseg = pseg->next;
		   }
#undef adjust_bbox
		ppath->bbox = box;
		ppath->box_last = ppath->current_subpath->last;
		*pbox = box;
	   }
	return 0;
}

/* Test if a path has any curves. */
int
gx_path_has_curves(gx_path *ppath)
{	return ppath->curve_count != 0;
}

/* Test if a path has any segments. */
int
gx_path_is_void(gx_path *ppath)
{	return ppath->segment_count == 0;
}

/* Test if a path is a rectangle. */
/* If so, return its bounding box. */
int
gx_path_is_rectangle(gx_path *ppath, gs_fixed_rect *pbox)
{	subpath *pseg0;
	if (	ppath->subpath_count == 1 &&
		ppath->segment_count == 4 && ppath->curve_count == 0 &&
		(pseg0 = ppath->first_subpath)->last->type == s_line_close )
	   {	fixed x0 = pseg0->pt.x, y0 = pseg0->pt.y;
		segment *pseg1 = pseg0->next;
		segment *pseg2 = pseg1->next;
		fixed x2 = pseg2->pt.x, y2 = pseg2->pt.y;
		segment *pseg3 = pseg2->next;
		if (	(x0 == pseg1->pt.x && pseg1->pt.y == y2 &&
			 x2 == pseg3->pt.x && pseg3->pt.y == y0) ||
			(x0 == pseg3->pt.x && pseg3->pt.y == y2 &&
			 x2 == pseg1->pt.x && pseg1->pt.y == y0)
		   )
		   {	/* Path is a rectangle.  Return bounding box. */
			if ( x0 < x2 )
				pbox->p.x = x0, pbox->q.x = x2;
			else
				pbox->p.x = x2, pbox->q.x = x0;
			if ( y0 < y2 )
				pbox->p.y = y0, pbox->q.y = y2;
			else
				pbox->p.y = y2, pbox->q.y = y0;
			return 1;
		   }
	   }
	return 0;
}

/* Return the quick-check rectangle for a clipping path. */
/* This only works for paths that have gone through set_clip_path. */
/* which is why the name is different. */
int
gx_cpath_box_for_check(register gx_path *ppath, gs_fixed_rect *pbox)
{	*pbox = ppath->cbox;
	return 0;
}

/* Test if a clipping path includes a rectangle. */
/* The rectangle need not be oriented correctly, i.e. x0 > x1 is OK. */
/* This only works for paths that have gone through set_clip_path. */
/* which is why the name is different. */
int
gx_cpath_includes_rectangle(register gx_path *ppath,
  fixed x0, fixed y0, fixed x1, fixed y1)
{	return
		(x0 <= x1 ?
			(ppath->cbox.p.x <= x0 && x1 <= ppath->cbox.q.x) :
			(ppath->cbox.p.x <= x1 && x0 <= ppath->cbox.q.x)) &&
		(y0 <= y1 ?
			(ppath->cbox.p.y <= y0 && y1 <= ppath->cbox.q.y) :
			(ppath->cbox.p.y <= y1 && y0 <= ppath->cbox.q.y));
}

/* Copy a path */
int
gx_path_copy(gx_path *ppath_old, gx_path *ppath)
{	return copy_path(ppath_old, ppath, gx_path_add_curve);
}

/* Merge a path into its parent (the path in the previous graphics */
/* context).  If ppto is not the parent of ppfrom, chaos may result! */
int
gx_path_merge(gx_path *ppfrom, gx_path *ppto)
{	/* If no new segments, don't release the parent. */
	subpath *psfrom = ppfrom->current_subpath;
	subpath *psto = ppto->current_subpath;
	if ( psto != 0 && psfrom->last != psto->last )
	   {	gx_path_release(ppto);
	   }
	*ppto = *ppfrom;
	ppfrom->shares_segments = 1;
	return 0;
}

/* Translate an already-constructed path (in device space). */
/* Don't bother to translate the cbox. */
int
gx_path_translate(gx_path *ppath, fixed dx, fixed dy)
{	segment *pseg;
#define translate_xy(pt)\
  pt.x += dx, pt.y += dy
	translate_xy(ppath->bbox.p);
	translate_xy(ppath->bbox.q);
	translate_xy(ppath->position);
	pseg = (segment *)(ppath->first_subpath);
	while ( pseg )
	   {	switch ( pseg->type )
		   {
		case s_curve:
		   {	curve_segment *pc = (curve_segment *)pseg;
			translate_xy(pc->p1);
			translate_xy(pc->p2);
		   }
		default:
			translate_xy(pseg->pt);
		   }
		pseg = pseg->next;
	   }
	return 0;
}

/* Define magic quantities used for flatness checking. */
/* Approximate sqrt(1+t*t)*scaled_flat by (1+t*t*0.45)*scaled_flat. */
/* This is good to within about 10% in the range 0<=abs(t)<=1. */
#define sqrt_magic 0.45
#define sqrt1t2xf(t) ((fixed)(t * t * scaled_flat_sq) + scaled_flat)
private fixed scaled_flat;
private float scaled_flat_sq;

/* Flatten a path */
int
gx_path_flatten(gx_path *ppath_old, gx_path *ppath, floatp flatness)
{	/* See the flattening algorithm below for an explanation of */
	/* the following computation. */
	float ff = flatness * ((float)int2fixed(1));
	scaled_flat = ff;
	scaled_flat_sq = ff * sqrt_magic;
	return copy_path(ppath_old, ppath, flatten_curve);
}

/* Copy a path, optionally flattening it. */
/* If the copy fails, free the new path. */
private int
copy_path(gx_path *ppath_old, gx_path *ppath,
  int (*curve_proc)(P7(gx_path *, fixed, fixed, fixed, fixed, fixed, fixed)))
{	gx_path old;
	segment *pseg;
	int code;
#ifdef DEBUG
if ( gs_debug['p'] )
	gx_dump_path(ppath_old, "before copy_path");
#endif
	old = *ppath_old;
	gx_path_init(ppath, &ppath_old->memory_procs);
	pseg = (segment *)(old.first_subpath);
	while ( pseg )
	   {	switch ( pseg->type )
		   {
		case s_start:
			code = gx_path_add_point(ppath, pseg->pt.x, pseg->pt.y);
			break;
		case s_curve:
		   {	curve_segment *pc = (curve_segment *)pseg;
			code = (*curve_proc)(ppath,
					pc->p1.x, pc->p1.y,
					pc->p2.x, pc->p2.y,
					pc->pt.x, pc->pt.y);
			break;
		   }
		case s_line:
			code = gx_path_add_line(ppath, pseg->pt.x, pseg->pt.y);
			break;
		case s_line_close:
			code = gx_path_close_subpath(ppath);
			break;
		   }
		if ( code )
		   {	gx_path_release(ppath);
			if ( ppath == ppath_old ) *ppath_old = old;
			return code;
		   }
		pseg = pseg->next;
	}
	ppath->position = old.position;		/* restore current point */
#ifdef DEBUG
if ( gs_debug['p'] )
	gx_dump_path(ppath, "after copy_path");
#endif
	return 0;
}
/* Internal routine to flatten a curve. */
/* This calls itself recursively, using binary subdivision, */
/* until the approximation is good enough to satisfy the */
/* flatness requirement.  The starting point is ppath->position, */
/* which gets updated as line segments are added. */

#ifdef DEBUG
private void
print_curve_point(fixed x, fixed y)
{	printf("[u]\t*** x=%f, y=%f ***\n", fixed2float(x), fixed2float(y));
}
#endif

private int
flatten_curve(gx_path *ppath,
  fixed x1, fixed y1, fixed x2, fixed y2, fixed x3, fixed y3)
{	fixed
	  x0 = ppath->position.x,
	  y0 = ppath->position.y;
top:
#ifdef DEBUG
if ( gs_debug['u'] )
	printf("[u]x0=%f y0=%f x1=%f y1=%f\n   x2=%f y2=%f x3=%f y3=%f\n",
		fixed2float(x0), fixed2float(y0), fixed2float(x1),
		fixed2float(y1), fixed2float(x2), fixed2float(y2),
		fixed2float(x3), fixed2float(y3));
#endif
	   {	/* Compute the maximum distance of the curve from */
		/* the line (x0,y0)->(x3,y3).  We do this conservatively */
		/* by observing that the curve is enclosed by the */
		/* quadrilateral of its control points, so we simply */
		/* compute the distances of (x1,y1) and (x2,y2) */
		/* from the line.  The distance of (xp,yp) from the line is */
		/* abs(N)/sqrt(D), where N = dy*(xp-x0)-dx*(yp-y0) and */
		/* D = dx*dx+dy*dy.  However, since we are only */
		/* interested in whether this is greater than the flatness */
		/* F, we may as well avoid the square root by testing */
		/* whether N*N > D*F*F, where we can precompute F*F. */
		/* Indeed, we can go further by letting t=dy/dx, and */
		/* testing N1*N1 > D1*F*F, where N1=t*(xp-x0)-(yp-y0) and */
		/* D1 = 1+t*t.  (If dx < dy, we swap x and y for this */
		/* computation, which incidentally guarantees abs(t) <= 1.) */
		/* We can even avoid doing any scaling in converting the */
		/* coordinates and distances from fixed to floating: */
		/* if s is the scale factor for fixed quantities, and we */
		/* disregard it in converting the d, x, and y values to */
		/* floating point, then we actually wind up comparing */
		/* N1*N1*(s^4) > D1*(s^2)*F1, and everything will come out */
		/* if we let F1 = F*F*s^2.  This explains the computation */
		/* of flat_factor above. */
		fixed dx3 = x3 - x0, dy3 = y3 - y0;
		float t;
		fixed d, dist;
		if ( (dx3 < 0 ? -dx3 : dx3) >= (dy3 < 0 ? -dy3 : dy3) )
		   {	if ( dx3 == 0 ) return 0;	/* degenerate */
			t = (float)dy3 / (float)dx3;
			d = sqrt1t2xf(t);
			if ( ((dist = (fixed)(t * (x1 - x0)) - y1 + y0) < 0 ?
			      -dist : dist) <= d &&
			     ((dist = (fixed)(t * (x2 - x0)) - y2 + y0) < 0 ?
			      -dist : dist) <= d
			   )
			   {	/* Curve is flat enough.  Add a line and exit. */
#ifdef DEBUG
if ( gs_debug['u'] )
				print_curve_point(x3, y3);
#endif
				return gx_path_add_line(ppath, x3, y3);
			   }
		   }
		else
		   {	t = (float)dx3 / (float)dy3;
			d = sqrt1t2xf(t);
			if ( ((dist = (fixed)(t * (y1 - y0)) - x1 + x0) < 0 ?
			      -dist : dist) <= d &&
			     ((dist = (fixed)(t * (y2 - y0)) - x2 + x0) < 0 ?
			      -dist : dist) <= d
			   )
			   {	/* Curve is flat enough.  Add a line and exit. */
#ifdef DEBUG
if ( gs_debug['u'] )
				print_curve_point(x3, y3);
#endif
				return gx_path_add_line(ppath, x3, y3);
			   }
		   }
	   }
	/* Curve isn't flat enough.  Break into two pieces and recur. */
	/* Algorithm is from "The Beta2-split: A special case of the */
	/* Beta-spline Curve and Surface Representation," B. A. Barsky */
	/* and A. D. DeRose, IEEE, 1985, courtesy of Crispin Goswell. */
#define midpoint(a,b) arith_rshift((a) +(b), 1)
	   {	fixed x01 = midpoint(x0, x1), y01 = midpoint(y0, y1);
		fixed x12 = midpoint(x1, x2), y12 = midpoint(y1, y2);
		fixed x02 = midpoint(x01, x12), y02 = midpoint(y01, y12);
		int code;
		/* Update x/y1, x/y2, and x/y0 now for the second half. */
		x2 = midpoint(x2, x3), y2 = midpoint(y2, y3);
		x1 = midpoint(x12, x2), y1 = midpoint(y12, y2);
		code = flatten_curve(
			ppath,
			x01, y01, x02, y02,
			(x0 = midpoint(x02, x1)),
			(y0 = midpoint(y02, y1)));
		if ( code < 0 ) return code;
	   }
	goto top;
}
