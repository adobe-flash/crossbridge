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

/* gxstroke.c */
/* Path stroking procedures for GhostScript library */
#include "math_.h"
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxarith.h"
#include "gxmatrix.h"
#include "gzstate.h"
#include "gzdevice.h"
#include "gzcolor.h"			/* requires gxdevice.h */
#include "gzline.h"
#include "gzpath.h"

/* stroke_add uses the following global for its path: */
private gx_path stroke_path_body;
private gx_path *stroke_path;

/* Structure for a partial line (passed to the drawing routine). */
/* Two of these are required to do joins right. */
/* Each endpoint includes the two ends of the cap as well, */
/* and the deltas for square and round cap computation. */
typedef struct endpoint_s {
	gs_fixed_point p;		/* the end of the line */
	gs_fixed_point co, ce;		/* ends of the cap */
	gs_fixed_point cdelta;		/* cap_length or -cap_length */
} endpoint;
typedef struct partial_line_s {
	endpoint o;			/* starting coordinate */
	endpoint e;			/* ending coordinate */
	gs_fixed_point width;		/* one-half line width, */
					/* perpendicular (in user space) */
					/* to line */
	gs_fixed_point cap_length;	/* ditto, parallel to line */
	int thin;			/* true if minimum-width line */
} partial_line;

/* Procedures that stroke a partial_line (the first argument). */
/* If both partial_lines are non-null, the procedure creates */
/* an appropriate join; otherwise, the procedure creates an */
/* end cap.  If the first int is 0, the procedure also starts with */
/* an appropriate cap. */
private int stroke_add(P4(int, partial_line *, partial_line *, gs_state *));
private int stroke_fill(P4(int, partial_line *, partial_line *, gs_state *));

/* Other forward declarations */
private int stroke(P3(gx_path *,
  int (*)(P4(int, partial_line *, partial_line *, gs_state *)),
  gs_state *));
private void compute_caps(P2(partial_line *, gs_state *));
private int add_capped(P4(gx_path *, gs_line_cap,
  int (*)(P3(gx_path *, fixed, fixed)),
  endpoint *));

/* Stroke a path for drawing or saving */
int
gx_stroke_fill(gx_path *ppath, gs_state *pgs)
{	int code;
	stroke_path = 0;
	code = stroke(ppath, stroke_fill, pgs);
	if ( stroke_path )		/* set if filling needed */
	  { if ( code >= 0 )
	      code = gx_fill_path(stroke_path, pgs->dev_color, pgs,
				  gx_rule_winding_number, (fixed)0);
		gx_path_release(stroke_path);
	  }
	return code;
}
int
gx_stroke_add(gx_path *ppath, gx_path *topath, gs_state *pgs)
{	stroke_path = topath;
	return stroke(ppath, stroke_add, pgs);
}

