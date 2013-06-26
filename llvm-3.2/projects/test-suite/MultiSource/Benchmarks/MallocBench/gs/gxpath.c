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

/* gxpath.c */
/* Private path routines for GhostScript library */
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gzpath.h"

/* These routines all assume that all points are */
/* already in device coordinates, and in fixed representation. */
/* As usual, they return either 0 or a (negative) error code. */

/* Forward references */
private subpath *path_alloc_copy(P1(gx_path *));
#ifdef DEBUG
void gx_print_segment(P2(FILE *, segment *));
#endif

/* ------ Initialize/free paths ------ */

/* Initialize a path */
void
gx_path_init(register gx_path *ppath, gs_memory_procs *pprocs)
{	ppath->memory_procs = *pprocs;
	ppath->box_last = 0;
	ppath->position_valid = 0;
	ppath->first_subpath = ppath->current_subpath = 0;
	ppath->subpath_count = 0;
	ppath->segment_count = 0;
	ppath->curve_count = 0;
	ppath->subpath_open = 0;
	ppath->shares_segments = 0;
}

/* Release the contents of a path.  We do this in reverse order */
/* so as to maximize LIFO allocator behavior. */
void
gx_path_release(gx_path *ppath)
{	segment *pseg;
	if ( ppath->first_subpath == 0 ) return;	/* empty path */
	if ( ppath->shares_segments ) return;	/* segments are shared */
	pseg = (segment *)ppath->current_subpath->last;
	while ( pseg )
	   {	segment *prev = pseg->prev;
		unsigned size;
#ifdef DEBUG
if ( gs_debug['p'] )
		printf("[p]release"), gx_print_segment(stdout, pseg);
#endif
		switch ( pseg->type )
		   {
		case s_start: size = sizeof(subpath); break;
		case s_line:
		case s_line_close: size = sizeof(line_segment); break;
		case s_curve: size = sizeof(curve_segment); break;
		default:
			dprintf1("bad type in gx_path_release: %x!\n", pseg->type);
			exit(1);
		   }
#ifdef DEBUG
if ( gs_debug['A'] )
		printf("[p]free %lx<%u>\n", (ulong)pseg, size);
#endif
		(*ppath->memory_procs.free)((char *)pseg, 1, size, "gx_path_release");
		pseg = prev;
	   }
	ppath->first_subpath = 0;	/* prevent re-release */
}

/* Mark a path as shared */
void
gx_path_share(gx_path *ppath)
{	if ( ppath->first_subpath ) ppath->shares_segments = 1;
}

/* ------ Incremental path building ------ */

/* Macro for opening the current subpath. */
/* ppath points to the path; psub has been set to ppath->current_subpath. */
#define path_open()\
	if ( !ppath->subpath_open )\
	   {	int code;\
		if ( !ppath->position_valid )\
		  return_error(gs_error_nocurrentpoint);\
		code = gx_path_new_subpath(ppath);\
		if ( code < 0 ) return code;\
		psub = ppath->current_subpath;\
	   }

/* Macros for allocating path segments. */
/* Note that they assume that ppath points to the path, */
/* and that psub points to the current subpath. */
/* We have to split the macro into two because of limitations */
/* on the size of a single statement (sigh). */
#ifdef DEBUG
#define p_alloc(pseg,size)\
  if ( gs_debug['A'] ) printf("[p]%lx<%u>\n", (ulong)pseg, size)
#else
#define p_alloc(pseg,size) 0
#endif
#define path_unshare(set_psub)\
  if(ppath->shares_segments)\
    if(!(set_psub path_alloc_copy(ppath)))return_error(gs_error_limitcheck)
#define path_alloc_segment(pseg,ctype,stype,cname)\
  path_unshare(psub=);\
  if( !(pseg = (ctype *)(*ppath->memory_procs.alloc)(1, sizeof(ctype), cname)) )\
    return_error(gs_error_limitcheck);\
  p_alloc((char *)pseg, sizeof(ctype));\
  pseg->type = stype, pseg->next = 0
#define path_alloc_link(pseg)\
  { segment *prev = psub->last;\
    prev->next = (segment *)pseg;\
    pseg->prev = prev;\
    psub->last = (segment *)pseg;\
  }

/* Open a new subpath */
int
gx_path_new_subpath(gx_path *ppath)
{	subpath *psub = ppath->current_subpath;
	register subpath *spp;
	path_alloc_segment(spp, subpath, s_start, "gx_path_new_subpath");
	spp->last = (segment *)spp;
	spp->line_count = spp->curve_count = 0;
	spp->closed = 0;
	spp->pt = ppath->position;
	ppath->subpath_open = 1;
	if ( !psub )			/* first subpath */
	   {	ppath->first_subpath = spp;
		spp->prev = 0;
	   }
	else
	   {	segment *prev = psub->last;
		prev->next = (segment *)spp;
		spp->prev = prev;
	   }
	ppath->current_subpath = spp;
	ppath->subpath_count++;
#ifdef DEBUG
if ( gs_debug['p'] )
	printf("[p]"), gx_print_segment(stdout, (segment *)spp);
#endif
	return 0;
}

