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

/* gxfont.h */
/* Internal font definition for GhostScript library */
/* Requires gsmatrix.h, gxdevice.h */
#include "gsfont.h"

/* A font object as seen by the user. */
/* See pp. 91-93 of the PostScript manual for details. */
typedef int (*gs_proc_build_char)
     (P5(struct gs_show_enum_s *, struct gs_state_s *, struct gs_font_s *,
	 int /* char code */, char * /* build_char_data */));
int gs_no_build_char_proc
     (P5(struct gs_show_enum_s *, struct gs_state_s *, struct gs_font_s *,
	 int, char *));
struct gs_font_s {
	gs_font *next, *prev;		/* chain for scaled font cache */
	gs_font *base;			/* original (unscaled) base font */
	gs_font_dir *dir;		/* directory where registered */
	char *client_data;		/* additional client data */
	gs_matrix matrix;		/* FontMatrix */
	int font_type;			/* FontType */
	float xmin, ymin, xmax, ymax;	/* FontBBox */
	int paint_type;			/* PaintType */
	long unique_id;			/* UniqueID */
	gs_proc_build_char build_char_proc;	/* BuildChar */
	char *build_char_data;		/* private data for BuildChar */
};