/* Stroke a path.  Call line_proc for each line segment. */
private int
stroke(gx_path *ppath,
  int (*line_proc)(P4(int, partial_line *, partial_line *, gs_state *)),
  gs_state *pgs)
{	subpath *psub;
	int code;
	dash_params *dash = &pgs->line_params->dash;
	float *dashes = dash->pattern;
	int dash_count = dash->pattern_size;
	gx_path fpath;
	int skewed = is_skewed(&pgs->ctm);
	float line_width = pgs->line_params->width;
	float xx = pgs->ctm.xx, yy = pgs->ctm.yy;
	int always_thin;
	float line_width_and_scale;
#ifdef DEBUG
if ( gs_debug['o'] )
   {	line_params *lp = pgs->line_params;
	int count = lp->dash.pattern_size;
	int i;
	printf("[o]half_width=%f, cap=%d, join=%d,\n",
		lp->width, (int)lp->cap, (int)lp->join);
	printf("   miter_limit=%f, miter_check=%f,\n",
		lp->miter_limit, lp->miter_check);
	printf("   dash pattern=%d", count);
	for ( i = 0; i < count; i++ )
		printf(",%f", lp->dash.pattern[i]);
	printf(",\n   offset=%f, init(ink_on=%d, index=%d, dist_left=%f)\n",
		lp->dash.offset, lp->dash.init_ink_on, lp->dash.init_index,
		lp->dash.init_dist_left);
   }
#endif
	if ( is_fzero(line_width) )
	   {	always_thin = 1;
		/* Set the line_width to something non-zero -- hack! */
		line_width = 0.01;
	   }
	else if ( skewed )
		always_thin = 0;
	else
	   {	float xxa = xx, yya = yy;
		if ( xxa < 0 ) xxa = -xxa;
		if ( yya < 0 ) yya = -yya;
		always_thin = (max(xxa, yya) * line_width < 0.75);
	   }
	if ( !always_thin )
		line_width_and_scale = line_width * (float)int2fixed(1);
	/* Start by flattening the path.  We should do this on-the-fly.... */
	if ( !ppath->curve_count )	/* don't need to flatten */
	   {	psub = ppath->first_subpath;
		if ( !psub ) return 0;
	   }
	else
	   {	if ( (code = gx_path_flatten(ppath, &fpath, pgs->flatness)) < 0 ) return code;
		psub = fpath.first_subpath;
	   }
	while ( psub )
	{	int segcount = psub->line_count;	/* no curves left */
		line_segment *pline = (line_segment *)(psub->next);
		fixed x = psub->pt.x;
		fixed y = psub->pt.y;
		partial_line pl, pl_prev, pl_first;
		int first = 0;
#define next_line(ex, ey)\
  (pl.o.p.x = x, pl.o.p.y = y, pl.e.p.x = (ex), pl.e.p.y = (ey),\
   (pl.thin ? 0 : (compute_caps(&pl, pgs), 0)),\
   (first++ == 0 ? (pl_first = pl, 0) : 0),\
   (index++ ? (*line_proc)(index - 2, &pl_prev, &pl, pgs) : 0),\
   pl_prev = pl, 0)
		char ink_on = dash->init_ink_on;
		int dash_index = dash->init_index;
		float dist_left = dash->init_dist_left;
		int index = 0;
		while ( segcount-- )
		{	fixed sx = pline->pt.x;
			fixed sy = pline->pt.y;
			/* Compute the width parameters in device space. */
			/* We work with unscaled values, for speed. */
			fixed udx, udy;
			float unscaled_dist;
			if ( sx == x && sy == y ) /* degenerate */
			  goto no_line;
			if ( !always_thin || dash_count )
			   {	gs_point dpt;	/* unscaled */
				udx = sx - x, udy = sy - y;
				if ( skewed )
				  gs_idtransform(pgs,
				    (float)udx, (float)udy, &dpt);
				else	/* shortcut */
				   {	dpt.x = udx / xx;
					dpt.y = udy / yy;
				   }
				unscaled_dist =
				  sqrt(dpt.x * dpt.x + dpt.y * dpt.y);
				if ( !always_thin )
				   {	float wl = line_width_and_scale /
					  unscaled_dist;
					/* Construct the width vector in */
					/* user space, still unscaled. */
					dpt.x *= wl;
					dpt.y *= wl;
					/* We now compute both parallel */
					/* and perpendicular half-widths, */
					/* as deltas in device space. */
					/* We use a fixed-point, unscaled */
					/* version of gs_dtransform. */
					/* The second computation folds in */
					/* a 90-degree rotation counter- */
					/* clockwise (in user space, */
					/* before transforming). */
					pl.width.x =
						-(fixed)(dpt.y * xx);
					pl.width.y =
						(fixed)(dpt.x * yy);
					if ( skewed )
						pl.width.x +=
						 (fixed)(dpt.x * pgs->ctm.yx),
						pl.width.y -=
						 (fixed)(dpt.y * pgs->ctm.xy);
					pl.thin =
					  ((pl.width.x < 0 ? -pl.width.x :
					    pl.width.x) +
					   (pl.width.y < 0 ? -pl.width.y :
					    pl.width.y)) < float2fixed(0.75);
					if ( pl.thin )
					   {	pl.width.x = pl.width.y = 0;
						pl.cap_length.x =
						  pl.cap_length.y = 0;
					   }
					else
					   {	pl.cap_length.x =
						 (fixed)(dpt.x * xx);
						pl.cap_length.y =
						 (fixed)(dpt.y * yy);
						if ( skewed )
						  pl.cap_length.x +=
						 (fixed)(dpt.y * pgs->ctm.yx),
						  pl.cap_length.y +=
						 (fixed)(dpt.x * pgs->ctm.xy);
					   }
				   }
				else
					pl.width.x = pl.width.y = 0,
					pl.cap_length.x = pl.cap_length.y = 0,
					pl.thin = 1;
			   }
			else
				pl.width.x = pl.width.y = 0,
				pl.cap_length.x = pl.cap_length.y = 0,
				pl.thin = 1;
			if ( dash_count )
			   {	/* Account for dash pattern */
				float dist_total =
				  unscaled_dist * (1 / (float)int2fixed(1));
				float dist = dist_total;
				float dx = udx, dy = udy;	/* unscaled */
				while ( dist > dist_left )
				   {	/* We are using up the dash element */
					float fraction = dist_left / dist_total;	/* unscaled-inverse */
					fixed nx = x + (fixed)(dx * fraction);
					fixed ny = y + (fixed)(dy * fraction);
					if ( ink_on )
						next_line(nx, ny);
					dist -= dist_left;
					if ( !(ink_on = !ink_on) )
					   {	if ( index )
						   {	(*line_proc)(index - 1, &pl_prev, (partial_line *)0, pgs);
							index = 0;
						   }
					   }
					if ( ++dash_index == dash_count )
						dash_index = 0;
					dist_left = dashes[dash_index];
					x = nx, y = ny;
				   }
				if ( ink_on )
					next_line(sx, sy);
				dist_left -= dist;
			   }
			else
				next_line(sx, sy);
no_line:		pline = (line_segment *)(pline->next);
			x = sx, y = sy;
		}
		if ( index )
		   {	/* If closed, join back to start, else cap */
			(*line_proc)(index - 1, &pl_prev,
				(psub->closed && ink_on && dash->init_ink_on ?
				 &pl_first : (partial_line *)0), pgs);
		   }
		psub = (subpath *)pline;
		if ( stroke_path == &stroke_path_body )
		   {	/* Fill and release the accumulated path */
			gx_fill_path(stroke_path, pgs->dev_color, pgs,
				     gx_rule_winding_number, (fixed)0);
			gx_path_release(stroke_path);
			stroke_path = 0;
		   }
	}
	if ( ppath->curve_count ) gx_path_release(&fpath);
	return 0;
}

