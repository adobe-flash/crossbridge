/*
 *	LAME MP3 encoding engine
 *
 *	Copyright (c) 1999 Mark Taylor
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


#include <assert.h>

#ifdef HAVEGTK
#include "gtkanal.h"
#include <gtk/gtk.h>
#endif
#include "lame.h"
#include "util.h"
#include "timestatus.h"
#include "psymodel.h"
#include "newmdct.h"
#include "quantize.h"
#include "quantize-pvt.h"
#include "l3bitstream.h"
#include "formatBitstream.h"
#include "version.h"
#include "VbrTag.h"
#include "id3tag.h"
#include "tables.h"
#include "brhist.h"
#include "get_audio.h"

#ifdef __riscos__
#include "asmstuff.h"
#endif


/* Global variable definitions for lame.c */
static Bit_stream_struc   bs;
static III_side_info_t l3_side;
#define MFSIZE (1152+1152+ENCDELAY-MDCTDELAY)
static short int mfbuf[2][MFSIZE];
static int mf_size;
static int mf_samples_to_encode;


static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}


/********************************************************************
 *   initialize internal params based on data in gf
 *   (globalflags struct filled in by calling program)
 *
 ********************************************************************/
void lame_init_params(lame_global_flags *gfp)
{
  int i;
  FLOAT compression_ratio;


  memset(&bs, 0, sizeof(Bit_stream_struc));
  memset(&l3_side,0x00,sizeof(III_side_info_t));


  gfp->frameNum=0;
  InitFormatBitStream();
  if (gfp->num_channels==1) {
    gfp->mode = MPG_MD_MONO;
  }
  gfp->stereo=2;
  if (gfp->mode == MPG_MD_MONO) gfp->stereo=1;

#ifdef BRHIST
  if (gfp->silent) {
    disp_brhist=0;  /* turn of VBR historgram */
  }
  if (!gfp->VBR) {
    disp_brhist=0;  /* turn of VBR historgram */
  }
#endif

  /* set the output sampling rate, and resample options if necessary
     samplerate = input sample rate
     resamplerate = ouput sample rate
  */
  if (gfp->out_samplerate==0) {
    /* user did not specify output sample rate */
    gfp->out_samplerate=gfp->in_samplerate;   /* default */


    /* if resamplerate is not valid, find a valid value */
    if (gfp->out_samplerate>=48000) gfp->out_samplerate=48000;
    else if (gfp->out_samplerate>=44100) gfp->out_samplerate=44100;
    else if (gfp->out_samplerate>=32000) gfp->out_samplerate=32000;
    else if (gfp->out_samplerate>=24000) gfp->out_samplerate=24000;
    else if (gfp->out_samplerate>=22050) gfp->out_samplerate=22050;
    else gfp->out_samplerate=16000;


    if (gfp->brate>0) {
      /* check if user specified bitrate requires downsampling */
      compression_ratio = gfp->out_samplerate*16*gfp->stereo/(1000.0*gfp->brate);
      if (!gfp->VBR && compression_ratio > 13 ) {
	/* automatic downsample, if possible */
	gfp->out_samplerate = (10*1000.0*gfp->brate)/(16*gfp->stereo);
	if (gfp->out_samplerate<=16000) gfp->out_samplerate=16000;
	else if (gfp->out_samplerate<=22050) gfp->out_samplerate=22050;
	else if (gfp->out_samplerate<=24000) gfp->out_samplerate=24000;
	else if (gfp->out_samplerate<=32000) gfp->out_samplerate=32000;
	else if (gfp->out_samplerate<=44100) gfp->out_samplerate=44100;
	else gfp->out_samplerate=48000;
      }
    }
  }

  gfp->mode_gr = (gfp->out_samplerate <= 24000) ? 1 : 2;  /* mode_gr = 2 */
  gfp->encoder_delay = ENCDELAY;
  gfp->framesize = gfp->mode_gr*576;

  if (gfp->brate==0) { /* user didn't specify a bitrate, use default */
    gfp->brate=128;
    if (gfp->mode_gr==1) gfp->brate=64;
  }


  gfp->resample_ratio=1;
  if (gfp->out_samplerate != gfp->in_samplerate) gfp->resample_ratio = (FLOAT)gfp->in_samplerate/(FLOAT)gfp->out_samplerate;

  /* estimate total frames.  must be done after setting sampling rate so
   * we know the framesize.  */
  gfp->totalframes=0;
  gfp->totalframes = 2+ gfp->num_samples/(gfp->resample_ratio*gfp->framesize);



  /* 44.1kHz at 56kbs/channel: compression factor of 12.6
     44.1kHz at 64kbs/channel: compression factor of 11.025
     44.1kHz at 80kbs/channel: compression factor of 8.82
     22.05kHz at 24kbs:  14.7
     22.05kHz at 32kbs:  11.025
     22.05kHz at 40kbs:  8.82
     16kHz at 16kbs:  16.0
     16kHz at 24kbs:  10.7

     compression_ratio
        11                                .70?
        12                   sox resample .66
        14.7                 sox resample .45

  */
  if (gfp->brate >= 320) gfp->VBR=0;  /* dont bother with VBR at 320kbs */
  compression_ratio = gfp->out_samplerate*16*gfp->stereo/(1000.0*gfp->brate);


  /* for VBR, take a guess at the compression_ratio */
  /* VBR_q           compression       like
     0                4.4             320kbs
     1                5.4             256kbs
     3                7.4             192kbs
     4                8.8             160kbs
     6                10.4            128kbs
  */
  if (gfp->VBR && compression_ratio>11) {
    compression_ratio = 4.4 + gfp->VBR_q;
  }


  /* At higher quality (lower compression) use STEREO instead of JSTEREO.
   * (unless the user explicitly specified a mode ) */
  if ( (!gfp->mode_fixed) && (gfp->mode !=MPG_MD_MONO)) {
    if (compression_ratio < 9 ) {
      gfp->mode = MPG_MD_STEREO;
    }
  }



  /****************************************************************/
  /* if a filter has not been enabled, see if we should add one: */
  /****************************************************************/
  if (gfp->lowpassfreq == 0) {
    /* If the user has not selected their own filter, add a lowpass
     * filter based on the compression ratio.  Formula based on
          44.1   /160    4.4x
          44.1   /128    5.5x      keep all bands
          44.1   /96kbs  7.3x      keep band 28
          44.1   /80kbs  8.8x      keep band 25
          44.1khz/64kbs  11x       keep band 21  22?

	  16khz/24kbs  10.7x       keep band 21
	  22kHz/32kbs  11x         keep band ?
	  22kHz/24kbs  14.7x       keep band 16
          16    16     16x         keep band 14
    */


    /* Should we use some lowpass filters? */
    int band = 1+floor(.5 + 14-18*log(compression_ratio/16.0));
    if (band < 31) {
      gfp->lowpass1 = band/31.0;
      gfp->lowpass2 = band/31.0;
    }
  }

  /****************************************************************/
  /* apply user driven filters*/
  /****************************************************************/
  if ( gfp->highpassfreq > 0 ) {
    gfp->highpass1 = 2.0*gfp->highpassfreq/gfp->out_samplerate; /* will always be >=0 */
    if ( gfp->highpasswidth >= 0 ) {
      gfp->highpass2 = 2.0*(gfp->highpassfreq+gfp->highpasswidth)/gfp->out_samplerate;
    } else {
      /* 15% above on default */
      /* gfp->highpass2 = 1.15*2.0*gfp->highpassfreq/gfp->out_samplerate;  */
      gfp->highpass2 = 1.00*2.0*gfp->highpassfreq/gfp->out_samplerate; 
    }
    gfp->highpass1 = Min( 1, gfp->highpass1 );
    gfp->highpass2 = Min( 1, gfp->highpass2 );
  }

  if ( gfp->lowpassfreq > 0 ) {
    gfp->lowpass2 = 2.0*gfp->lowpassfreq/gfp->out_samplerate; /* will always be >=0 */
    if ( gfp->lowpasswidth >= 0 ) {
      gfp->lowpass1 = 2.0*(gfp->lowpassfreq-gfp->lowpasswidth)/gfp->out_samplerate;
      if ( gfp->lowpass1 < 0 ) { /* has to be >= 0 */
	gfp->lowpass1 = 0;
      }
    } else {
      /* 15% below on default */
      /* gfp->lowpass1 = 0.85*2.0*gfp->lowpassfreq/gfp->out_samplerate;  */
      gfp->lowpass1 = 1.00*2.0*gfp->lowpassfreq/gfp->out_samplerate;
    }
    gfp->lowpass1 = Min( 1, gfp->lowpass1 );
    gfp->lowpass2 = Min( 1, gfp->lowpass2 );
  }


  /***************************************************************/
  /* compute info needed for polyphase filter                    */
  /***************************************************************/
  if (gfp->filter_type==0) {
    int band,maxband,minband;
    FLOAT8 amp,freq;
    if (gfp->lowpass1 > 0) {
      minband=999;
      maxband=-1;
      for (band=0;  band <=31 ; ++band) { 
	freq = band/31.0;
	amp = 1;
	/* this band and above will be zeroed: */
	if (freq >= gfp->lowpass2) {
	  gfp->lowpass_band= Min(gfp->lowpass_band,band);
	  amp=0;
	}
	if (gfp->lowpass1 < freq && freq < gfp->lowpass2) {
          minband = Min(minband,band);
          maxband = Max(maxband,band);
	  amp = cos((PI/2)*(gfp->lowpass1-freq)/(gfp->lowpass2-gfp->lowpass1));
	}
	/* printf("lowpass band=%i  amp=%f \n",band,amp);*/
      }
      /* compute the *actual* transition band implemented by the polyphase filter */
      if (minband==999) gfp->lowpass1 = (gfp->lowpass_band-.75)/31.0;
      else gfp->lowpass1 = (minband-.75)/31.0;
      gfp->lowpass2 = gfp->lowpass_band/31.0;
    }

    /* make sure highpass filter is within 90% of whan the effective highpass
     * frequency will be */
    if (gfp->highpass2 > 0) 
      if (gfp->highpass2 <  .9*(.75/31.0) ) {
	gfp->highpass1=0; gfp->highpass2=0;
	fprintf(stderr,"Warning: highpass filter disabled.  highpass frequency to small\n");
      }
    

    if (gfp->highpass2 > 0) {
      minband=999;
      maxband=-1;
      for (band=0;  band <=31; ++band) { 
	freq = band/31.0;
	amp = 1;
	/* this band and below will be zereod */
	if (freq <= gfp->highpass1) {
	  gfp->highpass_band = Max(gfp->highpass_band,band);
	  amp=0;
	}
	if (gfp->highpass1 < freq && freq < gfp->highpass2) {
          minband = Min(minband,band);
          maxband = Max(maxband,band);
	  amp = cos((PI/2)*(gfp->highpass2-freq)/(gfp->highpass2-gfp->highpass1));
	}
	/*	printf("highpass band=%i  amp=%f \n",band,amp);*/
      }
      /* compute the *actual* transition band implemented by the polyphase filter */
      gfp->highpass1 = gfp->highpass_band/31.0;
      if (maxband==-1) gfp->highpass2 = (gfp->highpass_band+.75)/31.0;
      else gfp->highpass2 = (maxband+.75)/31.0;
    }
    /*
    printf("lowpass band with amp=0:  %i \n",gfp->lowpass_band);
    printf("highpass band with amp=0:  %i \n",gfp->highpass_band);
    */
  }



  /***************************************************************/
  /* compute info needed for FIR filter */
  /***************************************************************/
  if (gfp->filter_type==1) {
  }




  gfp->mode_ext=MPG_MD_LR_LR;
  gfp->stereo = (gfp->mode == MPG_MD_MONO) ? 1 : 2;


  gfp->samplerate_index = SmpFrqIndex((long)gfp->out_samplerate, &gfp->version);
  if( gfp->samplerate_index < 0) {
    display_bitrates(stderr);
    exit(1);
  }
  if( (gfp->bitrate_index = BitrateIndex(gfp->brate, gfp->version,gfp->out_samplerate)) < 0) {
    display_bitrates(stderr);
    exit(1);
  }


  /* choose a min/max bitrate for VBR */
  if (gfp->VBR) {
    /* if the user didn't specify VBR_max_bitrate: */
    if (0==gfp->VBR_max_bitrate_kbps) {
      /* default max bitrate is 256kbs */
      /* we do not normally allow 320bps frams with VBR, unless: */
      gfp->VBR_max_bitrate=13;   /* default: allow 256kbs */
      if (gfp->VBR_min_bitrate_kbps>=256) gfp->VBR_max_bitrate=14;
      if (gfp->VBR_q == 0) gfp->VBR_max_bitrate=14;   /* allow 320kbs */
      if (gfp->VBR_q >= 4) gfp->VBR_max_bitrate=12;   /* max = 224kbs */
      if (gfp->VBR_q >= 8) gfp->VBR_max_bitrate=9;    /* low quality, max = 128kbs */
    }else{
      if( (gfp->VBR_max_bitrate  = BitrateIndex(gfp->VBR_max_bitrate_kbps, gfp->version,gfp->out_samplerate)) < 0) {
	display_bitrates(stderr);
	exit(1);
      }
    }
    if (0==gfp->VBR_min_bitrate_kbps) {
      gfp->VBR_min_bitrate=1;  /* 32 kbps */
    }else{
      if( (gfp->VBR_min_bitrate  = BitrateIndex(gfp->VBR_min_bitrate_kbps, gfp->version,gfp->out_samplerate)) < 0) {
	display_bitrates(stderr);
	exit(1);
      }
    }

  }


  if (gfp->VBR) gfp->quality=Min(gfp->quality,2);    /* always use quality <=2  with VBR */
  /* dont allow forced mid/side stereo for mono output */
  if (gfp->mode == MPG_MD_MONO) gfp->force_ms=0;


  /* Do not write VBR tag if VBR flag is not specified */
  if (gfp->VBR==0) gfp->bWriteVbrTag=0;

  /* some file options not allowed if output is: not specified or stdout */

  if (gfp->outPath!=NULL && gfp->outPath[0]=='-' ) {
    gfp->bWriteVbrTag=0; /* turn off VBR tag */
  }

  if (gfp->outPath==NULL || gfp->outPath[0]=='-' ) {
    id3tag.used=0;         /* turn of id3 tagging */
  }



  if (gfp->gtkflag) {
    gfp->bWriteVbrTag=0;  /* disable Xing VBR tag */
  }

  init_bit_stream_w(&bs);



  /* set internal feature flags.  USER should not access these since
   * some combinations will produce strange results */

  /* no psymodel, no noise shaping */
  if (gfp->quality==9) {
    gfp->filter_type=0;
    gfp->psymodel=0;
    gfp->quantization=0;
    gfp->noise_shaping=0;
    gfp->noise_shaping_stop=0;
    gfp->use_best_huffman=0;
  }

  if (gfp->quality==8) gfp->quality=7;

  /* use psymodel (for short block and m/s switching), but no noise shapping */
  if (gfp->quality==7) {
    gfp->filter_type=0;
    gfp->psymodel=1;
    gfp->quantization=0;
    gfp->noise_shaping=0;
    gfp->noise_shaping_stop=0;
    gfp->use_best_huffman=0;
  }

  if (gfp->quality==6) gfp->quality=5;

  if (gfp->quality==5) {
    /* the default */
    gfp->filter_type=0;
    gfp->psymodel=1;
    gfp->quantization=0;
    gfp->noise_shaping=1;
    gfp->noise_shaping_stop=0;
    gfp->use_best_huffman=0;
  }

  if (gfp->quality==4) gfp->quality=2;
  if (gfp->quality==3) gfp->quality=2;

  if (gfp->quality==2) {
    gfp->filter_type=0;
    gfp->psymodel=1;
    gfp->quantization=1;
    gfp->noise_shaping=1;
    gfp->noise_shaping_stop=0;
    gfp->use_best_huffman=1;
  }

  if (gfp->quality==1) {
    gfp->filter_type=0;
    gfp->psymodel=1;
    gfp->quantization=1;
    gfp->noise_shaping=1;
    gfp->noise_shaping_stop=1;
    gfp->use_best_huffman=1;
  }

  if (gfp->quality==0) {
    /* 0..1 quality */
    gfp->filter_type=1;         /* not yet coded */
    gfp->psymodel=1;
    gfp->quantization=1;
    gfp->noise_shaping=3;       /* not yet coded */
    gfp->noise_shaping_stop=2;  /* not yet coded */
    gfp->use_best_huffman=2;   /* not yet coded */
    exit(-99);
  }


  for (i = 0; i < SBMAX_l + 1; i++) {
    scalefac_band.l[i] =
      sfBandIndex[gfp->samplerate_index + (gfp->version * 3)].l[i];
  }
  for (i = 0; i < SBMAX_s + 1; i++) {
    scalefac_band.s[i] =
      sfBandIndex[gfp->samplerate_index + (gfp->version * 3)].s[i];
  }



  if (gfp->bWriteVbrTag)
    {
      /* Write initial VBR Header to bitstream */
      InitVbrTag(&bs,1-gfp->version,gfp->mode,gfp->samplerate_index);
    }

#ifdef HAVEGTK
  gtkflag=gfp->gtkflag;
#endif

#ifdef BRHIST
  if (gfp->VBR) {
    if (disp_brhist)
      brhist_init(gfp,1, 14);
  } else
    disp_brhist = 0;
#endif
  return;
}









