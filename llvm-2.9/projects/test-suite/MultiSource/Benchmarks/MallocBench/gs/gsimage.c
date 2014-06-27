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

/* gsimage.c */
/* Image procedures for GhostScript library */
#include "gx.h"
#include "arch.h"
#include "memory_.h"
#include "gserrors.h"
#include "gxfixed.h"
#include "gxarith.h"
#include "gxmatrix.h"
#include "gspaint.h"
#include "gzstate.h"
#include "gzdevice.h"			/* requires gsstate.h */
#include "gzcolor.h"			/* requires gxdevice.h */
#include "gzpath.h"
#include "gximage.h"
#include <stdint.h>

/* Exported size of enumerator */
int gs_image_enum_sizeof = sizeof(gs_image_enum);

/* Forward declarations */
private int image_init(P10(gs_image_enum *, int, int, int, int, int,
  gs_matrix *, gs_state *, gx_color_index, gx_color_index));
/* Procedures for unpacking the input data into 8 bits/sample. */
private void image_unpack_0(iunpack_proc_args);
private void image_unpack_0_spread(iunpack_proc_args);
private void image_unpack_1(iunpack_proc_args);
private void image_unpack_1_spread(iunpack_proc_args);
private void image_unpack_2(iunpack_proc_args);
private void image_unpack_3(iunpack_proc_args);
private void image_unpack_3_spread(iunpack_proc_args);
/* The image_render procedures work on fully expanded, complete rows. */
/* These take a height argument, which is an integer > 0; */
/* they return a negative code, or the number of */
/* rows actually processed (which may be less than the height). */
private int image_render_skip(irender_proc_args);
private int image_render_direct(irender_proc_args);
private int image_render_mono(irender_proc_args);
private int image_render_color(irender_proc_args);

/* Start processing an image */
int
gs_image_init(gs_image_enum *penum, gs_state *pgs,
  int width, int height, int bps, int spp, gs_matrix *pmat)
{	int log2_bps, spread;
	if ( pgs->in_cachedevice ) return_error(gs_error_undefined);
	switch ( bps )
	   {
	case 1: case 2: case 4:
		log2_bps = bps >> 1;	/* works for 1, 2, 4 */
		break;
	case 8:
		log2_bps = 3;
		break;
	default:
		return_error(gs_error_rangecheck);
	   }
	switch ( spp )
	   {
	case 1: case 3: case 4:
		spread = 1; break;
	case -3: case -4:
		spp = -spp; spread = spp; break;
	default:
		return_error(gs_error_rangecheck);
	   }
	return image_init(penum, width, height, log2_bps, spp, spread,
			  pmat, pgs, pgs->device->black, pgs->device->white);
}

/* Start processing a masked image */
int
gs_imagemask_init(gs_image_enum *penum, gs_state *pgs,
  int width, int height, int invert, gs_matrix *pmat)
{	gx_color_index color0, color1;
	gx_color_render(pgs->color, pgs->dev_color, pgs);
	/* The following is wrong for halftones, but */
	/* it doesn't matter, because color0 and color1 */
	/* won't be used if the color isn't pure. */
	if ( invert )
		color0 = gx_no_color_index,
		color1 = pgs->dev_color->color1;
	else
		color0 = pgs->dev_color->color1,
		color1 = gx_no_color_index;
	return image_init(penum, width, height, 0, 1, 1,
			  pmat, pgs, color0, color1);
}

/* Common setup for image and imagemask. */
/* Note that the mask tables depend on the end-orientation of the CPU. */
/* We can't simply define them as byte arrays, because */
/* they might not wind up properly 32-bit or 16-bit-aligned. */
#define map4tox(a,b,c,d)\
	0, a, b, a+b, c, a+c, b+c, a+b+c,\
	d, a+d, b+d, a+b+d, c+d, a+c+d, b+c+d, a+b+c+d
#if big_endian
private uint32_t map_4_to_32[16] =
   {	map4tox(0xffL, 0xff00L, 0xff0000L, 0xff000000L)	};
private uint16_t map_4_to_16[16] =
   {	map4tox(0x55, 0xaa, 0x5500, 0xaa00)	};
