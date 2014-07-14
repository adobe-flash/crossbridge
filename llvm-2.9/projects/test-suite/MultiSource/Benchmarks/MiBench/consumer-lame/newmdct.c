/*
 *	MP3 window subband -> subband filtering -> mdct routine
 *
 *	Copyright (c) 1999 Takehiro TOMINAGA
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 *         Special Thanks to Patrick De Smet for your advices.
 */


#include "util.h"
#include "l3side.h"
#include "newmdct.h"

#define SCALE 32768

static FLOAT8 enwindow[] = 
{
  3.5780907e-02,1.7876148e-02,3.134727e-03,2.457142e-03,
    9.71317e-04,  2.18868e-04, 1.01566e-04,  1.3828e-05,

  3.5758972e-02, 3.401756e-03,  9.83715e-04,   9.9182e-05,
      -4.77e-07,  1.03951e-04,  9.53674e-04, 2.841473e-03,
     1.2398e-05,  1.91212e-04, 2.283096e-03,1.6994476e-02,
  1.8756866e-02, 2.630711e-03,  2.47478e-04,   1.4782e-05,

  3.5694122e-02, 3.643036e-03,  9.91821e-04,   9.6321e-05,
      -4.77e-07,  1.05858e-04,  9.30786e-04, 2.521515e-03,
     1.1444e-05,  1.65462e-04, 2.110004e-03,1.6112804e-02,
  1.9634247e-02, 2.803326e-03,  2.77042e-04,   1.6689e-05,

  3.5586357e-02, 3.858566e-03,  9.95159e-04,   9.3460e-05,
      -4.77e-07,  1.07288e-04,  9.02653e-04, 2.174854e-03,
     1.0014e-05,  1.40190e-04, 1.937389e-03,1.5233517e-02,
  2.0506859e-02, 2.974033e-03,  3.07560e-04,   1.8120e-05,

  3.5435200e-02, 4.049301e-03,  9.94205e-04,   9.0599e-05,
      -4.77e-07,  1.08242e-04,  8.68797e-04, 1.800537e-03,
      9.060e-06,  1.16348e-04, 1.766682e-03,1.4358521e-02,
  2.1372318e-02,  3.14188e-03,  3.39031e-04,   1.9550e-05,

  3.5242081e-02, 4.215240e-03,  9.89437e-04,   8.7261e-05,
      -4.77e-07,  1.08719e-04,  8.29220e-04, 1.399517e-03,
      8.106e-06,   9.3937e-05, 1.597881e-03,1.3489246e-02,
  2.2228718e-02, 3.306866e-03,  3.71456e-04,   2.1458e-05,

  3.5007000e-02, 4.357815e-03,  9.80854e-04,   8.3923e-05,
      -4.77e-07,  1.08719e-04,   7.8392e-04,  9.71317e-04,
      7.629e-06,   7.2956e-05, 1.432419e-03,1.2627602e-02,
  2.3074150e-02, 3.467083e-03,  4.04358e-04,   2.3365e-05,

  3.4730434e-02, 4.477024e-03,  9.68933e-04,   8.0585e-05,
      -9.54e-07,  1.08242e-04,  7.31945e-04,  5.15938e-04,
      6.676e-06,   5.2929e-05, 1.269817e-03,1.1775017e-02,
  2.3907185e-02, 3.622532e-03,  4.38213e-04,   2.5272e-05,

  3.4412861e-02, 4.573822e-03,  9.54151e-04,   7.6771e-05,
      -9.54e-07,  1.06812e-04,  6.74248e-04,   3.3379e-05,
      6.199e-06,   3.4332e-05, 1.111031e-03,1.0933399e-02,
  2.4725437e-02, 3.771782e-03,  4.72546e-04,   2.7657e-05,

  3.4055710e-02, 4.649162e-03,  9.35555e-04,   7.3433e-05,
      -9.54e-07,  1.05381e-04,  6.10352e-04, -4.75883e-04,
      5.245e-06,   1.7166e-05,  9.56535e-04,1.0103703e-02,
  2.5527000e-02, 3.914356e-03,  5.07355e-04,   3.0041e-05,

  3.3659935e-02, 4.703045e-03,  9.15051e-04,   7.0095e-05,
      -9.54e-07,  1.02520e-04,  5.39303e-04,-1.011848e-03,
      4.768e-06,     9.54e-07,  8.06808e-04, 9.287834e-03,
  2.6310921e-02, 4.048824e-03,  5.42164e-04,   3.2425e-05,

  3.3225536e-02, 4.737377e-03,  8.91685e-04,   6.6280e-05,
     -1.431e-06,   9.9182e-05,  4.62532e-04,-1.573563e-03,
      4.292e-06,  -1.3828e-05,  6.61850e-04, 8.487225e-03,
  2.7073860e-02, 4.174709e-03,  5.76973e-04,   3.4809e-05,

  3.2754898e-02, 4.752159e-03,  8.66413e-04,   6.2943e-05,
     -1.431e-06,   9.5367e-05,  3.78609e-04,-2.161503e-03,
      3.815e-06,   -2.718e-05,  5.22137e-04, 7.703304e-03,
  2.7815342e-02, 4.290581e-03,  6.11782e-04,   3.7670e-05,

  3.2248020e-02, 4.748821e-03,  8.38757e-04,   5.9605e-05,
     -1.907e-06,   9.0122e-05,  2.88486e-04,-2.774239e-03,
      3.338e-06,  -3.9577e-05,  3.88145e-04, 6.937027e-03,
  2.8532982e-02, 4.395962e-03,  6.46591e-04,   4.0531e-05,

  3.1706810e-02, 4.728317e-03,  8.09669e-04,    5.579e-05,
     -1.907e-06,   8.4400e-05,  1.91689e-04,-3.411293e-03,
      3.338e-06,  -5.0545e-05,  2.59876e-04, 6.189346e-03,
  2.9224873e-02, 4.489899e-03,  6.80923e-04,   4.3392e-05,

  3.1132698e-02, 4.691124e-03,  7.79152e-04,   5.2929e-05,
     -2.384e-06,   7.7724e-05,   8.8215e-05,-4.072189e-03,
      2.861e-06,  -6.0558e-05,  1.37329e-04, 5.462170e-03,
  2.9890060e-02, 4.570484e-03,  7.14302e-04,   4.6253e-05,

  3.0526638e-02, 4.638195e-03,  7.47204e-04,   4.9591e-05,
   4.756451e-03,   2.1458e-05,  -6.9618e-05,    2.384e-06
};

