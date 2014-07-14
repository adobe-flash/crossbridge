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

/* gdevmem.c */
/* "Memory" (stored bitmap) device for Ghostscript library. */
#include "memory_.h"
#include "gs.h"
#include "arch.h"
#include "gxbitmap.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxdevice.h"
#include "gxdevmem.h"

typedef struct gx_device_s gx_device;

/*
   The obvious representation for a "memory" device is simply a
   contiguous bitmap stored in something like the PostScript
   representation, i.e., each scan line (in left-to-right order), padded
   to a byte or word boundary, followed immediately by the next one.
   Unfortunately, we can't use this representation, for two reasons:

	- On PCs with segmented architectures, there is no way to
	  obtain a contiguous block of storage larger than 64K bytes,
	  which isn't big enough for a full-screen bitmap, even in
	  monochrome.

	- The representation of strings in the Ghostscript
	  interpreter limits the size of a string to 64K-1 bytes,
	  which means we can't simply use a string for the contents
	  of a memory device.

   We get around the former problem by representing a memory device
   as an array of strings: each string holds one scan line.
   We get around the latter problem by making the client read out the
   contents of a memory device bitmap in pieces.
*/

/* ------ Generic macros ------ */

/* Macro for declaring the essential device procedures. */
#define declare_mem_map_procs(map_rgb_color, map_color_rgb)\
  private dev_proc_map_rgb_color(map_rgb_color);\
  private dev_proc_map_color_rgb(map_color_rgb)
#define declare_mem_procs(copy_mono, copy_color, fill_rectangle)\
  private dev_proc_copy_mono(copy_mono);\
  private dev_proc_copy_color(copy_color);\
  private dev_proc_fill_rectangle(fill_rectangle)

/* Macro for generating the procedure record in the device descriptor */
#define mem_procs(map_rgb_color, map_color_rgb, copy_mono, copy_color, fill_rectangle)\
{	mem_open,\
	mem_get_initial_matrix,\
	gx_default_sync_output,\
	gx_default_output_page,\
	gx_default_close_device,\
	map_rgb_color,			/* differs */\
	map_color_rgb,			/* differs */\
	fill_rectangle,			/* differs */\
	gx_default_tile_rectangle,\
	copy_mono,			/* differs */\
	copy_color,			/* differs */\
	gx_default_draw_line,\
	gx_default_fill_trapezoid,\
	gx_default_tile_trapezoid\
}

/* Macro for generating the device descriptor */
/* The "& 15" in max_value is bogus, to keep certain compilers */
/* from complaining about a left shift by 32. */
#define max_value(depth) (depth > 8 ? 255 : (1 << (depth & 15)) - 1)
#define mem_device(name, depth, procs)\
{	sizeof(gx_device_memory),\
	&procs,			/* differs */\
	name,			/* differs */\
	0, 0,			/* x and y extent (filled in) */\
	1, 1,			/* density (irrelevant) */\
	(depth > 1),		/* has_color */\
	max_value(depth),	/* max_rgb */\
	depth,			/* depth differs */\
	0,			/* not open yet */\
	identity_matrix_body,	/* initial matrix (filled in) */\
	0,			/* raster (filled in) */\
	(byte *)0,		/* base (filled in) */\
	(byte **)0,		/* line_ptrs (filled in by 'open') */\
	mem_no_fault_proc,	/* default bring_in_proc */\
	0,			/* invert (filled in for mono) */\
	0, (byte *)0		/* palette (filled in for color) */\
}

/* Macro for casting gx_device argument */
#define mdev ((gx_device_memory *)dev)

/* Macro for rectangle arguments (x,y,w,h) */
#define check_rect()\
	if ( w <= 0 || h <= 0 ) return 0;\
	if ( x < 0 || x > mdev->width - w || y < 0 || y > mdev->height - h )\
		return -1