/************************************************************************
 *
 * print_config
 *
 * PURPOSE:  Prints the encoding parameters used
 *
 ************************************************************************/
void lame_print_config(lame_global_flags *gfp)
{
  static const char *mode_names[4] = { "stereo", "j-stereo", "dual-ch", "single-ch" };
  FLOAT out_samplerate=gfp->out_samplerate/1000.0;
  FLOAT in_samplerate = gfp->resample_ratio*out_samplerate;
  FLOAT compression=
    (FLOAT)(gfp->stereo*16*out_samplerate)/(FLOAT)(gfp->brate);

  lame_print_version(stderr);
  if (gfp->num_channels==2 && gfp->stereo==1) {
    fprintf(stderr, "Autoconverting from stereo to mono. Setting encoding to mono mode.\n");
  }
  if (gfp->resample_ratio!=1) {
    fprintf(stderr,"Resampling:  input=%ikHz  output=%ikHz\n",
	    (int)in_samplerate,(int)out_samplerate);
  }
  if (gfp->highpass2>0.0)
    fprintf(stderr, "Using polyphase highpass filter, transition band: %.0f Hz -  %.0f Hz\n",
	    gfp->highpass1*out_samplerate*500,
	    gfp->highpass2*out_samplerate*500);
  if (gfp->lowpass1>0.0)
    fprintf(stderr, "Using polyphase lowpass filter,  transition band:  %.0f Hz - %.0f Hz\n",
	    gfp->lowpass1*out_samplerate*500,
	    gfp->lowpass2*out_samplerate*500);

  if (gfp->gtkflag) {
    fprintf(stderr, "Analyzing %s \n",gfp->inPath);
  }
  else {
    fprintf(stderr, "Encoding %s to %s\n",
	    (strcmp(gfp->inPath, "-")? mybasename(gfp->inPath) : "stdin"),
	    (strcmp(gfp->outPath, "-")? mybasename(gfp->outPath) : "stdout"));
    if (gfp->VBR)
      fprintf(stderr, "Encoding as %.1fkHz VBR(q=%i) %s MPEG%i LayerIII  qval=%i\n",
	      gfp->out_samplerate/1000.0,
	      gfp->VBR_q,mode_names[gfp->mode],2-gfp->version,gfp->quality);
    else
      fprintf(stderr, "Encoding as %.1f kHz %d kbps %s MPEG%i LayerIII (%4.1fx)  qval=%i\n",
	      gfp->out_samplerate/1000.0,gfp->brate,
	      mode_names[gfp->mode],2-gfp->version,compression,gfp->quality);
  }
  fflush(stderr);
}












