
/*!
 *************************************************************************************
 *
 * \file me_umhexsmp.c
 *
 * \brief
 *   Fast integer pixel and sub pixel motion estimation
 *   Improved and simplified from the original UMHexagonS algorithms
 *   See JVT-P021 for details
 *
 * \author
 *    Main contributors: (see contributors.h for copyright, address and affiliation details)
 *    - Zhibo Chen                      <chenzhibo@tsinghua.org.cn>
 *    - JianFeng Xu                     <fenax@video.mdc.tsinghua.edu.cn>
 *    - Wenfang Fu                      <fwf@video.mdc.tsinghua.edu.cn>
 *
 *    - Xiaoquan Yi                     <xyi@engr.scu.edu>
 *    - Jun Zhang                       <jzhang2@engr.scu.edu>
 *
 * \date
 *    6. Nov. 2006
 *************************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "global.h"
#include "memalloc.h"
#include "me_umhexsmp.h"
#include "refbuf.h"
#include "me_distortion.h"

extern  int *byte_abs;
extern  int *mvbits;

static const short Diamond_X[4]      = {-1, 1, 0, 0};
static const short Diamond_Y[4]      = { 0, 0,-1, 1};
static const short Hexagon_X[6]      = {-2, 2,-1, 1,-1, 1};
static const short Hexagon_Y[6]      = { 0, 0,-2, 2, 2,-2};
static const short Big_Hexagon_X[16] = {-4, 4, 0, 0,-4, 4,-4, 4,-4, 4,-4, 4,-2, 2,-2, 2};
static const short Big_Hexagon_Y[16] = { 0, 0,-4, 4,-1, 1, 1,-1,-2, 2, 2,-2,-3, 3, 3,-3};

const short block_type_shift_factor[8] = {0, 0, 1, 1, 2, 3, 3, 1}; // last one relaxed to 1 instead 4

static StorablePicture *ref_pic_ptr;
static int dist_method;

extern short*  spiral_hpel_search_x;
extern short*  spiral_hpel_search_y;
extern short*  spiral_search_x;
extern short*  spiral_search_y;

// Macro for motion estimation cost computation per match
#define SEARCH_ONE_PIXEL_HELPER                                                           \
  if(iabs(cand_x - center_x) <= search_range && iabs(cand_y - center_y) <= search_range)  \
  {                                                                                       \
    mcost = MV_COST (lambda_factor, mvshift, cand_x, cand_y, pred_x, pred_y);             \
    mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,             \
          min_mcost - mcost, (cand_x + IMG_PAD_SIZE) << 2, (cand_y + IMG_PAD_SIZE) << 2); \
    if (mcost < min_mcost)                                                                \
    {                                                                                     \
      best_x    = cand_x;                                                                 \
      best_y    = cand_y;                                                                 \
      min_mcost = mcost;                                                                  \
    }                                                                                     \
}

#define SEARCH_ONE_PIXEL_BIPRED_HELPER                                                    \
if (iabs(cand_x - center2_x) <= search_range && iabs(cand_y - center2_y) <= search_range) \
{                                                                                         \
  mcost  = MV_COST (lambda_factor, mvshift, center1_x, center1_y, pred_x1, pred_y1);      \
  mcost += MV_COST (lambda_factor, mvshift, cand_x,    cand_y,    pred_x2, pred_y2);      \
  if (mcost < min_mcost)                                                                  \
  {                                                                                       \
    mcost  += computeBiPred(cur_pic, blocksize_y, blocksize_x,                            \
                           min_mcost - mcost,                                             \
                           (center1_x << 2) + IMG_PAD_SIZE_TIMES4,                        \
                           (center1_y << 2) + IMG_PAD_SIZE_TIMES4,                        \
                           (cand_x << 2) + IMG_PAD_SIZE_TIMES4,                           \
                           (cand_y << 2) + IMG_PAD_SIZE_TIMES4);                          \
    if (mcost < min_mcost)                                                                \
    {                                                                                     \
      best_x = cand_x;                                                                    \
      best_y = cand_y;                                                                    \
      min_mcost = mcost;                                                                  \
    }                                                                                     \
  }                                                                                       \
}

/*!
 ************************************************************************
 * \brief
 *    Set thresholds for fast motion estimation
 *    Those thresholds may be adjusted to trade off rate-distortion
 *    performance and simplified UMHEX speed
 ************************************************************************
 */
void smpUMHEX_init()
{
  SymmetricalCrossSearchThreshold1 =  800;
  SymmetricalCrossSearchThreshold2 = 7000;
  ConvergeThreshold                = 1000;
  SubPelThreshold1                 = 1000;
  SubPelThreshold3                 =  400;
}

/*!
 ************************************************************************
 * \brief
 *    Allocation of space for fast motion estimation
 ************************************************************************
 */
