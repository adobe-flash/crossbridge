#include <assert.h>
#include "util.h"
#include "tables.h"
#include "reservoir.h"
#include "quantize-pvt.h"

FLOAT masking_lower=1;
int convert_mdct, reduce_sidechannel;
/*
mt 5/99.  These global flags denote 4 possibilities:
                                                                mode    l3_xmin
1   MDCT input L/R, quantize L/R,   psy-model thresholds: L/R   -m s     either
2   MDCT input L/R, quantize M/S,   psy-model thresholds: L/R   -m j     orig
3   MDCT input M/S, quantize M/S,   psy-model thresholds: M/S   -m f     either
4   MDCT input L/R, quantize M/S,   psy-model thresholds: M/S   -m j -h  m/s

1:  convert_mdct = 0, convert_psy=0,  reduce_sidechannel=0          
2:  convert_mdct = 1, convert_psy=1,  reduce_sidechannel=1
3:  convert_mdct = 0, convert_psy=0,  reduce_sidechannel=1   (this mode no longer used)
4:  convert_mdct = 1, convert_psy=0,  reduce_sidechannel=1

if (convert_mdct), then iteration_loop will quantize M/S data from
the L/R input MDCT coefficients.

if (convert_psy), then calc_noise will compute the noise for the L/R
channels from M/S MDCT data and L/R psy-model threshold information.
Distortion in ether L or R channel will be marked as distortion in
both Mid and Side channels.  
NOTE: 3/00: this mode has been removed.  

if (reduce_sidechannel) then outer_loop will allocate less bits
to the side channel and more bits to the mid channel based on relative 
energies.
*/



/*
  The following table is used to implement the scalefactor
  partitioning for MPEG2 as described in section
  2.4.3.2 of the IS. The indexing corresponds to the
  way the tables are presented in the IS:

  [table_number][row_in_table][column of nr_of_sfb]
*/
unsigned nr_of_sfb_block[6][3][4] =
{
  {
    {6, 5, 5, 5},
    {9, 9, 9, 9},
    {6, 9, 9, 9}
  },
  {
    {6, 5, 7, 3},
    {9, 9, 12, 6},
    {6, 9, 12, 6}
  },
  {
    {11, 10, 0, 0},
    {18, 18, 0, 0},
    {15,18,0,0}
  },
  {
    {7, 7, 7, 0},
    {12, 12, 12, 0},
    {6, 15, 12, 0}
  },
  {
    {6, 6, 6, 3},
    {12, 9, 9, 6},
    {6, 12, 9, 6}
  },
  {
    {8, 8, 5, 0},
    {15,12,9,0},
    {6,18,9,0}
  }
};


/* Table B.6: layer3 preemphasis */
int  pretab[21] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 2, 2, 3, 3, 3, 2
};

/*
  Here are MPEG1 Table B.8 and MPEG2 Table B.1
  -- Layer III scalefactor bands. 
  Index into this using a method such as:
    idx  = fr_ps->header->sampling_frequency
           + (fr_ps->header->version * 3)
*/

struct scalefac_struct sfBandIndex[6] =
{
  { /* Table B.2.b: 22.05 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    {0,4,8,12,18,24,32,42,56,74,100,132,174,192}
  },
  { /* Table B.2.c: 24 kHz */                 /* docs: 332. mpg123: 330 */
    {0,6,12,18,24,30,36,44,54,66,80,96,114,136,162,194,232,278, 332, 394,464,540,576},
    {0,4,8,12,18,26,36,48,62,80,104,136,180,192}
  },
  { /* Table B.2.a: 16 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    {0,4,8,12,18,26,36,48,62,80,104,134,174,192}
  },
  { /* Table B.8.b: 44.1 kHz */
    {0,4,8,12,16,20,24,30,36,44,52,62,74,90,110,134,162,196,238,288,342,418,576},
    {0,4,8,12,16,22,30,40,52,66,84,106,136,192}
  },
  { /* Table B.8.c: 48 kHz */
    {0,4,8,12,16,20,24,30,36,42,50,60,72,88,106,128,156,190,230,276,330,384,576},
    {0,4,8,12,16,22,28,38,50,64,80,100,126,192}
  },
  { /* Table B.8.a: 32 kHz */
    {0,4,8,12,16,20,24,30,36,44,54,66,82,102,126,156,194,240,296,364,448,550,576},
    {0,4,8,12,16,22,30,42,58,78,104,138,180,192}
  }
};

struct scalefac_struct scalefac_band;


FLOAT8 pow20[Q_MAX];
FLOAT8 ipow20[Q_MAX];
FLOAT8 pow43[PRECALC_SIZE];
static FLOAT8 adj43[PRECALC_SIZE];
static FLOAT8 adj43asm[PRECALC_SIZE];
static FLOAT8 ATH_l[SBPSY_l];
static FLOAT8 ATH_s[SBPSY_l];

FLOAT8 ATH_mdct_long[576];
FLOAT8 ATH_mdct_short[192];


