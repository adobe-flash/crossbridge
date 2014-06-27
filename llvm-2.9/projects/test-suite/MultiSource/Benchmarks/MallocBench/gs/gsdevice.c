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

/* gsdevice.c */
/* Device operators for GhostScript library */
#include "math_.h"			/* for fabs */
#include "memory_.h"			/* for memcpy */
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"			/* ditto */
#include "gxmatrix.h"			/* for gzstate.h */
#include "gxbitmap.h"
#include "gzstate.h"
#include "gzdevice.h"
#include "gxdevmem.h"

/* Import the device list from gdevs.c */
extern gx_device *gx_device_list[];

/* Device definitions */
/* Following defines the null device */
private dev_proc_fill_rectangle(null_fill_rectangle);
private dev_proc_tile_rectangle(null_tile_rectangle);
private dev_proc_copy_mono(null_copy_mono);
private dev_proc_copy_color(null_copy_color);
private dev_proc_draw_line(null_draw_line);
private dev_proc_fill_trapezoid(null_fill_trapezoid);
private dev_proc_tile_trapezoid(null_tile_trapezoid);

private gx_device_procs null_procs = {
	gx_default_open_device,
	gx_default_get_initial_matrix,
	gx_default_sync_output,
	gx_default_output_page,
	gx_default_close_device,
	gx_default_map_rgb_color,
	gx_default_map_color_rgb,
	null_fill_rectangle,
	null_tile_rectangle,
	null_copy_mono,
	null_copy_color,
	null_draw_line,
	null_fill_trapezoid,
	null_tile_trapezoid
};
private gx_device null_device = {
	sizeof(device),
	&null_procs,
	"null",
	0x3fff, 0x3fff,			/* 0, 0, */
	1, 1,
	0, 1, 1, 1
};

/* The null device */
gx_device *gx_device_null_p = &null_device;

/* Flush buffered output to the device */
int
gs_flushpage(gs_state *pgs)
{	gx_device *dev = pgs->device->info;
	return (*dev->procs->sync_output)(dev);
}

/* Make the device output the accumulated page description */
int
gs_copypage(gs_state *pgs)
{	gx_device *dev = pgs->device->info;
	return (*dev->procs->output_page)(dev);
}

/* Copy scan lines from an image device */
int
gs_copyscanlines(gx_device *dev, int start_y, byte *data, uint size,
  int *plines_copied, uint *pbytes_copied)
{	int count;
	if ( !gs_device_is_memory(dev) ) return_error(gs_error_undefined);
	count = mem_copy_scan_lines((gx_device_memory *)dev,
			start_y, data, size);
	if ( plines_copied != NULL )
		*plines_copied = count;
	if ( pbytes_copied != NULL )
		*pbytes_copied = count *
			mem_bytes_per_scan_line((gx_device_memory *)dev);
	return 0;
}

/* Get the current device from the graphics state */
gx_device *
gs_currentdevice(gs_state *pgs)
{	return pgs->device->info;
}

/* Get the name of a device */
char *
gs_devicename(gx_device *dev)
{	return dev->name;
}

/* Read out the current device parameters */
void
gs_deviceparams(gx_device *dev, gs_matrix *pmat, int *pwidth, int *pheight)
{	(*dev->procs->get_initial_matrix)(dev, pmat);
	*pwidth = dev->width;
	*pheight = dev->height;
}

/* Get the N'th device from the known device list */
gx_device *
gs_getdevice(int index)
{	int i;
	for ( i = 0; gx_device_list[i] != 0; i++ )
	   {	if ( i == index ) return gx_device_list[i];
	   }
	return 0;			/* index out of range */
}

/* Make a device by cloning an existing one */
int
gs_makedevice(gx_device **pnew_dev, gx_device *dev, gs_matrix *pmat,
  uint width, uint height, proc_alloc_t palloc)
{	register gx_device *new_dev;
	new_dev = (gx_device *)(*palloc)(1, dev->params_size, "gs_makedevice");
	if ( new_dev == 0 ) return_error(gs_error_VMerror);
	if ( width <= 0 || height <= 0 ) return_error(gs_error_rangecheck);
	memcpy(new_dev, dev, dev->params_size);
	new_dev->width = width;
	new_dev->height = height;
	new_dev->is_open = 0;
	*pnew_dev = new_dev;
	return 0;
}

