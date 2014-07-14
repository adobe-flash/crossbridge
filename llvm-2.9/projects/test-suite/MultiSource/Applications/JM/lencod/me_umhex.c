
/*!
 ************************************************************************
 *
 * \file me_umhex.c
 *
 * \brief
 *   Fast integer pel motion estimation and fractional pel motion estimation
 *   algorithms are described in this file.
 *   1. UMHEX_get_mem() and UMHEX_free_mem() are functions for allocation and release
 *      of memories about motion estimation
 *   2. UMHEX_BlockMotionSearch() is the function for fast integer pel motion
 *      estimation and fractional pel motion estimation
 *   3. UMHEX_DefineThreshold() defined thresholds for early termination
 * \author
 *    Main contributors: (see contributors.h for copyright, address and affiliation details)
 *    - Zhibo Chen         <chenzhibo@tsinghua.org.cn>
 *    - JianFeng Xu        <fenax@video.mdc.tsinghua.edu.cn>
 *    - Wenfang Fu         <fwf@video.mdc.tsinghua.edu.cn>
 *    - Xiaozhong Xu       <xxz@video.mdc.tsinghua.edu.cn>
 * \date
 *    2006.1
 ************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "global.h"
#include "memalloc.h"
#include "me_umhex.h"
#include "refbuf.h"
#include "mb_access.h"
#include "image.h"
#include "me_distortion.h"

#define Q_BITS          15
#define MIN_IMG_WIDTH   176
extern  int*   byte_abs;
extern  int*   mvbits;
extern  short*   spiral_search_x;
extern  short*   spiral_search_y;


static const int Diamond_x[4] = {-1, 0, 1, 0};
static const int Diamond_y[4] = {0, 1, 0, -1};
static const int Hexagon_x[6] = {2, 1, -1, -2, -1, 1};
static const int Hexagon_y[6] = {0, -2, -2, 0,  2, 2};
static const int Big_Hexagon_x[16] = {0,-2, -4,-4,-4, -4, -4, -2,  0,  2,  4,  4, 4, 4, 4, 2};
static const int Big_Hexagon_y[16] = {4, 3, 2,  1, 0, -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3};

// for bipred mode
static int pred_MV_ref_flag;
static int dist_method;
static StorablePicture *ref_pic_ptr;

static const int   Multi_Ref_Thd[8]   = {0,  300,  120,  120,  60,  30,   30,  15};
static const int   Big_Hexagon_Thd[8] = {0, 3000, 1500, 1500, 800, 400,  400, 200};
static const int   Median_Pred_Thd[8] = {0,  750,  350,  350, 170,  80,   80,  40};
static const int   Threshold_DSR[8]   = {0, 2200, 1000, 1000, 500, 250,  250, 120};

static int Median_Pred_Thd_MB[8];
static int Big_Hexagon_Thd_MB[8];
static int Multi_Ref_Thd_MB[8];


static const int quant_coef[6][4][4] = {
  {{13107, 8066,13107, 8066},{ 8066, 5243, 8066, 5243},{13107, 8066,13107, 8066},{ 8066, 5243, 8066, 5243}},
  {{11916, 7490,11916, 7490},{ 7490, 4660, 7490, 4660},{11916, 7490,11916, 7490},{ 7490, 4660, 7490, 4660}},
  {{10082, 6554,10082, 6554},{ 6554, 4194, 6554, 4194},{10082, 6554,10082, 6554},{ 6554, 4194, 6554, 4194}},
  {{ 9362, 5825, 9362, 5825},{ 5825, 3647, 5825, 3647},{ 9362, 5825, 9362, 5825},{ 5825, 3647, 5825, 3647}},
  {{ 8192, 5243, 8192, 5243},{ 5243, 3355, 5243, 3355},{ 8192, 5243, 8192, 5243},{ 5243, 3355, 5243, 3355}},
  {{ 7282, 4559, 7282, 4559},{ 4559, 2893, 4559, 2893},{ 7282, 4559, 7282, 4559},{ 4559, 2893, 4559, 2893}}
};


void UMHEX_DefineThreshold()
{
  AlphaFourth_1[1] = 0.01f;
  AlphaFourth_1[2] = 0.01f;
  AlphaFourth_1[3] = 0.01f;
  AlphaFourth_1[4] = 0.02f;
  AlphaFourth_1[5] = 0.03f;
  AlphaFourth_1[6] = 0.03f;
  AlphaFourth_1[7] = 0.04f;

  AlphaFourth_2[1] = 0.06f;
  AlphaFourth_2[2] = 0.07f;
  AlphaFourth_2[3] = 0.07f;
  AlphaFourth_2[4] = 0.08f;
  AlphaFourth_2[5] = 0.12f;
  AlphaFourth_2[6] = 0.11f;
  AlphaFourth_2[7] = 0.15f;

  UMHEX_DefineThresholdMB();
  return;
}
/*!
 ************************************************************************
 * \brief
 *    Set MB thresholds for fast motion estimation
 *    Those thresholds may be adjusted to trade off rate-distortion
 *    performance and UMHEX speed
 ************************************************************************
 */

