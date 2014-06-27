#ifndef GTKANAL_DOT_H
#define GTKANAL_DOT_H
#include "lame.h"
#include "encoder.h"

#define READ_AHEAD 10             /* number of frames to read ahead */
#define MAXMPGLAG READ_AHEAD      /* if the mpg123 lag becomes bigger than this 				     we have to stop */
#define NUMBACK 6                 /* number of frames we can back up */
#define NUMPINFO (NUMBACK+READ_AHEAD+1)



typedef struct {
  int frameNum;           /* current frame number */
  int frameNum123;
  int num_samples;        /* number of pcm samples read for this frame */
  double frametime;       /* starting time of frame, in seconds */
  double pcmdata[2][1600];
  double pcmdata2[2][1152+1152-DECDELAY];
  double xr[2][2][576];
  double mpg123xr[2][2][576];
  double ms_ratio[2];
  double ms_ener_ratio[2];

  /* L,R, M and S values */
  double energy[2][4][BLKSIZE];
  double pe[2][4];
  double thr[2][4][SBMAX_l];
  double en[2][4][SBMAX_l];
  double thr_s[2][4][3*SBMAX_s];
  double en_s[2][4][3*SBMAX_s];
  double ers[2][4];

  double sfb[2][2][SBMAX_l];
  double sfb_s[2][2][3*SBMAX_s];
  double LAMEsfb[2][2][SBMAX_l];
  double LAMEsfb_s[2][2][3*SBMAX_s];

  int LAMEqss[2][2];
  int qss[2][2];
  int big_values[2][2];
  int sub_gain[2][2][3];

  double xfsf[2][2][SBMAX_l];
  double xfsf_s[2][2][3*SBMAX_s];

  int over[2][2];
  double tot_noise[2][2];
  double max_noise[2][2];
  double over_noise[2][2];
  int blocktype[2][2];
  int scalefac_scale[2][2];
  int mpg123blocktype[2][2];
  int mixed[2][2];
  int mainbits[2][2];
  int LAMEmainbits[2][2];
  int framesize,stereo,js,ms_stereo,i_stereo,emph,bitrate,sampfreq,maindata;
  int crc,padding;
  int scfsi[2],mean_bits,resvsize;
  int totbits;
} plotting_data;
 

int gtkcontrol(lame_global_flags *gfp);
extern plotting_data *pinfo;
extern int gtkflag;

#endif