/* Make a memory (image) device. */
/* If num_colors = -24 or -32, this is a true-color device; */
/* otherwise, num_colors is the number of colors in the palette. */
int
gs_makeimagedevice(gx_device **pnew_dev, gs_matrix *pmat,
  uint width, uint height, float *colors, int num_colors, proc_alloc_t palloc)
{	gx_device_memory *old_dev = &mem_mapped_color_device;
	register gx_device_memory *new_dev;
	byte *bits;
	int palette_size = num_colors;
	int pcount;
	int bits_per_pixel;
	ulong bitmap_size;
	int white = -1, black = -1;
	byte palette[256 * 3];
	int has_color;
	if ( width <= 0 || height <= 0 ) return_error(gs_error_rangecheck);
	/************** TEMPORARY LIMITATION: **************/
	/*** only 1-, 8-, 24-, and 32-bit are supported. ***/
	switch ( num_colors )
	   {
	case 2:
		bits_per_pixel = 1; old_dev = &mem_mono_device;
		break;
	case 4:		/*** bits_per_pixel = 2; break; ***/
	case 16:	/*** bits_per_pixel = 4; break; ***/
		return_error(gs_error_rangecheck);
	case 256:
		bits_per_pixel = 8; break;
	case -24:
		bits_per_pixel = 24; old_dev = &mem_true24_color_device;
		palette_size = 0; break;
	case -32:
		bits_per_pixel = 32; old_dev = &mem_true32_color_device;
		palette_size = 0; break;
	default:
		return_error(gs_error_rangecheck);
	   }
	pcount = palette_size * 3;
	/* Check to make sure the palette contains white and black. */
	if ( bits_per_pixel <= 8 )
	   {	float *p;
		byte *q;
		int i;
		has_color = 0;
		for ( i = 0, p = colors, q = palette;
		      i < pcount; i++, p++, q++
		    )
		   {	if ( *p < -0.001 || *p > 1.001 )
				return_error(gs_error_rangecheck);
			*q = (*p * 255) + 0.5;
			if ( i % 3 == 2 )
			   {
				if ( *q == q[-1] && *q == q[-2] )
				   {	if ( *q == 0 ) black = i - 2;
					else if ( *q == 255 ) white = i - 2;
				   }
				else
					has_color = 1;
			   }
		   }
		if ( white < 0 || black < 0 )
			return_error(gs_error_rangecheck);
	   }
	else
		has_color = 1;
	new_dev = (gx_device_memory *)(*palloc)(1, old_dev->params_size, "gs_makeimagedevice(device)");
	if ( new_dev == 0 ) return_error(gs_error_VMerror);
	*new_dev = *old_dev;
	new_dev->initial_matrix = *pmat;
	new_dev->width = width;
	new_dev->height = height;
	new_dev->has_color = has_color;
	new_dev->bits_per_color_pixel = bits_per_pixel;
	bitmap_size = gx_device_memory_bitmap_size(new_dev);
	if ( bitmap_size > max_uint )	/* can't allocate it! */
		return_error(gs_error_limitcheck);
	bits = (byte *)(*palloc)(1, (uint)bitmap_size + pcount,
				 "gs_makeimagedevice(bits)");
	if ( bits == 0 ) return_error(gs_error_VMerror);
	new_dev->base = bits;
	new_dev->invert = (black == 0 ? 0 : -1);
	new_dev->palette_size = palette_size;
	new_dev->palette = bits + bitmap_size;
	memcpy(new_dev->palette, palette, pcount);
	new_dev->is_open = 0;
	*pnew_dev = (gx_device *)new_dev;
	return 0;
}

