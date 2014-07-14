/**********************************************************************
 *   date   programmers         comment                               *
 * 2/25/91  Davis Pan           start of version 1.0 records          *
 * 5/10/91  W. Joseph Carter    Ported to Macintosh and Unix.         *
 * 7/10/91  Earle Jennings      Ported to MsDos.                      *
 *                              replace of floats with FLOAT          *
 * 2/11/92  W. Joseph Carter    Fixed mem_alloc() arg for "absthr".   *
 * 3/16/92  Masahiro Iwadare	Modification for Layer III            *
 * 17/4/93  Masahiro Iwadare    Updated for IS Modification           *
 **********************************************************************/

#include "util.h"
#include "encoder.h"
#include "psymodel.h"
#include "l3side.h"
#include <assert.h>
#ifdef HAVEGTK
#include "gtkanal.h"
#endif
#include "tables.h"
#include "fft.h"

#ifdef M_LN10
#define		LN_TO_LOG10		(M_LN10/10)
#else
#define         LN_TO_LOG10             0.2302585093
#endif


void L3para_read( FLOAT8 sfreq, int numlines[CBANDS],int numlines_s[CBANDS], int partition_l[HBLKSIZE],
		  FLOAT8 minval[CBANDS], FLOAT8 qthr_l[CBANDS], 
		  FLOAT8 s3_l[CBANDS + 1][CBANDS + 1],
		  FLOAT8 s3_s[CBANDS + 1][CBANDS + 1],
                  FLOAT8 qthr_s[CBANDS],
		  FLOAT8 SNR_s[CBANDS],
		  int bu_l[SBPSY_l], int bo_l[SBPSY_l],
		  FLOAT8 w1_l[SBPSY_l], FLOAT8 w2_l[SBPSY_l],
		  int bu_s[SBPSY_s], int bo_s[SBPSY_s],
		  FLOAT8 w1_s[SBPSY_s], FLOAT8 w2_s[SBPSY_s] );
									







 