static FLOAT8 sb_sample[2][2][18][SBLIMIT];
static FLOAT8 mm[16][SBLIMIT - 1];

#define NS 12
#define NL 36

static const int all[] = {0,2,3,5,6,8,9,11,12,14,15,17};
static FLOAT8 ca[8], cs[8];
static FLOAT8 cos_s[NS / 2][NS / 2];
static FLOAT8 cos_l[(NL / 2) * 12 + (NL / 6) * 4 + (NL / 18) * 2];
static FLOAT8 win[4][36];

#define work (&win[2][4])

/************************************************************************
*
* window_subband()
*
* PURPOSE:  Overlapping window on PCM samples
*
* SEMANTICS:
* 32 16-bit pcm samples are scaled to fractional 2's complement and
* concatenated to the end of the window buffer #x#. The updated window
* buffer #x# is then windowed by the analysis window #c# to produce the
* windowed sample #z#
*
************************************************************************/

static void window_subband(short *xk, FLOAT8 d[SBLIMIT], FLOAT8 *in)
{
    int i;
    FLOAT8 s, t, *wp;
    wp = enwindow;
    {
	t  =  xk[255];
	t += (xk[223] - xk[287]) * *wp++;
	t += (xk[191] + xk[319]) * *wp++;
	t += (xk[159] - xk[351]) * *wp++;
	t += (xk[127] + xk[383]) * *wp++;
	t += (xk[ 95] - xk[415]) * *wp++;
	t += (xk[ 63] + xk[447]) * *wp++;
	t += (xk[ 31] - xk[479]) * *wp++;
	in[15] = t;
    }

    for (i = 14; i >= 0; --i) {
	short *x1 = &xk[i];
	short *x2 = &xk[-i];
	FLOAT8 w;

	s = x2[270]; t = x1[240];
	w = *wp++; s += x2[334] * w; t += x1[176] * w;
	w = *wp++; s += x2[398] * w; t += x1[112] * w;
	w = *wp++; s += x2[462] * w; t += x1[ 48] * w;
	w = *wp++; s += x2[ 14] * w; t += x1[496] * w;
	w = *wp++; s += x2[ 78] * w; t += x1[432] * w;
	w = *wp++; s += x2[142] * w; t += x1[368] * w;
	w = *wp++; s += x2[206] * w; t += x1[304] * w;

	w = *wp++; s += x1[ 16] * w; t -= x2[494] * w;
	w = *wp++; s += x1[ 80] * w; t -= x2[430] * w;
	w = *wp++; s += x1[144] * w; t -= x2[366] * w;
	w = *wp++; s += x1[208] * w; t -= x2[302] * w;
	w = *wp++; s -= x1[272] * w; t += x2[238] * w;
	w = *wp++; s -= x1[336] * w; t += x2[174] * w;
	w = *wp++; s -= x1[400] * w; t += x2[110] * w;
	w = *wp++; s -= x1[464] * w; t += x2[ 46] * w;

	in[30 - i] = s;
	in[i] = t;
    }

    {
	s  = xk[239];
	s += xk[175] * *wp++;
	s += xk[111] * *wp++;
	s += xk[ 47] * *wp++;
	s -= xk[303] * *wp++;
	s -= xk[367] * *wp++;
	s -= xk[431] * *wp++;
	s -= xk[495] * *wp++;
	/* in[-1] = s;  */
    }

    in++;
    wp = &mm[0][0];
    for (i = 15; i >= 0; --i) {
	int j;
	FLOAT8 s0 = s; /* mm[i][0] is always 1 */
	FLOAT8 s1 = t * *wp++;
	for (j = 14; j >= 0; j--) {
	    s0 += *wp++ * *in++;
	    s1 += *wp++ * *in++;
	}
	in -= 30;
	d[i     ] = s0 + s1;
	d[31 - i] = s0 - s1;
    }
}


