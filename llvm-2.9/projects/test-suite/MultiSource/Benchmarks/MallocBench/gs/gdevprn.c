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

/* gdevprn.c */
/* Generic printer support for Ghostscript */
#include "gdevprn.h"

/* ------ Virtual memory buffer support ------ */

/* Bring-in procedure.  The first argument is &prn_dev->mem, */
/* from which we have to find &prn_dev->vmem. */
private int
prn_bring_in_rect(gx_device_memory *mdev, int x, int y, int w, int h,
  int writing)
{	/* "mdev + 1" because vmem follows mem in the device_printer struct */
	return vmem_bring_in_rect((gx_vmem *)(mdev + 1), x, y, w, h, writing);
}

/* Generic initialization for the printer device. */
/* Specific devices may wish to extend this. */
int
gdev_prn_open(gx_device *dev)
{	ulong space;
	byte *base;
	prn_dev->mem = mem_mono_device;
	/* In the initial implementation, */
	/* we buffer the entire page in memory. */
	prn_dev->mem.width = dev->width;
	prn_dev->mem.height = dev->height;
	if ( prn_dev->vmem_space )	/* only allocate pointer table */
		space = dev->height * sizeof(byte **);
	else				/* allocate entire buffer */
	   {	space = gx_device_memory_bitmap_size(&prn_dev->mem);
		if ( space > (uint)(-4) ) return -1;	/* can't allocate */
	   }
	base = (byte *)gs_malloc((uint)space, 1, "printer buffer");
	if ( base == 0 ) return -1;
	prn_dev->mem.base = base;
	if ( (*prn_dev->mem.procs->open_device)((gx_device *)&prn_dev->mem) < 0 )
		return -1;
	if ( prn_dev->vmem_space )
	   {	int line_size = (prn_dev->width + 7) >> 3;
		prn_dev->mem.bring_in_proc = prn_bring_in_rect;
		vmem_init(&prn_dev->vmem, (char **)prn_dev->mem.line_ptrs,
		  line_size, prn_dev->height, 0,
		  (int)(prn_dev->x_pixels_per_inch / 8 * line_size),
		  max(prn_dev->vmem_space, line_size + 4),
		  gs_malloc, gs_free);
	   }
	printf("Printing in memory, please be patient.\n");
	return 0;
}

/* Generic closing for the printer device. */
/* Specific devices may wish to extend this. */
int
gdev_prn_close(gx_device *dev)
{	return ( prn_dev->vmem_space ? vmem_close(&prn_dev->vmem) : 0 );
}

/* Map a r-g-b "color" to a printer color: 0 = white, 1 = black. */
gx_color_index
gdev_prn_map_rgb_color(gx_device *dev, ushort r, ushort g, ushort b)
{	return ((r | g | b) ? 0 : 1);
}

/* Map the printer color back to black or white */
int
gdev_prn_map_color_rgb(gx_device *dev, gx_color_index color, ushort *prgb)
{	prgb[0] = prgb[1] = prgb[2] = (color ? 0 : 1);
	return 0;
}

/* Delegate the painting operations to the memory device. */
int
gdev_prn_fill_rectangle(gx_device *dev, int x, int y, int w, int h,
  gx_color_index color)
{	return (*prn_dev->mem.procs->fill_rectangle)
	  ((gx_device *)&prn_dev->mem, x, y, w, h, color);
}
int
gdev_prn_copy_mono(gx_device *dev, byte *data,
  int dx, int raster, int x, int y, int w, int h,
  gx_color_index zero, gx_color_index one)
{	return (*prn_dev->mem.procs->copy_mono)
	  ((gx_device *)&prn_dev->mem, data, dx, raster, x, y, w, h, zero, one);
}
int
gdev_prn_copy_color(gx_device *dev, byte *data,
  int dx, int raster, int x, int y, int w, int h)
{	return (*prn_dev->mem.procs->copy_color)
	  ((gx_device *)&prn_dev->mem, data, dx, raster, x, y, w, h);
}

/* ------ Driver services ------ */

/* Open the printer device. */
int
gdev_prn_open_printer(gx_device *dev)
{	if ( prn_dev->write_to_prn )
	   {
#ifdef __MSDOS__
		int fno = fileno(stdprn);
		int ctrl = ioctl(fno, 0);
		ioctl(fno, 1, (ctrl | 0x20) & 0xff); /* no ^Z intervention! */
		prn_dev->file = stdprn;
		printf("Printing from memory to printer.\n");
#else
		printf("Can't print directly from memory to printer yet.\n");
#endif
	   }
	else
	   {	char *fname = prn_dev->fname;
		strcpy(fname, SCRATCH_TEMPLATE);
		mktemp(fname);
		prn_dev->file = fopen(fname, "wb+");
		if ( prn_dev->file == NULL )
		   {	eprintf1("could not open the printer file %s!\n", fname);
			return -1;
		   }
		printf("Printing from memory to file %s.\n", fname);
	   }
	return 0;
}

/* Transpose an 8 x 8 block of bits.  line_size is the raster of */
/* the input data.  dist is the distance between output bytes. */
/* Not all printers need this, but some of the common ones do. */
void
gdev_prn_transpose_8x8(byte *inp, register int line_size, byte *outp,
  register int dist)
{	byte *inp4 = inp + (line_size << 2);
	register ushort ae = ((ushort)*inp << 8) + *inp4;
	register ushort bf = ((ushort)*(inp += line_size) << 8) +
						*(inp4 += line_size);
	register ushort cg = ((ushort)*(inp += line_size) << 8) +
						*(inp4 += line_size);
	register ushort dh = ((ushort)inp[line_size] << 8) +
						inp4[line_size];
	register ushort temp;

/* Transpose blocks of 4 x 4 */
#define transpose4(r)\
  r = (r & 0xf00f) + ((r & 0x0f00) >> 4) + ((r & 0x00f0) << 4)
	transpose4(ae);
	transpose4(bf);
	transpose4(cg);
	transpose4(dh);

/* Transpose blocks of 2 x 2 */
#define transpose(r,s,mask,shift)\
  temp = s;\
  s = (s & mask) + ((r & mask) << shift);\
  r = (r & ~mask) + ((temp >> shift) & mask)
	transpose(ae, cg, 0x3333, 2);
	transpose(bf, dh, 0x3333, 2);

/* Transpose blocks of 1 x 1 */
	transpose(ae, bf, 0x5555, 1);
	transpose(cg, dh, 0x5555, 1);

	*outp = ae >> 8;
	outp += dist;
	*outp = bf >> 8;
	outp += dist;
	*outp = cg >> 8;
	outp += dist;
	*outp = dh >> 8;
	outp += dist;
	*outp = (byte)ae;		/* low-order byte */
	outp += dist;
	*outp = (byte)bf;		/* ditto */
	outp += dist;
	*outp = (byte)cg;		/* ditto */
	outp += dist;
	*outp = (byte)dh;		/* ditto */
}

/* Flush the printer output from the scratch file, */
/* and close the printer device. */
int
gdev_prn_close_printer(gx_device *dev)
{	if ( !prn_dev->write_to_prn )
	   {	fclose(prn_dev->file);
		printf("To print the page, copy the file %s to the printer\n",
			prn_dev->fname);
	   }
	return 0;
}
