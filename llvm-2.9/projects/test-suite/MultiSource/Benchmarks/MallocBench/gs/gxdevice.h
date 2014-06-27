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

/* gxdevice.h */
/* Device description structure for GhostScript library */
/* Requires gsmatrix.h */

/* Define the structure for bitmaps (opaquely). */
typedef struct gx_bitmap_s gx_bitmap;

/* Define the type for device color indices. */
typedef unsigned long gx_color_index;
#define gx_no_color_index ((gx_color_index)(-1))	/* transparent */

/* See drivers.doc for documentation of the driver interface. */

/* Structure for device procedures */
typedef struct gx_device_procs_s gx_device_procs;

/* Structure for generic device description */
#define gx_device_common\
	int params_size;		/* size of this structure */\
	gx_device_procs *procs;\
	char *name;			/* the device name */\
	int width;			/* width in pixels */\
	int height;			/* height in pixels */\
	float x_pixels_per_inch;	/* x density */\
	float y_pixels_per_inch;	/* y density */\
	int has_color;			/* true if device supports color */\
	unsigned short max_rgb_value;	/* max r, g, b value */\
	int bits_per_color_pixel;	/* for copy_color */\
	int is_open			/* true if device has been opened */
/* A generic device */
struct gx_device_s {
	gx_device_common;
};

/* Definition of device procedures */
struct gx_device_procs_s {

#define dev_proc_open_device(proc)\
  int proc(P1(struct gx_device_s *))
	dev_proc_open_device((*open_device));

#define dev_proc_get_initial_matrix(proc)\
  void proc(P2(struct gx_device_s *, gs_matrix *))
	dev_proc_get_initial_matrix((*get_initial_matrix));

#define dev_proc_sync_output(proc)\
  int proc(P1(struct gx_device_s *))
	dev_proc_sync_output((*sync_output));

#define dev_proc_output_page(proc)\
  int proc(P1(struct gx_device_s *))
	dev_proc_output_page((*output_page));

#define dev_proc_close_device(proc)\
  int proc(P1(struct gx_device_s *))
	dev_proc_close_device((*close_device));

#define dev_proc_map_rgb_color(proc)\
  gx_color_index proc(P4(struct gx_device_s *,\
    unsigned short red, unsigned short green, unsigned short blue))
	dev_proc_map_rgb_color((*map_rgb_color));

#define dev_proc_map_color_rgb(proc)\
  int proc(P3(struct gx_device_s *,\
    gx_color_index color, unsigned short rgb[3]))
	dev_proc_map_color_rgb((*map_color_rgb));

#define dev_proc_fill_rectangle(proc)\
  int proc(P6(struct gx_device_s *,\
    int x, int y, int width, int height, gx_color_index color))
	dev_proc_fill_rectangle((*fill_rectangle));

#define dev_proc_tile_rectangle(proc)\
  int proc(P8(struct gx_device_s *,\
    gx_bitmap *tile, int x, int y, int width, int height,\
    gx_color_index color0, gx_color_index color1))
	dev_proc_tile_rectangle((*tile_rectangle));

#define dev_proc_copy_mono(proc)\
  int proc(P10(struct gx_device_s *,\
    unsigned char *data, int data_x, int raster,\
    int x, int y, int width, int height,\
    gx_color_index color0, gx_color_index color1))
	dev_proc_copy_mono((*copy_mono));

#define dev_proc_copy_color(proc)\
  int proc(P8(struct gx_device_s *,\
    unsigned char *data, int data_x, int raster,\
    int x, int y, int width, int height))
	dev_proc_copy_color((*copy_color));

#define dev_proc_draw_line(proc)\
  int proc(P6(struct gx_device_s *,\
    int x0, int y0, int x1, int y1, gx_color_index color))
	dev_proc_draw_line((*draw_line));

#define dev_proc_fill_trapezoid(proc)\
  int proc(P8(struct gx_device_s *,\
    int x0, int y0, int width0, int x1, int y1, int width1,\
    gx_color_index color))
	dev_proc_fill_trapezoid((*fill_trapezoid));

#define dev_proc_tile_trapezoid(proc)\
  int proc(P10(struct gx_device_s *, gx_bitmap *tile,\
    int x0, int y0, int width0, int x1, int y1, int width1,\
    gx_color_index color0, gx_color_index color1))
	dev_proc_tile_trapezoid((*tile_trapezoid));

};

/* Default implementations of optional procedures */
dev_proc_open_device(gx_default_open_device);
dev_proc_get_initial_matrix(gx_default_get_initial_matrix);
dev_proc_sync_output(gx_default_sync_output);
dev_proc_output_page(gx_default_output_page);
dev_proc_close_device(gx_default_close_device);
dev_proc_map_rgb_color(gx_default_map_rgb_color);
dev_proc_map_color_rgb(gx_default_map_color_rgb);
dev_proc_tile_rectangle(gx_default_tile_rectangle);
dev_proc_draw_line(gx_default_draw_line);
dev_proc_fill_trapezoid(gx_default_fill_trapezoid);
dev_proc_tile_trapezoid(gx_default_tile_trapezoid);