/* Set the device in the graphics state */
int
gs_setdevice(gs_state *pgs, gx_device *dev)
{	register device *pdev = pgs->device;
	int was_open = dev->is_open;
	int code;
	/* Initialize the device */
	if ( !was_open )
	   {	code = (*dev->procs->open_device)(dev);
		if ( code < 0 ) return code;
		dev->is_open = 1;
	   }
	/* Compute device white and black codes */
	pdev->black = (*dev->procs->map_rgb_color)(dev, 0, 0, 0);
	pdev->white = (*dev->procs->map_rgb_color)(dev, dev->max_rgb_value, dev->max_rgb_value, dev->max_rgb_value);
	pdev->info = dev;
	if (	(code = gs_initmatrix(pgs)) < 0 ||
		(code = gs_initclip(pgs)) < 0
	   )
		return code;
	if ( !was_open )
		if ( (code = gs_erasepage(pgs)) < 0 )
			return code;
	return 0;
}

/* Select the null device.  This is just a convenience. */
void
gs_nulldevice(gs_state *pgs)
{	gs_setdevice(pgs, gx_device_null_p);
}

/* Install enough of a null device to suppress graphics output */
/* during the execution of stringwidth. */
void
gx_device_no_output(gs_state *pgs)
{	pgs->device->info = &null_device;
}

/* Dummy device procedures */
private int
null_fill_rectangle(gx_device *dev, int x, int y, int w, int h,
  gx_color_index color)
{	return 0;
}
private int
null_tile_rectangle(gx_device *dev, gx_bitmap *tile,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	return 0;
}
private int
null_copy_mono(gx_device *dev, byte *data,
  int dx, int raster, int x, int y, int w, int h,
  gx_color_index zero, gx_color_index one)
{	return 0;
}
private int
null_copy_color(gx_device *dev, byte *data,
  int dx, int raster, int x, int y, int w, int h)
{	return 0;
}
private int
null_draw_line(gx_device *dev, int x0, int y0, int x1, int y1,
  gx_color_index color)
{	return 0;
}
private int
null_fill_trapezoid(gx_device *dev,
  int x0, int y0, int w0, int x1, int y1, int w1, gx_color_index color)
{	return 0;
}
private int
null_tile_trapezoid(gx_device *dev, gx_bitmap *tile,
  int x0, int y0, int w0, int x1, int y1, int w1,
  gx_color_index color0, gx_color_index color1)
{	return 0;
}

/* Default device open, matrix, sync, output, close, and map procedures. */
int
gx_default_open_device(gx_device *dev)
{	return 0;
}
void
gx_default_get_initial_matrix(register gx_device *dev, register gs_matrix *pmat)
{	pmat->xx = dev->x_pixels_per_inch / 72.0;
	pmat->xy = 0;
	pmat->yx = 0;
	pmat->yy = dev->y_pixels_per_inch / -72.0;
	pmat->tx = 0;
	pmat->ty = dev->height;
}
int
gx_default_sync_output(gx_device *dev)
{	return 0;
}
int
gx_default_output_page(gx_device *dev)
{	return (*dev->procs->sync_output)(dev);
}
int
gx_default_close_device(gx_device *dev)
{	return 0;
}
gx_color_index
gx_default_map_rgb_color(gx_device *dev, ushort r, ushort g, ushort b)
{	return (gx_color_index)max(max(r, g), b);
}
int
gx_default_map_color_rgb(gx_device *dev, gx_color_index color, ushort *prgb)
{	prgb[0] = prgb[1] = prgb[2] = (ushort)color;
	return 0;
}

#ifdef DEBUG

/* ------ Tracing 'device' ------*/

/* To avoid unpleasant interactions with makedevice, */
/* the tracing 'device' uses an external linked list to keep track of */
/* the real procedures that were replaced in the procedure vector. */

typedef struct trace_record_s trace_record;
struct trace_record_s {
	trace_record *next;
	gx_device_procs *tprocs;
	gx_device_procs procs;
};

private gx_device *trace_cache_device = NULL;
private gx_device_procs *trace_cache_procs;
private trace_record *trace_list = NULL;

#define rprocs\
	(dev == trace_cache_device ? trace_cache_procs :\
	 trace_find_procs(dev))