/* Macros for processing bitmaps in the largest possible chunks. */
/* Since bits within a byte are always stored big-endian, */
/* we can only use chunks larger than a byte on big-endian machines. */
/* Note that we use type uint for register variables holding a chunk: */
/* for this reason, we only use larger chunks if !ints_are_short. */
#if !big_endian || ints_are_short
#  define log2_chunk_bits 3
#  define chunk byte
#else
#  define log2_chunk_bits 5
#  define chunk ulong
#endif
/* Now generic macros defined in terms of the above. */
#define chunk_bits (1<<log2_chunk_bits)
#define chunk_bytes (chunk_bits/8)
#define chunk_bit_mask (chunk_bits-1)
#define chunk_hi_bit ((chunk)1<<(chunk_bits-1))
#define chunk_all_bits ((chunk_hi_bit<<1)-1)
#define chunk_hi_bits(n) (chunk_all_bits-(chunk_all_bits>>(n)))

/* Macros for scan line access. */
/* x_to_byte is different for each number of bits per pixel. */
/* Note that these macros depend on the definition of scan_chunk: */
/* each procedure that uses the scanning macros should #define */
/* (not typedef) scan_chunk as either chunk or byte. */
#define scan_chunk_bytes sizeof(scan_chunk)
#define log2_scan_chunk_bytes (scan_chunk_bytes >> 1)	/* works for 1,2,4 */
#define declare_scan_line(line,ptr)\
	byte **line; register scan_chunk *ptr
#define declare_scan_ptr(line,ptr,offset)\
	byte **line; scan_chunk *ptr; int offset
#define setup_scan_ptr(line,ptr,offset)\
	ptr = (scan_chunk *)((*line) + (offset))
#define setup_scan(line,ptr,offset)\
	line = mdev->line_ptrs + (y);\
	setup_scan_ptr(line,ptr,offset)
#define next_scan_line(line,ptr,offset)\
	++line; setup_scan_ptr(line,ptr,offset)
#define setup_rect(line,ptr,offset)\
	offset = x_to_byte(x) & -scan_chunk_bytes;\
	setup_scan(line,ptr,offset)

/* ------ Generic code ------ */

/* Compute the size of the bitmap storage, */
/* including the space for the scan line index. */
/* Note that scan lines are padded to a multiple of 4 bytes. */
ulong
gx_device_memory_bitmap_size(gx_device_memory *dev)
{	unsigned raster =
		((mdev->width * mdev->bits_per_color_pixel + 31) >> 5) << 2;
	mdev->raster = raster;
	return (ulong)mdev->height * (raster + sizeof(byte *));
}

/* 'Open' the memory device by creating the index table if needed. */
private int
mem_open(gx_device *dev)
{
#ifdef __MSDOS__			/* ****** NOTA BENE ****** */
#  include <dos.h>
#  define make_huge_ptr(ptr)\
    ((byte huge *)MK_FP(FP_SEG(ptr), 0) + FP_OFF(ptr))
	byte huge *scan_line = make_huge_ptr(mdev->base);
#else					/* ****** ****** */
	byte *scan_line = mdev->base;
#endif					/* ****** ****** */
	byte **pptr = (byte **)(scan_line + (ulong)mdev->height * mdev->raster);
	byte **pend = pptr + mdev->height;
	mdev->line_ptrs = pptr;
	while ( pptr != pend )
	   {	*pptr++ = (byte *)scan_line;
		scan_line += mdev->raster;
	   }	
	return 0;
}

/* Return the initial transformation matrix */
void
mem_get_initial_matrix(gx_device *dev, gs_matrix *pmat)
{	*pmat = mdev->initial_matrix;
}

/* Test whether a device is a memory device */
int
gs_device_is_memory(gx_device *dev)
{	/* We can't just compare the procs, or even an individual proc, */
	/* because we might be tracing.  Compare the device name, */
	/* and hope for the best. */
	char *name = dev->name;
	int i;
	for ( i = 0; i < 6; i++ )
	  if ( name[i] != "image("[i] ) return 0;
	return 1;
}