/************************************************************************/
/*  initialization for iteration_loop */
/************************************************************************/
void
iteration_init( lame_global_flags *gfp,III_side_info_t *l3_side, int l3_enc[2][2][576])
{
  gr_info *cod_info;
  int ch, gr, i;

  l3_side->resvDrain = 0;

  if ( gfp->frameNum==0 ) {
    for (i = 0; i < SBMAX_l + 1; i++) {
      scalefac_band.l[i] =
	sfBandIndex[gfp->samplerate_index + (gfp->version * 3)].l[i];
    }
    for (i = 0; i < SBMAX_s + 1; i++) {
      scalefac_band.s[i] =
	sfBandIndex[gfp->samplerate_index + (gfp->version * 3)].s[i];
    }

    l3_side->main_data_begin = 0;
    compute_ath(gfp,ATH_l,ATH_s);

    for(i=0;i<PRECALC_SIZE;i++)
        pow43[i] = pow((FLOAT8)i, 4.0/3.0);

    for (i = 0; i < PRECALC_SIZE-1; i++)
	adj43[i] = (i + 1) - pow(0.5 * (pow43[i] + pow43[i + 1]), 0.75);
    adj43[i] = 0.5;


    adj43asm[0] = 0.0;
    for (i = 1; i < PRECALC_SIZE; i++)
      adj43asm[i] = i - 0.5 - pow(0.5 * (pow43[i - 1] + pow43[i]),0.75);

    for (i = 0; i < Q_MAX; i++) {
	ipow20[i] = pow(2.0, (double)(i - 210) * -0.1875);
	pow20[i] = pow(2.0, (double)(i - 210) * 0.25);
    }
  }


  convert_mdct=0;
  reduce_sidechannel=0;
  if (gfp->mode_ext==MPG_MD_MS_LR) {
    convert_mdct = 1;
    reduce_sidechannel=1;
  }
  
  /* some intializations. */
  for ( gr = 0; gr < gfp->mode_gr; gr++ ){
    for ( ch = 0; ch < gfp->stereo; ch++ ){
      cod_info = (gr_info *) &(l3_side->gr[gr].ch[ch]);

      if (cod_info->block_type == SHORT_TYPE)
        {
	  cod_info->sfb_lmax = 0; /* No sb*/
	  cod_info->sfb_smax = 0;
        }
      else
	{
	  /* MPEG 1 doesnt use last scalefactor band */
	  cod_info->sfb_lmax = SBPSY_l;
	  cod_info->sfb_smax = SBPSY_s;    /* No sb */
	}

    }
  }


  /* dont bother with scfsi. */
  for ( ch = 0; ch < gfp->stereo; ch++ )
    for ( i = 0; i < 4; i++ )
      l3_side->scfsi[ch][i] = 0;
}





/* 
compute the ATH for each scalefactor band 
cd range:  0..96db

Input:  3.3kHz signal  32767 amplitude  (3.3kHz is where ATH is smallest = -5db)
longblocks:  sfb=12   en0/bw=-11db    max_en0 = 1.3db
shortblocks: sfb=5           -9db              0db

Input:  1 1 1 1 1 1 1 -1 -1 -1 -1 -1 -1 -1 (repeated)
longblocks:  amp=1      sfb=12   en0/bw=-103 db      max_en0 = -92db
            amp=32767   sfb=12           -12 db                 -1.4db 

Input:  1 1 1 1 1 1 1 -1 -1 -1 -1 -1 -1 -1 (repeated)
shortblocks: amp=1      sfb=5   en0/bw= -99                    -86 
            amp=32767   sfb=5           -9  db                  4db 


MAX energy of largest wave at 3.3kHz = 1db
AVE energy of largest wave at 3.3kHz = -11db
Let's take AVE:  -11db = maximum signal in sfb=12.  
Dynamic range of CD: 96db.  Therefor energy of smallest audible wave 
in sfb=12  = -11  - 96 = -107db = ATH at 3.3kHz.  

ATH formula for this wave: -5db.  To adjust to LAME scaling, we need
ATH = ATH_formula  - 103  (db)
ATH = ATH * 2.5e-10      (ener)

*/
FLOAT8 ATHformula(lame_global_flags *gfp,FLOAT8 f)
{
  FLOAT8 ath;
  f  = Max(0.02, f);
  /* from Painter & Spanias, 1997 */
  /* minimum: (i=77) 3.3kHz = -5db */
  ath=(3.640 * pow(f,-0.8)
       -  6.500 * exp(-0.6*pow(f-3.3,2.0))
       +  0.001 * pow(f,4.0));
  /* convert to energy */
  if (gfp->noATH)
    ath -= 200; /* disables ATH */
  else {
    ath -= 114;    /* MDCT scaling.  From tests by macik and MUS420 code */
    /* ath -= 109; */
  }
#ifdef RH_QUALITY_CONTROL 
  /* purpose of RH_QUALITY_CONTROL:
   * at higher quality lower ATH masking abilities   => needs more bits
   * at lower quality increase ATH masking abilities => needs less bits
   * works together with adjusted masking lowering of GPSYCHO thresholds
   * (Robert.Hegemann@gmx.de 2000-01-30)
   */
  ath -= (4-gfp->VBR_q)*4.0; 
#endif
  ath = pow( 10.0, ath/10.0 );
  return ath;
}
 

void compute_ath(lame_global_flags *gfp,FLOAT8 ATH_l[SBPSY_l],FLOAT8 ATH_s[SBPSY_l])
{
  int sfb,i,start,end;
  FLOAT8 ATH_f;
  FLOAT8 samp_freq = gfp->out_samplerate/1000.0;
#ifdef RH_ATH
  /* going from average to peak level ATH masking
   */
  FLOAT8 adjust_mdct_scaling = 10.0; 
#endif
  

  /* last sfb is not used */
  for ( sfb = 0; sfb < SBPSY_l; sfb++ ) {
    start = scalefac_band.l[ sfb ];
    end   = scalefac_band.l[ sfb+1 ];
    ATH_l[sfb]=1e99;
    for (i=start ; i < end; i++) {
      ATH_f = ATHformula(gfp,samp_freq*i/(2*576)); /* freq in kHz */
      ATH_l[sfb]=Min(ATH_l[sfb],ATH_f);
#ifdef RH_ATH
      ATH_mdct_long[i] = ATH_f*adjust_mdct_scaling;
#endif
    }
    /*
    printf("sfb=%i %f  ATH=%f %f  %f   \n",sfb,samp_freq*start/(2*576),
10*log10(ATH_l[sfb]),
10*log10( ATHformula(samp_freq*start/(2*576)))  ,
10*log10(ATHformula(samp_freq*end/(2*576))));
    */
  }

  for ( sfb = 0; sfb < SBPSY_s; sfb++ ){
    start = scalefac_band.s[ sfb ];
    end   = scalefac_band.s[ sfb+1 ];
    ATH_s[sfb]=1e99;
    for (i=start ; i < end; i++) {
      ATH_f = ATHformula(gfp,samp_freq*i/(2*192));     /* freq in kHz */
      ATH_s[sfb]=Min(ATH_s[sfb],ATH_f);
#ifdef RH_ATH
      ATH_mdct_short[i] = ATH_f*adjust_mdct_scaling;
#endif
    }
  }
}





