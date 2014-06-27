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

/* gxpath.h */
/* Lower-level path routines for GhostScript library */
/* Requires gxfixed.h */

/* The routines and types in this interface all use */
/* device, rather than user, coordinates, and almost all */
/* use fixed-point, rather than floating, representation. */

/* Opaque type for a path */
typedef struct gx_path_s gx_path;
extern unsigned gs_path_sizeof;

/* Opaque type for a path enumerator */
typedef struct gx_path_enum_s gx_path_enum;
extern unsigned gx_path_enum_sizeof;

/* Define the two insideness rules */
#define gx_rule_winding_number (-1)
#define gx_rule_even_odd 1

/* Path constructors.  Note that the only procedure that involves */
/* a graphics state is gx_path_init. */
extern	void	gx_path_init(P2(gx_path *, gs_memory_procs *)),
		gx_path_release(P1(gx_path *)),
		gx_path_share(P1(gx_path *));
extern	int	gx_path_new_subpath(P1(gx_path *)),
		gx_path_add_point(P3(gx_path *, fixed, fixed)),
		gx_path_add_relative_point(P3(gx_path *, fixed, fixed)),
		gx_path_add_line(P3(gx_path *, fixed, fixed)),
		gx_path_add_rectangle(P5(gx_path *, fixed, fixed, fixed, fixed)),
		gx_path_add_pgram(P7(gx_path *, fixed, fixed, fixed, fixed, fixed, fixed)),
		gx_path_add_curve(P7(gx_path *, fixed, fixed, fixed, fixed, fixed, fixed)),
		gx_path_add_arc(P7(gx_path *, fixed, fixed, fixed, fixed, fixed, fixed)),
		gx_path_close_subpath(P1(gx_path *));

/* Path accessors and transformers */
extern	int	gx_path_current_point(P2(gx_path *, gs_fixed_point *)),
		gx_path_bbox(P2(gx_path *, gs_fixed_rect *)),
		gx_path_has_curves(P1(gx_path *)),
		gx_path_is_void(P1(gx_path *)),
		gx_path_is_rectangle(P2(gx_path *, gs_fixed_rect *)),
		gx_cpath_box_for_check(P2(gx_path *, gs_fixed_rect *)),
		gx_cpath_includes_rectangle(P5(gx_path *, fixed, fixed, fixed, fixed)),
		gx_path_copy(P2(gx_path * /*old*/, gx_path * /*to*/)),
		gx_path_flatten(P3(gx_path * /*old*/, gx_path * /*new*/, floatp)),
		gx_path_merge(P2(gx_path * /*from*/, gx_path * /*to*/)),
		gx_path_translate(P3(gx_path *, fixed, fixed));

/* Debugging routines */
#ifdef DEBUG
extern	void	gx_path_print(P2(FILE *, gx_path *));
#endif
