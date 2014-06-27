
/*!
 *************************************************************************************
 * \file mv-search.c
 *
 * \brief
 *    Motion Vector Search, unified for B and P Pictures
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger                  <stewe@cs.tu-berlin.de>
 *      - Inge Lille-Langoy               <inge.lille-langoy@telenor.com>
 *      - Rickard Sjoberg                 <rickard.sjoberg@era.ericsson.se>
 *      - Stephan Wenger                  <stewe@cs.tu-berlin.de>
 *      - Jani Lainema                    <jani.lainema@nokia.com>
 *      - Detlev Marpe                    <marpe@hhi.de>
 *      - Thomas Wedi                     <wedi@tnt.uni-hannover.de>
 *      - Heiko Schwarz                   <hschwarz@hhi.de>
 *      - Alexis Michael Tourapis         <alexismt@ieee.org>
 *
 *************************************************************************************
*/

#include "contributors.h"

#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

#include "global.h"

#include "image.h"
#include "mv-search.h"
#include "refbuf.h"
#include "memalloc.h"
#include "mb_access.h"
#include "macroblock.h"

// Motion estimation distortion header file
#include "me_distortion.h"

// Motion estimation search algorithms
#include "me_epzs.h"
#include "me_fullfast.h"
#include "me_fullsearch.h"
#include "me_umhex.h"
#include "me_umhexsmp.h"

// Statistics, temporary
int     max_mvd;
short*  spiral_search_x;
short*  spiral_search_y;
short*  spiral_hpel_search_x;
short*  spiral_hpel_search_y;

int*    mvbits;
int*    refbits;
int*    byte_abs;
int**** motion_cost;
int     byte_abs_range;

static int diff  [16];
static int diff64[64];
static imgpel orig_pic [768];
void SetMotionVectorPredictor (short  pmv[2],
                               signed char **refPic,
                               short  ***tmp_mv,
                               short  ref_frame,
                               int    list,
                               int    block_x,
                               int    block_y,
                               int    blockshape_x,
                               int    blockshape_y);

extern ColocatedParams *Co_located;
extern const short block_type_shift_factor[8];

/*!
 ************************************************************************
 * \brief
 *    Set motion vector predictor
 ************************************************************************
 */
void SetMotionVectorPredictor (short  pmv[2],
                               signed char **refPic,
                               short  ***tmp_mv,
                               short  ref_frame,
                               int    list,
                               int    block_x,
                               int    block_y,
                               int    blockshape_x,
                               int    blockshape_y)
{
  int mb_x                 = 4*block_x;
  int mb_y                 = 4*block_y;
  int mb_nr                = img->current_mb_nr;

  int mv_a, mv_b, mv_c, pred_vec=0;
  int mvPredType, rFrameL, rFrameU, rFrameUR;
  int hv;

  PixelPos block_a, block_b, block_c, block_d;

  getLuma4x4Neighbour(mb_nr, mb_x - 1,            mb_y,     &block_a);
  getLuma4x4Neighbour(mb_nr, mb_x,                mb_y - 1, &block_b);
  getLuma4x4Neighbour(mb_nr, mb_x + blockshape_x, mb_y - 1, &block_c);
  getLuma4x4Neighbour(mb_nr, mb_x - 1,            mb_y - 1, &block_d);

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
      rFrameU  = block_b.available
        ? (img->mb_data[block_b.mb_addr].mb_field
        ? refPic[block_b.pos_y][block_b.pos_x] >>1
        : refPic[block_b.pos_y][block_b.pos_x]) : -1;
      rFrameUR = block_c.available
        ? (img->mb_data[block_c.mb_addr].mb_field
        ? refPic[block_c.pos_y][block_c.pos_x] >>1
        : refPic[block_c.pos_y][block_c.pos_x]) : -1;
    }
  }

  /* Prediction if only one of the neighbors uses the reference frame
  *  we are checking
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
  }
}

/*!
************************************************************************
* \brief
*    Initialize the motion search
************************************************************************
*/
void
Init_Motion_Search_Module ()
{
  int bits, i_min, i_max, k;
  int i, l;

  int search_range               = input->search_range;
  int max_search_points          = imax(9, (2*search_range+1)*(2*search_range+1));
  int max_ref_bits               = 1 + 2 * (int)floor(log(imax(16,img->max_num_references+1)) / log(2) + 1e-10);
  int max_ref                    = (1<<((max_ref_bits>>1)+1))-1;
  int number_of_subpel_positions = 4 * (2*search_range+3);
  int max_mv_bits                = 3 + 2 * (int)ceil (log(number_of_subpel_positions+1) / log(2) + 1e-10);
  max_mvd                        = (1<<( max_mv_bits >>1)   )-1;
  byte_abs_range                 = (img->max_imgpel_value > img->max_imgpel_value_uv) ? (img->max_imgpel_value + 1) * 64 : (img->max_imgpel_value_uv + 1) * 64;

  //=====   CREATE ARRAYS   =====
  //-----------------------------
  if ((spiral_search_x = (short*)calloc(max_search_points, sizeof(short))) == NULL)
    no_mem_exit("Init_Motion_Search_Module: spiral_search_x");
  if ((spiral_search_y = (short*)calloc(max_search_points, sizeof(short))) == NULL)
    no_mem_exit("Init_Motion_Search_Module: spiral_search_y");
  if ((spiral_hpel_search_x = (short*)calloc(max_search_points, sizeof(short))) == NULL)
    no_mem_exit("Init_Motion_Search_Module: spiral_hpel_search_x");
  if ((spiral_hpel_search_y = (short*)calloc(max_search_points, sizeof(short))) == NULL)
    no_mem_exit("Init_Motion_Search_Module: spiral_hpel_search_y");
  if ((mvbits = (int*)calloc(2*max_mvd+1, sizeof(int))) == NULL)
    no_mem_exit("Init_Motion_Search_Module: mvbits");
  if ((refbits = (int*)calloc(max_ref, sizeof(int))) == NULL)
    no_mem_exit("Init_Motion_Search_Module: refbits");
  if ((byte_abs = (int*)calloc(byte_abs_range, sizeof(int))) == NULL)
    no_mem_exit("Init_Motion_Search_Module: byte_abs");

  get_mem4Dint (&motion_cost, 8, 2, img->max_num_references, 4);

  //--- set array offsets ---
  mvbits   += max_mvd;
  byte_abs += byte_abs_range/2;

  //=====   INIT ARRAYS   =====
  //---------------------------
  //--- init array: motion vector bits ---
  mvbits[0] = 1;
  for (bits=3; bits<=max_mv_bits; bits+=2)
  {
    i_max = 1    << (bits >> 1);
    i_min = i_max >> 1;

    for (i = i_min; i < i_max; i++)
      mvbits[-i] = mvbits[i] = bits;
  }

  //--- init array: reference frame bits ---
  refbits[0] = 1;
  for (bits=3; bits<=max_ref_bits; bits+=2)
  {
    i_max = (1   << ((bits >> 1) + 1)) - 1;
    i_min = i_max >> 1;

    for (i = i_min; i < i_max; i++)
      refbits[i] = bits;
  }

  //--- init array: absolute value ---
  byte_abs[0] = 0;
  // Set scaler for integer/subpel motion refinement.
  // Currently only EPZS supports subpel positions


  for (i=1; i<byte_abs_range/2; i++)
  {
    byte_abs[i] = byte_abs[-i] = i;
  }

  //--- init array: search pattern ---
  spiral_search_x[0] = spiral_search_y[0] = 0;
  spiral_hpel_search_x[0] = spiral_hpel_search_y[0] = 0;

  for (k=1, l=1; l <= imax(1,search_range); l++)
  {
    for (i=-l+1; i< l; i++)
    {
      spiral_search_x[k] =  i;
      spiral_search_y[k] = -l;
      spiral_hpel_search_x[k] =  i<<1;
      spiral_hpel_search_y[k++] = -l<<1;
      spiral_search_x[k] =  i;
      spiral_search_y[k] =  l;
      spiral_hpel_search_x[k] =  i<<1;
      spiral_hpel_search_y[k++] =  l<<1;
    }
    for (i=-l;   i<=l; i++)
    {
      spiral_search_x[k] = -l;
      spiral_search_y[k] =  i;
      spiral_hpel_search_x[k] = -l<<1;
      spiral_hpel_search_y[k++] = i<<1;
      spiral_search_x[k] =  l;
      spiral_search_y[k] =  i;
      spiral_hpel_search_x[k] =  l<<1;
      spiral_hpel_search_y[k++] = i<<1;
    }
  }

  // set global variable prior to ME
  start_me_refinement_hp = (input->ChromaMEEnable == 1 || input->MEErrorMetric[F_PEL] != input->MEErrorMetric[H_PEL] ) ? 0 : 1;
  start_me_refinement_qp = (input->ChromaMEEnable == 1 || input->MEErrorMetric[H_PEL] != input->MEErrorMetric[Q_PEL] ) ? 0 : 1;

  // Setup Distortion Metrics depending on refinement level
  for (i=0; i<3; i++)
  {
    switch(input->MEErrorMetric[i])
    {
    case ERROR_SAD:
      computeUniPred[i] = computeSAD;
      computeUniPred[i + 3] = computeSADWP;
      computeBiPred1[i] = computeBiPredSAD1;
      computeBiPred2[i] = computeBiPredSAD2;
      break;
    case ERROR_SSE:
      computeUniPred[i] = computeSSE;
      computeUniPred[i + 3] = computeSSEWP;
      computeBiPred1[i] = computeBiPredSSE1;
      computeBiPred2[i] = computeBiPredSSE2;
      break;
    case ERROR_SATD :
    default:
      computeUniPred[i] = computeSATD;
      computeUniPred[i + 3] = computeSATDWP;
      computeBiPred1[i] = computeBiPredSATD1;
      computeBiPred2[i] = computeBiPredSATD2;
      break;
    }
  }
  // Setup buffer access methods
  get_line[0] = FastLine4X;
  get_line[1] = UMVLine4X;
  get_crline[0] = FastLine8X_chroma;
  get_crline[1] = UMVLine8X_chroma;

  if(input->SearchMode == FAST_FULL_SEARCH)
    InitializeFastFullIntegerSearch ();
}


