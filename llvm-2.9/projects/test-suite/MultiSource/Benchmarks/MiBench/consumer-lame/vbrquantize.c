/*
 *	MP3 quantization
 *
 *	Copyright (c) 1999 Mark Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <assert.h>
#include "util.h"
#include "l3side.h"
#include "quantize.h"
#include "l3bitstream.h"
#include "reservoir.h"
#include "quantize-pvt.h"
#ifdef HAVEGTK
#include "gtkanal.h"
#endif



#define DEBUGXX
FLOAT8 calc_sfb_ave_noise(FLOAT8 *xr, FLOAT8 *xr34, int stride, int bw, FLOAT8 sfpow)
{
  int j;
  FLOAT8 xfsf=0;
  FLOAT8 sfpow34 = pow(sfpow,3.0/4.0);

  for ( j=0; j < stride*bw ; j += stride) {
    int ix;
    FLOAT8 temp,temp2;

    /*    ix=(int)( xr34[j]/sfpow34  + 0.4054);*/
    ix=floor( xr34[j]/sfpow34);
    if (ix > IXMAX_VAL) return -1.0;

    temp = fabs(xr[j])- pow43[ix]*sfpow;
    if (ix < IXMAX_VAL) {
      temp2 = fabs(xr[j])- pow43[ix+1]*sfpow;
      if (fabs(temp2)<fabs(temp)) temp=temp2;
    }
#ifdef MAXQUANTERROR
    temp *= temp;
    xfsf = bw*Max(xfsf,temp);
#else
    xfsf += temp * temp;
#endif
  }
  return xfsf/bw;
}



FLOAT8 find_scalefac(FLOAT8 *xr,FLOAT8 *xr34,int stride,int sfb,
		     FLOAT8 l3_xmin,int bw)
{
  FLOAT8 xfsf,sfpow,sf,sf_ok,delsf;
  int sf4,sf_ok4,delsf4;
  int i;

  /* search will range from sf:  -52.25 -> 11.25  */
  /* search will range from sf4:  -209 -> 45  */
  sf = -20.5;
  sf4 = -82;
  delsf = 32;
  delsf4 = 128;

  sf_ok =10000; 
  sf_ok4=10000;
  for (i=0; i<7; i++) {
    delsf /= 2;
    delsf4 /= 2;
    sfpow = pow(2.0,sf);
    /* sfpow = pow(2.0,sf4/4.0); */
    xfsf = calc_sfb_ave_noise(xr,xr34,stride,bw,sfpow);

    if (xfsf < 0) {
      /* scalefactors too small */
      sf += delsf; 
      sf4 += delsf4;
    }else{
      if (sf_ok==10000) sf_ok=sf;  
      if (sf_ok4==10000) sf_ok4=sf4;  
      if (xfsf > l3_xmin)  {
	/* distortion.  try a smaller scalefactor */
	sf -= delsf;
	sf4 -= delsf4;
      }else{
	sf_ok=sf;
	sf_ok4 = sf4;
	sf += delsf;
	sf4 += delsf4;
      }
    }
  } 
  /* sf_ok accurate to within +/- 2*final_value_of_delsf */
  assert(sf_ok!=10000);

  /* NOTE: noise is not a monotone function of the sf, even though
   * the number of bits used is!  do a brute force search in the 
   * neighborhood of sf_ok: 
   * 
   *  sf = sf_ok + 1.75     works  1% of the time 
   *  sf = sf_ok + 1.50     works  1% of the time 
   *  sf = sf_ok + 1.25     works  2% of the time 
   *  sf = sf_ok + 1.00     works  3% of the time 
   *  sf = sf_ok + 0.75     works  9% of the time 
   *  sf = sf_ok + 0.50     0 %  (because it was tried above)
   *  sf = sf_ok + 0.25     works 39% of the time 
   *  sf = sf_ok + 0.00     works the rest of the time
   */

  sf = sf_ok + 0.75;
  sf4 = sf_ok4 + 3;

  while (sf>(sf_ok+.01)) { 
    /* sf = sf_ok + 2*delsf was tried above, skip it:  */
    if (fabs(sf-(sf_ok+2*delsf))  < .01) sf -=.25;
    if (sf4 == sf_ok4+2*delsf4) sf4 -=1;

    sfpow = pow(2.0,sf);
    /* sfpow = pow(2.0,sf4/4.0) */
    xfsf = calc_sfb_ave_noise(xr,xr34,stride,bw,sfpow);
    if (xfsf > 0) {
      if (xfsf <= l3_xmin) return sf;
    }
    sf -= .25;
    sf4 -= 1;
  }
  return sf_ok;
}