void L3psycho_anal( lame_global_flags *gfp,
                    short int *buffer[2],int gr_out , 
                    FLOAT8 *ms_ratio,
                    FLOAT8 *ms_ratio_next,
		    FLOAT8 *ms_ener_ratio,
		    III_psy_ratio masking_ratio[2][2],
		    III_psy_ratio masking_MS_ratio[2][2],
		    FLOAT8 percep_entropy[2],FLOAT8 percep_MS_entropy[2], 
                    int blocktype_d[2])
{

/* to get a good cache performance, one has to think about
 * the sequence, in which the variables are used
 */
  
/* The static variables "r", "phi_sav", "new", "old" and "oldest" have    */
/* to be remembered for the unpredictability measure.  For "r" and        */
/* "phi_sav", the first index from the left is the channel select and     */
/* the second index is the "age" of the data.                             */
  static FLOAT8	minval[CBANDS],qthr_l[CBANDS];
  static FLOAT8	qthr_s[CBANDS];
  static FLOAT8	nb_1[4][CBANDS], nb_2[4][CBANDS];
  static FLOAT8 s3_s[CBANDS + 1][CBANDS + 1];
  static FLOAT8 s3_l[CBANDS + 1][CBANDS + 1];

  static III_psy_xmin thm[4];
  static III_psy_xmin en[4];
  
  /* unpredictability calculation
   */
  static int cw_upper_index;
  static int cw_lower_index;
  static FLOAT ax_sav[4][2][HBLKSIZE];
  static FLOAT bx_sav[4][2][HBLKSIZE];
  static FLOAT rx_sav[4][2][HBLKSIZE];
  static FLOAT cw[HBLKSIZE];

  /* fft and energy calculation
   */
  FLOAT (*wsamp_l)[BLKSIZE];
  FLOAT (*wsamp_s)[3][BLKSIZE_s];
  FLOAT tot_ener[4];
  static FLOAT wsamp_L[2][BLKSIZE];
  static FLOAT energy[HBLKSIZE];
  static FLOAT wsamp_S[2][3][BLKSIZE_s];
  static FLOAT energy_s[3][HBLKSIZE_s];

  /* convolution
   */
  static FLOAT8 eb[CBANDS];
  static FLOAT8 cb[CBANDS];
  static FLOAT8 thr[CBANDS];
  
  /* Scale Factor Bands
   */
  static FLOAT8	w1_l[SBPSY_l], w2_l[SBPSY_l];
  static FLOAT8	w1_s[SBPSY_s], w2_s[SBPSY_s];
  static FLOAT8 mld_l[SBPSY_l],mld_s[SBPSY_s];
  static int	bu_l[SBPSY_l],bo_l[SBPSY_l] ;
  static int	bu_s[SBPSY_s],bo_s[SBPSY_s] ;
  static int	npart_l,npart_s;
  static int	npart_l_orig,npart_s_orig;
  
  static int	s3ind[CBANDS][2];
  static int	s3ind_s[CBANDS][2];

  static int	numlines_s[CBANDS] ;
  static int	numlines_l[CBANDS];
  static int	partition_l[HBLKSIZE];
  
  /* frame analyzer 
   */
#ifdef HAVEGTK
  static FLOAT energy_save[4][HBLKSIZE];
  static FLOAT8 pe_save[4];
  static FLOAT8 ers_save[4];
#endif

  /* ratios 
   */
  static FLOAT8 pe[4]={0,0,0,0};
  static FLOAT8 ms_ratio_s_old=0,ms_ratio_l_old=0;
  static FLOAT8 ms_ener_ratio_old=.25;
  FLOAT8 ms_ratio_l=0,ms_ratio_s=0;

  /* block type 
   */
  static int	blocktype_old[2];
  int blocktype[2],uselongblock[2];
  
  /* usual variables like loop indices, etc..
   */
  int numchn, chn;
  int   b, i, j, k;
  int	sb,sblock;
  FLOAT cwlimit;


  /* initialization of static variables
   */
  if((gfp->frameNum==0) && (gr_out==0)){
    FLOAT8	SNR_s[CBANDS];
    
    blocktype_old[0]=STOP_TYPE;
    blocktype_old[1]=STOP_TYPE;
    i = gfp->out_samplerate;
    switch(i){
    case 32000: break;
    case 44100: break;
    case 48000: break;
    case 16000: break;
    case 22050: break;
    case 24000: break;
    default:    fprintf(stderr,"error, invalid sampling frequency: %d Hz\n",i);
      exit(-1);
    }
    
    /* reset states used in unpredictability measure */
    memset (rx_sav,0, sizeof(rx_sav));
    memset (ax_sav,0, sizeof(ax_sav));
    memset (bx_sav,0, sizeof(bx_sav));
    memset (en,0, sizeof(en));
    memset (thm,0, sizeof(thm));
    

    /*  gfp->cwlimit = sfreq*j/1024.0;  */
    cw_lower_index=6;
    if (gfp->cwlimit>0) 
      cwlimit=gfp->cwlimit;
    else
      cwlimit=8.8717;
    cw_upper_index = cwlimit*1000.0*1024.0/((FLOAT8) gfp->out_samplerate);
    cw_upper_index=Min(HBLKSIZE-4,cw_upper_index);      /* j+3 < HBLKSIZE-1 */
    cw_upper_index=Max(6,cw_upper_index);

    for ( j = 0; j < HBLKSIZE; j++ )
      cw[j] = 0.4;
    
    /* setup stereo demasking thresholds */
    /* formula reverse enginerred from plot in paper */
    for ( sb = 0; sb < SBPSY_s; sb++ ) {
      FLOAT8 mld = 1.25*(1-cos(PI*sb/SBPSY_s))-2.5;
      mld_s[sb] = pow(10.0,mld);
    }
    for ( sb = 0; sb < SBPSY_l; sb++ ) {
      FLOAT8 mld = 1.25*(1-cos(PI*sb/SBPSY_l))-2.5;
      mld_l[sb] = pow(10.0,mld);
    }
    
    for (i=0;i<HBLKSIZE;i++) partition_l[i]=-1;

    L3para_read( (FLOAT8) gfp->out_samplerate,numlines_l,numlines_s,partition_l,minval,qthr_l,s3_l,s3_s,
		 qthr_s,SNR_s,
		 bu_l,bo_l,w1_l,w2_l, bu_s,bo_s,w1_s,w2_s );
    
    
    /* npart_l_orig   = number of partition bands before convolution */
    /* npart_l  = number of partition bands after convolution */
    npart_l_orig=0; npart_s_orig=0;
    for (i=0;i<HBLKSIZE;i++) 
      if (partition_l[i]>npart_l_orig) npart_l_orig=partition_l[i];
    npart_l_orig++;

    for (i=0;numlines_s[i]>=0;i++)
      ;
    npart_s_orig = i;
    
    npart_l=bo_l[SBPSY_l-1]+1;
    npart_s=bo_s[SBPSY_s-1]+1;

    /* MPEG2 tables are screwed up 
     * the mapping from paritition bands to scalefactor bands will use
     * more paritition bands than we have.  
     * So we will not compute these fictitious partition bands by reducing
     * npart_l below.  */
    if (npart_l > npart_l_orig) {
      npart_l=npart_l_orig;
      bo_l[SBPSY_l-1]=npart_l-1;
      w2_l[SBPSY_l-1]=1.0;
    }
    if (npart_s > npart_s_orig) {
      npart_s=npart_s_orig;
      bo_s[SBPSY_s-1]=npart_s-1;
      w2_s[SBPSY_s-1]=1.0;
    }
    
    
    
    for (i=0; i<npart_l; i++) {
      for (j = 0; j < npart_l_orig; j++) {
	if (s3_l[i][j] != 0.0)
	  break;
      }
      s3ind[i][0] = j;
      
      for (j = npart_l_orig - 1; j > 0; j--) {
	if (s3_l[i][j] != 0.0)
	  break;
      }
      s3ind[i][1] = j;
    }


    for (i=0; i<npart_s; i++) {
      for (j = 0; j < npart_s_orig; j++) {
	if (s3_s[i][j] != 0.0)
	  break;
      }
      s3ind_s[i][0] = j;
      
      for (j = npart_s_orig - 1; j > 0; j--) {
	if (s3_s[i][j] != 0.0)
	  break;
      }
      s3ind_s[i][1] = j;
    }
    
    
    /*  
      #include "debugscalefac.c"
    */
    

#define AACS3
#define NEWS3XX

#ifdef AACS3
    /* AAC values, results in more masking over MP3 values */
# define TMN 18
# define NMT 6
#else
    /* MP3 values */
# define TMN 29
# define NMT 6
#endif

#define rpelev 2
#define rpelev2 16

    /* compute norm_l, norm_s instead of relying on table data */
    for ( b = 0;b < npart_l; b++ ) {
      FLOAT8 norm=0;
      for ( k = s3ind[b][0]; k <= s3ind[b][1]; k++ ) {
	norm += s3_l[b][k];
      }
      for ( k = s3ind[b][0]; k <= s3ind[b][1]; k++ ) {
	s3_l[b][k] *= exp(-LN_TO_LOG10 * NMT) / norm;
      }
      /*printf("%i  norm=%f  norm_l=%f \n",b,1/norm,norm_l[b]);*/
    }

    /* MPEG1 SNR_s data is given in db, convert to energy */
    if (gfp->version == 1) {
      for ( b = 0;b < npart_s; b++ ) {
	SNR_s[b]=exp( (FLOAT8) SNR_s[b] * LN_TO_LOG10 );
      }
    }

    for ( b = 0;b < npart_s; b++ ) {
      FLOAT8 norm=0;
      for ( k = s3ind_s[b][0]; k <= s3ind_s[b][1]; k++ ) {
	norm += s3_s[b][k];
      }
      for ( k = s3ind_s[b][0]; k <= s3ind_s[b][1]; k++ ) {
	s3_s[b][k] *= SNR_s[b] / norm;
      }
      /*printf("%i  norm=%f  norm_s=%f \n",b,1/norm,norm_l[b]);*/
    }
    
    init_fft();
  }
  /************************* End of Initialization *****************************/
  


  
  
  numchn = gfp->stereo;
  /* chn=2 and 3 = Mid and Side channels */
  if (gfp->mode == MPG_MD_JOINT_STEREO) numchn=4;
  for (chn=0; chn<numchn; chn++) {
  
    wsamp_s = wsamp_S+(chn & 1);
    wsamp_l = wsamp_L+(chn & 1);


    if (chn<2) {    
      /**********************************************************************
       *  compute FFTs
       **********************************************************************/
      fft_long ( *wsamp_l, chn, buffer);
      fft_short( *wsamp_s, chn, buffer); 
      
      /* LR maskings  */
      percep_entropy[chn] = pe[chn]; 
      masking_ratio[gr_out][chn].thm = thm[chn];
      masking_ratio[gr_out][chn].en = en[chn];
    }else{
      /* MS maskings  */
      percep_MS_entropy[chn-2] = pe[chn]; 
      masking_MS_ratio[gr_out][chn-2].en = en[chn];
      masking_MS_ratio[gr_out][chn-2].thm = thm[chn];
      
      if (chn == 2)
      {
        for (j = BLKSIZE-1; j >=0 ; --j)
        {
          FLOAT l = wsamp_L[0][j];
          FLOAT r = wsamp_L[1][j];
          wsamp_L[0][j] = (l+r)*(FLOAT)(SQRT2*0.5);
          wsamp_L[1][j] = (l-r)*(FLOAT)(SQRT2*0.5);
        }
        for (b = 2; b >= 0; --b)
        {
          for (j = BLKSIZE_s-1; j >= 0 ; --j)
          {
            FLOAT l = wsamp_S[0][b][j];
            FLOAT r = wsamp_S[1][b][j];
            wsamp_S[0][b][j] = (l+r)*(FLOAT)(SQRT2*0.5);
            wsamp_S[1][b][j] = (l-r)*(FLOAT)(SQRT2*0.5);
          }
        }
      }
    }

    /**********************************************************************
     *  compute energies
     **********************************************************************/
    
    
    
    energy[0]  = (*wsamp_l)[0];
    energy[0] *= energy[0];
    
    tot_ener[chn] = energy[0]; /* sum total energy at nearly no extra cost */
    
    for (j=BLKSIZE/2-1; j >= 0; --j)
    {
      FLOAT re = (*wsamp_l)[BLKSIZE/2-j];
      FLOAT im = (*wsamp_l)[BLKSIZE/2+j];
      energy[BLKSIZE/2-j] = (re * re + im * im) * (FLOAT)0.5;
      
      tot_ener[chn] += energy[BLKSIZE/2-j];
    }
    for (b = 2; b >= 0; --b)
    {
      energy_s[b][0]  = (*wsamp_s)[b][0];
      energy_s[b][0] *=  energy_s [b][0];
      for (j=BLKSIZE_s/2-1; j >= 0; --j)
      {
        FLOAT re = (*wsamp_s)[b][BLKSIZE_s/2-j];
        FLOAT im = (*wsamp_s)[b][BLKSIZE_s/2+j];
        energy_s[b][BLKSIZE_s/2-j] = (re * re + im * im) * (FLOAT)0.5;
      }
    }


#ifdef HAVEGTK
  if(gfp->gtkflag) {
    for (j=0; j<HBLKSIZE ; j++) {
      pinfo->energy[gr_out][chn][j]=energy_save[chn][j];
      energy_save[chn][j]=energy[j];
    }
  }
#endif
    
    /**********************************************************************
     *    compute unpredicatability of first six spectral lines            * 
     **********************************************************************/
    for ( j = 0; j < cw_lower_index; j++ )
      {	 /* calculate unpredictability measure cw */
	FLOAT an, a1, a2;
	FLOAT bn, b1, b2;
	FLOAT rn, r1, r2;
	FLOAT numre, numim, den;

	a2 = ax_sav[chn][1][j];
	b2 = bx_sav[chn][1][j];
	r2 = rx_sav[chn][1][j];
	a1 = ax_sav[chn][1][j] = ax_sav[chn][0][j];
	b1 = bx_sav[chn][1][j] = bx_sav[chn][0][j];
	r1 = rx_sav[chn][1][j] = rx_sav[chn][0][j];
	an = ax_sav[chn][0][j] = (*wsamp_l)[j];
	bn = bx_sav[chn][0][j] = j==0 ? (*wsamp_l)[0] : (*wsamp_l)[BLKSIZE-j];  
	rn = rx_sav[chn][0][j] = sqrt(energy[j]);

	{ /* square (x1,y1) */
	  if( r1 != 0 ) {
	    numre = (a1*b1);
	    numim = (a1*a1-b1*b1)*(FLOAT)0.5;
	    den = r1*r1;
	  } else {
	    numre = 1;
	    numim = 0;
	    den = 1;
	  }
	}
	
	{ /* multiply by (x2,-y2) */
	  if( r2 != 0 ) {
	    FLOAT tmp2 = (numim+numre)*(a2+b2)*(FLOAT)0.5;
	    FLOAT tmp1 = -a2*numre+tmp2;
	    numre =       -b2*numim+tmp2;
	    numim = tmp1;
	    den *= r2;
	  } else {
	    /* do nothing */
	  }
	}
	
	{ /* r-prime factor */
	  FLOAT tmp = (2*r1-r2)/den;
	  numre *= tmp;
	  numim *= tmp;
	}
	den=rn+fabs(2*r1-r2);
	if( den != 0 ) {
	  numre = (an+bn)*(FLOAT)0.5-numre;
	  numim = (an-bn)*(FLOAT)0.5-numim;
	  den = sqrt(numre*numre+numim*numim)/den;
	}
	cw[j] = den;
      }



    /**********************************************************************
     *     compute unpredicatibility of next 200 spectral lines            *
     **********************************************************************/ 
    for ( j = cw_lower_index; j < cw_upper_index; j += 4 )
      {/* calculate unpredictability measure cw */
	FLOAT rn, r1, r2;
	FLOAT numre, numim, den;
	
	k = (j+2) / 4; 
	
	{ /* square (x1,y1) */
	  r1 = energy_s[0][k];
	  if( r1 != 0 ) {
	    FLOAT a1 = (*wsamp_s)[0][k]; 
	    FLOAT b1 = (*wsamp_s)[0][BLKSIZE_s-k]; /* k is never 0 */
	    numre = (a1*b1);
	    numim = (a1*a1-b1*b1)*(FLOAT)0.5;
	    den = r1;
	    r1 = sqrt(r1);
	  } else {
	    numre = 1;
	    numim = 0;
	    den = 1;
	  }
	}
	
	
	{ /* multiply by (x2,-y2) */
	  r2 = energy_s[2][k];
	  if( r2 != 0 ) {
	    FLOAT a2 = (*wsamp_s)[2][k]; 
	    FLOAT b2 = (*wsamp_s)[2][BLKSIZE_s-k];
	    
	    
	    FLOAT tmp2 = (numim+numre)*(a2+b2)*(FLOAT)0.5;
	    FLOAT tmp1 = -a2*numre+tmp2;
	    numre =       -b2*numim+tmp2;
	    numim = tmp1;
	    
	    r2 = sqrt(r2);
	    den *= r2;
	  } else {
	    /* do nothing */
	  }
	}
	
	{ /* r-prime factor */
	  FLOAT tmp = (2*r1-r2)/den;
	  numre *= tmp;
	  numim *= tmp;
	}
	
	rn = sqrt(energy_s[1][k]);
	den=rn+fabs(2*r1-r2);
	if( den != 0 ) {
	  FLOAT an = (*wsamp_s)[1][k]; 
	  FLOAT bn = (*wsamp_s)[1][BLKSIZE_s-k];
	  numre = (an+bn)*(FLOAT)0.5-numre;
	  numim = (an-bn)*(FLOAT)0.5-numim;
	  den = sqrt(numre*numre+numim*numim)/den;
	}
	
	cw[j+1] = cw[j+2] = cw[j+3] = cw[j] = den;
      }
    
#if 0
    for ( j = 14; j < HBLKSIZE-4; j += 4 )
      {/* calculate energy from short ffts */
	FLOAT8 tot,ave;
	k = (j+2) / 4; 
	for (tot=0, sblock=0; sblock < 3; sblock++)
	  tot+=energy_s[sblock][k];
	ave = energy[j+1]+ energy[j+2]+ energy[j+3]+ energy[j];
	ave /= 4.;
	/*
	  printf("energy / tot %i %5.2f   %e  %e\n",j,ave/(tot*16./3.),
	  ave,tot*16./3.);
	*/
	energy[j+1] = energy[j+2] = energy[j+3] =  energy[j]=tot;
      }
#endif
    
    
    
    
    
    
    
    
    /**********************************************************************
     *    Calculate the energy and the unpredictability in the threshold   *
     *    calculation partitions                                           *
     **********************************************************************/
#if 0
    for ( b = 0; b < CBANDS; b++ )
      {
	eb[b] = 0;
	cb[b] = 0;
      }
    for ( j = 0; j < HBLKSIZE; j++ )
      {
	int tp = partition_l[j];
	
	if ( tp >= 0 )
	  {
	    eb[tp] += energy[j];
	    cb[tp] += cw[j] * energy[j];
	  }
	assert(tp<npart_l_orig);
      }
#else
    b = 0;
    for (j = 0; j < cw_upper_index;)
      {
	FLOAT8 ebb, cbb;
	int i;

	ebb = energy[j];
	cbb = energy[j] * cw[j];
	j++;

	for (i = numlines_l[b] - 1; i > 0; i--)
	  {
	    ebb += energy[j];
	    cbb += energy[j] * cw[j];
	    j++;
	  }
	eb[b] = ebb;
	cb[b] = cbb;
	b++;
      }

    for (; b < npart_l_orig; b++ )
      {
	int i;
	FLOAT8 ebb = energy[j++];

	for (i = numlines_l[b] - 1; i > 0; i--)
	  {
	    ebb += energy[j++];
	  }
	eb[b] = ebb;
	cb[b] = ebb * 0.4;
      }
#endif

    /**********************************************************************
     *      convolve the partitioned energy and unpredictability           *
     *      with the spreading function, s3_l[b][k]                        *
     ******************************************************************** */
    pe[chn] = 0;		/*  calculate percetual entropy */
    for ( b = 0;b < npart_l; b++ )
      {
	FLOAT8 tbb,ecb,ctb;
	FLOAT8 temp_1; /* BUG of IS */

	ecb = 0;
	ctb = 0;
	for ( k = s3ind[b][0]; k <= s3ind[b][1]; k++ )
	  {
	    ecb += s3_l[b][k] * eb[k];	/* sprdngf for Layer III */
	    ctb += s3_l[b][k] * cb[k];
	  }

	/* calculate the tonality of each threshold calculation partition */
	/* calculate the SNR in each threshhold calculation partition */

	tbb = ecb;
	if (tbb != 0)
	  {
	    tbb = ctb / tbb;
	    if (tbb <= 0.04875584301)
	      {
		tbb = exp(-LN_TO_LOG10 * (TMN - NMT));
	      }
	    else if (tbb > 0.4989003827)
	      {
		tbb = 1;
	      }
	    else
	      {
		tbb = log(tbb);
		tbb = exp(((TMN - NMT)*(LN_TO_LOG10*0.299))
			+ ((TMN - NMT)*(LN_TO_LOG10*0.43 ))*tbb);  /* conv1=-0.299, conv2=-0.43 */
	      }
	  }

	tbb = Min(minval[b], tbb);
	ecb *= tbb;

	/* pre-echo control */
	/* rpelev=2.0, rpelev2=16.0 */
	temp_1 = Min(ecb, Min(rpelev*nb_1[chn][b],rpelev2*nb_2[chn][b]) );
	thr[b] = Max( qthr_l[b], temp_1 ); 
	nb_2[chn][b] = nb_1[chn][b];
	nb_1[chn][b] = ecb;

	/* note: all surges in PE are because of the above pre-echo formula
	 * for temp_1.  it this is not used, PE is always around 600
	 */

	if (thr[b] < eb[b])
	  {
	    /* there's no non sound portition, because thr[b] is
	     maximum of qthr_l and temp_1 */
	    pe[chn] -= numlines_l[b] * log(thr[b] / eb[b]);
	  }
      }


#ifdef HAVEGTK
    if (gfp->gtkflag) {
      FLOAT mn,mx,ma=0,mb=0,mc=0;

      for ( j = HBLKSIZE_s/2; j < HBLKSIZE_s; j ++)
      {
        ma += energy_s[0][j];
        mb += energy_s[1][j];
        mc += energy_s[2][j];
      }
      mn = Min(ma,mb);
      mn = Min(mn,mc);
      mx = Max(ma,mb);
      mx = Max(mx,mc);

      pinfo->ers[gr_out][chn]=ers_save[chn];
      ers_save[chn]=mx/(1e-12+mn);
      pinfo->pe[gr_out][chn]=pe_save[chn];
      pe_save[chn]=pe[chn];
    }
#endif
    
    /*************************************************************** 
     * determine the block type (window type) based on L & R channels
     * 
     ***************************************************************/
    if (chn<2) {
      if (gfp->no_short_blocks){
	uselongblock[chn]=1;
      } else {
	/* tuned for t1.wav.  doesnt effect most other samples */
	if (pe[chn] > 3000) {
	  uselongblock[chn]=0;
	} else { 
	  FLOAT mn,mx,ma=0,mb=0,mc=0;
	
	  for ( j = HBLKSIZE_s/2; j < HBLKSIZE_s; j ++)
	  {
	      ma += energy_s[0][j];
	      mb += energy_s[1][j];
	      mc += energy_s[2][j];
	  }
	  mn = Min(ma,mb);
	  mn = Min(mn,mc);
	  mx = Max(ma,mb);
	  mx = Max(mx,mc);

	  uselongblock[chn] = 1;
	  
	  if ( mx > 30*mn ) 
	  {/* big surge of energy - always use short blocks */
	    uselongblock[chn] = 0;
	  } 
	  else if ((mx > 10*mn) && (pe[chn] > 1000))
	  {/* medium surge, medium pe - use short blocks */
	    uselongblock[chn] = 0;
	  }
	} 
      }
    }



    /*************************************************************** 
     * compute masking thresholds for both short and long blocks
     ***************************************************************/
    /* longblock threshold calculation (part 2) */
    for ( sb = 0; sb < SBPSY_l; sb++ )
      {
	FLOAT8 enn = w1_l[sb] * eb[bu_l[sb]] + w2_l[sb] * eb[bo_l[sb]];
	FLOAT8 thmm = w1_l[sb] *thr[bu_l[sb]] + w2_l[sb] * thr[bo_l[sb]];
	for ( b = bu_l[sb]+1; b < bo_l[sb]; b++ )
	  {
	    enn  += eb[b];
	    thmm += thr[b];
	  }
	en[chn].l[sb] = enn;
	thm[chn].l[sb] = thmm;
      }
    
    
    /* threshold calculation for short blocks */
    for ( sblock = 0; sblock < 3; sblock++ )
      {
	j = 0;
	for ( b = 0; b < npart_s_orig; b++ )
	  {
	    int i;
	    FLOAT ecb = energy_s[sblock][j++];
	    for (i = numlines_s[b]; i > 0; i--)
	      {
		ecb += energy_s[sblock][j++];
	      }
	    eb[b] = ecb;
	  }

	for ( b = 0; b < npart_s; b++ )
	  {
	    FLOAT8 ecb = 0;
	    for ( k = s3ind_s[b][0]; k <= s3ind_s[b][1]; k++ )
	      {
		ecb += s3_s[b][k] * eb[k];
	      }
	    thr[b] = Max (qthr_s[b], ecb);
	  }

	for ( sb = 0; sb < SBPSY_s; sb++ )
	  {
	    FLOAT8 enn  = w1_s[sb] * eb[bu_s[sb]] + w2_s[sb] * eb[bo_s[sb]];
	    FLOAT8 thmm = w1_s[sb] *thr[bu_s[sb]] + w2_s[sb] * thr[bo_s[sb]];
	    for ( b = bu_s[sb]+1; b < bo_s[sb]; b++ )
	      {
		enn  += eb[b];
		thmm += thr[b];
	      }
	    en[chn].s[sb][sblock] = enn;
	    thm[chn].s[sb][sblock] = thmm;
	  }
      }
  } /* end loop over chn */


  /* compute M/S thresholds from Johnston & Ferreira 1992 ICASSP paper */
  if ( numchn==4 /* mid/side and r/l */) {
    FLOAT8 rside,rmid,mld;
    int chmid=2,chside=3; 
    
    for ( sb = 0; sb < SBPSY_l; sb++ ) {
      /* use this fix if L & R masking differs by 2db or less */
      /* if db = 10*log10(x2/x1) < 2 */
      /* if (x2 < 1.58*x1) { */
      if (thm[0].l[sb] <= 1.58*thm[1].l[sb]
	  && thm[1].l[sb] <= 1.58*thm[0].l[sb]) {

	mld = mld_l[sb]*en[chside].l[sb];
	rmid = Max(thm[chmid].l[sb], Min(thm[chside].l[sb],mld));

	mld = mld_l[sb]*en[chmid].l[sb];
	rside = Max(thm[chside].l[sb],Min(thm[chmid].l[sb],mld));

	thm[chmid].l[sb]=rmid;
	thm[chside].l[sb]=rside;
      }
    }
    for ( sb = 0; sb < SBPSY_s; sb++ ) {
      for ( sblock = 0; sblock < 3; sblock++ ) {
	if (thm[0].s[sb][sblock] <= 1.58*thm[1].s[sb][sblock]
	    && thm[1].s[sb][sblock] <= 1.58*thm[0].s[sb][sblock]) {

	  mld = mld_s[sb]*en[chside].s[sb][sblock];
	  rmid = Max(thm[chmid].s[sb][sblock],Min(thm[chside].s[sb][sblock],mld));

	  mld = mld_s[sb]*en[chmid].s[sb][sblock];
	  rside = Max(thm[chside].s[sb][sblock],Min(thm[chmid].s[sb][sblock],mld));

	  thm[chmid].s[sb][sblock]=rmid;
	  thm[chside].s[sb][sblock]=rside;
	}
      }
    }
  }


  

  
  
  if (gfp->mode == MPG_MD_JOINT_STEREO)  {
    /* determin ms_ratio from masking thresholds*/
    /* use ms_stereo (ms_ratio < .35) if average thresh. diff < 5 db */
    FLOAT8 db,x1,x2,sidetot=0,tot=0;
    for (sb= SBPSY_l/4 ; sb< SBPSY_l; sb ++ ) {
      x1 = Min(thm[0].l[sb],thm[1].l[sb]);
      x2 = Max(thm[0].l[sb],thm[1].l[sb]);
      /* thresholds difference in db */
      if (x2 >= 1000*x1)  db=3;
      else db = log10(x2/x1);  
      /*  printf("db = %f %e %e  \n",db,thm[0].l[sb],thm[1].l[sb]);*/
      sidetot += db;
      tot++;
    }
    ms_ratio_l= (sidetot/tot)*0.7; /* was .35*(sidetot/tot)/5.0*10 */
    ms_ratio_l = Min(ms_ratio_l,0.5);
    
    sidetot=0; tot=0;
    for ( sblock = 0; sblock < 3; sblock++ )
      for ( sb = SBPSY_s/4; sb < SBPSY_s; sb++ ) {
	x1 = Min(thm[0].s[sb][sblock],thm[1].s[sb][sblock]);
	x2 = Max(thm[0].s[sb][sblock],thm[1].s[sb][sblock]);
	/* thresholds difference in db */
	if (x2 >= 1000*x1)  db=3;
	else db = log10(x2/x1);  
	sidetot += db;
	tot++;
      }
    ms_ratio_s = (sidetot/tot)*0.7; /* was .35*(sidetot/tot)/5.0*10 */
    ms_ratio_s = Min(ms_ratio_s,.5);
  }

  /*************************************************************** 
   * determin final block type
   ***************************************************************/

  for (chn=0; chn<gfp->stereo; chn++) {
    blocktype[chn] = NORM_TYPE;
  }


  if (gfp->stereo==2) {
    if (!gfp->allow_diff_short || gfp->mode==MPG_MD_JOINT_STEREO) {
      /* force both channels to use the same block type */
      /* this is necessary if the frame is to be encoded in ms_stereo.  */
      /* But even without ms_stereo, FhG  does this */
      int bothlong= (uselongblock[0] && uselongblock[1]);
      if (!bothlong) {
	uselongblock[0]=0;
	uselongblock[1]=0;
      }
    }
  }

  
  
  /* update the blocktype of the previous granule, since it depends on what
   * happend in this granule */
  for (chn=0; chn<gfp->stereo; chn++) {
    if ( uselongblock[chn])
      {				/* no attack : use long blocks */
	switch( blocktype_old[chn] ) 
	  {
	  case NORM_TYPE:
	  case STOP_TYPE:
	    blocktype[chn] = NORM_TYPE;
	    break;
	  case SHORT_TYPE:
	    blocktype[chn] = STOP_TYPE; 
	    break;
	  case START_TYPE:
	    fprintf( stderr, "Error in block selecting\n" );
	    abort();
	    break; /* problem */
	  }
      } else   {
	/* attack : use short blocks */
	blocktype[chn] = SHORT_TYPE;
	if ( blocktype_old[chn] == NORM_TYPE ) {
	  blocktype_old[chn] = START_TYPE;
	}
	if ( blocktype_old[chn] == STOP_TYPE ) {
	  blocktype_old[chn] = SHORT_TYPE ;
	}
      }
    
    blocktype_d[chn] = blocktype_old[chn];  /* value returned to calling program */
    blocktype_old[chn] = blocktype[chn];    /* save for next call to l3psy_anal */
  }
  
  if (blocktype_d[0]==2) 
    *ms_ratio = ms_ratio_s_old;
  else
    *ms_ratio = ms_ratio_l_old;

  ms_ratio_s_old = ms_ratio_s;
  ms_ratio_l_old = ms_ratio_l;

  /* we dont know the block type of this frame yet - assume long */
  *ms_ratio_next = ms_ratio_l;



  /*********************************************************************/
  /* compute side_energy / (side+mid)_energy */
  /* 0 = no energy in side channel */
  /* .5 = half of total energy in side channel */
  /*********************************************************************/
  if (numchn==4)  {
    FLOAT tmp = tot_ener[3]+tot_ener[2];
    *ms_ener_ratio = ms_ener_ratio_old;
    ms_ener_ratio_old=0;
    if (tmp>0) ms_ener_ratio_old=tot_ener[3]/tmp;
  } else
    /* we didn't compute ms_ener_ratios */
    *ms_ener_ratio = 0;
 
}






