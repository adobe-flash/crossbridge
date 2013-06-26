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

/* gxdither.c */

/* 
 *	Improved dithering for Ghostscript.  The underlying device imaging 
 *	model supports dithering between two colors to generate intermediate
 *	shades.  
 *	
 *	The strategy is to first see if the color is either pure white or
 *	pure black.  In this case the problem is trivial.
 *	
 * 	Next, if we are on a High Quality RGB display with 8 or more
 *	bits for each R, G and B, we simply set the color and return.
 *
 *	Now, if the device is black and white, or the color happens
 *	to be achromatic, we perform simple B/W dithering.
 *	
 *	Otherwise, things are a bit more complicated.  If the device 
 * 	supports N shades of each R, G and B independently, there are a total 
 *	of N*N*N colors.  These colors form a 3-D grid in a cubical color 
 *	space.  The following dithering technique works by locating the 
 *	color we want in this 3-D color grid and finding the eight colors 
 * 	that surround it.  In the case of dithering into 8 colors with 1 
 *	bit for each red, green and blue, these eight colors will always 
 *	be the same.
 *
 *	Now we consider all possible diagonal paths between the eight colors
 *	and chose the path that runs closest to our desired color in 3-D
 *	color space.  There are 28 such paths.  Then we find the position
 *	on the path that is closest to out color.
 *
 *	The search is made more fast by always reflecting our color into
 *	the bottom octant of the cube and comparing it to 7 paths.
 *	After the best path and the best position on that path are found,
 *	the results are reflected back into the original color space.
 *
 *	NOTE: This code has been tested for B/W and Color imaging with
 *	1, 2, 3 and 8 bits per component.
 *
 *	--- original code by Paul Haeberli @ Silicon Graphics - 1990
 *	--- extensively revised by L. Peter Deutsch, Aladdin Enterprises
 */
#include "gx.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gzstate.h"
#include "gzdevice.h"
#include "gzcolor.h"
#include "gzht.h"

void gx_color_load(P2(gx_device_color *, gs_state *));

#define	WEIGHT1		(unsigned long)(100)	/* 1.0 			*/
#define	WEIGHT2		(unsigned long)(71)	/* 1/sqrt(2.0) 		*/
#define	WEIGHT3		(unsigned long)(62)	/* 1/sqrt(3.0)+tad 	*/

#define	DIAG_R		(0x1)
#define	DIAG_G		(0x2)
#define	DIAG_B		(0x4)
#define	DIAG_RG		(0x3)
#define	DIAG_GB		(0x6)
#define	DIAG_BR		(0x5)
#define	DIAG_RGB	(0x7)

static unsigned short lum[8] = {
    (0*lum_blue_weight+0*lum_green_weight+0*lum_red_weight),
    (0*lum_blue_weight+0*lum_green_weight+1*lum_red_weight),
    (0*lum_blue_weight+1*lum_green_weight+0*lum_red_weight),
    (0*lum_blue_weight+1*lum_green_weight+1*lum_red_weight),
    (1*lum_blue_weight+0*lum_green_weight+0*lum_red_weight),
    (1*lum_blue_weight+0*lum_green_weight+1*lum_red_weight),
    (1*lum_blue_weight+1*lum_green_weight+0*lum_red_weight),
    (1*lum_blue_weight+1*lum_green_weight+1*lum_red_weight),
};