void UMHEX_DefineThresholdMB()
{
  int gb_qp_per    = (input->qpN-MIN_QP)/6;
  int gb_qp_rem    = (input->qpN-MIN_QP)%6;

  int gb_q_bits    = Q_BITS+gb_qp_per;
  int gb_qp_const,Thresh4x4;

  float Quantize_step;
  int i;
// scale factor: defined for different image sizes
  float scale_factor = (float)((1-input->UMHexScale*0.1)+input->UMHexScale*0.1*(img->width/MIN_IMG_WIDTH));
// QP factor: defined for different quantization steps
  float QP_factor = (float)((1.0-0.90*(input->qpN/51.0f)));

  gb_qp_const=(1<<gb_q_bits)/6;
  Thresh4x4 =   ((1<<gb_q_bits) - gb_qp_const)/quant_coef[gb_qp_rem][0][0];
  Quantize_step = Thresh4x4/(4*5.61f)*2.0f*scale_factor;
  Bsize[7]=(16*16)*Quantize_step;

  Bsize[6]=Bsize[7]*4;
  Bsize[5]=Bsize[7]*4;
  Bsize[4]=Bsize[5]*4;
  Bsize[3]=Bsize[4]*4;
  Bsize[2]=Bsize[4]*4;
  Bsize[1]=Bsize[2]*4;

  for(i=1;i<8;i++)
  {
    //ET_Thd1: early termination after median prediction
    Median_Pred_Thd_MB[i]  = (int) (Median_Pred_Thd[i]* scale_factor*QP_factor);
    //ET_thd2: early termination after every circle of 16 points Big-Hex Search
    Big_Hexagon_Thd_MB[i]  = (int) (Big_Hexagon_Thd[i]* scale_factor*QP_factor);
    //threshold for multi ref case
    Multi_Ref_Thd_MB[i]    = (int) (Multi_Ref_Thd[i]  * scale_factor*QP_factor);
    //threshold for usage of DSR technique. DSR ref to JVT-R088
    Threshold_DSR_MB[i]    = (int) (Threshold_DSR[i]  * scale_factor*QP_factor);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Allocation of space for fast motion estimation
 ************************************************************************
 */
int UMHEX_get_mem()
{
  int memory_size = 0;
  if (NULL==(flag_intra = calloc ((img->width>>4)+1,sizeof(byte)))) no_mem_exit("UMHEX_get_mem: flag_intra"); //fwf 20050330

  memory_size += get_mem2D(&McostState, 2*input->search_range+1, 2*input->search_range+1);
  memory_size += get_mem4Dint(&(fastme_ref_cost), img->max_num_references, 9, 4, 4);
  memory_size += get_mem3Dint(&(fastme_l0_cost), 9, img->height/4, img->width/4);
  memory_size += get_mem3Dint(&(fastme_l1_cost), 9, img->height/4, img->width/4);
  memory_size += get_mem2D(&SearchState,7,7);
  memory_size += get_mem2Dint(&(fastme_best_cost), 7, img->width/4);
  if(input->BiPredMotionEstimation == 1)//memory allocation for bipred mode
  {
    memory_size += get_mem3Dint(&(fastme_l0_cost_bipred), 9, img->height/4, img->width/4);//for bipred
    memory_size += get_mem3Dint(&(fastme_l1_cost_bipred), 9, img->height/4, img->width/4);//for bipred
  }

  return memory_size;
}

/*!
 ************************************************************************
 * \brief
 *    Free space for fast motion estimation
 ************************************************************************
 */
void UMHEX_free_mem()
{
  free_mem2D(McostState);
  free_mem4Dint(fastme_ref_cost, img->max_num_references, 9);
  free_mem3Dint(fastme_l0_cost, 9);
  free_mem3Dint(fastme_l1_cost, 9);
  free_mem2D(SearchState);
  free_mem2Dint(fastme_best_cost);
  free (flag_intra);
  if(input->BiPredMotionEstimation == 1)
  {
    free_mem3Dint(fastme_l0_cost_bipred, 9);//for bipred
    free_mem3Dint(fastme_l1_cost_bipred, 9);//for bipred
  }
}

/*!
 ************************************************************************
 * \brief
 *    UMHEXIntegerPelBlockMotionSearch: fast pixel block motion search
 *    this algorithm is called UMHexagonS(see JVT-D016),which includes
 *    four steps with different kinds of search patterns
 * \par Input:
 * imgpel*   orig_pic,     // <--  original picture
 * int       ref,          // <--  reference frame (0... or -1 (backward))
 * int       pic_pix_x,    // <--  absolute x-coordinate of regarded AxB block
 * int       pic_pix_y,    // <--  absolute y-coordinate of regarded AxB block
 * int       blocktype,    // <--  block type (1-16x16 ... 7-4x4)
 * int       pred_mv_x,    // <--  motion vector predictor (x) in sub-pel units
 * int       pred_mv_y,    // <--  motion vector predictor (y) in sub-pel units
 * int*      mv_x,         //  --> motion vector (x) - in pel units
 * int*      mv_y,         //  --> motion vector (y) - in pel units
 * int       search_range, // <--  1-d search range in pel units
 * int       min_mcost,    // <--  minimum motion cost (cost for center or huge value)
 * int       lambda_factor // <--  lagrangian parameter for determining motion cost
 * \par
 * Two macro definitions defined in this program:
 * 1. EARLY_TERMINATION: early termination algrithm, refer to JVT-D016.doc
 * 2. SEARCH_ONE_PIXEL: search one pixel in search range
 * \author
 *   Main contributors: (see contributors.h for copyright, address and affiliation details)
 *   - Zhibo Chen         <chenzhibo@tsinghua.org.cn>
 *   - JianFeng Xu        <fenax@video.mdc.tsinghua.edu.cn>
 *   - Xiaozhong Xu       <xxz@video.mdc.tsinghua.edu.cn>
 * \date   :
 *   2006.1
 ************************************************************************
 */
int                                     //  ==> minimum motion cost after search
UMHEXIntegerPelBlockMotionSearch  (
                                  imgpel   *orig_pic,     //!< <--  not used
                                  short     ref,          //!< <--  reference frame (0... or -1 (backward))
                                  int       list,         //!< <--  reference picture list
                                  int       pic_pix_x,    //!< <--  absolute x-coordinate of regarded AxB block
                                  int       pic_pix_y,    //!< <--  absolute y-coordinate of regarded AxB block
                                  int       blocktype,    //!< <--  block type (1-16x16 ... 7-4x4)
                                  short     pred_mv_x,    //!< <--  motion vector predictor (x) in sub-pel units
                                  short     pred_mv_y,    //!< <--  motion vector predictor (y) in sub-pel units
                                  short*    mv_x,         //!< --> motion vector (x) - in pel units
                                  short*    mv_y,         //!< --> motion vector (y) - in pel units
                                  int       search_range, //!< <--  1-d search range in pel units
                                  int       min_mcost,    //!< <--  minimum motion cost (cost for center or huge value)
                                  int       lambda_factor //!< <--  lagrangian parameter for determining motion cost
                                  )
{
  int   list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))?
                         img->current_mb_nr%2 ? 4 : 2 : 0;
  int   mvshift       = 2;                                        //!< motion vector shift for getting sub-pel units
  int   blocksize_y   = input->blc_size[blocktype][1];            //!< vertical block size
  int   blocksize_x   = input->blc_size[blocktype][0];            //!< horizontal block size
  int   pred_x        = (pic_pix_x << mvshift) + pred_mv_x;       //!< predicted position x (in sub-pel units)
  int   pred_y        = (pic_pix_y << mvshift) + pred_mv_y;       //!< predicted position y (in sub-pel units)
  int   center_x      = pic_pix_x + *mv_x;                        //!< center position x (in pel units)
  int   center_y      = pic_pix_y + *mv_y;                        //!< center position y (in pel units)
  int   best_x        = 0, best_y = 0;
  int   search_step, iYMinNow, iXMinNow;
  int   pos, cand_x, cand_y,  mcost;
  int   i,m,j;
  float betaFourth_1,betaFourth_2;
  int  temp_Big_Hexagon_x[16];//  temp for Big_Hexagon_x;
  int  temp_Big_Hexagon_y[16];//  temp for Big_Hexagon_y;
  short mb_x = pic_pix_x - img->opix_x;
  short mb_y = pic_pix_y - img->opix_y;
  short pic_pix_x2 = pic_pix_x >> 2;
  short block_x = (mb_x >> 2);
  short block_y = (mb_y >> 2);
  int ET_Thred = Median_Pred_Thd_MB[blocktype];//ET threshold in use
  int   *SAD_prediction = fastme_best_cost[blocktype-1];//multi ref SAD prediction
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

  //////allocate memory for search state//////////////////////////
  memset(McostState[0],0,(2*input->search_range+1)*(2*input->search_range+1));


  //check the center median predictor
  cand_x = center_x ;
  cand_y = center_y ;
  mcost = MV_COST (lambda_factor, mvshift, cand_x, cand_y, pred_x, pred_y);

  mcost += computeUniPred[dist_method](orig_pic, blocksize_y,blocksize_x, min_mcost - mcost,
    (cand_x << 2) + IMG_PAD_SIZE_TIMES4, (cand_y << 2) + IMG_PAD_SIZE_TIMES4);

  McostState[search_range][search_range] = 1;
  if (mcost < min_mcost)
  {
    min_mcost = mcost;
    best_x    = cand_x;
    best_y    = cand_y;
  }

  iXMinNow = best_x;
  iYMinNow = best_y;
  for (m = 0; m < 4; m++)
  {
    cand_x = iXMinNow + Diamond_x[m];
    cand_y = iYMinNow + Diamond_y[m];
    SEARCH_ONE_PIXEL
  }

  if(center_x != pic_pix_x || center_y != pic_pix_y)
  {
    cand_x = pic_pix_x ;
    cand_y = pic_pix_y ;
    SEARCH_ONE_PIXEL

    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_x[m];
      cand_y = iYMinNow + Diamond_y[m];
      SEARCH_ONE_PIXEL
    }
  }
  /***********************************init process*************************/
  //for multi ref
  if(ref>0 && img->structure == FRAME  && min_mcost > ET_Thred && SAD_prediction[pic_pix_x2]<Multi_Ref_Thd_MB[blocktype])
    goto terminate_step;

  //ET_Thd1: early termination for low motion case
  if( min_mcost < ET_Thred)
  {
    goto terminate_step;
  }
  else // hybrid search for main search loop
  {
    /****************************(MV and SAD prediction)********************************/
    UMHEX_setup(ref, list, block_y, block_x, blocktype, img->all_mv );
    ET_Thred = Big_Hexagon_Thd_MB[blocktype];  // ET_Thd2: early termination Threshold for strong motion



    // Threshold defined for EARLY_TERMINATION
    if (pred_SAD == 0)
    {
      betaFourth_1=0;
      betaFourth_2=0;
    }
    else
    {
      betaFourth_1 = Bsize[blocktype]/(pred_SAD*pred_SAD)-AlphaFourth_1[blocktype];
      betaFourth_2 = Bsize[blocktype]/(pred_SAD*pred_SAD)-AlphaFourth_2[blocktype];

    }
    /*********************************************end of init ***********************************************/
  }
  // first_step: initial start point prediction

  if(blocktype>1)
  {
    cand_x = pic_pix_x + (pred_MV_uplayer[0]/4);
    cand_y = pic_pix_y + (pred_MV_uplayer[1]/4);
    SEARCH_ONE_PIXEL
  }


  //prediction using mV of last ref moiton vector
  if(pred_MV_ref_flag == 1)      //Notes: for interlace case, ref==1 should be added
  {
    cand_x = pic_pix_x + (pred_MV_ref[0]/4);
    cand_y = pic_pix_y + (pred_MV_ref[1]/4);
    SEARCH_ONE_PIXEL
  }
  //small local search
  iXMinNow = best_x;
  iYMinNow = best_y;
  for (m = 0; m < 4; m++)
  {
    cand_x = iXMinNow + Diamond_x[m];
    cand_y = iYMinNow + Diamond_y[m];
    SEARCH_ONE_PIXEL
  }

  //early termination algorithm, refer to JVT-G016
  EARLY_TERMINATION

  if(blocktype>6)
    goto fourth_1_step;
  else
    goto sec_step;

