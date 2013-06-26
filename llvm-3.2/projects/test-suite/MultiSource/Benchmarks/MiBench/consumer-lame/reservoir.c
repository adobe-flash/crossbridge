/**********************************************************************
 * ISO MPEG Audio Subgroup Software Simulation Group (1996)
 * ISO 13818-3 MPEG-2 Audio Encoder - Lower Sampling Frequency Extension
 *
 **********************************************************************/
/*
  Revision History:

  Date        Programmer                Comment
  ==========  ========================= ===============================
  1995/09/06  mc@fivebats.com           created

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "util.h"
#ifdef HAVEGTK
#include "gtkanal.h"
#endif


/*
  Layer3 bit reservoir:
  Described in C.1.5.4.2.2 of the IS
*/

static int ResvSize = 0; /* in bits */
static int ResvMax  = 0; /* in bits */

/*
  ResvFrameBegin:
  Called (repeatedly) at the beginning of a frame. Updates the maximum
  size of the reservoir, and checks to make sure main_data_begin
  was set properly by the formatter
*/
int
ResvFrameBegin(lame_global_flags *gfp,III_side_info_t *l3_side, int mean_bits, int frameLength )
{
    int fullFrameBits;
    int resvLimit;

    if (gfp->frameNum==0) {
      ResvSize=0;
    }


    if ( gfp->version == 1 )
    {
	resvLimit = 4088; /* main_data_begin has 9 bits in MPEG 1 */
    }
    else
    {
	resvLimit = 2040; /* main_data_begin has 8 bits in MPEG 2 */
    }

    /*
      main_data_begin was set by the formatter to the
      expected value for the next call -- this should
      agree with our reservoir size
    */

#ifdef DEBUG
    fprintf( stderr, ">>> ResvSize = %d\n", ResvSize );
#endif
    /* check expected resvsize */
    assert( (l3_side->main_data_begin * 8) == ResvSize );
    fullFrameBits = mean_bits * gfp->mode_gr + ResvSize;

    /*
      determine maximum size of reservoir:
      ResvMax + frameLength <= 7680;
    */
    if ( frameLength > 7680 )
	ResvMax = 0;
    else
	ResvMax = 7680 - frameLength;
    if (gfp->disable_reservoir) ResvMax=0;


    /*
      limit max size to resvLimit bits because
      main_data_begin cannot indicate a
      larger value
      */
    if ( ResvMax > resvLimit )
	ResvMax = resvLimit;

#ifdef HAVEGTK
  if (gfp->gtkflag){
    pinfo->mean_bits=mean_bits/2;  /* expected bits per channel per granule */
    pinfo->resvsize=ResvSize;
  }
#endif

    return fullFrameBits;
}


/*
  ResvMaxBits2:
  As above, but now it *really* is bits per granule (both channels).  
  Mark Taylor 4/99
*/
void ResvMaxBits(int mean_bits, int *targ_bits, int *extra_bits, int gr)
{
  int add_bits;
  *targ_bits = mean_bits ;
  /* extra bits if the reservoir is almost full */
  if (ResvSize > ((ResvMax * 9) / 10)) {
    add_bits= ResvSize-((ResvMax * 9) / 10);
    *targ_bits += add_bits;
  }else {
    add_bits =0 ;
    /* build up reservoir.  this builds the reservoir a little slower
     * than FhG.  It could simple be mean_bits/15, but this was rigged
     * to always produce 100 (the old value) at 128kbs */
    *targ_bits -= (int) (mean_bits/15.2);
  }

  
  /* amount from the reservoir we are allowed to use. ISO says 6/10 */
  *extra_bits =    
    (ResvSize  < (ResvMax*6)/10  ? ResvSize : (ResvMax*6)/10);
  *extra_bits -= add_bits;
  
  if (*extra_bits < 0) *extra_bits=0;

  
}

/*
  ResvAdjust:
  Called after a granule's bit allocation. Readjusts the size of
  the reservoir to reflect the granule's usage.
*/
void
ResvAdjust(lame_global_flags *gfp,gr_info *gi, III_side_info_t *l3_side, int mean_bits )
{
    ResvSize += (mean_bits / gfp->stereo) - gi->part2_3_length;
}


/*
  ResvFrameEnd:
  Called after all granules in a frame have been allocated. Makes sure
  that the reservoir size is within limits, possibly by adding stuffing
  bits. Note that stuffing bits are added by increasing a granule's
  part2_3_length. The bitstream formatter will detect this and write the
  appropriate stuffing bits to the bitstream.
*/
void
ResvFrameEnd(lame_global_flags *gfp,III_side_info_t *l3_side, int mean_bits)
{
    int stuffingBits;
    int over_bits;

    /* just in case mean_bits is odd, this is necessary... */
    if ( gfp->stereo == 2 && mean_bits & 1)
	ResvSize += 1;

    over_bits = ResvSize - ResvMax;
    if ( over_bits < 0 )
	over_bits = 0;
    
    ResvSize -= over_bits;
    stuffingBits = over_bits;

    /* we must be byte aligned */
    if ( (over_bits = ResvSize % 8) )
    {
	stuffingBits += over_bits;
	ResvSize -= over_bits;
    }


    l3_side->resvDrain = stuffingBits;
    return;

}


