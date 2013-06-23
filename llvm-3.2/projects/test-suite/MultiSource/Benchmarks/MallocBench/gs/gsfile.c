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

/* gsfile.c */
/* Bitmap file writing routines for GhostScript library */
#include "memory_.h"
#include "gx.h"
#include "gserrors.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxdevice.h"
#include "gxdevmem.h"

/* Dump the contents of a memory device in PPM format. */
int
gs_writeppmfile(gx_device_memory *md, FILE *file)
{	int raster = mem_bytes_per_scan_line(md);
	int height = md->height;
	int depth = md->bits_per_color_pixel;
	uint rsize = raster * 3;	/* * 3 just for mapped color */
	byte *row = (byte *)gs_malloc(rsize, 1, "ppm file buffer");
	char *header;
	int y;
	int code = 0;			/* return code */
	if ( row == 0 )			/* can't allocate row buffer */
		return_error(gs_error_VMerror);

	/* A PPM file consists of: magic number, comment, */
	/* width, height, maxvalue, (r,g,b).... */

	/* Dispatch on the type of the device -- 1-bit mono, */
	/* 8-bit (gray scale or color), 24- or 32-bit true color. */

	switch ( depth )
	   {
	case 1:
	  header = "P4\n# Ghostscript 1 bit mono image dump\n%d %d\n";
	  break;

	case 8:
	  header = (md->has_color ?
	    "P6\n# Ghostscript 8 bit mapped color image dump\n%d %d\n255\n" :
	    "P5\n# Ghostscript 8 bit gray scale image dump\n%d %d\n255\n");
	  break;

	case 24:
	  header = "P6\n# Ghostscript 24 bit color image dump\n%d %d\n255\n";
	  break;

	case 32:
	  header = "P6\n# Ghostscript 32 bit color image dump\n%d %d\n255\n";
	  break;

	default:			/* shouldn't happen! */
	  code = gs_error_undefinedresult;
	  goto done;
	   }

	/* Write the header. */
	fprintf(file, header, md->width, height);

	/* Dump the contents of the image. */
	for ( y = 0; y < height; y++ )
	   {	int count;
		register byte *from, *to, *end;
		mem_copy_scan_lines(md, y, row, raster);
		switch ( depth )
		   {
		case 8:
		   {	/* Mapped color, consult the map. */
			if ( md->has_color )
			   {	/* Map color */
				byte *palette = md->palette;
				from = row + raster + raster;
				memcpy(from, row, raster);
				to = row;
				end = from + raster;
				while ( from < end )
				   {	register byte *cp = palette + (int)*from++ * 3;
					*to++ = *cp++;	/* red */
					*to++ = *cp++;	/* green */
					*to++ = *cp++;	/* blue */
				   }
				count = raster * 3;
			   }
			else
			   {	/* Map gray scale */
				register byte *palette = md->palette;
				from = to = row, end = row + raster;
				while ( from < end )
					*to++ = palette[(int)*from++ * 3];
				count = raster;
			   }
		   }
			break;
		case 32:
		   {	/* This case is different, because we must skip */
			/* every fourth byte. */
			from = to = row, end = row + raster;
			while ( from < end )
			   {	from++;		/* skip unused byte */
				*to++ = *from++;	/* red */
				*to++ = *from++;	/* green */
				*to++ = *from++;	/* blue */
			   }
			count = to - row;
		   }
			break;
		default:
			count = raster;
		   }
		if ( fwrite(row, 1, count, file) < count )
		   {	code = gs_error_ioerror;
			goto done;
		   }
	   }

done:	gs_free((char *)row, rsize, 1, "ppm file buffer");
	return code;
}