void
gx_color_render(gs_color *pcolor, gx_device_color *pdevc, gs_state *pgs)
{	device *pdev = pgs->device;
	unsigned long max_value, mulval;
#define divval (max_color_param_long+1)
	unsigned long hsize;
	gx_device *dev;

/* Make a special check for black and white. */
	if ( pcolor->is_gray )
	   {	if ( pcolor->luminance == 0 )
		   {	pdevc->color2 = pdevc->color1 = pdev->black;
			pdevc->halftone_level = 0; /* pure color */
			return;
		   }
		else if ( pcolor->luminance == max_color_param )
		   {	pdevc->color2 = pdevc->color1 = pdev->white;
			pdevc->halftone_level = 0; /* pure color */
			return;
		   }
	   }

/* get a few handy values */
	dev = pdev->info;
	max_value = dev->max_rgb_value;
	mulval = max_value+1;

/* If we are on a high quality RGB display, don't bother dithering. */
	if (max_value >= 255)
	   {	color_param r = (pcolor->red*mulval)/divval;
		color_param g = (pcolor->green*mulval)/divval;
		color_param b = (pcolor->blue*mulval)/divval;
		pdevc->color2 = pdevc->color1 =
			(*dev->procs->map_rgb_color)(dev,r,g,b);
		pdevc->halftone_level = 0;	/* pure color */
		return;
	   }
	hsize = (unsigned)pgs->halftone->order_size;

/* see if we should do it in black and white */
	if ( !dev->has_color || pcolor->is_gray )
	   {

/* if bw device or gray color, use luminance of the color */
		unsigned long nshades = hsize*max_value+1;
		unsigned long lx = (nshades*color_luminance(pcolor))/divval;
		color_param l = lx/hsize;
		pdevc->halftone_level = lx%hsize;

/* check halftone level to see if we have to dither */
		pdevc->color1 = (*dev->procs->map_rgb_color)(dev,l,l,l);
		if ( pdevc->halftone_level == 0 )
			pdevc->color2 = pdevc->color1;
		else
		   {	++l;
			pdevc->color2 =
				(*dev->procs->map_rgb_color)(dev,l,l,l);
			gx_color_load(pdevc, pgs);
		   }
		return;
	   }

/* must do real color dithering */
   {	unsigned long want_r = pcolor->red*max_value;
	unsigned long want_g = pcolor->green*max_value;
	unsigned long want_b = pcolor->blue*max_value;
	color_param r = want_r/max_color_param_long;
	color_param g = want_g/max_color_param_long;
	color_param b = want_b/max_color_param_long;
	/* rem_{r,g,b} are short, so we can get away with short arithmetic. */
	color_param rem_r = (color_param)want_r-(r*max_color_param);
	color_param rem_g = (color_param)want_g-(g*max_color_param);
	color_param rem_b = (color_param)want_b-(b*max_color_param);
	int adjust_r, adjust_b, adjust_g;
	unsigned short amax;
	unsigned long dmax;
	int axisc, diagc;
	unsigned short lum_invert;
	unsigned long dot1, dot2, dot3;
	int level;

	/* Check for no dithering required */
	if ( !(rem_r | rem_g | rem_b) )
	   {	pdevc->color2 = pdevc->color1 =
		  (*dev->procs->map_rgb_color)(dev, r, g, b);
		pdevc->halftone_level = 0;
		return;
	   }

/* flip the remainder color into the 0, 0, 0 octant */
	lum_invert = 0;
#define half ((color_param)(max_color_param_long>>1))
	if ( rem_r > half )
		rem_r = max_color_param - rem_r,
		  adjust_r = -1, r++, lum_invert += lum_red_weight;
	else
		adjust_r = 1;
	if ( rem_g > half)
		rem_g = max_color_param - rem_g,
		  adjust_g = -1, g++, lum_invert += lum_green_weight;
	else
		adjust_g = 1;
	if ( rem_b > half )
		rem_b = max_color_param - rem_b,
		  adjust_b = -1, b++, lum_invert += lum_blue_weight;
	else
		adjust_b = 1;
	pdevc->color1 = (*dev->procs->map_rgb_color)(dev, r, g, b);
/* 
 * Dot the color with each axis to find the best one of 7;
 * find the color at the end of the axis chosen.
 */
	if ( rem_g > rem_r )
	   {	if ( rem_b > rem_g )
			amax = rem_b, axisc = DIAG_B;
		else
			amax = rem_g, axisc = DIAG_G;
		if ( rem_b > rem_r )
			dmax = (unsigned long)rem_g+rem_b, diagc = DIAG_GB;
		else
			dmax = (unsigned long)rem_r+rem_g, diagc = DIAG_RG;
	   }
	else
	   {	if ( rem_b > rem_r )
			amax = rem_b, axisc = DIAG_B;
		else
			amax = rem_r, axisc = DIAG_R;
		if ( rem_b > rem_g )
			dmax = (unsigned long)rem_b+rem_r, diagc = DIAG_BR;
		else
			dmax = (unsigned long)rem_r+rem_g, diagc = DIAG_RG;
	   }

	dot1 = amax*WEIGHT1;
	dot2 = dmax*WEIGHT2;
	dot3 = (ulong)rem_r+rem_g+rem_b;	/* rgb axis */
	if ( dot1 > dot2 )
	   {	if ( dot3*WEIGHT3 > dot1 )
			diagc = DIAG_RGB,
			  level = (hsize * dot3) / (3 * max_color_param_long);
		else
			diagc = axisc,
			  level = (hsize * amax) / max_color_param_long;
	   }
	else
	   {	if ( dot3*WEIGHT3 > dot2 )
			diagc = DIAG_RGB,
			  level = (hsize * dot3) / (3 * max_color_param_long);
		else
			level = (hsize * dmax) / (2 * max_color_param_long);
	   };
#ifdef DEBUG
if ( gs_debug['c'] )
   {	printf("[c]rgb=%x,%x,%x -->\n",
		(unsigned)pcolor->red, (unsigned)pcolor->green,
		(unsigned)pcolor->blue);
	printf("   %x+%x,%x+%x,%x+%x; adjust=%d,%d,%d;\n",
		(unsigned)r, (unsigned)rem_r, (unsigned)g, (unsigned)rem_g,
		(unsigned)b, (unsigned)rem_b, adjust_r, adjust_g, adjust_b);
   }
#endif

	if ( (pdevc->halftone_level = level) == 0 )
		pdevc->color2 = pdevc->color1;
	else
	   {
/* construct the second color, inverting back to original space if needed */
		if (diagc & DIAG_R) r += adjust_r;
		if (diagc & DIAG_G) g += adjust_g;
		if (diagc & DIAG_B) b += adjust_b;
/* get the second device color, sorting by luminance */
		if ( lum[diagc] < lum_invert )
		   {	pdevc->color2 = pdevc->color1;
			pdevc->color1 = (*dev->procs->map_rgb_color)(dev, r, g, b);
			pdevc->halftone_level = level = hsize - level;
		   }
		else
			pdevc->color2 = (*dev->procs->map_rgb_color)(dev, r, g, b);
		gx_color_load(pdevc, pgs);
	   }

#ifdef DEBUG
if ( gs_debug['c'] )
   {	printf("[c]diagc=%d; color1=%lx, color2=%lx, level=%d/%d\n",
		diagc, (ulong)pdevc->color1, (ulong)pdevc->color2,
		level, (unsigned)hsize);
   }
#endif

   }
}
