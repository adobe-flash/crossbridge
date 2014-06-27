
/*!
 ***************************************************************************
 * \file rdopt.c
 *
 * \brief
 *    Rate-Distortion optimized mode decision
 *
 * \author
 *    - Heiko Schwarz              <hschwarz@hhi.de>
 *    - Valeri George              <george@hhi.de>
 *    - Lowell Winger              <lwinger@lsil.com>
 *    - Alexis Michael Tourapis    <alexismt@ieee.org>
 * \date
 *    12. April 2001
 **************************************************************************
 */

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <string.h>

#include "global.h"

#include "rdopt_coding_state.h"
#include "memalloc.h"
#include "mb_access.h"
#include "elements.h"
#include "intrarefresh.h"
#include "image.h"
#include "transform8x8.h"
#include "cabac.h"
#include "vlc.h"
#include "me_umhex.h"
#include "ratectl.h"            // head file for rate control
#include "mode_decision.h"
#include "fmo.h"
#include "macroblock.h"
#include "symbol.h"


imgpel pred[16][16];

#define FASTMODE 1
//#define RESET_STATE

extern const int LEVELMVLIMIT[17][6];
extern int   QP2QUANT[40];

const int AdaptRndCrPos[2][5] =
{
  //  P,   B,   I,  SP,  SI
  {   4,   7,   1,   4,   1}, // Intra MB
  {  10,  13,  10,  10,  10}  // Inter MB
};

const int AdaptRndPos[4][5] =
{
  //  P,   B,   I,  SP,  SI
  {   3,   6,   0,   3,   0}, // 4x4 Intra MB
  {   1,   2,   0,   1,   2}, // 8x8 Intra MB
  {   9,  12,   9,   9,   9}, // 4x4 Inter MB
  {   3,   4,   3,   3,   3}, // 8x8 Inter MB
};

imgpel   rec_mbY[16][16], rec_mbU[16][16], rec_mbV[16][16];    // reconstruction values

int lrec_rec[16][16],lrec_rec_U[16][16],lrec_rec_V[16][16]; // store the transf. and quantized coefficients for SP frames

static int diff[16];
static int diff4x4[64];
static int diff8x8[64];
RD_8x8DATA tr4x4, tr8x8;

int   **bestInterFAdjust4x4=NULL, **bestIntraFAdjust4x4=NULL;
int   **bestInterFAdjust8x8=NULL, **bestIntraFAdjust8x8=NULL;
int   ***bestInterFAdjust4x4Cr=NULL, ***bestIntraFAdjust4x4Cr=NULL;
int   **fadjust8x8=NULL, **fadjust4x4=NULL, ***fadjust4x4Cr=NULL, ***fadjust8x8Cr=NULL;

int   ****cofAC=NULL, ****cofAC8x8=NULL;        // [8x8block][4x4block][level/run][scan_pos]
int   ***cofDC=NULL;                       // [yuv][level/run][scan_pos]
int   **cofAC4x4=NULL, ****cofAC4x4intern=NULL; // [level/run][scan_pos]
int   cbp, cbp8x8, cnt_nonz_8x8;
int64 cbp_blk;
int   cbp_blk8x8;
signed char frefframe[4][4], brefframe[4][4];
int   b8mode[4], b8pdir[4];
short best8x8mode [4];                // [block]
signed char best8x8pdir  [MAXMODE][4];       // [mode][block]
signed char best8x8fwref [MAXMODE][4];       // [mode][block]
signed char best8x8bwref [MAXMODE][4];       // [mode][block]


CSptr cs_mb=NULL, cs_b8=NULL, cs_cm=NULL, cs_imb=NULL, cs_ib8=NULL, cs_ib4=NULL, cs_pc=NULL;
int   best_c_imode;
int   best_i16offset;
short best_mode;
short  bi_pred_me;

//mixed transform sizes definitions
int   luma_transform_size_8x8_flag;

short all_mv8x8[2][2][4][4][2];       //[8x8_data/temp_data][LIST][block_x][block_y][MVx/MVy]
short pred_mv8x8[2][2][4][4][2];

int   ****cofAC_8x8ts = NULL;        // [8x8block][4x4block][level/run][scan_pos]

int64    cbp_blk8_8x8ts;
int      cbp8_8x8ts;
int      cost8_8x8ts;
int      cnt_nonz8_8x8ts;

// adaptive langrangian parameters
double mb16x16_cost;
double lambda_mf_factor;

void StoreMV8x8(int dir);
void RestoreMV8x8(int dir);
// end of mixed transform sizes definitions

//Adaptive Rounding update function
void update_offset_params(int mode, int luma_transform_size_8x8_flag);

signed char b4_ipredmode[16], b4_intra_pred_modes[16];

/*!
 ************************************************************************
 * \brief
 *    delete structure for RD-optimized mode decision
 ************************************************************************
 */
void clear_rdopt ()
{
  free_mem_DCcoeff (cofDC);
  free_mem_ACcoeff (cofAC);
  free_mem_ACcoeff (cofAC8x8);
  free_mem_ACcoeff (cofAC4x4intern);

  if (input->Transform8x8Mode)
  {
    free_mem_ACcoeff (cofAC_8x8ts);
  }

  if (input->AdaptiveRounding)
  {
    free_mem2Dint(bestInterFAdjust4x4);
    free_mem2Dint(bestIntraFAdjust4x4);
    free_mem2Dint(bestInterFAdjust8x8);
    free_mem2Dint(bestIntraFAdjust8x8);
    free_mem3Dint(bestInterFAdjust4x4Cr, 2);
    free_mem3Dint(bestIntraFAdjust4x4Cr, 2);
    free_mem2Dint(fadjust8x8);
    free_mem2Dint(fadjust4x4);
    free_mem3Dint(fadjust4x4Cr, 2);
    free_mem3Dint(fadjust8x8Cr, 2);
  }

  // structure for saving the coding state
  delete_coding_state (cs_mb);
  delete_coding_state (cs_b8);
  delete_coding_state (cs_cm);
  delete_coding_state (cs_imb);
  delete_coding_state (cs_ib8);
  delete_coding_state (cs_ib4);
  delete_coding_state (cs_pc);
}


/*!
 ************************************************************************
 * \brief
 *    create structure for RD-optimized mode decision
 ************************************************************************
 */
void init_rdopt ()
{
  rdopt = NULL;

  get_mem_DCcoeff (&cofDC);
  get_mem_ACcoeff (&cofAC);
  get_mem_ACcoeff (&cofAC8x8);
  get_mem_ACcoeff (&cofAC4x4intern);
  cofAC4x4 = cofAC4x4intern[0][0];

  if (input->Transform8x8Mode)
  {
    get_mem_ACcoeff (&cofAC_8x8ts);
  }

  switch (input->rdopt)
  {
  case 0:
    encode_one_macroblock = encode_one_macroblock_low;
    break;
  case 1:
    encode_one_macroblock = encode_one_macroblock_high;
    break;
  case 2:
    encode_one_macroblock = encode_one_macroblock_highfast;
    break;
  case 3:
    encode_one_macroblock = encode_one_macroblock_highloss;
    break;
  default:
    encode_one_macroblock = encode_one_macroblock_high;
    break;
  }
  if (input->AdaptiveRounding)
  {
    get_mem2Dint(&bestInterFAdjust4x4, 16, 16);
    get_mem2Dint(&bestIntraFAdjust4x4, 16, 16);
    get_mem2Dint(&bestInterFAdjust8x8, 16, 16);
    get_mem2Dint(&bestIntraFAdjust8x8, 16, 16);
    get_mem3Dint(&bestInterFAdjust4x4Cr, 2, img->mb_cr_size_y, img->mb_cr_size_x);
    get_mem3Dint(&bestIntraFAdjust4x4Cr, 2, img->mb_cr_size_y, img->mb_cr_size_x);
    get_mem2Dint(&fadjust8x8, 16, 16);
    get_mem2Dint(&fadjust4x4, 16, 16);
    get_mem3Dint(&fadjust4x4Cr, 2, img->mb_cr_size_y, img->mb_cr_size_x);
    get_mem3Dint(&fadjust8x8Cr, 2, img->mb_cr_size_y, img->mb_cr_size_x);
  }

  // structure for saving the coding state
  cs_mb  = create_coding_state ();
  cs_b8  = create_coding_state ();
  cs_cm  = create_coding_state ();
  cs_imb = create_coding_state ();
  cs_ib8 = create_coding_state ();
  cs_ib4 = create_coding_state ();
  cs_pc  = create_coding_state ();

  if (input->CtxAdptLagrangeMult == 1)
  {
    mb16x16_cost = CALM_MF_FACTOR_THRESHOLD;
    lambda_mf_factor = 1.0;
  }
}



/*!
 *************************************************************************************
 * \brief
 *    Updates the pixel map that shows, which reference frames are reliable for
 *    each MB-area of the picture.
 *
 * \note
 *    The new values of the pixel_map are taken from the temporary buffer refresh_map
 *
 *************************************************************************************
 */
