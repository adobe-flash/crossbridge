/*
 *	Interface to MP3 LAME encoding engine
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
#ifndef LAME_H_INCLUDE
#define LAME_H_INCLUDE
#include <stdio.h>

/* maximum size of mp3buffer needed if you encode at most 1152 samples for
   each call to lame_encode_buffer.  see lame_encode_buffer() below  */
#define LAME_MAXMP3BUFFER 16384


typedef enum sound_file_format_e {
	sf_unknown, sf_wave, sf_aiff, sf_mp3, sf_raw
} sound_file_format;


/***********************************************************************
*
*  Global Variables.  
*
*  substantiated in lame.c
*
*  Initilized and default values set by gf=lame_init()
*  gf is a pointer to this struct, which the user may use to 
*  override any of the default values
*
*  a call to lame_set_params() is also needed
*
***********************************************************************/
typedef struct  {
  /* input file description */
  unsigned long num_samples;  /* number of samples. default=2^32-1    */
  int num_channels;           /* input number of channels. default=2  */
  int in_samplerate;          /* input_samp_rate. default=44.1kHz     */
  int out_samplerate;         /* output_samp_rate. (usually determined automatically)   */ 


  /* general control params */
  int gtkflag;                /* run frame analyzer?       */
  int bWriteVbrTag;           /* add Xing VBR tag?         */
  int quality;                /* quality setting 0=best,  9=worst  */
  int silent;                 /* disable some status output */
  int mode;                       /* 0,1,2,3 stereo,jstereo,dual channel,mono */
  int mode_fixed;                 /* use specified the mode, do not use lame's opinion of the best mode */
  int force_ms;                   /* force M/S mode.  requires mode=1 */
  int brate;                      /* bitrate */

  /* frame params */
  int copyright;                  /* mark as copyright. default=0 */
  int original;                   /* mark as original. default=1 */
  int error_protection;           /* use 2 bytes per frame for a CRC checksum. default=0*/
  int padding_type;               /* 0=no padding, 1=always pad, 2=adjust padding */
  int extension;                  /* the MP3 'private extension' bit.  meaningless */

  /* quantization/noise shaping */
  int disable_reservoir;          /* use bit reservoir? */
  int experimentalX;            
  int experimentalY;
  int experimentalZ;

  /* VBR control */
  int VBR;
  int VBR_q;
  int VBR_min_bitrate_kbps;
  int VBR_max_bitrate_kbps;


  /* resampling and filtering */
  int lowpassfreq;                /* freq in Hz. 0=lame choses. -1=no filter */
  int highpassfreq;               /* freq in Hz. 0=lame choses. -1=no filter */
  int lowpasswidth;               /* freq width of filter, in Hz (default=15%)*/
  int highpasswidth;              /* freq width of filter, in Hz (default=15%)*/


  /* input file reading - not used if calling program does the i/o */
  sound_file_format input_format;   
  int swapbytes;              /* force byte swapping   default=0*/
  char *inPath;               /* name of input file */
  char *outPath;              /* name of output file. */
  /* Note: outPath must be set if you want Xing VBR or id3 tags
   * written */


  /* psycho acoustics and other aguments which you should not change 
   * unless you know what you are doing  */
  int ATHonly;                    /* only use ATH */
  int noATH;                      /* disable ATH */
  float cwlimit;                  /* predictability limit */
  int allow_diff_short;       /* allow blocktypes to differ between channels ? */
  int no_short_blocks;        /* disable short blocks       */
  int emphasis;                   /* obsolete */



  /********************************************************************/
  /* internal variables NOT set by calling program, and should not be */
  /* modified by the calling program                                  */
  /********************************************************************/
  long int frameNum;              /* frame counter */
  long totalframes;               /* frames: 0..totalframes-1 (estimate)*/
  int encoder_delay;
  int framesize;                  
  int version;                    /* 0=MPEG2  1=MPEG1 */
  int padding;                    /* padding for the current frame? */
  int mode_gr;                    /* granules per frame */
  int stereo;                     /* number of channels */
  int VBR_min_bitrate;            /* min bitrate index */
  int VBR_max_bitrate;            /* max bitrate index */
  float resample_ratio;           /* input_samp_rate/output_samp_rate */
  int bitrate_index;
  int samplerate_index;
  int mode_ext;

  /* lowpass and highpass filter control */
  float lowpass1,lowpass2;        /* normalized frequency bounds of passband */
  float highpass1,highpass2;      /* normalized frequency bounds of passband */
                                  
  /* polyphase filter (filter_type=0)  */
  int lowpass_band;          /* zero bands >= lowpass_band in the polyphase filterbank */
  int highpass_band;         /* zero bands <= highpass_band */



  int filter_type;          /* 0=polyphase filter, 1= FIR filter 2=MDCT filter(bad)*/
  int quantization;         /* 0 = ISO formual,  1=best amplitude */
  int noise_shaping;        /* 0 = none 
                               1 = ISO AAC model
                               2 = allow scalefac_select=1  
                             */

  int noise_shaping_stop;   /* 0 = stop at over=0, all scalefacs amplified or
                                   a scalefac has reached max value
                               1 = stop when all scalefacs amplified or        
                                   a scalefac has reached max value
                               2 = stop when all scalefacs amplified 
			    */

  int psymodel;             /* 0 = none   1=gpsycho */
  int use_best_huffman;     /* 0 = no.  1=outside loop  2=inside loop(slow) */


} lame_global_flags;