/* convert from L/R <-> Mid/Side */
void ms_convert(FLOAT8 xr[2][576],FLOAT8 xr_org[2][576])
{
  int i;
  for ( i = 0; i < 576; i++ ) {
    FLOAT8 l = xr_org[0][i];
    FLOAT8 r = xr_org[1][i];
    xr[0][i] = (l+r)*(SQRT2*0.5);
    xr[1][i] = (l-r)*(SQRT2*0.5);
  }
}



/************************************************************************
 * allocate bits among 2 channels based on PE
 * mt 6/99
 ************************************************************************/
void on_pe(lame_global_flags *gfp,FLOAT8 pe[2][2],III_side_info_t *l3_side,
int targ_bits[2],int mean_bits, int gr)
{
  gr_info *cod_info;
  int extra_bits,tbits,bits;
  int add_bits[2]; 
  int ch;

  /* allocate targ_bits for granule */
  ResvMaxBits( mean_bits, &tbits, &extra_bits, gr);
    

  for (ch=0 ; ch < gfp->stereo ; ch ++) {
    /******************************************************************
     * allocate bits for each channel 
     ******************************************************************/
    cod_info = &l3_side->gr[gr].ch[ch].tt;
    
    targ_bits[ch]=tbits/gfp->stereo;
    
    /* allocate extra bits from reservoir based on PE */
    bits=0;
    
    /* extra bits based on PE > 700 */
    add_bits[ch]=(pe[gr][ch]-750)/1.55;  /* 1.4; */
    
    /* short blocks need extra, no matter what the pe */
    if (cod_info->block_type==SHORT_TYPE) 
      if (add_bits[ch]<500) add_bits[ch]=500;
    
    if (add_bits[ch] < 0) add_bits[ch]=0;
    bits += add_bits[ch];
    
    if (bits > extra_bits) add_bits[ch] = (extra_bits*add_bits[ch])/bits;
    if ((targ_bits[ch]+add_bits[ch]) > 4095) 
      add_bits[ch]=4095-targ_bits[ch];

    targ_bits[ch] = targ_bits[ch] + add_bits[ch];
    extra_bits -= add_bits[ch];
  }
}

void reduce_side(int targ_bits[2],FLOAT8 ms_ener_ratio,int mean_bits)
{
int ch;
int numchn=2;
    /*  ms_ener_ratio = 0:  allocate 66/33  mid/side  fac=.33  
     *  ms_ener_ratio =.5:  allocate 50/50 mid/side   fac= 0 */
    /* 75/25 split is fac=.5 */
    /* float fac = .50*(.5-ms_ener_ratio[gr])/.5;*/
    float fac = .33*(.5-ms_ener_ratio)/.5;
    if (fac<0) fac=0;
    
    if (targ_bits[1] >= 125) {
      /* dont reduce side channel below 125 bits */
      if (targ_bits[1]-targ_bits[1]*fac > 125) {
	targ_bits[0] += targ_bits[1]*fac;
	targ_bits[1] -= targ_bits[1]*fac;
      } else {
	targ_bits[0] += targ_bits[1] - 125;
	targ_bits[1] = 125;
      }
    }
    
    /* dont allow to many bits per channel */  
    for (ch=0; ch<numchn; ch++) {
      int max_bits = Min(4095,mean_bits/2 + 1200);
      if (targ_bits[ch] > max_bits) {
	targ_bits[ch] = max_bits;
      }
    }

}

/*************************************************************************** 
 *         inner_loop                                                      * 
 *************************************************************************** 
 * The code selects the best global gain for a particular set of scalefacs */
 
int
inner_loop( lame_global_flags *gfp,FLOAT8 xrpow[576],
	    int l3_enc[576], int max_bits,
	    gr_info *cod_info)
{
    int bits;
    assert( max_bits >= 0 );
    cod_info->global_gain--;
    do
    {
      cod_info->global_gain++;
      bits = count_bits(gfp,l3_enc, xrpow, cod_info);
    }
    while ( bits > max_bits );
    return bits;
}



/*************************************************************************/
/*            scale_bitcount                                             */
/*************************************************************************/

/* Also calculates the number of bits necessary to code the scalefactors. */