/* ------ Internal routines ------ */

/* Compute the intersection of two lines.  This is a messy algorithm */
/* that somehow ought to be useful in more places than just here.... */
private void
line_intersect(
    gs_fixed_point *pp1,		/* point on 1st line */
    gs_fixed_point *pd1,		/* slope of 1st line (dx,dy) */
    gs_fixed_point *pp2,		/* point on 2nd line */
    gs_fixed_point *pd2,		/* slope of 2nd line */
    gs_fixed_point *pi)			/* return intersection here */
{	/* We don't have to do any scaling, the factors all work out right. */
	float x1 = pp1->x, y1 = pp1->y;
	float u1 = pd1->x, v1 = pd1->y;
	float x2 = pp2->x, y2 = pp2->y;
	float u2 = pd2->x, v2 = pd2->y;
	double denom = u1 * v2 - u2 * v1;
	double num1 = v1 * x1 - u1 * y1;
	double num2 = v2 * x2 - u2 * y2;
	pi->x = (fixed)( (u1 * num2 - u2 * num1) / denom );
	pi->y = (fixed)( (v1 * num2 - v2 * num1) / denom );
#ifdef DEBUG
if ( gs_debug['o'] )
   {	printf("[o]Intersect %f,%f(%f/%f) & %f,%f(%f/%f) ->\n",
		fixed2float(pp1->x), fixed2float(pp1->y),
		fixed2float(pd1->x), fixed2float(pd1->y),
		fixed2float(pp2->x), fixed2float(pp2->y),
		fixed2float(pd2->x), fixed2float(pd2->y));
	printf("	%f,%f\n",
		fixed2float(pi->x), fixed2float(pi->y));
   }
#endif
}

