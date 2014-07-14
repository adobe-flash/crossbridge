
/*!
 ************************************************************************
 * \file
 *     me_fullsearch.h
 *
 * \author
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *
 * \date
 *    9 September 2006
 *
 * \brief
 *    Headerfile for Full Search motion estimation
 **************************************************************************
 */


#ifndef _ME_FULLSEARCH_H_
#define _ME_FULLSEARCH_H_
extern int FullPelBlockMotionSearch (imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                              int blocktype, short pred_mv_x, short pred_mv_y, short* mv_x, short* mv_y,
                              int search_range,  int min_mcost, int lambda_factor);
extern int FullPelBlockMotionBiPred (imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                              int blocktype, short pred_mv_x1, short pred_mv_y1, short pred_mv_x2, short pred_mv_y2,
                              short* mv_x1, short* mv_y1, short* mv_x2, short* mv_y2,
                              int search_range, int min_mcost, int lambda_factor);
extern int SubPelBlockMotionSearch  (imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                              int blocktype, short pred_mv_x, short pred_mv_y, short* mv_x, short* mv_y,
                              int search_pos2, int search_pos4, int min_mcost, int* lambda_factor);
extern int SubPelBlockSearchBiPred  (imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                              int blocktype, short pred_mv_x, short pred_mv_y,
                              short* mv_x1, short* mv_y1, short* mv_x2, short* mv_y2,
                              int search_pos2, int search_pos4, int min_mcost, int* lambda_factor);

#endif

