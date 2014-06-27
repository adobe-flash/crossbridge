
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

/* ------------------------------------------------------------------------
 * L_canny.c   Canny Edge Detection
 *
 * V
 * University of Illinois at Urbana-Champaign
 *
 * Program written by A. Tabbone (CRIN/CNRS- Lorraine, FR)
 * Modified for V by C. P. Hess  (Univ. Illinois at Urbana, USA)
 * Original copyright appears at EOF.
 *
 * $Id: L_canny.c 6134 2003-05-12 18:06:33Z lattner $
 * ------------------------------------------------------------------------ */
#include <math.h>
#include <ctype.h>
#include "V.h"

int    GaussianMask();
int    DGaussianMask();
int    dfilter();
float *dnon_max();

int L_canny(sigma, image, nc, nr, edge, err)
float    sigma;
float   *image;
int      nc;
int      nr;
float  **edge;
char    *err;
{
  float    *Ix, *Iy, *gr, *gmask, *dgmask;
  int       tgauss, tdgauss, i;
  
  if(GaussianMask (sigma, &tgauss,  &gmask,  err)) return(1);
  if(DGaussianMask(sigma, &tdgauss, &dgmask, err)) return(1);

  if(dfilter(image, gmask,  dgmask, tgauss,  tdgauss, 
	     nc, nr, &Ix, err)) return(1);
  if(dfilter(image, dgmask, gmask,  tdgauss, tgauss,  
	     nc, nr, &Iy, err)) return(1);

  gr = (float *) calloc(nr*nc, FWS);
  if (!gr) {
    sprintf(err, "Out of memory");
    return(1);
  }
  for (i=0; i<nr*nc; i++) gr[i] = MOD(Iy[i], Ix[i]);
  
  *edge = dnon_max(gr, Ix, Iy, nc, nr);

  return(0);
}


int GaussianMask(sigma, tg, mask, err)
float    sigma;
int     *tg;  
float  **mask;
char    *err;
{
  short    i;
  float   *coeff_gauss;
  float   *p;

  *tg = (short) (GAUSS_MASK*sigma);
  if (*tg % 2 == 0) *tg += 1;
 
  coeff_gauss = p = (float *)calloc(*tg, FWS);
  if (!coeff_gauss) {
    sprintf(err,"Out of memory");
    return(1);
  }

  for (i=-(*tg/2); i<=*tg/2; i++) {
    if ((i+(*tg/2)) == 0) 
      *coeff_gauss++ = 
	(float)((erf((double)(i+0.5)/(sqrt((double)2.0)*sigma))+1.0)/2.0);
    else if ((i+(*tg/2)) == *tg-1) 
      *coeff_gauss++ =
	(float)((-erf((double)(i-0.5)/(sqrt((double)2.0)*sigma))+1.0)/2.0);
    else *coeff_gauss++ =
      (float)0.5*(erf((double)(i+0.5)/(sqrt((double) 2.0)*sigma))-
		  erf((double)(i-0.5)/(sqrt((double) 2.0)*sigma)));
  }
  *mask = p; 

  return(0);
}


int DGaussianMask(sigma, tg, mask, err)
float    sigma;
int     *tg;
float  **mask;
char    *err;
{
  short   i;
  float  *coeff_dgauss;
  float  *p;
   
  *tg  = (short) (DGAUSS_MASK*sigma);
  if (*tg % 2 == 0) *tg += 1;

  coeff_dgauss = p = (float *)calloc(*tg, FWS);
  if (!coeff_dgauss) {
    sprintf(err,"Out of memory");
    return(1);
  }

  for (i=-(*tg/2); i<=*tg/2; i++) {
    if ((i+(*tg/2)) == 0)
      *coeff_dgauss++ = 
	(float)(1.0/(sigma*sqrt((double)(2.0*M_PI))))*
	  exp(-(double)((i+0.5)*(i+0.5))/(double)(2.0*sigma*sigma));
    else if ((i+(*tg/2)) == *tg-1) 
      *coeff_dgauss++ = 
	(float)-(1.0/(sigma*sqrt(2.0*M_PI)))*
	  exp(-(double)((i-0.5)*(i-0.5))/(double)(2.0*sigma*sigma));
    else *coeff_dgauss++ =
      (float)((exp(-(double)((i+0.5)*(i+0.5))/(double)(2.0*sigma*sigma))-
	       exp(-(double)((i-0.5)*(i-0.5))/(double)(2.0*sigma*sigma)))/
	      (sigma*sqrt((double) 2.0*M_PI)));
  }
  coeff_dgauss = p;
  *mask = p;

  return(0);
}


