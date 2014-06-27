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

/* gdevsun.c */
/* (monochrome) SunView driver for GhostScript library */
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <stdio.h>
/* Hack to get around the fact that something in the SunView library */
/* defines uint and ushort.... */
#  define uint _uint
#  define ushort _ushort
#include "gx.h"			/* for gx_bitmap; includes std.h */
#  undef _uint
#  undef _ushort
#include "gsmatrix.h"			/* needed for gxdevice.h */
#include "gxdevice.h"
typedef struct gx_device_s gx_device;

#ifndef DEFAULT_DPI
#  define DEFAULT_DPI 75		/* Sun standard monitor */
#endif

/* Procedures */
int sun_open(P1(gx_device *));
int sun_close(P1(gx_device *));
int sun_sync(P1(gx_device *));
int sun_fill_rectangle(P6(gx_device *, int, int, int, int, gx_color_index));
int sun_copy_mono(P10(gx_device *, byte *, int, int, int, int, int, int,
  gx_color_index, gx_color_index));
int sun_copy_color(P8(gx_device *, byte *, int, int, int, int, int, int));
int sun_draw_line(P6(gx_device *, int, int, int, int, gx_color_index));

/* The device descriptor */
private gx_device_procs sun_procs = {
	sun_open,
	gx_default_get_initial_matrix,
	sun_sync,
	gx_default_output_page,
	sun_close,
	gx_default_map_rgb_color,
	gx_default_map_color_rgb,
	sun_fill_rectangle,
	gx_default_tile_rectangle,
	sun_copy_mono,
	sun_copy_color,
	sun_draw_line,
	gx_default_fill_trapezoid,
	gx_default_tile_trapezoid
};

/* Define the SunView device */
typedef struct gx_device_sun {
	gx_device_common;
	Frame frame;
	Canvas canvas;
	Pixwin *pw;
	struct mpr_data mpr;
	Pixrect	pr;
} gx_device_sun;

/* The instance is public. */
gx_device_sun gs_sunview_device = {
	sizeof(gx_device_sun),
	&sun_procs,
	"sunview",
 	(int)(8.5*DEFAULT_DPI), (int)(11*DEFAULT_DPI),	/* x and y extent */
 	DEFAULT_DPI, DEFAULT_DPI,	/* x and y density */
		/* Following parameters are initialized for monochrome */
	0,			/* has color */
	1,			/* max r-g-b value */
	1,			/* bits per color pixel */
 	0,			/* connection not initialized */
		/* End of monochrome/color parameters */
};

#define x_device gs_sunview_device	/* just an abbreviation */

/* Macro for casting gx_device argument */
#define xdev ((gx_device_sun *)dev)

/* Macro to validate arguments */
#define check_rect()\
	if ( w <= 0 || h <= 0 ) return 0;\
	if ( x < 0 || x > xdev->width - w || y < 0 || y > xdev->height - h )\
		return -1

void
sun_window_create(int *argc_ptr, char **argv)
{
	x_device.frame =
		window_create(NULL, FRAME, FRAME_LABEL, "ghostscript",
					FRAME_ARGC_PTR_ARGV, argc_ptr, argv, 0);
}

int
sun_open(register gx_device *dev)
{
#ifdef gs_DEBUG
if ( gs_debug['X'] )
	{ extern int _Xdebug;
	  _Xdebug = 1;
	}
#endif
	if (xdev->frame == (Frame)0)
	    xdev->frame =
		window_create(NULL, FRAME, FRAME_LABEL, "ghostscript", 0);
	xdev->canvas = window_create(xdev->frame, CANVAS,
			CANVAS_AUTO_EXPAND,		FALSE,
			CANVAS_AUTO_SHRINK,		FALSE,
			CANVAS_WIDTH,			xdev->width,
			CANVAS_HEIGHT,			xdev->height,
			WIN_VERTICAL_SCROLLBAR,		scrollbar_create(0),
			WIN_HORIZONTAL_SCROLLBAR,	scrollbar_create(0),
			0);
	xdev->pw = canvas_pixwin(xdev->canvas);
	window_set(xdev->frame, WIN_SHOW, TRUE, 0);
	(void) notify_do_dispatch();
	(void) notify_dispatch();
	return 0;
}

/* Close the device. */
int
sun_close(gx_device *dev)
{	window_destroy(xdev->frame);
	xdev->frame = (Frame)0;
	xdev->canvas = (Canvas)0;
	xdev->pw = (Pixwin *)0;
	return 0;
}

/* Synchronize the display with the commands already given */
int
sun_sync(register gx_device *dev)
{	(void) notify_dispatch();
	return 0;
}

/* Fill a rectangle with a color. */
int
sun_fill_rectangle(register gx_device *dev,
  int x, int y, int w, int h, gx_color_index color)
{	check_rect();
	if ( color != gx_no_color_index )
		pw_write(xdev->pw, x, y, w, h, (color ? PIX_CLR : PIX_SET),
			(Pixrect *)0, 0, 0);
	(void) notify_dispatch();
	return 0;
}

/* Copy a monochrome bitmap. */
int
sun_copy_mono(register gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	static int optab[3][3] = {
		PIX_DST,	PIX_SRC|PIX_DST, PIX_NOT(PIX_SRC)&PIX_DST,
		PIX_NOT(PIX_SRC)|PIX_DST, PIX_SET,	PIX_NOT(PIX_SRC),
		PIX_SRC&PIX_DST,	PIX_SRC,	PIX_CLR,
	};
	int op;
	register int i;
	extern struct pixrectops mem_ops;

	check_rect();
	if ((op = optab[zero+1][one+1]) == PIX_DST)
		return 0;
	xdev->pr.pr_ops = &mem_ops;
	xdev->pr.pr_width = w + sourcex + 8;
	xdev->pr.pr_height = h;
	xdev->pr.pr_depth = 1;
	xdev->pr.pr_data = (caddr_t)&(xdev->mpr);
	if ((raster & 1) == 0) {
		xdev->mpr.md_linebytes = raster;
		xdev->mpr.md_image = (short *)((int)base & ~1);
		pw_write(xdev->pw, x, y, w, h, op, &(xdev->pr), 
			((int)base & 1) ? sourcex + 8 : sourcex, 0);
	} else {
		xdev->pr.pr_height = 1;
		for (i = 0; i < h; i++) {
			xdev->mpr.md_linebytes = raster;
			xdev->mpr.md_image = (short *)((int)base & ~1);
			pw_write(xdev->pw, x, y, w, 1, op, &(xdev->pr), 
				((int)base & 1) ? sourcex + 8 : sourcex, 0);
			base += raster;
			y++;
		}
	}
	(void) notify_dispatch();
	return 0;
}

/* Copy a "color" bitmap.  Since "color" is the same as monochrome, */
/* this just reduces to copying a monochrome bitmap. */
int
sun_copy_color(register gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h)
{	return sun_copy_mono(dev, base, sourcex, raster, x, y, w, h,
	  (gx_color_index)0, (gx_color_index)1);
}

/* Draw a line */
int
sun_draw_line(register gx_device *dev,
  int x0, int y0, int x1, int y1, gx_color_index color)
{	if ( color != gx_no_color_index )
		pw_vector(xdev->pw, x0, y0, x1, y1,
			  (color ? PIX_CLR : PIX_SET), 0);
	(void) notify_dispatch();
	return 0;
}
