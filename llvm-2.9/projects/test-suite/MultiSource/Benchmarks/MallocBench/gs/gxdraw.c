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

/* gxdraw.c */
/* Primitive drawing routines for GhostScript imaging library */
#include "gx.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gxbitmap.h"
#include "gzstate.h"
#include "gzdevice.h"			/* requires gsstate.h */
#include "gzcolor.h"			/* requires gxdevice.h */

/* These routines all assume that gx_color_render has been called. */

/* Fill a rectangle. */
int
gz_fill_rectangle(int x, int y, int w, int h, gx_device_color *pdevc,
  gs_state *pgs)
{	gx_color_index darker = pdevc->color1;
	gx_color_index lighter;
	gx_device *dev = pgs->device->info;
	gx_bitmap *tile;
	int code;
#ifdef DEBUG
if ( gs_debug['q'] )
	printf("[q]x=%d y=%d w=%d h=%d  c1=%ld c2=%ld htl=%d\n",
		x, y, w, h, darker, (long)pdevc->color2,
		(long)pdevc->halftone_level);
#endif
	if ( color_is_pure(pdevc) )	/* no halftoning */
	   {	return (*dev->procs->fill_rectangle)(dev, x, y, w, h, darker);
	   }
	lighter = pdevc->color2;
	tile = pdevc->tile;
	/* See if the entire transfer falls within a single tile. */
	/* This is worth a quick check, because tiling is slow. */
	if ( w <= tile->width && h <= tile->height )
	   {	int xmod = x % tile->width, ymod;
		if ( xmod + w <= tile->width &&
		     (ymod = y % tile->height) + h <= tile->height
		   )
		   {	/* Just do a copy. */
			int raster = tile->raster;
			byte *tdata = tile->data + ymod * raster;
			return (color_is_color_halftone(pdevc) ?
				(*dev->procs->copy_color)(dev, tdata,
					xmod, raster, x, y, w, h) :
				(*dev->procs->copy_mono)(dev, tdata,
					xmod, raster, x, y, w, h,
					darker, lighter));
		   }
	   }
	/* Try to tile the rectangle primitively; */
	/* if this fails, use the default implementation. */
	if ( color_is_color_halftone(pdevc) )
		darker = lighter = gx_no_color_index;
	code = (*dev->procs->tile_rectangle)(dev, tile,
		x, y, w, h, darker, lighter);
	if ( code < 0 )
	   {	/* Use the default implementation */
		code = gx_default_tile_rectangle(dev, tile,
			x, y, w, h, darker, lighter);
	   }
	return code;
}