/************************************************************************
*
* encodeframe()           Layer 3
*
* encode a single frame
*
************************************************************************
lame_encode_frame()


                       gr 0            gr 1
inbuf:           |--------------|---------------|-------------|
MDCT output:  |--------------|---------------|-------------|

FFT's                    <---------1024---------->
                                         <---------1024-------->



    inbuf = buffer of PCM data size=MP3 framesize
    encoder acts on inbuf[ch][0], but output is delayed by MDCTDELAY
    so the MDCT coefficints are from inbuf[ch][-MDCTDELAY]

    psy-model FFT has a 1 granule day, so we feed it data for the next granule.
    FFT is centered over granule:  224+576+224
    So FFT starts at:   576-224-MDCTDELAY

    MPEG2:  FFT ends at:  BLKSIZE+576-224-MDCTDELAY
    MPEG1:  FFT ends at:  BLKSIZE+2*576-224-MDCTDELAY    (1904)

    FFT starts at 576-224-MDCTDELAY (304)  = 576-FFTOFFSET

*/
int lame_encode_frame(lame_global_flags *gfp,
short int inbuf_l[],short int inbuf_r[],
int mf_size,char *mp3buf, int mp3buf_size)
{
  static unsigned long frameBits;
  static unsigned long bitsPerSlot;
  static FLOAT8 frac_SpF;
  static FLOAT8 slot_lag;
  static unsigned long sentBits = 0;
  FLOAT8 xr[2][2][576];
  int l3_enc[2][2][576];
  int mp3count;
  III_psy_ratio masking_ratio[2][2];    /*LR ratios */
  III_psy_ratio masking_MS_ratio[2][2]; /*MS ratios */
  III_psy_ratio (*masking)[2][2];  /*LR ratios and MS ratios*/
  III_scalefac_t scalefac[2][2];
  short int *inbuf[2];

  typedef FLOAT8 pedata[2][2];
  pedata pe,pe_MS;
  pedata *pe_use;

  int ch,gr,mean_bits;
  int bitsPerFrame;

  int check_ms_stereo;
  static FLOAT8 ms_ratio[2]={0,0};
  FLOAT8 ms_ratio_next=0;
  FLOAT8 ms_ratio_prev=0;
  static FLOAT8 ms_ener_ratio[2]={0,0};

  memset((char *) masking_ratio, 0, sizeof(masking_ratio));
  memset((char *) masking_MS_ratio, 0, sizeof(masking_MS_ratio));
  memset((char *) scalefac, 0, sizeof(scalefac));
  inbuf[0]=inbuf_l;
  inbuf[1]=inbuf_r;

  gfp->mode_ext = MPG_MD_LR_LR;

  if (gfp->frameNum==0 )  {
    /* Figure average number of 'slots' per frame. */
    FLOAT8 avg_slots_per_frame;
    FLOAT8 sampfreq =   gfp->out_samplerate/1000.0;
    int bit_rate = gfp->brate;
    sentBits = 0;
    bitsPerSlot = 8;
    avg_slots_per_frame = (bit_rate*gfp->framesize) /
           (sampfreq* bitsPerSlot);
    /* -f fast-math option causes some strange rounding here, be carefull: */
    frac_SpF  = avg_slots_per_frame - floor(avg_slots_per_frame + 1e-9);
    if (fabs(frac_SpF) < 1e-9) frac_SpF = 0;

    slot_lag  = -frac_SpF;
    gfp->padding = 1;
    if (frac_SpF==0) gfp->padding = 0;
    /* check FFT will not use a negative starting offset */
    assert(576>=FFTOFFSET);
    /* check if we have enough data for FFT */
    assert(mf_size>=(BLKSIZE+gfp->framesize-FFTOFFSET));
  }


  /********************** padding *****************************/
  switch (gfp->padding_type) {
  case 0:
    gfp->padding=0;
    break;
  case 1:
    gfp->padding=1;
    break;
  case 2:
  default:
    if (gfp->VBR) {
      gfp->padding=0;
    } else {
      if (gfp->disable_reservoir) {
	gfp->padding = 0;
	/* if the user specified --nores, dont very gfp->padding either */
	/* tiny changes in frac_SpF rounding will cause file differences */
      }else{
	if (frac_SpF != 0) {
	  if (slot_lag > (frac_SpF-1.0) ) {
	    slot_lag -= frac_SpF;
	    gfp->padding = 0;
	  }
	  else {
	    gfp->padding = 1;
	    slot_lag += (1-frac_SpF);
	  }
	}
      }
    }
  }


  /********************** status display  *****************************/
  if (!gfp->gtkflag && !gfp->silent) {
    int mod = gfp->version == 0 ? 200 : 50;
    if (gfp->frameNum%mod==0) {
      timestatus(gfp->out_samplerate,gfp->frameNum,gfp->totalframes,gfp->framesize);
#ifdef BRHIST
      if (disp_brhist)
	{
	  brhist_add_count();
	  brhist_disp();
	}
#endif
    }
  }


  if (gfp->psymodel) {
    /* psychoacoustic model
     * psy model has a 1 granule (576) delay that we must compensate for
     * (mt 6/99).
     */
    short int *bufp[2];  /* address of beginning of left & right granule */
    int blocktype[2];

    ms_ratio_prev=ms_ratio[gfp->mode_gr-1];
    for (gr=0; gr < gfp->mode_gr ; gr++) {

      for ( ch = 0; ch < gfp->stereo; ch++ )
	bufp[ch] = &inbuf[ch][576 + gr*576-FFTOFFSET];

      L3psycho_anal( gfp,bufp, gr, 
		     &ms_ratio[gr],&ms_ratio_next,&ms_ener_ratio[gr],
		     masking_ratio, masking_MS_ratio,
		     pe[gr],pe_MS[gr],blocktype);

      for ( ch = 0; ch < gfp->stereo; ch++ )
	l3_side.gr[gr].ch[ch].tt.block_type=blocktype[ch];

    }
  }else{
    for (gr=0; gr < gfp->mode_gr ; gr++)
      for ( ch = 0; ch < gfp->stereo; ch++ ) {
	l3_side.gr[gr].ch[ch].tt.block_type=NORM_TYPE;
	pe[gr][ch]=700;
      }
  }


  /* block type flags */
  for( gr = 0; gr < gfp->mode_gr; gr++ ) {
    for ( ch = 0; ch < gfp->stereo; ch++ ) {
      gr_info *cod_info = &l3_side.gr[gr].ch[ch].tt;
      cod_info->mixed_block_flag = 0;     /* never used by this model */
      if (cod_info->block_type == NORM_TYPE )
	cod_info->window_switching_flag = 0;
      else
	cod_info->window_switching_flag = 1;
    }
  }

  /* polyphase filtering / mdct */
  mdct_sub48(gfp,inbuf[0], inbuf[1], xr, &l3_side);

  /* use m/s gfp->stereo? */
  check_ms_stereo =  (gfp->mode == MPG_MD_JOINT_STEREO);
  if (check_ms_stereo) {
    /* make sure block type is the same in each channel */
    check_ms_stereo =
      (l3_side.gr[0].ch[0].tt.block_type==l3_side.gr[0].ch[1].tt.block_type) &&
      (l3_side.gr[1].ch[0].tt.block_type==l3_side.gr[1].ch[1].tt.block_type);
  }
  if (check_ms_stereo) {
    /* ms_ratio = is like the ratio of side_energy/total_energy */
    FLOAT8 ms_ratio_ave,ms_ener_ratio_ave;
    /*     ms_ratio_ave = .5*(ms_ratio[0] + ms_ratio[1]);*/
    ms_ratio_ave = .25*(ms_ratio[0] + ms_ratio[1]+
			 ms_ratio_prev + ms_ratio_next);
    ms_ener_ratio_ave = .5*(ms_ener_ratio[0]+ms_ener_ratio[1]);
    if ( ms_ratio_ave <.35 /*&& ms_ener_ratio_ave<.75*/ ) gfp->mode_ext = MPG_MD_MS_LR;
  }
  if (gfp->force_ms) gfp->mode_ext = MPG_MD_MS_LR;


#ifdef HAVEGTK
  if (gfp->gtkflag) {
    int j;
    for ( gr = 0; gr < gfp->mode_gr; gr++ ) {
      for ( ch = 0; ch < gfp->stereo; ch++ ) {
	pinfo->ms_ratio[gr]=ms_ratio[gr];
	pinfo->ms_ener_ratio[gr]=ms_ener_ratio[gr];
	pinfo->blocktype[gr][ch]=
	  l3_side.gr[gr].ch[ch].tt.block_type;
	for ( j = 0; j < 576; j++ ) pinfo->xr[gr][ch][j]=xr[gr][ch][j];
	/* if MS stereo, switch to MS psy data */
	if (gfp->mode_ext==MPG_MD_MS_LR) {
	  pinfo->pe[gr][ch]=pinfo->pe[gr][ch+2];
	  pinfo->ers[gr][ch]=pinfo->ers[gr][ch+2];
	  memcpy(pinfo->energy[gr][ch],pinfo->energy[gr][ch+2],
		 sizeof(pinfo->energy[gr][ch]));
	}
      }
    }
  }
#endif




  /* bit and noise allocation */
  if (MPG_MD_MS_LR == gfp->mode_ext) {
    masking = &masking_MS_ratio;    /* use MS masking */
    pe_use=&pe_MS;
  } else {
    masking = &masking_ratio;    /* use LR masking */
    pe_use=&pe;
  }


  /*
  VBR_iteration_loop_new( gfp,*pe_use, ms_ratio, xr, masking, &l3_side, l3_enc,
  	  &scalefac);
  */


  if (gfp->VBR) {
    VBR_iteration_loop( gfp,*pe_use, ms_ratio, xr, *masking, &l3_side, l3_enc,
			scalefac);
  }else{
    iteration_loop( gfp,*pe_use, ms_ratio, xr, *masking, &l3_side, l3_enc,
		    scalefac);
  }




#ifdef BRHIST
  brhist_temp[gfp->bitrate_index]++;
#endif


  /*  write the frame to the bitstream  */
  getframebits(gfp,&bitsPerFrame,&mean_bits);
  III_format_bitstream( gfp,bitsPerFrame, l3_enc, &l3_side,
			scalefac, &bs);


  frameBits = bs.totbit - sentBits;


  if ( frameBits % bitsPerSlot )   /* a program failure */
    fprintf( stderr, "Sent %ld bits = %ld slots plus %ld\n",
	     frameBits, frameBits/bitsPerSlot,
	     frameBits%bitsPerSlot );
  sentBits += frameBits;

  /* copy mp3 bit buffer into array */
  mp3count = copy_buffer(mp3buf,mp3buf_size,&bs);

  if (gfp->bWriteVbrTag) AddVbrFrame((int)(sentBits/8));

#ifdef HAVEGTK
  if (gfp->gtkflag) {
    int j;
    for ( ch = 0; ch < gfp->stereo; ch++ ) {
      for ( j = 0; j < FFTOFFSET; j++ )
	pinfo->pcmdata[ch][j] = pinfo->pcmdata[ch][j+gfp->framesize];
      for ( j = FFTOFFSET; j < 1600; j++ ) {
	pinfo->pcmdata[ch][j] = inbuf[ch][j-FFTOFFSET];
      }
    }
  }
#endif
  gfp->frameNum++;

  return mp3count;
}