/*!
 ************************************************************************
 * \brief
 *    Free memory used by motion search
 ************************************************************************
 */
void
Clear_Motion_Search_Module ()
{
  //--- correct array offset ---
  mvbits   -= max_mvd;
  byte_abs -= byte_abs_range/2;

  //--- delete arrays ---
  free (spiral_search_x);
  free (spiral_search_y);
  free (spiral_hpel_search_x);
  free (spiral_hpel_search_y);
  free (mvbits);
  free (refbits);
  free (byte_abs);
  free_mem4Dint (motion_cost, 8, 2);

  if(input->SearchMode == FAST_FULL_SEARCH)
    ClearFastFullIntegerSearch ();
}

/*!
 ***********************************************************************
 * \brief
 *    Motion Cost for Bidirectional modes
 ***********************************************************************
 */
int BPredPartitionCost (int   blocktype,
                        int   block8x8,
                        short ref_l0,
                        short ref_l1,
                        int   lambda_factor,
                        int   list)
{
  static int  bx0[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,2,0,0}, {0,2,0,2}};
  static int  by0[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,2,0,0}, {0,0,0,0}, {0,0,2,2}};
  
  int   curr_blk[MB_BLOCK_SIZE][MB_BLOCK_SIZE]; // ABT pred.error buffer
  int   bsx       = imin(input->blc_size[blocktype][0],8);
  int   bsy       = imin(input->blc_size[blocktype][1],8);

  int   pic_pix_x, pic_pix_y, block_x, block_y;
  int   v, h, mcost, i, j, k;
  int   mvd_bits  = 0;
  int   parttype  = (blocktype<4?blocktype:4);
  int   step_h0   = (input->part_size[ parttype][0]);
  int   step_v0   = (input->part_size[ parttype][1]);
  int   step_h    = (input->part_size[blocktype][0]);
  int   step_v    = (input->part_size[blocktype][1]);
  int   bxx, byy;                               // indexing curr_blk

  short   ******all_mv = list ? img->bipred_mv1 : img->bipred_mv2;
  short   ******  p_mv = img->pred_mv;

  for (v=by0[parttype][block8x8]; v<by0[parttype][block8x8]+step_v0; v+=step_v)
  {
    for (h=bx0[parttype][block8x8]; h<bx0[parttype][block8x8]+step_h0; h+=step_h)
    {
      mvd_bits += mvbits[ all_mv [v][h][LIST_0][ref_l0][blocktype][0] - p_mv[v][h][LIST_0][ref_l0][blocktype][0] ];
      mvd_bits += mvbits[ all_mv [v][h][LIST_0][ref_l0][blocktype][1] - p_mv[v][h][LIST_0][ref_l0][blocktype][1] ];

      mvd_bits += mvbits[ all_mv [v][h][LIST_1][ref_l1][blocktype][0] - p_mv[v][h][LIST_1][ref_l1][blocktype][0] ];
      mvd_bits += mvbits[ all_mv [v][h][LIST_1][ref_l1][blocktype][1] - p_mv[v][h][LIST_1][ref_l1][blocktype][1] ];
    }
  }
    mcost = WEIGHTED_COST (lambda_factor, mvd_bits);

    //----- cost of residual signal -----
    for (byy=0, v=by0[parttype][block8x8]; v<by0[parttype][block8x8]+step_v0; byy+=4, v++)
    {

      pic_pix_y = img->opix_y + (block_y = (v<<2));
      for (bxx=0, h=bx0[parttype][block8x8]; h<bx0[parttype][block8x8]+step_h0; bxx+=4, h++)
      {
        pic_pix_x = img->opix_x + (block_x = (h<<2));
        LumaPrediction4x4Bi (block_x, block_y, blocktype, blocktype, ref_l0, ref_l1, list);

        for (k=j=0; j<4; j++)
        {
          for (  i=0; i<4; i++)
            diff64[k++] = curr_blk[byy+j][bxx+i] =
            imgY_org[pic_pix_y+j][pic_pix_x+i] - img->mpr[j+block_y][i+block_x];
        }
        if ((!input->Transform8x8Mode) || (blocktype>4))
        {
          mcost += distortion4x4 (diff64);
        }
      }
    }
    if (input->Transform8x8Mode && (blocktype<=4))  // tchen 4-29-04
    {
      for (byy=0; byy < input->blc_size[parttype][1]; byy+=bsy)
        for (bxx=0; bxx<input->blc_size[parttype][0]; bxx+=bsx)
        {
          for (k=0, j=byy;j<byy + 8;j++, k += 8)
            memcpy(&diff64[k], &(curr_blk[j][bxx]), 8 * sizeof(int));

          mcost += distortion8x8(diff64);
        }
    }
    return mcost;
}


/*!
 ***********************************************************************
 * \brief
 *    Block motion search
 ***********************************************************************
 */