/* Compute the number of data bytes per scan line. */
/* Note that this does not include the padding. */
int
mem_bytes_per_scan_line(gx_device_memory *dev)
{	return (dev->width * dev->bits_per_color_pixel + 7) >> 3;
}

/* Copy one or more scan lines to a client. */
#undef scan_chunk
#define scan_chunk byte
int
mem_copy_scan_lines(gx_device_memory *dev, int start_y, byte *str, uint size)
{	declare_scan_ptr(src_line, src, offset);
	uint bytes_per_line = mem_bytes_per_scan_line(dev);
	byte *dest = str;
	int y = start_y;
	uint count = min(size / bytes_per_line, dev->height - y);
	while ( (*dev->bring_in_proc)(dev, 0, y, bytes_per_line, count, 0) < 0 )
	   {	/* We can only split in Y, not in X. */
		uint part = count >> 1;
		uint part_size = part * bytes_per_line;
		mem_copy_scan_lines(dev, y, dest, part_size);
		dest += part_size;
		y += part;
		count -= part;
	   }
	setup_scan(src_line, src, 0);
	while ( count-- != 0 )
	   {	memcpy(dest, src, bytes_per_line);
		next_scan_line(src_line, src, 0);
		dest += bytes_per_line;
		y++;
	   }
	return y - start_y;
}

/* ------ Page fault recovery ------ */

/* Default (no-fault) bring_in_proc */
private int
mem_no_fault_proc(gx_device_memory *dev,
  int x, int y, int w, int h, int writing)
{	return 0;
}

/* Recover from bring_in_proc failure in fill_rectangle */
#define check_fault_fill(byte_x, byte_count)\
  if ( mdev->bring_in_proc != mem_no_fault_proc )\
   {	int fault = (*mdev->bring_in_proc)(mdev, byte_x, y, byte_count, h, 1);\
	if ( fault < 0 )\
		return mem_fill_recover(dev, x, y, w, h, color, fault);\
   }
private int
mem_fill_recover(gx_device *dev, int x, int y, int w, int h,
  gx_color_index color, int fault)
{	int nx = x, nw = w, ny = y, nh = h;
	switch ( fault )
	   {
	case mem_fault_split_X:
		nx += (nw >>= 1), w -= nw;
		break;
	case mem_fault_split_Y:
		ny += (nh >>= 1), h -= nh;
		break;
	default:
		return fault;
	   }
	(*dev->procs->fill_rectangle)(dev, x, y, w, h, color);
	return (*dev->procs->fill_rectangle)(dev, nx, ny, nw, nh, color);
}

/* Recover from bring_in_proc failure in copy_mono */
#define check_fault_copy_mono(byte_x, byte_count)\
  if ( mdev->bring_in_proc != mem_no_fault_proc )\
   {	int fault = (*mdev->bring_in_proc)(mdev, byte_x, y, byte_count, h, 1);\
	if ( fault < 0 )\
		return mem_copy_mono_recover(dev, base, sourcex, raster,\
				x, y, w, h, zero, one, fault);\
   }
private int
mem_copy_mono_recover(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one,
  int fault)
{	int nx = x, nw = w, ny = y, nh = h;
	switch ( fault )
	   {
	case mem_fault_split_X:
		nx += (nw >>= 1), w -= nw;
		break;
	case mem_fault_split_Y:
		ny += (nh >>= 1), h -= nh;
		break;
	default:
		return fault;
	   }
	(*dev->procs->copy_mono)(dev, base, sourcex, raster,
				 x, y, w, h, zero, one);
	return (*dev->procs->copy_mono)(dev, base, sourcex, raster,
					nx, ny, nw, nh, zero, one);
}

/* Recover from bring_in_proc failure in copy_color */
#define check_fault_copy_color(byte_x, byte_count)\
  if ( mdev->bring_in_proc != mem_no_fault_proc )\
   {	int fault = (*mdev->bring_in_proc)(mdev, byte_x, y, byte_count, h, 1);\
	if ( fault < 0 )\
		return mem_copy_color_recover(dev, base, sourcex, raster,\
				x, y, w, h, fault);\
   }