void L3para_read(FLOAT8 sfreq, int *numlines_l,int *numlines_s, int *partition_l, FLOAT8 *minval,
FLOAT8 *qthr_l, FLOAT8 s3_l[64][64], FLOAT8 s3_s[CBANDS + 1][CBANDS + 1],
FLOAT8 *qthr_s, FLOAT8 *SNR, 
int *bu_l, int *bo_l, FLOAT8 *w1_l, FLOAT8 *w2_l, 
int *bu_s, int *bo_s, FLOAT8 *w1_s, FLOAT8 *w2_s)
{
  FLOAT8 freq_tp;
  FLOAT8 bval_l[CBANDS], bval_s[CBANDS];
  int   cbmax=0, cbmax_tp;
  FLOAT8 *p = psy_data;

  int  sbmax ;
  int  i,j,k,k2,loop, part_max ;
  int freq_scale=1;


  /* use MPEG1 tables.  The MPEG2 tables in tables.c appear to be 
   * junk.  MPEG2 doc claims data for these tables is the same as the
   * MPEG1 data for 2x sampling frequency */
  /*  if (sfreq<32000) freq_scale=2; */
  


  /* Read long block data */

  for(loop=0;loop<6;loop++)
    {
      freq_tp = *p++;
      cbmax_tp = (int) *p++;
      cbmax_tp++;

      if (sfreq == freq_tp/freq_scale )
	{
	  cbmax = cbmax_tp;
	  for(i=0,k2=0;i<cbmax_tp;i++)
	    {
	      j = (int) *p++;
	      numlines_l[i] = (int) *p++;
	      minval[i] = exp(-((*p++) - NMT) * LN_TO_LOG10);
	      qthr_l[i] = *p++;
	      /* norm_l[i] = *p++*/ p++;
	      bval_l[i] = *p++;
	      if (j!=i)
		{
		  fprintf(stderr,"1. please check \"psy_data\"");
		  exit(-1);
		}
	      for(k=0;k<numlines_l[i];k++)
		partition_l[k2++] = i ;
	    }
	}
      else
	p += cbmax_tp * 6;
    }

#define NEWBARKXXX
#ifdef NEWBARK
  /* compute bark values of each critical band */
  j = 0;
  for(i=0;i<cbmax;i++)
    {
      FLOAT8 ji, freq, bark;

      ji = j + (numlines_l[i]-1)/2.0;
      freq = sfreq*ji/1024000.0;
      bark = 13*atan(.76*freq) + 3.5*atan(freq*freq/(7.5*7.5));

      printf("%i %i bval_l table=%f  f=%f  formaula=%f \n",i,j,bval_l[i],freq,bark);
      bval_l[i]=bark;
      j += numlines_l[i];
    }
#endif

  /************************************************************************
   * Now compute the spreading function, s[j][i], the value of the spread-*
   * ing function, centered at band j, for band i, store for later use    *
   ************************************************************************/
  /* i.e.: sum over j to spread into signal barkval=i  
     NOTE: i and j are used opposite as in the ISO docs */
  part_max = cbmax ;
  for(i=0;i<part_max;i++)
    {
      FLOAT8 tempx,x,tempy,temp;
      for(j=0;j<part_max;j++)
	{
	  /*tempx = (bval_l[i] - bval_l[j])*1.05;*/
	  if (j>=i) tempx = (bval_l[i] - bval_l[j])*3.0;
	  else    tempx = (bval_l[i] - bval_l[j])*1.5;

#ifdef AACS3	
          if (i>=j) tempx = (bval_l[i] - bval_l[j])*3.0;
	  else    tempx = (bval_l[i] - bval_l[j])*1.5; 
#endif

	  if(tempx>=0.5 && tempx<=2.5)
	    {
	      temp = tempx - 0.5;
	      x = 8.0 * (temp*temp - 2.0 * temp);
	    }
	  else x = 0.0;
	  tempx += 0.474;
	  tempy = 15.811389 + 7.5*tempx - 17.5*sqrt(1.0+tempx*tempx);

#ifdef NEWS3
	  if (j>=i) tempy = (bval_l[j] - bval_l[i])*(-15);
	  else    tempy = (bval_l[j] - bval_l[i])*25;
	  x=0; 
#endif
	  /*
	  if ((i==part_max/2)  && (fabs(bval_l[j] - bval_l[i])) < 3) {
	    printf("bark=%f   x+tempy = %f  \n",bval_l[j] - bval_l[i],x+tempy);
	  }
	  */

	  if (tempy <= -60.0) s3_l[i][j] = 0.0;
	  else                s3_l[i][j] = exp( (x + tempy)*LN_TO_LOG10 ); 
	}
    }

  /* Read short block data */
  for(loop=0;loop<6;loop++)
    {
      freq_tp = *p++;
      cbmax_tp = (int) *p++;
      cbmax_tp++;

      if (sfreq == freq_tp/freq_scale )
	{
	  cbmax = cbmax_tp;
	  for(i=0,k2=0;i<cbmax_tp;i++)
	    {
	      j = (int) *p++;
	      numlines_s[i] = (int) *p++;
	      qthr_s[i] = *p++;         
	      /* norm_s[i] =*p++ */ p++;         
	      SNR[i] = *p++;            
	      bval_s[i] = *p++;
	      if (j!=i)
		{
		  fprintf(stderr,"3. please check \"psy_data\"");
		  exit(-1);
		}
	      numlines_s[i]--;
	    }
	  numlines_s[i] = -1;
	}
      else
	p += cbmax_tp * 6;
    }


#ifdef NEWBARK
  /* compute bark values of each critical band */
  j = 0;
  for(i=0;i<cbmax;i++)
    {
      FLOAT8 ji, freq, bark;
      ji = (j * 2 + numlines_s[i]) / 2.0;
      freq = sfreq*ji/256000.0;
      bark = 13*atan(.76*freq) + 3.5*atan(freq*freq/(7.5*7.5));
      printf("%i %i bval_s = %f  %f  %f \n",i,j,bval_s[i],freq,bark);
      bval_s[i]=bark;
      j += numlines_s[i] + 1;
    }
#endif



  /************************************************************************
   * Now compute the spreading function, s[j][i], the value of the spread-*
   * ing function, centered at band j, for band i, store for later use    *
   ************************************************************************/
  part_max = cbmax ;
  for(i=0;i<part_max;i++)
    {
      FLOAT8 tempx,x,tempy,temp;
      for(j=0;j<part_max;j++)
	{
	  /* tempx = (bval_s[i] - bval_s[j])*1.05;*/
	  if (j>=i) tempx = (bval_s[i] - bval_s[j])*3.0;
	  else    tempx = (bval_s[i] - bval_s[j])*1.5;
#ifdef AACS3
          if (i>=j) tempx = (bval_s[i] - bval_s[j])*3.0;
	  else    tempx = (bval_s[i] - bval_s[j])*1.5; 
#endif
	  if(tempx>=0.5 && tempx<=2.5)
	    {
	      temp = tempx - 0.5;
	      x = 8.0 * (temp*temp - 2.0 * temp);
	    }
	  else x = 0.0;
	  tempx += 0.474;
	  tempy = 15.811389 + 7.5*tempx - 17.5*sqrt(1.0+tempx*tempx);
#ifdef NEWS3
	  if (j>=i) tempy = (bval_s[j] - bval_s[i])*(-15);
	  else    tempy = (bval_s[j] - bval_s[i])*25;
	  x=0; 
#endif
	  if (tempy <= -60.0) s3_s[i][j] = 0.0;
	  else                s3_s[i][j] = exp( (x + tempy)*LN_TO_LOG10 );
	}
    }
  /* Read long block data for converting threshold calculation 
     partitions to scale factor bands */

  for(loop=0;loop<6;loop++)
    {
      freq_tp = *p++;
      sbmax =  (int) *p++;
      sbmax++;

      if (sfreq == freq_tp/freq_scale)
	{
	  for(i=0;i<sbmax;i++)
	    {
	      j = (int) *p++;
	      p++;             
	      bu_l[i] = (int) *p++;
	      bo_l[i] = (int) *p++;
	      w1_l[i] = (FLOAT8) *p++;
	      w2_l[i] = (FLOAT8) *p++;
	      if (j!=i)
		{ fprintf(stderr,"30:please check \"psy_data\"\n");
		exit(-1);
		}

	      if (i!=0)
		if ( (fabs(1.0-w1_l[i]-w2_l[i-1]) > 0.01 ) )
		  {
		    fprintf(stderr,"31l: please check \"psy_data.\"\n");
                  fprintf(stderr,"w1,w2: %f %f \n",w1_l[i],w2_l[i-1]);
		    exit(-1);
		  }
	    }
	}
      else
	p += sbmax * 6;
    }

  /* Read short block data for converting threshold calculation 
     partitions to scale factor bands */

  for(loop=0;loop<6;loop++)
    {
      freq_tp = *p++;
      sbmax = (int) *p++;
      sbmax++;

      if (sfreq == freq_tp/freq_scale)
	{
	  for(i=0;i<sbmax;i++)
	    {
	      j = (int) *p++;
	      p++;
	      bu_s[i] = (int) *p++;
	      bo_s[i] = (int) *p++;
	      w1_s[i] = *p++;
	      w2_s[i] = *p++;
	      if (j!=i)
		{ fprintf(stderr,"30:please check \"psy_data\"\n");
		exit(-1);
		}

	      if (i!=0)
		if ( (fabs(1.0-w1_s[i]-w2_s[i-1]) > 0.01 ) )
		  { 
                  fprintf(stderr,"31s: please check \"psy_data.\"\n");
                  fprintf(stderr,"w1,w2: %f %f \n",w1_s[i],w2_s[i-1]);
		  exit(-1);
		  }
	    }
	}
      else
	p += sbmax * 6;
    }

}