#define lix plp->o.p.x
#define liy plp->o.p.y
#define litox plp->e.p.x
#define litoy plp->e.p.y

/* Draw a line on the device. */
private int
stroke_fill(int first,
  register partial_line *plp, partial_line *nplp, gs_state *pgs)
{	if ( plp->thin )
	   {	/* Minimum-width line, don't have to be careful. */
		/* We do have to check for the entire line being */
		/* within the clipping rectangle. */
		if ( gx_cpath_includes_rectangle(pgs->clip_path,
				lix, liy, litox, litoy) )
			return gz_draw_line_fixed(lix, liy, litox, litoy,
				pgs->dev_color, pgs);
		/* We didn't set up the endpoint parameters before, */
		/* because the line was thin.  Do it now. */
		/****** THE FOLLOWING IS A HACK. IT IS PROBABLY WRONG. ******/
		plp->width.x = float2fixed(0.25);
		plp->width.y = float2fixed(0.25);
		plp->cap_length.x = float2fixed(0.125);
		plp->cap_length.y = float2fixed(0.125);
		compute_caps(plp, pgs);
	   }
	   {	/* General case. */
		/* Construct a path and hand it to the fill algorithm. */
		if ( stroke_path == 0 )
		   {	/* We are rendering, and haven't run into the */
			/* general case yet.  Initialize the path. */
			stroke_path = &stroke_path_body;	/* set global for stroke_add */
			gx_path_init(stroke_path, &pgs->memory_procs);
		   }
		stroke_add(first, plp, nplp, pgs);
		   {	/****** PATCH ******/
			if ( stroke_path == &stroke_path_body )
			   {	gx_fill_path(stroke_path, pgs->dev_color, pgs,
					     gx_rule_winding_number, (fixed)0);
				gx_path_release(stroke_path);
				stroke_path = 0;
			   }
		   }
	   }
	return 0;
}

#undef lix
#undef liy
#undef litox
#undef litoy