#else					/* !big_endian */
private uint32_t map_4_to_32[16] =
   {	map4tox(0xff000000L, 0xff0000L, 0xff00L, 0xffL)	};
private uint16_t map_4_to_16[16] =
   {	map4tox(0x5500, 0xaa00, 0x55, 0xaa)	};
#endif
private int
image_init(register gs_image_enum *penum, int width, int height,
  int log2_bps, int spp, int spread, gs_matrix *pmat, gs_state *pgs,
  gx_color_index color0, gx_color_index color1)
{	int code;
	gs_matrix mat;
	uint bsize = (width + 8) * spp;	/* round up, +1 for end-of-run byte */
	byte *buffer;
	fixed mtx, mty;
	if ( width <= 0 || height < 0 )
		return_error(gs_error_undefinedresult);
	if ( height == 0 ) return 0;	/* empty image */
	if (	(code = gs_matrix_invert(pmat, &mat)) < 0 ||
		(code = gs_matrix_multiply(&mat, &ctm_only(pgs), &mat)) < 0
	   )	return code;
	buffer = (byte *)gs_malloc(1, bsize, "image buffer");
	if ( buffer == 0 ) return_error(gs_error_VMerror);
	penum->width = width;
	penum->height = height;
	penum->log2_bps = log2_bps;
	penum->spp = spp;
	penum->spread = spread;
	penum->fxx = float2fixed(mat.xx);
	penum->fyy = float2fixed(mat.yy);
	if ( (penum->skewed = is_skewed(&mat)) )
	   {	penum->fxy = float2fixed(mat.xy);
		penum->fyx = float2fixed(mat.yx);
	   }
	else
	   {	penum->fxy = 0;
		penum->fyx = 0;
	   }
	penum->xcur = mtx = float2fixed(mat.tx);
	penum->ycur = mty = float2fixed(mat.ty);
	penum->pgs = pgs;
	penum->buffer = buffer;
	penum->buffer_size = bsize;
	penum->bytes_per_row =
		(uint)((((ulong)width << log2_bps) * spp / spread + 7) >> 3);
	if ( spp == 1 )
	   {	/* Initialize the color table */
#define chtl(i)\
  penum->dev_colors[i].halftone_level
		switch ( log2_bps )
		   {
		case 3:
		   {	/* Yes, clearing the entire table is slow, */
			/* but for 8 bit-per-sample images, it's worth it. */
			register gx_device_color *pcht = &penum->dev_colors[0];
			register int n = 64;
			do
			   {	pcht[0].halftone_level =
				  pcht[1].halftone_level =
				  pcht[2].halftone_level =
				  pcht[3].halftone_level = -1;
				pcht += 4;
			   }
			while ( --n > 0 );
			break;
		   }
		case 2:
			chtl(17) = chtl(2*17) = chtl(3*17) =
			  chtl(4*17) = chtl(6*17) = chtl(7*17) =
			  chtl(8*17) = chtl(9*17) = chtl(11*17) =
			  chtl(12*17) = chtl(13*17) = chtl(14*17) = -1;
			/* falls through */
		case 1:
			chtl(5*17) = chtl(10*17) = -1;
		/* 0 doesn't need any further initialization */
		   }
		penum->icolor0 = color0;
		chtl(0) = 0;				/* pure color */
		penum->icolor1 = color1;
		chtl(255) = 0;				/* pure color */
#undef chtl
	   }
	/* If all four extrema of the image fall within the clipping */
	/* rectangle, clipping is never required. */
	   {	gx_path *pcpath = pgs->clip_path;
		fixed xmin = pcpath->cbox.p.x;
		fixed ymin = pcpath->cbox.p.y;
		fixed xmax = pcpath->cbox.q.x;
		fixed ymax = pcpath->cbox.q.y;
		fixed mdx = float2fixed(mat.xx) * width;
		fixed mdy = float2fixed(mat.yy) * height;
		if ( penum->skewed )
		   {	mdx += float2fixed(mat.yx) * height;
			mdy += float2fixed(mat.xy) * width;
		   }
		penum->never_clip =
			(mdx < 0 ?
				mtx + mdx >= xmin && mtx <= xmax :
				mtx >= xmin && mtx + mdx <= xmax) &&
			(mdy < 0 ?
				mty + mdy >= ymin && mty <= ymax :
				mty >= ymin && mty + mdy <= ymax);
#ifdef DEBUG
if ( gs_debug['b'] )
	printf("[b]Image: xmin=%g ymin=%g xmax=%g ymax=%g\n",
		fixed2float(xmin), fixed2float(ymin),
		fixed2float(xmax), fixed2float(ymax)),
	printf("     mtx=%g mty=%g mdx=%g mdy=%g never_clip=%d\n",
		fixed2float(mtx), fixed2float(mty),
		fixed2float(mdx), fixed2float(mdy), penum->never_clip);
#endif
	   }
	   {	static void (*procs[4])(iunpack_proc_args) = {
			image_unpack_0, image_unpack_1,
			image_unpack_2, image_unpack_3
		   };
		static void (*spread_procs[4])(iunpack_proc_args) = {
			image_unpack_0_spread, image_unpack_1_spread,
			image_unpack_2, image_unpack_3_spread
		   };
		penum->slow_loop = !penum->never_clip || penum->skewed ||
			/* Use slow loop for imagemask with a halftone */
			((color0 == gx_no_color_index || color1 == gx_no_color_index) && !color_is_pure(pgs->dev_color));
		penum->render =
			(pgs->in_charpath ? image_render_skip :
			 spp > 1 ? image_render_color :
			 log2_bps == 0 && !penum->slow_loop &&
			 (fixed2long_rounded(mtx + width * penum->fxx) -
			  fixed2long(mtx) == width) ?
			   image_render_direct : image_render_mono);
		/* If the image is 1-for-1 with the device, */
		/* we don't want to spread the samples. */
		if ( penum->render == image_render_direct )
		  penum->unpack = image_unpack_3;
		else if ( spread != 1 )
		  penum->unpack = spread_procs[log2_bps];
		else
		  penum->unpack = procs[log2_bps];
	   }
	penum->plane_index = 0;
	penum->byte_in_row = 0;
	penum->y = 0;
#ifdef DEBUG
if ( gs_debug['b'] )
	printf("[b]Image: w=%d h=%d %s\n   [%f %f %f %f %f %f]\n",
		width, height, (penum->never_clip ? "no clip" : "must clip"),
		mat.xx, mat.xy, mat.yx, mat.yy, mat.tx, mat.ty);
#endif
	return 0;
}