int dfilter(image, g, f, tm_g, tm_f, nc, nr, filt, err)
float  *image, *g, *f;
int     nc, nr, tm_g, tm_f;
float **filt;
char   *err;
{
  register short    i, j, l, high = nr, larg = nc;
  float             nv, *d, *d1, *d2, *temp;

  d = d1 = (float *) calloc(nc*nr, FWS);
  if (!d) {
    sprintf(err,"Out of memory");
    return(1);
  }
 
  for (i=0; i<high; i++)
    for (j=0; j<larg; j++) {
      for (l=-(tm_g/2); l<=(tm_g/2); l++) {
	if ((j+l) < 0) nv = (float) image[i*larg];
	else if ((j+l) >= larg) nv = (float) image[((i+1)*larg)-1];
	else nv = (float) image[(i*larg)+j+l];
	*d = (nv * g[(tm_g/2)-l]) + *d;
      }
      d++;
    }

  d = d1;
  
  d2 = (float *) calloc(nc*nr, FWS);
  if (!d2) {
    sprintf(err,"Out of memory");
    return(1);
  }

  for (j=0; j<larg ; j++)
    for (i=0; i<high; i++) {
      for (l=-(tm_f/2); l<=(tm_f/2); l++) {
	if ((i+l) < 0) nv = d[j];
	else if ((i+l) >= high) nv = d[((high-1)*nc)+j];
	else nv = d[((i+l)*larg)+j];
	temp = (d2+(i*larg)+j);
	*temp = (nv * f[(tm_f/2)-l]) + *temp;
      }
    }
  
  *filt = d2;
  return(0);
}

float *dnon_max(gr, Ix, Iy, nc, nr)
float  *gr;
float  *Ix;
float  *Iy;
int     nc;
int     nr;
{
  int      i, j;
  float   *maxima;
  float    R, ampl1, ampl2;
 
  maxima = (float  *) calloc(nc*nr, FWS);

  for (i=1;i<nr-2;i++)
    for (j=1;j<nc-2;j++) {
      if (ZERO(I(Ix,i,j,nc))) {
	ampl1 = I(gr,i,j-1,nc); ampl2 = I(gr,i,j+1,nc); 
      } else {
	R = I(Iy,i,j,nc) / I(Ix,i,j,nc); 
	if ((R >=0.) && (R < .4)) {  
	  ampl1 = I(gr,i+1,j+1,nc) * R + (1.-R)* I(gr,i+1,j,nc);
	  ampl2 = I(gr,i-1,j-1,nc) * R + (1.-R)* I(gr,i-1,j,nc);
	}
        /*
	  else if ( R >= 1.) {
	  ampl1 = (I(gr,i+1,j+1,nc) + (R - 1.) * I(gr,i,j+1,nc)) / R;
	  ampl2 = (I(gr,i-1,j-1,nc) + (R - 1.) * I(gr,i,j-1,nc)) / R;
	} */
          else if ((R <= 0.) && (R >  -.4)) { 
	  ampl1 = - I(gr,i+1,j-1,nc) * R + ( 1. + R)* I(gr,i+1,j,nc);
	  ampl2 = - I(gr,i-1,j+1,nc) * R + ( 1. + R)* I(gr,i-1,j,nc);
	} 
        /*
          else if (R <= -1.) {
	  ampl1 = (- I(gr,i+1,j-1,nc) + (R + 1.) * I(gr,i,j-1,nc)) / R;
	  ampl2 = (- I(gr,i-1,j+1,nc) + (R + 1.) * I(gr,i,j+1,nc)) / R;
	} */
	else
	  {
	    I(maxima,i,j,nc) = 0;
	    continue;
	  }
      }
      
      if (GEPS(I(gr,i,j,nc), ampl1) && GEPS(I(gr,i,j,nc),ampl2)) 
	I(maxima,i,j,nc) = NGMAX;
    }

  return(maxima);
}


/* ORIGINAL COPYRIGHT *****************************************************
*  Copyright (C) 1994 CRIN                                                *
*                                                                         *
*  This software was written by A. Tabbone at :                           *
*   C.R.I.N/CNRS-INRIA lorraine                                           *
*    Boite Postale 239                                                    *
*    54506 Vandoeuvre-les-Nancy Cedex                                     *
*    France                                                               *
*  It may be distributed or copied, in whole or in part, within the       *
*  following restrictions:                                                *
*    (1) It may not be sold at a profit.                                  *
*    (2) This credit and notice must remain intact.                       *
*  This software may be distributed with other software by a commercial   *
*  vendor, provided that it is included at no additional charge.          *
*                                                                         *
*  Please report bugs to tabbone@loria.crin.fr                            *
***************************************************************************/