int smpUMHEX_get_mem()
{
  int memory_size = 0;
  if (NULL==(smpUMHEX_flag_intra = calloc((img->width>>4)+1, sizeof(byte))))
    no_mem_exit("smpUMHEX_get_mem: smpUMHEX_flag_intra");

  memory_size += get_mem3Dint(&smpUMHEX_l0_cost, 9, img->height/4, img->width/4);
  memory_size += get_mem3Dint(&smpUMHEX_l1_cost, 9, img->height/4, img->width/4);
  memory_size += get_mem2D(&smpUMHEX_SearchState, 7, 7);

  return memory_size;
}

/*!
 ************************************************************************
 * \brief
 *    Free space for fast motion estimation
 ************************************************************************
 */
void smpUMHEX_free_mem()
{
  free_mem3Dint(smpUMHEX_l0_cost, 9);
  free_mem3Dint(smpUMHEX_l1_cost, 9);
  free_mem2D(smpUMHEX_SearchState);

  free (smpUMHEX_flag_intra);
}


/*!
************************************************************************
* \brief
*    Fast integer pixel block motion estimation
************************************************************************
*/
int                                     //  ==> minimum motion cost after search
smpUMHEXIntegerPelBlockMotionSearch (
                                     imgpel   *orig_pic,      // <--  not used
                                     short     ref,           // <--  reference frame (0... or -1 (backward))
                                     int       list,          // <--  reference picture list
                                     int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                                     int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                                     int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                                     short     pred_mv_x,     // <--  motion vector predictor (x) in sub-pel units
                                     short     pred_mv_y,     // <--  motion vector predictor (y) in sub-pel units
                                     short*    mv_x,          //  --> motion vector (x) - in pel units
                                     short*    mv_y,          //  --> motion vector (y) - in pel units
                                     int       search_range,  // <--  1-d search range in pel units
                                     int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                                     int       lambda_factor) // <--  lagrangian parameter for determining motion cost
{
  int   list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))?
                         img->current_mb_nr%2 ? 4 : 2 : 0;
  int   mvshift       = 2;                                        // motion vector shift for getting sub-pel units
  int   blocksize_y   = input->blc_size[blocktype][1];            // vertical block size
  int   blocksize_x   = input->blc_size[blocktype][0];            // horizontal block size
  int   pred_x        = (pic_pix_x << mvshift) + pred_mv_x;       // predicted position x (in sub-pel units)
  int   pred_y        = (pic_pix_y << mvshift) + pred_mv_y;       // predicted position y (in sub-pel units)
  int   center_x      = pic_pix_x + *mv_x;                        // center position x (in pel units)
  int   center_y      = pic_pix_y + *mv_y;                        // center position y (in pel units)
  int   best_x        = 0, best_y = 0;
  int   search_step, iYMinNow, iXMinNow;
  int   cand_x, cand_y, mcost;

  unsigned short        i, m;


  //===== Use weighted Reference for ME ====

  int  apply_weights = ( (active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;

  dist_method = F_PEL + 3 * apply_weights;

  ref_pic_ptr = listX[list+list_offset][ref];

  // Note that following seem to be universal for all functions and could be moved to a separate, clean public function in me_distortion.c
  ref_pic_sub.luma = ref_pic_ptr->imgY_sub;
  img_width  = ref_pic_ptr->size_x;
  img_height = ref_pic_ptr->size_y;
  width_pad  = ref_pic_ptr->size_x_pad;
  height_pad = ref_pic_ptr->size_y_pad;

  if (apply_weights)
  {
    weight_luma = wp_weight[list + list_offset][ref][0];
    offset_luma = wp_offset[list + list_offset][ref][0];
  }

  if (ChromaMEEnable)
  {
    ref_pic_sub.crcb[0] = ref_pic_ptr->imgUV_sub[0];
    ref_pic_sub.crcb[1] = ref_pic_ptr->imgUV_sub[1];
    width_pad_cr  = ref_pic_ptr->size_x_cr_pad;
    height_pad_cr = ref_pic_ptr->size_y_cr_pad;

    if (apply_weights)
    {
      weight_cr[0] = wp_weight[list + list_offset][ref][1];
      weight_cr[1] = wp_weight[list + list_offset][ref][2];
      offset_cr[0] = wp_offset[list + list_offset][ref][1];
      offset_cr[1] = wp_offset[list + list_offset][ref][2];
    }
  }

  //===== set function for getting reference picture lines =====
  if ((center_x > search_range) && (center_x < img_width - 1 - search_range - blocksize_x) &&
    (center_y > search_range) && (center_y < img_height - 1 - search_range - blocksize_y))
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  //check the center median predictor
  cand_x = center_x ;
  cand_y = center_y ;
  mcost = MV_COST (lambda_factor, mvshift, cand_x, cand_y, pred_x, pred_y);


  mcost += computeUniPred[dist_method](orig_pic, blocksize_y,blocksize_x, min_mcost - mcost,
    (cand_x << 2) + IMG_PAD_SIZE_TIMES4,  (cand_y << 2) + IMG_PAD_SIZE_TIMES4);

  if (mcost < min_mcost)
  {
    min_mcost = mcost;
    best_x    = cand_x;
    best_y    = cand_y;
  }

  iXMinNow = best_x;
  iYMinNow = best_y;
  if ((0 != pred_mv_x) || (0 != pred_mv_y))
  {
    cand_x = pic_pix_x;
    cand_y = pic_pix_y;
    SEARCH_ONE_PIXEL_HELPER
  }

  // If the min_mcost is small enough, do a local search then terminate
  // Ihis is good for stationary or quasi-stationary areas
  if (min_mcost < (ConvergeThreshold>>block_type_shift_factor[blocktype]))
  {
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_HELPER
    }
    *mv_x = (short) (best_x - pic_pix_x);
    *mv_y = (short) (best_y - pic_pix_y);
    return min_mcost;
  }

  // Small local search
  for (m = 0; m < 4; m++)
  {
    cand_x = iXMinNow + Diamond_X[m];
    cand_y = iYMinNow + Diamond_Y[m];
    SEARCH_ONE_PIXEL_HELPER
  }

  // First_step: Symmetrical-cross search
  // If distortion is large, use large shapes. Otherwise, compact shapes are faster
  if ( (blocktype == 1 &&
    min_mcost > (SymmetricalCrossSearchThreshold1>>block_type_shift_factor[blocktype])) ||
    (min_mcost > (SymmetricalCrossSearchThreshold2>>block_type_shift_factor[blocktype])) )
  {
    iXMinNow = best_x;
    iYMinNow = best_y;

    for(i = 1; i <= search_range/2; i++)
    {
      search_step = (i<<1) - 1;
      cand_x = iXMinNow + search_step;
      cand_y = iYMinNow;
      SEARCH_ONE_PIXEL_HELPER

      cand_x = iXMinNow - search_step;
      SEARCH_ONE_PIXEL_HELPER

      cand_x = iXMinNow;
      cand_y = iYMinNow + search_step;
      SEARCH_ONE_PIXEL_HELPER

      cand_y = iYMinNow - search_step;
      SEARCH_ONE_PIXEL_HELPER
    }

    // Hexagon Search
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 6; m++)
    {
      cand_x = iXMinNow + Hexagon_X[m];
      cand_y = iYMinNow + Hexagon_Y[m];
      SEARCH_ONE_PIXEL_HELPER
    }
    // Multi Big Hexagon Search
    iXMinNow = best_x;
    iYMinNow = best_y;
    for(i = 1; i <= search_range/4; i++)
    {
      for (m = 0; m < 16; m++)
      {
        cand_x = iXMinNow + Big_Hexagon_X[m]*i;
        cand_y = iYMinNow + Big_Hexagon_Y[m]*i;
        SEARCH_ONE_PIXEL_HELPER
      }
    }
  }

  // Search up_layer predictor for non 16x16 blocks
  if (blocktype > 1)
  {
    cand_x = pic_pix_x + (smpUMHEX_pred_MV_uplayer_X/4);
    cand_y = pic_pix_y + (smpUMHEX_pred_MV_uplayer_Y/4);
    SEARCH_ONE_PIXEL_HELPER
  }

  if(center_x != pic_pix_x || center_y != pic_pix_y)
  {
    cand_x = pic_pix_x;
    cand_y = pic_pix_y;
    SEARCH_ONE_PIXEL_HELPER

    iXMinNow = best_x;
    iYMinNow = best_y;
    // Local diamond search
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_HELPER
    }
  }

  // If the minimum cost is small enough, do a local search
  // and finish the search here
  if (min_mcost < (ConvergeThreshold>>block_type_shift_factor[blocktype]))
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_HELPER
    }
    *mv_x = (short) (best_x - pic_pix_x);
    *mv_y = (short) (best_y - pic_pix_y);
    return min_mcost;
  }

  //second_step:  Extended Hexagon-based Search
  for(i = 0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 6; m++)
    {
      cand_x = iXMinNow + Hexagon_X[m];
      cand_y = iYMinNow + Hexagon_Y[m];
      SEARCH_ONE_PIXEL_HELPER
    }
    // The minimum cost point happens in the center
    if (best_x == iXMinNow && best_y == iYMinNow)
    {
      break;
    }
  }

  //third_step: Small diamond search
  for(i = 0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_HELPER
    }

    // The minimum cost point happens in the center
    if (best_x == iXMinNow && best_y == iYMinNow)
    {
      break;
    }
  }

  *mv_x = (short) (best_x - pic_pix_x);
  *mv_y = (short) (best_y - pic_pix_y);
  return min_mcost;
}