/* Process the next piece of an image */
int
gs_image_next(register gs_image_enum *penum, byte *dbytes, uint dsize)
{	uint rsize = penum->bytes_per_row;
	uint pos = penum->byte_in_row;
	int width = penum->width;
	uint dleft = dsize;
	uint dpos = 0;
	int code;
	/* Accumulate separated colors, if needed */
	if ( penum->plane_index == 0 )
		penum->plane_size = dsize;
	else if ( dsize != penum->plane_size )
		return_error(gs_error_undefinedresult);
	penum->planes[penum->plane_index] = dbytes;
	if ( ++(penum->plane_index) != penum->spread )
		return 0;
	penum->plane_index = 0;
	/* We've accumulated an entire set of planes. */
	while ( dleft )
	   {	/* Fill up a row, then display it. */
		uint bcount = min(dleft, rsize - pos);
		byte *bptr = penum->buffer + (pos << (3 - penum->log2_bps)) * penum->spread;
		int px;
		for ( px = 0; px < penum->spread; px++ )
			(*penum->unpack)(penum, bptr + px, penum->planes[px] + dpos, bcount);
		pos += bcount;
		dpos += bcount;
		dleft -= bcount;
		if ( pos == rsize )	/* filled an entire row */
		   {	code = (*penum->render)(penum, penum->buffer, width * penum->spp, 1);
			if ( code < 0 ) goto err;
			if ( ++(penum->y) == penum->height ) goto end;
			pos = 0;
			penum->xcur += penum->fyx;
			penum->ycur += penum->fyy;
		   }
	   }
	penum->byte_in_row = pos;
	return 0;
end:	/* End of data */
	code = 1;
	/* falls through */
err:	/* Error, abort */
	gs_free((char *)penum->buffer, penum->buffer_size, 1, "image buffer");
	return code;
}