private int
mem_copy_color_recover(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h, int fault)
{	int nx = x, nw = w, ny = y, nh = h;
	switch ( fault )
	   {
	case mem_fault_split_X:
		nx += (nw >>= 1), w -= nw;
		break;
	case mem_fault_split_Y:
		ny += (nh >>= 1), h -= nh;
		break;
	default:
		return fault;
	   }
	(*dev->procs->copy_color)(dev, base, sourcex, raster,
				 x, y, w, h);
	return (*dev->procs->copy_color)(dev, base, sourcex, raster,
					 nx, ny, nw, nh);
}

/* ------ Monochrome ------ */

/* Procedures */
declare_mem_procs(mem_mono_copy_mono, mem_mono_copy_color, mem_mono_fill_rectangle);

/* The device descriptor. */
private gx_device_procs mem_mono_procs =
  mem_procs(gx_default_map_rgb_color, gx_default_map_color_rgb,
    mem_mono_copy_mono, mem_mono_copy_color, mem_mono_fill_rectangle);

/* The instance is public. */
gx_device_memory mem_mono_device =
  mem_device("image(mono)", 1, mem_mono_procs);

/* Convert x coordinate to byte offset in scan line. */
#define x_to_byte(x) ((x) >> 3)

/* Fill a rectangle with a color. */
#undef scan_chunk
#define scan_chunk chunk
private int
mem_mono_fill_rectangle(gx_device *dev, int x, int y, int w, int h,
  gx_color_index color)
{	uint bit;
	chunk right_mask;
	byte fill;
	declare_scan_ptr(dest_line, dest, offset);
	check_fault_fill(x >> 3, ((x + w + 7) >> 3) - (x >> 3));
	check_rect();
	setup_rect(dest_line, dest, offset);
#define write_loop(stat)\
 { int line_count = h;\
   declare_scan_line(ptr_line, ptr);\
   ptr_line = dest_line;\
   setup_scan_ptr(ptr_line, ptr, offset);\
   do { stat; next_scan_line(ptr_line, ptr, offset); }\
   while ( --line_count );\
 }
#define write_partial(msk)\
   if ( fill ) write_loop(*ptr |= msk)\
   else write_loop(*ptr &= ~msk)
	switch ( color )
	   {
	case 0: fill = mdev->invert; break;
	case 1: fill = ~mdev->invert; break;
	case gx_no_color_index: return 0;		/* transparent */
	default: return -1;		/* invalid */
	   }
	bit = x & chunk_bit_mask;
	if ( bit + w <= chunk_bits )
	   {	/* Only one word */
		right_mask = chunk_hi_bits(w) >> bit;
	   }
	else
	   {	int byte_count;
		if ( bit )
		   {	chunk mask = chunk_all_bits >> bit;
			write_partial(mask);
			offset += chunk_bytes;
			w += bit - chunk_bits;
		   }
		right_mask = chunk_hi_bits(w & chunk_bit_mask);
		if ( (byte_count = (w >> 3) & -chunk_bytes) != 0 )
		   {	write_loop(memset(ptr, fill, byte_count));
			offset += byte_count;
		   }
	   }
	if ( right_mask )
		write_partial(right_mask);
	return 0;
}

