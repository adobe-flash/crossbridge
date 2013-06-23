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

/* gscolor.c */
/* Color and halftone operators for GhostScript library */
#include "gx.h"
#include "gserrors.h"
#include "gxfixed.h"			/* ditto */
#include "gxmatrix.h"			/* for gzstate.h */
#include "gxdevice.h"			/* for gx_color_index */
#include "gzstate.h"
#include "gzcolor.h"
#include "gzht.h"

/* Exported values */
int gs_screen_enum_sizeof = sizeof(gs_screen_enum);
int gs_color_sizeof = sizeof(gs_color);

/* Forward declarations */
private int check_unit(P2(floatp, float *));
private int tri_param(P4(floatp, floatp, floatp, color_param [3]));
private void tri_return(P4(color_param, color_param, color_param, float [3]));
/* Imported from gxcolor.c */
void	gx_color_from_hsb(P4(gs_color *, color_param, color_param, color_param)),
	gx_color_from_rgb(P1(gs_color *)),
	gx_color_to_hsb(P2(gs_color *, color_param [3])),
	gx_sort_ht_order(P2(ht_bit *, uint));

/* setgray */
int
gs_setgray(gs_state *pgs, floatp gray)
{	float fgray;
	int code;
	gs_color *pcolor = pgs->color;
	if ( pgs->in_cachedevice ) return_error(gs_error_undefined);
	code = check_unit(gray, &fgray);
	if ( !code )
	   {	pcolor->luminance = max_color_param * fgray;
		pcolor->red = pcolor->green = pcolor->blue =
		  pcolor->luminance;
		pcolor->is_gray = pcolor->luminance_set = 1;
	   }
	return code;
}

/* currentgray */
float
gs_currentgray(gs_state *pgs)
{	return (float)(color_luminance(pgs->color) / (float)max_color_param);
}

/* setgscolor */
int
gs_setgscolor(gs_state *pgs, gs_color *pcolor)
{	if ( pgs->in_cachedevice ) return_error(gs_error_undefined);
	*pgs->color = *pcolor;
	return 0;
}

/* currentgscolor */
int
gs_currentgscolor(gs_state *pgs, gs_color *pcolor)
{	*pcolor = *pgs->color;
	return 0;
}

/* sethsbcolor */
int
gs_sethsbcolor(gs_state *pgs, floatp h, floatp s, floatp b)
{	color_param params[3];
	int code;
	if ( pgs->in_cachedevice ) return_error(gs_error_undefined);
	code = tri_param(h, s, b, params);
	if ( !code )
	   {	gx_color_from_hsb(pgs->color, params[0], params[1], params[2]);
	   }
	return code;
}

/* currenthsbcolor */
int
gs_currenthsbcolor(gs_state *pgs, float pr3[3])
{	color_param hsb[3];
	gx_color_to_hsb(pgs->color, hsb);
	tri_return(hsb[0], hsb[1], hsb[2], pr3);
	return 0;
}

/* setrgbcolor */
int
gs_setrgbcolor(gs_state *pgs, floatp r, floatp g, floatp b)
{	color_param params[3];
	int code;
	if ( pgs->in_cachedevice ) return_error(gs_error_undefined);
	code = tri_param(r, g, b, params);
	if ( !code )
	   {	gs_color *pcolor = pgs->color;
		pcolor->red = params[0];
		pcolor->green = params[1];
		pcolor->blue = params[2];
		gx_color_from_rgb(pcolor);
	   }
	return code;
}

/* currentrgbcolor */
int
gs_currentrgbcolor(gs_state *pgs, float pr3[3])
{	gs_color *pcolor = pgs->color;
	tri_return(pcolor->red, pcolor->green, pcolor->blue, pr3);
	return 0;
}
/* A special entry for makeimagedevice, for converting the palette. */
int
gs_colorrgb(gs_color *pcolor, float pr3[3])
{	tri_return(pcolor->red, pcolor->green, pcolor->blue, pr3);
	return 0;
}

