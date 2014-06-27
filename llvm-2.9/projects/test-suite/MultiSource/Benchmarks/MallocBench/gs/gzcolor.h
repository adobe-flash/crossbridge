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

/* gzcolor.h */
/* Private definition of color representation for GhostScript */

/* Representation of color. */
/* All the color parameters are stored internally as color_params. */
typedef unsigned short color_param;
/*
 * The following line should read:
 *	#define max_color_param max_ushort
 * but this seems to trigger brain-damage in a number of compilers,
 * so instead we use a variable:
 */
static const color_param max_color_param = 0xffff;
#define max_color_param_long 0xffffL
/* Expand a byte to a color_param without doing a multiply. */
#define color_param_from_byte(b) (((ushort)(b) << 8) + (b))

/* The following parameters are computed from the above, */
/* just before actually using the color to draw. */
typedef struct gx_device_color_s gx_device_color;
struct gx_device_color_s {
	gx_color_index color1;		/* device color, or */
					/* darker color for halftoning */
	gx_color_index color2;		/* lighter color for halftoning */
	int halftone_level;		/* number of spots to whiten */
					/* when halftoning, 0 if */
					/* halftoning not needed, */
					/* <0 if color halftone */
	struct gx_bitmap_s *tile;	/* pointer to cached halftone */
};
#define color_is_pure(pdevc)\
  ((pdevc)->halftone_level == 0)
#define color_is_color_halftone(pdevc)\
  ((pdevc)->halftone_level < 0)

/*typedef struct gs_color_s gs_color;*/	/* in gsstate.h */
struct gs_color_s {
	color_param red, green, blue;		/* rgb representation */
	color_param luminance;		/* computed luminance */
	byte is_gray;			/* quick test for gray */
					/* (red==green==blue) */
	byte luminance_set;		/* true if luminance is set */
};
extern color_param gx_color_luminance(P1(struct gs_color_s *));
#define color_luminance(pcolor)\
	((pcolor)->luminance_set ? (pcolor)->luminance :\
	 gx_color_luminance(pcolor))

/* Color weights in 100ths, used for computing luminance. */
#define lum_red_weight	30
#define lum_green_weight	59
#define lum_blue_weight	11
#define lum_all_weights	(lum_red_weight + lum_green_weight + lum_blue_weight)