sec_step: //Unsymmetrical-cross search
  iXMinNow = best_x;
  iYMinNow = best_y;

  for(i = 1; i < search_range; i+=2)
  {
    search_step = i;
    cand_x = iXMinNow + search_step;
    cand_y = iYMinNow ;
    SEARCH_ONE_PIXEL
    cand_x = iXMinNow - search_step;
    cand_y = iYMinNow ;
    SEARCH_ONE_PIXEL
  }
  for(i = 1; i < (search_range/2);i+=2)
  {
    search_step = i;
    cand_x = iXMinNow ;
    cand_y = iYMinNow + search_step;
    SEARCH_ONE_PIXEL
    cand_x = iXMinNow ;
    cand_y = iYMinNow - search_step;
    SEARCH_ONE_PIXEL
  }


  //early termination alogrithm, refer to JVT-G016
  EARLY_TERMINATION

  iXMinNow = best_x;
  iYMinNow = best_y;

  //third_step:    // Uneven Multi-Hexagon-grid Search
  //sub step 1: 5x5 squre search
  for(pos=1;pos<25;pos++)
  {
    cand_x = iXMinNow + spiral_search_x[pos];
    cand_y = iYMinNow + spiral_search_y[pos];
    SEARCH_ONE_PIXEL
  }

  //early termination alogrithm, refer to JVT-G016
  EARLY_TERMINATION

  //sub step 2:  Multi-Hexagon-grid search
  memcpy(temp_Big_Hexagon_x,Big_Hexagon_x,64);
  memcpy(temp_Big_Hexagon_y,Big_Hexagon_y,64);
  for(i=1;i<=(search_range/4); i++)
  {

    for (m = 0; m < 16; m++)
    {
      cand_x = iXMinNow + temp_Big_Hexagon_x[m];
      cand_y = iYMinNow + temp_Big_Hexagon_y[m];
      temp_Big_Hexagon_x[m] += Big_Hexagon_x[m];
      temp_Big_Hexagon_y[m] += Big_Hexagon_y[m];

      SEARCH_ONE_PIXEL
    }
    // ET_Thd2: early termination Threshold for strong motion
    if(min_mcost < ET_Thred)
    {
      goto terminate_step;
    }
  }


  //fourth_step:  //Extended Hexagon-based Search
  // the fourth step with a small search pattern
fourth_1_step:  //sub step 1: small Hexagon search
  for(i = 0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 6; m++)
    {
      cand_x = iXMinNow + Hexagon_x[m];
      cand_y = iYMinNow + Hexagon_y[m];
      SEARCH_ONE_PIXEL
    }

    if (best_x == iXMinNow && best_y == iYMinNow)
    {
      break;
    }
  }
fourth_2_step: //sub step 2: small Diamond search

  for(i = 0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_x[m];
      cand_y = iYMinNow + Diamond_y[m];
      SEARCH_ONE_PIXEL
    }
    if(best_x == iXMinNow && best_y == iYMinNow)
      break;
  }