/* setscreen */
int
gs_setscreen(gs_state *pgs,
  floatp freq, floatp angle, float (*proc)(P2(floatp, floatp)))
{	gs_screen_enum senum;
	gs_point pt;
	int code = gs_screen_init(&senum, pgs, freq, angle);
	if ( code < 0 ) return code;
	while ( (code = gs_screen_currentpoint(&senum, &pt)) == 0 )
		if ( (code = gs_screen_next(&senum, (*proc)(pt.x, pt.y))) < 0 )
			return code;
	if ( code < 0 ) return code;
	pgs->ht_proc = proc;
	return 0;
}

/* currentscreen */
int
gs_currentscreen(gs_state *pgs,
  float *pfreq, float *pangle, float (**pproc)(P2(floatp, floatp)))
{	halftone *pht = pgs->halftone;
	*pfreq = pht->frequency;
	*pangle = pht->angle;
	*pproc = pgs->ht_proc;
	return 0;
}

/* ------ Halftone sampling ------ */

/* Set up for halftone sampling */
int
gs_screen_init(gs_screen_enum *penum, gs_state *pgs,
  floatp freq, floatp angle)
{	int cwidth, cheight;
	int code;
	ht_bit *order;
	if ( freq < 0 ) return_error(gs_error_rangecheck);
	/* Convert the frequency to cell width and height */
	   {	float cell_size = 72.0 / freq;
		gs_point pcwh;
		gs_matrix imat = {};
		int dev_w, dev_h;
		gs_deviceparams(gs_currentdevice(pgs), &imat, &dev_w, &dev_h);
		if ( (code = gs_distance_transform(cell_size, cell_size,
						   &imat, &pcwh)) < 0
		    ) return code;
		/* It isn't clear to me whether we should round the */
		/* width and height, truncate them, or do something */
		/* more complicated.  All the problems arise from devices */
		/* whose X and Y resolutions aren't the same: */
		/* the halftone model isn't really designed for this. */
		/* For the moment, truncate and hope for the best. */
#define abs_round(z) (z < 0 ? -(int)(z) : (int)(z))
/*#define abs_round(z) (z < 0 ? -(int)(z - 0.5) : (int)(z + 0.5))*/
		cwidth = abs_round(pcwh.x);
		cheight = abs_round(pcwh.y);
#undef abs_round
	   }
	if ( cwidth == 0 ) cwidth = 1;
	if ( cheight == 0 ) cheight = 1;
	if ( cwidth > max_ushort / cheight )
		return_error(gs_error_rangecheck);
	order = (ht_bit *)gs_malloc(cwidth * cheight, sizeof(ht_bit),
				    "halftone samples");
	if ( order == 0 ) return_error(gs_error_VMerror);
	penum->freq = freq;
	penum->angle = angle;
	penum->order = order;
	penum->width = cwidth;
	penum->height = cheight;
	penum->x = penum->y = 0;
	penum->pgs = pgs;
	/* The transformation matrix must include normalization to the */
	/* interval (-1..1), and rotation by the negative of the angle. */
	   {	float xscale = 2.0 / cwidth;
		float yscale = 2.0 / cheight;
		gs_matrix mat;
		gs_make_identity(&mat);
		mat.xx = xscale;
		mat.yy = yscale;
		mat.tx = xscale * 0.5 - 1.0;
		mat.ty = yscale * 0.5 - 1.0;
		if ( (code = gs_matrix_rotate(&mat, -angle, &penum->mat)) < 0 )
			return code;
#ifdef DEBUG
if ( gs_debug['h'] )
	printf("[h]Screen: w=%d h=%d [%f %f %f %f %f %f]\n",
		cwidth, cheight, penum->mat.xx, penum->mat.xy,
		penum->mat.yx, penum->mat.yy, penum->mat.tx, penum->mat.ty);
#endif
	   }
	return 0;
}