/* Add a segment to the path.  This handles all the complex cases. */
private int add_capped(P4(gx_path *, gs_line_cap, int (*)(P3(gx_path *, fixed, fixed)), endpoint *));
private int
stroke_add(int first,
  register partial_line *plp, partial_line *nplp, gs_state *pgs)
{	gx_path *ppath = stroke_path;
	int code;
	if ( ppath == 0 ) return 0;	/****** strokepath is NYI ******/
	if ( plp->thin )
	   {	/* We didn't set up the endpoint parameters before, */
		/* because the line was thin.  Do it now. */
		compute_caps(plp, pgs);
	   }
	if ( (code = add_capped(ppath, (first == 0 ? pgs->line_params->cap : gs_cap_butt), gx_path_add_point, &plp->o)) < 0 )
		return code;
	if ( nplp == 0 )
	   {	code = add_capped(ppath, pgs->line_params->cap, gx_path_add_line, &plp->e);
	   }
	else if ( pgs->line_params->join == gs_join_round )
	   {	code = add_capped(ppath, gs_cap_round, gx_path_add_line, &plp->e);
	   }
	else if ( nplp->thin )		/* no join */
	  {	code = add_capped(ppath, gs_cap_butt, gx_path_add_line, &plp->e);
	  }
	else				/* join_bevel or join_miter */
	   {	gs_fixed_point jp1, jp2;
		/* Set np to whichever of nplp->o.co or .ce */
		/* is outside the current line, i.e., */
		/* whichever is on the same side as the cap. */
		float fwx = plp->width.x, fwy = plp->width.y;
		float fdx = plp->e.cdelta.x, fdy = plp->e.cdelta.y;
		float fnwx = nplp->width.x, fnwy = nplp->width.y;
		int cside = (fdx * fwy >= fdy * fwx);
		int nside = (fnwx * fwy >= fnwy * fwx);
		gs_fixed_point *np = (cside == nside ? &nplp->o.ce : &nplp->o.co);
#ifdef DEBUG
if ( gs_debug['o'] )
		printf("[o]e.cd-side=%d nw/side=%d\n", cside, nside);
#endif
		/* Compute the join point. */
		/* Initialize it for a bevel join. */
		jp1.x = plp->e.co.x, jp1.y = plp->e.co.y;
		jp2.x = plp->e.ce.x, jp2.y = plp->e.ce.y;
		if ( pgs->line_params->join == gs_join_miter )
		  { /* Check whether a miter join is appropriate. */
		    /* Let a, b be the angles of the two lines. */
		    /* We check tan(a-b) against the miter_check */
		    /* by using the following formula: */
		    /* If tan(a)=u1/v1 and tan(b)=u2/v2, then */
		    /* tan(a-b) = (u1*v2 - u2*v1) / (u1*u2 + v1*v2). */
		    /* We can do all the computations unscaled, */
		    /* because we're only concerned with ratios. */
		    fixed u1 = plp->e.cdelta.x, v1 = plp->e.cdelta.y;
		    fixed u2 = nplp->e.cdelta.x, v2 = nplp->e.cdelta.y;
		    float uf1, vf1, uf2, vf2;
		    float num, denom;
		    float check = pgs->line_params->miter_check;
		    /* We will want either tan(pi-a+b) or tan(pi-b+a)
		     * depending on the orientations of the lines.
		     *
		     * tan(pi - a + b) = -tan(a-b)
		     * tan(pi - b + a) = tan(a-b)
		     *
		     * First reflect both lines so that 1st
		     * line is in 1st quadrant.
		     */
		    if ( u1 < 0 ) u1 = -u1, u2 = -u2;
		    if ( v1 < 0 ) v1 = -v1, v2 = -v2;
		    uf1 = u1, vf1 = v1, uf2 = u2, vf2 = v2;
		    num = uf2 * vf1 - uf1 * vf2;
		    denom = uf1 * uf2 + vf1 * vf2;
		    /* Now proceed based on the angle of the 2nd line */
		    if ( u2 >= 0 )
		      { if ( v2 >= 0 )
			  { /* 1st quadrant => negative tangent */
			    if ( num > 0 ) num = -num;
			    if ( denom < 0 ) denom = -denom;
			  }
		        else
			  { /* 4th quadrant => use tan(b-a) */
			    num = -num;
			    if ( denom < 0 ) num = -num, denom = -denom;
			  }
		      }
		    else
		      { if ( v2 >= 0 )
			  { /* 2nd quadrant => use tan(a-b) */
			    if ( denom < 0 ) num = -num, denom = -denom;
			  }
		        else
			  { /* 3rd quadrant => positive tangent */
			    if ( num < 0 ) num = -num;
			    if ( denom < 0 ) denom = -denom;
			  }
		      }
#ifdef DEBUG
if ( gs_debug['o'] )
                   {    printf("[o]Miter check: u1/v1=%f/%f, u2/v2=%f/%f,\n",
			       uf1, vf1, uf2, vf2);
                        printf("        num=%f, denom=%f, check=%f\n",
			       num, denom, check);
                   }
#endif
		    if ( num >= 0 ?
                        (check > 0 && num >= denom * check) :
                        (check > 0 || num >= denom * check) )
			   {	/* OK to use a miter join. */
				/* Compute the intersection of */
				/* the extended edge lines. */
				/* Start by discovering which of */
				/* plp->e.co or .ce is outside */
				/* the next line.  fwx and fwy are negated */
				/* because the e-line goes in the opposite */
				/* direction from the o-line. */
				float fndx = nplp->o.cdelta.x,
					fndy = nplp->o.cdelta.y;
				int noside = (fndx * fnwy >= fndy * fnwx);
				int wside = (-fwx * fnwy >= -fwy * fnwx);
#ifdef DEBUG
if ( gs_debug['o'] )
				printf("[o]no.cd-side=%d, w-side=%d\n",
					noside, wside);
#endif
				if ( noside == wside )
					line_intersect(&plp->e.ce, &plp->e.cdelta, np, &nplp->o.cdelta, &jp2);
				else
					line_intersect(&plp->e.co, &plp->e.cdelta, np, &nplp->o.cdelta, &jp1);
			   }
		   }
		if (	(code = gx_path_add_line(ppath, jp1.x, jp1.y)) < 0 ||
			(code = gx_path_add_line(ppath, np->x, np->y)) < 0 ||
			(code = gx_path_add_line(ppath, jp2.x, jp2.y)) < 0
		   ) return code;
	   }
	if ( code < 0 || (code = gx_path_close_subpath(ppath)) < 0 )
		return code;
	return 0;
}