/* Fill a trapezoid.  Requires: wt >= 0, wb >= 0. */
/* Note that the arguments are fixeds, not ints! */
int
gz_fill_trapezoid_fixed(fixed fx0, fixed fw0, fixed fy0,
  fixed fx1, fixed fw1, fixed fh, int swap_axes,
  gx_device_color *pdevc, gs_state *pgs)
{	/* For the moment, we just convert everything to ints. */
	/* Later we will do the right thing with fractional pixels. */
	int x0 = fixed2int(fx0);
	fixed fx0r = fx0 + fw0;
	int w0 = fixed2int_ceiling(fx0r) - x0;
	int y0 = fixed2int(fy0);
	int x1 = fixed2int(fx1);
	fixed fx1r = fx1 + fw1;
	int w1 = fixed2int_ceiling(fx1r) - x1;
	fixed fy1 = fy0 + fh;
	int y1 = fixed2int_ceiling(fy1);
	int h = y1 - y0;
	if ( w0 == 0 && w1 == 0 || h <= 0 ) return 0;
	if ( !swap_axes && color_is_pure(pdevc) )
	   {	gx_device *dev = pgs->device->info;
		if ( (*dev->procs->fill_trapezoid)(dev,
			x0, y0, w0, x1, y1, w1,
			pdevc->color1) >= 0
		   )
			return 0;
	   }
	   {	int xl, fxl;
		int dxl, dxl1, dxlf = x1 - x0;
		int xr, fxr;
		int dxr, dxr1, dxrf = x1 + w1 - (x0 + w0);
		int y = y0;
		int rxl, rxr, ry;
		/* Compute integer and fractional deltas */
#define reduce_delta(df, d, d1, pos)\
	if ( df >= 0 )\
	   {	if ( df >= h )\
		  d1 = (d = df / h) + 1, df -= d * h;\
		else	/* save the divides */\
		   {	pos();\
			d = 0, d1 = 1;\
		   }\
	   }\
	else			/* df < 0 */\
	   {	if ( df <= -h )\
		  d1 = (d = df / h) - 1, df = d * h - df;\
		else	/* save the divides */\
		  d = 0, d1 = -1, df = -df;\
	   }
#define fill_trap_rect(x,y,w,h)\
  if ( swap_axes ) gz_fill_rectangle(y, x, h, w, pdevc, pgs);\
  else gz_fill_rectangle(x, y, w, h, pdevc, pgs)
#define pos_for_xl()			/* nothing */
		reduce_delta(dxlf, dxl, dxl1, pos_for_xl);
#define pos_for_xr()\
	if ( dxl == 0 && dxlf == 0 && dxrf == 0 )  /* detect rectangle */\
	   {	fill_trap_rect(x0, y0, w0, h);\
		return 0;\
	   }
		reduce_delta(dxrf, dxr, dxr1, pos_for_xr);
		xl = x0, fxl = arith_rshift(dxlf, 1);
		xr = x0 + w0, fxr = arith_rshift(dxrf, 1);
		rxl = xl, rxr = xr, ry = y;
		/* Do the fill */
		do
		   {	if ( xl != rxl || xr != rxr )	/* detect rectangles */
			   {	fill_trap_rect(rxl, ry, rxr - rxl, y - ry);
				rxl = xl, rxr = xr, ry = y;
			   }
			if ( (fxl += dxlf) >= h ) fxl -= h, xl += dxl1;
			else xl += dxl;
			if ( (fxr += dxrf) >= h ) fxr -= h, xr += dxr1;
			else xr += dxr;
		   }
		while ( ++y < y1 );
		if ( y != ry )
		   {	fill_trap_rect(rxl, ry, rxr - rxl, y - ry);
		   }
#undef fill_trap_rect
	   }
	return 0;
}

/* Default implementation of tile_rectangle */
int
gx_default_tile_rectangle(gx_device *dev, register gx_bitmap *tile,
  int x, int y, int w, int h, gx_color_index color0, gx_color_index color1)
{	/* Fill the rectangle in chunks */
	int width = tile->width;
	int height = tile->height;
	int raster = tile->raster;
	int ex = x + w, ey = y + h;
	int fex = ex - width, fey = ey - height;
	int irx = ((width & (width - 1)) == 0 ?	/* power of 2 */
		x & (width - 1) :
		x % width);
	int ry = y % height;
	int icw = width - irx;
	int ch = height - ry;
	byte *row = tile->data + ry * raster;
	int (*proc_mono)(P10(gx_device *, byte *, int, int,
		int, int, int, int, gx_color_index, gx_color_index));
	int (*proc_color)(P8(gx_device *, byte *, int, int,
		int, int, int, int));
	int color_halftone =
		(color0 == gx_no_color_index && color1 == gx_no_color_index);
	int cx, cy;
	if ( color_halftone )
		proc_color = dev->procs->copy_color;
	else
		proc_mono = dev->procs->copy_mono;
#ifdef DEBUG
if ( gs_debug['t'] )
   {	int ptx, pty;
	byte *ptp = tile->data;
	printf("[t]tile w=%d h=%d raster=%d; x=%d y=%d w=%d h=%d\n",
		tile->width, tile->height, tile->raster, x, y, w, h);
	for ( pty = 0; pty < tile->height; pty++ )
	   {	printf("   ");
		for ( ptx = 0; ptx < tile->raster; ptx++ )
			printf("%3x", *ptp++);
	   }
	printf("\n");
   }
#endif
	if ( icw > w ) icw = w;
	if ( ch > h ) ch = h;
#define real_copy_tile(sourcex, tx, tw)\
  (color_halftone ?\
    (*proc_color)(dev, row, sourcex, raster, tx, cy, tw, ch) :\
    (*proc_mono)(dev, row, sourcex, raster, tx, cy, tw, ch, color0, color1))
#ifdef DEBUG
#define copy_tile(sx, tx, tw)\
  if ( gs_debug['t'] )\
	printf("   copy sx=%d x=%d y=%d w=%d h=%d\n",\
		sx, tx, cy, tw, ch);\
  real_copy_tile(sx, tx, tw)
#else
#define copy_tile(sx, tx, tw)\
  real_copy_tile(sx, tx, tw)
#endif
	for ( cy = y; cy < ey; )
	   {	copy_tile(irx, x, icw);
		cx = x + icw;
		while ( cx <= fex )
		   {	copy_tile(0, cx, width);
			cx += width;
		   }
		if ( cx < ex )
		   {	copy_tile(0, cx, ex - cx);
		   }
		cy += ch;
		ch = (cy > fey ? ey - cy : height);
		row = tile->data;
	   }
#undef copy_tile
#undef real_copy_tile
	return 0;
}