/*!
 ***********************************************************************
 * \brief
 *    Sub pixel block motion search enhanced
 ***********************************************************************
 */
int                                               //  ==> minimum motion cost after search
smpUMHEXFullSubPelBlockMotionSearch (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
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
                                     int       lambda_factor  // <--  lagrangian parameter for determining motion cost
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
  int  apply_weights = ( (active_pps->weighted_pred_flag  &&
    (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE)) )
    && input->UseWeightedReferenceME;
  int   cmv_x, cmv_y;

  StorablePicture *ref_picture = listX[list+list_offset][ref];

  int max_pos_x4 = ((ref_picture->size_x - blocksize_x + 2*IMG_PAD_SIZE)<<2);
  int max_pos_y4 = ((ref_picture->size_y - blocksize_y + 2*IMG_PAD_SIZE)<<2);

  dist_method = Q_PEL + 3 * apply_weights;

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
    if (min_mcost < (SubPelThreshold3>>block_type_shift_factor[blocktype]))
    {
      break;
    }
  }

  if (best_pos)
  {
    *mv_x += (spiral_hpel_search_x [best_pos]);
    *mv_y += (spiral_hpel_search_y [best_pos]);
  }

  if ((*mv_x == 0) && (*mv_y == 0) && (pred_mv_x == 0 && pred_mv_y == 0) &&
    (min_mcost < (SubPelThreshold1>>block_type_shift_factor[blocktype])) )
  {
    best_pos = 0;
    return min_mcost;
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
    if (min_mcost < (SubPelThreshold3>>block_type_shift_factor[blocktype]))
    {
      break;
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
 ************************************************************************
 * \brief
 *    Fast sub pixel block motion estimation
 ************************************************************************
 */
int                                     //  ==> minimum motion cost after search
smpUMHEXSubPelBlockMotionSearch  (
                                  imgpel* orig_pic,        // <--  original pixel values for the AxB block
                                  short     ref,           // <--  reference frame (0... or -1 (backward))
                                  int       list,          // <--  reference picture list
                                  int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                                  int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                                  int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                                  short     pred_mv_x,     // <--  motion vector predictor (x) in sub-pel units
                                  short     pred_mv_y,     // <--  motion vector predictor (y) in sub-pel units
                                  short*    mv_x,          // <--> in: search center (x) / out: MV (x) - in pel units
                                  short*    mv_y,          // <--> in: search center (y) / out: MV (y) - in pel units
                                  int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                                  int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                                  int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                                  int       lambda_factor) // <--  lagrangian parameter for determining motion cost
{
  int   mcost;
  int   cand_mv_x, cand_mv_y;

  int   list_offset     = ((img->MbaffFrameFlag) &&
    (img->mb_data[img->current_mb_nr].mb_field)) ?
    img->current_mb_nr%2 ? 4 : 2 : 0;
  StorablePicture *ref_picture = listX[list+list_offset][ref];

  short mv_shift        = 0;
  short blocksize_x     = (short) input->blc_size[blocktype][0];
  short blocksize_y     = (short) input->blc_size[blocktype][1];
  int   pic4_pix_x      = ((pic_pix_x + IMG_PAD_SIZE)<<2);
  int   pic4_pix_y      = ((pic_pix_y + IMG_PAD_SIZE)<<2);
  short max_pos_x4      = (short) ((ref_picture->size_x - blocksize_x + 2*IMG_PAD_SIZE)<<2);
  short max_pos_y4      = (short) ((ref_picture->size_y - blocksize_y + 2*IMG_PAD_SIZE)<<2);

  int   iXMinNow, iYMinNow;
  short dynamic_search_range, i, m;
  int   currmv_x = 0, currmv_y = 0;
  int   pred_frac_mv_x,pred_frac_mv_y,abort_search;
  int   pred_frac_up_mv_x, pred_frac_up_mv_y;
  int   apply_weights = ( (active_pps->weighted_pred_flag  &&
    (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE)) )
    && input->UseWeightedReferenceME;

  dist_method = Q_PEL + 3 * apply_weights;
  ref_pic_sub.luma = ref_pic_ptr->imgY_sub;
  img_width  = ref_pic_ptr->size_x;
  img_height = ref_pic_ptr->size_y;
  width_pad  = ref_pic_ptr->size_x_pad;
  height_pad = ref_pic_ptr->size_y_pad;

  if (apply_weights)
  {
    weight_luma = wp_weight[list + list_offset][ref][0];
    offset_luma = wp_offset[list + list_offset][ref][0];
  }

  if (ChromaMEEnable)
  {
    ref_pic_sub.crcb[0] = ref_pic_ptr->imgUV_sub[0];
    ref_pic_sub.crcb[1] = ref_pic_ptr->imgUV_sub[1];
    width_pad_cr  = ref_pic_ptr->size_x_cr_pad;
    height_pad_cr = ref_pic_ptr->size_y_cr_pad;

    if (apply_weights)
    {
      weight_cr[0] = wp_weight[list + list_offset][ref][1];
      weight_cr[1] = wp_weight[list + list_offset][ref][2];
      offset_cr[0] = wp_offset[list + list_offset][ref][1];
      offset_cr[1] = wp_offset[list + list_offset][ref][2];
    }
  }


  if ((pic4_pix_x + *mv_x > 1) && (pic4_pix_x + *mv_x < max_pos_x4 - 1) &&
    (pic4_pix_y + *mv_y > 1) && (pic4_pix_y + *mv_y < max_pos_y4 - 1))
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  dynamic_search_range = 3;
  pred_frac_mv_x = (pred_mv_x - *mv_x) % 4;
  pred_frac_mv_y = (pred_mv_y - *mv_y) % 4;

  pred_frac_up_mv_x = (smpUMHEX_pred_MV_uplayer_X - *mv_x) % 4;
  pred_frac_up_mv_y = (smpUMHEX_pred_MV_uplayer_Y - *mv_y) % 4;

  memset(smpUMHEX_SearchState[0], 0,
    (2*dynamic_search_range+1)*(2*dynamic_search_range+1));

  smpUMHEX_SearchState[dynamic_search_range][dynamic_search_range] = 1;
  if( !start_me_refinement_hp )
  {
    cand_mv_x = *mv_x;
    cand_mv_y = *mv_y;
    mcost   = MV_COST (lambda_factor, mv_shift, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);
    mcost   += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
      min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);
    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      currmv_x  = cand_mv_x;
      currmv_y  = cand_mv_y;
    }
  }
  else
  {
    currmv_x = *mv_x;
    currmv_y = *mv_y;
  }

  // If the min_mcost is small enough and other statistics are positive,
  // better to stop the search now
  if ( ((*mv_x) == 0) && ((*mv_y) == 0) &&
    (pred_frac_mv_x == 0 && pred_frac_up_mv_x == 0) &&
    (pred_frac_mv_y == 0 && pred_frac_up_mv_y == 0) &&
    (min_mcost < (SubPelThreshold1>>block_type_shift_factor[blocktype])) )
  {
    *mv_x = (short) currmv_x;
    *mv_y = (short) currmv_y;
    return min_mcost;
  }

  if(pred_frac_mv_x || pred_frac_mv_y)
  {
    cand_mv_x = *mv_x + pred_frac_mv_x;
    cand_mv_y = *mv_y + pred_frac_mv_y;
    mcost   = MV_COST (lambda_factor, mv_shift, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);
    mcost   += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
      min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

    smpUMHEX_SearchState[cand_mv_y -*mv_y + dynamic_search_range][cand_mv_x - *mv_x + dynamic_search_range] = 1;
    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      currmv_x  = cand_mv_x;
      currmv_y  = cand_mv_y;
    }
  }

  // Multiple small diamond search
  for(i = 0; i < dynamic_search_range; i++)
  {
    abort_search = 1;

    iXMinNow = currmv_x;
    iYMinNow = currmv_y;
    for (m = 0; m < 4; m++)
    {
      cand_mv_x = iXMinNow + Diamond_X[m];
      cand_mv_y = iYMinNow + Diamond_Y[m];

      if(iabs(cand_mv_x - *mv_x) <= dynamic_search_range && iabs(cand_mv_y - *mv_y) <= dynamic_search_range)
      {
        if(!smpUMHEX_SearchState[cand_mv_y - *mv_y + dynamic_search_range][cand_mv_x - *mv_x + dynamic_search_range])
        {
          mcost = MV_COST (lambda_factor, mv_shift, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);
          mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
            min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

          smpUMHEX_SearchState[cand_mv_y - *mv_y + dynamic_search_range][cand_mv_x - *mv_x + dynamic_search_range] = 1;

          if (mcost < min_mcost)
          {
            min_mcost    = mcost;
            currmv_x     = cand_mv_x;
            currmv_y     = cand_mv_y;
            abort_search = 0;
          }
          if (min_mcost < (SubPelThreshold3>>block_type_shift_factor[blocktype]))
          {
            *mv_x = (short) currmv_x;
            *mv_y = (short) currmv_y;
            return min_mcost;
          }
        }
      }
    }
    // If the minimum cost point is in the center, break out the loop
    if (abort_search)
    {
      break;
    }
  }

  *mv_x = (short) currmv_x;
  *mv_y = (short) currmv_y;
  return min_mcost;
}