/* ------ Unpacking procedures ------ */

private void
image_unpack_0(gs_image_enum *penum, byte *bptr,
  register byte *data, uint dsize)
{	register uint32_t *bufp = (uint32_t *)bptr;
	int left = dsize;
	while ( left-- )
	   {	register unsigned b = *data++;
		*bufp++ = map_4_to_32[b >> 4];
		*bufp++ = map_4_to_32[b & 0xf];
	   }
}

private void
image_unpack_0_spread(gs_image_enum *penum, register byte *bufp,
  register byte *data, uint dsize)
{	register int spread = penum->spread;
	int left = dsize;
	while ( left-- )
	   {	register unsigned b = *data++;
		*bufp = -(b >> 7); bufp += spread;
		*bufp = -((b >> 6) & 1); bufp += spread;
		*bufp = -((b >> 5) & 1); bufp += spread;
		*bufp = -((b >> 4) & 1); bufp += spread;
		*bufp = -((b >> 3) & 1); bufp += spread;
		*bufp = -((b >> 2) & 1); bufp += spread;
		*bufp = -((b >> 1) & 1); bufp += spread;
		*bufp = -(b & 1); bufp += spread;
	   }
}

private void
image_unpack_1(gs_image_enum *penum, byte *bptr,
  register byte *data, uint dsize)
{	register uint16_t *bufp = (uint16_t *)bptr;
	int left = dsize;
	while ( left-- )
	   {	register unsigned b = *data++;
		*bufp++ = map_4_to_16[b >> 4];
		*bufp++ = map_4_to_16[b & 0xf];
	   }
}

private void
image_unpack_1_spread(gs_image_enum *penum, register byte *bufp,
  register byte *data, uint dsize)
{	register int spread = penum->spread;
	int left = dsize;
	while ( left-- )
	   {	register unsigned b = *data++;
		register ushort b2;
		b2 = map_4_to_16[b >> 4];
		*bufp = b2 >> 8; bufp += spread;
		*bufp = (byte)b2; bufp += spread;
		b2 = map_4_to_16[b & 0xf];
		*bufp = b2 >> 8; bufp += spread;
		*bufp = (byte)b2; bufp += spread;
	   }
}

private void
image_unpack_2(gs_image_enum *penum, register byte *bufp,
  register byte *data, uint dsize)
{	register int spread = penum->spread;
	int left = dsize;
	while ( left-- )
	   {	register unsigned b = *data++;
		*bufp = (b & 0xf0) + (b >> 4); bufp += spread;
		b &= 0xf;
		*bufp = (b << 4) + b; bufp += spread;
	   }
}

private void
image_unpack_3(gs_image_enum *penum, byte *bufp,
  byte *data, uint dsize)
{	if ( data != bufp ) memcpy(bufp, data, dsize);
}

private void
image_unpack_3_spread(gs_image_enum *penum, register byte *bufp,
  register byte *data, uint dsize)
{	register int spread = penum->spread;
	register int left = dsize;
	while ( left-- )
	   {	*bufp = *data++; bufp += spread;
	   }
}

/* ------ Rendering procedures ------ */

/* Rendering procedure for ignoring an image.  We still need to iterate */
/* over the samples, because the procedure might have side effects. */
private int
image_render_skip(gs_image_enum *penum, byte *data, uint w, int h)
{	return h;
}

