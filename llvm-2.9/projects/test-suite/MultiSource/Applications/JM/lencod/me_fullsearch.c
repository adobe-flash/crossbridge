
/*!
*************************************************************************************
* \file me_fullsearch.c
*
* \brief
*    Motion Estimation using Fullsearch
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*      - Alexis Michael Tourapis <alexismt@ieee.org>
*      - Athanasios Leontaris    <aleon@dolby.com>
*
*************************************************************************************
*/

// Includes
#include "contributors.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "global.h"
#include "image.h"
#include "memalloc.h"
#include "mb_access.h"
#include "refbuf.h"

#include "me_distortion.h"
#include "me_fullsearch.h"

// Define Global Parameters
extern int *mvbits;
extern short*  spiral_search_x;
extern short*  spiral_search_y;
extern short*  spiral_hpel_search_x;
extern short*  spiral_hpel_search_y;

// Functions
/*!
 ***********************************************************************
 * \brief
 *    Full pixel block motion search
 ***********************************************************************
 */
int                                               //  ==> minimum motion cost after search
FullPelBlockMotionSearch (imgpel*   orig_pic,     // <--  original pixel values for the AxB block
                          short     ref,          // <--  reference frame (0... or -1 (backward))
                          int       list,         // <--  current list
                          int       pic_pix_x,    // <--  absolute x-coordinate of regarded AxB block
                          int       pic_pix_y,    // <--  absolute y-coordinate of regarded AxB block
                          int       blocktype,    // <--  block type (1-16x16 ... 7-4x4)
                          short     pred_mv_x,    // <--  motion vector predictor (x) in sub-pel units
                          short     pred_mv_y,    // <--  motion vector predictor (y) in sub-pel units
                          short*    mv_x,         // <--> in: search center (x) / out: motion vector (x) - in pel units
                          short*    mv_y,         // <--> in: search center (y) / out: motion vector (y) - in pel units
                          int       search_range, // <--  1-d search range in pel units
                          int       min_mcost,    // <--  minimum motion cost (cost for center or huge value)
                          int       lambda_factor)       // <--  lagrangian parameter for determining motion cost
{
  int   pos, cand_x, cand_y, mcost;

  int   list_offset   = img->mb_data[img->current_mb_nr].list_offset;
  StorablePicture *ref_picture = listX[list+list_offset][ref];

  int   best_pos      = 0;                                        // position with minimum motion cost
  int   max_pos       = (2*search_range+1)*(2*search_range+1);    // number of search positions
  int   blocksize_y   = input->blc_size[blocktype][1];            // vertical block size
  int   blocksize_x   = input->blc_size[blocktype][0];            // horizontal block size

  int   pred_x        = (pic_pix_x << 2) + pred_mv_x;       // predicted position x (in sub-pel units)
  int   pred_y        = (pic_pix_y << 2) + pred_mv_y;       // predicted position y (in sub-pel units)
  int   center_x      = pic_pix_x + *mv_x;                        // center position x (in pel units)
  int   center_y      = pic_pix_y + *mv_y;                        // center position y (in pel units)
  int   check_for_00  = (blocktype==1 && !input->rdopt && img->type!=B_SLICE && ref==0);
  int   apply_weights = ( (active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;
  int   dist_method = F_PEL + 3 * apply_weights;

  ref_pic_sub.luma = ref_picture->imgY_sub;
  img_width  = ref_picture->size_x;
  img_height = ref_picture->size_y;
  width_pad  = ref_picture->size_x_pad;
  height_pad = ref_picture->size_y_pad;

  if (apply_weights)
  {
    weight_luma = wp_weight[list + list_offset][ref][0];
    offset_luma = wp_offset[list + list_offset][ref][0];
  }

  if (ChromaMEEnable)
  {
    ref_pic_sub.crcb[0] = ref_picture->imgUV_sub[0];
    ref_pic_sub.crcb[1] = ref_picture->imgUV_sub[1];
    width_pad_cr  = ref_picture->size_x_cr_pad;
    height_pad_cr = ref_picture->size_y_cr_pad;

    if (apply_weights)
    {
      weight_cr[0] = wp_weight[list + list_offset][ref][1];
      weight_cr[1] = wp_weight[list + list_offset][ref][2];
      offset_cr[0] = wp_offset[list + list_offset][ref][1];
      offset_cr[1] = wp_offset[list + list_offset][ref][2];
    }
  }

  //===== set function for getting reference picture lines =====
  if ((center_x > search_range) && (center_x < img_width -1-search_range-blocksize_x) &&
    (center_y > search_range) && (center_y < img_height-1-search_range-blocksize_y)   )
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  //===== loop over all search positions =====
  for (pos=0; pos<max_pos; pos++)
  {
    //--- set candidate position (absolute position in pel units) ---
    cand_x = (center_x + spiral_search_x[pos])<<2;
    cand_y = (center_y + spiral_search_y[pos])<<2;

    //--- initialize motion cost (cost for motion vector) and check ---
    mcost = MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_x, pred_y);
    if (check_for_00 && cand_x==pic_pix_x && cand_y==pic_pix_y)
    {
      mcost -= WEIGHTED_COST (lambda_factor, 16);
    }
    if (mcost >= min_mcost)   continue;

    //--- add residual cost to motion cost ---
    mcost += computeUniPred[dist_method](orig_pic, blocksize_y, blocksize_x,
      min_mcost - mcost, cand_x + IMG_PAD_SIZE_TIMES4, cand_y + IMG_PAD_SIZE_TIMES4);

    //--- check if motion cost is less than minimum cost ---
    if (mcost < min_mcost)
    {
      best_pos  = pos;
      min_mcost = mcost;
    }
  }


  //===== set best motion vector and return minimum motion cost =====
  if (best_pos)
  {
    *mv_x += spiral_search_x[best_pos];
    *mv_y += spiral_search_y[best_pos];
  }
  return min_mcost;
}

/*!
 ***********************************************************************
 * \brief
 *    Full pixel block motion search
 ***********************************************************************
 */
int                                                //  ==> minimum motion cost after search
FullPelBlockMotionBiPred (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                          short       ref,         // <--  reference frame (0... or -1 (backward))
                          int       list,
                          int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                          int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                          int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                          short     pred_mv_x1,    // <--  motion vector predictor (x) in sub-pel units
                          short     pred_mv_y1,    // <--  motion vector predictor (y) in sub-pel units
                          short     pred_mv_x2,    // <--  motion vector predictor (x) in sub-pel units
                          short     pred_mv_y2,    // <--  motion vector predictor (y) in sub-pel units
                          short*    mv_x,          // <--> in: search center (x) / out: motion vector (x) - in pel units
                          short*    mv_y,          // <--> in: search center (y) / out: motion vector (y) - in pel units
                          short*    s_mv_x,        // <--> in: search center (x) / out: motion vector (x) - in pel units
                          short*    s_mv_y,        // <--> in: search center (y) / out: motion vector (y) - in pel units
                          int       search_range,  // <--  1-d search range in pel units
                          int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                          int       lambda_factor) // <--  lagrangian parameter for determining motion cost
{
  int   pos, cand_x, cand_y, mcost;


  int   list_offset   = img->mb_data[img->current_mb_nr].list_offset;

  int   best_pos      = 0;                                        // position with minimum motion cost
  int   max_pos       = (2*search_range+1)*(2*search_range+1);    // number of search positions
  int   blocksize_y   = input->blc_size[blocktype][1];            // vertical block size
  int   blocksize_x   = input->blc_size[blocktype][0];            // horizontal block size

  int   pred_x1        = (pic_pix_x << 2) + pred_mv_x1;       // predicted position x (in sub-pel units)
  int   pred_y1        = (pic_pix_y << 2) + pred_mv_y1;       // predicted position y (in sub-pel units)
  int   pred_x2        = (pic_pix_x << 2) + pred_mv_x2;       // predicted position x (in sub-pel units)
  int   pred_y2        = (pic_pix_y << 2) + pred_mv_y2;       // predicted position y (in sub-pel units)
  short center_x      = pic_pix_x + *mv_x;                      // center position x (in pel units)
  short center_y      = pic_pix_y + *mv_y;                      // center position y (in pel units)
  short ref1_center_x = pic_pix_x + *s_mv_x;                      // mvx of second pred (in pel units)
  short ref1_center_y = pic_pix_y + *s_mv_y;                      // mvy of second pred (in pel units)


  short apply_weights = (active_pps->weighted_bipred_idc>0);

  short offset1 = (apply_weights ? (list == 0?  wp_offset[list_offset    ][ref][0]:  wp_offset[list_offset + 1][0  ][ref]) : 0);
  short offset2 = (apply_weights ? (list == 0?  wp_offset[list_offset + 1][ref][0]:  wp_offset[list_offset    ][0  ][ref]) : 0);

  StorablePicture *ref_picture1 = listX[list       + list_offset][ref];
  StorablePicture *ref_picture2 = listX[(list ^ 1) + list_offset][0];

  ref_pic1_sub.luma = ref_picture1->imgY_sub;
  ref_pic2_sub.luma = ref_picture2->imgY_sub;
  img_width     = ref_picture1->size_x;
  img_height    = ref_picture1->size_y;
  width_pad    = ref_picture1->size_x_pad;
  height_pad   = ref_picture1->size_y_pad;

  if (apply_weights)
  {
    weight1 = list == 0
      ? wbp_weight[list_offset         ][ref][0][0]
      : wbp_weight[list_offset + LIST_1][0  ][ref][0];
    weight2 = list == 0
      ? wbp_weight[list_offset + LIST_1][ref][0][0]
      : wbp_weight[list_offset         ][0  ][ref][0];
    offsetBi=(offset1 + offset2 + 1)>>1;
    computeBiPred = computeBiPred2[F_PEL];
  }
  else
  {
    weight1 = 1<<luma_log_weight_denom;
    weight2 = 1<<luma_log_weight_denom;
    offsetBi = 0;
    computeBiPred = computeBiPred1[F_PEL];
  }

  if (ChromaMEEnable )
  {
    ref_pic1_sub.crcb[0] = ref_picture1->imgUV_sub[0];
    ref_pic1_sub.crcb[1] = ref_picture1->imgUV_sub[1];
    ref_pic2_sub.crcb[0] = ref_picture2->imgUV_sub[0];
    ref_pic2_sub.crcb[1] = ref_picture2->imgUV_sub[1];
    width_pad_cr  = ref_picture1->size_x_cr_pad;
    height_pad_cr = ref_picture1->size_y_cr_pad;
    if (apply_weights)
    {
      weight1_cr[0] = list == 0
        ? wbp_weight[list_offset         ][ref][0][1]
        : wbp_weight[list_offset + LIST_1][0  ][ref][1];
      weight1_cr[1] = list == 0
        ? wbp_weight[list_offset         ][ref][0][2]
        : wbp_weight[list_offset + LIST_1][0  ][ref][2];
      weight2_cr[0] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][1] : wbp_weight[list_offset         ][0  ][ref][1];
      weight2_cr[1] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][2] : wbp_weight[list_offset         ][0  ][ref][2];
      offsetBi_cr[0] = (list == 0)
        ? (wp_offset[list_offset         ][ref][1] + wp_offset[list_offset + LIST_1][ref][1] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][1] + wp_offset[list_offset         ][0  ][1] + 1) >> 1;
      offsetBi_cr[1] = (list == 0)
        ? (wp_offset[list_offset         ][ref][2] + wp_offset[list_offset + LIST_1][ref][2] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][2] + wp_offset[list_offset         ][0  ][2] + 1) >> 1;
    }
    else
    {
      weight1_cr[0] = 1<<chroma_log_weight_denom;
      weight1_cr[1] = 1<<chroma_log_weight_denom;
      weight2_cr[0] = 1<<chroma_log_weight_denom;
      weight2_cr[1] = 1<<chroma_log_weight_denom;
      offsetBi_cr[0] = 0;
      offsetBi_cr[1] = 0;
    }
  }

  //===== set function for getting reference picture lines =====
  if ((center_x > search_range) && (center_x < img_width -1-search_range-blocksize_x) &&
    (center_y > search_range) && (center_y < img_height-1-search_range-blocksize_y)   )
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
    bipred2_access_method = UMV_ACCESS;
  }

  //===== set function for getting reference picture lines =====
  if ((ref1_center_x > search_range) && (ref1_center_x < img_width -1-search_range-blocksize_x) &&
    (ref1_center_y > search_range) && (ref1_center_y < img_height-1-search_range-blocksize_y)   )
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  //===== loop over all search positions =====
  for (pos=0; pos<max_pos; pos++)
  {
    //--- set candidate position (absolute position in pel units) ---
    cand_x = (center_x + spiral_search_x[pos])<<2;
    cand_y = (center_y + spiral_search_y[pos])<<2;


    //--- initialize motion cost (cost for motion vector) and check ---
    mcost =  MV_COST_SMP (lambda_factor, (ref1_center_x<<2), (ref1_center_y<<2), pred_x1, pred_y1);
    mcost += MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_x2, pred_y2);

    if (mcost >= min_mcost)   continue;

    //--- add residual cost to motion cost ---
    mcost += computeBiPred(orig_pic,
      blocksize_y, blocksize_x, min_mcost - mcost,
      (ref1_center_x << 2) + IMG_PAD_SIZE_TIMES4,
      (ref1_center_y << 2) + IMG_PAD_SIZE_TIMES4,
      cand_x + IMG_PAD_SIZE_TIMES4, cand_y + IMG_PAD_SIZE_TIMES4);

    //--- check if motion cost is less than minimum cost ---
    if (mcost < min_mcost)
    {
      best_pos  = pos;
      min_mcost = mcost;
    }
  }


  //===== set best motion vector and return minimum motion cost =====
  if (best_pos)
  {
    *mv_x += spiral_search_x[best_pos];
    *mv_y += spiral_search_y[best_pos];
  }
  return min_mcost;
}