/*-------------------------------------------------------------------*/
/*                                                                   */
/*   Function: Calculation of the MDCT                               */
/*   In the case of long blocks (type 0,1,3) there are               */
/*   36 coefficents in the time domain and 18 in the frequency       */
/*   domain.                                                         */
/*   In the case of short blocks (type 2) there are 3                */
/*   transformations with short length. This leads to 12 coefficents */
/*   in the time and 6 in the frequency domain. In this case the     */
/*   results are stored side by side in the vector out[].            */
/*                                                                   */
/*   New layer3                                                      */
/*                                                                   */
/*-------------------------------------------------------------------*/

static void mdct_short(FLOAT8 *out, FLOAT8 *in)
{
    int m;
    for (m = NS / 2 - 1; m >= 0; --m) {
	int l;
	FLOAT8 a0, a1, a2, a3, a4, a5;
	a0 = cos_s[m][0];
	a1 = cos_s[m][1];
	a2 = cos_s[m][2];
	a3 = cos_s[m][3];
	a4 = cos_s[m][4];
	a5 = cos_s[m][5];
	for (l = 2; l >= 0; l--) {
	    out[3 * m + l] =
		a0 * in[6 * l    ] +
		a1 * in[6 * l + 1] +
		a2 * in[6 * l + 2] +
		a3 * in[6 * l + 3] +
		a4 * in[6 * l + 4] +
		a5 * in[6 * l + 5];
	}
    }
}

