
/*!
 ***************************************************************************
 * \file
 *    ratectl.h
 *
 * \author
 *    Zhengguo LI
 *
 * \date
 *    14 Jan 2003
 *
 * \brief
 *    Headerfile for rate control
 **************************************************************************
 */

#ifndef _RATE_CTL_H_
#define _RATE_CTL_H_

/* generic rate control variables */
typedef struct {
  // RC flags
  int TopFieldFlag;
  int FieldControl;
  int FieldFrame;
  int NoGranularFieldRC;
  // bits stats
  int NumberofHeaderBits;
  int NumberofTextureBits;
  int NumberofBasicUnitHeaderBits;
  int NumberofBasicUnitTextureBits;
  // frame stats
  int NumberofCodedBFrame;
  int NumberofCodedPFrame;
  int NumberofGOP;
  int TotalQpforPPicture;
  int NumberofPPicture;
  // MAD stats
  int64  TotalMADBasicUnit;
  int   *MADofMB;
  // buffer and budget
  int64 CurrentBufferFullness; //LIZG 25/10/2002
  int   RemainingBits;
  // bit allocations for RC_MODE_3
  int   RCPSliceBits;
  int   RCISliceBits;
  int   RCBSliceBits[RC_MAX_TEMPORAL_LEVELS];
  int   temporal_levels;
  int   hierNb[RC_MAX_TEMPORAL_LEVELS];
  int   NPslice;
  int   NIslice;
} rc_generic;

// macroblock activity
int    diffy[16][16];
int    qp_mbaff[2][2], qp_mbaff[2][2];
int    delta_qp_mbaff[2][2], delta_qp_mbaff[2][2];

// generic functions
int    Qstep2QP( double Qstep );
double QP2Qstep( int QP );
int    calc_MAD( void );
double ComputeFrameMAD( void );
void   update_rc(Macroblock *currMB, short best_mode);

// rate control functions
// init/copy
void generic_alloc( rc_generic **prc );
void generic_free( rc_generic **prc );
void copy_rc_generic( rc_generic *dst, rc_generic *src );

// rate control CURRENT pointers
rc_generic   *generic_RC;
// rate control object pointers for RDPictureDecision buffering...
rc_generic   *generic_RC_init,   *generic_RC_best;


#endif