int                                         //!< minimum motion cost after search
BlockMotionSearch (short     ref,           //!< reference idx
                   int       list,          //!< reference pciture list
                   int       mb_x,          //!< x-coordinate inside macroblock
                   int       mb_y,          //!< y-coordinate inside macroblock
                   int       blocktype,     //!< block type (1-16x16 ... 7-4x4)
                   int       search_range,  //!< 1-d search range for integer-position search
                   int*      lambda_factor) //!< lagrangian parameter for determining motion cost
{
  // each 48-pel line stores the 16 luma pels (at 0) followed by 8 or 16 crcb[0] (at 16) and crcb[1] (at 32) pels
  // depending on the type of chroma subsampling used: YUV 4:4:4, 4:2:2, and 4:2:0
  imgpel *orig_pic_tmp = orig_pic;

  short     mv[2];
  int       i, j;

  int       max_value = INT_MAX;
  int       min_mcost = max_value;

  int       block_x   = (mb_x>>2);
  int       block_y   = (mb_y>>2);

  int       bsx       = input->blc_size[blocktype][0];
  int       bsy       = input->blc_size[blocktype][1];

  int       pic_pix_x = img->opix_x + mb_x;
  int       pic_pix_y = img->opix_y + mb_y;

  int pic_pix_x_c = pic_pix_x >> (chroma_shift_x - 2);
  int pic_pix_y_c = pic_pix_y >> (chroma_shift_y - 2);
  int bsx_c = bsx >> (chroma_shift_x - 2);
  int bsy_c = bsy >> (chroma_shift_y - 2);

  short*    pred_mv = img->pred_mv[block_y][block_x][list][ref][blocktype];
  short****** all_mv    = img->all_mv;
  int list_offset = ((img->MbaffFrameFlag) && (img->mb_data[img->current_mb_nr].mb_field)) ? img->current_mb_nr % 2 ? 4 : 2 : 0;
  int *prevSad = (input->SearchMode == EPZS)? EPZSDistortion[list + list_offset][blocktype - 1]: NULL;

#if GET_METIME
  static struct TIMEB tstruct1;
  static struct TIMEB tstruct2;
  time_t me_tmp_time;

  ftime( &tstruct1 );    // start time ms
#endif
  //==================================
  //=====   GET ORIGINAL BLOCK   =====
  //==================================
  for (j = 0; j < bsy; j++)
  {
    memcpy(orig_pic_tmp,&imgY_org[pic_pix_y+j][pic_pix_x], bsx *sizeof(imgpel));
    orig_pic_tmp += bsx;
  }
  ChromaMEEnable = input->ChromaMEEnable;

  if ( ChromaMEEnable )
  {
    // copy the original cmp1 and cmp2 data to the orig_pic matrix
    orig_pic_tmp = orig_pic + 256;
    for (j = 0; j < bsy_c; j++)
    {
      memcpy(orig_pic_tmp, &(imgUV_org[0][pic_pix_y_c+j][pic_pix_x_c]), bsx_c *sizeof(imgpel));
      orig_pic_tmp += bsx_c;
    }
    orig_pic_tmp = orig_pic + 512;
    for (j = 0; j < bsy_c; j++)
    {
      memcpy(orig_pic_tmp, &(imgUV_org[1][pic_pix_y_c+j][pic_pix_x_c]), bsx_c *sizeof(imgpel));
      orig_pic_tmp += bsx_c;
    }
  }


  if(input->SearchMode == UM_HEX)
  {
    UMHEX_blocktype = blocktype;
    bipred_flag = 0;
  }
  else if (input->SearchMode == UM_HEX_SIMPLE)
  {
    smpUMHEX_setup(ref, list, block_y, block_x, blocktype, all_mv );
  }

  // Set if 8x8 transform will be used if SATD is used
  test8x8transform = input->Transform8x8Mode && blocktype <= 4;

  //===========================================
  //=====   GET MOTION VECTOR PREDICTOR   =====
  //===========================================

  if (input->SearchMode == UM_HEX)
    UMHEXSetMotionVectorPredictor(pred_mv, enc_picture->ref_idx[list], enc_picture->mv[list], ref, list, block_x, block_y, bsx, bsy, &search_range);
  else
    SetMotionVectorPredictor (pred_mv, enc_picture->ref_idx[list], enc_picture->mv[list], ref, list, block_x, block_y, bsx, bsy);

  //==================================
  //=====   INTEGER-PEL SEARCH   =====
  //==================================

  if (input->SearchMode == UM_HEX)
  {
    mv[0] = pred_mv[0] / 4;
    mv[1] = pred_mv[1] / 4;

    if (!input->rdopt)
    {
      //--- adjust search center so that the (0,0)-vector is inside ---
      mv[0] = iClip3(-search_range, search_range, mv[0]);
      mv[1] = iClip3(-search_range, search_range, mv[1]);
    }

    mv[0] = iClip3(-2047 + search_range, 2047 - search_range, mv[0]);
    mv[1] = iClip3(LEVELMVLIMIT[img->LevelIndex][0] + search_range, LEVELMVLIMIT[img->LevelIndex][1]  - search_range, mv[1]);

    min_mcost = UMHEXIntegerPelBlockMotionSearch(orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
      pred_mv[0], pred_mv[1], &mv[0], &mv[1], search_range,
      min_mcost, lambda_factor[F_PEL]);
  }
  else if (input->SearchMode == UM_HEX_SIMPLE)
  {
    mv[0] = pred_mv[0] / 4;
    mv[1] = pred_mv[1] / 4;

    if (!input->rdopt)
    {
      //--- adjust search center so that the (0,0)-vector is inside ---
      mv[0] = iClip3(-search_range, search_range, mv[0]);
      mv[1] = iClip3(-search_range, search_range, mv[1]);
    }

    mv[0] = iClip3(-2047 + search_range, 2047 - search_range, mv[0]);
    mv[1] = iClip3(LEVELMVLIMIT[img->LevelIndex][0] + search_range, LEVELMVLIMIT[img->LevelIndex][1]  - search_range, mv[1]);


    min_mcost = smpUMHEXIntegerPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
                                                 pred_mv[0], pred_mv[1], &mv[0], &mv[1], search_range,
                                                 min_mcost, lambda_factor[F_PEL]);
    for (i=0; i < (bsx>>2); i++)
    {
      for (j=0; j < (bsy>>2); j++)
      {
        if(list == 0)
        {
         smpUMHEX_l0_cost[blocktype][(img->pix_y>>2)+block_y+j][(img->pix_x>>2)+block_x+i] = min_mcost;
        }
        else
        {
          smpUMHEX_l1_cost[blocktype][(img->pix_y>>2)+block_y+j][(img->pix_x>>2)+block_x+i] = min_mcost;
        }
      }
    }
  }
  //--- perform motion search using EPZS schemes---
  else if (input->SearchMode == EPZS)
  {
    //--- set search center ---
    // This has to be modified in the future
    if (input->EPZSSubPelGrid)
    {
      mv[0] = pred_mv[0];
      mv[1] = pred_mv[1];
    }
    else
    {
      mv[0] = (pred_mv[0] + 2)>> 2;
      mv[1] = (pred_mv[1] + 2)>> 2;
    }

    if (!input->rdopt)
    {
      //--- adjust search center so that the (0,0)-vector is inside ---
      mv[0] = iClip3 (-search_range<<(input->EPZSSubPelGrid * 2), search_range<<(input->EPZSSubPelGrid * 2), mv[0]);
      mv[1] = iClip3 (-search_range<<(input->EPZSSubPelGrid * 2), search_range<<(input->EPZSSubPelGrid * 2), mv[1]);
    }

    // valid search range limits could be precomputed once during the initialization process
    mv[0] = iClip3((-2047 + search_range)<<(input->EPZSSubPelGrid * 2), (2047 - search_range)<<(input->EPZSSubPelGrid * 2), mv[0]);
    mv[1] = iClip3((LEVELMVLIMIT[img->LevelIndex][0] + search_range)<<(input->EPZSSubPelGrid * 2),
      (LEVELMVLIMIT[img->LevelIndex][1]  - search_range)<<(input->EPZSSubPelGrid * 2), mv[1]);

    min_mcost = EPZSPelBlockMotionSearch (orig_pic, ref, list, list_offset,
      enc_picture->ref_idx, enc_picture->mv, pic_pix_x, pic_pix_y, blocktype,
      pred_mv, mv, search_range<<(input->EPZSSubPelGrid * 2), min_mcost, lambda_factor[F_PEL]);

  }
  else if (input->SearchMode == FAST_FULL_SEARCH)
  {
    // comments:   - orig_pic is not used  -> be careful
    //             - search center is automatically determined
    min_mcost = FastFullPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
      pred_mv[0], pred_mv[1], &mv[0], &mv[1], search_range,
    min_mcost, lambda_factor[F_PEL]);
  }
  else
  {
    //--- set search center ---
    mv[0] = pred_mv[0] / 4;
    mv[1] = pred_mv[1] / 4;
    if (!input->rdopt)
    {
      //--- adjust search center so that the (0,0)-vector is inside ---
      mv[0] = iClip3 (-search_range, search_range, mv[0]);
      mv[1] = iClip3 (-search_range, search_range, mv[1]);
    }

    mv[0] = iClip3(-2047 + search_range, 2047 - search_range, mv[0]);
    mv[1] = iClip3(LEVELMVLIMIT[img->LevelIndex][0] + search_range, LEVELMVLIMIT[img->LevelIndex][1]  - search_range, mv[1]);

    //--- perform motion search ---
    min_mcost = FullPelBlockMotionSearch     (orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
      pred_mv[0], pred_mv[1], &mv[0], &mv[1], search_range,
      min_mcost, lambda_factor[F_PEL]);
  }
  //===== convert search center to quarter-pel units =====
  if (input->EPZSSubPelGrid == 0 || input->SearchMode != EPZS)
  {
    mv[0] <<= 2;
    mv[1] <<= 2;
  }
  //==============================
  //=====   SUB-PEL SEARCH   =====
  //==============================
  ChromaMEEnable = (input->ChromaMEEnable == ME_YUV_FP_SP ) ? 1 : 0; // set it externally

  if (!input->DisableSubpelME)
  {
    if (input->SearchMode != EPZS || (ref == 0 || img->structure != FRAME || (ref > 0 && min_mcost < 3.5 * prevSad[pic_pix_x >> 2])))
    {
      if ( !start_me_refinement_hp )
      {
        min_mcost = max_value;
      }

      if (input->SearchMode == UM_HEX)
      {
        if(blocktype >3)
        {
          min_mcost =  UMHEXSubPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
                       pred_mv[0], pred_mv[1], &mv[0], &mv[1], 9, 9, min_mcost, lambda_factor[Q_PEL]);
        }
        else
        {
          min_mcost =  SubPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
                       pred_mv[0], pred_mv[1], &mv[0], &mv[1], 9, 9, min_mcost, lambda_factor);
        }
      }
      else if (input->SearchMode == UM_HEX_SIMPLE)
      {
        if(blocktype > 1)
        {
          min_mcost =  smpUMHEXSubPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y,
                       blocktype, pred_mv[0], pred_mv[1], &mv[0], &mv[1], 9, 9, min_mcost, lambda_factor[Q_PEL]);
        }
        else
        {
          min_mcost =  smpUMHEXFullSubPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y,
                       blocktype, pred_mv[0], pred_mv[1], &mv[0], &mv[1], 9, 9, min_mcost, lambda_factor[Q_PEL]);
        }
      }
      else if (input->SearchMode == EPZS && input->EPZSSubPelME)
      {
          min_mcost =  EPZSSubPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
                       pred_mv, mv, 9, 9, min_mcost, lambda_factor);
      }
      else
      {
          min_mcost =  SubPelBlockMotionSearch (orig_pic, ref, list, pic_pix_x, pic_pix_y, blocktype,
          pred_mv[0], pred_mv[1], &mv[0], &mv[1], 9, 9, min_mcost, lambda_factor);
      }
    }
  }

  if (!input->rdopt)
  {
    // Get the skip mode cost
    if (blocktype == 1 && (img->type == P_SLICE||img->type == SP_SLICE))
    {
      int cost;

      FindSkipModeMotionVector ();

      cost  = GetSkipCostMB ();
      cost -= ((lambda_factor[Q_PEL] + 4096) >> 13);

      if (cost < min_mcost)
      {
        min_mcost = cost;
        mv[0]      = img->all_mv [0][0][0][0][0][0];
        mv[1]      = img->all_mv [0][0][0][0][0][1];
      }
    }
  }

  //===============================================
  //=====   SET MV'S AND RETURN MOTION COST   =====
  //===============================================

  for (j=block_y; j < block_y + (bsy>>2); j++)
  {
    for (i=block_x; i < block_x + (bsx>>2); i++)
    {
      all_mv[j][i][list][ref][blocktype][0] = mv[0];
      all_mv[j][i][list][ref][blocktype][1] = mv[1];
    }
  }

  if (img->type==B_SLICE && input->BiPredMotionEstimation!=0 && (blocktype == 1) && (ref==0))
  {
    short   ******bipred_mv = list ? img->bipred_mv1 : img->bipred_mv2;
    int     min_mcostbi = max_value;
    short   bimv[2] = {0, 0}, tempmv[2] = {0, 0};
    short   *pred_mv1 = NULL;
    short   *pred_mv2 = NULL;
    short   iterlist=list;
    short   pred_mv_bi[2];

    if (input->SearchMode == UM_HEX)
    {
      bipred_flag = 1;
      UMHEXSetMotionVectorPredictor(pred_mv_bi, enc_picture->ref_idx[list ^ 1], enc_picture->mv[(list == LIST_0? LIST_1: LIST_0)], 0, (list == LIST_0? LIST_1: LIST_0), block_x, block_y, bsx, bsy, &search_range);
    }
    else
      SetMotionVectorPredictor     (pred_mv_bi, enc_picture->ref_idx[list ^ 1], enc_picture->mv[(list == LIST_0? LIST_1: LIST_0)], 0, (list == LIST_0? LIST_1: LIST_0), block_x, block_y, bsx, bsy);

    if ((input->SearchMode != EPZS) || (input->EPZSSubPelGrid == 0))
    {
      mv[0]=(mv[0] + 2)>>2;
      mv[1]=(mv[1] + 2)>>2;
    }

    //Bi-predictive motion Refinements
    for (i=0;i<=input->BiPredMERefinements;i++)
    {
      if (i%2)
      {
        pred_mv2=pred_mv;
        pred_mv1=pred_mv_bi;
        tempmv[0]=bimv[0];
        tempmv[1]=bimv[1];
        bimv[0]=mv[0];
        bimv[1]=mv[1];
        iterlist= list ^ 1;
      }
      else
      {
        pred_mv1=pred_mv;
        pred_mv2=pred_mv_bi;

        if (i!=0)
        {
          tempmv[0]=bimv[0];
          tempmv[1]=bimv[1];
          bimv[0]=mv[0];
          bimv[1]=mv[1];
        }
        else
        {
          tempmv[0]=mv[0];
          tempmv[1]=mv[1];
          if ((input->SearchMode != EPZS) || (input->EPZSSubPelGrid == 0))
          {
            bimv[0] = (pred_mv2[0] + 2)>>2;
            bimv[1] = (pred_mv2[1] + 2)>>2;
          }
          else
          {
            bimv[0] = pred_mv2[0];
            bimv[1] = pred_mv2[1];
          }
        }

        iterlist=list;
      }
      mv[0]=bimv[0];
      mv[1]=bimv[1];

      if (input->SearchMode == EPZS)
      {
        min_mcostbi = EPZSBiPredBlockMotionSearch (orig_pic, ref, iterlist,
          list_offset, enc_picture->ref_idx, enc_picture->mv,
          pic_pix_x, pic_pix_y, blocktype,
          pred_mv1, pred_mv2, bimv, tempmv,
          (input->BiPredMESearchRange<<(input->EPZSSubPelGrid * 2))>>i, min_mcostbi, lambda_factor[F_PEL]);
      }
      else if(input->SearchMode == UM_HEX)
      {
        min_mcostbi = UMHEXBipredIntegerPelBlockMotionSearch (orig_pic, ref, iterlist,
          pic_pix_x, pic_pix_y, blocktype,
          pred_mv1[0], pred_mv1[1], pred_mv2[0], pred_mv2[1],
          &bimv[0], &bimv[1], &tempmv[0], &tempmv[1],
          input->BiPredMESearchRange>>i, min_mcostbi, lambda_factor[F_PEL]);
      }
      else if(input->SearchMode == UM_HEX_SIMPLE)
      {
        min_mcostbi = smpUMHEXBipredIntegerPelBlockMotionSearch (orig_pic, ref, iterlist,
          pic_pix_x, pic_pix_y, blocktype,
          pred_mv[0], pred_mv[1], pred_mv[0], pred_mv[1],
          &bimv[0], &bimv[1], &tempmv[0], &tempmv[1],
          input->BiPredMESearchRange>>i, min_mcostbi, lambda_factor[F_PEL]);
      }
      else
      {
        min_mcostbi = FullPelBlockMotionBiPred (orig_pic, ref, iterlist,
          pic_pix_x, pic_pix_y, blocktype,
          pred_mv1[0], pred_mv1[1], pred_mv2[0], pred_mv2[1],
          &bimv[0], &bimv[1], &tempmv[0], &tempmv[1],
          input->BiPredMESearchRange>>i, min_mcostbi, lambda_factor[F_PEL]);
      }
      if ((mv[0] == bimv[0]) && (mv[1] == bimv[1]))
      {
        //mv[0]=tempmv[0];
        //mv[1]=tempmv[1];
        //break;
      }

      mv[0]=tempmv[0];
      mv[1]=tempmv[1];
    }
    if ((input->SearchMode != EPZS) || (input->EPZSSubPelGrid == 0))
    {
      mv[0]=tempmv[0] << 2;
      mv[1]=tempmv[1] << 2;
      bimv[0] = bimv[0] << 2;
      bimv[1] = bimv[1] << 2;
    }

    if (input->BiPredMESubPel && !input->DisableSubpelME)
    {
      if ( !start_me_refinement_hp )
      {
        min_mcostbi = max_value;
      }

      if (input->SearchMode == EPZS && input->EPZSSubPelMEBiPred)
      {
        min_mcostbi =  EPZSSubPelBlockSearchBiPred (orig_pic, ref, iterlist, pic_pix_x, pic_pix_y, blocktype,
          pred_mv2, pred_mv1, bimv, mv, 9, 9, min_mcostbi, lambda_factor);
      }
      else
      {
        min_mcostbi =  SubPelBlockSearchBiPred (orig_pic, ref, iterlist, pic_pix_x, pic_pix_y, blocktype,
          pred_mv2[0], pred_mv2[1], &bimv[0], &bimv[1], &mv[0], &mv[1], 9, 9,
          min_mcostbi, lambda_factor);
      }
    }

    if (input->BiPredMESubPel==2 && !input->DisableSubpelME)
    {
      if ( !start_me_refinement_hp || !start_me_refinement_qp)
      {
        min_mcostbi = max_value;
      }

      if (input->SearchMode == EPZS && input->EPZSSubPelMEBiPred)
      {
        min_mcostbi =  EPZSSubPelBlockSearchBiPred (orig_pic, ref, iterlist ^ 1, pic_pix_x, pic_pix_y, blocktype,
          pred_mv1, pred_mv2, mv, bimv, 9, 9, min_mcostbi, lambda_factor);
      }
      else
      {
        min_mcostbi =  SubPelBlockSearchBiPred (orig_pic, ref, iterlist ^ 1, pic_pix_x, pic_pix_y, blocktype,
          pred_mv1[0], pred_mv1[1], &mv[0], &mv[1], &bimv[0], &bimv[1], 9, 9,
          min_mcostbi, lambda_factor);
      }
    }

    for (j=block_y; j < block_y + (bsy>>2); j++)
    {
      for (i=block_x ; i < block_x + (bsx>>2); i++)
      {
        bipred_mv[j][i][iterlist    ][0][blocktype][0] = mv[0];
        bipred_mv[j][i][iterlist    ][0][blocktype][1] = mv[1];
        bipred_mv[j][i][iterlist ^ 1][0][blocktype][0] = bimv[0];
        bipred_mv[j][i][iterlist ^ 1][0][blocktype][1] = bimv[1];
      }
    }
  }