void UpdatePixelMap()
{
  int mx,my,y,x,i,j;
  if (img->type==I_SLICE)
  {
    for (y=0; y<img->height; y++)
      for (x=0; x<img->width; x++)
      {
        pixel_map[y][x]=1;
      }
  }
  else
  {
    for (my=0; my<img->height >> 3; my++)
      for (mx=0; mx<img->width >> 3;  mx++)
      {
        j = my*8 + 8;
        i = mx*8 + 8;
        if (refresh_map[my][mx])
        {
          for (y=my*8; y<j; y++)
            for (x=mx*8; x<i; x++)
              pixel_map[y][x] = 1;
        }
        else
        {
          for (y=my*8; y<j; y++)
            for (x=mx*8; x<i; x++)
            {
              pixel_map[y][x] = imin(pixel_map[y][x] + 1, input->num_ref_frames+1);
            }
        }
     }
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Checks if a given reference frame is reliable for the current
 *    macroblock, given the motion vectors that the motion search has
 *    returned.
 *
 * \return
 *    If the return value is 1, the reference frame is reliable. If it
 *    is 0, then it is not reliable.
 *
 * \note
 *    A specific area in each reference frame is assumed to be unreliable
 *    if the same area has been intra-refreshed in a subsequent frame.
 *    The information about intra-refreshed areas is kept in the pixel_map.
 *
 *************************************************************************************
 */
int CheckReliabilityOfRef (int block, int list_idx, int ref, int mode)
{
  int y,x, block_y, block_x, dy, dx, y_pos, x_pos, yy, xx, pres_x, pres_y;
  int maxold_x  = img->width-1;
  int maxold_y  = img->height-1;
  int ref_frame = ref+1;

  int by0 = (mode>=4?2*(block >> 1):mode==2?2*block:0);
  int by1 = by0 + (mode>=4||mode==2?2:4);
  int bx0 = (mode>=4?2*(block & 0x01):mode==3?2*block:0);
  int bx1 = bx0 + (mode>=4||mode==3?2:4);

  for (block_y=by0; block_y<by1; block_y++)
  {
    for (block_x=bx0; block_x<bx1; block_x++)
    {
      y_pos  = img->all_mv[block_y][block_x][list_idx][ref][mode][1];
      y_pos += (img->block_y + block_y) * BLOCK_SIZE * 4;
      x_pos  = img->all_mv[block_y][block_x][list_idx][ref][mode][0];
      x_pos += (img->block_x + block_x) * BLOCK_SIZE * 4;

      /* Here we specify which pixels of the reference frame influence
      the reference values and check their reliability. This is
      based on the function Get_Reference_Pixel */

      dy = y_pos & 3;
      dx = x_pos & 3;

      y_pos = (y_pos-dy) >> 2;
      x_pos = (x_pos-dx) >> 2;

      if (dy==0 && dx==0) //full-pel
      {
        for (y=y_pos ; y < y_pos + BLOCK_SIZE ; y++)
          for (x=x_pos ; x < x_pos + BLOCK_SIZE ; x++)
            if (pixel_map[iClip3(0,maxold_y,y)][iClip3(0,maxold_x,x)] < ref_frame)
              return 0;
      }
      else  /* other positions */
      {
        if (dy == 0)
        {
          for (y = y_pos ; y < y_pos + BLOCK_SIZE ; y++)
          {
            pres_y = iClip3(0,maxold_y,y);
            for (x = x_pos ; x < x_pos + BLOCK_SIZE ; x++)
            {
              for(xx = -2 ; xx < 4 ; xx++) {
                pres_x = iClip3(0, maxold_x, x + xx);
                if (pixel_map[pres_y][pres_x] < ref_frame)
                  return 0;
              }
            }
          }
        }
        else if (dx == 0)
        {
          for (y = y_pos ; y < y_pos + BLOCK_SIZE ; y++)
            for (x=x_pos ; x < x_pos + BLOCK_SIZE ; x++)
            {
              pres_x = iClip3(0,maxold_x,x);
              for(yy=-2;yy<4;yy++) {
                pres_y = iClip3(0,maxold_y, yy + y);
                if (pixel_map[pres_y][pres_x] < ref_frame)
                  return 0;
              }
            }
        }
        else if (dx == 2)
        {
          for (y = y_pos ; y < y_pos + BLOCK_SIZE ; y++)
            for (x = x_pos ; x < x_pos + BLOCK_SIZE ; x++)
            {
              for(yy=-2;yy<4;yy++) {
                pres_y = iClip3(0,maxold_y, yy + y);
                for(xx=-2;xx<4;xx++) {
                  pres_x = iClip3(0,maxold_x, xx + x);
                  if (pixel_map[pres_y][pres_x] < ref_frame)
                    return 0;
                }
              }
            }
        }
        else if (dy == 2)
        {
          for (y = y_pos ; y < y_pos + BLOCK_SIZE ; y++)
            for (x = x_pos ; x < x_pos + BLOCK_SIZE ; x++)
            {
              for(xx=-2;xx<4;xx++) {
                pres_x = iClip3(0,maxold_x, xx + x);
                for(yy=-2;yy<4;yy++) {
                  pres_y = iClip3(0,maxold_y, yy + y);
                  if (pixel_map[pres_y][pres_x] < ref_frame)
                    return 0;
                }
              }
            }
        }
        else
        {
          for (y = y_pos ; y < y_pos + BLOCK_SIZE ; y++)
          {
            for (x = x_pos ; x < x_pos + BLOCK_SIZE ; x++)
            {
              pres_y = dy == 1 ? y : y + 1;
              pres_y = iClip3(0,maxold_y,pres_y);

              for(xx=-2;xx<4;xx++)
              {
                pres_x = iClip3(0,maxold_x,xx + x);
                if (pixel_map[pres_y][pres_x] < ref_frame)
                  return 0;
              }

              pres_x = dx == 1 ? x : x + 1;
              pres_x = iClip3(0,maxold_x,pres_x);

              for(yy=-2;yy<4;yy++)
              {
                pres_y = iClip3(0,maxold_y, yy + y);
                if (pixel_map[pres_y][pres_x] < ref_frame)
                  return 0;
              }
            }
          }
        }
      }
    }
  }
  return 1;
}



/*!
 *************************************************************************************
 * \brief
 *    R-D Cost for an 4x4 Intra block
 *************************************************************************************
 */
double RDCost_for_4x4IntraBlocks (int*    nonzero,
                                  int     b8,
                                  int     b4,
                                  int     ipmode,
                                  double  lambda,
                                  double  min_rdcost,
                                  int mostProbableMode)
{
  double  rdcost;
  int     dummy, x, y, rate;
  int64   distortion  = 0;
  int     block_x     = 8*(b8 & 0x01)+4*(b4 & 0x01);
  int     block_y     = 8*(b8 >> 1)+4*(b4 >> 1);
  int     pic_pix_x   = img->pix_x+block_x;
  int     pic_pix_y   = img->pix_y+block_y;
  int     pic_opix_y  = img->opix_y+block_y;
  imgpel  **imgY      = enc_picture->imgY;

  Slice          *currSlice    =  img->currentSlice;
  SyntaxElement  se;
  const int      *partMap      = assignSE2partition[input->partition_mode];
  DataPartition  *dataPart;

  //===== perform DCT, Q, IQ, IDCT, Reconstruction =====
  dummy = 0;

  if(img->type!=SP_SLICE)
    *nonzero = dct_luma (block_x, block_y, &dummy, 1);
  else if(!si_frame_indicator && !sp2_frame_indicator)
  {
    *nonzero = dct_luma_sp(block_x, block_y, &dummy);
  }
  else
  {
    *nonzero = dct_luma_sp2(block_x, block_y, &dummy);
  }

  //===== get distortion (SSD) of 4x4 block =====
  for (y=0; y<4; y++)
  {
    for (x=pic_pix_x; x<pic_pix_x+4; x++)
    {
      distortion += img->quad [imgY_org[pic_opix_y+y][x] - imgY[pic_pix_y+y][x]];
    }
  }

  //===== RATE for INTRA PREDICTION MODE  (SYMBOL MODE MUST BE SET TO UVLC) =====
  se.value1 = (mostProbableMode == ipmode) ? -1 : ipmode < mostProbableMode ? ipmode : ipmode-1;

  //--- set position and type ---
  se.context = 4*b8 + b4;
  se.type    = SE_INTRAPREDMODE;

  //--- choose data partition ---
  dataPart = &(currSlice->partArr[partMap[SE_INTRAPREDMODE]]);
  //--- encode and update rate ---
  writeIntraPredMode (&se, dataPart);
  rate = se.len;

  //===== RATE for LUMINANCE COEFFICIENTS =====
  if (input->symbol_mode == UVLC)
  {
    rate  += writeCoeff4x4_CAVLC (LUMA, b8, b4, 0);
  }
  else
  {
    rate  += writeLumaCoeff4x4_CABAC (b8, b4, 1);
  }
  //reset_coding_state (cs_cm);
  rdcost = (double)distortion + lambda*(double)rate;

  return rdcost;
}


/*!
 *************************************************************************************
 * \brief
 *    Mode Decision for an 4x4 Intra block
 *************************************************************************************
 */
int Mode_Decision_for_4x4IntraBlocks (int  b8,  int  b4,  double  lambda,  int*  min_cost)
{
  int     ipmode, best_ipmode = 0, i, j, k, y, cost, dummy;
  int     c_nz, nonzero = 0;
  imgpel  rec4x4[4][4];
  double  rdcost;
  int     block_x     = 8 * (b8 & 0x01) + 4 * (b4 & 0x01);
  int     block_y     = 8 * (b8 >> 1)   + 4 * (b4 >> 1);
  int     pic_pix_x   = img->pix_x  + block_x;
  int     pic_pix_y   = img->pix_y  + block_y;
  int     pic_opix_x  = img->opix_x + block_x;
  int     pic_opix_y  = img->opix_y + block_y;
  int     pic_block_x = pic_pix_x >> 2;
  int     pic_block_y = pic_pix_y >> 2;
  double  min_rdcost  = 1e30;

  int left_available, up_available, all_available;

  signed char upMode;
  signed char leftMode;
  int     mostProbableMode;

  PixelPos left_block;
  PixelPos top_block;

  int  lrec4x4[4][4];
  int  fixedcost = (int) floor(4 * lambda );

#ifdef BEST_NZ_COEFF
  int best_nz_coeff = 0;
  int best_coded_block_flag = 0;
  int bit_pos = 1 + ((((b8>>1)<<1)+(b4>>1))<<2) + (((b8&1)<<1)+(b4&1));
  static int64 cbp_bits;

  if (b8==0 && b4==0)
   cbp_bits = 0;
#endif

  getLuma4x4Neighbour(img->current_mb_nr, block_x - 1, block_y,   &left_block);
  getLuma4x4Neighbour(img->current_mb_nr, block_x,     block_y-1, &top_block);

  // constrained intra pred
  if (input->UseConstrainedIntraPred)
  {
    left_block.available = left_block.available ? img->intra_block[left_block.mb_addr] : 0;
    top_block.available  = top_block.available  ? img->intra_block[top_block.mb_addr]  : 0;
  }

  upMode            =  top_block.available ? img->ipredmode[top_block.pos_y ][top_block.pos_x ] : -1;
  leftMode          = left_block.available ? img->ipredmode[left_block.pos_y][left_block.pos_x] : -1;

  mostProbableMode  = (upMode < 0 || leftMode < 0) ? DC_PRED : upMode < leftMode ? upMode : leftMode;

  *min_cost = INT_MAX;

  //===== INTRA PREDICTION FOR 4x4 BLOCK =====
  intrapred_luma (pic_pix_x, pic_pix_y, &left_available, &up_available, &all_available);

  //===== LOOP OVER ALL 4x4 INTRA PREDICTION MODES =====
  for (ipmode=0; ipmode<NO_INTRA_PMODE; ipmode++)
  {
    int available_mode =  (ipmode==DC_PRED) ||
      ((ipmode==VERT_PRED||ipmode==VERT_LEFT_PRED||ipmode==DIAG_DOWN_LEFT_PRED) && up_available ) ||
      ((ipmode==HOR_PRED||ipmode==HOR_UP_PRED) && left_available ) ||(all_available);

    if (input->IntraDisableInterOnly==0 || img->type != I_SLICE)
    {
      if (input->Intra4x4ParDisable && (ipmode==VERT_PRED||ipmode==HOR_PRED))
        continue;

      if (input->Intra4x4DiagDisable && (ipmode==DIAG_DOWN_LEFT_PRED||ipmode==DIAG_DOWN_RIGHT_PRED))
        continue;

      if (input->Intra4x4DirDisable && ipmode>=VERT_RIGHT_PRED)
        continue;
    }

    if( available_mode)
    {
      if (!input->rdopt)
      {
        for (k=j=0; j<4; j++)
        {
          int jj = pic_opix_y+j;
          for (i=0; i<4; i++, k++)
          {
            diff[k] = imgY_org[jj][pic_opix_x+i] - img->mprr[ipmode][j][i];
          }
        }
        cost  = (ipmode == mostProbableMode) ? 0 : fixedcost;
        cost += distortion4x4 (diff);
        if (cost < *min_cost)
        {
          best_ipmode = ipmode;
          *min_cost   = cost;
        }
      }
      else
      {
        // get prediction and prediction error
        for (j=0; j<4; j++)
        {
          memcpy(&img->mpr[block_y+j][block_x], img->mprr[ipmode][j], BLOCK_SIZE * sizeof(imgpel));
          for (i=0; i<4; i++)
          {
            img->m7[j][i] = (int) (imgY_org[pic_opix_y+j][pic_opix_x+i] - img->mprr[ipmode][j][i]);
          }
        }

        //===== store the coding state =====
        //store_coding_state (cs_cm);
        // get and check rate-distortion cost
#ifdef BEST_NZ_COEFF
        img->mb_data[img->current_mb_nr].cbp_bits = cbp_bits;
#endif
        if ((rdcost = RDCost_for_4x4IntraBlocks (&c_nz, b8, b4, ipmode, lambda, min_rdcost, mostProbableMode)) < min_rdcost)
        {
          //--- set coefficients ---
          memcpy(cofAC4x4[0],img->cofAC[b8][b4][0], 18 * sizeof(int));
          memcpy(cofAC4x4[1],img->cofAC[b8][b4][1], 18 * sizeof(int));

          //--- set reconstruction ---
          for (y=0; y<4; y++)
          {
            memcpy(rec4x4[y],&enc_picture->imgY[pic_pix_y+y][pic_pix_x], BLOCK_SIZE * sizeof(imgpel));
            if(img->type==SP_SLICE &&(!si_frame_indicator && !sp2_frame_indicator))
              memcpy(lrec4x4[y],&lrec[pic_pix_y+y][pic_pix_x], BLOCK_SIZE * sizeof(int));// stores the mode coefficients
          }
          //--- flag if dct-coefficients must be coded ---
          nonzero = c_nz;

          //--- set best mode update minimum cost ---
          min_rdcost    = rdcost;
          best_ipmode   = ipmode;
#ifdef BEST_NZ_COEFF
          best_nz_coeff = img->nz_coeff [img->current_mb_nr][block_x4][block_y4];
          best_coded_block_flag = (int)((img->mb_data[img->current_mb_nr].cbp_bits>>bit_pos)&(int64)(1));
#endif
          //store_coding_state (cs_ib4);
          if (img->AdaptiveRounding)
          {
            for (j=0; j<4; j++)
              memcpy(&fadjust4x4[block_y+j][block_x],&img->fadjust4x4[1][block_y+j][block_x], BLOCK_SIZE * sizeof(int));
          }
        }

#ifndef RESET_STATE
        reset_coding_state (cs_cm);
#endif
      }
    }
  }

#ifdef BEST_NZ_COEFF
  img->nz_coeff [img->current_mb_nr][block_x4][block_y4] = best_nz_coeff;
  cbp_bits &= (~(int64)(1<<bit_pos));
  cbp_bits |= (int64)(best_coded_block_flag<<bit_pos);
#endif
  //===== set intra mode prediction =====
  img->ipredmode[pic_block_y][pic_block_x] = (signed char) best_ipmode;
  img->mb_data[img->current_mb_nr].intra_pred_modes[4*b8+b4] =
    (signed char) (mostProbableMode == best_ipmode ? -1 : (best_ipmode < mostProbableMode ? best_ipmode : best_ipmode-1));

  if (!input->rdopt)
  {
    // get prediction and prediction error
    for (j=0; j<4; j++)
    {
      int jj = pic_opix_y+j;
      for (i=0; i<4; i++)
      {
        img->mpr[block_y+j][block_x+i]  = img->mprr[best_ipmode][j][i];
        img->m7[j][i]                   = imgY_org[jj][pic_opix_x+i] - img->mprr[best_ipmode][j][i];
      }
    }
    nonzero = dct_luma (block_x, block_y, &dummy, 1);
  }
  else
  {
    //===== restore coefficients =====
    for (j=0; j<2; j++)
    {
      memcpy (img->cofAC[b8][b4][j],cofAC4x4[j], 18 * sizeof(int));
    }

    //===== restore reconstruction and prediction (needed if single coeffs are removed) =====
    for (y=0; y<BLOCK_SIZE; y++)
    {
      memcpy (&enc_picture->imgY[pic_pix_y+y][pic_pix_x],rec4x4[y],    BLOCK_SIZE * sizeof(imgpel));
      memcpy (&img->mpr[block_y+y][block_x],img->mprr[best_ipmode][y], BLOCK_SIZE * sizeof(imgpel));
      if(img->type==SP_SLICE &&(!si_frame_indicator && !sp2_frame_indicator))
        memcpy (&lrec[pic_pix_y+y][pic_pix_x],lrec4x4[y], BLOCK_SIZE * sizeof(int));//restore coefficients when encoding primary SP frame
    }

    if (img->AdaptiveRounding)
    {
      for (j=0; j<BLOCK_SIZE; j++)
        memcpy (&img->fadjust4x4[1][block_y+j][block_x],&fadjust4x4[block_y+j][block_x], BLOCK_SIZE * sizeof(int));
    }

  }
  return nonzero;
}


/*!
 *************************************************************************************
 * \brief
 *    Mode Decision for an 8x8 Intra block
 *************************************************************************************
 */
int Mode_Decision_for_8x8IntraBlocks(int b8,double lambda,int *cost)
{
  int  nonzero=0, b4;
  int  cost4x4;

  *cost = (int)floor(6.0 * lambda + 0.4999);

  for (b4=0; b4<4; b4++)
  {
    if (Mode_Decision_for_4x4IntraBlocks (b8, b4, lambda, &cost4x4))
    {
      nonzero        = 1;
    }
    *cost += cost4x4;
  }
#ifdef RESET_STATE
  //reset_coding_state (cs_cm);
#endif

  return nonzero;
}

/*!
 *************************************************************************************
 * \brief
 *    4x4 Intra mode decision for an macroblock
 *************************************************************************************
 */
int Mode_Decision_for_Intra4x4Macroblock (double lambda,  int* cost)
{
  int  cbp=0, b8, cost8x8;

  for (*cost=0, b8=0; b8<4; b8++)
  {
    if (Mode_Decision_for_8x8IntraBlocks (b8, lambda, &cost8x8))
    {
      cbp |= (1<<b8);
    }
    *cost += cost8x8;
  }

  return cbp;
}


/*!
 *************************************************************************************
 * \brief
 *    R-D Cost for an 8x8 Partition
 *************************************************************************************
 */
double RDCost_for_8x8blocks (int*    cnt_nonz,   // --> number of nonzero coefficients
                             int64*  cbp_blk,    // --> cbp blk
                             double  lambda,     // <-- lagrange multiplier
                             int     block,      // <-- 8x8 block number
                             int     mode,       // <-- partitioning mode
                             short   pdir,       // <-- prediction direction
                             short   l0_ref,     // <-- L0 reference picture
                             short   l1_ref)     // <-- L1 reference picture
{
  int  i, j, k;
  int  rate=0;
  int64 distortion=0;
  int  dummy = 0, mrate;
  int  fw_mode, bw_mode;
  int  cbp     = 0;
  int  pax     = 8*(block & 0x01);
  int  pay     = 8*(block >> 1);
  int  i0      = pax >> 2;
  int  j0      = pay >> 2;
  int  bframe  = (img->type==B_SLICE);
  int  direct  = (bframe && mode==0);
  int  b8value = B8Mode2Value (mode, pdir);

  Macroblock    *currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement se;
  Slice         *currSlice = img->currentSlice;
  DataPartition *dataPart;
  const int     *partMap   = assignSE2partition[input->partition_mode];

  EncodingEnvironmentPtr eep_dp;

  //=====
  //=====  GET COEFFICIENTS, RECONSTRUCTIONS, CBP
  //=====
  currMB->bi_pred_me=0;

  if (direct)
  {
    if (direct_pdir[img->block_y+j0][img->block_x+i0]<0) // mode not allowed
      return (1e20);
    else
      *cnt_nonz = LumaResidualCoding8x8 (&cbp, cbp_blk, block, direct_pdir[img->block_y+j0][img->block_x+i0], 0, 0,
      (short)imax(0,direct_ref_idx[LIST_0][img->block_y+j0][img->block_x+i0]),
      direct_ref_idx[LIST_1][img->block_y+j0][img->block_x+i0]);
  }
  else
  {
    if (pdir == 2 && active_pps->weighted_bipred_idc == 1)
    {
      int weight_sum = (active_pps->weighted_bipred_idc == 1)? wbp_weight[0][l0_ref][l1_ref][0] + wbp_weight[1][l0_ref][l1_ref][0] : 0;
      if (weight_sum < -128 ||  weight_sum > 127)
      {
        return (1e20);
      }
    }

    fw_mode   = (pdir==0||pdir==2 ? mode : 0);
    bw_mode   = (pdir==1||pdir==2 ? mode : 0);
    *cnt_nonz = LumaResidualCoding8x8 (&cbp, cbp_blk, block, pdir, fw_mode, bw_mode, l0_ref, l1_ref);
  }

  //===== get residue =====
  if (input->rdopt==3 && img->type!=B_SLICE)
  {
    // We need the reconstructed prediction residue for the simulated decoders.
    compute_residue_b8block (block, -1);
  }

  //=====
  //=====   GET DISTORTION
  //=====
  if (input->rdopt==3 && img->type!=B_SLICE)
  {
    for (k=0; k<input->NoOfDecoders ;k++)
    {
      decode_one_b8block (k, P8x8, block, mode, l0_ref);
      for (j=img->opix_y+pay; j<img->opix_y+pay+8; j++)
        for (i=img->opix_x+pax; i<img->opix_x+pax+8; i++)
        {
          distortion += img->quad[imgY_org[j][i] - decs->decY[k][j][i]];
        }
    }
    distortion /= input->NoOfDecoders;
  }
  else
  {
    for (j=pay; j<pay+8; j++)
      for (i=img->pix_x+pax; i<img->pix_x+pax+8; i++)
      {
        distortion += img->quad [imgY_org[img->opix_y+j][i] - enc_picture->imgY[img->pix_y+j][i]];
      }
  }

  //=====
  //=====   GET RATE
  //=====
  //----- block 8x8 mode -----
  if (input->symbol_mode == UVLC)
  {
    ue_linfo (b8value, dummy, &mrate, &dummy);
    rate += mrate;
  }
  else
  {
    se.value1  = b8value;
    se.type    = SE_MBTYPE;
    dataPart = &(currSlice->partArr[partMap[se.type]]);
    writeB8_typeInfo(&se, dataPart);
    rate += se.len;
  }

  //----- motion information -----
  if (!direct)
  {
    if ((img->num_ref_idx_l0_active > 1 ) && (pdir==0 || pdir==2))
      rate  += writeReferenceFrame (mode, i0, j0, 1, l0_ref);
    if(img->num_ref_idx_l1_active > 1 && img->type== B_SLICE)
    {
      if (pdir==1 || pdir==2)
      {
        rate  += writeReferenceFrame (mode, i0, j0, 0, l1_ref);
      }
    }

    if (pdir==0 || pdir==2)
    {
      rate  += writeMotionVector8x8 (i0, j0, i0+2, j0+2, l0_ref,LIST_0, mode);
    }
    if (pdir==1 || pdir==2)
    {
      rate  += writeMotionVector8x8 (i0, j0, i0+2, j0+2, l1_ref, LIST_1, mode);
    }
  }

  //----- coded block pattern (for CABAC only) -----
  if (input->symbol_mode == CABAC)
  {
    dataPart = &(currSlice->partArr[partMap[SE_CBP]]);
    eep_dp   = &(dataPart->ee_cabac);
    mrate    = arienco_bits_written (eep_dp);
    writeCBP_BIT_CABAC (block, ((*cnt_nonz>0)?1:0), cbp8x8, currMB, 1, eep_dp);
    mrate    = arienco_bits_written (eep_dp) - mrate;
    rate    += mrate;
  }

  //----- luminance coefficients -----
  if (*cnt_nonz)
  {
    rate += writeLumaCoeff8x8 (block, mode, currMB->luma_transform_size_8x8_flag);
  }

  return (double)distortion + lambda * (double)rate;
}


/*!
 *************************************************************************************
 * \brief
 *    Gets mode offset for intra16x16 mode
 *************************************************************************************
 */
int I16Offset (int cbp, int i16mode)
{
  return (cbp&15?13:1) + i16mode + ((cbp&0x30)>>2);
}


/*!
 *************************************************************************************
 * \brief
 *    Sets modes and reference frames for a macroblock
 *************************************************************************************
 */
void SetModesAndRefframeForBlocks (int mode)
{
  int i,j,k,l;
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];
  int  bframe  = (img->type==B_SLICE);
  int  block_x, block_y;
  int  cur_ref[2];

  //--- macroblock type ---
  currMB->mb_type = mode;
  currMB->bi_pred_me= (mode == 1 ? img->bi_pred_me[mode] : 0);

  //--- block 8x8 mode and prediction direction ---
  switch (mode)
  {
  case 0:
    for(i=0;i<4;i++)
    {
      currMB->b8mode[i] = 0;
      currMB->b8pdir[i] = (bframe ? direct_pdir[img->block_y + (i >> 1)*2][img->block_x + (i & 0x01)*2] : 0);
    }
    break;
  case 1:
  case 2:
  case 3:
    for(i=0;i<4;i++)
    {
      currMB->b8mode[i] = mode;
      currMB->b8pdir[i] = best8x8pdir[mode][i];
    }
    break;
  case P8x8:
    for(i=0;i<4;i++)
    {
      currMB->b8mode[i]   = best8x8mode[i];
      currMB->b8pdir[i]   = best8x8pdir[mode][i];
    }
    break;
  case I4MB:
    for(i=0;i<4;i++)
    {
      currMB->b8mode[i] = IBLOCK;
      currMB->b8pdir[i] = -1;
    }
    break;
  case I16MB:
    for(i=0;i<4;i++)
    {
      currMB->b8mode[i] =  0;
      currMB->b8pdir[i] = -1;
    }
    break;
  case I8MB:
    for(i=0;i<4;i++)
    {
      currMB->b8mode[i] = I8MB;
      currMB->b8pdir[i] = -1;
    }
    //switch to 8x8 transform
    currMB->luma_transform_size_8x8_flag = 1;
    break;
  case IPCM:
    for(i=0;i<4;i++)
    {
      currMB->b8mode[i] = IPCM;
      currMB->b8pdir[i] = -1;
    }
    currMB->luma_transform_size_8x8_flag = 0;
    break;
  default:
    printf ("Unsupported mode in SetModesAndRefframeForBlocks!\n");
    exit (1);
  }

#define IS_FW ((best8x8pdir[mode][k]==0 || best8x8pdir[mode][k]==2) && (mode!=P8x8 || best8x8mode[k]!=0 || !bframe))
#define IS_BW ((best8x8pdir[mode][k]==1 || best8x8pdir[mode][k]==2) && (mode!=P8x8 || best8x8mode[k]!=0))
  //--- reference frame arrays ---
  if (mode==0 || mode==I4MB || mode==I16MB || mode==I8MB)
  {
    if (bframe)
    {
      if (!mode)
      {
        for (j = img->block_y; j < img->block_y + 4; j++)
        {
          memcpy(&enc_picture->ref_idx[LIST_0][j][img->block_x],&direct_ref_idx[LIST_0][j][img->block_x], 4 * sizeof(char));
          memcpy(&enc_picture->ref_idx[LIST_1][j][img->block_x],&direct_ref_idx[LIST_1][j][img->block_x], 4 * sizeof(char));
        }
      }
      else
      {
        for (j = img->block_y; j < img->block_y + 4; j++)
        {
          memset(&enc_picture->ref_idx[LIST_0][j][img->block_x],-1, 4 * sizeof(char));
          memset(&enc_picture->ref_idx[LIST_1][j][img->block_x],-1, 4 * sizeof(char));
        }
      }
    }
    else
    {
      if (!mode)
      {
        for (j = img->block_y; j < img->block_y + 4; j++)
          memset(&enc_picture->ref_idx[LIST_0][j][img->block_x],0, 4 * sizeof(char));
      }
      else
      {
        for (j = img->block_y; j < img->block_y + 4; j++)
          memset(&enc_picture->ref_idx[LIST_0][j][img->block_x],-1, 4 * sizeof(char));
      }
    }
  }
  else
  {
    if (bframe)
    {
      for (j=0;j<4;j++)
      {
        block_y = img->block_y + j;
        for (i=0;i<4;i++)
        {
          block_x = img->block_x + i;
          k = 2*(j >> 1) + (i >> 1);
          l = 2*(j & 0x01) + (i & 0x01);

          if(mode == P8x8 && best8x8mode[k]==0)
          {
            enc_picture->ref_idx[LIST_0][block_y][block_x] = direct_ref_idx[LIST_0][block_y][block_x];
            enc_picture->ref_idx[LIST_1][block_y][block_x] = direct_ref_idx[LIST_1][block_y][block_x];
          }
          else if (mode ==1 && currMB->bi_pred_me && IS_FW && IS_BW)
          {
            enc_picture->ref_idx[LIST_0][block_y][block_x] = 0;
            enc_picture->ref_idx[LIST_1][block_y][block_x] = 0;
          }
          else
          {
            enc_picture->ref_idx[LIST_0][block_y][block_x] = (IS_FW ? best8x8fwref[mode][k] : -1);
            enc_picture->ref_idx[LIST_1][block_y][block_x] = (IS_BW ? best8x8bwref[mode][k] : -1);
          }
        }
      }
    }
    else
    {
      for (j=0;j<4;j++)
      {
        block_y = img->block_y + j;
        for (i=0;i<4;i++)
        {
          block_x = img->block_x + i;
          k = 2*(j >> 1) + (i >> 1);
          l = 2*(j & 0x01) + (i & 0x01);
          enc_picture->ref_idx[LIST_0][block_y][block_x] = (IS_FW ? best8x8fwref[mode][k] : -1);
        }
      }
    }
  }

  if (bframe)
  {

    for (j = img->block_y; j < img->block_y + 4; j++)
      for (i = img->block_x; i < img->block_x + 4;i++)
      {
        cur_ref[LIST_0] = (int) enc_picture->ref_idx[LIST_0][j][i];
        cur_ref[LIST_1] = (int) enc_picture->ref_idx[LIST_1][j][i];

        enc_picture->ref_pic_id [LIST_0][j][i] = (cur_ref[LIST_0]>=0
          ? enc_picture->ref_pic_num[LIST_0 + currMB->list_offset][cur_ref[LIST_0]]
          : -1);
        enc_picture->ref_pic_id [LIST_1][j][i] = (cur_ref[LIST_1]>=0
          ? enc_picture->ref_pic_num[LIST_1 + currMB->list_offset][cur_ref[LIST_1]]
          : -1);
      }
  }
  else
  {
    for (j = img->block_y; j < img->block_y + 4; j++)
      for (i = img->block_x; i < img->block_x + 4;i++)
      {
        cur_ref[LIST_0] = (int) enc_picture->ref_idx[LIST_0][j][i];
        enc_picture->ref_pic_id [LIST_0][j][i] = (cur_ref[LIST_0]>=0
          ? enc_picture->ref_pic_num[LIST_0 + currMB->list_offset][cur_ref[LIST_0]]
          : -1);
      }
  }

#undef IS_FW
#undef IS_BW
}


/*!
 *************************************************************************************
 * \brief
 *    Intra 16x16 mode decision
 *************************************************************************************
 */
void Intra16x16_Mode_Decision (Macroblock* currMB, int* i16mode)
{
  intrapred_luma_16x16 ();   /* make intra pred for all 4 new modes */

  find_sad_16x16 (i16mode);   /* get best new intra mode */

  currMB->cbp = dct_luma_16x16 (*i16mode);
}



/*!
 *************************************************************************************
 * \brief
 *    Sets Coefficients and reconstruction for an 8x8 block
 *************************************************************************************
 */
void SetCoeffAndReconstruction8x8 (Macroblock* currMB)
{
  int block, k, j, i;
  int cur_ref[2];

  //============= MIXED TRANSFORM SIZES FOR 8x8 PARTITION ==============
  //--------------------------------------------------------------------
  int l;
  int bframe = img->type==B_SLICE;

  if (currMB->luma_transform_size_8x8_flag)
  {

    //============= set mode and ref. frames ==============
    for(i = 0;i<4;i++)
    {
      currMB->b8mode[i]   = tr8x8.part8x8mode[i];
      currMB->b8pdir[i]   = tr8x8.part8x8pdir[i];
    }

    if (bframe)
    {
      for (j = 0;j<4;j++)
        for (i = 0;i<4;i++)
        {
          k = 2*(j >> 1)+(i >> 1);
          l = 2*(j & 0x01)+(i & 0x01);
          enc_picture->ref_idx[LIST_0][img->block_y+j][img->block_x+i] = ((currMB->b8pdir[k] & 0x01) == 0) ? tr8x8.part8x8fwref[k] : - 1;
          enc_picture->ref_idx[LIST_1][img->block_y+j][img->block_x+i] = (currMB->b8pdir[k] > 0) ? tr8x8.part8x8bwref[k] : - 1;
        }
    }
    else
    {
      for (j = 0;j<4;j++)
        for (i = 0;i<4;i++)
        {
          k = 2*(j >> 1)+(i >> 1);
          l = 2*(j & 0x01)+(i & 0x01);
          enc_picture->ref_idx[LIST_0][img->block_y+j][img->block_x+i] = tr8x8.part8x8fwref[k];
        }
    }


    for (j = img->block_y;j<img->block_y + BLOCK_MULTIPLE;j++)
    {
      for (i = img->block_x;i<img->block_x + BLOCK_MULTIPLE;i++)
      {
        cur_ref[LIST_0] = (int) enc_picture->ref_idx[LIST_0][j][i];

        enc_picture->ref_pic_id [LIST_0][j][i] =(cur_ref[LIST_0]>=0
        ? enc_picture->ref_pic_num[LIST_0 + currMB->list_offset][cur_ref[LIST_0]]
        : -1);
      }
    }

    if (bframe)
    {
      for (j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
      {
        for (i = img->block_x;i<img->block_x + BLOCK_MULTIPLE;i++)
        {
          cur_ref[LIST_1] = (int) enc_picture->ref_idx[LIST_1][j][i];

          enc_picture->ref_pic_id [LIST_1][j][i] = (cur_ref[LIST_1]>=0
            ? enc_picture->ref_pic_num[LIST_1 + currMB->list_offset][cur_ref[LIST_1]]
            : -1);
        }

      }
    }

    //====== set the mv's for 8x8 partition with transform size 8x8 ======
    //save the mv data for 4x4 transform

    StoreMV8x8(1);
    //set new mv data for 8x8 transform
    RestoreMV8x8(0);

    //============= get pre-calculated data ==============
    //restore coefficients from 8x8 transform

    for (block = 0; block<4; block++)
    {
      for (k = 0; k<4; k++)
        for (j = 0; j<2; j++)
          memcpy (img->cofAC[block][k][j],cofAC_8x8ts[block][k][j], 65 * sizeof(int));
    }
    //restore reconstruction
    if (cnt_nonz8_8x8ts <= _LUMA_8x8_COEFF_COST_ &&
      ((img->qp_scaled)!=0 || img->lossless_qpprime_flag==0) &&
      (img->type!=SP_SLICE))// modif ES added last condition (we probably never go there so is the next modification useful ? check)
    {
      currMB->cbp     = 0;
      currMB->cbp_blk = 0;
      for (j = 0; j < MB_BLOCK_SIZE; j++)
      {
        memcpy(&enc_picture->imgY[img->pix_y+j][img->pix_x], tr8x8.mpr8x8[j], MB_BLOCK_SIZE * sizeof(imgpel));
        if(img->type==SP_SLICE &&(!si_frame_indicator && !sp2_frame_indicator ))
          memcpy(&lrec[img->pix_y+j][img->pix_x],tr8x8.lrec[j], MB_BLOCK_SIZE * sizeof(int));
      }
    }
    else
    {
      currMB->cbp     = cbp8_8x8ts;
      currMB->cbp_blk = cbp_blk8_8x8ts;
      for (j = 0; j < MB_BLOCK_SIZE; j++)
      {
        memcpy (&enc_picture->imgY[img->pix_y+j][img->pix_x],tr8x8.rec_mbY8x8[j], MB_BLOCK_SIZE * sizeof(imgpel));
        if(img->type==SP_SLICE &&(!si_frame_indicator && !sp2_frame_indicator))
          memcpy (&lrec[img->pix_y+j][img->pix_x],tr8x8.lrec[j], MB_BLOCK_SIZE * sizeof(int));
      }
    }
  }
  else
  {
    //============= get pre-calculated data ==============
    //---------------------------------------------------
    //--- restore coefficients ---
    for (block = 0; block<4+img->num_blk8x8_uv; block++)
    {
      for (k = 0; k<4; k++)
        for (j = 0; j<2; j++)
          memcpy (img->cofAC[block][k][j],cofAC8x8[block][k][j], 65 * sizeof(int));
    }

    if (cnt_nonz_8x8<=5 && img->type!=SP_SLICE &&
      ((img->qp_scaled)!=0 || img->lossless_qpprime_flag==0))
    {
      currMB->cbp     = 0;
      currMB->cbp_blk = 0;
      for (j = 0; j < MB_BLOCK_SIZE; j++)
      {
        memcpy (&enc_picture->imgY[img->pix_y+j][img->pix_x],tr4x4.mpr8x8[j], MB_BLOCK_SIZE * sizeof(imgpel));
        if(img->type ==SP_SLICE &&(!si_frame_indicator && !sp2_frame_indicator))
          memcpy (&lrec[img->pix_y+j][img->pix_x],tr4x4.lrec[j], MB_BLOCK_SIZE * sizeof(int)); // restore coeff. SP frame
      }
    }
    else
    {
      currMB->cbp     = cbp8x8;
      currMB->cbp_blk = cbp_blk8x8;
      for (j = 0; j < MB_BLOCK_SIZE; j++)
      {
        memcpy (&enc_picture->imgY[img->pix_y+j][img->pix_x],tr4x4.rec_mbY8x8[j], MB_BLOCK_SIZE * sizeof(imgpel));
        if(img->type==SP_SLICE &&(!si_frame_indicator && !sp2_frame_indicator))
          memcpy (&lrec[img->pix_y+j][img->pix_x],tr4x4.lrec[j], MB_BLOCK_SIZE * sizeof(int));
      }
    }
  }
}


/*!
 *************************************************************************************
 * \brief
 *    Sets motion vectors for a macroblock
 *************************************************************************************
 */
void SetMotionVectorsMB (Macroblock* currMB, int bframe)
{
  int i, j, k, l, m, mode8, pdir8, ref, by, bx;
  short ******all_mv  = img->all_mv;
  short ******pred_mv = img->pred_mv;
  int  bw_ref;
  int jdiv, jmod;

  if (!bframe)
  {
    for (j = 0; j<4; j++)
    {
      jmod = j & 0x01;
      jdiv = j >>   1;
      by    = img->block_y+j;
      for (i = 0; i<4; i++)
      {
        mode8 = currMB->b8mode[k=2*jdiv+(i>>1)];
        l     = 2*jmod + (i & 0x01);

        bx   = img->block_x+i;

        pdir8 = currMB->b8pdir[k];
        ref    = enc_picture->ref_idx[LIST_0][by][bx];

        if (pdir8>=0)
        {
          enc_picture->mv[LIST_0][by][bx][0] = all_mv [j][i][LIST_0][ ref][mode8][0];
          enc_picture->mv[LIST_0][by][bx][1] = all_mv [j][i][LIST_0][ ref][mode8][1];
        }
        else
        {
          enc_picture->mv[LIST_0][by][bx][0] = 0;
          enc_picture->mv[LIST_0][by][bx][1] = 0;
        }
      }
    }
  }
else
{
  for (j = 0; j<4; j++)
  {
    jmod = j & 0x01;
    jdiv = j >>   1;
    by    = img->block_y+j;
    for (i = 0; i<4; i++)
    {
      mode8 = currMB->b8mode[k=2*jdiv+(i>>1)];
      l     = 2*jmod + (i & 0x01);

      bx    = img->block_x+i;

      pdir8 = currMB->b8pdir[k];
      ref    = enc_picture->ref_idx[LIST_0][by][bx];
      bw_ref = enc_picture->ref_idx[LIST_1][by][bx];

      if (currMB->bi_pred_me && (pdir8 == 2) && currMB->mb_type==1)
      {
        all_mv  = currMB->bi_pred_me == 1 ? img->bipred_mv1 : img->bipred_mv2;
        ref = 0;
        bw_ref = 0;
      }

      if (pdir8==-1) // intra
      {
        enc_picture->mv[LIST_0][by][bx][0] = 0;
        enc_picture->mv[LIST_0][by][bx][1] = 0;
        enc_picture->mv[LIST_1][by][bx][0] = 0;
        enc_picture->mv[LIST_1][by][bx][1] = 0;
      }
      else if (pdir8==0) // list 0
      {
        enc_picture->mv[LIST_0][by][bx][0] = all_mv [j][i][LIST_0][ ref][mode8][0];
        enc_picture->mv[LIST_0][by][bx][1] = all_mv [j][i][LIST_0][ ref][mode8][1];
        enc_picture->mv[LIST_1][by][bx][0] = 0;
        enc_picture->mv[LIST_1][by][bx][1] = 0;
        enc_picture->ref_idx[LIST_1][by][bx] = -1;
      }
      else if (pdir8==1) // list 1
      {
        enc_picture->mv[LIST_0][by][bx][0] = 0;
        enc_picture->mv[LIST_0][by][bx][1] = 0;
        enc_picture->ref_idx[LIST_0][by][bx] = -1;
        enc_picture->mv[LIST_1][by][bx][0] = all_mv [j][i][LIST_1][bw_ref][mode8][0];
        enc_picture->mv[LIST_1][by][bx][1] = all_mv [j][i][LIST_1][bw_ref][mode8][1];
      }
      else if (pdir8==2) // bipredictive
      {
        enc_picture->mv[LIST_0][by][bx][0] = all_mv [j][i][LIST_0][ ref][mode8][0];
        enc_picture->mv[LIST_0][by][bx][1] = all_mv [j][i][LIST_0][ ref][mode8][1];
        enc_picture->mv[LIST_1][by][bx][0] = all_mv [j][i][LIST_1][bw_ref][mode8][0];
        enc_picture->mv[LIST_1][by][bx][1] = all_mv [j][i][LIST_1][bw_ref][mode8][1];
      }
      else
      {
        error("invalid direction mode", 255);
      }
    }
  }
}

  // copy all the motion vectors into rdopt structure
  // Can simplify this by copying the MV's of the best mode (TBD)
  if(img->MbaffFrameFlag)
  {
    for(i = 0;i<4;i++)
    {
      for(j = 0;j<4;j++)
      {
        for (k = 0;k<2;k++)
        {
          for(l = 0;l<img->max_num_references;l++)
          {
            for(m = 0;m<9;m++)
            {
              rdopt->all_mv [j][i][k][l][m][0]  = all_mv [j][i][k][l][m][0];
              rdopt->pred_mv[j][i][k][l][m][0]  = pred_mv[j][i][k][l][m][0];

              rdopt->all_mv [j][i][k][l][m][1]  = all_mv [j][i][k][l][m][1];
              rdopt->pred_mv[j][i][k][l][m][1]  = pred_mv[j][i][k][l][m][1];
            }
          }
        }
      }
    }
  }
}



/*!
 *************************************************************************************
 * \brief
 *    R-D Cost for a macroblock
 *************************************************************************************
 */
int RDCost_for_macroblocks (double   lambda,       // <-- lagrange multiplier
                            int      mode,         // <-- modus (0-COPY/DIRECT, 1-16x16, 2-16x8, 3-8x16, 4-8x8(+), 5-Intra4x4, 6-Intra16x16)
                            double*  min_rdcost,   // <-> minimum rate-distortion cost
                            double*  min_rate,     // --> bitrate of mode which has minimum rate-distortion cost.
                            int i16mode )
{
  int         i, j, k; //, k, ****ip4;
  int         j1, j2;
  int         rate = 0, coeff_rate = 0;
  int64       distortion = 0;
  double      rdcost;
  int         prev_mb_nr  = FmoGetPreviousMBNr(img->current_mb_nr);
  Macroblock  *currMB   = &img->mb_data[img->current_mb_nr];
  Macroblock  *prevMB   = (prev_mb_nr >= 0) ? &img->mb_data[prev_mb_nr] : NULL;
  int         bframe    = (img->type==B_SLICE);
  int         tmp_cc;
  int         use_of_cc =  (img->type!=I_SLICE &&  input->symbol_mode!=CABAC);
  int         cc_rate, dummy;

  //=====
  //=====  SET REFERENCE FRAMES AND BLOCK MODES
  //=====
  SetModesAndRefframeForBlocks (mode);

  //=====
  //=====  GET COEFFICIENTS, RECONSTRUCTIONS, CBP
  //=====
  if (bframe && mode==0)
  {
    int block_x=img->pix_x>>2;
    int block_y=img->pix_y>>2;
    for (j = block_y;j< block_y + 4;j++)
      for (i = block_x;i<block_x + 4;i++)
        if (direct_pdir[j][i] < 0)
          return 0;
  }

  // Test MV limits for Skip Mode. This could be necessary for MBAFF case Frame MBs.
  if ((img->MbaffFrameFlag) && (!currMB->mb_field) && (img->type==P_SLICE) && (mode==0) )
  {
    if ( img->all_mv[0][0][0][0][0][0] < -8192
      || img->all_mv[0][0][0][0][0][0] > 8191
      || img->all_mv[0][0][0][0][0][1] < LEVELMVLIMIT[img->LevelIndex][4]
      || img->all_mv[0][0][0][0][0][1] > LEVELMVLIMIT[img->LevelIndex][5])
      return 0;
  }

  if (img->AdaptiveRounding)
  {
    memset(&(img->fadjust4x4[0][0][0]), 0, MB_PIXELS * sizeof(int));
    memset(&(img->fadjust8x8[0][0][0]), 0, MB_PIXELS * sizeof(int));
    memset(&(img->fadjust4x4Cr[0][0][0][0]), 0, img->mb_cr_size_y * img->mb_cr_size_x * sizeof(int));
    memset(&(img->fadjust4x4Cr[0][1][0][0]), 0, img->mb_cr_size_y * img->mb_cr_size_x  * sizeof(int));
  }

  if (mode<P8x8)
  {
    LumaResidualCoding ();

    if(mode==0 && currMB->cbp!=0 && (img->type != B_SLICE || img->NoResidueDirect==1))
      return 0;
    if(mode==0 && currMB->cbp==0 && currMB->luma_transform_size_8x8_flag==1) //for B_skip, luma_transform_size_8x8_flag=0 only
      return 0;
  }
  else if (mode==P8x8)
  {
    SetCoeffAndReconstruction8x8 (currMB);
  }
  else if (mode==I4MB)
  {
    currMB->cbp = Mode_Decision_for_Intra4x4Macroblock (lambda, &dummy);
  }
  else if (mode==I16MB)
  {
    Intra16x16_Mode_Decision  (currMB, &i16mode);
  }
  else if(mode==I8MB)
  {
    currMB->cbp = Mode_Decision_for_new_Intra8x8Macroblock(lambda, &dummy);
  }
  else if(mode==IPCM)
  {
    for (j = 0; j < MB_BLOCK_SIZE; j++)
    {
      j1 = j + img->opix_y;
      j2 = j + img->pix_y;
      for (i=img->opix_x; i<img->opix_x+MB_BLOCK_SIZE; i++)
        enc_picture->imgY[j2][i] = imgY_org[j1][i];
    }
    if (img->yuv_format != YUV400)
    {
      // CHROMA
      for (j = 0; j<img->mb_cr_size_y; j++)
      {
        j1 = j + img->opix_c_y;
        j2 = j + img->pix_c_y;
        for (i=img->opix_c_x; i<img->opix_c_x+img->mb_cr_size_x; i++)
        {
          enc_picture->imgUV[0][j2][i] = imgUV_org[0][j1][i];
          enc_picture->imgUV[1][j2][i] = imgUV_org[1][j1][i];
        }
      }
    }
    for (j=0;j<4;j++)
      for (i=0; i<(4+img->num_blk8x8_uv); i++)
        img->nz_coeff[img->current_mb_nr][j][i] = 16;

  }

  if (input->rdopt==3 && img->type!=B_SLICE)
  {
    // We need the reconstructed prediction residue for the simulated decoders.
    compute_residue_mb (mode==I16MB?i16mode:-1);
  }

  //Rate control
  if (input->RCEnable)
  {
    if (mode == I16MB)
      memcpy(pred,img->mprr_2[i16mode],MB_PIXELS * sizeof(imgpel));
    else
      memcpy(pred,img->mpr,MB_PIXELS * sizeof(imgpel));
  }

  img->i16offset = 0;
  dummy = 0;
  if ((img->yuv_format!=YUV400) && (mode != IPCM))
    ChromaResidualCoding (&dummy);

  if (mode==I16MB)
    img->i16offset = I16Offset  (currMB->cbp, i16mode);

  //=====
  //=====   GET DISTORTION
  //=====
  // LUMA
  if (input->rdopt==3 && img->type!=B_SLICE)
  {
    for (k = 0; k<input->NoOfDecoders ;k++)
    {
      decode_one_mb (k, currMB);
      for (j = 0; j<MB_BLOCK_SIZE; j++)
      {
        for (i=img->opix_x; i<img->opix_x+MB_BLOCK_SIZE; i++)
          distortion += img->quad [imgY_org[img->opix_y+j][i] - decs->decY[k][img->opix_y+j][i]];
      }
    }
    distortion /= input->NoOfDecoders;

    if (img->yuv_format != YUV400)
    {
      // CHROMA
      for (j = 0; j<img->mb_cr_size_y; j++)
      {
        j1 = j + img->opix_c_y;
        j2 = j + img->pix_c_y;
        for (i=img->opix_c_x; i<img->opix_c_x+img->mb_cr_size_x; i++)
        {
          distortion += img->quad [imgUV_org[0][j1][i] - enc_picture->imgUV[0][j2][i]];
          distortion += img->quad [imgUV_org[1][j1][i] - enc_picture->imgUV[1][j2][i]];
        }
      }
    }
  }
  else
  {
    // LUMA
    for (j = 0; j < MB_BLOCK_SIZE; j++)
    {
      j1 = j + img->opix_y;
      j2 = j + img->pix_y;
      for (i=img->opix_x; i<img->opix_x+MB_BLOCK_SIZE; i++)
        distortion += img->quad [imgY_org[j1][i] - enc_picture->imgY[j2][i]];
    }

    if (img->yuv_format != YUV400)
    {
      // CHROMA
      for (j = 0; j<img->mb_cr_size_y; j++)
      {
        j1 = j + img->opix_c_y;
        j2 = j + img->pix_c_y;
        for (i=img->opix_c_x; i<img->opix_c_x+img->mb_cr_size_x; i++)
        {
          distortion += img->quad [imgUV_org[0][j1][i] - enc_picture->imgUV[0][j2][i]];
          distortion += img->quad [imgUV_org[1][j1][i] - enc_picture->imgUV[1][j2][i]];
        }
      }
    }
  }

  //=====   S T O R E   C O D I N G   S T A T E   =====
  //---------------------------------------------------
  store_coding_state (cs_cm);

  //=====
  //=====   GET RATE
  //=====
  //----- macroblock header -----
  if (use_of_cc)
  {
    if (currMB->mb_type!=0 || (bframe && currMB->cbp!=0))
    {
      // cod counter and macroblock mode are written ==> do not consider code counter
      tmp_cc = img->cod_counter;
      rate   = writeMBLayer (1, &coeff_rate);
      ue_linfo (tmp_cc, dummy, &cc_rate, &dummy);
      rate  -= cc_rate;
      img->cod_counter = tmp_cc;
    }
    else
    {
      // cod counter is just increased  ==> get additional rate
      ue_linfo (img->cod_counter+1, dummy, &rate,    &dummy);
      ue_linfo (img->cod_counter,   dummy, &cc_rate, &dummy);
      rate -= cc_rate;
    }
  }
  else
  {
    rate = writeMBLayer (1, &coeff_rate);
  }

  //=====   R E S T O R E   C O D I N G   S T A T E   =====
  //-------------------------------------------------------
  reset_coding_state (cs_cm);

  rdcost = (double)distortion + lambda * dmax(0.5,(double)rate);

  if (rdcost >= *min_rdcost ||
    ((img->qp_scaled)==0 && img->lossless_qpprime_flag==1 && distortion!=0))
  {
#if FASTMODE
    // Reordering RDCost comparison order of mode 0 and mode 1 in P_SLICE
    // if RDcost of mode 0 and mode 1 is same, we choose best_mode is 0
    // This might not always be good since mode 0 is more biased towards rate than quality.
    if((img->type!=P_SLICE || mode != 0 || rdcost != *min_rdcost) || input->ProfileIDC>=FREXT_HP)
#endif
      return 0;
  }


  if ((img->MbaffFrameFlag) && (mode ? 0: ((img->type == B_SLICE) ? !currMB->cbp:1)))  // AFF and current is skip
  {
    if (img->current_mb_nr & 0x01) //bottom
    {
      if (prevMB->mb_type ? 0:((img->type == B_SLICE) ? !prevMB->cbp:1)) //top is skip
      {
        if (!(field_flag_inference() == currMB->mb_field)) //skip only allowed when correct inference
          return 0;
      }
    }
  }

  //=====   U P D A T E   M I N I M U M   C O S T   =====
  //-----------------------------------------------------
  *min_rdcost = rdcost;
  *min_rate = lambda * (double)coeff_rate;

#ifdef BEST_NZ_COEFF
  for (j=0;j<4;j++)
    for (i=0; i<(4+img->num_blk8x8_uv); i++)
      gaaiMBAFF_NZCoeff[j][i] = img->nz_coeff[img->current_mb_nr][j][i];
#endif

  return 1;
}

/*!
 *************************************************************************************
 * \brief
 *    Store adaptive rounding parameters
 *************************************************************************************
 */
void store_adaptive_rounding_parameters (int mode, Macroblock *currMB)
{
  int j;
  int is_inter = (mode != I4MB)&&(mode != I16MB)&&(mode != I8MB);

  if (currMB->luma_transform_size_8x8_flag)
  {
    if ((mode == P8x8))
      memcpy(&(bestInterFAdjust8x8[0][0]),&(img->fadjust8x8[2][0][0]),MB_PIXELS * sizeof(int));
    else if (is_inter)
      memcpy(&(bestInterFAdjust8x8[0][0]),&(img->fadjust8x8[0][0][0]),MB_PIXELS * sizeof(int));
    else
      memcpy(&(bestIntraFAdjust8x8[0][0]),&(img->fadjust8x8[1][0][0]),MB_PIXELS * sizeof(int));
  }
  else
  {
    if ((mode == P8x8))
      memcpy(&(bestInterFAdjust4x4[0][0]),&(img->fadjust4x4[3][0][0]),MB_PIXELS * sizeof(int));
    else if (is_inter)
      memcpy(&(bestInterFAdjust4x4[0][0]),&(img->fadjust4x4[0][0][0]),MB_PIXELS * sizeof(int));
    else
      memcpy(&(bestIntraFAdjust4x4[0][0]),&(img->fadjust4x4[1 + mode == I16MB][0][0]),MB_PIXELS * sizeof(int));
  }
  if (input->AdaptRndChroma)
  {
    if (currMB->luma_transform_size_8x8_flag && mode == P8x8)
    {
      for (j = 0; j < img->mb_cr_size_y; j++)
      {
        memcpy(bestInterFAdjust4x4Cr[0][j],img->fadjust8x8Cr[0][0][j],img->mb_cr_size_x * sizeof(int));
        memcpy(bestInterFAdjust4x4Cr[1][j],img->fadjust8x8Cr[0][1][j],img->mb_cr_size_x * sizeof(int));
      }
    }
    else if (mode == P8x8)
    {
      for (j = 0; j < img->mb_cr_size_y; j++)
      {
        memcpy(bestInterFAdjust4x4Cr[0][j],img->fadjust4x4Cr[2][0][j],img->mb_cr_size_x * sizeof(int));
        memcpy(bestInterFAdjust4x4Cr[1][j],img->fadjust4x4Cr[2][1][j],img->mb_cr_size_x * sizeof(int));
      }
    }
    else if (is_inter)
    {
      for (j = 0; j < img->mb_cr_size_y; j++)
      {
        memcpy(bestInterFAdjust4x4Cr[0][j],img->fadjust4x4Cr[0][0][j],img->mb_cr_size_x * sizeof(int));
        memcpy(bestInterFAdjust4x4Cr[1][j],img->fadjust4x4Cr[0][1][j],img->mb_cr_size_x * sizeof(int));
      }
    }
    else
    {
      for (j = 0; j < img->mb_cr_size_y; j++)
      {
        memcpy(bestIntraFAdjust4x4Cr[0][j],img->fadjust4x4Cr[1][0][j],img->mb_cr_size_x * sizeof(int));
        memcpy(bestIntraFAdjust4x4Cr[1][j],img->fadjust4x4Cr[1][1][j],img->mb_cr_size_x * sizeof(int));
      }
    }
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Store macroblock parameters
 *************************************************************************************
 */
void store_macroblock_parameters (int mode)
{
  int  i, j, k, ****i4p, ***i3p;
  Macroblock *currMB  = &img->mb_data[img->current_mb_nr];
  int        bframe   = (img->type==B_SLICE);

  //--- store best mode ---
  best_mode = mode;
  best_c_imode = currMB->c_ipred_mode;
  best_i16offset = img->i16offset;

  // If condition is not really necessary.
  bi_pred_me = (mode == 1) ? currMB->bi_pred_me : 0;

  memcpy(b8mode, currMB->b8mode, BLOCK_MULTIPLE * sizeof(int));
  memcpy(b8pdir, currMB->b8pdir, BLOCK_MULTIPLE * sizeof(int));
  memcpy(b4_intra_pred_modes,currMB->intra_pred_modes, MB_BLOCK_PARTITIONS * sizeof(char));
  memcpy(b8_intra_pred_modes8x8,currMB->intra_pred_modes8x8, MB_BLOCK_PARTITIONS * sizeof(char));

  for (j = 0 ; j < BLOCK_MULTIPLE; j++)
  {
    memcpy(&b4_ipredmode[j * BLOCK_MULTIPLE],&img->ipredmode[img->block_y + j][img->block_x],BLOCK_MULTIPLE * sizeof(char));
    memcpy(b8_ipredmode8x8[j],&img->ipredmode8x8[img->block_y + j][img->block_x],BLOCK_MULTIPLE * sizeof(char));
  }
  //--- reconstructed blocks ----
  for (j = 0; j < MB_BLOCK_SIZE; j++)
  {
    memcpy(rec_mbY[j],&enc_picture->imgY[img->pix_y+j][img->pix_x], MB_BLOCK_SIZE * sizeof(imgpel));
  }
  if((img->type==SP_SLICE) && (si_frame_indicator==0 && sp2_frame_indicator==0))
  {
    for (j = 0; j < MB_BLOCK_SIZE; j++)
    {
      memcpy(lrec_rec[j],&lrec[img->pix_y+j][img->pix_x], MB_BLOCK_SIZE * sizeof(int));//store coefficients SP frame
    }
  }

  if (img->AdaptiveRounding)
    store_adaptive_rounding_parameters (mode, currMB);

  if (img->yuv_format != YUV400)
  {
    for (j = 0; j<img->mb_cr_size_y; j++)
    {
      memcpy(rec_mbU[j],&enc_picture->imgUV[0][img->pix_c_y+j][img->pix_c_x], img->mb_cr_size_x * sizeof(imgpel));
      memcpy(rec_mbV[j],&enc_picture->imgUV[1][img->pix_c_y+j][img->pix_c_x], img->mb_cr_size_x * sizeof(imgpel));
    }
    if((img->type==SP_SLICE) && (si_frame_indicator==0 && sp2_frame_indicator==0))
    {
      //store uv coefficients SP frame
      for (j = 0; j<img->mb_cr_size_y; j++)
      {
        memcpy(lrec_rec_U[j],&lrec_uv[0][img->pix_c_y+j][img->pix_c_x], img->mb_cr_size_x * sizeof(int));
        memcpy(lrec_rec_V[j],&lrec_uv[1][img->pix_c_y+j][img->pix_c_x], img->mb_cr_size_x * sizeof(int));
      }
    }
  }

  //--- store results of decoders ---
  if (input->rdopt==3 && img->type!=B_SLICE)
  {
    for (k = 0; k<input->NoOfDecoders; k++)
    {
      for (j=img->pix_y; j<img->pix_y+16; j++)
        for (i=img->pix_x; i<img->pix_x+16; i++)
        {
          // Keep the decoded values of each MB for updating the ref frames
          decs->decY_best[k][j][i] = decs->decY[k][j][i];
        }
    }
  }

  //--- coeff, cbp, kac ---
  if (mode || bframe)
  {
    i4p=cofAC; cofAC=img->cofAC; img->cofAC=i4p;
    i3p=cofDC; cofDC=img->cofDC; img->cofDC=i3p;
    cbp     = currMB->cbp;
    cbp_blk = currMB->cbp_blk;
  }
  else
  {
    cbp_blk = cbp = 0;
  }

  //--- store transform size ---
  luma_transform_size_8x8_flag = currMB->luma_transform_size_8x8_flag;


  for (j = 0; j<4; j++)
    memcpy(frefframe[j],&enc_picture->ref_idx[LIST_0][img->block_y+j][img->block_x], BLOCK_MULTIPLE * sizeof(char));

  if (bframe)
  {
    for (j = 0; j<4; j++)
      memcpy(brefframe[j],&enc_picture->ref_idx[LIST_1][img->block_y+j][img->block_x], BLOCK_MULTIPLE * sizeof(char));
    }
}


/*!
 *************************************************************************************
 * \brief
 *    Set stored macroblock parameters
 *************************************************************************************
 */
void set_stored_macroblock_parameters ()
{
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];
  imgpel     **imgY  = enc_picture->imgY;
  imgpel    ***imgUV = enc_picture->imgUV;

  int         mode   = best_mode;
  int         bframe = (img->type==B_SLICE);
  int         i, j, k, ****i4p, ***i3p;
  int         block_x, block_y;
  signed char **ipredmodes = img->ipredmode;
  short   *cur_mv;

  //===== reconstruction values =====
  for (j = 0; j < MB_BLOCK_SIZE; j++)
  {
    memcpy(&imgY[img->pix_y+j][img->pix_x],rec_mbY[j], MB_BLOCK_SIZE * sizeof(imgpel));
  }
  if((img->type==SP_SLICE) &&(si_frame_indicator==0 && sp2_frame_indicator==0 ))
  {
    for (j = 0; j < MB_BLOCK_SIZE; j++)
      memcpy(&lrec[img->pix_y+j][img->pix_x],lrec_rec[j], MB_BLOCK_SIZE * sizeof(int)); //restore coeff SP frame
  }
  if(img->MbaffFrameFlag)
  {
    for (j = 0; j < MB_BLOCK_SIZE; j++)
      memcpy(rdopt->rec_mbY[j],rec_mbY[j], MB_BLOCK_SIZE * sizeof(imgpel));
  }

  if (img->AdaptiveRounding)
  {
    update_offset_params(mode,luma_transform_size_8x8_flag);
  }

  if (img->yuv_format != YUV400)
  {
    for (j = 0; j<img->mb_cr_size_y; j++)
    {
      memcpy(&imgUV[0][img->pix_c_y+j][img->pix_c_x],rec_mbU[j], img->mb_cr_size_x * sizeof(imgpel));
      memcpy(&imgUV[1][img->pix_c_y+j][img->pix_c_x],rec_mbV[j], img->mb_cr_size_x * sizeof(imgpel));
      if((img->type==SP_SLICE) &&(!si_frame_indicator && !sp2_frame_indicator))
      {
        memcpy(&lrec_uv[0][img->pix_c_y+j][img->pix_c_x],lrec_rec_U[j], img->mb_cr_size_x * sizeof(int));
        memcpy(&lrec_uv[1][img->pix_c_y+j][img->pix_c_x],lrec_rec_V[j], img->mb_cr_size_x * sizeof(int));
      }
      if(img->MbaffFrameFlag)
      {
        memcpy(rdopt->rec_mbU[j],rec_mbU[j], img->mb_cr_size_x * sizeof(imgpel));
        memcpy(rdopt->rec_mbV[j],rec_mbV[j], img->mb_cr_size_x * sizeof(imgpel));
      }
    }

    if((img->type==SP_SLICE) &&(!si_frame_indicator && !sp2_frame_indicator))
    {
      for (j = 0; j<img->mb_cr_size_y; j++)
      {
        memcpy(&lrec_uv[0][img->pix_c_y+j][img->pix_c_x],lrec_rec_U[j], img->mb_cr_size_x * sizeof(int));
        memcpy(&lrec_uv[1][img->pix_c_y+j][img->pix_c_x],lrec_rec_V[j], img->mb_cr_size_x * sizeof(int));
      }
    }

    if(img->MbaffFrameFlag)
    {
      for (j = 0; j<img->mb_cr_size_y; j++)
      {

        memcpy(rdopt->rec_mbU[j],rec_mbU[j], img->mb_cr_size_x * sizeof(imgpel));
        memcpy(rdopt->rec_mbV[j],rec_mbV[j], img->mb_cr_size_x * sizeof(imgpel));
      }
    }
  }

  //===== coefficients and cbp =====
  i4p=cofAC; cofAC=img->cofAC; img->cofAC=i4p;
  i3p=cofDC; cofDC=img->cofDC; img->cofDC=i3p;
  currMB->cbp      = cbp;
  currMB->cbp_blk = cbp_blk;
  //==== macroblock type ====
  currMB->mb_type = mode;

  if(img->MbaffFrameFlag)
  {
    rdopt->mode = mode;
    rdopt->i16offset = img->i16offset;
    rdopt->cbp = cbp;
    rdopt->cbp_blk = cbp_blk;
    rdopt->mb_type  = mode;

    rdopt->prev_qp       = currMB->prev_qp;
    rdopt->prev_delta_qp = currMB->prev_delta_qp;
    rdopt->delta_qp      = currMB->delta_qp;
    rdopt->qp            = currMB->qp;
    rdopt->prev_cbp      = currMB->prev_cbp;

    for(i = 0;i<4+img->num_blk8x8_uv;i++)
    {
      for(j = 0;j<4;j++)
        for(k = 0;k<2;k++)
          memcpy(rdopt->cofAC[i][j][k], img->cofAC[i][j][k], 65 * sizeof(int));
    }
    for(i = 0;i<3;i++)
      for(k = 0;k<2;k++)
        memcpy(rdopt->cofDC[i][k], img->cofDC[i][k], 18 * sizeof(int));
  }


  memcpy(currMB->b8mode,b8mode, BLOCK_MULTIPLE * sizeof(int));
  memcpy(currMB->b8pdir,b8pdir, BLOCK_MULTIPLE * sizeof(int));
  if(img->MbaffFrameFlag)
  {
    memcpy(rdopt->b8mode,b8mode, BLOCK_MULTIPLE * sizeof(int));
    memcpy(rdopt->b8pdir,b8pdir, BLOCK_MULTIPLE * sizeof(int));
  }

  currMB->bi_pred_me = currMB->mb_type == 1 ? bi_pred_me : 0;


  //if P8x8 mode and transform size 4x4 choosen, restore motion vector data for this transform size
  if (mode == P8x8 && !luma_transform_size_8x8_flag && input->Transform8x8Mode)
    RestoreMV8x8(1);

  //==== transform size flag ====
  if (((currMB->cbp & 15) == 0) && !(IS_OLDINTRA(currMB) || currMB->mb_type == I8MB))
    currMB->luma_transform_size_8x8_flag = 0;
  else
    currMB->luma_transform_size_8x8_flag = luma_transform_size_8x8_flag;

  rdopt->luma_transform_size_8x8_flag  = currMB->luma_transform_size_8x8_flag;

  if (input->rdopt==3 && img->type!=B_SLICE)
  {
    //! save the MB Mode of every macroblock
    decs->dec_mb_mode[img->mb_y][img->mb_x] = mode;
  }

  //==== reference frames =====
  for (j = 0; j < 4; j++)
  {
    block_y = img->block_y + j;
    for (i = 0; i < 4; i++)
    {
      block_x = img->block_x + i;
      k = 2*(j >> 1)+(i >> 1);

      // backward prediction or intra
      if ((currMB->b8pdir[k] == 1) || IS_INTRA(currMB))
      {
        enc_picture->ref_idx    [LIST_0][block_y][block_x]    = -1;
        enc_picture->ref_pic_id [LIST_0][block_y][block_x]    = -1;
        enc_picture->mv         [LIST_0][block_y][block_x][0] = 0;
        enc_picture->mv         [LIST_0][block_y][block_x][1] = 0;
        if(img->MbaffFrameFlag)
          rdopt->refar[LIST_0][j][i] = -1;
      }
      else
      {
        if (currMB->bi_pred_me && (currMB->b8pdir[k] == 2) && currMB->mb_type==1)
        {
          cur_mv = currMB->bi_pred_me == 1
            ? img->bipred_mv1[j][i][LIST_0][0][currMB->b8mode[k]]
            : img->bipred_mv2[j][i][LIST_0][0][currMB->b8mode[k]];

          enc_picture->ref_idx    [LIST_0][block_y][block_x] = 0;
          enc_picture->ref_pic_id [LIST_0][block_y][block_x] = enc_picture->ref_pic_num[LIST_0 + currMB->list_offset][0];
          enc_picture->mv         [LIST_0][block_y][block_x][0] = cur_mv[0];
          enc_picture->mv         [LIST_0][block_y][block_x][1] = cur_mv[1];
          if(img->MbaffFrameFlag)
            rdopt->refar[LIST_0][j][i] = 0;
        }
        else
        {
          cur_mv = img->all_mv[j][i][LIST_0][(short)frefframe[j][i]][currMB->b8mode[k]];

          enc_picture->ref_idx    [LIST_0][block_y][block_x]    = frefframe[j][i];
          enc_picture->ref_pic_id [LIST_0][block_y][block_x]    = enc_picture->ref_pic_num[LIST_0 + currMB->list_offset][(short)frefframe[j][i]];
          enc_picture->mv         [LIST_0][block_y][block_x][0] = cur_mv[0];
          enc_picture->mv         [LIST_0][block_y][block_x][1] = cur_mv[1];
          if(img->MbaffFrameFlag)
            rdopt->refar[LIST_0][j][i] = frefframe[j][i];
        }
      }

      // forward prediction or intra
      if ((currMB->b8pdir[k] == 0) || IS_INTRA(currMB))
      {
        enc_picture->ref_idx    [LIST_1][block_y][block_x]    = -1;
        enc_picture->ref_pic_id [LIST_1][block_y][block_x]    = -1;
        enc_picture->mv         [LIST_1][block_y][block_x][0] = 0;
        enc_picture->mv         [LIST_1][block_y][block_x][1] = 0;
        if(img->MbaffFrameFlag)
          rdopt->refar[LIST_1][j][i] = -1;
      }
    }
  }

  if (bframe)
  {
    for (j=0; j<4; j++)
    {
      block_y = img->block_y + j;
      for (i=0; i<4; i++)
      {
        block_x = img->block_x + i;
        k = 2*(j >> 1)+(i >> 1);

        // forward
        if (IS_INTRA(currMB)||(currMB->b8pdir[k] == 0))
        {
          enc_picture->ref_idx    [LIST_1][block_y][block_x]    = -1;
          enc_picture->ref_pic_id [LIST_1][block_y][block_x]    = -1;
          enc_picture->mv         [LIST_1][block_y][block_x][0] = 0;
          enc_picture->mv         [LIST_1][block_y][block_x][1] = 0;
          if(img->MbaffFrameFlag)
            rdopt->refar[LIST_1][j][i] = -1;
        }
        else
        {
          if (currMB->bi_pred_me && (currMB->b8pdir[k] == 2) && currMB->mb_type==1)
          {
            cur_mv = currMB->bi_pred_me == 1
              ? img->bipred_mv1[j][i][LIST_1][0][currMB->b8mode[k]]
              : img->bipred_mv2[j][i][LIST_1][0][currMB->b8mode[k]];

            enc_picture->ref_idx    [LIST_1][block_y][block_x] = 0;
            enc_picture->ref_pic_id [LIST_1][block_y][block_x] = enc_picture->ref_pic_num[LIST_1 + currMB->list_offset][0];
            enc_picture->mv         [LIST_1][block_y][block_x][0] = cur_mv[0];
            enc_picture->mv         [LIST_1][block_y][block_x][1] = cur_mv[1];
            if(img->MbaffFrameFlag)
              rdopt->refar[LIST_1][j][i] = 0;
          }
          else
          {
            cur_mv = img->all_mv[j][i][LIST_1][(short)brefframe[j][i]][currMB->b8mode[k]];

            enc_picture->ref_idx    [LIST_1][block_y][block_x] = brefframe[j][i];
            enc_picture->ref_pic_id [LIST_1][block_y][block_x] = enc_picture->ref_pic_num[LIST_1 + currMB->list_offset][(short)brefframe[j][i]];
            enc_picture->mv         [LIST_1][block_y][block_x][0] = cur_mv[0];
            enc_picture->mv         [LIST_1][block_y][block_x][1] = cur_mv[1];
            if(img->MbaffFrameFlag)
              rdopt->refar[LIST_1][j][i] = brefframe[j][i];
          }
        }
      }
    }
  }

  //==== intra prediction modes ====
  currMB->c_ipred_mode = best_c_imode;
  img->i16offset = best_i16offset;

  if(currMB->mb_type == I8MB)
  {
    memcpy(currMB->intra_pred_modes8x8,b8_intra_pred_modes8x8, MB_BLOCK_PARTITIONS * sizeof(char));
    memcpy(currMB->intra_pred_modes,b8_intra_pred_modes8x8, MB_BLOCK_PARTITIONS * sizeof(char));
    for(j = 0; j < BLOCK_MULTIPLE; j++)
    {
      memcpy(&img->ipredmode[img->block_y+j][img->block_x],b8_ipredmode8x8[j], BLOCK_MULTIPLE * sizeof(char));
      memcpy(&img->ipredmode8x8[img->block_y+j][img->block_x], b8_ipredmode8x8[j], BLOCK_MULTIPLE * sizeof(char));
    }
  }
  else if (mode!=I4MB && mode!=I8MB)
  {
    memset(currMB->intra_pred_modes,DC_PRED, MB_BLOCK_PARTITIONS * sizeof(char));
    for(j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
      memset(&img->ipredmode[j][img->block_x], DC_PRED, BLOCK_MULTIPLE * sizeof(char));
  }
  // Residue Color Transform
  else if (mode == I4MB)
  {
    memcpy(currMB->intra_pred_modes,b4_intra_pred_modes, MB_BLOCK_PARTITIONS * sizeof(char));
    for(j = 0; j < BLOCK_MULTIPLE; j++)
      memcpy(&img->ipredmode[img->block_y + j][img->block_x],&b4_ipredmode[BLOCK_MULTIPLE * j], BLOCK_MULTIPLE * sizeof(char));
  }

  if(img->MbaffFrameFlag)
  {
    rdopt->c_ipred_mode = currMB->c_ipred_mode;
    rdopt->i16offset = img->i16offset;
    memcpy(rdopt->intra_pred_modes,currMB->intra_pred_modes, MB_BLOCK_PARTITIONS * sizeof(char));
    memcpy(rdopt->intra_pred_modes8x8,currMB->intra_pred_modes8x8, MB_BLOCK_PARTITIONS * sizeof(char));
    for(j = img->block_y; j < img->block_y +BLOCK_MULTIPLE; j++)
      memcpy(&rdopt->ipredmode[j][img->block_x],&ipredmodes[j][img->block_x], BLOCK_MULTIPLE * sizeof(char));
  }

  //==== motion vectors =====
  SetMotionVectorsMB (currMB, bframe);
}



/*!
 *************************************************************************************
 * \brief
 *    Set reference frames and motion vectors
 *************************************************************************************
 */
void SetRefAndMotionVectors (int block, int mode, int pdir, int fwref, int bwref)
{
  int     i, j=0;
  int     bslice  = (img->type==B_SLICE);
  int     pmode   = (mode==1||mode==2||mode==3?mode:4);
  int     j0      = ((block >> 1)<<1);
  int     i0      = ((block & 0x01)<<1);
  int     j1      = j0 + (input->part_size[pmode][1]);
  int     i1      = i0 + (input->part_size[pmode][0]);
  int     block_x, block_y;
  short   *cur_mv;
  Macroblock  *currMB  = &img->mb_data[img->current_mb_nr];

  if (pdir<0)
  {
    for (j = img->block_y + j0; j < img->block_y + j1; j++)
    {
      for (i=img->block_x + i0; i<img->block_x +i1; i++)
      {
        enc_picture->ref_pic_id[LIST_0][j][i] = -1;
        enc_picture->ref_pic_id[LIST_1][j][i] = -1;
      }
      memset(&enc_picture->ref_idx[LIST_0][j][img->block_x + i0], -1, (input->part_size[pmode][0]) * sizeof(char));
      memset(&enc_picture->ref_idx[LIST_1][j][img->block_x + i0], -1, (input->part_size[pmode][0]) * sizeof(char));
      memset(enc_picture->mv[LIST_0][j][img->block_x + i0], 0, 2*(input->part_size[pmode][0]) * sizeof(short));
      memset(enc_picture->mv[LIST_1][j][img->block_x + i0], 0, 2*(input->part_size[pmode][0]) * sizeof(short));
    }
    return;
  }

  if (!bslice)
  {
    for (j=j0; j<j1; j++)
    {
      block_y = img->block_y + j;
      memset(&enc_picture->ref_idx   [LIST_0][block_y][img->block_x + i0], fwref, (input->part_size[pmode][0]) * sizeof(char));
      for (i=i0; i<i1; i++)
      {
        block_x = img->block_x + i;
        cur_mv = img->all_mv[j][i][LIST_0][fwref][mode];
        enc_picture->mv        [LIST_0][block_y][block_x][0] = cur_mv[0];
        enc_picture->mv        [LIST_0][block_y][block_x][1] = cur_mv[1];
        enc_picture->ref_pic_id[LIST_0][block_y][block_x] = enc_picture->ref_pic_num[LIST_0+currMB->list_offset][fwref];
      }
    }
    return;
  }
  else
  {
    for (j=j0; j<j1; j++)
    {
      block_y = img->block_y + j;
      for (i=i0; i<i1; i++)
      {
        block_x = img->block_x + i;
        if (mode==0)
        {
          pdir  = direct_pdir[block_y][block_x];
          fwref = direct_ref_idx[LIST_0][block_y][block_x];
          bwref = direct_ref_idx[LIST_1][block_y][block_x];
        }

        if ((pdir==0 || pdir==2))
        {
          if (currMB->bi_pred_me && (pdir == 2) && mode == 1)
          {
            cur_mv = currMB->bi_pred_me == 1
              ? img->bipred_mv1[j][i][LIST_0][0][mode]
              : img->bipred_mv2[j][i][LIST_0][0][mode];

            enc_picture->mv        [LIST_0][block_y][block_x][0] = cur_mv[0];
            enc_picture->mv        [LIST_0][block_y][block_x][1] = cur_mv[1];
            enc_picture->ref_idx   [LIST_0][block_y][block_x]    = 0;
            enc_picture->ref_pic_id[LIST_0][block_y][block_x]    = enc_picture->ref_pic_num[LIST_0+currMB->list_offset][0];
          }
          else
          {
            cur_mv = img->all_mv[j][i][LIST_0][fwref][mode];

            enc_picture->mv        [LIST_0][block_y][block_x][0] = cur_mv[0];
            enc_picture->mv        [LIST_0][block_y][block_x][1] = cur_mv[1];
            enc_picture->ref_idx   [LIST_0][block_y][block_x] = fwref;
            enc_picture->ref_pic_id[LIST_0][block_y][block_x] =
            enc_picture->ref_pic_num[LIST_0+currMB->list_offset][(short)enc_picture->ref_idx[LIST_0][block_y][block_x]];
          }
        }
        else
        {
          enc_picture->mv        [LIST_0][block_y][block_x][0] = 0;
          enc_picture->mv        [LIST_0][block_y][block_x][1] = 0;
          enc_picture->ref_idx   [LIST_0][block_y][block_x]    = -1;
          enc_picture->ref_pic_id[LIST_0][block_y][block_x]    = -1;
        }

        if ((pdir==1 || pdir==2))
        {
          if (currMB->bi_pred_me && (pdir == 2) && mode == 1)
          {
            cur_mv = currMB->bi_pred_me == 1
              ? img->bipred_mv1[j][i][LIST_1][0][mode]
              : img->bipred_mv2[j][i][LIST_1][0][mode];

            enc_picture->mv        [LIST_1][block_y][block_x][0] = cur_mv[0];
            enc_picture->mv        [LIST_1][block_y][block_x][1] = cur_mv[1];
            enc_picture->ref_idx   [LIST_1][block_y][block_x]    = 0;
            enc_picture->ref_pic_id[LIST_1][block_y][block_x]    = enc_picture->ref_pic_num[LIST_1+currMB->list_offset][0];
          }
          else
          {
            cur_mv = img->all_mv[j][i][LIST_1][bwref][mode];

            enc_picture->mv        [LIST_1][block_y][block_x][0] = cur_mv[0];
            enc_picture->mv        [LIST_1][block_y][block_x][1] = cur_mv[1];
            enc_picture->ref_idx   [LIST_1][block_y][block_x] = bwref;
            enc_picture->ref_pic_id[LIST_1][block_y][block_x] =
            enc_picture->ref_pic_num[LIST_1+currMB->list_offset][(short)enc_picture->ref_idx[LIST_1][block_y][block_x]];
          }
        }
        else
        {
          enc_picture->mv        [LIST_1][block_y][block_x][0] = 0;
          enc_picture->mv        [LIST_1][block_y][block_x][1] = 0;
          enc_picture->ref_idx   [LIST_1][block_y][block_x]    = -1;
          enc_picture->ref_pic_id[LIST_1][block_y][block_x]    = -1;
        }
      }
    }
  }
}

/*!
 *************************************************************************************
 * \brief
 *    skip macroblock field inference
 * \return
 *    inferred field flag
 *************************************************************************************
 */
int field_flag_inference()
{
  int mb_field;
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];

  if (currMB->mbAvailA)
  {
    mb_field = img->mb_data[currMB->mbAddrA].mb_field;
  }
  else
  {
    // check top macroblock pair
    if (currMB->mbAvailB)
      mb_field = img->mb_data[currMB->mbAddrB].mb_field;
    else
      mb_field = 0;
  }

  return mb_field;
}

/*!
 *************************************************************************************
 * \brief
 *    Store motion vectors for 8x8 partition
 *************************************************************************************
 */

void StoreMVBlock8x8(int dir, int block8x8, int mode, int ref, int bw_ref, int pdir8, int bframe)
{
  int i, j, i0, j0, ii, jj;
  short ******all_mv  = img->all_mv;
  short ******pred_mv = img->pred_mv;
  short (*lc_l0_mv8x8)[4][2] = all_mv8x8[dir][LIST_0];
  short (*lc_l1_mv8x8)[4][2] = all_mv8x8[dir][LIST_1];
  short (*lc_pr_mv8x8)[4][2] = NULL;

  i0 = (block8x8 & 0x01) << 1;
  j0 = (block8x8 >> 1) << 1;
  ii = i0+2;
  jj = j0+2;

  if (!bframe)
  {
    if (pdir8>=0) //(mode8!=IBLOCK)&&(mode8!=I16MB))  // && ref != -1)
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];
      for (j=j0; j<jj; j++)
        for (i=i0; i<ii; i++)
        {
          lc_l0_mv8x8[j][i][0] = all_mv [j][i][LIST_0][ref][4][0];
          lc_l0_mv8x8[j][i][1] = all_mv [j][i][LIST_0][ref][4][1];
          lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_0][ref][4][0];
          lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_0][ref][4][1];
        }
    }
  }
  else
  {
    if (pdir8 == 0) // list0
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];
      for (j=j0; j<jj; j++)
        for (i=i0; i<ii; i++)
        {
          lc_l0_mv8x8[j][i][0] = all_mv [j][i][LIST_0][ref][mode][0];
          lc_l0_mv8x8[j][i][1] = all_mv [j][i][LIST_0][ref][mode][1];
          lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_0][ref][mode][0];
          lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_0][ref][mode][1];
        }
    }
    else if (pdir8 == 1) // list1
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_1];
      for (j=j0; j<jj; j++)
        for (i=i0; i<ii; i++)
        {
          lc_l1_mv8x8[j][i][0] = all_mv [j][i][LIST_1][bw_ref][mode][0];
          lc_l1_mv8x8[j][i][1] = all_mv [j][i][LIST_1][bw_ref][mode][1];
          lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_1][bw_ref][mode][0];
          lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_1][bw_ref][mode][1];
        }
    }
    else if (pdir8==2) // bipred
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];
      for (j=j0; j<jj; j++)
      {
        for (i=i0; i<ii; i++)
        {
          lc_l0_mv8x8[j][i][0] = all_mv [j][i][LIST_0][ref][mode][0];
          lc_l0_mv8x8[j][i][1] = all_mv [j][i][LIST_0][ref][mode][1];
          lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_0][ref][mode][0];
          lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_0][ref][mode][1];
        }
      }
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_1];
      for (j=j0; j<jj; j++)
      {
        for (i=i0; i<ii; i++)
        {
          lc_l1_mv8x8[j][i][0] = all_mv [j][i][LIST_1][bw_ref][mode][0];
          lc_l1_mv8x8[j][i][1] = all_mv [j][i][LIST_1][bw_ref][mode][1];
          lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_1][bw_ref][mode][0];
          lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_1][bw_ref][mode][1];
        }
      }
    }
    else
    {
      error("invalid direction mode", 255);
    }
  }
}



