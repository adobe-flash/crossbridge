
/*!
 ************************************************************************
 *
 * \file me_umhex.h
 *
 * \brief
 *   Macro definitions and global variables for UMHEX fast
 *   integer pel motion estimation and fractional pel motion estimation
 *
 * \author
 *   Main contributors: (see contributors.h for copyright, address and affiliation details)
 *    - Zhibo Chen         <chenzhibo@tsinghua.org.cn>
 *    - JianFeng Xu        <fenax@video.mdc.tsinghua.edu.cn>
 *    - Wenfang Fu         <fwf@video.mdc.tsinghua.edu.cn>
 *    - Xiaozhong Xu       <xxz@video.mdc.tsinghua.edu.cn>
 *
 * \date
 *   2006.1
 ************************************************************************
 */

#ifndef _ME_UMHEX_H_
#define _ME_UMHEX_H_

#include "mbuffer.h"

#define EARLY_TERMINATION                                                             \
  if ((min_mcost-pred_SAD)<pred_SAD*betaFourth_2)                                     \
  goto fourth_2_step;                                                                 \
  else if((min_mcost-pred_SAD)<pred_SAD*betaFourth_1)                                 \
  goto fourth_1_step;

#define SEARCH_ONE_PIXEL                                                              \
  if(iabs(cand_x - center_x) <=search_range && iabs(cand_y - center_y)<= search_range)\
  {                                                                                   \
    if(!McostState[cand_y-center_y+search_range][cand_x-center_x+search_range])       \
    {                                                                                 \
      mcost = MV_COST (lambda_factor, mvshift, cand_x, cand_y, pred_x, pred_y);       \
      if(mcost<min_mcost)                                                             \
      {                                                                               \
        mcost += computeUniPred[dist_method](orig_pic,                                \
        blocksize_y,blocksize_x, min_mcost - mcost,                                   \
        (cand_x << 2) + IMG_PAD_SIZE_TIMES4, (cand_y << 2) + IMG_PAD_SIZE_TIMES4);    \
        McostState[cand_y-center_y+search_range][cand_x-center_x+search_range] = 1;   \
        if (mcost < min_mcost)                                                        \
        {                                                                             \
          best_x = cand_x;                                                            \
          best_y = cand_y;                                                            \
          min_mcost = mcost;                                                          \
        }                                                                             \
      }                                                                               \
    }                                                                                 \
   }

#define SEARCH_ONE_PIXEL_BIPRED                                                       \
if(iabs(cand_x - center2_x) <=search_range && iabs(cand_y - center2_y)<= search_range)\
{                                                                                     \
  if(!McostState[cand_y-center2_y+search_range][cand_x-center2_x+search_range])       \
  {                                                                                   \
    mcost  = MV_COST (lambda_factor, mvshift, center1_x, center1_y, pred_x1, pred_y1);\
    mcost += MV_COST (lambda_factor, mvshift, cand_x, cand_y, pred_x2, pred_y2);      \
  if(mcost<min_mcost)                                                                 \
  {                                                                                   \
      mcost  += computeBiPred(cur_pic, blocksize_y, blocksize_x,                      \
      min_mcost - mcost,                                                              \
      (center1_x << 2) + IMG_PAD_SIZE_TIMES4,                                         \
      (center1_y << 2) + IMG_PAD_SIZE_TIMES4,                                         \
      (cand_x << 2) + IMG_PAD_SIZE_TIMES4,                                            \
      (cand_y << 2) + IMG_PAD_SIZE_TIMES4);                                           \
      McostState[cand_y-center2_y+search_range][cand_x-center2_x+search_range] = 1;   \
      if (mcost < min_mcost)                                                          \
      {                                                                               \
        best_x = cand_x;                                                              \
        best_y = cand_y;                                                              \
        min_mcost = mcost;                                                            \
      }                                                                               \
    }                                                                                   \
  }                                                                                   \
}

byte **McostState;                          //!< state for integer pel search
byte **SearchState;                         //!< state for fractional pel search

int ****fastme_ref_cost;                    //!< store SAD information needed for forward ref-frame prediction
int ***fastme_l0_cost;                      //!< store SAD information needed for forward median and uplayer prediction
int ***fastme_l1_cost;                      //!< store SAD information needed for backward median and uplayer prediction
int ***fastme_l0_cost_bipred;               //!< store SAD information for bipred mode
int ***fastme_l1_cost_bipred;               //!< store SAD information for bipred mode
int bipred_flag;                            //!< flag for bipred
int **fastme_best_cost;                     //!< for multi ref early termination threshold
int pred_SAD;                               //!<  SAD prediction in use.
int pred_MV_ref[2], pred_MV_uplayer[2];     //!< pred motion vector by space or temporal correlation,Median is provided

int UMHEX_blocktype;                        //!< blocktype for UMHEX SetMotionVectorPredictor
int predict_point[5][2];
int SAD_a,SAD_b,SAD_c,SAD_d;
int Threshold_DSR_MB[8];                    //!<  Threshold for usage of DSR. DSR refer to JVT-Q088
//for early termination
float  Bsize[8];
float AlphaFourth_1[8];
float AlphaFourth_2[8];
byte *flag_intra;
int  flag_intra_SAD;

void UMHEX_DefineThreshold(void);
void UMHEX_DefineThresholdMB(void);
int  UMHEX_get_mem(void);
void UMHEX_free_mem(void);

void UMHEX_decide_intrabk_SAD(void);
void UMHEX_skip_intrabk_SAD(int best_mode, int ref_max);
void UMHEX_setup(short ref, int list, int block_y, int block_x, int blocktype, short   ******all_mv);

int                                     //  ==> minimum motion cost after search
UMHEXIntegerPelBlockMotionSearch  (
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
                                  int       lambda_factor);// <--  lagrangian parameter for determining motion cost

int                                                   //  ==> minimum motion cost after search
UMHEXSubPelBlockMotionSearch (
                             imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                             short       ref,         // <--  reference frame (0... or -1 (backward))
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
                             int       lambda_factor);// <--  lagrangian parameter for determining motion cost


extern int                                        //  ==> minimum motion cost after search
SubPelBlockMotionSearch (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                         short     ref,           // <--  reference frame (0... or -1 (backward))
                         int       list,
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
                         int       *lambda_factor   // <--  lagrangian parameter for determining motion cost
                         );

int                                                //  ==> minimum motion cost after search
UMHEXBipredIntegerPelBlockMotionSearch (
                                       imgpel*   orig_pic,      // <--  original pixel values for the AxB block
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
                                       int       lambda_factor // <--  lagrangian parameter for determining motion cost
                                       );

void UMHEXSetMotionVectorPredictor (short pmv[2], signed char **refPic, short ***tmp_mv,
                                    short  ref_frame, int list, int block_x, int block_y,
                                    int blockshape_x, int blockshape_y, int *search_range);

#endif