/*!
 ************************************************************************
 * \brief
 *    smpUMHEXBipredIntegerPelBlockMotionSearch: fast pixel block motion search for bipred mode
 *
 ************************************************************************
 */
int                                                           //  ==> minimum motion cost after search
smpUMHEXBipredIntegerPelBlockMotionSearch (imgpel* cur_pic,  // <--  original pixel values for the AxB block
                                     short     ref,           // <--  reference frame (0... or -1 (backward))
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
  int   mvshift       = 2;                              // motion vector shift for getting sub-pel units

  int   search_step, iYMinNow, iXMinNow;
  int   i, m;
  int   cand_x, cand_y, mcost;
  int   list_offset   = img->mb_data[img->current_mb_nr].list_offset;
  int   blocksize_y   = input->blc_size[blocktype][1];  // vertical block size
  int   blocksize_x   = input->blc_size[blocktype][0];  // horizontal block size
  int   pred_x1       = (pic_pix_x << 2) + pred_mv_x1;  // predicted position x (in sub-pel units)
  int   pred_y1       = (pic_pix_y << 2) + pred_mv_y1;  // predicted position y (in sub-pel units)
  int   pred_x2       = (pic_pix_x << 2) + pred_mv_x2;  // predicted position x (in sub-pel units)
  int   pred_y2       = (pic_pix_y << 2) + pred_mv_y2;  // predicted position y (in sub-pel units)
  short center2_x     = pic_pix_x + *mv_x;              // center position x (in pel units)
  short center2_y     = pic_pix_y + *mv_y;              // center position y (in pel units)
  short center1_x     = pic_pix_x + *s_mv_x;            // mvx of second pred (in pel units)
  short center1_y     = pic_pix_y + *s_mv_y;            // mvy of second pred (in pel units)
  int   best_x        = center2_x;
  int   best_y        = center2_y;

  short apply_weights = (active_pps->weighted_bipred_idc>0);
  short offset1 = (apply_weights ? (list == 0?  wp_offset[list_offset    ][ref][0]:  wp_offset[list_offset + 1][0  ][ref]) : 0);
  short offset2 = (apply_weights ? (list == 0?  wp_offset[list_offset + 1][ref][0]:  wp_offset[list_offset    ][0  ][ref]) : 0);

  ref_pic1_sub.luma = listX[list + list_offset][ref]->imgY_sub;
  ref_pic2_sub.luma = listX[list == 0 ? 1 + list_offset: list_offset][ 0 ]->imgY_sub;
  img_width  = listX[list + list_offset][ref]->size_x;
  img_height = listX[list + list_offset][ref]->size_y;
  width_pad  = listX[list + list_offset][ref]->size_x_pad;
  height_pad = listX[list + list_offset][ref]->size_y_pad;

  if (apply_weights)
  {
    weight1 = list == 0 ? wbp_weight[list_offset         ][ref][0][0] : wbp_weight[list_offset + LIST_1][0  ][ref][0];
    weight2 = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][0] : wbp_weight[list_offset         ][0  ][ref][0];
    offsetBi=(offset1 + offset2 + 1)>>1;
    computeBiPred = computeBiPredSAD2; //ME only supports SAD computations
  }
  else
  {
    weight1 = 1<<luma_log_weight_denom;
    weight2 = 1<<luma_log_weight_denom;
    offsetBi = 0;
    computeBiPred = computeBiPredSAD1; //ME only supports SAD computations
  }

  if (ChromaMEEnable )
  {
    ref_pic1_sub.crcb[0] = listX[list + list_offset][ref]->imgUV_sub[0];
    ref_pic1_sub.crcb[1] = listX[list + list_offset][ref]->imgUV_sub[1];
    ref_pic2_sub.crcb[0] = listX[list == 0 ? 1 + list_offset: list_offset][ 0 ]->imgUV_sub[0];
    ref_pic2_sub.crcb[1] = listX[list == 0 ? 1 + list_offset: list_offset][ 0 ]->imgUV_sub[1];
    width_pad_cr  = listX[list + list_offset][ref]->size_x_cr_pad;
    height_pad_cr = listX[list + list_offset][ref]->size_y_cr_pad;
    if (apply_weights)
    {
      weight1_cr[0] = list == 0 ? wbp_weight[list_offset         ][ref][0][1] : wbp_weight[list_offset + LIST_1][0  ][ref][1];
      weight1_cr[1] = list == 0 ? wbp_weight[list_offset         ][ref][0][2] : wbp_weight[list_offset + LIST_1][0  ][ref][2];
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

  // Set function for getting reference picture lines
  if ((center2_x > search_range) && (center2_x < img_width -1-search_range-blocksize_x) &&
      (center2_y > search_range) && (center2_y < img_height-1-search_range-blocksize_y))
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
    bipred2_access_method = UMV_ACCESS;
  }

  // Set function for getting reference picture lines
  if ((center1_y > search_range) && (center1_y < img_height-1-search_range-blocksize_y))
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  // Check the center median predictor
  cand_x = center2_x ;
  cand_y = center2_y ;
  mcost  = MV_COST (lambda_factor, mvshift, center1_x, center1_y, pred_x1, pred_y1);
  mcost += MV_COST (lambda_factor, mvshift, cand_x,    cand_y,    pred_x2, pred_y2);

  mcost += computeBiPred(cur_pic,
                        blocksize_y, blocksize_x, INT_MAX,
                        (center1_x << 2) + IMG_PAD_SIZE_TIMES4,
                        (center1_y << 2) + IMG_PAD_SIZE_TIMES4,
                        (cand_x << 2) + IMG_PAD_SIZE_TIMES4,
                        (cand_y << 2) + IMG_PAD_SIZE_TIMES4);

  if (mcost < min_mcost)
  {
    min_mcost = mcost;
    best_x = cand_x;
    best_y = cand_y;
  }

  iXMinNow = best_x;
  iYMinNow = best_y;
  if (0 != pred_mv_x1 || 0 != pred_mv_y1 || 0 != pred_mv_x2 || 0 != pred_mv_y2)
  {
    cand_x = pic_pix_x;
    cand_y = pic_pix_y;
    SEARCH_ONE_PIXEL_BIPRED_HELPER;
  }

  // If the min_mcost is small enough, do a local search then terminate
  // This is good for stationary or quasi-stationary areas
  if ((min_mcost<<3) < (ConvergeThreshold>>(block_type_shift_factor[blocktype])))
  {
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_BIPRED_HELPER;
    }
    *mv_x = best_x - pic_pix_x;
    *mv_y = best_y - pic_pix_y;
    return min_mcost;
  }

  // Small local search
  for (m = 0; m < 4; m++)
  {
    cand_x = iXMinNow + Diamond_X[m];
    cand_y = iYMinNow + Diamond_Y[m];
    SEARCH_ONE_PIXEL_BIPRED_HELPER;
  }

  // First_step: Symmetrical-cross search
  // If distortion is large, use large shapes. Otherwise, compact shapes are faster
  if ((blocktype == 1 &&
    (min_mcost<<2) > (SymmetricalCrossSearchThreshold1>>block_type_shift_factor[blocktype])) ||
    ((min_mcost<<2) > (SymmetricalCrossSearchThreshold2>>block_type_shift_factor[blocktype])))
  {
    iXMinNow = best_x;
    iYMinNow = best_y;

    for (i = 1; i <= search_range / 2; i++)
    {
      search_step = (i<<1) - 1;
      cand_x = iXMinNow + search_step;
      cand_y = iYMinNow;
      SEARCH_ONE_PIXEL_BIPRED_HELPER

      cand_x = iXMinNow - search_step;
      SEARCH_ONE_PIXEL_BIPRED_HELPER

      cand_x = iXMinNow;
      cand_y = iYMinNow + search_step;
      SEARCH_ONE_PIXEL_BIPRED_HELPER

      cand_y = iYMinNow - search_step;
      SEARCH_ONE_PIXEL_BIPRED_HELPER
    }

    // Hexagon Search
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 6; m++)
    {
      cand_x = iXMinNow + Hexagon_X[m];
      cand_y = iYMinNow + Hexagon_Y[m];
      SEARCH_ONE_PIXEL_BIPRED_HELPER
    }
    // Multi Big Hexagon Search
    iXMinNow = best_x;
    iYMinNow = best_y;
    for(i = 1; i <= search_range / 4; i++)
    {
      for (m = 0; m < 16; m++)
      {
        cand_x = iXMinNow + Big_Hexagon_X[m] * i;
        cand_y = iYMinNow + Big_Hexagon_Y[m] * i;
        SEARCH_ONE_PIXEL_BIPRED_HELPER
      }
    }
  }

  // Search up_layer predictor for non 16x16 blocks
  if (blocktype > 1)
  {
    cand_x = pic_pix_x + (smpUMHEX_pred_MV_uplayer_X / 4);
    cand_y = pic_pix_y + (smpUMHEX_pred_MV_uplayer_Y / 4);
    SEARCH_ONE_PIXEL_BIPRED_HELPER
  }

  if(center2_x != pic_pix_x || center2_y != pic_pix_y)
  {
    cand_x = pic_pix_x;
    cand_y = pic_pix_y;
    SEARCH_ONE_PIXEL_BIPRED_HELPER

    iXMinNow = best_x;
    iYMinNow = best_y;
    // Local diamond search
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_BIPRED_HELPER
    }
  }

  // If the minimum cost is small enough, do a local search
  // and finish the search here
  if ((min_mcost<<2) < (ConvergeThreshold>>block_type_shift_factor[blocktype]))
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_BIPRED_HELPER
    }
    *mv_x = (short) (best_x - pic_pix_x);
    *mv_y = (short) (best_y - pic_pix_y);
    return min_mcost;
  }

  // Second_step:  Extended Hexagon-based Search
  for (i = 0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 6; m++)
    {
      cand_x = iXMinNow + Hexagon_X[m];
      cand_y = iYMinNow + Hexagon_Y[m];
      SEARCH_ONE_PIXEL_BIPRED_HELPER
    }
    // The minimum cost point happens in the center
    if (best_x == iXMinNow && best_y == iYMinNow)
    {
      break;
    }
  }

  // Third_step: Small diamond search
  for (i = 0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_X[m];
      cand_y = iYMinNow + Diamond_Y[m];
      SEARCH_ONE_PIXEL_BIPRED_HELPER
    }

    // The minimum cost point happens in the center
    if (best_x == iXMinNow && best_y == iYMinNow)
    {
      break;
    }
  }

  *mv_x = (short) (best_x - pic_pix_x);
  *mv_y = (short) (best_y - pic_pix_y);
  return min_mcost;
}