int fill_buffer_resample(lame_global_flags *gfp,short int *outbuf,int desired_len,
        short int *inbuf,int len,int *num_used,int ch) {

  static FLOAT8 itime[2];
#define OLDBUFSIZE 5
  static short int inbuf_old[2][OLDBUFSIZE];
  static int init[2]={0,0};
  int i,j=0,k,linear,value;

  if (gfp->frameNum==0 && !init[ch]) {
    init[ch]=1;
    itime[ch]=0;
    memset((char *) inbuf_old[ch], 0, sizeof(short int)*OLDBUFSIZE);
  }
  if (gfp->frameNum!=0) init[ch]=0; /* reset, for next time framenum=0 */


  /* if downsampling by an integer multiple, use linear resampling,
   * otherwise use quadratic */
  linear = ( fabs(gfp->resample_ratio - floor(.5+gfp->resample_ratio)) < .0001 );

  /* time of j'th element in inbuf = itime + j/ifreq; */
  /* time of k'th element in outbuf   =  j/ofreq */
  for (k=0;k<desired_len;k++) {
    int y0,y1,y2,y3;
    FLOAT8 x0,x1,x2,x3;
    FLOAT8 time0;

    time0 = k*gfp->resample_ratio;       /* time of k'th output sample */
    j = floor( time0 -itime[ch]  );
    /* itime[ch] + j;    */            /* time of j'th input sample */
    if (j+2 >= len) break;             /* not enough data in input buffer */

    x1 = time0-(itime[ch]+j);
    x2 = x1-1;
    y1 = (j<0) ? inbuf_old[ch][OLDBUFSIZE+j] : inbuf[j];
    y2 = ((1+j)<0) ? inbuf_old[ch][OLDBUFSIZE+1+j] : inbuf[1+j];

    /* linear resample */
    if (linear) {
      outbuf[k] = floor(.5 +  (y2*x1-y1*x2) );
    } else {
      /* quadratic */
      x0 = x1+1;
      x3 = x1-2;
      y0 = ((j-1)<0) ? inbuf_old[ch][OLDBUFSIZE+(j-1)] : inbuf[j-1];
      y3 = ((j+2)<0) ? inbuf_old[ch][OLDBUFSIZE+(j+2)] : inbuf[j+2];
      value = floor(.5 +
			-y0*x1*x2*x3/6 + y1*x0*x2*x3/2 - y2*x0*x1*x3/2 +y3*x0*x1*x2/6
			);
      if (value > 32767) outbuf[k]=32767;
      else if (value < -32767) outbuf[k]=-32767;
      else outbuf[k]=value;

      /*
      printf("k=%i  new=%i   [ %i %i %i %i ]\n",k,outbuf[k],
	     y0,y1,y2,y3);
      */
    }
  }


  /* k = number of samples added to outbuf */
  /* last k sample used data from j,j+1, or j+1 overflowed buffer */
  /* remove num_used samples from inbuf: */
  *num_used = Min(len,j+2);
  itime[ch] += *num_used - k*gfp->resample_ratio;
  for (i=0;i<OLDBUFSIZE;i++)
    inbuf_old[ch][i]=inbuf[*num_used + i -OLDBUFSIZE];
  return k;
}