static void mdct_long(FLOAT8 *out, FLOAT8 *in)
{
    FLOAT8 s0, s1, s2, s3, s4, s5;
    int j = sizeof(all) / sizeof(int) - 1;
    FLOAT8 *cos_l0 = cos_l;
    do {
	out[all[j]] =
	    in[ 0] * cos_l0[ 0] +
	    in[ 1] * cos_l0[ 1] +
	    in[ 2] * cos_l0[ 2] +
	    in[ 3] * cos_l0[ 3] +
	    in[ 4] * cos_l0[ 4] +
	    in[ 5] * cos_l0[ 5] +
	    in[ 6] * cos_l0[ 6] +
	    in[ 7] * cos_l0[ 7] +
	    in[ 8] * cos_l0[ 8] +
	    in[ 9] * cos_l0[ 9] +
	    in[10] * cos_l0[10] +
	    in[11] * cos_l0[11] +
	    in[12] * cos_l0[12] +
	    in[13] * cos_l0[13] +
	    in[14] * cos_l0[14] +
	    in[15] * cos_l0[15] +
	    in[16] * cos_l0[16] +
	    in[17] * cos_l0[17];
	cos_l0 += 18;
    } while (--j >= 0);

    s0 = in[0] + in[ 5] + in[15];
    s1 = in[1] + in[ 4] + in[16];
    s2 = in[2] + in[ 3] + in[17];
    s3 = in[6] - in[ 9] + in[14];
    s4 = in[7] - in[10] + in[13];
    s5 = in[8] - in[11] + in[12];

    /* 16 */
    out[16] =
	s0 * cos_l0[0] + s1 * cos_l0[1] + s2 * cos_l0[2] +
	s3 * cos_l0[3] + s4 * cos_l0[4] + s5 * cos_l0[5];
    cos_l0 += 6;

    /* 10 */
    out[10] =
	s0 * cos_l0[0] + s1 * cos_l0[1] + s2 * cos_l0[2] +
	s3 * cos_l0[3] + s4 * cos_l0[4] + s5 * cos_l0[5];
    cos_l0 += 6;

    /* 7 */
    out[7] =
	s0 * cos_l0[0] + s1 * cos_l0[1] + s2 * cos_l0[2] +
	s3 * cos_l0[3] + s4 * cos_l0[4] + s5 * cos_l0[5];
    cos_l0 += 6;

    /* 1 */
    out[1] =
	s0 * cos_l0[0] + s1 * cos_l0[1] + s2 * cos_l0[2] +
	s3 * cos_l0[3] + s4 * cos_l0[4] + s5 * cos_l0[5];
    cos_l0 += 6;

    s0 = s0 - s1 + s5;
    s2 = s2 - s3 - s4;
    /* 13 */
    out[13] = s0 * cos_l0[0] + s2 * cos_l0[1];

    /* 4 */
    out[4] = s0 * cos_l0[2] + s2 * cos_l0[3];
}


