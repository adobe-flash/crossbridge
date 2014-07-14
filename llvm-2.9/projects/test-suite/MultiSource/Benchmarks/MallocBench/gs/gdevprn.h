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

/* gdevprn.h */
/* Common header file for memory-buffered black-and-white printers */

/*
 * This header file is actually used in two different ways.
 * gdevprn.c includes it without defining DRIVER.
 * The individual drivers include it with DRIVER defined.
 * When DRIVER is defined, this file generates actual data structures.
 *
 * >>>>>> NOTE: in addition to DRIVER, the printer driver must #define <<<<<<
 * >>>>>> the following before #include'ing this file: <<<<<<
 * >>>>>> GS_PRN_DEVICE, DEVICE_NAME, <<<<<<
 * >>>>>> PAGE_WIDTH_10THS, PAGE_HEIGHT_10THS, X_DPI, Y_DPI, <<<<<<
 * >>>>>> PRN_OPEN, PRN_CLOSE, PRN_OUTPUT_PAGE. <<<<<<
 */

#ifdef __MSDOS__
#  include <io.h>			/* for ioctl */
#endif
#include "string_.h"
#include "gs.h"
#include "gxbitmap.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxdevice.h"
#include "gxdevmem.h"
#include "gvirtmem.h"

typedef struct gx_device_s gx_device;

/****** ENABLE OR DISABLE VIRTUAL MEMORY ******/
#ifdef __MSDOS__
#  define VMEM_SPACE 20000
#else
#  define VMEM_SPACE 0
#endif

/****** ENABLE OR DISABLE DIRECT WRITING TO PRINTER ******/
#ifdef __MSDOS__
#  define WRITE_TO_PRN 1
#else
#  define WRITE_TO_PRN 0
#endif

/* Define the scratch file name template for mktemp */
#ifdef __MSDOS__
#  define SCRATCH_TEMPLATE "_temp_XXXXXX"
#else
#  define SCRATCH_TEMPLATE "/tmp/prn_XXXXXX"
#endif

/* See gxdevice.h for the definitions of the procedures. */

gx_color_index
  gdev_prn_map_rgb_color(P4(gx_device *, ushort, ushort, ushort));

int gdev_prn_map_color_rgb(P3(gx_device *, gx_color_index, ushort *));

int
  gdev_prn_fill_rectangle(P6(gx_device *, int, int, int, int,
    gx_color_index)),
  gdev_prn_copy_mono(P10(gx_device *, byte *, int, int, int, int, int, int,
    gx_color_index, gx_color_index)),
  gdev_prn_copy_color(P8(gx_device *, byte *, int, int, int, int, int, int));

/* The device descriptor */
typedef struct gx_device_printer_s gx_device_printer;
struct gx_device_printer_s {
	gx_device_common;
	/* The following two items must be set before calling */
	/* the device open routine. */
	long vmem_space;		/* amount of space for vmem */
	int write_to_prn;	/* if true, write directly to printer */
	gx_device_memory mem;		/* buffer */
	gx_vmem vmem;			/* virtual memory instance */
	char fname[30];			/* scratch file name, */
					/* strlen(SCRATCH_TEMPLATE)+1 */
	FILE *file;			/* scratch file */
};

/* Macro for casting gx_device argument */
#define prn_dev ((gx_device_printer *)dev)

/* Common procedures defined in gdevprn.c */
int gdev_prn_open(P1(gx_device *));
int gdev_prn_close(P1(gx_device *));
int gdev_prn_open_printer(P1(gx_device *));
void gdev_prn_transpose_8x8(P4(byte *, int, byte *, int));
int gdev_prn_close_printer(P1(gx_device *));

/****** The remainder of this file is only included in the ******/
/****** actual driver, not in gdevprn.c. ******/

#ifdef DRIVER

/* Device-specific procedures. */
/* The first two aren't declared private, because they might actually be */
/* the generic procedures gdev_prn_open or gdev_prn_close. */
int PRN_OPEN(P1(gx_device *));
int PRN_CLOSE(P1(gx_device *));
private int PRN_OUTPUT_PAGE(P1(gx_device *));

/* Computed parameters */
/*
 * The following should be
 *	#define LINE_WIDTH ((int)(PAGE_WIDTH_INCHES*X_DPI))
 * but some compilers (the Ultrix 3.X pcc compiler and the HPUX compiler)
 * can't cast a computed float to an int.  That's why we specify
 * the page height and width in inches/10 instead of inches.
 */
#define LINE_WIDTH (PAGE_WIDTH_10THS * X_DPI / 10)
#define PAGE_HEIGHT (PAGE_HEIGHT_10THS * Y_DPI / 10)
#define LINE_SIZE ((LINE_WIDTH + 7) / 8)

static gx_device_procs prn_procs = {
	PRN_OPEN,
	gx_default_get_initial_matrix,
	gx_default_sync_output,
	PRN_OUTPUT_PAGE,
	PRN_CLOSE,
	gdev_prn_map_rgb_color,
	gdev_prn_map_color_rgb,
	gdev_prn_fill_rectangle,
	gx_default_tile_rectangle,
	gdev_prn_copy_mono,
	gdev_prn_copy_color,
	gx_default_draw_line,
	gx_default_fill_trapezoid,
	gx_default_tile_trapezoid
};

gx_device_printer GS_PRN_DEVICE = {
	sizeof(gx_device_printer),
	&prn_procs,
	DEVICE_NAME,
	LINE_WIDTH,	/* width */
	PAGE_HEIGHT,	/* height */
	X_DPI,
	Y_DPI,
	0,		/* set for BLACK and WHITE device */
	1,		/* rgb_max_value */
	1,		/* bits per color pixel */
	0,		/* not initialized yet */
	VMEM_SPACE,
	WRITE_TO_PRN
};

#endif					/* ifdef DRIVER */
