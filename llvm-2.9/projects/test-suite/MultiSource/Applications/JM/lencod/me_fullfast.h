
/*!
 ************************************************************************
 * \file
 *     me_fullfast.h
 *
 * \author
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *
 * \date
 *    9 September 2006
 *
 * \brief
 *    Headerfile for Fast Full Search motion estimation
 **************************************************************************
 */


#ifndef _ME_FULLFAST_H_
#define _ME_FULLFAST_H_
int FastFullPelBlockMotionSearch (imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                              int blocktype, short pred_mv_x, short pred_mv_y, short* mv_x, short* mv_y,
                              int search_range,  int min_mcost, int lambda_factor);
void InitializeFastFullIntegerSearch ();
void ResetFastFullIntegerSearch ();
void ClearFastFullIntegerSearch ();

#endif