/* Rendering procedure for a 1-bit-per-pixel sampled image */
/* with no clipping, skewing, rotation, or X scaling. */
/* In this case a direct BitBlt is possible. */
private int
image_render_direct(gs_image_enum *penum, byte *data, uint w, int h)
{	fixed xt = penum->xcur;
	fixed yt = penum->ycur, yn = yt + penum->fyy;
	int ix = fixed2int(xt), iy = fixed2int(yt);
	int ht = fixed2int(yn) - iy;
	gx_device *dev = penum->pgs->device->info;
	int (*proc)(P10(gx_device *, byte *, int, int, int, int, int, int, gx_color_index, gx_color_index)) = dev->procs->copy_mono;
	gx_color_index zero = penum->icolor0, one = penum->icolor1;
	if ( ht == 1 )
	   {	/* We can do the whole thing at once. */
		(*proc)(dev, data, 0, (w + 7) >> 3,
			ix, iy, w, h, zero, one);
		return h;
	   }
	else
	   {	/* Do just one row. */
		int dy;
		if ( ht < 0 )
			iy += ht, ht = -ht;
		for ( dy = 0; dy < ht; dy++ )
			(*proc)(dev, data, 0, (w + 7) >> 3,
				ix, iy + dy, w, 1, zero, one);
		return 1;
	   }
}

