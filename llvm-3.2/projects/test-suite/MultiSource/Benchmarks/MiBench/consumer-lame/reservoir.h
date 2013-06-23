/**********************************************************************
 * ISO MPEG Audio Subgroup Software Simulation Group (1996)
 * ISO 13818-3 MPEG-2 Audio Encoder - Lower Sampling Frequency Extension
 *
 * $Id: reservoir.h 33045 2007-01-09 23:44:35Z lattner $
 *
 * $Log$
 * Revision 1.1  2007/01/09 23:44:35  lattner
 * Readd mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:55:54  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.4  2000/03/21 23:02:17  markt
 * replaced all "gf." by gfp->
 *
 * Revision 1.3  2000/03/14 21:01:47  markt
 * removed fr_ps struct
 *
 * Revision 1.2  2000/01/13 16:26:50  takehiro
 * moved info.stereo into gf.stereo
 *
 * Revision 1.1.1.1  1999/11/24 08:43:40  markt
 * initial checkin of LAME
 * Starting with LAME 3.57beta with some modifications
 *
 * Revision 1.1  1996/02/14 04:04:23  rowlands
 * Initial revision
 *
 * Received from Mike Coleman
 **********************************************************************/
/*
  Revision History:

  Date        Programmer                Comment
  ==========  ========================= ===============================
  1995/09/06  mc@fivebats.com           created

*/

#ifndef RESERVOIR_H
#define RESERVOIR_H

int ResvFrameBegin( lame_global_flags *gfp,III_side_info_t *l3_side, int mean_bits, int frameLength );
void ResvMaxBits( int mean_bits, int *targ_bits, int *max_bits, int gr);
void ResvAdjust(lame_global_flags *gfp,gr_info *gi, III_side_info_t *l3_side, int mean_bits );
void ResvFrameEnd(lame_global_flags *gfp,III_side_info_t *l3_side, int mean_bits );

#endif