/*!
 ************************************************************************
 * \brief
 *    Set neighbouring block mode (intra/inter)
 *    used for fast motion estimation
 ************************************************************************
 */
void smpUMHEX_decide_intrabk_SAD()
{
  if (img->type != I_SLICE)
  {
    if (img->pix_x == 0 && img->pix_y == 0)
    {
      smpUMHEX_flag_intra_SAD = 0;
    }
    else if (img->pix_x == 0)
    {
      smpUMHEX_flag_intra_SAD = smpUMHEX_flag_intra[(img->pix_x)>>4];
    }
    else if (img->pix_y == 0)
    {
      smpUMHEX_flag_intra_SAD = smpUMHEX_flag_intra[((img->pix_x)>>4)-1];
    }
    else
    {
      smpUMHEX_flag_intra_SAD = ((smpUMHEX_flag_intra[(img->pix_x)>>4])||
        (smpUMHEX_flag_intra[((img->pix_x)>>4)-1])||
        (smpUMHEX_flag_intra[((img->pix_x)>>4)+1])) ;
    }
  }
  return;
}

/*!
 ************************************************************************
 * \brief
 *    Set cost to zero if neighbouring block is intra
 *    used for fast motion estimation
 ************************************************************************
 */
void smpUMHEX_skip_intrabk_SAD(int best_mode, int ref_max)
{
  short i, j, k;

  if (img->number > 0)
  {
    smpUMHEX_flag_intra[(img->pix_x)>>4] = (best_mode == 9 || best_mode == 10) ? 1 : 0;
  }

  if (img->type != I_SLICE  && (best_mode == 9 || best_mode == 10))
  {
    for (i=0; i < 4; i++)
    {
      for (j=0; j < 4; j++)
      {
        for (k=0; k < 9;k++)
        {
          smpUMHEX_l0_cost[k][j][i] = 0;
          smpUMHEX_l1_cost[k][j][i] = 0;
        }
      }
    }
  }
  return;
}