/* Procedure structure */
private dev_proc_open_device(trace_open_device);
private dev_proc_get_initial_matrix(trace_get_initial_matrix);
private dev_proc_sync_output(trace_sync_output);
private dev_proc_output_page(trace_output_page);
private dev_proc_close_device(trace_close_device);
private dev_proc_map_rgb_color(trace_map_rgb_color);
private dev_proc_map_color_rgb(trace_map_color_rgb);
private dev_proc_fill_rectangle(trace_fill_rectangle);
private dev_proc_tile_rectangle(trace_tile_rectangle);
private dev_proc_copy_mono(trace_copy_mono);
private dev_proc_copy_color(trace_copy_color);
private dev_proc_draw_line(trace_draw_line);
private dev_proc_fill_trapezoid(trace_fill_trapezoid);
private dev_proc_tile_trapezoid(trace_tile_trapezoid);

private gx_device_procs trace_procs = {
	trace_open_device,
	trace_get_initial_matrix,
	trace_sync_output,
	trace_output_page,
	trace_close_device,
	trace_map_rgb_color,
	trace_map_color_rgb,
	trace_fill_rectangle,
	trace_tile_rectangle,
	trace_copy_mono,
	trace_copy_color,
	trace_draw_line,
	trace_fill_trapezoid,
	trace_tile_trapezoid
};

/* Find the real procedures for a traced device */
private gx_device_procs *
trace_find_procs(gx_device *tdev)
{	gx_device_procs *tprocs = tdev->procs;
	register trace_record *tp = trace_list;
	while ( tp != NULL )
	   {	if ( tp->tprocs == tprocs )
		   {	trace_cache_device = tdev;
			return (trace_cache_procs = &tp->procs);
		   }
		tp = tp->next;
	   }
	dprintf("Traced procedures not found!\n");
	exit(1);
}

/* Trace a device. */
gx_device *
gs_trace_device(gx_device *rdev)
{	trace_record *tp;
	if ( rdev->procs->open_device == trace_procs.open_device )
		return rdev;		/* already traced */
	tp = (trace_record *)gs_malloc(1, sizeof(trace_record), 
				       "gs_trace_device");
	if ( tp == 0 ) return 0;
	tp->next = trace_list;
	tp->tprocs = rdev->procs;
	tp->procs = *rdev->procs;
	trace_list = tp;
	*rdev->procs = trace_procs;
	return rdev;
}

/* Utilities */
private int
trace_print_code(int result)
{	if ( result == 0 )
		printf(";\n");
	else
		printf(";	/* = %d */\n", result);
	return result;
}
private void
trace_print_tile(gx_bitmap *tile)
{	int i;
	printf("\t{ static byte data = { 0x%x", tile->data[0]);
	for ( i = 1; i < tile->raster * tile->height; i++ )
		printf(", 0x%x", tile->data[i]);
	printf(" };\n\t  static gx_bitmap tile = { &data, %d, %d, %d };\n",
		tile->raster, tile->width, tile->height);
}

