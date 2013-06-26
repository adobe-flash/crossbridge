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

/* gzpath.h */
/* Private representation of paths for GhostScript library */
/* Requires gxfixed.h */
#include "gxpath.h"

/* Definition of a path segment: a segment start, a line, */
/* or a Bezier curve. */
typedef enum {
	s_start,
	s_line,
	s_line_close,
	s_curve
} segment_type;
#define segment_common\
	struct segment_s *prev;\
	struct segment_s *next;\
	segment_type type;\
	gs_fixed_point pt;		/* initial point for starts, */\
					/* final point for others */
/* A generic segment */
typedef struct segment_s {
	segment_common
} segment;
/* A start segment.  This serves as the head of a subpath. */
typedef struct {
	segment_common
	segment *last;			/* last segment of subpath, */
					/* points back to here if empty */
	int line_count;			/* # of lines */
	int curve_count;		/* # of curves */
	char closed;			/* true if subpath is closed */
} subpath;
/* Line segments currently have no special data. */
/* s_line is for ordinary lines, s_line_close is for the line */
/* appended by closepath. */
typedef struct {
	segment_common
} line_segment;
/* Curve segments store the control points, not the coefficients. */
/* We may want to change this someday. */
typedef struct {
	segment_common
	gs_fixed_point p1, p2;
} curve_segment;

/* A path is stored as a linked list of segments, */
/* but each path occupies a single contiguous block of memory. */
struct gx_path_s {
	gs_memory_procs memory_procs;
	gs_fixed_rect bbox;		/* bounding box (in device space) */
	segment *box_last;		/* box incorporates segments */
					/* up to & including this one */
	gs_fixed_rect cbox;		/* an inner clipping rectangle */
					/* for a quick check */
	subpath *first_subpath;
	subpath *current_subpath;
	int subpath_count;
	int segment_count;
	int curve_count;
	gs_fixed_point position;	/* current position */
	char position_valid;
	char subpath_open;
	char shares_segments;		/* if true, this path shares its */
					/* segment storage with the one in */
					/* the previous saved graphics state */
};

/* Macros equivalent to a few heavily used procedures. */
/* Be aware that these macros may evaluate arguments more than once. */
#define gx_path_current_point_inline(ppath,ppt)\
 ( !ppath->position_valid ? gs_error_nocurrentpoint :\
   ((ppt)->x = ppath->position.x, (ppt)->y = ppath->position.y, 0) )
#define gx_path_add_relative_point_inline(ppath,dx,dy)\
 ( !ppath->position_valid ? gs_error_nocurrentpoint :\
   (ppath->position.x += dx, ppath->position.y += dy,\
    ppath->subpath_open = 0) )