/*!
 ***********************************************************************
 * \brief
 *    Sub pixel block motion search
 ***********************************************************************
 */
int                                               //  ==> minimum motion cost after search
SubPelBlockMotionSearch (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                         short     ref,           // <--  reference frame (0... or -1 (backward))
                         int       list,          // <--  reference picture list
                         int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                         int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                         int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                         short     pred_mv_x,     // <--  motion vector predictor (x) in sub-pel units
                         short     pred_mv_y,     // <--  motion vector predictor (y) in sub-pel units
                         short*    mv_x,          // <--> in: search center (x) / out: motion vector (x) - in pel units
                         short*    mv_y,          // <--> in: search center (y) / out: motion vector (y) - in pel units
                         int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                         int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                         int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                         int*      lambda         // <--  lagrangian parameter for determining motion cost
                         )
{
  int   pos, best_pos, mcost;

  int   cand_mv_x, cand_mv_y;

  int   check_position0 = (!input->rdopt && img->type!=B_SLICE && ref==0 && blocktype==1 && *mv_x==0 && *mv_y==0);
  int   blocksize_x     = input->blc_size[blocktype][0];
  int   blocksize_y     = input->blc_size[blocktype][1];
  int   pic4_pix_x      = ((pic_pix_x + IMG_PAD_SIZE)<< 2);
  int   pic4_pix_y      = ((pic_pix_y + IMG_PAD_SIZE)<< 2);
  int   max_pos2        = ( !start_me_refinement_hp ? imax(1,search_pos2) : search_pos2);
  int   list_offset     = img->mb_data[img->current_mb_nr].list_offset;
  int   apply_weights   = ((active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;
  int   cmv_x, cmv_y;
  int dist_method = H_PEL + 3 * apply_weights;
  StorablePicture *ref_picture = listX[list+list_offset][ref];

  int max_pos_x4 = ((ref_picture->size_x - blocksize_x + 2*IMG_PAD_SIZE)<<2);
  int max_pos_y4 = ((ref_picture->size_y - blocksize_y + 2*IMG_PAD_SIZE)<<2);
  int lambda_factor = lambda[H_PEL];

  ref_pic_sub.luma = ref_picture->imgY_sub;
  width_pad  = ref_picture->size_x_pad;
  height_pad = ref_picture->size_y_pad;

  if (apply_weights)
  {
    weight_luma = wp_weight[list + list_offset][ref][0];
    offset_luma = wp_offset[list + list_offset][ref][0];
  }

  if (ChromaMEEnable)
  {
    ref_pic_sub.crcb[0] = ref_picture->imgUV_sub[0];
    ref_pic_sub.crcb[1] = ref_picture->imgUV_sub[1];
    width_pad_cr  = ref_picture->size_x_cr_pad;
    height_pad_cr = ref_picture->size_y_cr_pad;

    if (apply_weights)
    {
      weight_cr[0] = wp_weight[list + list_offset][ref][1];
      weight_cr[1] = wp_weight[list + list_offset][ref][2];
      offset_cr[0] = wp_offset[list + list_offset][ref][1];
      offset_cr[1] = wp_offset[list + list_offset][ref][2];
    }
  }

  /*********************************
   *****                       *****
   *****  HALF-PEL REFINEMENT  *****
   *****                       *****
   *********************************/

  //===== set function for getting pixel values =====
  if ((pic4_pix_x + *mv_x > 1) && (pic4_pix_x + *mv_x < max_pos_x4 - 1) &&
    (pic4_pix_y + *mv_y > 1) && (pic4_pix_y + *mv_y < max_pos_y4 - 1)   )
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  //===== loop over search positions =====
  for (best_pos = 0, pos = start_me_refinement_hp; pos < max_pos2; pos++)
  {
    cand_mv_x = *mv_x + (spiral_hpel_search_x[pos]);    // quarter-pel units
    cand_mv_y = *mv_y + (spiral_hpel_search_y[pos]);    // quarter-pel units

    //----- set motion vector cost -----
    mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);


    if (mcost >= min_mcost) continue;

    cmv_x = cand_mv_x + pic4_pix_x;
    cmv_y = cand_mv_y + pic4_pix_y;

    mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x, min_mcost - mcost, cmv_x, cmv_y);

    if (pos==0 && check_position0)
    {
      mcost -= WEIGHTED_COST (lambda_factor, 16);
    }

    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      best_pos  = pos;
    }
  }
  if (best_pos)
  {
    *mv_x += (spiral_hpel_search_x [best_pos]);
    *mv_y += (spiral_hpel_search_y [best_pos]);
  }

  if ( !start_me_refinement_qp )
    min_mcost = INT_MAX;



  /************************************
  *****                          *****
  *****  QUARTER-PEL REFINEMENT  *****
  *****                          *****
  ************************************/

  //===== set function for getting pixel values =====
  if ((pic4_pix_x + *mv_x > 0) && (pic4_pix_x + *mv_x < max_pos_x4) &&
    (pic4_pix_y + *mv_y > 0) && (pic4_pix_y + *mv_y < max_pos_y4)   )
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  dist_method = Q_PEL + 3 * apply_weights;
  lambda_factor = lambda[Q_PEL];

  //===== loop over search positions =====
  for (best_pos = 0, pos = start_me_refinement_qp; pos < search_pos4; pos++)
  {
    cand_mv_x = *mv_x + spiral_search_x[pos];    // quarter-pel units
    cand_mv_y = *mv_y + spiral_search_y[pos];    // quarter-pel units

    //----- set motion vector cost -----
    mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);

    if (mcost >= min_mcost) continue;

    cmv_x = cand_mv_x + pic4_pix_x;
    cmv_y = cand_mv_y + pic4_pix_y;

    mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x, min_mcost - mcost, cmv_x, cmv_y);

    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      best_pos  = pos;
    }
  }
  if (best_pos)
  {
    *mv_x += spiral_search_x [best_pos];
    *mv_y += spiral_search_y [best_pos];
  }

  //===== return minimum motion cost =====
  return min_mcost;
}