/*

The LAME API

 */


/* REQUIRED: initialize the encoder.  sets default for all encoder paramters,
 * returns pointer to encoder parameters listed above 
 */
void lame_init(lame_global_flags *);




/*********************************************************************
 * command line argument parsing & option setting.  Only supported
 * if libmp3lame compiled with LAMEPARSE defined 
 *********************************************************************/
/* OPTIONAL: call this to print an error with a brief command line usage guide and quit 
 * only supported if libmp3lame compiled with LAMEPARSE defined.  
 */
void lame_usage(lame_global_flags *, char *);

/* OPTIONAL: call this to print a command line interface usage guide and quit   */
void lame_help(lame_global_flags *, char *);

/* OPTIONAL: get the version number, in a string. of the form:  "3.63 (beta)" or 
   just "3.63".  Max allows length is 20 characters  */
void lame_version(lame_global_flags *, char *);


/* OPTIONAL: set internal options via command line argument parsing 
 * You can skip this call if you like the default values, or if
 * set the encoder parameters your self 
 */
void lame_parse_args(lame_global_flags *, int argc, char **argv);





/* REQUIRED:  sets more internal configuration based on data provided
 * above
 */
void lame_init_params(lame_global_flags *);


/* OPTONAL:  print internal lame configuration on stderr*/
void lame_print_config(lame_global_flags *);




/* input pcm data, output (maybe) mp3 frames.
 * This routine handles all buffering, resampling and filtering for you.
 * 
 * leftpcm[]       array of 16bit pcm data, left channel
 * rightpcm[]      array of 16bit pcm data, right channel
 * num_samples     number of samples in leftpcm[] and rightpcm[] (if stereo)
 * mp3buffer       pointer to buffer where mp3 output is written
 * mp3buffer_size  size of mp3buffer, in bytes
 * return code     number of bytes output in mp3buffer.  can be 0 
 *                 if return code = -1:  mp3buffer was too small
 *
 * The required mp3buffer_size can be computed from num_samples, 
 * samplerate and encoding rate, but here is a worst case estimate:
 *
 * mp3buffer_size in bytes = 1.25*num_samples + 7200
 *
 * I think a tighter bound could be:  (mt, March 2000)
 * MPEG1:
 *    num_samples*(bitrate/8)/samplerate + 4*1152*(bitrate/8)/samplerate + 512
 * MPEG2:
 *    num_samples*(bitrate/8)/samplerate + 4*576*(bitrate/8)/samplerate + 256
 *
 * but test first if you use that!
 *
 * set mp3buffer_size = 0 and LAME will not check if mp3buffer_size is
 * large enough.
 *
 * NOTE: if gfp->num_channels=2, but gfp->mode = 3 (mono), the L & R channels
 * will be averaged into the L channel before encoding only the L channel
 * This will overwrite the data in leftpcm[] and rightpcm[].
 * 
*/
int lame_encode_buffer(lame_global_flags *,short int leftpcm[], short int rightpcm[],int num_samples,
char *mp3buffer,int  mp3buffer_size);