/* Rendering procedure for the general case of displaying a */
/* monochrome image, dealing with multiple bit-per-sample images, */
/* bits not 1-for-1 with the device, clipping, and general transformations. */
/* This procedure handles a single scan line. */
private int
image_render_mono(gs_image_enum *penum, byte *buffer, uint w, int h)
{	gx_color_index zero = penum->icolor0, one = penum->icolor1;
	gs_state *pgs = penum->pgs;
	fixed	dxx = penum->fxx, dxy = penum->fxy,
		dyx = penum->fyx, dyy = penum->fyy;
	gs_fixed_rect cbox;
	int skew = penum->skewed;
	fixed xt = penum->xcur;
	fixed ytf = penum->ycur;
	fixed yn;		/* ytf + dyy, if no skew */
	int yt, yb, idy, iht;
	gs_color rcolor;
	gx_device_color *pdevc = pgs->dev_color;
     /* Note: image_set_gray assumes that log2_bps != 0. */
#define image_set_rgb(sample_value)\
  rcolor.luminance = rcolor.red = rcolor.green = rcolor.blue =\
    color_param_from_byte(sample_value)
#define image_set_gray(sample_value)\
   { pdevc = &penum->dev_colors[sample_value];\
     if ( pdevc->halftone_level < 0 )\
      { image_set_rgb(sample_value);\
        gx_color_render(&rcolor, pdevc, pgs);\
      }\
     else\
       gx_color_load(pdevc, pgs);\
   }
	int xcnt = w;
	fixed xl = xt;
	byte *psrc = buffer;
	fixed xrun = xt;		/* x at start of run */
	fixed yrun = ytf;		/* y ditto */
	int run = *psrc;		/* run value */
	int htrun = -2;			/* halftone run value */
	gx_device *dev = pgs->device->info;
	int (*fill_proc)(P6(gx_device *, int, int, int, int, gx_color_index)) = dev->procs->fill_rectangle;
	cbox = pgs->clip_path->bbox;	/* box is known to be up to date */
	if ( !skew )
	  { /* We're going to round to pixel boundaries later, */
	    /* so we might as well do this for Y now. */
	    yn = fixed_rounded(ytf + dyy);
	    ytf = fixed_rounded(ytf);
	    yt = fixed2int(ytf);
	    yb = fixed2int(yn);
	    iht = yb - yt;
	    /* Do a quick clipping and empty check now. */
	    if ( iht > 0 ) idy = 0;
	    else if ( iht < 0 ) idy = iht, iht = -iht;
	    else return 1;
	    if ( dyy > 0 )
	      { if ( ytf >= cbox.q.y || yn <= cbox.p.y ) return 1;
	      }
	    else
	      { if ( yn >= cbox.q.y || ytf <= cbox.p.y ) return 1;
	      }
	  }
	buffer[w] = ~buffer[w - 1];	/* force end of run */
#ifdef DEBUG
if ( gs_debug['b'] )
	printf("[b]y=%d w=%d xt=%f yt=%f yb=%f\n",
		penum->y, w,
		fixed2float(xt), fixed2float(ytf), fixed2float(ytf + dyy));
#endif
	rcolor.is_gray = rcolor.luminance_set = 1;
	while ( xcnt-- >= 0 )	/* 1 extra iteration */
				/* to handle final run */
	   {	if ( *psrc++ != run )
		   {	/* Fill the region between */
			/* xrun and xl */
			if ( penum->slow_loop )
			  { /* Must use general fill */
			    gx_path ipath;
			    gx_path *ppath = &ipath;
			    int code;
			    /* Use halftone if needed. */
			    /* Also handle imagemask here. */
			    if ( run != htrun )
			      { htrun = run;
				if ( run == 0 )
				  { if ( zero == gx_no_color_index ) goto trans;
				  }
				else if ( run == 255 )
				  { if ( one == gx_no_color_index ) goto trans;
				  }
				image_set_gray(run);
			      }
			    if ( !skew )
			      { /* Do the empty/clipping check */
				/* before going to the trouble of */
				/* building the path. */
				/* In the non-skew case, we take the */
				/* trouble to round the box correctly. */
				/* We pre-checked Y, only do X now. */
				fixed xlr = fixed_rounded(xl);
				fixed xrr = fixed_rounded(xrun);
				if ( xrr == xlr )
				  goto trans;
				if ( dxx >= 0 )
				  { if ( xrr >= cbox.q.x || xlr <= cbox.p.x )
				      goto trans;
				  }
				else
				  { if ( xlr >= cbox.q.x || xrr <= cbox.p.x )
				      goto trans;
				  }
				gx_path_init(ppath, &pgs->memory_procs);
				code = gx_path_add_pgram(ppath,
							 xrr, ytf, xlr, ytf,
							 xlr, yn);
			      }
			    else
			      { gx_path_init(ppath, &pgs->memory_procs);
				code = gx_path_add_pgram(ppath,
							 xrun, yrun, xl, ytf,
							 xl + dyx, ytf + dyy);
			      }
			    if ( code < 0 )
			      {	gx_path_release(ppath);
				return code;
			      }
			    if ( skew )
			      {	gs_fixed_rect box;
				gx_path_bbox(ppath, &box);
#define check_clipped_or_empty(xy, label)\
  if ( box.q.xy <= cbox.p.xy || box.p.xy >= cbox.q.xy ||\
       fixed2int_rounded(box.p.xy) == fixed2int_rounded(box.q.xy)\
   ) goto label
				check_clipped_or_empty(x, nofill);
				check_clipped_or_empty(y, nofill);
#undef check_clipped_or_empty
			      }
			    gx_fill_path(ppath, pdevc, pgs,
					 gx_rule_winding_number, (fixed)0);
nofill:			    gx_path_release(ppath);
trans: ;		  }
			else
			   {	/* No clipping, no skew, and not */
				/* imagemask with a halftone. */
				gx_color_index rcx;
				int xi = fixed2int_rounded(xrun);
				int wi = fixed2int_rounded(xl) - xi;
				if ( wi <= 0 )
				   {	if ( wi == 0 ) goto mt;
					xi += wi, wi = -wi;
				   }
				switch ( run )
				   {
				case 0:
					rcx = zero;
					goto zo;
				case 255:
					rcx = one;
zo:					if ( rcx != gx_no_color_index )
					   {	(*fill_proc)(dev, xi, yt + idy, wi, iht, rcx);
					   }
					break;
				default:
					/* Use halftone if needed */
					if ( run != htrun )
					   {	image_set_gray(run);
						htrun = run;
					   }
					gz_fill_rectangle(xi, yt + idy, wi, iht, pdevc, pgs);
				   }
mt: ;			   }
			xrun = xl;
			yrun = ytf;
			run = psrc[-1];
		   }
		xl += dxx;
		ytf += dxy;		/* harmless if no skew */
	   }
	return 1;
}