int scale_bitcount( III_scalefac_t *scalefac, gr_info *cod_info)
{
    int i, k, sfb, max_slen1 = 0, max_slen2 = 0, /*a, b, */ ep = 2;

    static int slen1[16] = { 1, 1, 1, 1, 8, 2, 2, 2, 4, 4, 4, 8, 8, 8,16,16 };
    static int slen2[16] = { 1, 2, 4, 8, 1, 2, 4, 8, 2, 4, 8, 2, 4, 8, 4, 8 };

    static int slen1_tab[16] = {0,
	18, 36, 54, 54, 36, 54, 72, 54, 72, 90, 72, 90,108,108,126
    };
    static int slen2_tab[16] = {0,
	10, 20, 30, 33, 21, 31, 41, 32, 42, 52, 43, 53, 63, 64, 74
    };
    int *tab;


    if ( cod_info->block_type == SHORT_TYPE )
    {
            tab = slen1_tab;
            /* a = 18; b = 18;  */
            for ( i = 0; i < 3; i++ )
            {
                for ( sfb = 0; sfb < 6; sfb++ )
                    if (scalefac->s[sfb][i] > max_slen1 )
                        max_slen1 = scalefac->s[sfb][i];
                for (sfb = 6; sfb < SBPSY_s; sfb++ )
                    if ( scalefac->s[sfb][i] > max_slen2 )
                        max_slen2 = scalefac->s[sfb][i];
            }
    }
    else
    { /* block_type == 1,2,or 3 */
        tab = slen2_tab;
        /* a = 11; b = 10;   */
        for ( sfb = 0; sfb < 11; sfb++ )
            if ( scalefac->l[sfb] > max_slen1 )
                max_slen1 = scalefac->l[sfb];

	if (!cod_info->preflag) {
	    for ( sfb = 11; sfb < SBPSY_l; sfb++ )
		if (scalefac->l[sfb] < pretab[sfb])
		    break;

	    if (sfb == SBPSY_l) {
		cod_info->preflag = 1;
		for ( sfb = 11; sfb < SBPSY_l; sfb++ )
		    scalefac->l[sfb] -= pretab[sfb];
	    }
	}

        for ( sfb = 11; sfb < SBPSY_l; sfb++ )
            if ( scalefac->l[sfb] > max_slen2 )
                max_slen2 = scalefac->l[sfb];
    }



    /* from Takehiro TOMINAGA <tominaga@isoternet.org> 10/99
     * loop over *all* posible values of scalefac_compress to find the
     * one which uses the smallest number of bits.  ISO would stop
     * at first valid index */
    cod_info->part2_length = LARGE_BITS;
    for ( k = 0; k < 16; k++ )
    {
        if ( (max_slen1 < slen1[k]) && (max_slen2 < slen2[k]) &&
             ((int)cod_info->part2_length > tab[k])) {
	  cod_info->part2_length=tab[k];
	  cod_info->scalefac_compress=k;
	  ep=0;  /* we found a suitable scalefac_compress */
	}
    }
    return ep;
}



/*
  table of largest scalefactors (number of bits) for MPEG2
*/
/*
static unsigned max_sfac_tab[6][4] =
{
    {4, 4, 3, 3},
    {4, 4, 3, 0},
    {3, 2, 0, 0},
    {4, 5, 5, 0},
    {3, 3, 3, 0},
    {2, 2, 0, 0}
};
*/
/*
  table of largest scalefactor values for MPEG2
*/
static unsigned max_range_sfac_tab[6][4] =
{
 { 15, 15, 7,  7},
 { 15, 15, 7,  0},
 { 7,  3,  0,  0},
 { 15, 31, 31, 0},
 { 7,  7,  7,  0},
 { 3,  3,  0,  0}
};





/*************************************************************************/
/*            scale_bitcount_lsf                                         */
/*************************************************************************/

/* Also counts the number of bits to encode the scalefacs but for MPEG 2 */ 
/* Lower sampling frequencies  (24, 22.05 and 16 kHz.)                   */
 
/*  This is reverse-engineered from section 2.4.3.2 of the MPEG2 IS,     */
/* "Audio Decoding Layer III"                                            */