/* Copy a monochrome bitmap. */
#undef scan_chunk
#define scan_chunk chunk
private int
mem_mono_copy_mono(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	chunk *line;
	int sleft, dleft;
	uint mask, rmask;
	uint invert, zmask, omask;
	declare_scan_ptr(dest_line, dest, offset);
#define izero (int)zero
#define ione (int)one
	if ( ione == izero )		/* vacuous case */
		return mem_mono_fill_rectangle(dev, x, y, w, h, zero);
	check_fault_copy_mono(x >> 3, ((x + w + 7) >> 3) - (x >> 3));
	check_rect();
	setup_rect(dest_line, dest, offset);
	line = (chunk *)base + (sourcex >> log2_chunk_bits);
	sleft = chunk_bits - (sourcex & chunk_bit_mask);
	dleft = chunk_bits - (x & chunk_bit_mask);
	mask = chunk_all_bits >> (chunk_bits - dleft);
	if ( w < dleft )
		mask -= mask >> w;
	else
		rmask = chunk_hi_bits((w - dleft) & chunk_bit_mask);
/* Macros for writing partial chunks. */
/* bits has already been inverted by xor'ing with invert. */
#define write_chunk_masked(ptr, bits, mask)\
  *ptr = ((bits | ~mask | zmask) & *ptr | (bits & mask & omask))
#define write_chunk(ptr, bits)\
  *ptr = ((bits | zmask) & *ptr | (bits & omask))
	if ( mdev->invert )
	   {	if ( izero != (int)gx_no_color_index ) zero ^= 1;
		if ( ione != (int)gx_no_color_index ) one ^= 1;
	   }
	invert = (izero == 1 || ione == 0 ? -1 : 0);
	zmask = (izero == 0 || ione == 0 ? 0 : -1);
	omask = (izero == 1 || ione == 1 ? -1 : 0);
#undef izero
#undef ione
	if ( sleft == dleft )		/* optimize the aligned case */
	   {	w -= dleft;
		while ( --h >= 0 )
		   {	register chunk *bptr = line;
			int count = w;
			register chunk *optr = dest;
			register uint bits = *bptr ^ invert;	/* first partial chunk */
			write_chunk_masked(optr, bits, mask);
			/* Do full chunks. */
			while ( (count -= chunk_bits) >= 0 )
			   {	bits = *++bptr ^ invert;
				++optr;
				write_chunk(optr, bits);
			   }
			/* Do last chunk */
			if ( count > -chunk_bits )
			   {	bits = *++bptr ^ invert;
				++optr;
				write_chunk_masked(optr, bits, rmask);
			   }
			next_scan_line(dest_line, dest, offset);
			line = (chunk *)((byte *)line + raster);
		   }
	   }
	else
	   {	int skew = (sleft - dleft) & chunk_bit_mask;
		int cskew = chunk_bits - skew;
		while ( --h >= 0 )
		   {	chunk *bptr = line;
			int count = w;
			chunk *optr = dest;
			register int bits;
			/* Do the first partial chunk */
			if ( sleft >= dleft )
			   {	bits = *bptr >> skew;
			   }
			else /* ( sleft < dleft ) */
			   {	bits = *bptr++ << cskew;
				if ( count > sleft )
					bits += *bptr >> skew;
			   }
			bits ^= invert;
			write_chunk_masked(optr, bits, mask);
			count -= dleft;
			optr++;
			/* Do full chunks. */
			while ( count >= chunk_bits )
			   {	bits = *bptr++ << cskew;
				bits += *bptr >> skew;
				bits ^= invert;
				write_chunk(optr, bits);
				count -= chunk_bits;
				optr++;
			   }
			/* Do last chunk */
			if ( count > 0 )
			   {	bits = *bptr++ << cskew;
				if ( count > skew ) bits += *bptr >> skew;
				bits ^= invert;
				write_chunk_masked(optr, bits, rmask);
			   }
			next_scan_line(dest_line, dest, offset);
			line = (chunk *)((byte *)line + raster);
		   }
	   }
	return 0;
}

/* Copy a "color" bitmap.  Since "color" is the same as monochrome, */
/* this just reduces to copying a monochrome bitmap. */
private int
mem_mono_copy_color(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h)
{	return mem_mono_copy_mono(dev, base, sourcex, raster, x, y, w, h,
	  (gx_color_index)0, (gx_color_index)1);
}

/* ------ Color (mapped or true) ------ */

/* Copy a rectangle of bytes from a source to a destination. */
#undef scan_chunk
#define scan_chunk byte
private int
copy_byte_rect(gx_device *dev, byte *source, int sraster,
  int offset, int y, int w, int h)
{	declare_scan_line(dest_line, dest);
	setup_scan(dest_line, dest, offset);
	while ( h-- > 0 )
	   {	memcpy(dest, source, w);
		source += sraster;
		next_scan_line(dest_line, dest, offset);
	   }
	return 0;
}