/* as above, but input has L & R channel data interleaved.  Note: 
 * num_samples = number of samples in the L (or R)
 * channel, not the total number of samples in pcm[]  
 */
int lame_encode_buffer_interleaved(lame_global_flags *,short int pcm[], 
int num_samples, char *mp3buffer,int  mp3buffer_size);



/* input 1 pcm frame, output (maybe) 1 mp3 frame.  
 * return code = number of bytes output in mp3buffer.  can be 0 
 * NOTE: this interface is outdated, please use lame_encode_buffer() instead 
 * declair mp3buffer with:  char mp3buffer[LAME_MAXMP3BUFFER] 
 * if return code = -1:  mp3buffer was too small 
 */
int lame_encode(lame_global_flags *,short int Buffer[2][1152],char *mp3buffer,int mp3buffer_size);



/* REQUIRED:  lame_encode_finish will flush the buffers and may return a 
 * final few mp3 frames.  mp3buffer should be at least 7200 bytes.
 *
 * return code = number of bytes output to mp3buffer.  can be 0
 */
int lame_encode_finish(lame_global_flags *,char *mp3buffer, int size);


/* OPTIONAL:  lame_mp3_tags will append id3 and Xing VBR tags to
the mp3 file with name given by gf->outPath.  These calls open the file,
write tags, and close the file, so make sure the the encoding is finished
before calling these routines.  
Note: if VBR and id3 tags are turned off by the user, or turned off
by LAME because the output is not a regular file, this call does nothing
*/
void lame_mp3_tags(lame_global_flags *);




/*********************************************************************
 * lame file i/o.  Only supported
 * if libmp3lame compiled with LAMESNDFILE or LIBSNDFILE
 *********************************************************************/
/* OPTIONAL: open the input file, and parse headers if possible 
 * you can skip this call if you will do your own PCM input 
 */
void lame_init_infile(lame_global_flags *);

/* OPTIONAL:  read one frame of PCM data from audio input file opened by 
 * lame_init_infile.  Input file can be wav, aiff, raw pcm, anything
 * supported by libsndfile, or an mp3 file
 */
int lame_readframe(lame_global_flags *,short int Buffer[2][1152]);

/* OPTIONAL: close the sound input file if lame_init_infile() was used */
void lame_close_infile(lame_global_flags *);





/*********************************************************************
 * a simple interface to mpglib, part of mpg123, is also included if
 * libmp3lame is compiled with HAVEMPGLIB
 * input 1 mp3 frame, output (maybe) 1 pcm frame.   
 * lame_decode return code:  -1: error.  0: need more data.  n>0: size of pcm output
 *********************************************************************/
int lame_decode_init(void);
int lame_decode(char *mp3buf,int len,short pcm_l[],short pcm_r[]);
/* read mp3 file until mpglib returns one frame of PCM data */
#ifdef AMIGA_MPEGA
int lame_decode_initfile(const char *fullname,int *stereo,int *samp,int *bitrate, unsigned long *nsamp);
int lame_decode_fromfile(FILE *fd,short int pcm_l[], short int pcm_r[]);
#else
int lame_decode_initfile(FILE *fd,int *stereo,int *samp,int *bitrate, unsigned long *nsamp);
int lame_decode_fromfile(FILE *fd,short int pcm_l[],short int pcm_r[]);
#endif




#endif