/* Draw a one-pixel-wide line. */
int
gz_draw_line_fixed(fixed ixf, fixed iyf, fixed itoxf, fixed itoyf,
  gx_device_color *pdevc, gs_state *pgs)
{	int ix = fixed2int(ixf);
	int iy = fixed2int(iyf);
	int itox = fixed2int(itoxf);
	int itoy = fixed2int(itoyf);
	if ( itoy == iy )		/* horizontal line */
	   {	if ( ix <= itox )
			gz_fill_rectangle(ix, iy, fixed2int_ceiling(itoxf) -
						ix, 1, pdevc, pgs);
		else
			gz_fill_rectangle(itox, iy, fixed2int_ceiling(ixf) -
						itox, 1, pdevc, pgs);
	   }
	else
	   {	gx_device *dev = pgs->device->info;
		fixed h, w, tf;
#define fswap(a, b) tf = a, a = b, b = tf
		if ( color_is_pure(pdevc) &&
		    (*dev->procs->draw_line)(dev, ix, iy, itox, itoy,
					     pdevc->color1) >= 0 )
		  return 0;
		h = itoyf - iyf;
		w = itoxf - ixf;
#define fixed_eps (fixed)1
		if ( (w < 0 ? -w : w) <= (h < 0 ? -h : h) )
		   {	if ( h < 0 )
				fswap(ixf, itoxf), fswap(iyf, itoyf),
				h = -h;
			gz_fill_trapezoid_fixed(ixf, fixed_eps, iyf,
						itoxf, fixed_eps, h,
						0, pdevc, pgs);
		   }
		else
		   {	if ( w < 0 )
				fswap(ixf, itoxf), fswap(iyf, itoyf),
				w = -w;
			gz_fill_trapezoid_fixed(iyf, fixed_eps, ixf,
						itoyf, fixed_eps, w,
						1, pdevc, pgs);
		   }
#undef fixed_eps
#undef fswap
	   }
	return 0;
}

/****** STUBS ******/
int
gx_default_draw_line(gx_device *dev,
  int x0, int y0, int x1, int y1, gx_color_index color)
{	return -1;
}
int
gx_default_fill_trapezoid(gx_device *dev,
  int x0, int y0, int width0, int x1, int y1, int width1,
  gx_color_index color)
{	return -1;
}
int
gx_default_tile_trapezoid(gx_device *dev, gx_bitmap *tile,
  int x0, int y0, int width0, int x1, int y1, int width1,
  gx_color_index color0, gx_color_index color1)
{	return -1;
}
int
gx_default_tile_trapezoid_color(gx_device *dev, gx_bitmap *tile,
  int x0, int y0, int width0, int x1, int y1, int width1)
{	return -1;
}