/* Add a point to the current path (moveto). */
int
gx_path_add_point(register gx_path *ppath, fixed x, fixed y)
{	ppath->subpath_open = 0;
	ppath->position_valid = 1;
	ppath->position.x = x;
	ppath->position.y = y;
	return 0;
}

/* Add a relative point to the current path (rmoveto). */
int
gx_path_add_relative_point(register gx_path *ppath, fixed dx, fixed dy)
{	if ( !ppath->position_valid )
	  return_error(gs_error_nocurrentpoint);
	ppath->subpath_open = 0;
	ppath->position.x += dx;
	ppath->position.y += dy;
	return 0;
}

/* Set the segment point and the current point in the path. */
/* Assumes ppath points to the path. */
#define path_set_point(pseg, fx, fy)\
	(pseg)->pt.x = ppath->position.x = (fx),\
	(pseg)->pt.y = ppath->position.y = (fy)

/* Add a line to the current path (lineto). */
int
gx_path_add_line(gx_path *ppath, fixed x, fixed y)
{	subpath *psub = ppath->current_subpath;
	register line_segment *lp;
	path_open();
	path_alloc_segment(lp, line_segment, s_line, "gx_path_add_line");
	path_alloc_link(lp);
	path_set_point(lp, x, y);
	psub->line_count++;
	ppath->segment_count++;
#ifdef DEBUG
if ( gs_debug['p'] )
	printf("[p]"), gx_print_segment(stdout, (segment *)lp);
#endif
	return 0;
}

/* Add a rectangle to the current path. */
/* This is a special case of adding a parallelogram. */
int
gx_path_add_rectangle(gx_path *ppath, fixed x0, fixed y0, fixed x1, fixed y1)
{	return gx_path_add_pgram(ppath, x0, y0, x0, y1, x1, y1);
}

/* Add a parallelogram to the current path. */
/* This is equivalent to an add_point, three add_lines, */
/* and a close_subpath. */
int
gx_path_add_pgram(gx_path *ppath,
  fixed x0, fixed y0, fixed x1, fixed y1, fixed x2, fixed y2)
{	int code;
 	if (	(code = gx_path_add_point(ppath, x0, y0)) < 0 ||
		(code = gx_path_add_line(ppath, x1, y1)) < 0 ||
		(code = gx_path_add_line(ppath, x2, y2)) < 0 ||
		(code = gx_path_add_line(ppath, x0 + x2 - x1, y0 + y2 - y1)) < 0 ||
		(code = gx_path_close_subpath(ppath)) < 0
	   ) return code;
	return 0;
}

/* Add a curve to the current path (curveto). */
int
gx_path_add_curve(gx_path *ppath,
  fixed x1, fixed y1, fixed x2, fixed y2, fixed x3, fixed y3)
{	subpath *psub = ppath->current_subpath;
	register curve_segment *lp;
	path_open();
	path_alloc_segment(lp, curve_segment, s_curve, "gx_path_add_curve");
	path_alloc_link(lp);
	lp->p1.x = x1;
	lp->p1.y = y1;
	lp->p2.x = x2;
	lp->p2.y = y2;
	path_set_point(lp, x3, y3);
	psub->curve_count++;
	ppath->segment_count++;
	ppath->curve_count++;
#ifdef DEBUG
if ( gs_debug['p'] )
	printf("[p]"), gx_print_segment(stdout, (segment *)lp);
#endif
	return 0;
}

/* Add an approximation of an arc to the current path. */
/* Parameters are the initial and final points of the arc, */
/* and the point at which the extended tangents meet.*/
/* We assume that the arc is less than a semicircle. */
/* The arc may go either clockwise or counterclockwise. */
/* The approximation is a very simple one: a single curve */
/* whose other two control points are .55 of the way to the */
/* intersection of the tangents.  This appears to produce a */
/* very accurate circular arc: I haven't worked out the math */
/* to understand why. */
#define arc_magic 0.55
int
gx_path_add_arc(gx_path *ppath,
  fixed x0, fixed y0, fixed x3, fixed y3, fixed xt, fixed yt)
{	fixed xm = (fixed)(xt * arc_magic), ym = (fixed)(yt * arc_magic);
	return gx_path_add_curve(ppath,
			(fixed)(x0 * (1 - arc_magic)) + xm,
			(fixed)(y0 * (1 - arc_magic)) + ym,
			(fixed)(x3 * (1 - arc_magic)) + xm,
			(fixed)(y3 * (1 - arc_magic)) + ym,
			x3, y3);
}