/* ------ Mapped 8-bit color ------ */

/* Procedures */
declare_mem_map_procs(mem_mapped_map_rgb_color, mem_mapped_map_color_rgb);
declare_mem_procs(mem_mapped_copy_mono, mem_mapped_copy_color, mem_mapped_fill_rectangle);

/* The device descriptor. */
private gx_device_procs mem_mapped_procs =
  mem_procs(mem_mapped_map_rgb_color, mem_mapped_map_color_rgb,
    mem_mapped_copy_mono, mem_mapped_copy_color, mem_mapped_fill_rectangle);

/* The instance is public. */
gx_device_memory mem_mapped_color_device =
  mem_device("image(8)", 8, mem_mapped_procs);

/* Convert x coordinate to byte offset in scan line. */
#undef x_to_byte
#define x_to_byte(x) (x)

/* Map a r-g-b color to a color index. */
/* This requires searching the palette. */
private gx_color_index
mem_mapped_map_rgb_color(gx_device *dev, ushort r, ushort g, ushort b)
{	register byte *pptr = mdev->palette;
	int cnt = mdev->palette_size;
	byte *which;
	int best = 256*3;
	while ( cnt-- > 0 )
	   {	register int diff = *pptr - r;
		if ( diff < 0 ) diff = -diff;
		if ( diff < best )	/* quick rejection */
		   {	int dg = pptr[1] - g;
			if ( dg < 0 ) dg = -dg;
			if ( (diff += dg) < best )	/* quick rejection */
			   {	int db = pptr[2] - b;
				if ( db < 0 ) db = -db;
				if ( (diff += db) < best )
					which = pptr, best = diff;
			   }
		   }
		pptr += 3;
	   }
	return (gx_color_index)((which - mdev->palette) / 3);
}

/* Map a color index to a r-g-b color. */
private int
mem_mapped_map_color_rgb(gx_device *dev, gx_color_index color, ushort *prgb)
{	byte *pptr = mdev->palette + (int)color * 3;
	prgb[0] = pptr[0];
	prgb[1] = pptr[1];
	prgb[2] = pptr[2];
	return 0;
}

/* Fill a rectangle with a color. */
private int
mem_mapped_fill_rectangle(gx_device *dev,
  int x, int y, int w, int h, gx_color_index color)
{	declare_scan_ptr(dest_line, dest, offset);
	check_fault_fill(x, w);
	setup_rect(dest_line, dest, offset);
	while ( h-- > 0 )
	   {	memset(dest, (byte)color, w);
		next_scan_line(dest_line, dest, offset);
	   }
	return 0;
}

/* Copy a monochrome bitmap. */
private int
mem_mapped_copy_mono(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	byte *line;
	int first_bit;
	declare_scan_ptr(dest_line, dest, offset);
	check_fault_copy_mono(x, w);
	setup_rect(dest_line, dest, offset);
	line = base + (sourcex >> 3);
	first_bit = 0x80 >> (sourcex & 7);
	while ( h-- > 0 )
	   {	register byte *pptr = dest;
		byte *sptr = line;
		register int sbyte = *sptr++;
		register int bit = first_bit;
		int count = w;
		do
		   {	if ( sbyte & bit )
			   {	if ( one != gx_no_color_index )
				  *pptr = (byte)one;
			   }
			else
			   {	if ( zero != gx_no_color_index )
				  *pptr = (byte)zero;
			   }
			if ( (bit >>= 1) == 0 )
				bit = 0x80, sbyte = *sptr++;
			pptr++;
		   }
		while ( --count > 0 );
		line += raster;
		next_scan_line(dest_line, dest, offset);
	   }
	return 0;
}

/* Copy a color bitmap. */
private int
mem_mapped_copy_color(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h)
{	check_fault_copy_color(x, w);
	check_rect();
	return copy_byte_rect(dev, base + x_to_byte(sourcex), raster,
		x_to_byte(x), y, x_to_byte(w), h);
}

