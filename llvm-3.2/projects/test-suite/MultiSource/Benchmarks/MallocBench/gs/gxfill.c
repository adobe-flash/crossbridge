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

/* gxfill.c */
/* Lower-level path filling procedures for GhostScript library */
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gxdevice.h"			/* for gx_color_index */
#include "gzcolor.h"
#include "gzpath.h"
#include "gzstate.h"

/* Define the structure for keeping track of active lines. */
typedef struct active_line_s {
	gs_fixed_point start;		/* x,y where line starts */
	gs_fixed_point end;		/* x,y where line ends */
#define al_dx(alp) ((alp)->end.x - (alp)->start.x)
#define al_dy(alp) ((alp)->end.y - (alp)->start.y)
	fixed y_fast_max;		/* can do x_at_y in fixed point */
					/* if y <= y_fast_max */
#define set_al_points(alp, startp, endp)\
  (alp)->y_fast_max = max_fixed / (((endp).x > (startp).x ?\
    (endp).x - (startp).x : (startp).x - (endp).x) | 1) + (startp).y,\
  (alp)->start = startp, (alp)->end = endp
#define _xaty(alp, yv)\
  ((yv) - (alp)->start.y) * al_dx(alp) / al_dy(alp)
#define al_x_at_y(alp, yv)\
  ((yv) == (alp)->end.y ? (alp)->end.x :\
   ((yv) <= (alp)->y_fast_max ? _xaty(alp, yv) :\
    (stat(n_slow_x), (fixed)((double)_xaty(alp, yv)))) + (alp)->start.x)
	fixed x_current;		/* current x position */
	fixed x_next;			/* x position at end of band */
	segment *pseg;			/* endpoint of this line */
	int direction;			/* direction of line segment */
#define dir_up 1
#define dir_down (-1)
	short tag;			/* distinguish path from clip path */
	short end_mark;			/* marks lines about to end (even) */
					/* or cross (odd) */
/* "Pending" lines (not reached in the Y ordering yet) use next and prev */
/* to order lines by increasing starting Y.  "Active" lines (being scanned) */
/* use next and prev to order lines by increasing current X, or if the */
/* current Xs are equal, by increasing final X. */
	struct active_line_s
		*prev, *next;
} active_line;

#ifdef DEBUG
/* Internal procedure for printing an active line */
private void
print_active_line(char *label, active_line *alp)
{	printf("[f]%s %lx(%d): x_current=%f x_next=%f\n",
	       label, (ulong)alp, alp->tag, fixed2float(alp->x_current),
	       fixed2float(alp->x_next));
	printf("    start=(%f,%f) pt_end=%lx(%f,%f)\n",
	       fixed2float(alp->start.x), fixed2float(alp->start.y),
	       (ulong)alp->pseg,
	       fixed2float(alp->end.x), fixed2float(alp->end.y));
	printf("    mark=%d dir=%d prev=%lx next=%lx\n",
		alp->end_mark, alp->direction,
		(ulong)alp->prev, (ulong)alp->next);
}
#define print_al(label,alp)\
  if ( gs_debug['F'] ) print_active_line(label, alp)
#else
#define print_al(label,alp) 0
#endif

/* Line list structure */
struct line_list_s {
	active_line *area;		/* allocated area */
	uint area_count;
	short tag;			/* tag for lines being added */
	active_line *next;		/* next allocation slot */
	active_line *y_list;		/* Y-sorted list of pending lines */
	active_line *y_line;		/* most recently inserted line */
	active_line x_head;		/* X-sorted list of active lines */
#define x_list x_head.next
	int no_clip;			/* true if clipping not needed */
};
typedef struct line_list_s line_list;

/* Forward declarations */
private int alloc_line_list(P2(line_list *, int));
private void add_y_list(P4(gx_path *, short, line_list *, gs_fixed_rect *));
private void add_y_line(P4(segment *, segment *, int, line_list *));
private void fill_loop(P6(gx_device_color *, int, line_list *,
  gs_fixed_rect *, gs_state *, fixed));
