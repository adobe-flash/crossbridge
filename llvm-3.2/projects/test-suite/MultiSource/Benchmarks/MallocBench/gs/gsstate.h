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

/* gsstate.h */
/* Graphics state routines for GhostScript library */

/* Opaque type for a graphics state */
/*typedef struct gs_state_s gs_state;*/	/* define in gs.h */
extern uint gs_state_sizeof;

/* Line cap values */
typedef enum {
	gs_cap_butt = 0,
	gs_cap_round = 1,
	gs_cap_square = 2
} gs_line_cap;

/* Line join values */
typedef enum {
	gs_join_miter = 0,
	gs_join_round = 1,
	gs_join_bevel = 2
} gs_line_join;

/* Allocation, freeing, and initialization */
gs_state *gs_state_alloc(P2(proc_alloc_t, proc_free_t)); /* 0 if fails */
int	gs_state_free(P1(gs_state *));
int	gs_gsave(P1(gs_state *)),
	gs_grestore(P1(gs_state *)),
	gs_grestoreall(P1(gs_state *));
gs_state *gs_state_swap_saved(P2(gs_state *, gs_state *));	/* special for save/restore */
int	gs_initgraphics(P1(gs_state *));
/* Device control */
typedef struct gx_device_s gx_device;
int	gs_flushpage(P1(gs_state *));
int	gs_copypage(P1(gs_state *));
int	gs_copyscanlines(P6(gx_device *, int, byte *, uint, int *, uint *));
gx_device *	gs_getdevice(P1(int));
int	gs_makedevice(P6(gx_device **, gx_device *, gs_matrix *, uint, uint, proc_alloc_t));
int	gs_makeimagedevice(P7(gx_device **, gs_matrix *, uint, uint, float *, int, proc_alloc_t));
void	gs_nulldevice(P1(gs_state *));
int	gs_setdevice(P2(gs_state *, gx_device *));
gx_device *	gs_currentdevice(P1(gs_state *));
char *	gs_devicename(P1(gx_device *));
void	gs_deviceparams(P4(gx_device *, gs_matrix *, int *, int *));

/* Line parameters and quality */
int	gs_setlinewidth(P2(gs_state *, floatp));
float	gs_currentlinewidth(P1(gs_state *));
int	gs_setlinecap(P2(gs_state *, gs_line_cap));
gs_line_cap	gs_currentlinecap(P1(gs_state *));
int	gs_setlinejoin(P2(gs_state *, gs_line_join));
gs_line_join	gs_currentlinejoin(P1(gs_state *));
int	gs_setmiterlimit(P2(gs_state *, floatp));
float	gs_currentmiterlimit(P1(gs_state *));
int	gs_setdash(P4(gs_state *, float *, uint, floatp));
uint	gs_currentdash_length(P1(gs_state *));
int	gs_currentdash_pattern(P2(gs_state *, float *));
float	gs_currentdash_offset(P1(gs_state *));
int	gs_setflat(P2(gs_state *, floatp));
float	gs_currentflat(P1(gs_state *));

/* Color and gray */
typedef struct gs_color_s gs_color;
extern int gs_color_sizeof;
int	gs_setgray(P2(gs_state *, floatp));
float	gs_currentgray(P1(gs_state *));
int	gs_setgscolor(P2(gs_state *, gs_color *)),
	gs_currentgscolor(P2(gs_state *, gs_color *)),
	gs_sethsbcolor(P4(gs_state *, floatp, floatp, floatp)),
	gs_currenthsbcolor(P2(gs_state *, float [3])),
	gs_setrgbcolor(P4(gs_state *, floatp, floatp, floatp)),
	gs_currentrgbcolor(P2(gs_state *, float [3]));

/* Halftone screen */
int	gs_setscreen(P4(gs_state *, floatp, floatp, float (*)(P2(floatp, floatp))));
int	gs_currentscreen(P4(gs_state *, float *, float *, float (**)(P2(floatp, floatp))));
/* Enumeration-style screen definition */
typedef struct gs_screen_enum_s gs_screen_enum;
extern int gs_screen_enum_sizeof;
int	gs_screen_init(P4(gs_screen_enum *, gs_state *, floatp, floatp));
int	gs_screen_currentpoint(P2(gs_screen_enum *, gs_point *));
int	gs_screen_next(P2(gs_screen_enum *, floatp));