/* ------ True (24- or 32-bit) color ------ */

/* Procedures */
declare_mem_map_procs(mem_true_map_rgb_color, mem_true_map_color_rgb);

/* The device descriptor. */
#define mem_true_procs(copy_mono, copy_color, fill_rectangle)\
  mem_procs(mem_true_map_rgb_color, mem_true_map_color_rgb,\
    copy_mono, copy_color, fill_rectangle)

/* The instance is public. */
#define mem_true_color_device(name, depth, procs)\
{	sizeof(gx_device_memory),\
	&procs,				/* differs */\
	name,				/* differs */\
	0, 0,			/* x and y extent (filled in) */\
	1, 1,			/* density (irrelevant) */\
	1, 255, depth,			/* depth differs */\
	0,			/* not open yet */\
	identity_matrix_body,	/* initial matrix (filled in) */\
	0,			/* raster (filled in) */\
	(byte *)0,		/* base (filled in) */\
	(byte **)0,		/* line_ptrs (filled in by 'open') */\
	mem_no_fault_proc,	/* default bring_in_proc */\
	0,			/* invert (unused) */\
	0, (byte *)0		/* palette (unused) */\
}

/* We want the bytes of a color always to be in the order -,r,g,b, */
/* but we want to manipulate colors as longs.  This requires careful */
/* handling to be byte-order independent. */
#define color_byte(cx,i) (((byte *)&(cx))[i])

/* Map a r-g-b color to a color index. */
private gx_color_index
mem_true_map_rgb_color(gx_device *dev, ushort r, ushort g, ushort b)
{	gx_color_index color = 0;
	color_byte(color, 1) = r;
	color_byte(color, 2) = g;
	color_byte(color, 3) = b;
	return color;
}

/* Map a color index to a r-g-b color. */
private int
mem_true_map_color_rgb(gx_device *dev, gx_color_index color, ushort *prgb)
{	prgb[0] = color_byte(color, 1);
	prgb[1] = color_byte(color, 2);
	prgb[2] = color_byte(color, 3);
	return 0;
}

/* ------ 24-bit color ------ */
/* 24-bit takes less space than 32-bit, but is slower. */

/* Procedures */
declare_mem_procs(mem_true24_copy_mono, mem_true24_copy_color, mem_true24_fill_rectangle);

/* The device descriptor. */
private gx_device_procs mem_true24_procs =
  mem_true_procs(mem_true24_copy_mono, mem_true24_copy_color,
    mem_true24_fill_rectangle);
gx_device_memory mem_true24_color_device =
  mem_device("image(24)", 24, mem_true24_procs);

/* Convert x coordinate to byte offset in scan line. */
#undef x_to_byte
#define x_to_byte(x) ((x) * 3)

/* Unpack a color into its bytes. */
#define declare_unpack_color(r, g, b, color)\
	byte r = color_byte(color, 1);\
	byte g = color_byte(color, 2);\
	byte b = color_byte(color, 3)
#define put3(ptr, r, g, b)\
	*ptr++ = r, *ptr++ = g, *ptr++ = b

/* Fill a rectangle with a color. */
private int
mem_true24_fill_rectangle(gx_device *dev,
  int x, int y, int w, int h, gx_color_index color)
{	declare_unpack_color(r, g, b, color);
	declare_scan_ptr(dest_line, dest, offset);
	check_fault_fill(x * 3, w * 3);
	setup_rect(dest_line, dest, offset);
	while ( h-- > 0 )
	   {	register int cnt = w;
		register byte *pptr = dest;
		do { put3(pptr, r, g, b); } while ( --cnt > 0 );
		next_scan_line(dest_line, dest, offset);
	   }
	return 0;
}

