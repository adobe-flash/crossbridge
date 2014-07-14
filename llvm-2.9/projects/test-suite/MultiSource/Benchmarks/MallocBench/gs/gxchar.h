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

/* gxchar.h */
/* Internal character definition for GhostScript library */
/* Requires gsmatrix.h, gxfixed.h, gzdevice.h */
#include "gschar.h"

/* An entry for a (font,matrix) pair in the character cache. */
typedef struct cached_fm_pair_s cached_fm_pair;
struct cached_fm_pair_s {
	struct gs_font_s *font;		/* base font */
	float mxx, mxy, myx, myy;	/* transformation */
	int num_chars;			/* # of cached chars with this */
					/* f/m pair */
};

/* A cached bitmap for an individual character. */
typedef struct cached_char_s cached_char;
struct cached_char_s {
	cached_char *next;		/* next in replacement ring */
	/* The code and font/matrix pair are the 'key' in the cache. */
	int code;			/* character code */
	cached_fm_pair *pair;		/* font/matrix pair */
	/* The rest of the structure is the 'value'. */
	unsigned short raster, height;	/* dimensions of bitmap */
	unsigned short width;
	gs_fixed_point wxy;		/* width in device coords */
	gs_fixed_point offset;		/* (-llx, -lly) in device coords */
	byte *bits;
};

/* An enumeration object for string display. */
typedef enum {
	sws_none,
	sws_cache,
	sws_no_cache
} show_width_status;
struct gs_show_enum_s {
	/* Following are set at creation time */
	gs_state *pgs;
	byte *str;			/* char may be signed! */
	uint size;
	float cx, cy;			/* for widthshow */
	int chr;			/* ditto */
	float ax, ay;			/* for ashow */
	int add;			/* true if a[width]show */
	int do_kern;			/* true if kshow */
	int slow_show;			/* [a][width]show or kshow */
	int charpath_flag;		/* 0 for show, 1 for false */
					/* charpath, 2 for true charpath */
	int can_cache;			/* true if can cache chars */
	int cxmin, cymin, cxmax, cymax;	/* int version of quick-check */
					/* clipping box */
	/* Following are set at most once */
	gx_device_memory dev_cache_info;
	device dev_cache_dev;
	int dev_cache_set;
	/* Following are updated dynamically */
	uint index;			/* index within string */
	gs_fixed_point wxy;		/* for current char in device coords */
	cached_char *cc;		/* being accumulated */
	gs_point width;			/* total width of string, set at end */
	show_width_status width_set;
	int color_loaded;		/* true if have done gx_color_render */
	int (*continue_proc)(P1(struct gs_show_enum_s *));	/* continuation procedure */
};

/* Cached character procedures (in gxfont.c) */
	/* The following line is needed to make the GNU compiler happy.... */
	struct gs_font_dir_s;
cached_char *	gx_alloc_char_bits(P4(struct gs_font_dir_s *,
			gx_device_memory *, ushort, ushort));
void	gx_unalloc_cached_char(P2(struct gs_font_dir_s *, cached_char *));
cached_fm_pair *	gx_lookup_fm_pair(P1(gs_state *));
void	gx_add_cached_char(P3(struct gs_font_dir_s *, cached_char *, cached_fm_pair *));
cached_char *	gx_lookup_cached_char(P3(gs_state *, cached_fm_pair *, int));
int	gx_copy_cached_char(P2(gs_show_enum *, cached_char *));