private void insert_x_new(P2(active_line *, line_list *));
private void swap_group(P1(active_line *));
private void ended_line(P1(active_line *));

/* Statistics */
#ifdef DEBUG
#define stat(x) (x++)
#define statn(x,n) (x += (n))
private long n_fill;
private long n_fill_alloc;
private long n_y_up;
private long n_y_down;
private long n_x_step;
private long n_slow_x;
private long n_iter;
private long n_find_y;
private long n_band;
private long n_band_step;
private long n_band_fill;
#else
#define stat(x) 0
#define statn(x,n) 0
#endif

/* Main filling algorithm. */
/* Caller has done compute_draw_color. */
/* The trim parameter is a hack for keeping small characters */
/* from coming out too blurry: it specifies an amount by which to shrink */
/* all sides of every filled region. */
int
gx_fill_path(gx_path *ppath, gx_device_color *pdevc, gs_state *pgs,
  int rule, fixed trim)
{	gx_path *cpath = pgs->clip_path;
	gx_path *pfpath;
	gx_path ffpath;
	int code;
	line_list lst;
	int max_active;
	/* The closepath is only needed because of the check in */
	/* ended_line as to whether we are the end of a subpath. */
	gx_path_close_subpath(ppath);
	/* Start by flattening the path.  We should do this on-the-fly.... */
	if ( !ppath->curve_count )	/* don't need to flatten */
		pfpath = ppath;
	else
	   {	if ( (code = gx_path_flatten(ppath, &ffpath, pgs->flatness)) < 0 ) return code;
		pfpath = &ffpath;
	   }
	/* may be 1 extra segment per subpath, for closing */
	max_active = pfpath->segment_count + pfpath->subpath_count +
		cpath->segment_count + cpath->subpath_count;
	if ( !(code = alloc_line_list(&lst, max_active)) )
	   {	gs_fixed_rect ibox;
		gx_path_bbox(pfpath, &ibox);
		if ( ibox.q.y <= cpath->cbox.q.y &&
		     ibox.q.x <= cpath->cbox.q.x &&
		     ibox.p.y >= cpath->cbox.p.y &&
		     ibox.p.x >= cpath->cbox.p.x
		   )
		   {	/* Path lies entirely within clipping rectangle */
			lst.no_clip = 1;
		   }
		else
		   {	lst.no_clip = 0;
			/* Intersect the path box and the clip bounding box */
#define int_box(pqxy, rel)\
  if ( cpath->bbox.pqxy rel ibox.pqxy )\
    ibox.pqxy = cpath->bbox.pqxy
			int_box(p.x, >);
			int_box(p.y, >);
			int_box(q.x, <);
			int_box(q.y, <);
#undef int_box
			if ( ibox.p.x >= ibox.q.x || ibox.p.y >= ibox.q.y )
			   {	/* Intersection of boxes is empty! */
				goto skip;
			   }
			add_y_list(cpath, (short)1, &lst, &ibox);
		   }
		add_y_list(pfpath, (short)0, &lst, &ibox);
		fill_loop(pdevc, rule, &lst, &ibox, pgs, trim);
skip:		gs_free((char *)lst.area, lst.area_count, sizeof(active_line), "active lines");
	   }
	if ( pfpath != ppath )	/* had to flatten */
		gx_path_release(pfpath);
#ifdef DEBUG
if ( gs_debug['f'] || gs_debug['F'] )
	   {	printf("[f]calls alloc   up   down  step slowx  iter  find  band bstep bfill\n");
		printf("   %5ld %5ld %5ld %5ld %5ld %5ld %5ld %5ld %5ld %5ld %5ld\n",
			n_fill, n_fill_alloc, n_y_up, n_y_down,
			n_x_step, n_slow_x, n_iter, n_find_y,
			n_band, n_band_step, n_band_fill);
	   }
#endif
	return code;
}