/*!
 *************************************************************************************
 * \brief
 *    Store motion vectors of 8x8 partitions of one macroblock
 *************************************************************************************
 */
void StoreMV8x8(int dir)
{
  int block8x8;

  int bframe = (img->type == B_SLICE);

  for (block8x8=0; block8x8<4; block8x8++)
    StoreMVBlock8x8(dir, block8x8, tr8x8.part8x8mode[block8x8], tr8x8.part8x8fwref[block8x8],
    tr8x8.part8x8bwref[block8x8], tr8x8.part8x8pdir[block8x8], bframe);
}

/*!
*************************************************************************************
* \brief
*    Restore motion vectors for 8x8 partition
*************************************************************************************
*/
void RestoreMVBlock8x8(int dir, int block8x8, RD_8x8DATA tr, int bframe)
{
  int i, j, i0, j0, ii, jj;
  short ******all_mv  = img->all_mv;
  short ******pred_mv = img->pred_mv;
  short (*lc_l0_mv8x8)[4][2] = all_mv8x8[dir][LIST_0];
  short (*lc_l1_mv8x8)[4][2] = all_mv8x8[dir][LIST_1];
  short (*lc_pr_mv8x8)[4][2] = NULL;

  short pdir8  = tr.part8x8pdir [block8x8];
  short mode   = tr.part8x8mode [block8x8];
  short ref    = tr.part8x8fwref[block8x8];
  short bw_ref = tr.part8x8bwref[block8x8];

  i0 = (block8x8 & 0x01) << 1;
  j0 = (block8x8 >> 1) << 1;
  ii = i0+2;
  jj = j0+2;

  if (!bframe)
  {
    if (pdir8>=0) //(mode8!=IBLOCK)&&(mode8!=I16MB))  // && ref != -1)
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];
      for (j=j0; j<jj; j++)
        for (i=i0; i<ii; i++)
        {
          all_mv [j][i][LIST_0][ref][4][0] = lc_l0_mv8x8[j][i][0] ;
          all_mv [j][i][LIST_0][ref][4][1] = lc_l0_mv8x8[j][i][1] ;
          pred_mv[j][i][LIST_0][ref][4][0] = lc_pr_mv8x8[j][i][0];
          pred_mv[j][i][LIST_0][ref][4][1] = lc_pr_mv8x8[j][i][1];
        }
    }
  }
  else
  {
    if (pdir8==0) // forward
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];
      for (j=j0; j<jj; j++)
      {
        for (i=i0; i<ii; i++)
        {
          all_mv [j][i][LIST_0][ref][mode][0] = lc_l0_mv8x8[j][i][0] ;
          all_mv [j][i][LIST_0][ref][mode][1] = lc_l0_mv8x8[j][i][1] ;
          pred_mv[j][i][LIST_0][ref][mode][0] = lc_pr_mv8x8[j][i][0];
          pred_mv[j][i][LIST_0][ref][mode][1] = lc_pr_mv8x8[j][i][1];
        }
      }
    }
    else if (pdir8==1) // backward
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_1];
      for (j=j0; j<jj; j++)
      {
        for (i=i0; i<ii; i++)
        {
          all_mv [j][i][LIST_1][bw_ref][mode][0] = lc_l1_mv8x8[j][i][0] ;
          all_mv [j][i][LIST_1][bw_ref][mode][1] = lc_l1_mv8x8[j][i][1] ;
          pred_mv[j][i][LIST_1][bw_ref][mode][0] = lc_pr_mv8x8[j][i][0];
          pred_mv[j][i][LIST_1][bw_ref][mode][1] = lc_pr_mv8x8[j][i][1];
        }
      }
    }
    else if (pdir8==2) // bidir
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];
      for (j=j0; j<jj; j++)
      {
        for (i=i0; i<ii; i++)
        {
          all_mv [j][i][LIST_0][ref][mode][0] = lc_l0_mv8x8[j][i][0] ;
          all_mv [j][i][LIST_0][ref][mode][1] = lc_l0_mv8x8[j][i][1] ;
          pred_mv[j][i][LIST_0][ref][mode][0] = lc_pr_mv8x8[j][i][0];
          pred_mv[j][i][LIST_0][ref][mode][1] = lc_pr_mv8x8[j][i][1];
        }
      }
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_1];
      for (j=j0; j<jj; j++)
      {
        for (i=i0; i<ii; i++)
        {
          all_mv [j][i][LIST_1][bw_ref][mode][0] = lc_l1_mv8x8[j][i][0] ;
          all_mv [j][i][LIST_1][bw_ref][mode][1] = lc_l1_mv8x8[j][i][1] ;
          pred_mv[j][i][LIST_1][bw_ref][mode][0] = lc_pr_mv8x8[j][i][0];
          pred_mv[j][i][LIST_1][bw_ref][mode][1] = lc_pr_mv8x8[j][i][1];
        }
      }
    }
    else
    {
      error("invalid direction mode", 255);
    }
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Restore motion vectors of 8x8 partitions of one macroblock
 *************************************************************************************
 */