#if GET_METIME
  ftime(&tstruct2);   // end time ms
  me_tmp_time=(tstruct2.time*1000+tstruct2.millitm) - (tstruct1.time*1000+tstruct1.millitm);
  me_tot_time += me_tmp_time;
  me_time += me_tmp_time;
#endif
  return min_mcost;
}


/*!
 ***********************************************************************
 * \brief
 *    Motion Cost for Bidirectional modes
 ***********************************************************************
 */
int BIDPartitionCost (int   blocktype,
                      int   block8x8,
                      short ref_l0,
                      short ref_l1,
                      int   lambda_factor)
{
  static int  bx0[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,2,0,0}, {0,2,0,2}};
  static int  by0[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,2,0,0}, {0,0,0,0}, {0,0,2,2}};
  
  int   curr_blk[MB_BLOCK_SIZE][MB_BLOCK_SIZE]; // ABT pred.error buffer
  int   bsx       = imin(input->blc_size[blocktype][0],8);
  int   bsy       = imin(input->blc_size[blocktype][1],8);

  int   pic_pix_x, pic_pix_y, block_x, block_y;
  int   v, h, mcost, i, j, k;
  int   mvd_bits  = 0;
  int   parttype  = (blocktype<4?blocktype:4);
  int   step_h0   = (input->part_size[ parttype][0]);
  int   step_v0   = (input->part_size[ parttype][1]);
  int   step_h    = (input->part_size[blocktype][0]);
  int   step_v    = (input->part_size[blocktype][1]);
  int   bxx, byy;                               // indexing curr_blk
  int   bx = bx0[parttype][block8x8];
  int   by = by0[parttype][block8x8];
  short   ******all_mv = img->all_mv;
  short   ******  p_mv = img->pred_mv;

  //----- cost for motion vector bits -----
  for (v=by; v<by + step_v0; v+=step_v)
  {
    for (h=bx; h<bx + step_h0; h+=step_h)
    {
      mvd_bits += mvbits[ all_mv [v][h][LIST_0][ref_l0][blocktype][0] - p_mv[v][h][LIST_0][ref_l0][blocktype][0] ];
      mvd_bits += mvbits[ all_mv [v][h][LIST_0][ref_l0][blocktype][1] - p_mv[v][h][LIST_0][ref_l0][blocktype][1] ];

      mvd_bits += mvbits[ all_mv [v][h][LIST_1][ref_l1][blocktype][0] - p_mv[v][h][LIST_1][ref_l1][blocktype][0] ];
      mvd_bits += mvbits[ all_mv [v][h][LIST_1][ref_l1][blocktype][1] - p_mv[v][h][LIST_1][ref_l1][blocktype][1] ];
    }
  }

  mcost = WEIGHTED_COST (lambda_factor, mvd_bits);

  //----- cost of residual signal -----
  for (byy=0, v=by; v<by + step_v0; byy+=4, v++)
  {
    pic_pix_y = img->opix_y + (block_y = (v<<2));
    for (bxx=0, h=bx; h<bx + step_h0; bxx+=4, h++)
    {
      pic_pix_x = img->opix_x + (block_x = (h<<2));
      LumaPrediction4x4 (block_x, block_y, 2, blocktype, blocktype, ref_l0, ref_l1);

      for (k=j=0; j<4; j++)
      {
        for (  i=0; i<4; i++)
          diff64[k++] = curr_blk[byy+j][bxx+i] =
          imgY_org[pic_pix_y+j][pic_pix_x+i] - img->mpr[j+block_y][i+block_x];
      }
      if ((!input->Transform8x8Mode) || (blocktype>4))
        mcost += distortion4x4 (diff64);
    }
  }
  if (input->Transform8x8Mode && (blocktype<=4))  // tchen 4-29-04
  {
    for (byy=0; byy < input->blc_size[parttype][1]; byy+=bsy)
      for (bxx=0; bxx<input->blc_size[parttype][0]; bxx+=bsx)
      {
        for (k=0, j=byy;j<byy + 8;j++, k += 8)
          memcpy(&diff64[k], &(curr_blk[j][bxx]), 8 * sizeof(int));

        mcost += distortion8x8(diff64);
      }
  }
  return mcost;
}