/*!
***********************************************************************
* \brief
*    Bipred Sub pixel block motion search
***********************************************************************
*/
int                                               //  ==> minimum motion cost after search
SubPelBlockSearchBiPred (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                         short     ref,           // <--  reference frame (0... or -1 (backward))
                         int       list,          // <--  reference picture list
                         int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                         int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                         int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                         short     pred_mv_x,     // <--  motion vector predictor (x) in sub-pel units
                         short     pred_mv_y,     // <--  motion vector predictor (y) in sub-pel units
                         short*    mv_x,          // <--> in: search center (x) / out: motion vector (x) - in pel units
                         short*    mv_y,          // <--> in: search center (y) / out: motion vector (y) - in pel units
                         short*    s_mv_x,        // <--> in: search center (x) / out: motion vector (x) - in pel units
                         short*    s_mv_y,        // <--> in: search center (y) / out: motion vector (y) - in pel units
                         int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                         int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                         int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                         int*      lambda         // <--  lagrangian parameter for determining motion cost
                         )
{
  int   apply_weights =  (active_pps->weighted_bipred_idc > 0);
  int   list_offset   = img->mb_data[img->current_mb_nr].list_offset;

  short offset1 = (apply_weights ? (list == 0?  wp_offset[list_offset    ][ref]     [0]:  wp_offset[list_offset + 1][0  ]     [0]) : 0);
  short offset2 = (apply_weights ? (list == 0?  wp_offset[list_offset + 1][ref]     [0]:  wp_offset[list_offset    ][0  ]     [0]) : 0);

  int   pos, best_pos, mcost;
  int   cand_mv_x, cand_mv_y;

  int   blocksize_x     = input->blc_size[blocktype][0];
  int   blocksize_y     = input->blc_size[blocktype][1];

  int   pic4_pix_x      = ((pic_pix_x + IMG_PAD_SIZE)<< 2);
  int   pic4_pix_y      = ((pic_pix_y + IMG_PAD_SIZE)<< 2);

  int   max_pos2        = ( !start_me_refinement_hp ? imax(1,search_pos2) : search_pos2);
  int   cmv_x, cmv_y;
  int   smv_x = *s_mv_x + pic4_pix_x;
  int   smv_y = *s_mv_y + pic4_pix_y;

  StorablePicture *ref_picture1 = listX[list       + list_offset][ref];
  StorablePicture *ref_picture2 = listX[(list ^ 1) + list_offset][0];

  int max_pos_x4 = ((ref_picture1->size_x - blocksize_x + 2*IMG_PAD_SIZE)<<2);
  int max_pos_y4 = ((ref_picture1->size_y - blocksize_y + 2*IMG_PAD_SIZE)<<2);
  int lambda_factor = lambda[H_PEL];

  ref_pic1_sub.luma = ref_picture1->imgY_sub;
  ref_pic2_sub.luma = ref_picture2->imgY_sub;
  img_width     = ref_picture1->size_x;
  img_height    = ref_picture1->size_y;
  width_pad    = ref_picture1->size_x_pad;
  height_pad   = ref_picture1->size_y_pad;


  if (apply_weights)
  {
    weight1 = list == 0
      ? wbp_weight[list_offset         ][ref][0][0]
      : wbp_weight[list_offset + LIST_1][0  ][ref][0];
    weight2 = list == 0
      ? wbp_weight[list_offset + LIST_1][ref][0][0]
      : wbp_weight[list_offset         ][0  ][ref][0];
    offsetBi=(offset1 + offset2 + 1)>>1;
    computeBiPred = computeBiPred2[H_PEL];
  }
  else
  {
    weight1 = 1<<luma_log_weight_denom;
    weight2 = 1<<luma_log_weight_denom;
    offsetBi = 0;
    computeBiPred = computeBiPred1[H_PEL];
  }


  if ( ChromaMEEnable )
  {
    ref_pic1_sub.crcb[0] = ref_picture1->imgUV_sub[0];
    ref_pic1_sub.crcb[1] = ref_picture1->imgUV_sub[1];
    ref_pic2_sub.crcb[0] = ref_picture2->imgUV_sub[0];
    ref_pic2_sub.crcb[1] = ref_picture2->imgUV_sub[1];
    width_pad_cr  = ref_picture1->size_x_cr_pad;
    height_pad_cr = ref_picture1->size_y_cr_pad;

    if (apply_weights)
    {
      weight1_cr[0] = list == 0
        ? wbp_weight[list_offset         ][ref][0][1]
        : wbp_weight[list_offset + LIST_1][0  ][ref][1];
      weight1_cr[1] = list == 0
        ? wbp_weight[list_offset         ][ref][0][2]
        : wbp_weight[list_offset + LIST_1][0  ][ref][2];
      weight2_cr[0] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][1] : wbp_weight[list_offset         ][0  ][ref][1];
      weight2_cr[1] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][2] : wbp_weight[list_offset         ][0  ][ref][2];
      offsetBi_cr[0] = (list == 0)
        ? (wp_offset[list_offset         ][ref][1] + wp_offset[list_offset + LIST_1][ref][1] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][1] + wp_offset[list_offset         ][0  ][1] + 1) >> 1;
      offsetBi_cr[1] = (list == 0)
        ? (wp_offset[list_offset         ][ref][2] + wp_offset[list_offset + LIST_1][ref][2] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][2] + wp_offset[list_offset         ][0  ][2] + 1) >> 1;
    }
    else
    {
      weight1_cr[0] = 1<<chroma_log_weight_denom;
      weight1_cr[1] = 1<<chroma_log_weight_denom;
      weight2_cr[0] = 1<<chroma_log_weight_denom;
      weight2_cr[1] = 1<<chroma_log_weight_denom;
      offsetBi_cr[0] = 0;
      offsetBi_cr[1] = 0;
    }
  }


  /*********************************
   *****                       *****
   *****  HALF-PEL REFINEMENT  *****
   *****                       *****
   *********************************/

  //===== set function for getting pixel values =====
  if ((pic4_pix_x + *mv_x > 1) && (pic4_pix_x + *mv_x < max_pos_x4 - 1) &&
    (pic4_pix_y + *mv_y > 1) && (pic4_pix_y + *mv_y < max_pos_y4 - 1))
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
    bipred2_access_method = UMV_ACCESS;
  }

  if ((pic4_pix_x + *s_mv_x > 1) && (pic4_pix_x + *s_mv_x < max_pos_x4 - 1) &&
    (pic4_pix_y + *s_mv_y > 1) && (pic4_pix_y + *s_mv_y < max_pos_y4 - 1))
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  //===== loop over search positions =====
  for (best_pos = 0, pos = start_me_refinement_hp; pos < max_pos2; pos++)
  {
    cand_mv_x = *mv_x + (spiral_hpel_search_x[pos]);    // quarter-pel units
    cand_mv_y = *mv_y + (spiral_hpel_search_y[pos]);    // quarter-pel units

    //----- set motion vector cost -----
    mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);

    if (mcost >= min_mcost) continue;

    cmv_x = cand_mv_x + pic4_pix_x;
    cmv_y = cand_mv_y + pic4_pix_y;

    mcost += computeBiPred(orig_pic, blocksize_y, blocksize_x,
      min_mcost - mcost, smv_x, smv_y, cmv_x, cmv_y);

    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      best_pos  = pos;
    }
  }

  if (best_pos)
  {
    *mv_x += (spiral_hpel_search_x [best_pos]);
    *mv_y += (spiral_hpel_search_y [best_pos]);
  }

  computeBiPred = apply_weights? computeBiPred2[Q_PEL] : computeBiPred1[Q_PEL];

  /************************************
  *****                          *****
  *****  QUARTER-PEL REFINEMENT  *****
  *****                          *****
  ************************************/
  //===== set function for getting pixel values =====
  if ((pic4_pix_x + *mv_x > 0) && (pic4_pix_x + *mv_x < max_pos_x4) &&
    (pic4_pix_y + *mv_y > 0) && (pic4_pix_y + *mv_y < max_pos_y4))
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
    bipred2_access_method = UMV_ACCESS;
  }

  if ((pic4_pix_x + *s_mv_x > 0) && (pic4_pix_x + *s_mv_x < max_pos_x4) &&
    (pic4_pix_y + *s_mv_y > 0) && (pic4_pix_y + *s_mv_y < max_pos_y4))
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  if ( !start_me_refinement_qp )
    min_mcost = INT_MAX;

  lambda_factor = lambda[Q_PEL];

  //===== loop over search positions =====
  for (best_pos = 0, pos = start_me_refinement_qp; pos < search_pos4; pos++)
  {
    cand_mv_x = *mv_x + spiral_search_x[pos];    // quarter-pel units
    cand_mv_y = *mv_y + spiral_search_y[pos];    // quarter-pel units

    //----- set motion vector cost -----
    mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);

    if (mcost >= min_mcost) continue;
    cmv_x = cand_mv_x + pic4_pix_x;
    cmv_y = cand_mv_y + pic4_pix_y;

    mcost += computeBiPred(orig_pic, blocksize_y, blocksize_x,
      min_mcost - mcost, smv_x, smv_y, cmv_x, cmv_y);

    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      best_pos  = pos;
    }

  }

  if (best_pos)
  {
    *mv_x += spiral_search_x [best_pos];
    *mv_y += spiral_search_y [best_pos];
  }

  //===== return minimum motion cost =====
  return min_mcost;
}
