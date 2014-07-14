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

/* gxht.c */
/* Halftone rendering routines for Ghostscript imaging library */
#include "memory_.h"
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"			/* ditto */
#include "gxmatrix.h"			/* for gxdevice.h */
#include "gxbitmap.h"
#include "gzstate.h"
#include "gzdevice.h"
#include "gzcolor.h"			/* requires gxdevice.h */
#include "gzht.h"

/*
 * We don't want to remember all the values of the halftone screen,
 * because they would take up space proportional to P^3, where P is
 * the number of pixels in a cell.  Instead, we pick some number N of
 * patterns to cache.  Each cache slot covers a range of (P+1)/N
 * different gray levels: we "slide" the contents of the slot back and
 * forth within this range by incrementally adding and dropping 1-bits.
 * N>=0 (obviously); N<=P+1 (likewise); also, so that we can simplify things
 * by preallocating the bookkeeping information for the cache, we define
 * a constant max_cached_tiles which is an a priori maximum value for N.
 *
 * Note that the raster for each tile must be a multiple of 32 bits,
 * to satisfy the copy_mono device routine, even though a multiple of
 * 16 bits would otherwise be sufficient.
 */
#define max_cached_tiles 25
typedef struct bit_tile_s {
	int level;			/* the cached gray level, i.e. */
					/* the number of spots whitened, */
					/* or -1 if the cache is empty */
	gx_bitmap tile;			/* the currently rendered tile */
} bit_tile;
typedef struct gx_ht_cache_s {
	/* The following are set when the cache is created. */
	byte *bits;			/* the base of the bits */
	uint bits_size;			/* the space available for bits */
	/* The following are reset each time the cache is initialized */
	/* for a new screen. */
	ht_bit *order;			/* the cached order vector */
	int num_cached;			/* actual # of cached tiles */
	int levels_per_tile;		/* # of levels per cached tile */
	bit_tile tiles[max_cached_tiles];	/* the cached tiles */
} ht_cache;
private ht_cache cache;
#define max_ht_bits 1000		/* arbitrary, maybe too small */
private byte cache_bits[max_ht_bits];
/* Bit masks for whitening vector.  We have to initialize these */
/* indirectly, because they are different for big- and little-endian */
/* machines. */
typedef unsigned short bit16;
private byte single_bits8[16*2] =
   {	0x80,0, 0x40,0, 0x20,0, 0x10,0, 8,0, 4,0, 2,0, 1,0,
	0,0x80, 0,0x40, 0,0x20, 0,0x10, 0,8, 0,4, 0,2, 0,1
   };
#define single_bits ((bit16 *)single_bits8)
private byte mb1[2] =
   {	0xff,0xff };
private byte mb2[4] =
   {	0xaa,0xaa, 0x55,0x55 };
private byte mb3[6] =
   {	0x92,0x49, 0x49,0x24, 0x24,0x92 };
private byte mb4[8] =
   {	0x88,0x88, 0x44,0x44, 0x22,0x22, 0x11,0x11 };
private byte mb5[10] =
   {	0x84,0x21, 0x42,0x10, 0x21,0x08, 0x10,0x84, 0x08,0x42 };
private byte mb6[12] =
   {	0x82,0x08, 0x41,0x04, 0x20,0x82, 0x10,0x41, 0x08,0x20, 0x04,0x10 };
private byte mb7[14] =
   {	0x81,0x02, 0x40,0x81, 0x20,0x40, 0x10,0x20, 0x08,0x10, 0x04,0x08,
		0x02,0x04
   };
private byte mb8[16] =
   {	0x80,0x80, 0x40,0x40, 0x20,0x20, 0x10,0x10, 0x08,0x08, 0x04,0x04,
		0x02,0x02, 0x01,0x01
   };
private bit16 *multi_bits[9] =
   {	0, (bit16 *)mb1, (bit16 *)mb2, (bit16 *)mb3, (bit16 *)mb4,
	(bit16 *)mb5, (bit16 *)mb6, (bit16 *)mb7, (bit16 *)mb8
   };

/* Construct the order vector.  order is an array of ht_bits: */
/* order[i].offset contains the index of the bit position */
/* that is i'th in the whitening order. */
int
gx_ht_construct_order(ht_bit *order, int width, int height)
{	uint i;
	uint size = (uint)(width * height);
	int padding = (-width) & 31;
	if ( (width + padding) / 8 * height > max_ht_bits )
		return_error(gs_error_limitcheck);	/* can't cache the rendering */
	/* Clear the cache, to avoid confusion in case */
	/* the address of a new order vector matches that of a */
	/* (deallocated) old one. */
	cache.order = NULL;
	cache.bits = cache_bits;
	cache.bits_size = max_ht_bits;
	/* Convert sequential indices to */
	/* byte indices and mask values. */
	for ( i = 0; i < size; i++ )
	   {	int pix = order[i].offset;
		pix += pix / width * padding;
		order[i].offset = (pix >> 4) << 1;
		order[i].mask =
			(width <= 8 ?
			 multi_bits[width][pix & 15] :
			 single_bits[pix & 15]);
	   }
#ifdef DEBUG
if ( gs_debug['h'] )
	   {	printf("[h]Halftone order %lx:\n", (ulong)order);
		for ( i = 0; i < size; i++ )
			printf("%4d: %u:%x\n", i, order[i].offset,
				order[i].mask);
	   }
#endif
	return 0;
}