/* Routines for cap computations */

/* Compute the endpoints of the two caps of a segment. */
private void
compute_caps(register partial_line *plp, gs_state *pgs)
{	fixed wx2 = plp->width.x;
	fixed wy2 = plp->width.y;
#define lx2 plp->cap_length.x
#define ly2 plp->cap_length.y
	plp->o.co.x = plp->o.p.x - wx2, plp->o.co.y = plp->o.p.y - wy2;
	plp->o.ce.x = plp->o.p.x + wx2, plp->o.ce.y = plp->o.p.y + wy2;
	plp->e.co.x = plp->e.p.x + wx2, plp->e.co.y = plp->e.p.y + wy2;
	plp->e.ce.x = plp->e.p.x - wx2, plp->e.ce.y = plp->e.p.y - wy2;
	plp->o.cdelta.x = -lx2, plp->o.cdelta.y = -ly2;
	plp->e.cdelta.x = lx2, plp->e.cdelta.y = ly2;
#ifdef DEBUG
if ( gs_debug['o'] )
	printf("[o]Stroke o=(%f,%f) e=(%f,%f)\n",
		fixed2float(plp->o.p.x), fixed2float(plp->o.p.y),
		fixed2float(plp->e.p.x), fixed2float(plp->e.p.y)),
	printf("	wxy=(%f,%f) lxy=(%f,%f)\n",
		fixed2float(wx2), fixed2float(wy2),
		fixed2float(lx2), fixed2float(ly2));
#endif
#undef lx2
#undef ly2
}

/* Add a properly capped line endpoint to the path. */
/* The first point may require either moveto or lineto. */
private int
add_capped(gx_path *ppath, gs_line_cap type,
  int (*add_proc)(P3(gx_path *, fixed, fixed)), /* gx_path_add_point/line */
  register endpoint *endp)
{	int code;
#define px endp->p.x
#define py endp->p.y
#define xo endp->co.x
#define yo endp->co.y
#define xe endp->ce.x
#define ye endp->ce.y
#define cdx endp->cdelta.x
#define cdy endp->cdelta.y
#ifdef DEBUG
if ( gs_debug['o'] )
	printf("[o]cap: p=(%g,%g), co=(%g,%g), ce=(%g,%g), cd=(%g,%g)\n",
		fixed2float(px), fixed2float(py),
		fixed2float(xo), fixed2float(yo),
		fixed2float(xe), fixed2float(ye),
		fixed2float(cdx), fixed2float(cdy));
#endif
	switch ( type )
	   {
	case gs_cap_round:
	   {	fixed xm = px + cdx;
		fixed ym = py + cdy;
		if (	(code = (*add_proc)(ppath, xo, yo)) < 0 ||
			(code = gx_path_add_arc(ppath, xo, yo, xm, ym,
				xo + cdx, yo + cdy)) < 0 ||
			(code = gx_path_add_arc(ppath, xm, ym, xe, ye,
				xe + cdx, ye + cdy)) < 0
		   ) return code;
	   }
		break;
	case gs_cap_square:
		if (	(code = (*add_proc)(ppath, xo + cdx, yo + cdy)) < 0 ||
			(code = gx_path_add_line(ppath, xe + cdx, ye + cdy)) < 0
		   ) return code;
		break;
	case gs_cap_butt:
		if (	(code = (*add_proc)(ppath, xo, yo)) < 0 ||
			(code = gx_path_add_line(ppath, xe, ye)) < 0
		   ) return code;
	   }
	return code;
}