/* Procedures */
private int
trace_open_device(gx_device *dev)
{	int result = (*rprocs->open_device)(dev);
if ( gs_debug['v'] )
	printf("[v]\topen_device(dev)"),
	  trace_print_code(result);
	return result;
}
private void
trace_get_initial_matrix(gx_device *dev, gs_matrix *pmat)
{	(*rprocs->get_initial_matrix)(dev, pmat);
if ( gs_debug['v'] )
	printf("[v]\tget_initial_matrix(dev) = (%6g, %6g, %6g, %6g, %6g, %6g);\n",
		pmat->xx, pmat->xy, pmat->yx, pmat->yy, pmat->tx, pmat->ty);
}
private int
trace_sync_output(gx_device *dev)
{	int result = (*rprocs->sync_output)(dev);
if ( gs_debug['v'] )
	printf("[v]\tsync_output(dev)"),
	  trace_print_code(result);
	return result;
}
private int
trace_output_page(gx_device *dev)
{	int result = (*rprocs->output_page)(dev);
if ( gs_debug['v'] )
	printf("[v]\toutput_page(dev)"),
	  trace_print_code(result);
	return result;
}
private int
trace_close_device(gx_device *dev)
{	int result = (*rprocs->close_device)(dev);
if ( gs_debug['v'] )
	printf("[v]\tclose_device(dev)"),
	  trace_print_code(result);
	return result;
}
private gx_color_index
trace_map_rgb_color(gx_device *dev, ushort r, ushort g, ushort b)
{	gx_color_index result = (*rprocs->map_rgb_color)(dev, r, g, b);
if ( gs_debug['v'] )
	printf("[v]\tmap_rgb_color(dev, %u, %u, %u) /*= %ld */;\n", r, g, b, (long)result);
	return result;
}
private int
trace_map_color_rgb(gx_device *dev, gx_color_index color, ushort *prgb)
{	int result = (*rprocs->map_color_rgb)(dev, color, prgb);
if ( gs_debug['v'] )
	printf("\t{ ushort rgb[3]; T(map_color_rgb)(dev, %ld, rgb /* %u, %u, %u */); }",
		(long)color, prgb[0], prgb[1], prgb[2]),
	  trace_print_code(result);
	return result;
}
private int
trace_fill_rectangle(gx_device *dev, int x, int y, int w, int h,
  gx_color_index color)
{	int result = (*rprocs->fill_rectangle)(dev, x, y, w, h, color);
if ( gs_debug['v'] )
	printf("[v]\tfill_rectangle(dev, %d, %d, %d, %d, %ld)", x, y, w, h, (long)color),
	  trace_print_code(result);
	return result;
}
private int
trace_tile_rectangle(gx_device *dev, gx_bitmap *tile,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	int result = (*rprocs->tile_rectangle)(dev, tile,
		x, y, w, h, zero, one);
if ( gs_debug['v'] )
   {	trace_print_tile(tile);
	printf("\t  T(tile_rectangle)(dev, &tile, %d, %d, %d, %d, %ld, %ld);\n\t}",
		x, y, w, h, (long)zero, (long)one);
	trace_print_code(result);
   }
	return result;
}
private int
trace_copy_mono(gx_device *dev, byte *data,
  int dx, int raster, int x, int y, int w, int h,
  gx_color_index zero, gx_color_index one)
{	int result = (*rprocs->copy_mono)(dev, data,
		dx, raster, x, y, w, h, zero, one);
if ( gs_debug['v'] )
	printf("[v]\tcopy_mono(dev, data, %d, %d, %d, %d, %d, %d, %ld, %ld)",
		dx, raster, x, y, w, h, (long)zero, (long)one),
	  trace_print_code(result);
	return result;
}
private int
trace_copy_color(gx_device *dev, byte *data,
  int dx, int raster, int x, int y, int w, int h)
{	int result = (*rprocs->copy_color)(dev, data,
		dx, raster, x, y, w, h);
if ( gs_debug['v'] )
	printf("[v]\tcopy_color(dev, data, %d, %d, %d, %d, %d, %d)",
		dx, raster, x, y, w, h),
	  trace_print_code(result);
	return result;
}
private int
trace_draw_line(gx_device *dev, int x0, int y0, int x1, int y1,
  gx_color_index color)
{	int result = (*rprocs->draw_line)(dev, x0, y0, x1, y1, color);
if ( gs_debug['v'] )
	printf("[v]\tdraw_line(dev, %d, %d, %d, %d, %ld)",
		x0, y0, x1, y1, (long)color),
	  trace_print_code(result);
	return result;
}
private int
trace_fill_trapezoid(gx_device *dev,
  int x0, int y0, int w0, int x1, int y1, int w1, gx_color_index color)
{	int result = (*rprocs->fill_trapezoid)(dev,
		x0, y0, w0, x1, y1, w1, color);
if ( gs_debug['v'] )
	printf("[v]\tfill_trapezoid(dev, %d, %d, %d, %d, %d, %d, %ld)",
		x0, y0, w0, x1, y1, w1, (long)color),
	  trace_print_code(result);
	return result;
}
private int
trace_tile_trapezoid(gx_device *dev, gx_bitmap *tile,
  int x0, int y0, int w0, int x1, int y1, int w1,
  gx_color_index color0, gx_color_index color1)
{	int result = (*rprocs->tile_trapezoid)(dev, tile,
		x0, y0, w0, x1, y1, w1, color0, color1);
if ( gs_debug['v'] )
   {	trace_print_tile(tile);
	printf("[v]\ttile_trapezoid(dev, %d, %d, %d, %d, %d, %d, %ld, %ld)",
		x0, y0, w0, x1, y1, w1, (long)color0, (long)color1);
	trace_print_code(result);
   }
	return result;
}

#endif