int scale_bitcount_lsf(III_scalefac_t *scalefac, gr_info *cod_info)
{
    int table_number, row_in_table, partition, nr_sfb, window, over;
    int i, sfb, max_sfac[ 4 ];
    unsigned *partition_table;

    /*
      Set partition table. Note that should try to use table one,
      but do not yet...
    */
    if ( cod_info->preflag )
	table_number = 2;
    else
	table_number = 0;

    for ( i = 0; i < 4; i++ )
	max_sfac[i] = 0;

    if ( cod_info->block_type == SHORT_TYPE )
    {
	    row_in_table = 1;
	    partition_table = &nr_of_sfb_block[table_number][row_in_table][0];
	    for ( sfb = 0, partition = 0; partition < 4; partition++ )
	    {
		nr_sfb = partition_table[ partition ] / 3;
		for ( i = 0; i < nr_sfb; i++, sfb++ )
		    for ( window = 0; window < 3; window++ )
			if ( scalefac->s[sfb][window] > max_sfac[partition] )
			    max_sfac[partition] = scalefac->s[sfb][window];
	    }
    }
    else
    {
	row_in_table = 0;
	partition_table = &nr_of_sfb_block[table_number][row_in_table][0];
	for ( sfb = 0, partition = 0; partition < 4; partition++ )
	{
	    nr_sfb = partition_table[ partition ];
	    for ( i = 0; i < nr_sfb; i++, sfb++ )
		if ( scalefac->l[sfb] > max_sfac[partition] )
		    max_sfac[partition] = scalefac->l[sfb];
	}
    }

    for ( over = 0, partition = 0; partition < 4; partition++ )
    {
	if ( max_sfac[partition] > (int)max_range_sfac_tab[table_number][partition] )
	    over++;
    }
    if ( !over )
    {
	/*
	  Since no bands have been over-amplified, we can set scalefac_compress
	  and slen[] for the formatter
	*/
	static int log2tab[] = { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

	unsigned slen1, slen2, slen3, slen4;

        cod_info->sfb_partition_table = &nr_of_sfb_block[table_number][row_in_table][0];
	for ( partition = 0; partition < 4; partition++ )
	    cod_info->slen[partition] = log2tab[max_sfac[partition]];

	/* set scalefac_compress */
	slen1 = cod_info->slen[ 0 ];
	slen2 = cod_info->slen[ 1 ];
	slen3 = cod_info->slen[ 2 ];
	slen4 = cod_info->slen[ 3 ];

	switch ( table_number )
	{
	  case 0:
	    cod_info->scalefac_compress = (((slen1 * 5) + slen2) << 4)
		+ (slen3 << 2)
		+ slen4;
	    break;

	  case 1:
	    cod_info->scalefac_compress = 400
		+ (((slen1 * 5) + slen2) << 2)
		+ slen3;
	    break;

	  case 2:
	    cod_info->scalefac_compress = 500 + (slen1 * 3) + slen2;
	    break;

	  default:
	    fprintf( stderr, "intensity stereo not implemented yet\n" );
	    exit( EXIT_FAILURE );
	    break;
	}
    }
#ifdef DEBUG
    if ( over ) 
        printf( "---WARNING !! Amplification of some bands over limits\n" );
#endif
    if (!over) {
      assert( cod_info->sfb_partition_table );     
      cod_info->part2_length=0;
      for ( partition = 0; partition < 4; partition++ )
	cod_info->part2_length += cod_info->slen[partition] * cod_info->sfb_partition_table[partition];
    }
    return over;
}





/*************************************************************************/
/*            calc_xmin                                                  */
/*************************************************************************/

/*
  Calculate the allowed distortion for each scalefactor band,
  as determined by the psychoacoustic model.
  xmin(sb) = ratio(sb) * en(sb) / bw(sb)

  returns number of sfb's with energy > ATH
*/
int calc_xmin( lame_global_flags *gfp,FLOAT8 xr[576], III_psy_ratio *ratio,
	       gr_info *cod_info, III_psy_xmin *l3_xmin)
{
    int start, end, bw,l, b, ath_over=0;
	u_int	sfb;
    FLOAT8 en0, xmin, ener;

    if (gfp->ATHonly) {    
      for ( sfb = cod_info->sfb_smax; sfb < SBPSY_s; sfb++ )
	  for ( b = 0; b < 3; b++ )
	      l3_xmin->s[sfb][b]=ATH_s[sfb];
      for ( sfb = 0; sfb < cod_info->sfb_lmax; sfb++ )
	  l3_xmin->l[sfb]=ATH_l[sfb];

    }else{

      for ( sfb = cod_info->sfb_smax; sfb < SBPSY_s; sfb++ ) {
	start = scalefac_band.s[ sfb ];
        end   = scalefac_band.s[ sfb + 1 ];
	bw = end - start;
        for ( b = 0; b < 3; b++ ) {
	  for (en0 = 0.0, l = start; l < end; l++) {
	    ener = xr[l * 3 + b];
	    ener = ener * ener;
	    en0 += ener;
	  }
	  en0 /= bw;

	  xmin = ratio->en.s[sfb][b];
	  if (xmin > 0.0)
	    xmin = en0 * ratio->thm.s[sfb][b] * masking_lower / xmin;

#ifdef RH_ATH
          /* do not mix up ATH masking with GPSYCHO thresholds
	   */
	  l3_xmin->s[sfb][b] = Max(1e-20, xmin);
#else
	  l3_xmin->s[sfb][b] = Max(ATH_s[sfb], xmin);
#endif
	  if (en0 > ATH_s[sfb]) ath_over++;
	}
      }

      for ( sfb = 0; sfb < cod_info->sfb_lmax; sfb++ ){
	start = scalefac_band.l[ sfb ];
	end   = scalefac_band.l[ sfb+1 ];
	bw = end - start;

        for (en0 = 0.0, l = start; l < end; l++ ) {
	  ener = xr[l] * xr[l];
	  en0 += ener;
	}
	en0 /= bw;

	xmin = ratio->en.l[sfb];
	if (xmin > 0.0)
	  xmin = en0 * ratio->thm.l[sfb] * masking_lower / xmin;


#ifdef RH_ATH
        /* do not mix up ATH masking with GPSYCHO thresholds
	 */
	l3_xmin->l[sfb]=Max(1e-20, xmin);
#else
	l3_xmin->l[sfb]=Max(ATH_l[sfb], xmin);
#endif
	if (en0 > ATH_l[sfb]) ath_over++;
      }
    }
    return ath_over;
}



/*************************************************************************/
/*            loop_break                                                 */
/*************************************************************************/

/*  Function: Returns zero if there is a scalefac which has not been
    amplified. Otherwise it returns one. 
*/

int loop_break( III_scalefac_t *scalefac, gr_info *cod_info)
{
    int i;
	u_int sfb;

    for ( sfb = 0; sfb < cod_info->sfb_lmax; sfb++ )
        if ( scalefac->l[sfb] == 0 )
	    return 0;

    for ( sfb = cod_info->sfb_smax; sfb < SBPSY_s; sfb++ )
      for ( i = 0; i < 3; i++ ) 
            if ( scalefac->s[sfb][i] == 0 )
		return 0;

    return 1;
}













/*
 ----------------------------------------------------------------------
  if someone wants to try to find a faster step search function,
  here is some code which gives a lower bound for the step size:
  
  for (max_xrspow = 0, i = 0; i < 576; ++i)
  {
    max_xrspow = Max(max_xrspow, xrspow[i]);
  }
  lowerbound = 210+log10(max_xrspow/IXMAX_VAL)/(0.1875*LOG2);
 
 
                                                 Robert.Hegemann@gmx.de
 ----------------------------------------------------------------------
*/


typedef enum {
    BINSEARCH_NONE,
    BINSEARCH_UP, 
    BINSEARCH_DOWN
} binsearchDirection_t;

/*-------------------------------------------------------------------------*/
int 
bin_search_StepSize2 (lame_global_flags *gfp,int desired_rate, int start, int *ix, 
                      FLOAT8 xrspow[576], gr_info *cod_info)
/*-------------------------------------------------------------------------*/
{
    static int CurrentStep = 4;
    int nBits;
    int flag_GoneOver = 0;
    int StepSize = start;
    binsearchDirection_t Direction = BINSEARCH_NONE;

    do
    {
	cod_info->global_gain = StepSize;
	nBits = count_bits(gfp,ix, xrspow, cod_info);  

	if (CurrentStep == 1 )
        {
	    break; /* nothing to adjust anymore */
	}
	if (flag_GoneOver)
	{
	    CurrentStep /= 2;
	}
	if (nBits > desired_rate)  /* increase Quantize_StepSize */
	{
	    if (Direction == BINSEARCH_DOWN && !flag_GoneOver)
	    {
		flag_GoneOver = 1;
		CurrentStep /= 2; /* late adjust */
	    }
	    Direction = BINSEARCH_UP;
	    StepSize += CurrentStep;
	    if (StepSize > 255) break;
	}
	else if (nBits < desired_rate)
	{
	    if (Direction == BINSEARCH_UP && !flag_GoneOver)
	    {
		flag_GoneOver = 1;
		CurrentStep /= 2; /* late adjust */
	    }
	    Direction = BINSEARCH_DOWN;
	    StepSize -= CurrentStep;
	    if (StepSize < 0) break;
	}
	else break; /* nBits == desired_rate;; most unlikely to happen.*/
    } while (1); /* For-ever, break is adjusted. */

    CurrentStep = abs(start - StepSize);
    
    if (CurrentStep >= 4) {
	CurrentStep = 4;
    } else {
	CurrentStep = 2;
    }

    return nBits;
}








#if 0
#if (defined(__GNUC__) && defined(__i386__))
#define USE_GNUC_ASM
#endif
#ifdef _MSC_VER
#define USE_MSC_ASM
#endif
#endif



/*********************************************************************
 * XRPOW_FTOI is a macro to convert floats to ints.  
 * if XRPOW_FTOI(x) = nearest_int(x), then QUANTFAC(x)=adj43asm[x]
 *                                         ROUNDFAC= -0.0946
 *
 * if XRPOW_FTOI(x) = floor(x), then QUANTFAC(x)=asj43[x]   
 *                                   ROUNDFAC=0.4054
 *********************************************************************/
#ifdef USE_GNUC_ASM
#  define QUANTFAC(rx)  adj43asm[rx]
#  define ROUNDFAC -0.0946
#  define XRPOW_FTOI(src, dest) \
     asm ("fistpl %0 " : "=m"(dest) : "t"(src) : "st")
#elif defined (USE_MSC_ASM)
#  define QUANTFAC(rx)  adj43asm[rx]
#  define ROUNDFAC -0.0946
#  define XRPOW_FTOI(src, dest) do { \
     FLOAT8 src_ = (src); \
     int dest_; \
     { \
       __asm fld src_ \
       __asm fistp dest_ \
     } \
     (dest) = dest_; \
   } while (0)