/* Load the device color into the halftone cache if needed. */
private void render_ht(P3(bit_tile *, int, halftone *));
private void init_ht(P2(ht_cache *, halftone *));
void
gx_color_load(register gx_device_color *pdevc, gs_state *pgs)
{	int level = pdevc->halftone_level;
	halftone *pht;
	bit_tile *bt;
	if ( level == 0 ) return;	/* no halftone */
	pht = pgs->halftone;
	if ( cache.order != pht->order )
		init_ht(&cache, pht);
	bt = &cache.tiles[level / cache.levels_per_tile];
	if ( bt->level != level )
		render_ht(bt, level, pht);
	pdevc->tile = &bt->tile;
}

/* Initialize the tile cache for a given screen. */
/* Cache as many different levels as will fit. */
private void
init_ht(ht_cache *pcache, halftone *pht)
{	int width = pht->width;
	int height = pht->height;
	int size = width * height;
	static int up_to_16[] =
		/* up_to_16[i] = 16 / i * i */
		{ 0, 16, 16, 15, 16, 15, 12, 14, 16 };
	int width_unit = (width <= 8 ? up_to_16[width] : width);
	uint raster = ((width + 31) >> 5) << 2;
	uint tile_bytes = raster * height;
	int num_cached;
	int i;
	byte *tbits = pcache->bits;
	/* Make sure num_cached is within bounds */
	num_cached = max_ht_bits / tile_bytes;
	if ( num_cached > size ) num_cached = size;
	if ( num_cached > max_cached_tiles ) num_cached = max_cached_tiles;
	for ( i = 0; i < num_cached; i++ )
	   {	register bit_tile *bt = &pcache->tiles[i];
		bt->level = -1;
		bt->tile.data = tbits;
		bt->tile.raster = raster;
		bt->tile.width = width_unit;
		bt->tile.height = height;
		tbits += tile_bytes;
	   }
	pcache->order = pht->order;
	pcache->num_cached = num_cached;
	pcache->levels_per_tile = (size + num_cached - 1) / num_cached;
}

/*
 * Compute and save the rendering of a given gray level
 * with the current halftone.  The cache holds multiple tiles,
 * where each tile covers a range of possible levels.
 * If the tile whose range includes the desired level is already loaded,
 * we adjust it incrementally: this saves a lot of time for
 * the average image, where gray levels don't change abruptly.
 * Note that we will never be asked to cache levels 0 or order_size,
 * which correspond to black or white respectively.
 */
private void
render_ht(bit_tile *pbt, int level /* [1..order_size-1] */, halftone *pht)
{	ht_bit *order = pht->order;
	register ht_bit *p;
	register ht_bit *endp;
	register byte *bits = pbt->tile.data;
	int old_level = pbt->level;
	if ( old_level < 0 )
	   {	/* The cache is empty.  Preload it with */
		/* whichever of all-0s and all-1s will be faster. */
		uint tile_bytes = pbt->tile.raster * pbt->tile.height;
		if ( level >= pht->order_size >> 1 )
		   {	old_level = pht->order_size;
			memset(bits, 0xff, tile_bytes);
		   }
		else
		   {	old_level = 0;
			memset(bits, 0, tile_bytes);
		   }
	   }
#ifdef DEBUG
	if ( level < 0 || level > pht->order_size || level == old_level )
	   {	dprintf3("Error in render_ht: level=%d, old_level=%d, order_size=%d=n", level, old_level, pht->order_size);
		exit(1);
	   }
#endif
	/* Note that we can use the same loop to turn bits either */
	/* on or off, using xor.  We use < to compare pointers, */
	/* rather than ==, because Turbo C only compares the */
	/* low 16 bits for < and > but compares all 32 bits for ==. */
	if ( level > old_level )
		p = &order[old_level], endp = &order[level];
	else
		p = &order[level], endp = &order[old_level];
	/* Invert bits between the two pointers */
	do
	   {	*(bit16 *)&bits[p->offset] ^= p->mask;
	   }
	while ( ++p < endp );
#ifdef DEBUG
if ( gs_debug['h'] )
	   {	byte *p = bits;
		int wb = pbt->tile.raster;
		byte *ptr = bits + wb * pbt->tile.height;
		printf("[h]Halftone cache %lx: old=%d, new=%d, w=%d(%d), h=%d(%d):\n",
			(ulong)bits, old_level, level, pbt->tile.width,
		        pht->width, pbt->tile.height, pht->height);
		while ( p < ptr )
		   {	printf(" %02x", *p++);
			if ( (p - bits) % wb == 0 ) printf("\n");
		   }
	   }
#endif
	pbt->level = level;
}