int fill_buffer(lame_global_flags *gfp,short int *outbuf,int desired_len,short int *inbuf,int len) {
  int j;
  j=Min(desired_len,len);
  memcpy( (char *) outbuf,(char *)inbuf,sizeof(short int)*j);
  return j;
}




/*
 * THE MAIN LAME ENCODING INTERFACE
 * mt 3/00
 *
 * input pcm data, output (maybe) mp3 frames.
 * This routine handles all buffering, resampling and filtering for you.
 * The required mp3buffer_size can be computed from num_samples,
 * samplerate and encoding rate, but here is a worst case estimate:
 *
 * mp3buffer_size in bytes = 1.25*num_samples + 7200
 *
 * return code = number of bytes output in mp3buffer.  can be 0
*/
int lame_encode_buffer(lame_global_flags *gfp,
   short int buffer_l[], short int buffer_r[],int nsamples,
   char *mp3buf, int mp3buf_size)
{
  static int frame_buffered=0;
  int mp3size=0,ret,i,ch,mf_needed;

  short int *in_buffer[2];
  in_buffer[0] = buffer_l;
  in_buffer[1] = buffer_r;

  /* some sanity checks */
  assert(ENCDELAY>=MDCTDELAY);
  assert(BLKSIZE-FFTOFFSET >= 0);
  mf_needed = BLKSIZE+gfp->framesize-FFTOFFSET;
  assert(MFSIZE>=mf_needed);

  /* The reason for
   *       int mf_samples_to_encode = ENCDELAY + 288;
   * ENCDELAY = internal encoder delay.  And then we have to add 288
   * because of the 50% MDCT overlap.  A 576 MDCT granule decodes to
   * 1152 samples.  To synthesize the 576 samples centered under this granule
   * we need the previous granule for the first 288 samples (no problem), and
   * the next granule for the next 288 samples (not possible if this is last
   * granule).  So we need to pad with 288 samples to make sure we can
   * encode the 576 samples we are interested in.
   */
  if (gfp->frameNum==0 && !frame_buffered) {
    memset((char *) mfbuf, 0, sizeof(mfbuf));
    frame_buffered=1;
    mf_samples_to_encode = ENCDELAY+288;
    mf_size=ENCDELAY-MDCTDELAY;  /* we pad input with this many 0's */
  }
  if (gfp->frameNum==1) {
    /* reset, for the next time frameNum==0 */
    frame_buffered=0;
  }

  if (gfp->num_channels==2  && gfp->stereo==1) {
    /* downsample to mono */
    for (i=0; i<nsamples; ++i) {
      in_buffer[0][i]=((int)in_buffer[0][i]+(int)in_buffer[1][i])/2;
      in_buffer[1][i]=0;
    }
  }


  while (nsamples > 0) {
    int n_in=0;
    int n_out=0;
    /* copy in new samples */
    for (ch=0; ch<gfp->stereo; ch++) {
      if (gfp->resample_ratio!=1)  {
	n_out=fill_buffer_resample(gfp,&mfbuf[ch][mf_size],gfp->framesize,
					  in_buffer[ch],nsamples,&n_in,ch);
      } else {
	n_out=fill_buffer(gfp,&mfbuf[ch][mf_size],gfp->framesize,in_buffer[ch],nsamples);
	n_in = n_out;
      }
      in_buffer[ch] += n_in;
    }


    nsamples -= n_in;
    mf_size += n_out;
    assert(mf_size<=MFSIZE);
    mf_samples_to_encode += n_out;

    if (mf_size >= mf_needed) {
      /* encode the frame */
      ret = lame_encode_frame(gfp,mfbuf[0],mfbuf[1],mf_size,mp3buf,mp3buf_size);
      if (ret == -1) {
	/* fatel error: mp3buffer was too small */
	return -1;
      }
      mp3buf += ret;
      mp3size += ret;

      /* shift out old samples */
      mf_size -= gfp->framesize;
      mf_samples_to_encode -= gfp->framesize;
      for (ch=0; ch<gfp->stereo; ch++)
	for (i=0; i<mf_size; i++)
	  mfbuf[ch][i]=mfbuf[ch][i+gfp->framesize];
    }
  }
  assert(nsamples==0);
  return mp3size;
}




