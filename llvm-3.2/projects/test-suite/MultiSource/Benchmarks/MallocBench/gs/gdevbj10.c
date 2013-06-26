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

/* gdevbj10.c */
/* Canon Bubble Jet BJ-10e printer driver for GhostScript */
#define GS_PRN_DEVICE gs_bj10_device
#define DEVICE_NAME "bj10"
#define PAGE_WIDTH_10THS 80L		/* 8" */
#define PAGE_HEIGHT_10THS 105L		/* 10.5" */
#define X_DPI 360			/* pixels per inch */
#define Y_DPI 360			/* pixels per inch */
#define PRN_OPEN gdev_prn_open
#define PRN_CLOSE gdev_prn_close
#define PRN_OUTPUT_PAGE bj10_output_page
#define DRIVER				/* see gdevprn.h */
#include "gdevprn.h"

/* Send the page to the printer. */
private int
bj10_output_page(gx_device *dev)
{	int code = gdev_prn_open_printer(dev);
	if ( code < 0 ) return code;

	/* print the accumulated page description */
	bj10_print_page(prn_dev, prn_dev->file);

	gdev_prn_close_printer(dev);
	return 0;
}

/* ------ Internal routines ------ */

/* Send the page to the printer. */
private int
bj10_print_page(gx_device_printer *pdev, FILE *prn_stream)
{	int line_size = mem_bytes_per_scan_line(&pdev->mem);
	byte *in = (byte *)gs_malloc(1, 48*line_size, "bj10_print_page(in)");
	byte *out = (byte *)gs_malloc(1, 48*line_size, "bj10_print_page(out)");
	char *cmp = gs_malloc(1, 18, "bj10_print_page(cmp)");
	int lnum;

	if ( in == 0 || out == 0 || cmp == 0 )
		return -1;
	memset((char *)cmp, 0, 18);

	/* I don't know how to initialize the printer -- */
	/* \033@ doesn't work. */

	/* Linewidth for 48 pixel lines */
	fprintf(prn_stream, "\033A%c\033\002", 8);

	/* Print lines with 48 pixel height of graphics */
	for ( lnum = 0; lnum < pdev->height; lnum += 48 )
	   {	byte *inp = in;
		byte *outp = out;
		byte *in_end = in + line_size;
		byte *out_beg = out;
		byte *out_end = out + 6*LINE_WIDTH;
		int count;

		mem_copy_scan_lines(&pdev->mem, lnum, inp, line_size*48);

		while ( inp < in_end )
		   {	int i;
			for ( i = 0; i < 6; i++, outp++ )
				gdev_prn_transpose_8x8(inp + 8*i*line_size,
							line_size, outp, 6);
			inp++;
			outp += 42;
		   }

		/* Remove trailing 0s. */
		while ( out_end - 6 >= out )
		   {	if ( memcmp(cmp, (char *)out_end-6, 6) != 0 )
				break;
			out_end -= 6;
		   }

		/* Remove leading 0s. */
		while ( out_beg + 18 <= out_end )
		   {	if( memcmp(cmp, (char *)out_beg, 18) != 0 )
				break;
			out_beg += 18;
		   }

		/* Transfer the bits */
		count = (out_end - out_beg) / 6;
		if ( count > 0 )
		   {    if ( out_beg > out )
			   {	putc(033, prn_stream);
				putc('d', prn_stream);	/* displace to right */
				putc( ((out_beg - out) / 18) & 0x0ff, prn_stream);
				putc( ((out_beg - out) / 18) >> 8, prn_stream);
			   }
			putc(033, prn_stream);
			putc('*', prn_stream);
			putc(48, prn_stream);
			putc(count & 0xff, prn_stream);
			putc(count >> 8, prn_stream);
			fwrite((char *)out_beg, 1, 6*count, prn_stream);
		   }
		putc(015, prn_stream);
		putc(012, prn_stream);
	   }

	/* Reinitialize the printer ?? */
	putc(014, prn_stream);	/* form feed */

	gs_free(in, 1, 48*line_size, "bj10_print_page(in)");
	gs_free(out, 1, 48*line_size, "bj10_print_page(out)");
	gs_free(cmp, 1, 18, "bj10_print_page(cmp)");
	return 0;
}
