/* Copyright (C) 1990 Aladdin Enterprises.  All rights reserved.
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

/* gdevvga.h */
/* Parameters for VGA driver for Ghostscript */
/* This file must precede gdevpcfb.h. */

#define DEVICE_STRUCT_NAME gs_vga_device
#define DEVICE_NAME "vga"
#define FB_RASTER 80
#define SCREEN_HEIGHT 480
#define SCREEN_ASPECT_RATIO 1.0
#define VIDEO_MODE 0x12