/* Create a line list for a (flattened) path. */
/* We pass in the list size, so that we can use this to iterate */
/* over more than one path simultaneously (needed for clipping). */
private int
alloc_line_list(line_list *ll, int count)
{	ll->area_count = count;
	ll->area = (active_line *)gs_malloc(count, sizeof(active_line),
					    "active lines");
	if ( ll->area == 0 ) return_error(gs_error_VMerror);
	ll->next = ll->area;
	ll->y_list = 0;
	ll->y_line = 0;
	stat(n_fill);
	statn(n_fill_alloc, count);
	return 0;
}

/* Construct a Y-sorted list of lines for a (flattened) path. */
/* We assume the path is non-empty.  Only include non-horizontal */
/* lines where one endpoint is locally Y-minimal. */
private void
add_y_list(gx_path *ppath, short tag, line_list *ll, gs_fixed_rect *pbox)
{	register segment *pseg = (segment *)ppath->first_subpath;
	subpath *psub;
	segment *plast;
	int prev_dir, dir;
	segment *prev;
	fixed xmin = pbox->p.x, ymin = pbox->p.y;
	fixed xmax = pbox->q.x, ymax = pbox->q.y;
	ll->tag = tag;
	while ( pseg )
	   {	fixed dy;
		switch ( pseg->type )
		   {	/* No curves left */
		case s_start:
			psub = (subpath *)pseg;
			plast = psub->last;
			dir = 0;
			break;
		default:		/* s_line, _close */
		   {	fixed iy = pseg->pt.y;
			fixed py = prev->pt.y;
			/* Lines falling entirely outside the ibox */
			/* are treated as though they were horizontal, */
			/* i.e., they are never put on the list. */
#define compute_dir(xo, xe, yo, ye)\
  (xo > xmax && xe > xmax ? 0 :\
   (dy = ye - yo) > 0 ? (ye <= ymin || yo >= ymax ? 0 : dir_up) :\
   dy < 0 ? (yo <= ymin || ye >= ymax ? 0 : dir_down) :\
   0)
			dir = compute_dir(prev->pt.x, pseg->pt.x, py, iy);
			if ( dir > prev_dir )
			   {	if ( prev_dir )
				  add_y_line(prev->prev, prev, prev_dir, ll);
				if ( dir )
				  add_y_line(prev, pseg, dir, ll);
			   }
			if ( pseg == plast )
			   {	/* The beginning of a subpath is always */
				/* treated like a horizontal line, */
				/* so the last segment must receive */
				/* special consideration. */
				if ( pseg->type != s_line_close )
				   {	/* "Close" an open subpath */
					int cdir;
					py = psub->pt.y;
					cdir = compute_dir(pseg->pt.x, psub->pt.x, iy, py);
					if ( cdir > dir && dir )
					  add_y_line(prev, pseg, dir, ll);
					if ( cdir > dir && cdir || cdir < 0 )
					  add_y_line(pseg, (segment *)psub, cdir, ll);
				   }
				else
				   {	/* Just add the closing line if needed */
					if ( dir < 0 )
					  add_y_line(prev, pseg, dir, ll);
				   }
			   }
		   }
#undef compute_dir
		   }
		prev = pseg;
		prev_dir = dir;
		pseg = pseg->next;
	   }
}
/* Internal routine to test a line segment and add it to the */
/* pending list if appropriate. */
private void
add_y_line(segment *prev_lp, segment *lp, int dir, line_list *ll)
{	gs_fixed_point this, prev;
	register active_line *alp = ll->next++;
	fixed y_start;
	this.x = lp->pt.x;
	this.y = lp->pt.y;
	prev.x = prev_lp->pt.x;
	prev.y = prev_lp->pt.y;
	alp->tag = ll->tag;
	if ( (alp->direction = dir) > 0 )
	   {	/* Upward line */
		y_start = prev.y;
		set_al_points(alp, prev, this);
		alp->pseg = lp;
	   }
	else
	   {	/* Downward line */
		y_start = this.y;
		set_al_points(alp, this, prev);
		alp->pseg = prev_lp;
	   }
	/* Insert the new line in the Y ordering */
	   {	register active_line *yp = ll->y_line;
		register active_line *nyp;
		if ( yp == 0 )
		   {	alp->next = alp->prev = 0;
			ll->y_list = alp;
		   }
		else if ( y_start >= yp->start.y )
		   {	/* Insert the new line after y_line */
			while ( stat(n_y_up), (nyp = yp->next) != NULL && y_start > nyp->start.y )
				yp = nyp;
			alp->next = nyp;
			alp->prev = yp;
			yp->next = alp;
			if ( nyp ) nyp->prev = alp;
		   }
		else
		   {	/* Insert the new line before y_line */
			while ( stat(n_y_down), (nyp = yp->prev) != NULL && y_start < nyp->start.y )
				yp = nyp;
			alp->prev = nyp;
			alp->next = yp;
			yp->prev = alp;
			if ( nyp ) nyp->next = alp;
			else ll->y_list = alp;
		   }
	   }
	ll->y_line = alp;
	print_al("add ", alp);
}