/*!
 ************************************************************************
 * \brief
 *    Get cost for skip mode for an macroblock
 ************************************************************************
 */
int GetSkipCostMB (void)
{
  int block_y, block_x, pic_pix_y, pic_pix_x, i, j, k;
  int cost = 0;

  int curr_diff[8][8];
  int mb_x, mb_y;
  int block;
  for(block=0;block<4;block++)
  {
    mb_y    = (block/2)<<3;
    mb_x    = (block%2)<<3;
    for (block_y=mb_y; block_y<mb_y+8; block_y+=4)
    {
      pic_pix_y = img->opix_y + block_y;
      for (block_x=mb_x; block_x<mb_x+8; block_x+=4)
      {
        pic_pix_x = img->opix_x + block_x;

        //===== prediction of 4x4 block =====
        LumaPrediction4x4 (block_x, block_y, 0, 0, 0, 0, 0);

        //===== get displaced frame difference ======
        for (k=j=0; j<4; j++)
          for (i=0; i<4; i++, k++)
          {
            diff[k] = curr_diff[block_y-mb_y+j][block_x-mb_x+i] = imgY_org[pic_pix_y+j][pic_pix_x+i] - img->mpr[j+block_y][i+block_x];
          }

          if(!((input->rdopt==0)&&(input->Transform8x8Mode)))
            cost += distortion4x4 (diff);
      }
    }

    if((input->rdopt==0)&&(input->Transform8x8Mode))
    {
      for(k=j=0; j<8; j++, k+=8)
        memcpy(&diff64[k], &(curr_diff[j]), 8 * sizeof(int));
      cost += distortion8x8 (diff64);
    }
  }

  return cost;
}

/*!
 ************************************************************************
 * \brief
 *    Find motion vector for the Skip mode
 ************************************************************************
 */
