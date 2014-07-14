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

/* gdevs.c */
/* Installed device table for Ghostscript */
/* We don't really need the definition of gx_device, because we only */
/* declare pointers to devices, but some compilers seem to want it.... */
#include "gs.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxbitmap.h"
#include "gxdevice.h"

typedef struct gx_device_s gx_device;

/*
 * The Ghostscript makefile generates the file gdevs.h, which consists of
 * lines of the form
 *	d(gs_xxx_device)
 * for each installed device.  We include this file twice, once to declare
 * the devices as extern, and once to generate the table of pointers.
 */

#define d(dev) extern gx_device dev;
#include "gdevs.h"
#undef d

gx_device *gx_device_list[] = {
#define d(dev) &dev,
#include "gdevs.h"
#undef d
	0
};

/* Some C compilers insist on having executable code in every file, so: */
private void
gdevs_dummy()
{
}