terminate_step:

  // store SAD infomation for prediction
  //FAST MOTION ESTIMATION. ZHIBO CHEN 2003.3
  for (i=0; i < (blocksize_x>>2); i++)
  {
    for (j=0; j < (blocksize_y>>2); j++)
    {
      if(list == 0)
      {
        fastme_ref_cost[ref][blocktype][block_y+j][block_x+i] = min_mcost;
        if (ref==0)
          fastme_l0_cost[blocktype][(img->pix_y>>2)+block_y+j][(img->pix_x>>2)+block_x+i] = min_mcost;
      }
      else
      {
        fastme_l1_cost[blocktype][(img->pix_y>>2)+block_y+j][(img->pix_x>>2)+block_x+i] = min_mcost;
      }
    }
  }
  //for multi ref SAD prediction
  if ((ref==0) || (SAD_prediction[pic_pix_x2] > min_mcost))
    SAD_prediction[pic_pix_x2] = min_mcost;

  *mv_x = (short) (best_x - pic_pix_x);
  *mv_y = (short) (best_y - pic_pix_y);
  return min_mcost;
}

int                                                   //  ==> minimum motion cost after search
UMHEXSubPelBlockMotionSearch (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
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
                             int       lambda_factor)
{
  static int Diamond_x[4] = {-1, 0, 1, 0};
  static int Diamond_y[4] = {0, 1, 0, -1};
  int   mcost;
  int   cand_mv_x, cand_mv_y;

  int   list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? img->current_mb_nr%2 ? 4 : 2 : 0;
  StorablePicture *ref_picture = listX[list+list_offset][ref];

  int   mv_shift        = 0;
  int   blocksize_x     = input->blc_size[blocktype][0];
  int   blocksize_y     = input->blc_size[blocktype][1];
  int   pic4_pix_x      = ((pic_pix_x + IMG_PAD_SIZE)<< 2);
  int   pic4_pix_y      = ((pic_pix_y + IMG_PAD_SIZE)<< 2);
  short max_pos_x4      = ((ref_picture->size_x - blocksize_x + 2*IMG_PAD_SIZE)<<2);
  short max_pos_y4      = ((ref_picture->size_y - blocksize_y + 2*IMG_PAD_SIZE)<<2);

  int   search_range_dynamic,iXMinNow,iYMinNow,i;
  int   m,currmv_x = 0,currmv_y = 0;
  int   pred_frac_mv_x,pred_frac_mv_y,abort_search;

  int   pred_frac_up_mv_x, pred_frac_up_mv_y;
  int  apply_weights = ( (active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;

  dist_method = Q_PEL + 3 * apply_weights;
  if ((pic4_pix_x + *mv_x > 1) && (pic4_pix_x + *mv_x < max_pos_x4 - 1) &&
    (pic4_pix_y + *mv_y > 1) && (pic4_pix_y + *mv_y < max_pos_y4 - 1)   )
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

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

  if (ChromaMEEnable )
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

  search_range_dynamic = 3;
  pred_frac_mv_x = (pred_mv_x - *mv_x)%4;
  pred_frac_mv_y = (pred_mv_y - *mv_y)%4;

  pred_frac_up_mv_x = (pred_MV_uplayer[0] - *mv_x)%4;
  pred_frac_up_mv_y = (pred_MV_uplayer[1] - *mv_y)%4;


  memset(SearchState[0],0,(2*search_range_dynamic+1)*(2*search_range_dynamic+1));

  if( !start_me_refinement_hp )
  {
    cand_mv_x = *mv_x;
    cand_mv_y = *mv_y;
    mcost = MV_COST (lambda_factor, mv_shift, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);

    mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
      min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

    SearchState[search_range_dynamic][search_range_dynamic] = 1;
    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      currmv_x = cand_mv_x;
      currmv_y = cand_mv_y;
    }
  }
  else
  {
    SearchState[search_range_dynamic][search_range_dynamic] = 1;
    currmv_x = *mv_x;
    currmv_y = *mv_y;
  }

  if(pred_frac_mv_x!=0 || pred_frac_mv_y!=0)
  {
    cand_mv_x = *mv_x + pred_frac_mv_x;
    cand_mv_y = *mv_y + pred_frac_mv_y;
    mcost = MV_COST (lambda_factor, mv_shift, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);
    mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
      min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);
    SearchState[cand_mv_y -*mv_y + search_range_dynamic][cand_mv_x - *mv_x + search_range_dynamic] = 1;
    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      currmv_x = cand_mv_x;
      currmv_y = cand_mv_y;
    }
  }


  iXMinNow = currmv_x;
  iYMinNow = currmv_y;
  for(i=0;i<search_range_dynamic;i++)
  {
    abort_search=1;
    for (m = 0; m < 4; m++)
    {
      cand_mv_x = iXMinNow + Diamond_x[m];
      cand_mv_y = iYMinNow + Diamond_y[m];

      if(iabs(cand_mv_x - *mv_x) <=search_range_dynamic && iabs(cand_mv_y - *mv_y)<= search_range_dynamic)
      {
        if(!SearchState[cand_mv_y -*mv_y+ search_range_dynamic][cand_mv_x -*mv_x+ search_range_dynamic])
        {
          mcost = MV_COST (lambda_factor, mv_shift, cand_mv_x, cand_mv_y, pred_mv_x, pred_mv_y);
          mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
            min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);
          SearchState[cand_mv_y - *mv_y + search_range_dynamic][cand_mv_x - *mv_x + search_range_dynamic] = 1;
          if (mcost < min_mcost)
          {
            min_mcost = mcost;
            currmv_x = cand_mv_x;
            currmv_y = cand_mv_y;
            abort_search = 0;
          }
        }
      }
    }
    iXMinNow = currmv_x;
    iYMinNow = currmv_y;
    if(abort_search)
      break;
  }

  *mv_x = currmv_x;
  *mv_y = currmv_y;

  //===== return minimum motion cost =====
  return min_mcost;
}

/*!
 ************************************************************************
 * \brief
 * Functions for SAD prediction of intra block cases.
 * 1. void UMHEX_decide_intrabk_SAD() judges the block coding type(intra/inter)
 *    of neibouring blocks
 * 2. void UMHEX_skip_intrabk_SAD() set the SAD to zero if neigouring block coding
 *    type is intra
 * \date
 *    2003.4
 ************************************************************************
 */
void UMHEX_decide_intrabk_SAD()
{
  if (img->type != I_SLICE)
  {
    if (img->pix_x == 0 && img->pix_y == 0)
    {
      flag_intra_SAD = 0;
    }
    else if (img->pix_x == 0)
    {
      flag_intra_SAD = flag_intra[(img->pix_x)>>4];
    }
    else if (img->pix_y == 0)
    {
      flag_intra_SAD = flag_intra[((img->pix_x)>>4)-1];
    }
    else
    {
      flag_intra_SAD = ((flag_intra[(img->pix_x)>>4])||(flag_intra[((img->pix_x)>>4)-1])||(flag_intra[((img->pix_x)>>4)+1])) ;
    }
  }
  return;
}