void FindSkipModeMotionVector ()
{
  int   bx, by;
  short ******all_mv = img->all_mv;

  short pmv[2];

  int zeroMotionAbove;
  int zeroMotionLeft;
  PixelPos mb_a, mb_b;
  int      a_mv_y = 0;
  int      a_ref_idx = 0;
  int      b_mv_y = 0;
  int      b_ref_idx = 0;
  short    ***mv = enc_picture->mv[LIST_0];

  Macroblock *currMB = &img->mb_data[img->current_mb_nr];

  getLuma4x4Neighbour(img->current_mb_nr,-1, 0, &mb_a);
  getLuma4x4Neighbour(img->current_mb_nr, 0,-1, &mb_b);

  if (mb_a.available)
  {
    a_mv_y    = mv[mb_a.pos_y][mb_a.pos_x][1];
    a_ref_idx = enc_picture->ref_idx[LIST_0][mb_a.pos_y][mb_a.pos_x];

    if (currMB->mb_field && !img->mb_data[mb_a.mb_addr].mb_field)
    {
      a_mv_y    /=2;
      a_ref_idx *=2;
    }
    if (!currMB->mb_field && img->mb_data[mb_a.mb_addr].mb_field)
    {
      a_mv_y    *=2;
      a_ref_idx >>=1;
    }
  }

  if (mb_b.available)
  {
    b_mv_y    = mv[mb_b.pos_y][mb_b.pos_x][1];
    b_ref_idx = enc_picture->ref_idx[LIST_0][mb_b.pos_y][mb_b.pos_x];

    if (currMB->mb_field && !img->mb_data[mb_b.mb_addr].mb_field)
    {
      b_mv_y    /=2;
      b_ref_idx *=2;
    }
    if (!currMB->mb_field && img->mb_data[mb_b.mb_addr].mb_field)
    {
      b_mv_y    *=2;
      b_ref_idx >>=1;
    }
  }

  zeroMotionLeft  = !mb_a.available ? 1 : a_ref_idx==0 && mv[mb_a.pos_y][mb_a.pos_x][0]==0 && a_mv_y==0 ? 1 : 0;
  zeroMotionAbove = !mb_b.available ? 1 : b_ref_idx==0 && mv[mb_b.pos_y][mb_b.pos_x][0]==0 && b_mv_y==0 ? 1 : 0;

  if (zeroMotionAbove || zeroMotionLeft)
  {
    for (by = 0;by < 4;by++)
      for (bx = 0;bx < 4;bx++)
      {
        memset(all_mv [by][bx][0][0][0], 0, 2* sizeof(short));
        //all_mv [by][bx][0][0][0][0] = 0;
        //all_mv [by][bx][0][0][0][1] = 0;
      }
  }
  else
  {
    SetMotionVectorPredictor (pmv, enc_picture->ref_idx[LIST_0], mv, 0, LIST_0, 0, 0, 16, 16);
    for (by = 0;by < 4;by++)
      for (bx = 0;bx < 4;bx++)
      {
        memcpy(all_mv [by][bx][0][0][0], pmv, 2* sizeof(short));
      }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Get cost for direct mode for an 8x8 block
 ************************************************************************
 */
int GetDirectCost8x8 (int block, int *cost8x8)
{
  int block_y, block_x, pic_pix_y, pic_pix_x, i, j, k;
  int curr_diff[8][8];
  int cost  = 0;
  int mb_y  = (block/2)<<3;
  int mb_x  = (block%2)<<3;

  for (block_y=mb_y; block_y<mb_y+8; block_y+=4)
  {
    pic_pix_y = img->opix_y + block_y;

    for (block_x=mb_x; block_x<mb_x+8; block_x+=4)
    {
      pic_pix_x = img->opix_x + block_x;

      if (direct_pdir[pic_pix_y>>2][pic_pix_x>>2]<0)
      {
        *cost8x8=INT_MAX;
        return INT_MAX; //mode not allowed
      }

      //===== prediction of 4x4 block =====

      LumaPrediction4x4 (block_x, block_y, direct_pdir[pic_pix_y>>2][pic_pix_x>>2], 0, 0,
        direct_ref_idx[LIST_0][pic_pix_y>>2][pic_pix_x>>2],
        direct_ref_idx[LIST_1][pic_pix_y>>2][pic_pix_x>>2]);

      //===== get displaced frame difference ======
      for (k=j=0; j<4; j++)
        for (i=0; i<4; i++, k++)
        {
          diff[k] = curr_diff[block_y-mb_y+j][block_x-mb_x+i] =
            imgY_org[pic_pix_y+j][pic_pix_x+i] - img->mpr[j+block_y][i+block_x];
        }

        cost += distortion4x4 (diff);
    }
  }

  if((input->rdopt==0)&&(input->Transform8x8Mode))
  {
    k=0;
    for(j=0; j<8; j++, k+=8)
      memcpy(&diff64[k], &(curr_diff[j]), 8 * sizeof(int));          

    *cost8x8 += distortion8x8 (diff64);
  }

  return cost;
}



/*!
 ************************************************************************
 * \brief
 *    Get cost for direct mode for an macroblock
 ************************************************************************
 */
int GetDirectCostMB (void)
{
  int i;
  int cost = 0;
  int cost8x8 = 0;

  for (i=0; i<4; i++)
  {
    cost += GetDirectCost8x8 (i, &cost8x8);
    if (cost8x8 == INT_MAX) return INT_MAX;
  }

  switch(input->Transform8x8Mode)
  {
  case 1: // Mixture of 8x8 & 4x4 transform
    if((cost8x8 < cost)||
      !(input->InterSearch8x4 &&
      input->InterSearch4x8 &&
      input->InterSearch4x4)
      )
    {
      cost = cost8x8; //return 8x8 cost
    }
    break;
  case 2: // 8x8 Transform only
    cost = cost8x8;
    break;
  default: // 4x4 Transform only
    break;
  }

  return cost;
  // T.Nishi(MEI ACC) 04-28-2004 end
}


/*!
 ************************************************************************
 * \brief
 *    Motion search for a partition
 ************************************************************************
 */
void
PartitionMotionSearch (int    blocktype,
                       int    block8x8,
                       int    *lambda_factor)
{
  static int  bx0[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,2,0,0}, {0,2,0,2}};
  static int  by0[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,2,0,0}, {0,0,0,0}, {0,0,2,2}};

  signed char **ref_array;
  short ***mv_array;
  short *all_mv;
  short ref;
  int   v, h, mcost, search_range, i, j;
  int   pic_block_x, pic_block_y;
  int   bslice    = (img->type==B_SLICE);
  int   parttype  = (blocktype<4?blocktype:4);
  int   step_h0   = (input->part_size[ parttype][0]);
  int   step_v0   = (input->part_size[ parttype][1]);
  int   step_h    = (input->part_size[blocktype][0]);
  int   step_v    = (input->part_size[blocktype][1]);
  int   list;
  int   numlists  = bslice ? 2 : 1;
  int   list_offset = img->mb_data[img->current_mb_nr].list_offset;
  int   *m_cost;
  int   by = by0[parttype][block8x8];
  int   bx = bx0[parttype][block8x8];

  //===== LOOP OVER REFERENCE FRAMES =====
  for (list=0; list<numlists;list++)
  {
    for (ref=0; ref < listXsize[list+list_offset]; ref++)
    {
       m_cost = &motion_cost[blocktype][list][ref][block8x8];
      //----- set search range ---
#ifdef _FULL_SEARCH_RANGE_
      if      (input->full_search == 2)
        search_range = input->search_range;
      else if (input->full_search == 1)
        search_range = input->search_range /  (imin(ref,1)+1);
      else
        search_range = input->search_range / ((imin(ref,1)+1) * imin(2,blocktype));
#else
      search_range = input->search_range / ((imin(ref,1)+1) * imin(2,blocktype));
#endif

      //----- set arrays -----
      ref_array = enc_picture->ref_idx[list];
      mv_array  = enc_picture->mv[list];

      //----- init motion cost -----
      //motion_cost[blocktype][list][ref][block8x8] = 0;
      *m_cost = 0;

      //===== LOOP OVER SUB MACRO BLOCK partitions
      for (v=by; v<by + step_v0; v += step_v)
      {
        pic_block_y = img->block_y + v;

        for (h=bx; h<bx+step_h0; h+=step_h)
        {
          all_mv = img->all_mv[v][h][list][ref][blocktype];
          pic_block_x = img->block_x + h;

          //--- motion search for block ---
          mcost = BlockMotionSearch     (ref, list, h<<2, v<<2, blocktype, search_range, lambda_factor);
          *m_cost += mcost;

          //--- set motion vectors and reference frame (for motion vector prediction) ---
          for (j=pic_block_y; j<pic_block_y + step_v; j++)
          {
            memset(&ref_array [j][pic_block_x], ref, step_h * sizeof(char));

            for (i=pic_block_x; i<pic_block_x + step_h; i++)
            {
              memcpy(mv_array  [j][i], all_mv, 2* sizeof(short));
            }
          }
        }
      }
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Calculate Direct Motion Vectors  *****
 ************************************************************************
 */
void Get_Direct_Motion_Vectors ()
{

  int   block_x, block_y, pic_block_x, pic_block_y, opic_block_x, opic_block_y;
  short ****all_mvs;
  int   mv_scale;
  int refList;
  int ref_idx;

  byte  **   moving_block;
  short ****   co_located_mv;
  signed char *** co_located_ref_idx;
  int64 ***    co_located_ref_id;
  signed char ** ref_pic_l0 = enc_picture->ref_idx[LIST_0];
  signed char ** ref_pic_l1 = enc_picture->ref_idx[LIST_1];
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];

  if (currMB->list_offset)
  {
    if(img->current_mb_nr%2)
    {
      moving_block = Co_located->bottom_moving_block;
      co_located_mv = Co_located->bottom_mv;
      co_located_ref_idx = Co_located->bottom_ref_idx;
      co_located_ref_id = Co_located->bottom_ref_pic_id;
    }
    else
    {
      moving_block = Co_located->top_moving_block;
      co_located_mv = Co_located->top_mv;
      co_located_ref_idx = Co_located->top_ref_idx;
      co_located_ref_id = Co_located->top_ref_pic_id;
    }
  }
  else
  {
    moving_block = Co_located->moving_block;
    co_located_mv = Co_located->mv;
    co_located_ref_idx = Co_located->ref_idx;
    co_located_ref_id = Co_located->ref_pic_id;
  }

  if (img->direct_spatial_mv_pred_flag)  //spatial direct mode copy from decoder
  {

    short l0_refA, l0_refB, l0_refD, l0_refC;
    short l1_refA, l1_refB, l1_refD, l1_refC;
    short l0_refX,l1_refX;
    short pmvfw[2]={0,0},pmvbw[2]={0,0};

    PixelPos mb_a, mb_b, mb_d, mb_c;

    getLuma4x4Neighbour(img->current_mb_nr, -1,  0,&mb_a);
    getLuma4x4Neighbour(img->current_mb_nr,  0, -1,&mb_b);
    getLuma4x4Neighbour(img->current_mb_nr, 16, -1,&mb_c);
    getLuma4x4Neighbour(img->current_mb_nr, -1, -1,&mb_d);

    if (!img->MbaffFrameFlag)
    {
      l0_refA = mb_a.available ? ref_pic_l0[mb_a.pos_y][mb_a.pos_x] : -1;
      l0_refB = mb_b.available ? ref_pic_l0[mb_b.pos_y][mb_b.pos_x] : -1;
      l0_refD = mb_d.available ? ref_pic_l0[mb_d.pos_y][mb_d.pos_x] : -1;
      l0_refC = mb_c.available ? ref_pic_l0[mb_c.pos_y][mb_c.pos_x] : l0_refD;

      l1_refA = mb_a.available ? ref_pic_l1[mb_a.pos_y][mb_a.pos_x] : -1;
      l1_refB = mb_b.available ? ref_pic_l1[mb_b.pos_y][mb_b.pos_x] : -1;
      l1_refD = mb_d.available ? ref_pic_l1[mb_d.pos_y][mb_d.pos_x] : -1;
      l1_refC = mb_c.available ? ref_pic_l1[mb_c.pos_y][mb_c.pos_x] : l1_refD;
    }
    else
    {
      if (currMB->mb_field)
      {
        l0_refA = mb_a.available
          ? (img->mb_data[mb_a.mb_addr].mb_field  || ref_pic_l0[mb_a.pos_y][mb_a.pos_x] < 0
          ?  ref_pic_l0[mb_a.pos_y][mb_a.pos_x]
          :  ref_pic_l0[mb_a.pos_y][mb_a.pos_x] * 2) : -1;

        l0_refB = mb_b.available
          ? (img->mb_data[mb_b.mb_addr].mb_field || ref_pic_l0[mb_b.pos_y][mb_b.pos_x] < 0
          ?  ref_pic_l0[mb_b.pos_y][mb_b.pos_x]
          :  ref_pic_l0[mb_b.pos_y][mb_b.pos_x] * 2) : -1;

        l0_refD = mb_d.available
          ? (img->mb_data[mb_d.mb_addr].mb_field || ref_pic_l0[mb_d.pos_y][mb_d.pos_x] < 0
          ?  ref_pic_l0[mb_d.pos_y][mb_d.pos_x]
          :  ref_pic_l0[mb_d.pos_y][mb_d.pos_x] * 2) : -1;

        l0_refC = mb_c.available
          ? (img->mb_data[mb_c.mb_addr].mb_field || ref_pic_l0[mb_c.pos_y][mb_c.pos_x] < 0
          ?  ref_pic_l0[mb_c.pos_y][mb_c.pos_x]
          :  ref_pic_l0[mb_c.pos_y][mb_c.pos_x] * 2) : l0_refD;

        l1_refA = mb_a.available
          ? (img->mb_data[mb_a.mb_addr].mb_field || ref_pic_l1[mb_a.pos_y][mb_a.pos_x] < 0
          ?  ref_pic_l1[mb_a.pos_y][mb_a.pos_x]
          :  ref_pic_l1[mb_a.pos_y][mb_a.pos_x] * 2) : -1;

        l1_refB = mb_b.available
          ? (img->mb_data[mb_b.mb_addr].mb_field || ref_pic_l1[mb_b.pos_y][mb_b.pos_x] < 0
          ?  ref_pic_l1[mb_b.pos_y][mb_b.pos_x]
          :  ref_pic_l1[mb_b.pos_y][mb_b.pos_x] * 2) : -1;

        l1_refD = mb_d.available
          ? (img->mb_data[mb_d.mb_addr].mb_field || ref_pic_l1[mb_d.pos_y][mb_d.pos_x] < 0
          ?  ref_pic_l1[mb_d.pos_y][mb_d.pos_x]
          :  ref_pic_l1[mb_d.pos_y][mb_d.pos_x] * 2) : -1;

        l1_refC = mb_c.available
          ? (img->mb_data[mb_c.mb_addr].mb_field || ref_pic_l1[mb_c.pos_y][mb_c.pos_x] < 0
          ?  ref_pic_l1[mb_c.pos_y][mb_c.pos_x]
          :  ref_pic_l1[mb_c.pos_y][mb_c.pos_x] * 2) : l1_refD;
      }
      else
      {
        l0_refA = mb_a.available
          ? (img->mb_data[mb_a.mb_addr].mb_field || ref_pic_l0[mb_a.pos_y][mb_a.pos_x]  < 0
          ?  ref_pic_l0[mb_a.pos_y][mb_a.pos_x] >> 1
          :  ref_pic_l0[mb_a.pos_y][mb_a.pos_x]) : -1;

        l0_refB = mb_b.available
          ? (img->mb_data[mb_b.mb_addr].mb_field || ref_pic_l0[mb_b.pos_y][mb_b.pos_x] < 0
          ?  ref_pic_l0[mb_b.pos_y][mb_b.pos_x] >> 1
          :  ref_pic_l0[mb_b.pos_y][mb_b.pos_x]) : -1;

        l0_refD = mb_d.available
          ? (img->mb_data[mb_d.mb_addr].mb_field || ref_pic_l0[mb_d.pos_y][mb_d.pos_x] < 0
          ?  ref_pic_l0[mb_d.pos_y][mb_d.pos_x] >> 1
          :  ref_pic_l0[mb_d.pos_y][mb_d.pos_x]) : -1;

        l0_refC = mb_c.available
          ? (img->mb_data[mb_c.mb_addr].mb_field || ref_pic_l0[mb_c.pos_y][mb_c.pos_x] < 0
          ?  ref_pic_l0[mb_c.pos_y][mb_c.pos_x] >> 1
          :  ref_pic_l0[mb_c.pos_y][mb_c.pos_x]) : l0_refD;

        l1_refA = mb_a.available
          ? (img->mb_data[mb_a.mb_addr].mb_field || ref_pic_l1[mb_a.pos_y][mb_a.pos_x] < 0
          ?  ref_pic_l1[mb_a.pos_y][mb_a.pos_x] >> 1
          :  ref_pic_l1[mb_a.pos_y][mb_a.pos_x]) : -1;

        l1_refB = mb_b.available
          ? (img->mb_data[mb_b.mb_addr].mb_field || ref_pic_l1[mb_b.pos_y][mb_b.pos_x] < 0
          ?  ref_pic_l1[mb_b.pos_y][mb_b.pos_x] >> 1
          :  ref_pic_l1[mb_b.pos_y][mb_b.pos_x]) : -1;

        l1_refD = mb_d.available
          ? (img->mb_data[mb_d.mb_addr].mb_field || ref_pic_l1[mb_d.pos_y][mb_d.pos_x] < 0
          ?  ref_pic_l1[mb_d.pos_y][mb_d.pos_x] >> 1
          :  ref_pic_l1[mb_d.pos_y][mb_d.pos_x]) : -1;

        l1_refC = mb_c.available
          ? (img->mb_data[mb_c.mb_addr].mb_field || ref_pic_l1[mb_c.pos_y][mb_c.pos_x] < 0
          ?  ref_pic_l1[mb_c.pos_y][mb_c.pos_x] >> 1
          :  ref_pic_l1[mb_c.pos_y][mb_c.pos_x]) : l1_refD;
      }
    }

    l0_refX = (l0_refA >= 0 && l0_refB >= 0) ? imin(l0_refA,l0_refB): imax(l0_refA,l0_refB);
    l0_refX = (l0_refX >= 0 && l0_refC >= 0) ? imin(l0_refX,l0_refC): imax(l0_refX,l0_refC);

    l1_refX = (l1_refA >= 0 && l1_refB >= 0) ? imin(l1_refA,l1_refB): imax(l1_refA,l1_refB);
    l1_refX = (l1_refX >= 0 && l1_refC >= 0) ? imin(l1_refX,l1_refC): imax(l1_refX,l1_refC);

    if (l0_refX >=0)
      SetMotionVectorPredictor (pmvfw, enc_picture->ref_idx[LIST_0], enc_picture->mv[LIST_0], l0_refX, LIST_0, 0, 0, 16, 16);

    if (l1_refX >=0)
      SetMotionVectorPredictor (pmvbw, enc_picture->ref_idx[LIST_1], enc_picture->mv[LIST_1], l1_refX, LIST_1, 0, 0, 16, 16);

    for (block_y=0; block_y<4; block_y++)
    {
      pic_block_y  = (img->pix_y  >> 2) + block_y;
      opic_block_y = (img->opix_y >> 2) + block_y;

      for (block_x=0; block_x<4; block_x++)
      {
        pic_block_x  = (img->pix_x  >> 2) + block_x;
        opic_block_x = (img->opix_x >> 2) + block_x;

        all_mvs = img->all_mv[block_y][block_x];

        if (l0_refX >=0)
        {
          if (!l0_refX  && !moving_block[opic_block_y][opic_block_x])
          {

            memset(all_mvs[LIST_0][0][0], 0, 2* sizeof(short));
            direct_ref_idx[LIST_0][pic_block_y][pic_block_x]=0;
          }
          else
          {
            all_mvs[LIST_0][l0_refX][0][0] = pmvfw[0];
            all_mvs[LIST_0][l0_refX][0][1] = pmvfw[1];
            direct_ref_idx[LIST_0][pic_block_y][pic_block_x]= (signed char)l0_refX;
          }
        }
        else
        {
          all_mvs[LIST_0][0][0][0] = 0;
          all_mvs[LIST_0][0][0][1] = 0;
          direct_ref_idx[LIST_0][pic_block_y][pic_block_x]=-1;
        }

        if (l1_refX >=0)
        {
          if(l1_refX==0 && !moving_block[opic_block_y][opic_block_x])
          {
            all_mvs[LIST_1][0][0][0] = 0;
            all_mvs[LIST_1][0][0][1] = 0;
            direct_ref_idx[LIST_1][pic_block_y][pic_block_x]= (signed char)l1_refX;
          }
          else
          {
            all_mvs[LIST_1][l1_refX][0][0] = pmvbw[0];
            all_mvs[LIST_1][l1_refX][0][1] = pmvbw[1];
            direct_ref_idx[LIST_1][pic_block_y][pic_block_x]= (signed char)l1_refX;
          }
        }
        else
        {
          direct_ref_idx[LIST_1][pic_block_y][pic_block_x]=-1;

          all_mvs[LIST_1][0][0][0] = 0;
          all_mvs[LIST_1][0][0][1] = 0;
        }

        // Test Level Limits if satisfied.
        if (img->MbaffFrameFlag
          && (all_mvs[LIST_0][l0_refX < 0? 0 : l0_refX][0][0] < -8192
          ||  all_mvs[LIST_0][l0_refX < 0? 0 : l0_refX][0][0] >  8191
          ||  all_mvs[LIST_0][l0_refX < 0? 0 : l0_refX][0][1] < LEVELMVLIMIT[img->LevelIndex][4]
          ||  all_mvs[LIST_0][l0_refX < 0? 0 : l0_refX][0][1] > LEVELMVLIMIT[img->LevelIndex][5]
          ||  all_mvs[LIST_1][l1_refX < 0? 0 : l1_refX][0][0] < -8192
          ||  all_mvs[LIST_1][l1_refX < 0? 0 : l1_refX][0][0] > 8191
          ||  all_mvs[LIST_1][l1_refX < 0? 0 : l1_refX][0][1] < LEVELMVLIMIT[img->LevelIndex][4]
          ||  all_mvs[LIST_1][l1_refX < 0? 0 : l1_refX][0][1] > LEVELMVLIMIT[img->LevelIndex][5]))
        {
          direct_ref_idx[LIST_0][pic_block_y][pic_block_x] = -1;
          direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = -1;
          direct_pdir           [pic_block_y][pic_block_x] = -1;
        }
        else
        {
          if (l0_refX < 0 && l1_refX < 0)
          {
            direct_ref_idx[LIST_0][pic_block_y][pic_block_x] =
              direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = 0;
            l0_refX = 0;
            l1_refX = 0;
          }
          if      (direct_ref_idx[LIST_1][pic_block_y][pic_block_x] == -1)
            direct_pdir[pic_block_y][pic_block_x] = 0;
          else if (direct_ref_idx[LIST_0][pic_block_y][pic_block_x] == -1)
            direct_pdir[pic_block_y][pic_block_x] = 1;
          else if (active_pps->weighted_bipred_idc == 1)
          {
            int weight_sum, i;
            Boolean invalid_wp = FALSE;
            for (i=0;i< (active_sps->chroma_format_idc == YUV400 ? 1 : 3); i++)
            {
              weight_sum = wbp_weight[0][l0_refX][l1_refX][i] + wbp_weight[1][l0_refX][l1_refX][i];
              if (weight_sum < -128 ||  weight_sum > 127)
              {
                invalid_wp = TRUE;
                break;
              }
            }
            if (invalid_wp == FALSE)
              direct_pdir[pic_block_y][pic_block_x] = 2;
            else
            {
              direct_ref_idx[LIST_0][pic_block_y][pic_block_x] = -1;
              direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = -1;
              direct_pdir           [pic_block_y][pic_block_x] = -1;
            }
          }
          else
            direct_pdir[pic_block_y][pic_block_x] = 2;
        }
      }
    }
  }
  else
  {
    int64 *refpic = enc_picture->ref_pic_num[LIST_0 +currMB->list_offset];

    //temporal direct mode copy from decoder
    for (block_y = 0; block_y < 4; block_y++)
    {
      pic_block_y  = (img->pix_y  >> 2) + block_y;
      opic_block_y = (img->opix_y >> 2) + block_y;

      for (block_x = 0; block_x < 4; block_x++)
      {
        pic_block_x  = (img->pix_x>>2) + block_x;
        opic_block_x = (img->opix_x>>2) + block_x;
        all_mvs = img->all_mv[block_y][block_x];

        refList = (co_located_ref_idx[LIST_0][opic_block_y][opic_block_x]== -1 ? LIST_1 : LIST_0);
        ref_idx = co_located_ref_idx[refList][opic_block_y][opic_block_x];

        // next P is intra mode
        if (ref_idx==-1)
        {
          all_mvs[LIST_0][0][0][0] = 0;
          all_mvs[LIST_0][0][0][1] = 0;
          all_mvs[LIST_1][0][0][0] = 0;
          all_mvs[LIST_1][0][0][1] = 0;
          direct_ref_idx[LIST_0][pic_block_y][pic_block_x] = 0;
          direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = 0;
          direct_pdir[pic_block_y][pic_block_x] = 2;
        }
        // next P is skip or inter mode
        else
        {
          int mapped_idx=INVALIDINDEX;
          int iref;

          for (iref=0;iref<imin(img->num_ref_idx_l0_active,listXsize[LIST_0+currMB->list_offset]);iref++)
          {
            if (refpic[iref]==co_located_ref_id[refList ][opic_block_y][opic_block_x])
            {
              mapped_idx=iref;
              break;
            }
            else //! invalid index. Default to zero even though this case should not happen
            {
              mapped_idx=INVALIDINDEX;
            }
          }

          if (mapped_idx !=INVALIDINDEX)
          {
            mv_scale = img->mvscale[LIST_0+currMB->list_offset][mapped_idx];

            if (mv_scale==9999)
            {
              // forward
              all_mvs[LIST_0][0][0][0] = co_located_mv[refList][opic_block_y][opic_block_x][0];
              all_mvs[LIST_0][0][0][1] = co_located_mv[refList][opic_block_y][opic_block_x][1];
              // backward
              all_mvs[LIST_1][0][0][0] = 0;
              all_mvs[LIST_1][0][0][1] = 0;
            }
            else
            {
              // forward
              all_mvs[LIST_0][mapped_idx][0][0] = (mv_scale * co_located_mv[refList][opic_block_y][opic_block_x][0] + 128) >> 8;
              all_mvs[LIST_0][mapped_idx][0][1] = (mv_scale * co_located_mv[refList][opic_block_y][opic_block_x][1] + 128) >> 8;
              // backward
              all_mvs[LIST_1][         0][0][0] = ((mv_scale - 256)* co_located_mv[refList][opic_block_y][opic_block_x][0] + 128) >> 8;
              all_mvs[LIST_1][         0][0][1] = ((mv_scale - 256)* co_located_mv[refList][opic_block_y][opic_block_x][1] + 128) >> 8;
            }

            // Test Level Limits if satisfied.
            if ( all_mvs[LIST_0][mapped_idx][0][0] < -8192
              || all_mvs[LIST_0][mapped_idx][0][0] >  8191
              || all_mvs[LIST_0][mapped_idx][0][1] < LEVELMVLIMIT[img->LevelIndex][4]
              || all_mvs[LIST_0][mapped_idx][0][1] > LEVELMVLIMIT[img->LevelIndex][5]
              || all_mvs[LIST_1][0][0][0] < -8192
              || all_mvs[LIST_1][0][0][0] > 8191
              || all_mvs[LIST_1][0][0][1] < LEVELMVLIMIT[img->LevelIndex][4]
              || all_mvs[LIST_1][0][0][1] > LEVELMVLIMIT[img->LevelIndex][5])
            {
              direct_ref_idx[LIST_0][pic_block_y][pic_block_x] = -1;
              direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = -1;
              direct_pdir[pic_block_y][pic_block_x] = -1;
            }
            else
            {
              direct_ref_idx[LIST_0][pic_block_y][pic_block_x] = mapped_idx;
              direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = 0;
              direct_pdir[pic_block_y][pic_block_x] = 2;
            }
          }
          else
          {
            direct_ref_idx[LIST_0][pic_block_y][pic_block_x] = -1;
            direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = -1;
            direct_pdir[pic_block_y][pic_block_x] = -1;
          }
        }
        if (active_pps->weighted_bipred_idc == 1 && direct_pdir[pic_block_y][pic_block_x] == 2)
        {
          int weight_sum, i;
          short l0_refX = direct_ref_idx[LIST_0][pic_block_y][pic_block_x];
          short l1_refX = direct_ref_idx[LIST_1][pic_block_y][pic_block_x];
          for (i=0;i< (active_sps->chroma_format_idc == YUV400 ? 1 : 3); i++)
          {
            weight_sum = wbp_weight[0][l0_refX][l1_refX][i] + wbp_weight[1][l0_refX][l1_refX][i];
            if (weight_sum < -128 ||  weight_sum > 127)
            {
              direct_ref_idx[LIST_0][pic_block_y][pic_block_x] = -1;
              direct_ref_idx[LIST_1][pic_block_y][pic_block_x] = -1;
              direct_pdir           [pic_block_y][pic_block_x] = -1;
              break;
            }
          }
        }
      }
    }
  }
}