#else
#  define QUANTFAC(rx)  adj43[rx]
#  define ROUNDFAC 0.4054
#  define XRPOW_FTOI(src,dest) ((dest) = (int)(src))
#endif

#ifdef USE_MSC_ASM
/* define F8type and F8size according to type of FLOAT8 */
# if defined FLOAT8_is_double
#  define F8type qword
#  define F8size 8
# elif defined FLOAT8_is_float
#  define F8type dword
#  define F8size 4
# else
/* only float and double supported */
#  error invalid FLOAT8 type for USE_MSC_ASM
# endif
#endif

#ifdef USE_GNUC_ASM
/* define F8type and F8size according to type of FLOAT8 */
# if defined FLOAT8_is_double
#  define F8type "l"
#  define F8size "8"
# elif defined FLOAT8_is_float
#  define F8type "s"
#  define F8size "4"
# else
/* only float and double supported */
#  error invalid FLOAT8 type for USE_GNUC_ASM
# endif
#endif

/*********************************************************************
 * nonlinear quantization of xr 
 * More accurate formula than the ISO formula.  Takes into account
 * the fact that we are quantizing xr -> ix, but we want ix^4/3 to be 
 * as close as possible to x^4/3.  (taking the nearest int would mean
 * ix is as close as possible to xr, which is different.)
 * From Segher Boessenkool <segher@eastsite.nl>  11/1999
 * ASM optimization from 
 *    Mathew Hendry <scampi@dial.pipex.com> 11/1999
 *    Acy Stapp <AStapp@austin.rr.com> 11/1999
 *    Takehiro Tominaga <tominaga@isoternet.org> 11/1999
 *********************************************************************/