/*!
 ************************************************************************
 * \brief
 *    Set up prediction MV and prediction up layer cost
 *    used for fast motion estimation
 ************************************************************************
 */
void smpUMHEX_setup(short ref,
                          int list,
                          int block_y,
                          int block_x,
                          int blocktype,
                          short ******all_mv)
{
  if (blocktype > 6)
  {
    smpUMHEX_pred_MV_uplayer_X = all_mv[block_y][block_x][list][ref][5][0];
    smpUMHEX_pred_MV_uplayer_Y = all_mv[block_y][block_x][list][ref][5][1];
  }
  else if (blocktype > 4)
  {
    smpUMHEX_pred_MV_uplayer_X = all_mv[block_y][block_x][list][ref][4][0];
    smpUMHEX_pred_MV_uplayer_Y = all_mv[block_y][block_x][list][ref][4][1];
  }
  else if (blocktype == 4)
  {
    smpUMHEX_pred_MV_uplayer_X = all_mv[block_y][block_x][list][ref][2][0];
    smpUMHEX_pred_MV_uplayer_Y = all_mv[block_y][block_x][list][ref][2][1];
  }
  else if (blocktype > 1)
  {
    smpUMHEX_pred_MV_uplayer_X = all_mv[block_y][block_x][list][ref][1][0];
    smpUMHEX_pred_MV_uplayer_Y = all_mv[block_y][block_x][list][ref][1][1];
  }

  if (blocktype > 1)
  {
    if (blocktype > 6)
    {
      smpUMHEX_pred_SAD_uplayer = (list==1) ?
        (smpUMHEX_l1_cost[5][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x])
        : (smpUMHEX_l0_cost[5][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x]);
      smpUMHEX_pred_SAD_uplayer /= 2;
    }
    else if (blocktype > 4)
    {
      smpUMHEX_pred_SAD_uplayer = (list==1) ?
        (smpUMHEX_l1_cost[4][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x])
        : (smpUMHEX_l0_cost[4][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x]);
      smpUMHEX_pred_SAD_uplayer /= 2;
    }
    else if (blocktype == 4)
    {
      smpUMHEX_pred_SAD_uplayer = (list==1) ?
        (smpUMHEX_l1_cost[2][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x])
        : (smpUMHEX_l0_cost[2][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x]);
      smpUMHEX_pred_SAD_uplayer /= 2;
    }
    else
    {
      smpUMHEX_pred_SAD_uplayer = (list==1) ?
        (smpUMHEX_l1_cost[1][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x])
        : (smpUMHEX_l0_cost[1][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x]);
      smpUMHEX_pred_SAD_uplayer /= 2;
    }

    smpUMHEX_pred_SAD_uplayer = smpUMHEX_flag_intra_SAD ? 0 : smpUMHEX_pred_SAD_uplayer;
  }
}