/* Report current point for sampling */
private int gx_screen_finish(P1(gs_screen_enum *));
int
gs_screen_currentpoint(gs_screen_enum *penum, gs_point *ppt)
{	gs_point pt;
	int code;
	if ( penum->y >= penum->height )	/* all done */
		return gx_screen_finish(penum);
	if ( (code = gs_point_transform((floatp)penum->x, (floatp)penum->y, &penum->mat, &pt)) < 0 )
		return code;
	if ( pt.x < -1.0 ) pt.x += 2.0;
	else if ( pt.x > 1.0 ) pt.x -= 2.0;
	if ( pt.y < -1.0 ) pt.y += 2.0;
	else if ( pt.y > 1.0 ) pt.y -= 2.0;
	*ppt = pt;
	return 0;
}

/* Record next halftone sample */
int
gs_screen_next(gs_screen_enum *penum, floatp value)
{	ushort sample;
	ht_bit *order = penum->order;
	if ( value < -1.0 || value > 1.0 )
	  return_error(gs_error_rangecheck);
	sample = (ushort)(value * (float)(int)(max_ushort >> 1)) +
		   (max_ushort >> 1);
#ifdef DEBUG
if ( gs_debug['h'] )
   {	gs_point pt;
	gs_screen_currentpoint(penum, &pt);
	printf("[h]sample x=%d y=%d (%f,%f): %f -> %u\n",
		penum->x, penum->y, pt.x, pt.y, value, sample);
   }
#endif
	order[penum->y * penum->width + penum->x].mask = sample;
	if ( ++(penum->x) >= penum->width )
		penum->x = 0, ++(penum->y);
	return 0;
}

/* All points have been sampled. */
/* Finish constructing the halftone. */
private int
gx_screen_finish(gs_screen_enum *penum)
{	ht_bit *order = penum->order;
	uint size = penum->width * penum->height;
	uint i;
	int code;
	halftone *pht;
	/* Label each element with its ordinal position. */
	for ( i = 0; i < size; i++ )
		order[i].offset = i;
	/* Sort the samples in increasing order by value. */
	gx_sort_ht_order(order, size);
	/* Set up the actual halftone description. */
	code = gx_ht_construct_order(penum->order, penum->width, penum->height);
	if ( code < 0 ) return code;
	pht = penum->pgs->halftone;
	pht->frequency = penum->freq;
	pht->angle = penum->angle;
	pht->width = penum->width;
	pht->height = penum->height;
	pht->order = penum->order;
	pht->order_size = pht->width * pht->height;
	return 1;			/* all done */
}

/* ------ Internal routines ------ */

/* Force a parameter into the range [0..1]. */
private int
check_unit(floatp fval, float *pp)
{	if ( fval < 0.0 )
		*pp = 0.0;
	else if ( fval > 1.0 )
		*pp = 1.0;
	else
		*pp = fval;
	return 0;
}

/* Get 3 real parameters in the range [0..1], */
/* and convert them to color_params. */
/* If any of them are invalid, none of the return values are set. */
private int
tri_param(floatp p1, floatp p2, floatp p3, color_param pq3[3])
{	int code;
	float f1, f2, f3;
	if (	(code = check_unit(p1, &f1)) < 0 ||
		(code = check_unit(p2, &f2)) < 0 ||
		(code = check_unit(p3, &f3)) < 0
	   ) return code;
	pq3[0] = f1 * max_color_param;
	pq3[1] = f2 * max_color_param;
	pq3[2] = f3 * max_color_param;
	return 0;
}

/* Convert 3 color_params to reals */
private void
tri_return(color_param p1, color_param p2, color_param p3, float pr3[3])
{	pr3[0] = p1 / (float)max_color_param;
	pr3[1] = p2 / (float)max_color_param;
	pr3[2] = p3 / (float)max_color_param;
}