void quantize_xrpow(FLOAT8 xr[576], int ix[576], gr_info *cod_info) {
  /* quantize on xr^(3/4) instead of xr */
  const FLOAT8 istep = IPOW20(cod_info->global_gain);

#if defined (USE_GNUC_ASM) 
  {
      int rx[4];
      __asm__ __volatile__(
        "\n\nloop1:\n\t"

        "fld" F8type " 0*" F8size "(%1)\n\t"
        "fld" F8type " 1*" F8size "(%1)\n\t"
        "fld" F8type " 2*" F8size "(%1)\n\t"
        "fld" F8type " 3*" F8size "(%1)\n\t"

        "fxch %%st(3)\n\t"
        "fmul %%st(4)\n\t"
        "fxch %%st(2)\n\t"
        "fmul %%st(4)\n\t"
        "fxch %%st(1)\n\t"
        "fmul %%st(4)\n\t"
        "fxch %%st(3)\n\t"
        "fmul %%st(4)\n\t"

        "addl $4*" F8size ", %1\n\t"
        "addl $16, %3\n\t"

        "fxch %%st(2)\n\t"
        "fistl %5\n\t"
        "fxch %%st(1)\n\t"
        "fistl 4+%5\n\t"
        "fxch %%st(3)\n\t"
        "fistl 8+%5\n\t"
        "fxch %%st(2)\n\t"
        "fistl 12+%5\n\t"

        "dec %4\n\t"

        "movl %5, %%eax\n\t"
        "movl 4+%5, %%ebx\n\t"
        "fxch %%st(1)\n\t"
        "fadd" F8type " (%2,%%eax," F8size ")\n\t"
        "fxch %%st(3)\n\t"
        "fadd" F8type " (%2,%%ebx," F8size ")\n\t"

        "movl 8+%5, %%eax\n\t"
        "movl 12+%5, %%ebx\n\t"
        "fxch %%st(2)\n\t"
        "fadd" F8type " (%2,%%eax," F8size ")\n\t"
        "fxch %%st(1)\n\t"
        "fadd" F8type " (%2,%%ebx," F8size ")\n\t"

        "fxch %%st(3)\n\t"
        "fistpl -16(%3)\n\t"
        "fxch %%st(1)\n\t"
        "fistpl -12(%3)\n\t"
        "fistpl -8(%3)\n\t"
        "fistpl -4(%3)\n\t"

        "jnz loop1\n\n"
        : /* no outputs */
        : "t" (istep), "r" (xr), "r" (adj43asm), "r" (ix), "r" (576 / 4), "m" (rx)
        : "%eax", "%ebx", "memory", "cc"
      );
  }
#elif defined (USE_MSC_ASM)
  {
      /* asm from Acy Stapp <AStapp@austin.rr.com> */
      int rx[4];
      _asm {
          fld F8type ptr [istep]
          mov esi, dword ptr [xr]
          lea edi, dword ptr [adj43asm]
          mov edx, dword ptr [ix]
          mov ecx, 576/4
      } loop1: _asm {
          fld F8type ptr [esi+(0*F8size)] // 0
          fld F8type ptr [esi+(1*F8size)] // 1 0
          fld F8type ptr [esi+(2*F8size)] // 2 1 0
          fld F8type ptr [esi+(3*F8size)] // 3 2 1 0
          fxch st(3)                  // 0 2 1 3
          fmul st(0), st(4)
          fxch st(2)                  // 1 2 0 3
          fmul st(0), st(4)
          fxch st(1)                  // 2 1 0 3
          fmul st(0), st(4)
          fxch st(3)                  // 3 1 0 2
          fmul st(0), st(4)

          add esi, 4*F8size
          add edx, 16

          fxch st(2)                  // 0 1 3 2
          fist dword ptr [rx]
          fxch st(1)                  // 1 0 3 2
          fist dword ptr [rx+4]
          fxch st(3)                  // 2 0 3 1
          fist dword ptr [rx+8]
          fxch st(2)                  // 3 0 2 1
          fist dword ptr [rx+12]

          dec ecx

          mov eax, dword ptr [rx]
          mov ebx, dword ptr [rx+4]
          fxch st(1)                  // 0 3 2 1
          fadd F8type ptr [edi+eax*F8size]
          fxch st(3)                  // 1 3 2 0
          fadd F8type ptr [edi+ebx*F8size]

          mov eax, dword ptr [rx+8]
          mov ebx, dword ptr [rx+12]
          fxch st(2)                  // 2 3 1 0
          fadd F8type ptr [edi+eax*F8size]
          fxch st(1)                  // 3 2 1 0
          fadd F8type ptr [edi+ebx*F8size]
          fxch st(3)                  // 0 2 1 3
          fistp dword ptr [edx-16]    // 2 1 3
          fxch st(1)                  // 1 2 3
          fistp dword ptr [edx-12]    // 2 3
          fistp dword ptr [edx-8]     // 3
          fistp dword ptr [edx-4]

          jnz loop1

          mov dword ptr [xr], esi
          mov dword ptr [ix], edx
          fstp st(0)
      }
  }
#else
#if 0
  {   /* generic code if you write ASM for XRPOW_FTOI() */
      FLOAT8 x;
      int j, rx;
      for (j = 576 / 4; j > 0; --j) {
          x = *xr++ * istep;
          XRPOW_FTOI(x, rx);
          XRPOW_FTOI(x + QUANTFAC(rx), *ix++);

          x = *xr++ * istep;
          XRPOW_FTOI(x, rx);
          XRPOW_FTOI(x + QUANTFAC(rx), *ix++);

          x = *xr++ * istep;
          XRPOW_FTOI(x, rx);
          XRPOW_FTOI(x + QUANTFAC(rx), *ix++);

          x = *xr++ * istep;
          XRPOW_FTOI(x, rx);
          XRPOW_FTOI(x + QUANTFAC(rx), *ix++);
      }
  }
#endif
  {/* from Wilfried.Behne@t-online.de.  Reported to be 2x faster than 
      the above code (when not using ASM) on PowerPC */
     	int j;
     	
     	for ( j = 576/8; j > 0; --j)
     	{
			FLOAT8	x1, x2, x3, x4, x5, x6, x7, x8;
			int		rx1, rx2, rx3, rx4, rx5, rx6, rx7, rx8;
			x1 = *xr++ * istep;
			x2 = *xr++ * istep;
			XRPOW_FTOI(x1, rx1);
			x3 = *xr++ * istep;
			XRPOW_FTOI(x2, rx2);
			x4 = *xr++ * istep;
			XRPOW_FTOI(x3, rx3);
			x5 = *xr++ * istep;
			XRPOW_FTOI(x4, rx4);
			x6 = *xr++ * istep;
			XRPOW_FTOI(x5, rx5);
			x7 = *xr++ * istep;
			XRPOW_FTOI(x6, rx6);
			x8 = *xr++ * istep;
			XRPOW_FTOI(x7, rx7);
			x1 += QUANTFAC(rx1);
			XRPOW_FTOI(x8, rx8);
			x2 += QUANTFAC(rx2);
			XRPOW_FTOI(x1,*ix++);
			x3 += QUANTFAC(rx3);
			XRPOW_FTOI(x2,*ix++);
			x4 += QUANTFAC(rx4);		
			XRPOW_FTOI(x3,*ix++);
			x5 += QUANTFAC(rx5);
			XRPOW_FTOI(x4,*ix++);
			x6 += QUANTFAC(rx6);
			XRPOW_FTOI(x5,*ix++);
			x7 += QUANTFAC(rx7);
			XRPOW_FTOI(x6,*ix++);
			x8 += QUANTFAC(rx8);		
			XRPOW_FTOI(x7,*ix++);
			XRPOW_FTOI(x8,*ix++);
     	}
	}
#endif
}