int lame_encode_buffer_interleaved(lame_global_flags *gfp,
   short int buffer[], int nsamples, char *mp3buf, int mp3buf_size)
{
  static int frame_buffered=0;
  int mp3size=0,ret,i,ch,mf_needed;

  /* some sanity checks */
  assert(ENCDELAY>=MDCTDELAY);
  assert(BLKSIZE-FFTOFFSET >= 0);
  mf_needed = BLKSIZE+gfp->framesize-FFTOFFSET;
  assert(MFSIZE>=mf_needed);

  if (gfp->num_channels == 1) {
    return lame_encode_buffer(gfp,buffer, NULL ,nsamples,mp3buf,mp3buf_size);
  }

  if (gfp->resample_ratio!=1)  {
    short int *buffer_l;
    short int *buffer_r;
    buffer_l=malloc(sizeof(short int)*nsamples);
    buffer_r=malloc(sizeof(short int)*nsamples);
    if (buffer_l == NULL || buffer_r == NULL) {
      return -1;
    }
    for (i=0; i<nsamples; i++) {
      buffer_l[i]=buffer[2*i];
      buffer_r[i]=buffer[2*i+1];
    }
    ret = lame_encode_buffer(gfp,buffer_l,buffer_r,nsamples,mp3buf,mp3buf_size);
    free(buffer_l);
    free(buffer_r);
    return ret;
  }


  if (gfp->frameNum==0 && !frame_buffered) {
    memset((char *) mfbuf, 0, sizeof(mfbuf));
    frame_buffered=1;
    mf_samples_to_encode = ENCDELAY+288;
    mf_size=ENCDELAY-MDCTDELAY;  /* we pad input with this many 0's */
  }
  if (gfp->frameNum==1) {
    /* reset, for the next time frameNum==0 */
    frame_buffered=0;
  }

  if (gfp->num_channels==2  && gfp->stereo==1) {
    /* downsample to mono */
    for (i=0; i<nsamples; ++i) {
      buffer[2*i]=((int)buffer[2*i]+(int)buffer[2*i+1])/2;
      buffer[2*i+1]=0;
    }
  }


  while (nsamples > 0) {
    int n_out;
    /* copy in new samples */
    n_out = Min(gfp->framesize,nsamples);
    for (i=0; i<n_out; ++i) {
      mfbuf[0][mf_size+i]=buffer[2*i];
      mfbuf[1][mf_size+i]=buffer[2*i+1];
    }
    buffer += 2*n_out;

    nsamples -= n_out;
    mf_size += n_out;
    assert(mf_size<=MFSIZE);
    mf_samples_to_encode += n_out;

    if (mf_size >= mf_needed) {
      /* encode the frame */
      ret = lame_encode_frame(gfp,mfbuf[0],mfbuf[1],mf_size,mp3buf,mp3buf_size);
      if (ret == -1) {
	/* fatel error: mp3buffer was too small */
	return -1;
      }
      mp3buf += ret;
      mp3size += ret;

      /* shift out old samples */
      mf_size -= gfp->framesize;
      mf_samples_to_encode -= gfp->framesize;
      for (ch=0; ch<gfp->stereo; ch++)
	for (i=0; i<mf_size; i++)
	  mfbuf[ch][i]=mfbuf[ch][i+gfp->framesize];
    }
  }
  assert(nsamples==0);
  return mp3size;
}