/* Close the current subpath. */
int
gx_path_close_subpath(gx_path *ppath)
{	subpath *psub = ppath->current_subpath;
	register line_segment *lp;
	if ( !ppath->subpath_open ) return 0;
	/* If the current point is the same as the initial point, */
	/* and the last segment was a s_line, change it to a s_line_close; */
	/* otherwise add a s_line_close segment.  This is just a minor */
	/* efficiency hack, nothing depends on it. */
	if ( ppath->position.x == psub->pt.x &&
	     ppath->position.y == psub->pt.y &&
	     psub->last->type == s_line
		)
	   {	path_unshare(0 !=); /* don't set psub */
		psub->last->type = s_line_close;
		lp = 0;		/* (actually only needed for debugging) */
	   }
	else
	   {	path_alloc_segment(lp, line_segment, s_line_close,
				   "gx_path_close_subpath");
		path_alloc_link(lp);
		path_set_point(lp, psub->pt.x, psub->pt.y);
		psub->line_count++;
		ppath->segment_count++;
	   }
	psub->closed = 1;
	ppath->subpath_open = 0;
#ifdef DEBUG
if ( gs_debug['p'] )
	if ( lp != 0 )
	  printf("[p]"), gx_print_segment(stdout, (segment *)lp);
#endif
	return 0;
}

/* ------ Internal routines ------ */

/* Copy the current path, because it was shared. */
/* Return a pointer to the current subpath, or 0. */
private subpath *
path_alloc_copy(gx_path *ppath)
{	gx_path path_new;
	int code;
	code = gx_path_copy(ppath, &path_new);
	if ( code < 0 ) return 0;
	*ppath = path_new;
	ppath->shares_segments = 0;
	return ppath->current_subpath;
}

/* ------ Debugging printout ------ */

#ifdef DEBUG

/* Print out a path with a label */
void
gx_dump_path(gx_path *ppath, char *tag)
{	printf("[p]Path %lx %s:\n", (ulong)ppath, tag);
	gx_path_print(stdout, ppath);
}

/* Print a path */
void
gx_path_print(FILE *file, gx_path *ppath)
{	segment *pseg = (segment *)ppath->first_subpath;
	fprintf(file, "   subpaths=%d, segments=%d, curves=%d, point=(%f,%f)\n",
		ppath->subpath_count, ppath->segment_count, ppath->curve_count,
		fixed2float(ppath->position.x),
		fixed2float(ppath->position.y));
	fprintf(file, "   box=(%f,%f),(%f,%f) last=%lx\n",
		fixed2float(ppath->bbox.p.x), fixed2float(ppath->bbox.p.y),
		fixed2float(ppath->bbox.q.x), fixed2float(ppath->bbox.q.y),
		(ulong)ppath->box_last);
	fprintf(file, "   cbox=(%f,%f),(%f,%f)\n",
		fixed2float(ppath->cbox.p.x), fixed2float(ppath->cbox.p.y),
		fixed2float(ppath->cbox.q.x), fixed2float(ppath->cbox.q.y));
	while ( pseg )
	   {	gx_print_segment(file, pseg);
		pseg = pseg->next;
	   }
}
void
gx_print_segment(FILE *file, segment *pseg)
{	char out[80];
	sprintf(out, "   %lx<%lx,%lx>: %%s (%6g,%6g) ",
		(ulong)pseg, (ulong)pseg->prev, (ulong)pseg->next,
		fixed2float(pseg->pt.x), fixed2float(pseg->pt.y));
	switch ( pseg->type )
	   {
	case s_start:
#define psub ((subpath *)pseg)
		fprintf(file, out, "start");
		fprintf(file, "#lines=%d #curves=%d last=%lx",
			psub->line_count, psub->curve_count, (ulong)psub->last);
#undef psub
		break;
	case s_curve:
		fprintf(file, out, "curve");
#define pcur ((curve_segment *)pseg)
		fprintf(file, "\n\tp1=(%f,%f) p2=(%f,%f)",
			fixed2float(pcur->p1.x), fixed2float(pcur->p1.y),
			fixed2float(pcur->p2.x), fixed2float(pcur->p2.y));
#undef pcur
		break;
	case s_line:
		fprintf(file, out, "line");
		break;
	case s_line_close:
		fprintf(file, out, "close");
		break;
	default:
	   {	char t[20];
		sprintf(t, "type 0x%x", pseg->type);
		fprintf(file, out, t);
	   }
	   }
	fprintf(file, "\n");
}

#endif					/* DEBUG */