void mdct_sub48(lame_global_flags *gfp,
    short *w0, short *w1,
    FLOAT8 mdct_freq[2][2][576],
    III_side_info_t *l3_side)
{
    int gr, k, ch;
    short *wk;
    static int init = 0;

    if ( init == 0 ) {
        void mdct_init48(void);
	mdct_init48();
	init++;
    }

    wk = w0;
    /* thinking cache performance, ch->gr loop is better than gr->ch loop */
    for (ch = 0; ch < gfp->stereo; ch++) {
	for (gr = 0; gr < gfp->mode_gr; gr++) {
	    int	band;
	    FLOAT8 *mdct_enc = mdct_freq[gr][ch];
	    gr_info *gi = &(l3_side->gr[gr].ch[ch].tt);
	    FLOAT8 *samp = sb_sample[ch][1 - gr][0];

	    for (k = 0; k < 18 / 2; k++) {
		window_subband(wk, samp, work);
		window_subband(wk + 32, samp + 32, work);
		/*
		 * Compensate for inversion in the analysis filter
		 */
		for (band = 1; band < 32; band += 2)
		    samp[band + 32] *= -1.0;
		samp += 64;
		wk += 64;
	    }


	    /* apply filters on the polyphase filterbank outputs */
	    /* bands <= gfp->highpass_band will be zeroed out below */
	    /* bands >= gfp->lowpass_band  will be zeroed out below */
	    if (gfp->filter_type==0) {
	      FLOAT8 amp,freq;
	      for (band=gfp->highpass_band+1;  band < gfp->lowpass_band ; band++) { 
		freq = band/31.0;
		if (gfp->lowpass1 < freq && freq < gfp->lowpass2) {
		  amp = cos((PI/2)*(gfp->lowpass1-freq)/(gfp->lowpass2-gfp->lowpass1));
		  for (k=0; k<18; k++) 
		    sb_sample[ch][1-gr][k][band]*=amp;
		}
		if (gfp->highpass1 < freq && freq < gfp->highpass2) {
		  amp = cos((PI/2)*(gfp->highpass2-freq)/(gfp->highpass2-gfp->highpass1));
		  for (k=0; k<18; k++) 
		    sb_sample[ch][1-gr][k][band]*=amp;
		}
	      }
	    }
	    


	    /*
	     * Perform imdct of 18 previous subband samples
	     * + 18 current subband samples
	     */
	    for (band = 0; band < 32; band++, mdct_enc += 18) 
              {
		int type = gi->block_type;
#ifdef ALLOW_MIXED
		if (gi->mixed_block_flag && band < 2)
		    type = 0;
#endif
		if (band >= gfp->lowpass_band || band <= gfp->highpass_band) {
		    memset((char *)mdct_enc,0,18*sizeof(FLOAT8));
		}else {
		  if (type == SHORT_TYPE) {
		    for (k = 2; k >= 0; --k) {
		      FLOAT8 w1 = win[SHORT_TYPE][k];
		      work[k] =
			sb_sample[ch][gr][k+6][band] * w1 -
			sb_sample[ch][gr][11-k][band];
		      work[k+3] =
			sb_sample[ch][gr][k+12][band] +
			sb_sample[ch][gr][17-k][band] * w1;
		      
		      work[k+6] =
			sb_sample[ch][gr][k+12][band] * w1 -
			sb_sample[ch][gr][17-k][band];
		      work[k+9] =
			sb_sample[ch][1-gr][k][band] +
			sb_sample[ch][1-gr][5-k][band] * w1;
		      
		      work[k+12] =
			sb_sample[ch][1-gr][k][band] * w1 -
			sb_sample[ch][1-gr][5-k][band];
		      work[k+15] =
			sb_sample[ch][1-gr][k+6][band] +
			sb_sample[ch][1-gr][11-k][band] * w1;
		    }
		    mdct_short(mdct_enc, work);
		  } else {
		    for (k = 8; k >= 0; --k) {
		      work[k] =
			win[type][k  ] * sb_sample[ch][gr][k   ][band]
			- win[type][k+9] * sb_sample[ch][gr][17-k][band];
		      
		      work[9+k] =
			win[type][k+18] * sb_sample[ch][1-gr][k   ][band]
			+ win[type][k+27] * sb_sample[ch][1-gr][17-k][band];
		    }
		    mdct_long(mdct_enc, work);
		  }
		}
		
		
		/*
		  Perform aliasing reduction butterfly
		*/
		if (type != SHORT_TYPE) {
		  if (band == 0)
		    continue;
		  for (k = 7; k >= 0; --k) {
		    FLOAT8 bu,bd;
		    bu = mdct_enc[k] * ca[k] + mdct_enc[-1-k] * cs[k];
		    bd = mdct_enc[k] * cs[k] - mdct_enc[-1-k] * ca[k];
		    
		    mdct_enc[-1-k] = bu;
		    mdct_enc[k]    = bd;
		  }
		}
	      }
	}
	wk = w1;
	if (gfp->mode_gr == 1) {
	    memcpy(sb_sample[ch][0], sb_sample[ch][1], 576 * sizeof(FLOAT8));
	}
    }
}