/* Find the intersection of two active lines that are known to cross. */
/* Only called if al_x_at_y(alp, y1) < al_x_at_y(endp, y1). */
private fixed
find_cross_y(register active_line *endp, register active_line *alp)
{	double edy = (double)al_dy(endp);
	double ady = (double)al_dy(alp);
#define xprod(alp)\
  ((double)alp->start.x * alp->end.y - (double)alp->start.y * alp->end.x)
#define ycross()\
  (fixed)((xprod(endp) * ady - xprod(alp) * edy) /\
	  (al_dx(alp) * edy - al_dx(endp) * ady))
#ifndef DEBUG
	return ycross();
#else
	fixed cross_y;
	if ( al_dx(alp) * edy - al_dx(endp) * ady == 0 )
	   {	printf("[f]denom == 0!\n");
		gs_debug['F'] = 1;
		print_al(" l1 ", endp);
		print_al(" l2 ", alp);
		exit(1);
	   }
	cross_y = ycross();
if ( gs_debug['F'] )
	printf("[f]cross %lx %lx -> %f\n",
	       (ulong)endp, (ulong)alp,
	       fixed2float(cross_y));
	return cross_y;
#endif
#undef xprod
#undef ycross
}

/* Main filling loop.  Takes lines off of y_list and adds them to */
/* x_list as needed. */
private void
fill_loop(gx_device_color *pdevc, int rule, line_list *ll,
  gs_fixed_rect *pbox, gs_state *pgs, fixed trim)
{	active_line *yll = ll->y_list;
	gs_fixed_point pmax;
	fixed y;
	if ( yll == 0 ) return;		/* empty list */
	pmax = pbox->q;
	y = yll->start.y;		/* first Y value */
	ll->x_list = 0;
	ll->x_head.end_mark = -4;	/* to delimit swap group */
	while ( 1 )
	   {	fixed y1;
		int end_count;
		active_line *endp, *alp, *alstop;
		fixed x;
		stat(n_iter);
		/* Check whether we've reached the maximum y. */
		if ( y >= pmax.y ) break;
		/* Move newly active lines from y to x list. */
		while ( yll != 0 && yll->start.y == y )
		   {	active_line *ynext = yll->next;	/* insert smashes next/prev links */
			insert_x_new(yll, ll);
			yll = ynext;
		   }
		if ( ll->x_list == 0 )
		   {	/* No active lines, skip to next start */
			if ( yll == 0 ) break;	/* no lines left */
			y = yll->start.y;
			continue;
		   }
		/* Find the next evaluation point. */
		/* Start by finding the smallest y value */
		/* at which any currently active line ends */
		/* (or the next to-be-active line begins). */
		y1 = (yll != 0 ? yll->start.y : max_fixed);
		for ( alp = ll->x_list; alp != 0; alp = alp->next )
		  if ( alp->end.y < y1 ) y1 = alp->end.y;
#ifdef DEBUG
if ( gs_debug['F'] )
   {		active_line *lp;
		printf("[f]y=%f y1=%f:\n", fixed2float(y), fixed2float(y1));
		for ( lp = ll->x_list; lp != 0; lp = lp->next )
			printf("[f]%lx: x_current=%f\n", (ulong)lp,
				fixed2float(lp->x_current));
   }
#endif
		/* Now look for line intersections before y1. */
		/* The lines requiring attention at the end of */
		/* band filling are those whose end_mark >= end_count. */
		/* Each time we reset y1 downward, */
		/* we increment end_count by 4 so that we don't */
		/* try to swap or drop lines that haven't */
		/* crossed or ended yet. */
		end_count = 0;
		x = min_fixed;
		alstop = ll->x_list;
		/* Loop invariant: x == al_x_at_y(endp, y1); */
		/* for all lines alp up to alstop, */
		/* alp->x_next = al_x_at_y(alp, y1). */
		for ( alp = alstop; stat(n_find_y), alp != 0;
		     endp = alp, alp = alp->next
		    )
		   {	/* Check for intersecting lines. */
			fixed nx = al_x_at_y(alp, y1);
			alp->x_next = nx;
			if ( nx < x )
			   {	/* stop at intersection */
				y1 = find_cross_y(endp, alp);
				while ( 1 )
				   {	x = al_x_at_y(endp, y1);
					nx = al_x_at_y(alp, y1);
					if ( nx <= x ) break;
					/* This can only result from */
					/* low-order-bit inaccuracy */
					/* in computing the crossing y. */
					/* Bump y by 1. */
					y1++;
				   }
				endp->x_next = x;
				alp->x_next = nx;
				alstop = endp;
				end_count += 4;
				endp->end_mark = end_count + 3;
				alp->end_mark = end_count + 1;
			   }
			else if ( alp->end.y == y1 )	/* can't be < */
			   {	alp->end_mark = end_count;
			   }
			else
				alp->end_mark = -2;
			x = nx;
		   }
		/* Recompute next_x for lines before the intersection. */
		for ( alp = ll->x_list; alp != alstop; alp = alp->next )
			alp->x_next = al_x_at_y(alp, y1);
#ifdef DEBUG
if ( gs_debug['F'] )
   {		active_line *lp;
		printf("[f]y1=%f end_count=%d\n",
			fixed2float(y1), end_count);
		for ( lp = ll->x_list; lp != 0; lp = lp->next )
		  printf("[f]%lx: x_next=%f end_mark=%d\n",
			 (ulong)lp, fixed2float(lp->x_next), lp->end_mark);
   }
#endif
		/* Fill a multi-trapezoid band for the active lines. */
		/* Drop ended lines (with end_mark = end_count) from the */
		/* list.  Reverse the order of groups of adjacent lines */
		/* that intersect at y = y1: the last line of such a group */
		/* has end_mark = end_count+1, the previous ones have */
		/* end_mark = end_count+3. */
		   {	active_line *alp = ll->x_list;
			fixed height = y1 - y;
			fixed xlbot, xltop;	/* as of last "outside" line */
			int inside[2];
			inside[0] = 0;			/* 0 for path */
			inside[1] = ll->no_clip;	/* 1 for clip path */
			stat(n_band);
			/* rule = -1 for winding number rule, i.e. */
			/* we are inside if the winding number is non-zero; */
			/* rule = 1 for even-odd rule, i.e. */
			/* we are inside if the winding number is odd. */
			/* Clever, eh? */
#define inside_path_p() ((inside[0] & rule) && (inside[1] & pgs->clip_rule))
			while ( alp != 0 )
			   {	fixed xbot = alp->x_current;
				fixed xtop = alp->x_next;
				active_line *next = alp->next;
				print_al("step", alp);
				stat(n_band_step);
				if ( inside_path_p() )
				 { inside[alp->tag] += alp->direction;
				   if ( !inside_path_p() )	/* about to go out */
				    {	stat(n_band_fill);
					gz_fill_trapezoid_fixed(xlbot + trim,
					  xbot - xlbot - (trim << 1), y,
					  xltop + trim,
					  xtop - xltop - (trim << 1), height,
					  0, pdevc, pgs);
				    }
				 }
				else			/* outside */
				   {	inside[alp->tag] += alp->direction;
					if ( inside_path_p() )	/* about to go in */
						xlbot = xbot, xltop = xtop;
				   }
				alp->x_current = xtop;
				if ( alp->end_mark >= end_count )
				   {	/* This line is ending here, or */
					/* this is the last of an */
					/* intersection group. */
					switch ( alp->end_mark & 3 )
					   {
					/* case 3: in a group, not last */
					case 1:	/* last line of a group */
						swap_group(alp);
						break;
					case 0:	/* ending line */
						ended_line(alp);
					   }
				   }
				alp = next;
			   }
		   }
		y = y1;
	   }
}