void quantize_xrpow_ISO( FLOAT8 xr[576], int ix[576], gr_info *cod_info )
{
  /* quantize on xr^(3/4) instead of xr */
  const FLOAT8 istep = IPOW20(cod_info->global_gain);
  
#if defined(USE_GNUC_ASM)
   {
      __asm__ __volatile__ (
        "\n\nloop0:\n\t"

        "fld" F8type " 0*" F8size "(%3)\n\t"
        "fld" F8type " 1*" F8size "(%3)\n\t"
        "fld" F8type " 2*" F8size "(%3)\n\t"
        "fld" F8type " 3*" F8size "(%3)\n\t"

        "addl $4*" F8size ", %3\n\t"
        "addl $16, %4\n\t"

        "fxch %%st(3)\n\t"
        "fmul %%st(4)\n\t"
        "fxch %%st(2)\n\t"
        "fmul %%st(4)\n\t"
        "fxch %%st(1)\n\t"
        "fmul %%st(4)\n\t"
        "fxch %%st(3)\n\t"
        "fmul %%st(4)\n\t"

        "dec %0\n\t"

        "fxch %%st(2)\n\t"
        "fadd %%st(5)\n\t"
        "fxch %%st(1)\n\t"
        "fadd %%st(5)\n\t"
        "fxch %%st(3)\n\t"
        "fadd %%st(5)\n\t"
        "fxch %%st(2)\n\t"
        "fadd %%st(5)\n\t"

        "fxch %%st(1)\n\t"
        "fistpl -16(%4)\n\t"
        "fxch %%st(2)\n\t"
        "fistpl -12(%4)\n\t"
        "fistpl -8(%4)\n\t"
        "fistpl -4(%4)\n\t"

        "jnz loop0\n\n"

        : /* no outputs */
        : "r" (576 / 4), "u" ((FLOAT8)(0.4054 - 0.5)), "t" (istep), "r" (xr), "r" (ix)
        : "memory", "cc"
      );
  }
#elif defined(USE_MSC_ASM)
  {
      /* asm from Acy Stapp <AStapp@austin.rr.com> */
      const FLOAT8 temp0 = 0.4054 - 0.5;
      _asm {
          mov ecx, 576/4;
          fld F8type ptr [temp0];
          fld F8type ptr [istep];
          mov eax, dword ptr [xr];
          mov edx, dword ptr [ix];
      } loop0: _asm {
          fld F8type ptr [eax+0*F8size]; // 0
          fld F8type ptr [eax+1*F8size]; // 1 0
          fld F8type ptr [eax+2*F8size]; // 2 1 0
          fld F8type ptr [eax+3*F8size]; // 3 2 1 0

          add eax, 4*F8size;
          add edx, 16;

          fxch st(3); // 0 2 1 3
          fmul st(0), st(4);
          fxch st(2); // 1 2 0 3
          fmul st(0), st(4);
          fxch st(1); // 2 1 0 3
          fmul st(0), st(4);
          fxch st(3); // 3 1 0 2
          fmul st(0), st(4);

          dec ecx;

          fxch st(2); // 0 1 3 2
          fadd st(0), st(5);
          fxch st(1); // 1 0 3 2
          fadd st(0), st(5);
          fxch st(3); // 2 0 3 1
          fadd st(0), st(5);
          fxch st(2); // 3 0 2 1
          fadd st(0), st(5);

          fxch st(1); // 0 3 2 1 
          fistp dword ptr [edx-16]; // 3 2 1
          fxch st(2); // 1 2 3
          fistp dword ptr [edx-12];
          fistp dword ptr [edx-8];
          fistp dword ptr [edx-4];

          jnz loop0;

          mov dword ptr [xr], eax;
          mov dword ptr [ix], edx;
          fstp st(0);
          fstp st(0);
      }
  }
#else
#if 0
   /* generic ASM */
      register int j;
      for (j=576/4;j>0;j--) {
         XRPOW_FTOI(istep * (*xr++) + ROUNDFAC, *ix++);
         XRPOW_FTOI(istep * (*xr++) + ROUNDFAC, *ix++);
         XRPOW_FTOI(istep * (*xr++) + ROUNDFAC, *ix++);
         XRPOW_FTOI(istep * (*xr++) + ROUNDFAC, *ix++);
      }
#endif
  {
      register int j;
      const FLOAT8 compareval0 = (1.0 - 0.4054)/istep;
      /* depending on architecture, it may be worth calculating a few more compareval's.
         eg.  compareval1 = (2.0 - 0.4054/istep); 
              .. and then after the first compare do this ...
              if compareval1>*xr then ix = 1;
         On a pentium166, it's only worth doing the one compare (as done here), as the second
         compare becomes more expensive than just calculating the value. Architectures with 
         slow FP operations may want to add some more comparevals. try it and send your diffs 
         statistically speaking
         73% of all xr*istep values give ix=0
         16% will give 1
         4%  will give 2
      */
      for (j=576;j>0;j--) 
        {
          if (compareval0 > *xr) {
            *(ix++) = 0;
            xr++;
          } else
	    /*    *(ix++) = (int)( istep*(*(xr++))  + 0.4054); */
            XRPOW_FTOI(  istep*(*(xr++))  + ROUNDFAC , *(ix++) );
        }
  }
#endif
}