/*
    sfb=0..5  scalefac < 16 
    sfb>5     scalefac < 8

    ifqstep = ( cod_info->scalefac_scale == 0 ) ? .5 : 1.0;
    ol_sf =  (cod_info->global_gain-210.0)/4.0;
    ol_sf -= 2*cod_info->subblock_gain[i];
    ol_sf -= ifqstep*scalefac[gr][ch].s[sfb][i];
*/
FLOAT8 compute_scalefacs_short(FLOAT8 vbrsf[SBPSY_s][3],gr_info *cod_info,int scalefac[SBPSY_s][3])
{
  FLOAT8 maxrange,maxover;
  FLOAT8 sf[SBPSY_s][3];
  int sfb,i;
  int ifqstep_inv = ( cod_info->scalefac_scale == 0 ) ? 2 : 1;

  /* make a working copy of the desired scalefacs */
  memcpy(sf,vbrsf,SBPSY_s*3*sizeof(FLOAT8));

  /* see if we should use subblock gain */


  maxover=0;
  for ( sfb = 0; sfb < SBPSY_s; sfb++ ) {
    for (i=0; i<3; ++i) {
      /* ifqstep*scalefac + 2*subblock_gain >= -sf[sfb] */
      scalefac[sfb][i]=floor( -sf[sfb][i]*ifqstep_inv  +.75 + .0001)   ;
      
      if (sfb < 6) maxrange = 15.0/ifqstep_inv;
      else maxrange = 7.0/ifqstep_inv;
      
      if (maxrange + sf[sfb][i] > maxover) maxover = maxrange+sf[sfb][i];
    }
  }
  return maxover;
}




/*
	  sfb=0..10  scalefac < 16 
	  sfb>10     scalefac < 8
		
	  ifqstep = ( cod_info->scalefac_scale == 0 ) ? .5 : 1.0;
	  ol_sf =  (cod_info->global_gain-210.0)/4.0;
	  ol_sf -= ifqstep*scalefac[gr][ch].l[sfb];
	  if (cod_info->preflag && sfb>=11) 
	  ol_sf -= ifqstep*pretab[sfb];
*/
FLOAT8 compute_scalefacs_long(FLOAT8 vbrsf[SBPSY_l],gr_info *cod_info,int scalefac[SBPSY_l])
{
  int sfb;
  FLOAT8 sf[SBPSY_l];
  FLOAT8 maxrange,maxover;
  int ifqstep_inv = ( cod_info->scalefac_scale == 0 ) ? 2 : 1;

  /* make a working copy of the desired scalefacs */
  memcpy(sf,vbrsf,SBPSY_l*sizeof(FLOAT8));

  cod_info->preflag=0;
  for ( sfb = 11; sfb < SBPSY_l; sfb++ ) {
    if (sf[sfb] + pretab[sfb]/ifqstep_inv > 0) break;
  }
  if (sfb==SBPSY_l) {
    cod_info->preflag=1;
    for ( sfb = 11; sfb < SBPSY_l; sfb++ ) 
      sf[sfb] += pretab[sfb]/ifqstep_inv;
  }

  maxover=0;
  for ( sfb = 0; sfb < SBPSY_l; sfb++ ) {
    /* ifqstep*scalefac >= -sf[sfb] */
    scalefac[sfb]=floor( -sf[sfb]*ifqstep_inv  +.75 + .0001)   ;

    if (sfb < 11) maxrange = 15.0/ifqstep_inv;
    else maxrange = 7.0/ifqstep_inv;

    if (maxrange + sf[sfb] > maxover) maxover = maxrange+sf[sfb];
  }
  return maxover;
}
  
  



/************************************************************************
 *
 * VBR_iteration_loop()   
 *
 *
 ************************************************************************/