void UMHEX_skip_intrabk_SAD(int best_mode, int ref_max)
{
  int i,j,k, ref;
  if (img->number > 0)
    flag_intra[(img->pix_x)>>4] = (best_mode == 9 || best_mode == 10) ? 1:0;
  if (img->type != I_SLICE  && (best_mode == 9 || best_mode == 10))
  {
    for (i=0; i < 4; i++)
    {
      for (j=0; j < 4; j++)
      {
        for (k=0; k < 9;k++)
        {
          fastme_l0_cost[k][j][i] = 0;
          fastme_l1_cost[k][j][i] = 0;
          for (ref=0; ref<ref_max;ref++)
          {
            fastme_ref_cost[ref][k][j][i] = 0;
          }
        }
      }
    }

  }
  return;
}


void UMHEX_setup(short ref, int list, int block_y, int block_x, int blocktype, short   ******all_mv)
{
  int  N_Bframe=0;
  int n_Bframe=0;
  int temp_blocktype = 0;
  int indication_blocktype[8]={0,0,1,1,2,4,4,5};
  N_Bframe = input->successive_Bframe;
  n_Bframe =(N_Bframe) ? (frame_ctr[B_SLICE]%(N_Bframe+1)): 0;


  /**************************** MV prediction **********************/
  //MV uplayer prediction
  if (blocktype>1)
  {
    temp_blocktype = indication_blocktype[blocktype];
    pred_MV_uplayer[0] = all_mv[block_y][block_x][list][ref][temp_blocktype][0];
    pred_MV_uplayer[1] = all_mv[block_y][block_x][list][ref][temp_blocktype][1];
  }


  //MV ref-frame prediction
  pred_MV_ref_flag = 0;
  if(list==0)
  {
    if (img->field_picture)
    {
      if ( ref > 1)
      {
        pred_MV_ref[0] = all_mv[block_y][block_x][0][ref-2][blocktype][0];
        pred_MV_ref[0] = (int)(pred_MV_ref[0]*((ref>>1)+1)/(float)((ref>>1)));
        pred_MV_ref[1] = all_mv[block_y][block_x][0][ref-2][blocktype][1];
        pred_MV_ref[1] = (int)(pred_MV_ref[1]*((ref>>1)+1)/(float)((ref>>1)));
        pred_MV_ref_flag = 1;
      }
      if (img->type == B_SLICE &&  (ref==0 || ref==1) )
      {
        pred_MV_ref[0] =(int) (all_mv[block_y][block_x][1][0][blocktype][0]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
        pred_MV_ref[1] =(int) (all_mv[block_y][block_x][1][0][blocktype][1]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
        pred_MV_ref_flag = 1;
      }
    }
    else //frame case
    {
      if ( ref > 0)
      {
        pred_MV_ref[0] = all_mv[block_y][block_x][0][ref-1][blocktype][0];
        pred_MV_ref[0] = (int)(pred_MV_ref[0]*(ref+1)/(float)(ref));
        pred_MV_ref[1] = all_mv[block_y][block_x][0][ref-1][blocktype][1];
        pred_MV_ref[1] = (int)(pred_MV_ref[1]*(ref+1)/(float)(ref));
        pred_MV_ref_flag = 1;
      }
      if (img->type == B_SLICE && (ref==0)) //B frame forward prediction, first ref
      {
        pred_MV_ref[0] =(int) (all_mv[block_y][block_x][1][0][blocktype][0]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
        pred_MV_ref[1] =(int) (all_mv[block_y][block_x][1][0][blocktype][1]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
        pred_MV_ref_flag = 1;
      }
    }
  }
  /******************************SAD prediction**********************************/
  if (list==0 && ref>0)  //pred_SAD_ref
  {

    if (flag_intra_SAD) //add this for irregular motion
    {
      pred_SAD = 0;
    }
    else
    {
      if (img->field_picture)
      {
        if (ref > 1)
        {
          pred_SAD = fastme_ref_cost[ref-2][blocktype][block_y][block_x];
        }
        else
        {
          pred_SAD = fastme_ref_cost[0][blocktype][block_y][block_x];
        }
      }
      else
      {
        pred_SAD = fastme_ref_cost[ref-1][blocktype][block_y][block_x];
      }

    }
  }
  else if (blocktype>1)  // pred_SAD_uplayer
  {
    if (flag_intra_SAD)
    {
      pred_SAD = 0;
    }
    else
    {
      pred_SAD = (list==1) ? (fastme_l1_cost[temp_blocktype][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x]) : (fastme_l0_cost[temp_blocktype][(img->pix_y>>2)+block_y][(img->pix_x>>2)+block_x]);
      pred_SAD /= 2;
    }
  }
  else pred_SAD = 0 ;  // pred_SAD_space

}

/*!
 ************************************************************************
 * \brief
 *    UMHEXBipredIntegerPelBlockMotionSearch: fast pixel block motion search for bipred mode
 *    this algrithm is called UMHexagonS(see JVT-D016),which includes
 *    four steps with different kinds of search patterns
 * \author
 *   Main contributors: (see contributors.h for copyright, address and affiliation details)
 *   - Zhibo Chen         <chenzhibo@tsinghua.org.cn>
 *   - JianFeng Xu        <fenax@video.mdc.tsinghua.edu.cn>
 *   - Xiaozhong Xu       <xxz@video.mdc.tsinghua.edu.cn>
 * \date   :
 *   2006.1
 ************************************************************************
 */
int                                                //  ==> minimum motion cost after search
UMHEXBipredIntegerPelBlockMotionSearch (imgpel*   cur_pic,      // <--  original pixel values for the AxB block
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
  int   temp_Big_Hexagon_x[16];// = Big_Hexagon_x;
  int   temp_Big_Hexagon_y[16];// = Big_Hexagon_y;
  int   mvshift       = 2;                  // motion vector shift for getting sub-pel units

  int   search_step,iYMinNow, iXMinNow;
  int   i,m,j;
  float betaFourth_1,betaFourth_2;
  int   pos, cand_x, cand_y,mcost;
  int   list_offset   = img->mb_data[img->current_mb_nr].list_offset;
  int   blocksize_y   = input->blc_size[blocktype][1];            // vertical block size
  int   blocksize_x   = input->blc_size[blocktype][0];            // horizontal block size
  int   pred_x1        = (pic_pix_x << 2) + pred_mv_x1;       // predicted position x (in sub-pel units)
  int   pred_y1        = (pic_pix_y << 2) + pred_mv_y1;       // predicted position y (in sub-pel units)
  int   pred_x2        = (pic_pix_x << 2) + pred_mv_x2;       // predicted position x (in sub-pel units)
  int   pred_y2        = (pic_pix_y << 2) + pred_mv_y2;       // predicted position y (in sub-pel units)
  short center2_x      = pic_pix_x + *mv_x;                      // center position x (in pel units)
  short center2_y      = pic_pix_y + *mv_y;                      // center position y (in pel units)
  short center1_x      = pic_pix_x + *s_mv_x;                      // mvx of second pred (in pel units)
  short center1_y      = pic_pix_y + *s_mv_y;                      // mvy of second pred (in pel units)
  short mb_x = pic_pix_x - img->opix_x;
  short mb_y = pic_pix_y - img->opix_y;
  short block_x = (mb_x >> 2);
  short block_y = (mb_y >> 2);
  int   best_x = center2_x;
  int   best_y = center2_y;
  int ET_Thred = Median_Pred_Thd_MB[blocktype];

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

  //===== set function for getting reference picture lines =====
  if ((center2_x > search_range) && (center2_x < img_width -1-search_range-blocksize_x) &&
    (center2_y > search_range) && (center2_y < img_height-1-search_range-blocksize_y)   )
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
    bipred2_access_method = UMV_ACCESS;
  }

  //===== set function for getting reference picture lines =====
  if ((center1_y > search_range) && (center1_y < img_height-1-search_range-blocksize_y)   )
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  //////////////////////////////////////////////////////////////////////////

  //////allocate memory for search state//////////////////////////
  memset(McostState[0],0,(2*search_range+1)*(2*search_range+1));

  //check the center median predictor
  cand_x = center2_x ;
  cand_y = center2_y ;
  mcost  = MV_COST (lambda_factor, mvshift, center1_x, center1_y, pred_x1, pred_y1);
  mcost += MV_COST (lambda_factor, mvshift, cand_x,    cand_y,    pred_x2, pred_y2);

  mcost += computeBiPred( cur_pic,
                         blocksize_y, blocksize_x, INT_MAX,
                         (center1_x << 2) + IMG_PAD_SIZE_TIMES4,
                         (center1_y << 2) + IMG_PAD_SIZE_TIMES4,
                         (cand_x << 2) + IMG_PAD_SIZE_TIMES4,
                         (cand_y << 2) + IMG_PAD_SIZE_TIMES4);

  McostState[search_range][search_range] = 1;

  if (mcost < min_mcost)
  {
    min_mcost = mcost;
    best_x = cand_x;
    best_y = cand_y;
  }

  iXMinNow = best_x;
  iYMinNow = best_y;
  for (m = 0; m < 4; m++)
  {
    cand_x = iXMinNow + Diamond_x[m];
    cand_y = iYMinNow + Diamond_y[m];
    SEARCH_ONE_PIXEL_BIPRED;
  }

  if(center2_x != pic_pix_x || center2_y != pic_pix_y)
  {
    cand_x = pic_pix_x ;
    cand_y = pic_pix_y ;

    SEARCH_ONE_PIXEL_BIPRED;

    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_x[m];
      cand_y = iYMinNow + Diamond_y[m];
      SEARCH_ONE_PIXEL_BIPRED;
    }
  }
  /***********************************init process*************************/

  if( min_mcost < ET_Thred)
  {
    goto terminate_step;
  }
  else
  {
    int  N_Bframe=0;
    int  n_Bframe=0;
    short****** bipred_mv = list ? img->bipred_mv1 : img->bipred_mv2;
    N_Bframe = input->successive_Bframe;
    n_Bframe = frame_ctr[B_SLICE]%(N_Bframe+1);


    /**************************** MV prediction **********************/
    //MV uplayer prediction
    // non for bipred mode

    //MV ref-frame prediction

    if(list==0)
    {
      if (img->field_picture)
      {
        pred_MV_ref[0] =(int) (bipred_mv[block_y][block_x][1][0][blocktype][0]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
        pred_MV_ref[1] =(int) (bipred_mv[block_y][block_x][1][0][blocktype][1]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
      }
      else //frame case
      {
        pred_MV_ref[0] =(int) (bipred_mv[block_y][block_x][1][0][blocktype][0]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
        pred_MV_ref[1] =(int) (bipred_mv[block_y][block_x][1][0][blocktype][1]*(-n_Bframe)/(N_Bframe-n_Bframe+1.0f));
      }
    }
    /******************************SAD prediction**********************************/

    pred_SAD =imin(imin(SAD_a,SAD_b),SAD_c);  // pred_SAD_space
    ET_Thred = Big_Hexagon_Thd_MB[blocktype];

    ///////Threshold defined for early termination///////////////////
    if (pred_SAD == 0)
    {
      betaFourth_1=0;
      betaFourth_2=0;
    }
    else
    {
      betaFourth_1 = Bsize[blocktype]/(pred_SAD*pred_SAD)-AlphaFourth_1[blocktype];
      betaFourth_2 = Bsize[blocktype]/(pred_SAD*pred_SAD)-AlphaFourth_2[blocktype];
    }
  }

  /***********************************end of init *************************/



  // first_step: initial start point prediction
  //prediction using mV of last ref moiton vector
  if(list == 0)
  {
    cand_x = pic_pix_x + (pred_MV_ref[0]/4);
    cand_y = pic_pix_y + (pred_MV_ref[1]/4);
    SEARCH_ONE_PIXEL_BIPRED;
  }


  //small local search
  iXMinNow = best_x;
  iYMinNow = best_y;
  for (m = 0; m < 4; m++)
  {
    cand_x = iXMinNow + Diamond_x[m];
    cand_y = iYMinNow + Diamond_y[m];
    SEARCH_ONE_PIXEL_BIPRED;
  }

  //early termination alogrithm, refer to JVT-G016
  EARLY_TERMINATION;


  //sec_step: //Unsymmetrical-cross search
  iXMinNow = best_x;
  iYMinNow = best_y;

  for(i = 1; i < search_range; i+=2)
  {
    search_step = i;
    cand_x = iXMinNow + search_step;
    cand_y = iYMinNow ;
    SEARCH_ONE_PIXEL_BIPRED;
    cand_x = iXMinNow - search_step;
    cand_y = iYMinNow ;
    SEARCH_ONE_PIXEL_BIPRED;
  }

  for(i = 1; i < (search_range/2);i+=2)
  {
    search_step = i;
    cand_x = iXMinNow ;
    cand_y = iYMinNow + search_step;
    SEARCH_ONE_PIXEL_BIPRED;
    cand_x = iXMinNow ;
    cand_y = iYMinNow - search_step;
    SEARCH_ONE_PIXEL_BIPRED;
  }
  //early termination alogrithm, refer to JVT-G016
  EARLY_TERMINATION;

  //third_step:     // Uneven Multi-Hexagon-grid Search
  iXMinNow = best_x;
  iYMinNow = best_y;
  //sub step1: 5x5 square search
  for(pos=1;pos<25;pos++)
  {
    cand_x = iXMinNow + spiral_search_x[pos];
    cand_y = iYMinNow + spiral_search_y[pos];
    SEARCH_ONE_PIXEL_BIPRED;
  }

  //early termination alogrithm, refer to JVT-G016
  EARLY_TERMINATION;      //added back by xxz

  //sub step2: multi-grid-hexagon-search
  memcpy(temp_Big_Hexagon_x,Big_Hexagon_x,64);
  memcpy(temp_Big_Hexagon_y,Big_Hexagon_y,64);
  for(i=1;i<=(input->search_range>>2); i++)
  {

    for (m = 0; m < 16; m++)
    {
      cand_x = iXMinNow + temp_Big_Hexagon_x[m];
      cand_y = iYMinNow + temp_Big_Hexagon_y[m];
      temp_Big_Hexagon_x[m] += Big_Hexagon_x[m];
      temp_Big_Hexagon_y[m] += Big_Hexagon_y[m];

      SEARCH_ONE_PIXEL_BIPRED;
    }
    if(min_mcost < ET_Thred)
    {
      goto terminate_step;

    }
  }
  //fourth step: Local Refinement: Extended Hexagon-based Search
fourth_1_step:

  for(i=0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 6; m++)
    {
      cand_x = iXMinNow + Hexagon_x[m];
      cand_y = iYMinNow + Hexagon_y[m];
      SEARCH_ONE_PIXEL_BIPRED;
    }
    if(best_x == iXMinNow && best_y == iYMinNow)
      break;
  }
fourth_2_step:

  for(i = 0; i < search_range; i++)
  {
    iXMinNow = best_x;
    iYMinNow = best_y;
    for (m = 0; m < 4; m++)
    {
      cand_x = iXMinNow + Diamond_x[m];
      cand_y = iYMinNow + Diamond_y[m];
      SEARCH_ONE_PIXEL_BIPRED;
    }
    if(best_x == iXMinNow && best_y == iYMinNow)
      break;
  }

terminate_step:
  for (i=0; i < (blocksize_x>>2); i++)
  {
    for (j=0; j < (blocksize_y>>2); j++)
    {
      if(list == 0)
      {
        fastme_l0_cost_bipred[blocktype][(img->pix_y>>2)+block_y+j][(img->pix_x>>2)+block_x+i] = min_mcost;
      }
      else
      {
        fastme_l1_cost_bipred[blocktype][(img->pix_y>>2)+block_y+j][(img->pix_x>>2)+block_x+i] = min_mcost;
      }
    }
  }

  *mv_x = best_x - pic_pix_x;
  *mv_y = best_y - pic_pix_y;


  return min_mcost;
}

/*!
 ************************************************************************
 * \brief
 *    Set motion vector predictor
 ************************************************************************
 */
void UMHEXSetMotionVectorPredictor (short  pmv[2],
                               signed char **refPic,
                               short  ***tmp_mv,
                               short  ref_frame,
                               int    list,
                               int    block_x,
                               int    block_y,
                               int    blockshape_x,
                               int    blockshape_y,
                               int    *search_range)
{
  int mb_x                 = 4*block_x;
  int mb_y                 = 4*block_y;
  int mb_nr                = img->current_mb_nr;

  int mv_a, mv_b, mv_c, pred_vec=0;
  int mvPredType, rFrameL, rFrameU, rFrameUR;
  int hv;

  PixelPos block_a, block_b, block_c, block_d;

  // added for bipred mode
  int *** fastme_l0_cost_flag = (bipred_flag ? fastme_l0_cost_bipred:fastme_l0_cost);
  int *** fastme_l1_cost_flag = (bipred_flag ? fastme_l1_cost_bipred:fastme_l1_cost);

  //Dynamic Search Range

  int dsr_temp_search_range[2];
  int dsr_mv_avail, dsr_mv_max, dsr_mv_sum, dsr_small_search_range;

  // neighborhood SAD init
  SAD_a=0;
  SAD_b=0;
  SAD_c=0;
  SAD_d=0;

  getLuma4x4Neighbour(mb_nr, mb_x - 1, mb_y, &block_a);
  getLuma4x4Neighbour(mb_nr, mb_x, mb_y -1, &block_b);
  getLuma4x4Neighbour(mb_nr, mb_x + blockshape_x, mb_y-1, &block_c);
  getLuma4x4Neighbour(mb_nr, mb_x -1, mb_y -1, &block_d);

  if (mb_y > 0)
  {
    if (mb_x < 8)  // first column of 8x8 blocks
    {
      if (mb_y==8)
      {
        if (blockshape_x == 16)      block_c.available  = 0;
      }
      else
      {
        if (mb_x+blockshape_x == 8)  block_c.available = 0;
      }
    }
    else
    {
      if (mb_x+blockshape_x == 16)   block_c.available = 0;
    }
  }

  if (!block_c.available)
  {
    block_c=block_d;
  }

  mvPredType = MVPRED_MEDIAN;

  if (!img->MbaffFrameFlag)
  {
    rFrameL    = block_a.available    ? refPic[block_a.pos_y][block_a.pos_x] : -1;
    rFrameU    = block_b.available    ? refPic[block_b.pos_y][block_b.pos_x] : -1;
    rFrameUR   = block_c.available    ? refPic[block_c.pos_y][block_c.pos_x] : -1;
  }
  else
  {
    if (img->mb_data[img->current_mb_nr].mb_field)
    {
      rFrameL  = block_a.available
        ? (img->mb_data[block_a.mb_addr].mb_field
        ? refPic[block_a.pos_y][block_a.pos_x]
        : refPic[block_a.pos_y][block_a.pos_x] * 2) : -1;
      rFrameU  = block_b.available
        ? (img->mb_data[block_b.mb_addr].mb_field
        ? refPic[block_b.pos_y][block_b.pos_x]
        : refPic[block_b.pos_y][block_b.pos_x] * 2) : -1;
      rFrameUR = block_c.available
        ? (img->mb_data[block_c.mb_addr].mb_field
        ? refPic[block_c.pos_y][block_c.pos_x]
        : refPic[block_c.pos_y][block_c.pos_x] * 2) : -1;
    }
    else
      {
      rFrameL = block_a.available
        ? (img->mb_data[block_a.mb_addr].mb_field
        ? refPic[block_a.pos_y][block_a.pos_x] >>1
        : refPic[block_a.pos_y][block_a.pos_x]) : -1;
      rFrameU    = block_b.available    ?
        img->mb_data[block_b.mb_addr].mb_field ?
        refPic[block_b.pos_y][block_b.pos_x] >>1:
      refPic[block_b.pos_y][block_b.pos_x] :
      -1;
      rFrameUR    = block_c.available    ?
        img->mb_data[block_c.mb_addr].mb_field ?
        refPic[block_c.pos_y][block_c.pos_x] >>1:
      refPic[block_c.pos_y][block_c.pos_x] :
      -1;
    }
  }

  /* Prediction if only one of the neighbors uses the reference frame
  * we are checking
  */
  if(rFrameL == ref_frame && rFrameU != ref_frame && rFrameUR != ref_frame)       mvPredType = MVPRED_L;
  else if(rFrameL != ref_frame && rFrameU == ref_frame && rFrameUR != ref_frame)  mvPredType = MVPRED_U;
  else if(rFrameL != ref_frame && rFrameU != ref_frame && rFrameUR == ref_frame)  mvPredType = MVPRED_UR;
  // Directional predictions
  if(blockshape_x == 8 && blockshape_y == 16)
  {
    if(mb_x == 0)
    {
      if(rFrameL == ref_frame)
        mvPredType = MVPRED_L;
    }
    else
    {
      if( rFrameUR == ref_frame)
        mvPredType = MVPRED_UR;
    }
  }
  else if(blockshape_x == 16 && blockshape_y == 8)
  {
    if(mb_y == 0)
    {
      if(rFrameU == ref_frame)
        mvPredType = MVPRED_U;
    }
    else
    {
      if(rFrameL == ref_frame)
        mvPredType = MVPRED_L;
    }
  }

  // neighborhood SAD prediction
  if((input->UMHexDSR == 1 || input->BiPredMotionEstimation == 1))
  {
    SAD_a = block_a.available ? ((list==1) ? (fastme_l1_cost_flag[UMHEX_blocktype][block_a.pos_y][block_a.pos_x]) : (fastme_l0_cost_flag[UMHEX_blocktype][block_a.pos_y][block_a.pos_x])) : 0;
    SAD_b = block_b.available ? ((list==1) ? (fastme_l1_cost_flag[UMHEX_blocktype][block_b.pos_y][block_b.pos_x]) : (fastme_l0_cost_flag[UMHEX_blocktype][block_b.pos_y][block_b.pos_x])) : 0;
    SAD_d = block_d.available ? ((list==1) ? (fastme_l1_cost_flag[UMHEX_blocktype][block_d.pos_y][block_d.pos_x]) : (fastme_l0_cost_flag[UMHEX_blocktype][block_d.pos_y][block_d.pos_x])) : 0;
    SAD_c = block_c.available ? ((list==1) ? (fastme_l1_cost_flag[UMHEX_blocktype][block_c.pos_y][block_c.pos_x]) : (fastme_l0_cost_flag[UMHEX_blocktype][block_c.pos_y][block_c.pos_x])) : SAD_d;
  }
  for (hv=0; hv < 2; hv++)
  {
    if (!img->MbaffFrameFlag || hv==0)
    {
      mv_a = block_a.available  ? tmp_mv[block_a.pos_y][block_a.pos_x][hv] : 0;
      mv_b = block_b.available  ? tmp_mv[block_b.pos_y][block_b.pos_x][hv] : 0;
      mv_c = block_c.available  ? tmp_mv[block_c.pos_y][block_c.pos_x][hv] : 0;
    }
    else
    {
      if (img->mb_data[img->current_mb_nr].mb_field)
      {
        mv_a = block_a.available  ? img->mb_data[block_a.mb_addr].mb_field
          ? tmp_mv[block_a.pos_y][block_a.pos_x][hv]
          : tmp_mv[block_a.pos_y][block_a.pos_x][hv] / 2
          : 0;
        mv_b = block_b.available  ? img->mb_data[block_b.mb_addr].mb_field
          ? tmp_mv[block_b.pos_y][block_b.pos_x][hv]
          : tmp_mv[block_b.pos_y][block_b.pos_x][hv] / 2
          : 0;
        mv_c = block_c.available  ? img->mb_data[block_c.mb_addr].mb_field
          ? tmp_mv[block_c.pos_y][block_c.pos_x][hv]
          : tmp_mv[block_c.pos_y][block_c.pos_x][hv] / 2
          : 0;
      }
      else
      {
        mv_a = block_a.available  ? img->mb_data[block_a.mb_addr].mb_field
          ? tmp_mv[block_a.pos_y][block_a.pos_x][hv] * 2
          : tmp_mv[block_a.pos_y][block_a.pos_x][hv]
          : 0;
        mv_b = block_b.available  ? img->mb_data[block_b.mb_addr].mb_field
          ? tmp_mv[block_b.pos_y][block_b.pos_x][hv] * 2
          : tmp_mv[block_b.pos_y][block_b.pos_x][hv]
          : 0;
        mv_c = block_c.available  ? img->mb_data[block_c.mb_addr].mb_field
          ? tmp_mv[block_c.pos_y][block_c.pos_x][hv] * 2
          : tmp_mv[block_c.pos_y][block_c.pos_x][hv]
          : 0;
      }
    }

    switch (mvPredType)
    {
    case MVPRED_MEDIAN:
      if(!(block_b.available || block_c.available))
      {
        pred_vec = mv_a;
      }
      else
      {
        pred_vec = mv_a+mv_b+mv_c-imin(mv_a,imin(mv_b,mv_c))-imax(mv_a,imax(mv_b,mv_c));
      }
      break;
    case MVPRED_L:
      pred_vec = mv_a;
      break;
    case MVPRED_U:
      pred_vec = mv_b;
      break;
    case MVPRED_UR:
      pred_vec = mv_c;
      break;
    default:
      break;
    }

    pmv[hv] = pred_vec;
    //Dynamic Search Range
    if (input->UMHexDSR)
    {
      dsr_mv_avail=block_a.available+block_b.available+block_c.available;
      if(dsr_mv_avail < 2)
      {
        dsr_temp_search_range[hv] = input->search_range;
      }
      else
      {
        dsr_mv_max = imax(iabs(mv_a),imax(iabs(mv_b),iabs(mv_c)));
        dsr_mv_sum = (iabs(mv_a)+iabs(mv_b)+iabs(mv_c));
        if(dsr_mv_sum == 0) dsr_small_search_range = (input->search_range + 4) >> 3;
        else if(dsr_mv_sum > 3 ) dsr_small_search_range = (input->search_range + 2) >>2;
        else dsr_small_search_range = (3*input->search_range + 8) >> 4;
        dsr_temp_search_range[hv]=imin(input->search_range,imax(dsr_small_search_range,dsr_mv_max<<1));
        if(imax(SAD_a,imax(SAD_b,SAD_c)) > Threshold_DSR_MB[UMHEX_blocktype])
          dsr_temp_search_range[hv] = input->search_range;
      }
    }
  }

  //Dynamic Search Range
  if (input->UMHexDSR) {
    dsr_new_search_range = imax(dsr_temp_search_range[0],dsr_temp_search_range[1]);

#ifdef _FULL_SEARCH_RANGE_

    if      (input->full_search == 2) *search_range = dsr_new_search_range;
    else if (input->full_search == 1) *search_range = dsr_new_search_range /  (imin(ref_frame,1)+1);
    else                              *search_range = dsr_new_search_range / ((imin(ref_frame,1)+1) * imin(2,input->blocktype_lut[(blockshape_y >> 2) - 1][(blockshape_x >> 2) - 1]));
#else
    *search_range = dsr_new_search_range / ((imin(ref_frame,1)+1) * imin(2,input->blocktype_lut[(blockshape_y >> 2) - 1][(blockshape_x >> 2) - 1]));
#endif
  }
}