void mdct_init48(void)
{
    int i, k, m;
    FLOAT8 sq;
    FLOAT8 max;

    /* prepare the aliasing reduction butterflies */
    for (k = 0; k < 8; k++) {
	/*
	  This is table B.9: coefficients for aliasing reduction
	  */
	static const FLOAT8 c[8] = {
	    -0.6,-0.535,-0.33,-0.185,-0.095,-0.041,-0.0142, -0.0037
	};
	sq = 1.0 + c[k] * c[k];
	sq = sqrt(sq);
	ca[k] = c[k] / sq;
	cs[k] = 1.0 / sq;
    }

    /* type 0*/
    for (i = 0; i < 36; i++)
	win[0][i] = sin(PI/36 * (i + 0.5));
    /* type 1*/
    for (i = 0; i < 18; i++) 
	win[1][i] = win[0][i];
    for (; i < 24; i++)
	win[1][i] = 1.0;
    for (; i < 30; i++)
	win[1][i] = cos(PI/12 * (i + 0.5));
    for (; i < 36; i++)
	win[1][i] = 0.0;
    /* type 3*/
    for (i = 0; i < 36; i++)
	win[3][i] = win[1][35 - i];

    sq = 4.0 / NL;
    {
	FLOAT8 *cos_l0 = cos_l;
	static const int d3[] = {1,7,10,16};
	static const int d9[] = {4,13};

	int j = sizeof(all) / sizeof(int) - 1;
	do {
	    m = all[j];
	    for (k = 0; k < NL / 4; k++) {
		*cos_l0++ = sq *
		    cos((PI / (4 * NL)) * (2 * m + 1) * (4 * k + 2 + NL));
	    }
	    for (k = 0; k < NL / 4; k++) {
		*cos_l0++ = sq *
		    cos((PI / (4 * NL)) * (2 * m + 1) * (4 * k + 2 + NL * 3));
	    }
	} while (--j >= 0);

	j = sizeof(d3) / sizeof(int) - 1;
	do {
	    m = d3[j];
	    for (k = 0; k < 3; k++) {
		*cos_l0++ = sq *
		    cos((PI / (4 * NL)) * (2 * m + 1) * (4 * k + 2 + NL));
	    }
	    for (k = 6; k < 9; k++) {
		*cos_l0++ = sq *
		    cos((PI / (4 * NL)) * (2 * m + 1) * (4 * k + 2 + NL));
	    }
	} while (--j >= 0);

	j = sizeof(d9) / sizeof(int) - 1;
	do {
	    m = d9[j];
	    *cos_l0++ = sq *
		cos((PI / (4 * NL)) * (2 * m + 1) * (2 + NL));
	    *cos_l0++ = sq *
		cos((PI / (4 * NL)) * (2 * m + 1) * (4 * 2 + 2 + NL));
	} while (--j >= 0);
    }

    max = enwindow[256 - 8];
    {
	FLOAT8 *wp = enwindow;
	FLOAT8 *wr = enwindow;
	FLOAT8 mmax[32 - 1];

	{
	    FLOAT8 w = *wp++;
	    mmax[15] = w / max;

	    for (k = 0; k < 7; k++) {
		*wr++ = *wp++ / w;
	    }
	}

	for (i = 14; i >= 0; --i) {
	    FLOAT8 w = *wp++;
	    mmax[i] = mmax[30 - i] = w / max;

	    for (k = 0; k < 15; k++) {
		*wr++ = *wp++ / w;
	    }
	}

	{
	    wp++;
	    for (k = 0; k < 7; k++) {
		*wr++ = *wp++ / max;
	    }
	}

	wp = &mm[0][0];
	for (i = 15; i >= 0; --i) {
	    for (k = 1; k < 32; k++) {
		*wp++ = cos((2 * i + 1) * k * PI/64) * mmax[k - 1];
	    }
	}
    }

    /* swap window data*/
    for (k = 0; k < 4; k++) {
	FLOAT8 a;

	a = win[0][17-k];
	win[0][17-k] = win[0][9+k];
	win[0][9+k] = a;

	a = win[0][35-k];
	win[0][35-k] = win[0][27+k];
	win[0][27+k] = a;

	a = win[1][17-k];
	win[1][17-k] = win[1][9+k];
	win[1][9+k] = a;

	a = win[1][35-k];
	win[1][35-k] = win[1][27+k];
	win[1][27+k] = a;

	a = win[3][17-k];
	win[3][17-k] = win[3][9+k];
	win[3][9+k] = a;

	a = win[3][35-k];
	win[3][35-k] = win[3][27+k];
	win[3][27+k] = a;
    }

    for (i = 0; i < 36; i++) {
	win[0][i] *= max / SCALE;
	win[1][i] *= max / SCALE;
	win[3][i] *= max / SCALE;
    }

    /* type 2(short)*/
    sq = 4.0 / NS;
    for (i = 0; i < NS / 4; i++) {
	FLOAT8 w2 = cos(PI/12 * (i + 0.5)) * max / SCALE * sq;
	win[SHORT_TYPE][i] = tan(PI/12 * (i + 0.5));

	for (m = 0; m < NS / 2; m++) {
	    cos_s[m][i] = w2 *
		cos((PI / (4 * NS)) * (2 * m + 1) * (4 * i + 2 + NS));
	    cos_s[m][i + NS / 4] = w2 *
		cos((PI / (4 * NS)) * (2 * m + 1) * (4 * i + 2 + NS * 3));
	}
    }
}