/* old LAME interface */
/* With this interface, it is the users responsibilty to keep track of the
 * buffered, unencoded samples.  Thus mf_samples_to_encode is not incremented.
 *
 * lame_encode() is also used to flush the PCM input buffer by
 * lame_encode_finish()
 */
int lame_encode(lame_global_flags *gfp, short int in_buffer[2][1152],char *mp3buf,int size){
  int imp3,save;
  save = mf_samples_to_encode;
  imp3= lame_encode_buffer(gfp,in_buffer[0],in_buffer[1],576*gfp->mode_gr,
        mp3buf,size);
  mf_samples_to_encode = save;
  return imp3;
}


#ifdef __FreeBSD__
# include <floatingpoint.h>
#endif


/* initialize mp3 encoder */
void lame_init(lame_global_flags *gfp)
{

  /*
   *  Disable floating point exepctions
   */
#ifdef __FreeBSD__
  {
  /* seet floating point mask to the Linux default */
  fp_except_t mask;
  mask=fpgetmask();
  /* if bit is set, we get SIGFPE on that error! */
  fpsetmask(mask & ~(FP_X_INV|FP_X_DZ));
  /*  fprintf(stderr,"FreeBSD mask is 0x%x\n",mask); */
  }
#endif
#if defined(__riscos__) && !defined(ABORTFP)
  /* Disable FPE's under RISC OS */
  /* if bit is set, we disable trapping that error! */
  /*   _FPE_IVO : invalid operation */
  /*   _FPE_DVZ : divide by zero */
  /*   _FPE_OFL : overflow */
  /*   _FPE_UFL : underflow */
  /*   _FPE_INX : inexact */
  DisableFPETraps( _FPE_IVO | _FPE_DVZ | _FPE_OFL );
#endif


  /*
   *  Debugging stuff
   *  The default is to ignore FPE's, unless compiled with -DABORTFP
   *  so add code below to ENABLE FPE's.
   */

#if defined(ABORTFP) && !defined(__riscos__)
#if defined(_MSC_VER)
  {
	#include <float.h>
	unsigned int mask;
	mask=_controlfp( 0, 0 );
	mask&=~(_EM_OVERFLOW|_EM_UNDERFLOW|_EM_ZERODIVIDE|_EM_INVALID);
	mask=_controlfp( mask, _MCW_EM );
	}
#elif defined(__CYGWIN__)
#  define _FPU_GETCW(cw) __asm__ ("fnstcw %0" : "=m" (*&cw))
#  define _FPU_SETCW(cw) __asm__ ("fldcw %0" : : "m" (*&cw))

#  define _EM_INEXACT     0x00000001 /* inexact (precision) */
#  define _EM_UNDERFLOW   0x00000002 /* underflow */
#  define _EM_OVERFLOW    0x00000004 /* overflow */
#  define _EM_ZERODIVIDE  0x00000008 /* zero divide */
#  define _EM_INVALID     0x00000010 /* invalid */
  {
    unsigned int mask;
    _FPU_GETCW(mask);
    /* Set the FPU control word to abort on most FPEs */
    mask &= ~(_EM_UNDERFLOW | _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID);
    _FPU_SETCW(mask);
  }
# else
  {
#  include <fpu_control.h>
#ifndef _FPU_GETCW
#define _FPU_GETCW(cw) __asm__ ("fnstcw %0" : "=m" (*&cw))
#endif
#ifndef _FPU_SETCW
#define _FPU_SETCW(cw) __asm__ ("fldcw %0" : : "m" (*&cw))
#endif
    unsigned int mask;
    _FPU_GETCW(mask);
    /* Set the Linux mask to abort on most FPE's */
    /* if bit is set, we _mask_ SIGFPE on that error! */
    /*  mask &= ~( _FPU_MASK_IM | _FPU_MASK_ZM | _FPU_MASK_OM | _FPU_MASK_UM );*/
    mask &= ~( _FPU_MASK_IM | _FPU_MASK_ZM | _FPU_MASK_OM );
    _FPU_SETCW(mask);
  }
#endif
#endif /* ABORTFP && !__riscos__ */



  /* Global flags.  set defaults here */
  gfp->allow_diff_short=0;
  gfp->ATHonly=0;
  gfp->noATH=0;
  gfp->bWriteVbrTag=1;
  gfp->cwlimit=0;
  gfp->disable_reservoir=0;
  gfp->experimentalX = 0;
  gfp->experimentalY = 0;
  gfp->experimentalZ = 0;
  gfp->frameNum=0;
  gfp->gtkflag=0;
  gfp->quality=5;
  gfp->input_format=sf_unknown;

  gfp->filter_type=0;
  gfp->lowpassfreq=0;
  gfp->highpassfreq=0;
  gfp->lowpasswidth=-1;
  gfp->highpasswidth=-1;
  gfp->lowpass1=0;
  gfp->lowpass2=0;
  gfp->highpass1=0;
  gfp->highpass2=0;
  gfp->lowpass_band=32;
  gfp->highpass_band=-1;

  gfp->no_short_blocks=0;
  gfp->resample_ratio=1;
  gfp->padding_type=2;
  gfp->padding=0;
  gfp->swapbytes=0;
  gfp->silent=0;
  gfp->totalframes=0;
  gfp->VBR=0;
  gfp->VBR_q=4;
  gfp->VBR_min_bitrate_kbps=0;
  gfp->VBR_max_bitrate_kbps=0;
  gfp->VBR_min_bitrate=1;
  gfp->VBR_max_bitrate=13;


  gfp->version = 1;   /* =1   Default: MPEG-1 */
  gfp->mode = MPG_MD_JOINT_STEREO;
  gfp->mode_fixed=0;
  gfp->force_ms=0;
  gfp->brate=0;
  gfp->copyright=0;
  gfp->original=1;
  gfp->extension=0;
  gfp->error_protection=0;
  gfp->emphasis=0;
  gfp->in_samplerate=1000*44.1;
  gfp->out_samplerate=0;
  gfp->num_channels=2;
  gfp->num_samples=MAX_U_32_NUM;

  gfp->inPath=NULL;
  gfp->outPath=NULL;
  id3tag.used=0;

}



