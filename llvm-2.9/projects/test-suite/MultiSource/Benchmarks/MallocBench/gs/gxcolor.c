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

/* gxcolor.c */
/* Private color procedures for GhostScript library */
#include "gx.h"
#include "gxfixed.h"			/* for gxmatrix.h */
#include "gxmatrix.h"
#include "gxdevice.h"			/* for gx_color_index */
#include "gzcolor.h"
#include "gzht.h"

/* Define the color type (normally obtained from gsstate.h). */
typedef struct gs_color_s gs_color;

/* ------ Color conversion routines ------ */

/* Note: the color model conversion algorithms are taken from */
/* Rogers, Procedural Elements for Computer Graphics, pp. 401-403. */

/* Complete color specified by rgb */
void
gx_color_from_rgb(register gs_color *pcolor)
{	if ( pcolor->red == pcolor->green && pcolor->green == pcolor->blue )
	   {	pcolor->luminance = pcolor->red;	/* pick any one */
		pcolor->is_gray = pcolor->luminance_set = 1;
	   }
	else
	   {	/* Compute luminance later */
		pcolor->is_gray = pcolor->luminance_set = 0;
	   }
}

/* Compute (if necessary) and return the luminance of a color. */
color_param
gx_color_luminance(register gs_color *pcolor)
{	if ( !pcolor->luminance_set )
	   {	pcolor->luminance =
			(pcolor->red * (unsigned long)lum_red_weight +
			pcolor->green * (unsigned long)lum_green_weight +
			pcolor->blue * (unsigned long)lum_blue_weight +
			(lum_all_weights / 2))
		    / lum_all_weights;
		pcolor->luminance_set = 1;
	   }
	return pcolor->luminance;
}

/* Convert RGB to HSB */
void
gx_color_to_hsb(register gs_color *pcolor, color_param hsb[3])
{
#define rhue hsb[0]
#define rsat hsb[1]
#define rbri hsb[2]
	if ( pcolor->is_gray )
	   {	rhue = 0;	/* arbitrary */
		rsat = 0;
		rbri = pcolor->red;	/* pick any one */
	   }
	else
	   {	/* Convert rgb to hsb */
		gs_color c;
		color_param V, Temp;
		long diff, H;
		c.red = pcolor->red;
		c.green = pcolor->green;
		c.blue = pcolor->blue;
		V = (c.red > c.green ? c.red : c.green);
		if ( c.blue > V ) V = c.blue;
		Temp = (c.red > c.green ? c.green : c.red);
		if ( c.blue < Temp ) Temp = c.blue;
		diff = V - Temp;
		if ( V == c.red )
			H = (c.green - c.blue) * (long)max_color_param / diff;
		else if ( V == c.green )
			H = (c.blue - c.red) * (long)max_color_param / diff + 2 * max_color_param;
		else /* V == c.blue */
			H = (c.red - c.green) * (long)max_color_param / diff + 4 * max_color_param;
		if ( H < 0 ) H += 6 * max_color_param;
		rhue = H / 6;
		rsat = diff * (long)max_color_param / V;
		rbri = V;
	   }
#undef rhue
#undef rsat
#undef rbri
}

/* Complete color specified by hsb */
void
gx_color_from_hsb(register gs_color *pcolor,
  color_param hue, color_param saturation, color_param brightness)
{	if ( saturation == 0 )
	   {	pcolor->red = pcolor->green = pcolor->blue = brightness;
	   }
	else
	   {	/* Convert hsb to rgb */
		/* The +0 in the following lines is to compensate for */
		/* a bug in the UTek C compiler (sigh). */
		float V = (float)(brightness + 0) / max_color_param;
		float S = (float)(saturation + 0) / max_color_param;
#define mcp6 (max_color_param / 6 + 1)
		int I = hue / mcp6;
		float F = (float)(hue % mcp6) / mcp6;
#undef mcp6
		float M = V * (1 - S);
		float N = V * (1 - S * F);
		float K = V * (1 - S * (1 - F));
		float R, G, B;
		switch ( I )
		   {
		default: R = V; G = K; B = M; break;
		case 1: R = N; G = V; B = M; break;
		case 2: R = M; G = V; B = K; break;
		case 3: R = M; G = N; B = V; break;
		case 4: R = K; G = M; B = V; break;
		case 5: R = V; G = M; B = N; break;
		   }
		pcolor->red = R * max_color_param;
		pcolor->green = G * max_color_param;
		pcolor->blue = B * max_color_param;
	   }
	gx_color_from_rgb(pcolor);	/* compute luminance */
}

/* ------ Internal routines ------ */

/* Heapsort (algorithm 5.2.3H, Knuth vol. 2, p. 146), */
/* modified for 0-origin indexing. */
void
gx_sort_ht_order(ht_bit *recs, uint N)
{	uint l = N >> 1;
	uint r = N - 1;
	uint j;
	ht_bit R;
	if ( N <= 1 ) return;
#define key(m) recs[m].mask
#define K R.mask
	while ( 1 )
	   {	if ( l > 0 )
			R = recs[--l];
		else
		   {	R = recs[r];
			recs[r] = recs[0];
			if ( --r == 0 )
			   {	recs[0] = R;
				break;
			   }
		   }
		j = l;
		while ( 1 )
		   {	uint i = j;
			j = j + j + 1;
			if ( j < r )
				if ( key(j) < key(j + 1) ) j++;
			if ( j > r || K >= key(j) )
			   {	recs[i] = R;
				break;	/* to outer loop */
			   }
			recs[i] = recs[j];
		   }
	   }
}