/* Rendering procedure for handling color images. */
typedef union { struct { byte r, g, b, skip; } v; ulong all; } color_sample;
private int
image_render_color(gs_image_enum *penum, byte *buffer, uint w, int h)
{	gs_state *pgs = penum->pgs;
	fixed	dxx = penum->fxx, dxy = penum->fxy,
		dyx = penum->fyx, dyy = penum->fyy;
	int skew = penum->skewed;
	fixed xt = penum->xcur;
	fixed ytf = penum->ycur;
	int yt, idy, iht;
	gs_color rcolor;
	gx_device_color devc1, devc2;
	gx_device_color *pdevc = &devc1, *pdevc_next = &devc2;
	int spp = penum->spp;
	fixed xl = xt;
	byte *psrc = buffer;
	fixed xrun = xt;		/* x at start of run */
	fixed yrun = ytf;		/* y ditto */
	color_sample run;		/* run value */
	color_sample next;		/* next sample value */
	byte *bufend = buffer + w;
	bufend[0] = ~bufend[-spp];	/* force end of run */
	if ( !skew )
	   {	fixed yn = ytf + dyy;
		yt = fixed2int_rounded(ytf);
		iht = fixed2int_rounded(yn) - yt;
		if ( iht >= 0 ) idy = 0;
		else idy = iht, iht = -iht;
	   }
#ifdef DEBUG
if ( gs_debug['b'] )
	printf("[b]y=%d w=%d xt=%f yt=%f yb=%f\n",
		penum->y, w,
		fixed2float(xt), fixed2float(ytf), fixed2float(ytf + dyy));
#endif
	run.all = 0;
	next.all = 0;
	rcolor.red = rcolor.green = rcolor.blue = 0;
	gx_color_from_rgb(&rcolor);
	gx_color_render(&rcolor, pdevc, pgs);
	while ( psrc <= bufend )	/* 1 extra iteration */
				/* to handle final run */
	   {	if ( spp == 4 )		/* cmyk */
		   {	uint black = 0xff - psrc[3];
			next.v.r = (0xff - psrc[0]) * black / 0xff;
			next.v.g = (0xff - psrc[1]) * black / 0xff;
			next.v.b = (0xff - psrc[2]) * black / 0xff;
			psrc += 4;
		   }
		else			/* rgb */
		   {	next.v.r = psrc[0];
			next.v.g = psrc[1];
			next.v.b = psrc[2];
			psrc += 3;
		   }
		if ( next.all != run.all )
		   {	rcolor.red = color_param_from_byte(next.v.r);
			rcolor.green = color_param_from_byte(next.v.g);
			rcolor.blue = color_param_from_byte(next.v.b);
			gx_color_from_rgb(&rcolor);
			gx_color_render(&rcolor, pdevc_next, pgs);
			/* Even though the supplied colors don't match, */
			/* the device colors might. */
			if ( devc1.color1 != devc2.color1 ||
			     devc1.halftone_level != devc2.halftone_level ||
			     (devc1.halftone_level &&
			      devc1.color2 != devc2.color2) ||
			     psrc > bufend	/* force end of last run */
			   )
			   {	/* Fill the region between */
				/* xrun and xl */
				gx_device_color *ptemp;
				if ( penum->slow_loop )
			   {	/* Must use general fill */
				gx_path ipath;
				gx_path *ppath = &ipath;
				int code;
				gx_path_init(ppath, &pgs->memory_procs);
				code = gx_path_add_pgram(ppath,
					xrun, yrun, xl, ytf,
					xl + dyx, ytf + dyy);
				if ( code < 0 )
				   {	gx_path_release(ppath);
					return code;
				   }
				gx_fill_path(ppath, pdevc, pgs,
					     gx_rule_winding_number, (fixed)0);
				gx_path_release(ppath);
			   }
				else
			   {	/* No clipping, no skew. */
				int xi = fixed2int_rounded(xrun);
				int wi = fixed2int_rounded(xl) - xi;
				if ( wi < 0 ) xi += wi, wi = -wi;
				gz_fill_rectangle(xi, yt + idy, wi, iht, pdevc, pgs);
			   }
				xrun = xl;
				yrun = ytf;
				ptemp = pdevc;
				pdevc = pdevc_next;
				pdevc_next = ptemp;
			   }
			run.all = next.all;
		   }
		xl += dxx;
		ytf += dxy;		/* harmless if no skew */
	   }
	return 1;
}