/*****************************************************************/
/* flush internal mp3 buffers,                                   */
/*****************************************************************/
int lame_encode_finish(lame_global_flags *gfp,char *mp3buffer, int mp3buffer_size)
{
  int imp3,mp3count,mp3buffer_size_remaining;
  short int buffer[2][1152];
  memset((char *)buffer,0,sizeof(buffer));
  mp3count = 0;

  while (mf_samples_to_encode > 0) {

    mp3buffer_size_remaining = mp3buffer_size - mp3count;
    /* if user specifed buffer size = 0, dont check size */
    if (mp3buffer_size == 0) mp3buffer_size_remaining=0;  
    imp3=lame_encode(gfp,buffer,mp3buffer,mp3buffer_size_remaining);

    if (imp3 == -1) {
      /* fatel error: mp3buffer too small */
      desalloc_buffer(&bs);    /* Deallocate all buffers */
      return -1;
    }
    mp3buffer += imp3;
    mp3count += imp3;
    mf_samples_to_encode -= gfp->framesize;
  }


  gfp->frameNum--;
  if (!gfp->gtkflag && !gfp->silent) {
      timestatus(gfp->out_samplerate,gfp->frameNum,gfp->totalframes,gfp->framesize);
#ifdef BRHIST
      if (disp_brhist)
	{
	  brhist_add_count();
	  brhist_disp();
	  brhist_disp_total(gfp);
	}
#endif
      fprintf(stderr,"\n");
      fflush(stderr);
  }


  III_FlushBitstream();
  mp3buffer_size_remaining = mp3buffer_size - mp3count;
  /* if user specifed buffer size = 0, dont check size */
  if (mp3buffer_size == 0) mp3buffer_size_remaining=0;  

  imp3= copy_buffer(mp3buffer,mp3buffer_size_remaining,&bs);
  if (imp3 == -1) {
    /* fatel error: mp3buffer too small */
    desalloc_buffer(&bs);    /* Deallocate all buffers */
    return -1;
  }

  mp3count += imp3;
  desalloc_buffer(&bs);    /* Deallocate all buffers */
  return mp3count;
}


/*****************************************************************/
/* write VBR Xing header, and ID3 tag, if asked for               */
/*****************************************************************/
void lame_mp3_tags(lame_global_flags *gfp)
{
  if (gfp->bWriteVbrTag)
    {
      /* Calculate relative quality of VBR stream
       * 0=best, 100=worst */
      int nQuality=gfp->VBR_q*100/9;
      /* Write Xing header again */
      PutVbrTag(gfp->outPath,nQuality,1-gfp->version);
    }


  /* write an ID3 tag  */
  if(id3tag.used) {
    id3_buildtag(&id3tag);
    id3_writetag(gfp->outPath, &id3tag);
  }
}


void lame_version(lame_global_flags *gfp,char *ostring) {
  strncpy(ostring,get_lame_version(),20);
}

