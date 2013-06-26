/* Copyright (C) 1989, 1990, 1991 Aladdin Enterprises.  All rights reserved.
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

/* gdevdjet.c */
/* H-P DeskJet/LaserJet driver for GhostScript */

/* Define whether we are writing to a DeskJet (which can do bitmap */
/* compression) or a LaserJet (which can't). */
/* Normally this is defined in the compiler command line -- */
/* see the DeskJet and LaserJet entries in gdevs.mak. */
#ifndef LASER
#  define LASER 0
#endif

#if LASER
#  define GS_PRN_DEVICE gs_laserjet_device
#  define DEVICE_NAME "laserjet"
#else
#  define GS_PRN_DEVICE gs_deskjet_device
#  define DEVICE_NAME "deskjet"
#endif
#define PAGE_WIDTH_10THS 80		/* not quite a full page */
#define PAGE_HEIGHT_10THS 105		/* ditto */
/* Normally we set up at 300 DPI; however, on MS-DOS systems with */
/* virtual memory disabled, we can't do more than 60 DPI */
/* because the maximum buffer is only 64K. */
#define X_DPI 300
#define Y_DPI 300
#ifdef __MSDOS__
#  if !VMEM_SPACE
#    undef X_DPI
#    define X_DPI 60
#    undef Y_DPI
#    define Y_DPI 60
#  endif
#endif
#define PRN_OPEN gdev_prn_open
#define PRN_CLOSE gdev_prn_close
#define PRN_OUTPUT_PAGE djet_output_page
#define DRIVER				/* see gdevprn.h */
#include "gdevprn.h"

/* Forward references */
private int djet_print_page(P2(gx_device_printer *, FILE *));

/* Send the page to the printer. */
private int
djet_output_page(gx_device *dev)
{	int code = gdev_prn_open_printer(dev);
	if ( code < 0 ) return code;

	/* print the accumulated page description */
	djet_print_page(prn_dev, prn_dev->file);

	gdev_prn_close_printer(dev);
	return 0;
}

/* ------ Internal routines ------ */

/* Send the page to the printer.  For the DeskJet, compress each scan line, */
/* since computer-to-printer communication time is often a bottleneck. */
private int
djet_print_page(gx_device_printer *pdev, FILE *prn_stream)
{	char data[LINE_SIZE + 4];

#define escape_print(str) fprintf(prn_stream, str, 27)
#define escape_print_1(str,arg) fprintf(prn_stream, str, 27, arg)

	/* ends raster graphics to set raster graphics resolution */
	escape_print("%c*rB");

	/* set raster graphics resolution -- 75 or 300 dpi */
#if X_DPI == 300
	escape_print("%c*t300R");
#else
	escape_print("%c*t75R");
#endif

	/* move to top left of page */
	escape_print("%c*p0x0Y");

	/* start raster graphics */
	escape_print("%c*r0A");

	/* select the most compressed mode available */
#if !LASER				/* LaserJet can't handle it */
	escape_print("%c*b2M");
#endif

	/* Send each scan line in turn */
	   {	int lnum;
		int line_size = mem_bytes_per_scan_line(&pdev->mem);
		int num_blank_lines = 0;
		for ( lnum = 0; lnum < pdev->height; lnum++ )
		   {	char *end_data = data + LINE_SIZE;
			mem_copy_scan_lines(&pdev->mem, lnum,
					    (byte *)data, line_size);
			/* Mask off 1-bits beyond LINE_WIDTH. */
#if (LINE_WIDTH & 7)
			end_data[-1] &= (byte)(0xff00 >> (LINE_WIDTH & 7));
#endif
			/* Remove trailing 0s. */
			while ( end_data > data && end_data[-1] == 0 )
				end_data--;
			if ( end_data == data )
			   {	/* Blank line */
				num_blank_lines++;
			   }
			else
			   {
#if LASER				/* no compression */
				char *out_data = data;
				int out_count = end_data - data;
#else
				char out_data[LINE_SIZE + LINE_SIZE / 127 + 1];
				int out_count =
				  compress_row(data, end_data, out_data);
#endif

				/* Skip blank lines if any */
				if ( num_blank_lines > 0 )
				   {	/* move down from current position */
					escape_print_1("%c*p+%dY",
						       num_blank_lines);
					num_blank_lines = 0;
				   }

				/* transfer raster graphics */
				escape_print_1("%c*b%dW", out_count);

				/* send the row */
				fwrite(out_data, sizeof(char),
				       out_count, prn_stream);
			   }
		   }
	}

	/* end raster graphics */
	escape_print("%c*rB");

	/* eject page */
	escape_print("%c&l0H");

	return 0;
}

#if !LASER

/* Row compression routine for the HP DeskJet. */
/* Compresses data from row up to end_row, storing the result */
/* starting at compressed.  Returns the number of bytes stored. */
/* Runs of K<=127 literal bytes are encoded as K-1 followed by */
/* the bytes; runs of 2<=K<=127 identical bytes are encoded as */
/* 257-K followed by the byte. */
/* In the worst case, the result is N+(N/127)+1 bytes long, */
/* where N is the original byte count (end_row - row). */
int
compress_row(char *row, char *end_row, char *compressed)
{	register char *i_exam = row; /* byte being examined in the row to compress */
	char *stop_exam = end_row - 4; /* stop scanning for similar bytes here */
	register char *cptr = compressed; /* output pointer into compressed bytes */

	while ( i_exam < end_row )
	   {	/* Search ahead in the input looking for a run */
		/* of at least 4 identical bytes. */
		char *i_compr = i_exam;
		char *i_next;		/* end of run */
		char byte_value;
		while ( i_exam <= stop_exam &&
			((byte_value = *i_exam) != i_exam[1] ||
			 byte_value != i_exam[2] ||
			 byte_value != i_exam[3]) )
		  i_exam++;

		/* Find out how long the run is */
		if ( i_exam > stop_exam )	/* no run */
			i_next = i_exam = end_row;
		else
		   {	i_next = i_exam + 4;
			while ( i_next < end_row && *i_next == byte_value )
				i_next++;
		   }

		/* Now [i_compr..i_exam) should be encoded as dissimilar, */
		/* and [i_exam..i_next) should be encoded as similar. */
		/* Note that either of these ranges may be empty. */

		while ( i_compr < i_exam )
		   {	/* Encode up to 127 dissimilar bytes */
			int count = i_exam - i_compr;
			if ( count > 127 ) count = 127;
			*cptr++ = count - 1;
			while ( count > 0 )
			   {	*cptr++ = *i_compr++;
				count--;
			   }
		   }

		while ( i_exam < i_next )
		   {	/* Encode up to 127 similar bytes */
			int count = i_next - i_exam;
			if ( count > 127 ) count = 127;
			*cptr++ = 1 - count;
			*cptr++ = byte_value;
			i_exam += count;
		   }
	   }
	return (cptr - compressed);
}

#endif					/* !LASER */
