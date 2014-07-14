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

/* gxcache.c */
/* Character cache routines for GhostScript library */
#include "gx.h"
#include "memory_.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gspaint.h"
#include "gzstate.h"
#include "gzdevice.h"			/* requires gsstate.h */
#include "gzcolor.h"
#include "gzpath.h"
#include "gxdevmem.h"
#include "gxchar.h"
#include "gxfont.h"
#include "gxfdir.h"

/* Define the size of the cache structures */
int cached_char_sizeof = sizeof(cached_char);
int cached_fm_pair_sizeof = sizeof(cached_fm_pair);

/* Forward references */
private void zap_cache(P1(gs_font_dir *));

/* Allocate storage for caching a rendered character, */
/* and set up the memory device. */
/* Return the cached_char if OK, 0 if too big. */
cached_char *
gx_alloc_char_bits(gs_font_dir *dir, gx_device_memory *dev,
  ushort iwidth, ushort iheight)
{	ulong isize;
	cached_char *cc;
	dev->width = iwidth;
	dev->height = iheight;
	isize = gx_device_memory_bitmap_size(dev);	/* sets raster */
	if ( dev->raster != 0 && iheight > dir->upper / dev->raster )
		return 0;		/* too big */
	if ( dir->csize >= dir->cmax || dir->bmax - dir->bsize < isize )
	   {	/* There isn't enough room.  Clear the entire cache. */
		/* We'll do something better someday.... */
		if ( dir->cmax == 0 || dir->bmax < isize ) return 0;
		zap_cache(dir);
	   }
	/* Allocate the cache entry and the bits. */
	cc = &dir->cdata[dir->cnext++];
	cc->bits = &dir->bdata[dir->bnext];
	dir->bnext += isize;
	memset((char *)cc->bits, 0, (uint)isize);
	cc->width = iwidth;
	cc->height = iheight;
	cc->raster = dev->raster;
	dev->base = cc->bits;
	(*dev->procs->open_device)((gx_device *)dev);	/* initialize */
	dir->csize++;
	dir->bsize += isize;
	return cc;
}

/* Remove the just-allocated character from the cache. */
/* The character hasn't been added yet. */
void
gx_unalloc_cached_char(gs_font_dir *dir, cached_char *cc)
{	uint isize = cc->raster * cc->height;
	dir->cnext--;
	dir->bnext -= isize;
	dir->csize--;
	dir->bsize -= isize;
}

/* Look up, and if necessary add, a font/matrix pair in the cache */
cached_fm_pair *
gx_lookup_fm_pair(register gs_state *pgs)
{	float	mxx = pgs->char_tm.xx, mxy = pgs->char_tm.xy,
		myx = pgs->char_tm.yx, myy = pgs->char_tm.yy;
	gs_font *font = pgs->font;
	register gs_font_dir *dir = font->dir;
	register cached_fm_pair *pair = dir->mdata + dir->mnext;
	int count = dir->msize;
	while ( count-- )
	   {	if ( pair == dir->mdata ) pair += dir->mmax;
		else pair--;
		if (	pair->font == font &&
			pair->mxx == mxx && pair->mxy == mxy &&
			pair->myx == myx && pair->myy == myy
		   )
		  return pair;
	   }
	/* Add the pair to the cache */
	if ( dir->msize == dir->mmax )
		zap_cache(dir);		/* crude, but it works */
	dir->msize++;
	pair = dir->mdata + dir->mnext;
	if ( ++dir->mnext == dir->mmax ) dir->mnext = 0;
	pair->font = font;
	pair->mxx = mxx, pair->mxy = mxy;
	pair->myx = myx, pair->myy = myy;
	pair->num_chars = 0;
	return pair;
}

/* Add a character to the cache */
void
gx_add_cached_char(gs_font_dir *dir, cached_char *cc, cached_fm_pair *pair)
{	/* Add the new character at the tail of its chain. */
	cached_char **head = &dir->chars[cc->code & (gx_char_cache_modulus - 1)];
	while ( *head != 0 ) head = &(*head)->next;
	*head = cc;
	cc->next = 0;
	cc->pair = pair;
	pair->num_chars++;
}

/* Look up a character in the cache. */
/* Return the cached_char or 0. */
cached_char *
gx_lookup_cached_char(gs_state *pgs, cached_fm_pair *pair, int ccode)
{	register cached_char *cc =
		pgs->font->dir->chars[ccode & (gx_char_cache_modulus - 1)];
	while ( cc != 0 )
	   {	if ( cc->code == ccode && cc->pair == pair )
		  return cc;
		cc = cc->next;
	   }
	return 0;
}

/* Copy a cached character to the screen. */
/* Assume the caller has already done gx_color_render (or gx_color_load), */
/* and the color is not a halftone. */
/* Return 0 if OK, 1 if we couldn't do the operation but no error */
/* occurred, or a negative error code. */
int
gx_copy_cached_char(register gs_show_enum *penum, register cached_char *cc)
{	register gs_state *pgs = penum->pgs;
	int x, y, w, h;
	int code;
	gs_fixed_point pt;
	code = gx_path_current_point_inline(pgs->path, &pt);
	if ( code < 0 ) return code;
	/* Compute the device color if needed; abort if it isn't pure. */
	if ( !penum->color_loaded )
	   {	gx_device_color *pdevc = pgs->dev_color;
		gx_color_render(pgs->color, pdevc, pgs);
		if ( !color_is_pure(pdevc) )
			return 1;	/* can't use cache */
		penum->color_loaded = 1;
	   }
	/* If the character doesn't lie entirely within the */
	/* quick-check clipping rectangle, we have to use */
	/* the general case of image rendering. */
	pt.x -= cc->offset.x;
	x = fixed2int_rounded(pt.x);
	pt.y -= cc->offset.y;
	y = fixed2int_rounded(pt.y);
	w = cc->width;
	h = cc->height;
	if (	x < penum->cxmin || x + w > penum->cxmax ||
		y < penum->cymin || y + h > penum->cymax
	   )
	   {	gs_matrix mat;
		mat = ctm_only(pgs);
		mat.tx -= fixed2float(pt.x);
		mat.ty -= fixed2float(pt.y);
		code = gs_imagemask(pgs, cc->raster * 8, h, 1,
				    &mat, cc->bits);
	   }
	else
	   {	/* Just copy the bits */
		gx_device *dev = pgs->device->info;
		code = (*dev->procs->copy_mono)
			(dev, cc->bits, 0, cc->raster,
			 x, y, w, h,
			 gx_no_color_index, pgs->dev_color->color1);
	   }
	return ( code < 0 ? code : 0 );
}

/* ------ Internal routines ------ */

/* Zap the cache when it overflows. */
/* This is inefficient and crude, but it works. */
private void
zap_cache(register gs_font_dir *dir)
{	dir->bsize = 0;
	dir->msize = 0;
	dir->csize = 0;
	dir->bnext = 0;
	dir->mnext = 0;
	dir->cnext = 0;
	memset((char *)dir->chars, 0, gx_char_cache_modulus * sizeof(cached_char *));
}