/* Copy a monochrome bitmap. */
private int
mem_true24_copy_mono(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	byte *line;
	int first_bit;
	declare_unpack_color(r0, g0, b0, zero);
	declare_unpack_color(r1, g1, b1, one);
	declare_scan_ptr(dest_line, dest, offset);
	check_fault_copy_mono(x * 3, w * 3);
	setup_rect(dest_line, dest, offset);
	line = base + (sourcex >> 3);
	first_bit = 0x80 >> (sourcex & 7);
	while ( h-- > 0 )
	   {	register byte *pptr = dest;
		byte *sptr = line;
		register int sbyte = *sptr++;
		register int bit = first_bit;
		int count = w;
		do
		   {	if ( sbyte & bit )
			   {	if ( one != gx_no_color_index )
				  put3(pptr, r1, g1, b1);
			   }
			else
			   {	if ( zero != gx_no_color_index )
				  put3(pptr, r0, g0, b0);
			   }
			if ( (bit >>= 1) == 0 )
				bit = 0x80, sbyte = *sptr++;
		   }
		while ( --count > 0 );
		line += raster;
		next_scan_line(dest_line, dest, offset);
	   }
	return 0;
}

/* Copy a color bitmap. */
private int
mem_true24_copy_color(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h)
{	check_fault_copy_color(x * 3, w * 3);
	check_rect();
	return copy_byte_rect(dev, base + x_to_byte(sourcex), raster,
		x_to_byte(x), y, x_to_byte(w), h);
}

/* ------ 32-bit color ------ */

/* Procedures */
declare_mem_procs(mem_true32_copy_mono, mem_true32_copy_color, mem_true32_fill_rectangle);

/* The device descriptor. */
private gx_device_procs mem_true32_procs =
  mem_true_procs(mem_true32_copy_mono, mem_true32_copy_color,
    mem_true32_fill_rectangle);
gx_device_memory mem_true32_color_device =
  mem_device("image(32)", 32, mem_true32_procs);

/* Convert x coordinate to byte offset in scan line. */
#undef x_to_byte
#define x_to_byte(x) ((x) << 2)

/* Fill a rectangle with a color. */
private int
mem_true32_fill_rectangle(gx_device *dev,
  int x, int y, int w, int h, gx_color_index color)
{	declare_scan_ptr(dest_line, dest, offset);
	check_fault_fill(x << 2, w << 2);
	setup_rect(dest_line, dest, offset);
	while ( h-- > 0 )
	   {	gx_color_index *pptr = (gx_color_index *)dest;
		int cnt = w;
		do { *pptr++ = color; } while ( --cnt > 0 );
		next_scan_line(dest_line, dest, offset);
	   }
	return 0;
}

/* Copy a monochrome bitmap. */
private int
mem_true32_copy_mono(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	byte *line;
	int first_bit;
	declare_scan_ptr(dest_line, dest, offset);
	check_fault_copy_mono(x << 2, w << 2);
	setup_rect(dest_line, dest, offset);
	line = base + (sourcex >> 3);
	first_bit = 0x80 >> (sourcex & 7);
	while ( h-- > 0 )
	   {	register gx_color_index *pptr = (gx_color_index *)dest;
		byte *sptr = line;
		register int sbyte = *sptr++;
		register int bit = first_bit;
		int count = w;
		do
		   {	if ( sbyte & bit )
			   {	if ( one != gx_no_color_index )
				  *pptr = one;
			   }
			else
			   {	if ( zero != gx_no_color_index )
				  *pptr = zero;
			   }
			if ( (bit >>= 1) == 0 )
				bit = 0x80, sbyte = *sptr++;
			pptr++;
		   }
		while ( --count > 0 );
		line += raster;
		next_scan_line(dest_line, dest, offset);
	   }
	return 0;
}

/* Copy a color bitmap. */
private int
mem_true32_copy_color(gx_device *dev, byte *base, int sourcex, int raster,
  int x, int y, int w, int h)
{	check_fault_copy_color(x << 2, w << 2);
	check_rect();
	return copy_byte_rect(dev, base + x_to_byte(sourcex), raster,
		x_to_byte(x), y, x_to_byte(w), h);
}