void RestoreMV8x8(int dir)
{
  int block8x8;

  int bframe = (img->type == B_SLICE);

  for (block8x8=0; block8x8<4; block8x8++)
    RestoreMVBlock8x8(dir, block8x8, tr8x8, bframe);
}


/*!
 *************************************************************************************
 * \brief
 *    Store predictors for 8x8 partition
 *************************************************************************************
 */

void StoreNewMotionVectorsBlock8x8(int dir, int block8x8, int mode, int fw_ref, int bw_ref, int pdir8, int bframe)
{
  int i, j, i0, j0, ii, jj;
  short ******all_mv  = img->all_mv;
  short ******pred_mv = img->pred_mv;
  short (*lc_l0_mv8x8)[4][2] = all_mv8x8[dir][LIST_0];
  short (*lc_l1_mv8x8)[4][2] = all_mv8x8[dir][LIST_1];
  short (*lc_pr_mv8x8)[4][2] = NULL;

  i0 = (block8x8 & 0x01) << 1;
  j0 = (block8x8 >> 1) << 1;
  ii = i0+2;
  jj = j0+2;

  if (pdir8<0)
  {
    for (j=j0; j<jj; j++)
    {
      memset(&lc_l0_mv8x8[j][i0], 0, 4 * sizeof(short));
      memset(&lc_l1_mv8x8[j][i0], 0, 4 * sizeof(short));
    }
    return;
  }

  if (!bframe)
  {

    lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];
    for (j=j0; j<jj; j++)
    {
      for (i=i0; i<ii; i++)
      {
        lc_l0_mv8x8[j][i][0] = all_mv [j][i][LIST_0][fw_ref][4][0];
        lc_l0_mv8x8[j][i][1] = all_mv [j][i][LIST_0][fw_ref][4][1];
        lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_0][fw_ref][4][0];
        lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_0][fw_ref][4][1];
      }
      memset(&lc_l1_mv8x8[j][i0], 0, 4 * sizeof(short));
    }
    return;
  }
  else
  {
    if ((pdir8==0 || pdir8==2))
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_0];

      for (j=j0; j<jj; j++)
        for (i=i0; i<ii; i++)
        {
          lc_l0_mv8x8[j][i][0] = all_mv [j][i][LIST_0][fw_ref][mode][0];
          lc_l0_mv8x8[j][i][1] = all_mv [j][i][LIST_0][fw_ref][mode][1];
          lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_0][fw_ref][mode][0];
          lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_0][fw_ref][mode][1];
        }
    }
    else
    {
      for (j=j0; j<jj; j++)
        memset(&lc_l0_mv8x8[j][i0], 0, 4 * sizeof(short));
    }

    if ((pdir8==1 || pdir8==2))
    {
      lc_pr_mv8x8 = pred_mv8x8[dir][LIST_1];

      for (j=j0; j<jj; j++)
        for (i=i0; i<ii; i++)
        {
          lc_l1_mv8x8[j][i][0] = all_mv [j][i][LIST_1][bw_ref][mode][0];
          lc_l1_mv8x8[j][i][1] = all_mv [j][i][LIST_1][bw_ref][mode][1];
          lc_pr_mv8x8[j][i][0] = pred_mv[j][i][LIST_1][bw_ref][mode][0];
          lc_pr_mv8x8[j][i][1] = pred_mv[j][i][LIST_1][bw_ref][mode][1];
        }
    }
    else
    {
      for (j=j0; j<jj; j++)
        memset(&lc_l1_mv8x8[j][i0], 0, 4 * sizeof(short));
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Makes the decision if 8x8 tranform will be used (for RD-off)
 ************************************************************************
 */
int GetBestTransformP8x8()
{
  int    block_y, block_x, pic_pix_y, pic_pix_x, i, j, k;
  int    mb_y, mb_x, block8x8;
  int    cost8x8=0, cost4x4=0;
  int    *diff_ptr;
  
  if(input->Transform8x8Mode==2) //always allow 8x8 transform
    return 1;

  for (block8x8=0; block8x8<4; block8x8++)
  {
    mb_y = (block8x8 >>   1) << 3;
    mb_x = (block8x8 & 0x01) << 3;
    //===== loop over 4x4 blocks =====
    k=0;
    for (block_y=mb_y; block_y<mb_y+8; block_y+=4)
    {
      pic_pix_y = img->opix_y + block_y;

      //get cost for transform size 4x4
      for (block_x=mb_x; block_x<mb_x+8; block_x+=4)
      {
        pic_pix_x = img->opix_x + block_x;

        //===== get displaced frame difference ======
        diff_ptr=&diff4x4[k];
        for (j=0; j<4; j++)
        {
          for (i=0; i<4; i++, k++)
          {
            //4x4 transform size
            diff4x4[k] = imgY_org[pic_pix_y+j][pic_pix_x+i] - tr4x4.mpr8x8[j+block_y][i+block_x];
            //8x8 transform size
            diff8x8[k] = imgY_org[pic_pix_y+j][pic_pix_x+i] - tr8x8.mpr8x8[j+block_y][i+block_x];
          }
        }

        cost4x4 += distortion4x4 (diff_ptr);
      }
    }
    cost8x8 += distortion8x8 (diff8x8);
  }
  return (cost8x8 < cost4x4);
}

/*!
************************************************************************
* \brief
*    Sets MBAFF RD parameters
************************************************************************
*/
void set_mbaff_parameters()
{
  int  i, j, k;
  Macroblock  *currMB  = &img->mb_data[img->current_mb_nr];
  int         mode     = best_mode;
  int         bframe   = (img->type==B_SLICE);
  signed char    **ipredmodes = img->ipredmode;


  //===== reconstruction values =====
  for (j=0; j < MB_BLOCK_SIZE; j++)
    memcpy(rdopt->rec_mbY[j],&enc_picture->imgY[img->pix_y + j][img->pix_x], MB_BLOCK_SIZE * sizeof(imgpel));

  if (img->yuv_format != YUV400)
  {
    for (j=0; j<img->mb_cr_size_y; j++)
    {
      memcpy(rdopt->rec_mbU[j],&enc_picture->imgUV[0][img->pix_c_y + j][img->pix_c_x], img->mb_cr_size_x * sizeof(imgpel));
      memcpy(rdopt->rec_mbV[j],&enc_picture->imgUV[1][img->pix_c_y + j][img->pix_c_x], img->mb_cr_size_x * sizeof(imgpel));
    }
  }

  //===== coefficients and cbp =====
  rdopt->mode      = mode;
  rdopt->i16offset = img->i16offset;
  rdopt->cbp       = currMB->cbp;
  rdopt->cbp_blk   = currMB->cbp_blk;
  rdopt->mb_type   = currMB->mb_type;

  rdopt->luma_transform_size_8x8_flag = currMB->luma_transform_size_8x8_flag;

  if(rdopt->mb_type == 0 && mode != 0)
  {
    mode=0;
    rdopt->mode=0;
  }

  for(i=0;i<4+img->num_blk8x8_uv;i++)
  {
    for(j=0;j<4;j++)
      for(k=0;k<2;k++)
        memcpy(rdopt->cofAC[i][j][k], img->cofAC[i][j][k], 65 * sizeof(int));
  }

  for(i=0;i<3;i++)
  {
    for(k=0;k<2;k++)
        memcpy(rdopt->cofDC[i][k], img->cofDC[i][k], 18 * sizeof(int));
  }

  memcpy(rdopt->b8mode,currMB->b8mode, BLOCK_MULTIPLE * sizeof(int));
  memcpy(rdopt->b8pdir,currMB->b8pdir, BLOCK_MULTIPLE * sizeof(int));

  //==== reference frames =====
  if (bframe)
  {
    for (j = 0; j < BLOCK_MULTIPLE; j++)
    {
      memcpy(rdopt->refar[LIST_0][j],&enc_picture->ref_idx[LIST_0][img->block_y + j][img->block_x] , BLOCK_MULTIPLE * sizeof(char));
      memcpy(rdopt->refar[LIST_1][j],&enc_picture->ref_idx[LIST_1][img->block_y + j][img->block_x] , BLOCK_MULTIPLE * sizeof(char));
    }
    rdopt->bi_pred_me = currMB->bi_pred_me;
  }
  else
  {
    for (j = 0; j < BLOCK_MULTIPLE; j++)
      memcpy(rdopt->refar[LIST_0][j],&enc_picture->ref_idx[LIST_0][img->block_y + j][img->block_x] , BLOCK_MULTIPLE * sizeof(char));
  }

  memcpy(rdopt->intra_pred_modes,currMB->intra_pred_modes, MB_BLOCK_PARTITIONS * sizeof(char));
  memcpy(rdopt->intra_pred_modes8x8,currMB->intra_pred_modes8x8, MB_BLOCK_PARTITIONS * sizeof(char));
  for (j = img->block_y; j < img->block_y + 4; j++)
  {
    memcpy(&rdopt->ipredmode[j][img->block_x],&ipredmodes[j][img->block_x], BLOCK_MULTIPLE * sizeof(char));
  }
}

/*!
************************************************************************
* \brief
*    store coding state (for rd-optimized mode decision), used for 8x8 transformation
************************************************************************
*/
void store_coding_state_cs_cm()
{
  store_coding_state(cs_cm);
}

/*!
************************************************************************
* \brief
*    restore coding state (for rd-optimized mode decision), used for 8x8 transformation
************************************************************************
*/
void reset_coding_state_cs_cm()
{
  reset_coding_state(cs_cm);
}

/*!
************************************************************************
* \brief
*    update rounding offsets based on JVT-N011
************************************************************************
*/
void update_offset_params(int mode, int luma_transform_size_8x8_flag)
{
  int is_inter = (mode != I4MB)&&(mode != I16MB) && (mode != I8MB);
  int luma_pos = AdaptRndPos[(is_inter<<1) + luma_transform_size_8x8_flag][img->type];
  int i,j;
  int temp = 0;
  int offsetRange = 1 << (OffsetBits - 1);
  int blk_mask = 0x03 + (luma_transform_size_8x8_flag<<2);
  int blk_shift = 2 + luma_transform_size_8x8_flag;
  short **offsetList = luma_transform_size_8x8_flag ? OffsetList8x8 : OffsetList4x4;

  int **fAdjust = is_inter
    ? (luma_transform_size_8x8_flag ? bestInterFAdjust8x8 : bestInterFAdjust4x4)
    : (luma_transform_size_8x8_flag ? bestIntraFAdjust8x8 : bestIntraFAdjust4x4);

  for (j=0; j < MB_BLOCK_SIZE; j++)
  {
    int j_pos = ((j & blk_mask)<<blk_shift);
    for (i=0; i < MB_BLOCK_SIZE; i++)
    {
      temp = j_pos + (i & blk_mask);
      offsetList[luma_pos][temp] += fAdjust[j][i];
      offsetList[luma_pos][temp] = iClip3(0,offsetRange,offsetList[luma_pos][temp]);
    }
  }

  if (input->AdaptRndChroma)
  {
    int u_pos = AdaptRndCrPos[is_inter][img->type];
    int v_pos = u_pos + 1;
    int jpos;

    int ***fAdjustCr = is_inter ? bestInterFAdjust4x4Cr : bestIntraFAdjust4x4Cr;

    for (j=0; j < img->mb_cr_size_y; j++)
    {
      jpos = ((j & 0x03)<<2);
      for (i=0; i < img->mb_cr_size_x; i++)
      {
        temp = jpos + (i & 0x03);
        OffsetList4x4[u_pos][temp] += fAdjustCr[0][j][i];
        OffsetList4x4[u_pos][temp] = iClip3(0,offsetRange,OffsetList4x4[u_pos][temp]);
        OffsetList4x4[v_pos][temp] += fAdjustCr[1][j][i];
        OffsetList4x4[v_pos][temp] = iClip3(0,offsetRange,OffsetList4x4[v_pos][temp]);
      }
    }
  }
}

void assign_enc_picture_params(int mode, signed char best_pdir, int block, int list_offset, int best_fw_ref, int best_bw_ref, int bframe)
{
  int i,j;
  int block_x, block_y;
  short *cur_mv;

  if (mode==1)
  {
    if (best_pdir==1)
    {
      for (j=img->block_y+(block&2); j<img->block_y+(block&2) + BLOCK_MULTIPLE; j++)
      {
        block_x = img->block_x+(block&1)*2;

        memset(&enc_picture->ref_idx[LIST_0][j][block_x], -1 ,     BLOCK_MULTIPLE * sizeof(char));
        memset(enc_picture->mv      [LIST_0][j][block_x],  0 , 2 * BLOCK_MULTIPLE * sizeof(short));
        for (i=block_x; i<block_x + BLOCK_MULTIPLE; i++)
        {
          enc_picture->ref_pic_id [LIST_0][j][i]    = -1;
        }
      }
    }
    else if (img->bi_pred_me[mode])
    {
      for (j=0; j<BLOCK_MULTIPLE; j++)
      {
        block_y = img->block_y+(block&2)+j;
        block_x = img->block_x+(block&1)*2;
        memset(&enc_picture->ref_idx[LIST_0][block_y][block_x], 0, BLOCK_MULTIPLE * sizeof(char));
        for (i=0; i<BLOCK_MULTIPLE; i++)
        {
          cur_mv = img->bi_pred_me[mode] == 1
            ? img->bipred_mv1[i][j][LIST_0][0][mode]
            : img->bipred_mv2[i][j][LIST_0][0][mode];

          enc_picture->ref_pic_id [LIST_0][block_y][block_x + i]    = enc_picture->ref_pic_num[LIST_0 + list_offset][0];
          enc_picture->mv         [LIST_0][block_y][block_x + i][0] = cur_mv[0];
          enc_picture->mv         [LIST_0][block_y][block_x + i][1] = cur_mv[1];
        }
      }
    }
    else
    {
      for (j=0; j<BLOCK_MULTIPLE; j++)
      {
        block_y = img->block_y+(block&2)+j;
        block_x = img->block_x+(block&1)*2;
        memset(&enc_picture->ref_idx[LIST_0][block_y][block_x], best_fw_ref , BLOCK_MULTIPLE * sizeof(char));
        for (i=0; i<BLOCK_MULTIPLE; i++)
        {
          cur_mv = img->all_mv[j][i][LIST_0][best_fw_ref][mode];

          enc_picture->ref_pic_id [LIST_0][block_y][block_x + i]    = enc_picture->ref_pic_num[LIST_0 + list_offset][best_fw_ref];
          enc_picture->mv         [LIST_0][block_y][block_x + i][0] = cur_mv[0];
          enc_picture->mv         [LIST_0][block_y][block_x + i][1] = cur_mv[1];
        }
      }
    }

    if (bframe)
    {
      if (best_pdir==0)
      {
        for (j=img->block_y+(block&2); j<img->block_y+(block&2) + BLOCK_MULTIPLE; j++)
        {
          block_x = img->block_x+(block&1)*2;
          memset(&enc_picture->ref_idx[LIST_1][j][block_x], -1 , BLOCK_MULTIPLE * sizeof(char));
          memset(enc_picture->mv[LIST_1][j][block_x], 0 , 2 * BLOCK_MULTIPLE * sizeof(short));
          for (i=block_x; i<block_x + BLOCK_MULTIPLE; i++)
          {
            enc_picture->ref_pic_id [LIST_1][j][i] = -1;
          }
        }
      }
      else
      {
        if (img->bi_pred_me[mode])
        {
          for (j=0; j<BLOCK_MULTIPLE; j++)
          {
            block_y = img->block_y+(block&2)+j;
            block_x = img->block_x+(block&1)*2;
            memset(&enc_picture->ref_idx[LIST_1][block_y][block_x], 0, BLOCK_MULTIPLE * sizeof(char));
            for (i=0; i<BLOCK_MULTIPLE; i++)
            {
              cur_mv = img->bi_pred_me[mode] == 1
                ? img->bipred_mv1[i][j][LIST_1][0][mode]
                : img->bipred_mv2[i][j][LIST_1][0][mode];

              enc_picture->ref_pic_id [LIST_1][block_y][block_x + i] =
                enc_picture->ref_pic_num[LIST_1 + list_offset][0];
              enc_picture->mv         [LIST_1][block_y][block_x + i][0] = cur_mv[0];
              enc_picture->mv         [LIST_1][block_y][block_x + i][1] = cur_mv[1];
            }
          }
        }
        else
        {
          for (j=0; j<BLOCK_MULTIPLE; j++)
          {
            block_y = img->block_y+(block&2)+j;
            block_x = img->block_x+(block&1)*2;
            memset(&enc_picture->ref_idx[LIST_1][block_y][block_x], best_bw_ref, BLOCK_MULTIPLE * sizeof(char));
            for (i=0; i<BLOCK_MULTIPLE; i++)
            {

              enc_picture->ref_pic_id [LIST_1][block_y][block_x + i] =
                enc_picture->ref_pic_num[LIST_1 + list_offset][best_bw_ref];
              if(best_bw_ref>=0)
              {
                cur_mv = img->all_mv[j][i][LIST_1][best_bw_ref][mode];
                enc_picture->mv[LIST_1][block_y][block_x + i][0] = cur_mv[0];
                enc_picture->mv[LIST_1][block_y][block_x + i][1] = cur_mv[1];
              }
            }
          }
        }
      }
    }
  }
  else if (mode==2)
  {
    for (j=0; j<2; j++)
    {
      block_y = img->block_y + block * 2 + j;
      for (i=0; i<BLOCK_MULTIPLE; i++)
      {
        block_x = img->block_x + i;
        if (best_pdir==1)
        {
          enc_picture->ref_idx    [LIST_0][block_y][block_x]    = -1;
          enc_picture->ref_pic_id [LIST_0][block_y][block_x]    = -1;
          enc_picture->mv         [LIST_0][block_y][block_x][0] = 0;
          enc_picture->mv         [LIST_0][block_y][block_x][1] = 0;
        }
        else
        {
          cur_mv = img->all_mv[j+block*2][i][LIST_0][best_fw_ref][mode];

          enc_picture->ref_idx    [LIST_0][block_y][block_x]    = best_fw_ref;
          enc_picture->ref_pic_id [LIST_0][block_y][block_x]    =
            enc_picture->ref_pic_num[LIST_0 + list_offset][best_fw_ref];
          enc_picture->mv         [LIST_0][block_y][block_x][0] = cur_mv[0];
          enc_picture->mv         [LIST_0][block_y][block_x][1] = cur_mv[1];
        }

        if (bframe)
        {
          if (best_pdir==0)
          {
            enc_picture->ref_idx    [LIST_1][block_y][block_x]    = -1;
            enc_picture->ref_pic_id [LIST_1][block_y][block_x]    = -1;
            enc_picture->mv         [LIST_1][block_y][block_x][0] = 0;
            enc_picture->mv         [LIST_1][block_y][block_x][1] = 0;
          }
          else
          {
            enc_picture->ref_idx[LIST_1][block_y][block_x] = best_bw_ref;
            if(best_bw_ref>=0)
            {
              cur_mv = img->all_mv[j+ block*2][i][LIST_1][best_bw_ref][mode];

              enc_picture->ref_pic_id [LIST_1][block_y][block_x] =
                enc_picture->ref_pic_num[LIST_1 + list_offset][best_bw_ref];
              enc_picture->mv[LIST_1][block_y][block_x][0] = cur_mv[0];
              enc_picture->mv[LIST_1][block_y][block_x][1] = cur_mv[1];
            }
          }
        }
      }
    }
  }
  else
  {
    for (j=0; j<BLOCK_MULTIPLE; j++)
    {
      block_y = img->block_y+j;
      for (i=0; i<2; i++)
      {
        block_x = img->block_x + block*2 + i;
        if (best_pdir==1)
        {
          enc_picture->ref_idx    [LIST_0][block_y][block_x]    = -1;
          enc_picture->ref_pic_id [LIST_0][block_y][block_x]    = -1;
          enc_picture->mv         [LIST_0][block_y][block_x][0] = 0;
          enc_picture->mv         [LIST_0][block_y][block_x][1] = 0;
        }
        else
        {
          cur_mv = img->all_mv[j][block*2+i][LIST_0][best_fw_ref][mode];

          enc_picture->ref_idx    [LIST_0][block_y][block_x] = best_fw_ref;
          enc_picture->ref_pic_id [LIST_0][block_y][block_x] =
            enc_picture->ref_pic_num[LIST_0 + list_offset][best_fw_ref];
          enc_picture->mv[LIST_0][block_y][block_x][0] = cur_mv[0];
          enc_picture->mv[LIST_0][block_y][block_x][1] = cur_mv[1];
        }

        if (bframe)
        {
          if (best_pdir==0)
          {
            enc_picture->ref_idx    [LIST_1][block_y][block_x]    = -1;
            enc_picture->ref_pic_id [LIST_1][block_y][block_x]    = -1;
            enc_picture->mv         [LIST_1][block_y][block_x][0] = 0;
            enc_picture->mv         [LIST_1][block_y][block_x][1] = 0;
          }
          else
          {
            enc_picture->ref_idx[LIST_1][block_y][block_x] = best_bw_ref;
            if(best_bw_ref>=0)
            {
              cur_mv = img->all_mv[j][block*2+i][LIST_1][best_bw_ref][mode];
              enc_picture->ref_pic_id [LIST_1][block_y][block_x] =
                enc_picture->ref_pic_num[LIST_1 + list_offset][best_bw_ref];

              enc_picture->mv[LIST_1][block_y][block_x][0] = cur_mv[0];
              enc_picture->mv[LIST_1][block_y][block_x][1] = cur_mv[1];
            }
          }
        }
      }
    }
  }
}

void update_refresh_map(int intra, int intra1, Macroblock *currMB)
{
  if (input->RestrictRef==1)
  {
    // Modified for Fast Mode Decision. Inchoon Choi, SungKyunKwan Univ.
    if (input->rdopt<2)
    {
      refresh_map[2*img->mb_y  ][2*img->mb_x  ] = (intra ? 1 : 0);
      refresh_map[2*img->mb_y  ][2*img->mb_x+1] = (intra ? 1 : 0);
      refresh_map[2*img->mb_y+1][2*img->mb_x  ] = (intra ? 1 : 0);
      refresh_map[2*img->mb_y+1][2*img->mb_x+1] = (intra ? 1 : 0);
    }
    else if (input->rdopt==3)
    {
      refresh_map[2*img->mb_y  ][2*img->mb_x  ] = (intra1==0 && (currMB->mb_type==I16MB || currMB->mb_type==I4MB) ? 1 : 0);
      refresh_map[2*img->mb_y  ][2*img->mb_x+1] = (intra1==0 && (currMB->mb_type==I16MB || currMB->mb_type==I4MB) ? 1 : 0);
      refresh_map[2*img->mb_y+1][2*img->mb_x  ] = (intra1==0 && (currMB->mb_type==I16MB || currMB->mb_type==I4MB) ? 1 : 0);
      refresh_map[2*img->mb_y+1][2*img->mb_x+1] = (intra1==0 && (currMB->mb_type==I16MB || currMB->mb_type==I4MB) ? 1 : 0);
    }
  }
  else if (input->RestrictRef==2)
  {
    refresh_map[2*img->mb_y  ][2*img->mb_x  ] = (currMB->mb_type==I16MB || currMB->mb_type==I4MB ? 1 : 0);
    refresh_map[2*img->mb_y  ][2*img->mb_x+1] = (currMB->mb_type==I16MB || currMB->mb_type==I4MB ? 1 : 0);
    refresh_map[2*img->mb_y+1][2*img->mb_x  ] = (currMB->mb_type==I16MB || currMB->mb_type==I4MB ? 1 : 0);
    refresh_map[2*img->mb_y+1][2*img->mb_x+1] = (currMB->mb_type==I16MB || currMB->mb_type==I4MB ? 1 : 0);
  }
}

