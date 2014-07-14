/* Copyright (C) 1989 Aladdin Enterprises.  All rights reserved.
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

/* gxdevmem.h */
/* "Memory" device structure for GhostScript library */
/* Requires gxdevice.h */

/* A 'memory' device -- a stored bitmap. */
/* There are several different kinds: monochrome, 8-bit mapped color, */
/* and 24- and 32-bit true color.  (24- and 32-bit are equivalent: */
/* 24-bit takes less space, but is slower.)  All use the same structure, */
/* since it's so awkward to get the effect of subclasses in C. */
typedef struct gx_device_memory_s gx_device_memory;
struct gx_device_memory_s {
	gx_device_common;		/* (see gxdevice.h) */
	gs_matrix initial_matrix;	/* the initial transformation */
	int raster;			/* bytes per scan line, */
					/* filled in by '...bitmap_size' */
	byte *base;
	byte **line_ptrs;		/* scan line pointers */
	int (*bring_in_proc)(P6(gx_device_memory *dev,	/* (see below) */
		int byte_in_line /* (x) */, int line /* (y) */,
		int width, int height, int writing));
	/* Following is only needed for monochrome */
	int invert;			/* 0 if 1=white, -1 if 1=black */
	/* Following are only needed for mapped color */
	int palette_size;		/* # of entries */
	byte *palette;			/* RGB triples */
};
extern gx_device_memory
	mem_mono_device,
	mem_mapped_color_device,
	mem_true24_color_device,
	mem_true32_color_device;

/* Memory devices may have special setup requirements. */
/* In particular, it may not be obvious how much space to allocate */
/* for the bitmap.  Here is the routine that computes this */
/* from the width and height in the device structure. */
extern ulong gx_device_memory_bitmap_size(P1(gx_device_memory *));

/* Test whether a device is a memory device. */
extern int gs_device_is_memory(P1(struct gx_device_s *));

/* Copy data from the bitmap to a client. */
/* Return the number of lines copied. */
extern int mem_copy_scan_lines(P4(gx_device_memory *, int /* first_line */,
		byte * /* string */, uint /* string_size */));

/* Return the number of bytes per scan line for copy_scan_lines. */
extern int mem_bytes_per_scan_line(P1(gx_device_memory *));

/*
 * Memory devices may page their bitmaps.  The hook for this is the
 * bring_in_proc.  This procedure is called with the device and the
 * rectangle parameters at the beginning of any copy, fill, draw, or tile
 * operation (writing=1), or when copying scan lines back to a client
 * (writing=0).  The procedure should do whatever is necessary to ensure
 * that the relevant part of the bitmap is in memory, and then return 0.
 * If it can't do that (i.e., if there isn't enough buffer space),
 * it should return -1 or -2.  -1 means divide the rectangle in half
 * in X, and try again; -2 means divide it in Y and try again.
 * The default bring_in_proc always returns 0.
 */
#define mem_fault_split_X (-1)
#define mem_fault_split_Y (-2)

/*
 * A memory device that does not page its bitmap is guaranteed
 * to allocate the bitmap consecutively, i.e., in the form that
 * can serve as input to copy_mono or copy_color operations.
 */