/* Internal routine to insert a newly active line in the X ordering */
private void
insert_x_new(active_line *alp, line_list *ll)
{	register active_line *next;
	register active_line *prev = &ll->x_head;
	register fixed x = alp->start.x;
	while ( stat(n_x_step), (next = prev->next) != 0 &&
		  (next->x_current < x || next->x_current == x &&
		   (next->start.x > alp->start.x ||
		    next->end.x < alp->end.x))
	       )
		prev = next;
	alp->next = next;
	alp->prev = prev;
	if ( next != 0 ) next->prev = alp;
	prev->next = alp;
	alp->x_current = x;
}
/* Auxiliary procedure to swap the lines of an intersection group */
private void
swap_group(active_line *alp)
{	/* This line is the last of the group: */
	/* find the beginning of the group. */
	active_line *prev;
	active_line *last = alp;
	active_line *first;
	active_line *next = alp->next;
	while ( ((prev = alp->prev)->end_mark & 3) == 3 )
		alp = prev;
	first = alp;
#ifdef DEBUG
if ( gs_debug['F'] )
	printf("[f]swap %lx thru %lx\n", (ulong)first, (ulong)last);
#endif
	/* Swap the group end-for-end. */
	/* Start by fixing up the ends. */
	prev->next = last;
	if ( next != 0 )
		next->prev = first;
	first->prev = next;
	last->next = prev;
	do
	   {	active_line *nlp = alp->next;
		alp->next = alp->prev;
		alp->prev = nlp;
		alp = nlp;
	   }
	while ( alp != prev );
}
/* Auxiliary procedure to handle a line that just ended */
private void
ended_line(register active_line *alp)
{	segment *pseg = alp->pseg;
	segment *next;
	fixed y = pseg->pt.y;
	gs_fixed_point npt;
	if ( alp->direction == dir_up )
	   {	/* Upward line, go forward along path */
		next = pseg->next;
		if ( next == 0 || next->type == s_start )
			next = pseg;	/* stop here */
	   }
	else
	   {	/* Downward line, go backward along path */
		next = (pseg->type == s_start ? pseg /* stop here */ : pseg->prev);
	   }
	npt.y = next->pt.y;
#ifdef DEBUG
if ( gs_debug['F'] )
	printf("[f]ended %lx: pseg=%lx y=%f next=%lx npt.y=%f\n",
		(ulong)alp, (ulong)pseg, fixed2float(y),
		(ulong)next, fixed2float(npt.y));
#endif
	if ( npt.y <= y )
	   {	/* End of a line sequence */
		active_line *nlp = alp->next;
		alp->prev->next = nlp;
		if ( nlp ) nlp->prev = alp->prev;
#ifdef DEBUG
if ( gs_debug['F'] )
		printf("[f]drop %lx\n", (ulong)alp);
#endif
	   }
	else
	   {	alp->pseg = next;
		npt.x = next->pt.x;
		set_al_points(alp, alp->end, npt);
		print_al("repl", alp);
	   }
}