void
VBR_iteration_loop_new (lame_global_flags *gfp,
                FLOAT8 pe[2][2], FLOAT8 ms_ener_ratio[2],
                FLOAT8 xr[2][2][576], III_psy_ratio ratio[2][2],
                III_side_info_t * l3_side, int l3_enc[2][2][576],
                III_scalefac_t scalefac[2][2])
{
  III_psy_xmin l3_xmin[2][2];
  FLOAT8    masking_lower_db;
  FLOAT8    ifqstep;
  int       start,end,bw,sfb, i,ch, gr, over;
  III_psy_xmin vbrsf;
  FLOAT8 vbrmax;


  iteration_init(gfp,l3_side,l3_enc);

  /* Adjust allowed masking based on quality setting */
  /* db_lower varies from -10 to +8 db */
  masking_lower_db = -10 + 2*gfp->VBR_q;
  /* adjust by -6(min)..0(max) depending on bitrate */
  masking_lower = pow(10.0,masking_lower_db/10);
  masking_lower = 1;


  for (gr = 0; gr < gfp->mode_gr; gr++) {
    if (convert_mdct)
      ms_convert(xr[gr],xr[gr]);
    for (ch = 0; ch < gfp->stereo; ch++) { 
      FLOAT8 xr34[576];
      gr_info *cod_info = &l3_side->gr[gr].ch[ch].tt;
      int shortblock;
      over = 0;
      shortblock = (cod_info->block_type == SHORT_TYPE);

      for(i=0;i<576;i++) {
	FLOAT8 temp=fabs(xr[gr][ch][i]);
	xr34[i]=sqrt(sqrt(temp)*temp);
      }

      calc_xmin( gfp,xr[gr][ch], &ratio[gr][ch], cod_info, &l3_xmin[gr][ch]);

      vbrmax=0;
      if (shortblock) {
	for ( sfb = 0; sfb < SBPSY_s; sfb++ )  {
	  for ( i = 0; i < 3; i++ ) {
	    start = scalefac_band.s[ sfb ];
	    end   = scalefac_band.s[ sfb+1 ];
	    bw = end - start;
	    vbrsf.s[sfb][i] = find_scalefac(&xr[gr][ch][3*start+i],&xr34[3*start+i],3,sfb,
		   masking_lower*l3_xmin[gr][ch].s[sfb][i],bw);
	    if (vbrsf.s[sfb][i]>vbrmax) vbrmax=vbrsf.s[sfb][i];
	  }
	}
      }else{
	for ( sfb = 0; sfb < SBPSY_l; sfb++ )   {
	  start = scalefac_band.l[ sfb ];
	  end   = scalefac_band.l[ sfb+1 ];
	  bw = end - start;
	  vbrsf.l[sfb] = find_scalefac(&xr[gr][ch][start],&xr34[start],1,sfb,
	  		 masking_lower*l3_xmin[gr][ch].l[sfb],bw);
	  if (vbrsf.l[sfb]>vbrmax) vbrmax = vbrsf.l[sfb];
	}

      } /* compute scalefactors */

      /* sf =  (cod_info->global_gain-210.0)/4.0; */
      cod_info->global_gain = floor(4*vbrmax +210 + .5);


      if (shortblock) {
	for ( sfb = 0; sfb < SBPSY_s; sfb++ ) {
	  for ( i = 0; i < 3; i++ ) {
	    vbrsf.s[sfb][i] -= vbrmax;
	  }
	}
	cod_info->scalefac_scale = 0;
	if (compute_scalefacs_short(vbrsf.s,cod_info,scalefac[gr][ch].s) > 0) {
	  cod_info->scalefac_scale = 1;
	  if (compute_scalefacs_short(vbrsf.s,cod_info,scalefac[gr][ch].s) >0) {
	    /* what do we do now? */
	    exit(32);
	  }
	}
      }else{
	for ( sfb = 0; sfb < SBPSY_l; sfb++ )   
	  vbrsf.l[sfb] -= vbrmax;

	/* can we get away with scalefac_scale=0? */
	cod_info->scalefac_scale = 0;
	if (compute_scalefacs_long(vbrsf.l,cod_info,scalefac[gr][ch].l) > 0) {
	  cod_info->scalefac_scale = 1;
	  if (compute_scalefacs_long(vbrsf.l,cod_info,scalefac[gr][ch].l) >0) {
	    /* what do we do now? */
	    exit(32);
	  }
	}
      } 
    } /* ch */
  } /* gr */
}



