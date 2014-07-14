
/*!
 ***************************************************************************
 * \file mode_decision.c
 *
 * \brief
 *    Main macroblock mode decision functions and helpers
 *
 **************************************************************************
 */

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <memory.h>
#include <string.h>

#include "global.h"
#include "rdopt_coding_state.h"
#include "mb_access.h"
#include "intrarefresh.h"
#include "image.h"
#include "transform8x8.h"
#include "ratectl.h"
#include "mode_decision.h"
#include "fmo.h"
#include "me_umhex.h"
#include "me_umhexsmp.h"
#include "macroblock.h"


//==== MODULE PARAMETERS ====
imgpel temp_imgY[16][16]; // to temp store the Y data for 8x8 transform

const int  b8_mode_table[6]  = {0, 4, 5, 6, 7};         // DO NOT CHANGE ORDER !!!
const int  mb_mode_table[9]  = {0, 1, 2, 3, P8x8, I16MB, I4MB, I8MB, IPCM}; // DO NOT CHANGE ORDER !!!

double *mb16x16_cost_frame;

/*!
*************************************************************************************
* \brief
*    Update Rate Control Difference
*************************************************************************************
*/
void rc_store_diff(int cpix_x, int cpix_y, imgpel prediction[16][16])
{
  int i, j;
  int *iDst;
  imgpel *Src1, *Src2;

  for(j=0; j<MB_BLOCK_SIZE; j++)
  {
    iDst = diffy[j];
    Src1 = imgY_org[cpix_y + j];
    Src2 = prediction[j];
    for (i=0; i<MB_BLOCK_SIZE; i++)
    {
      iDst[i] = Src1[cpix_x + i] - Src2[i];
    }
  }
}


/*!
*************************************************************************************
* \brief
*    Fast intra decision
*************************************************************************************
*/
void fast_mode_intra_decision(short *intra_skip, double min_rate)
{
  int i;
  int mb_available_up, mb_available_left, mb_available_up_left;
  long SBE;
  double AR = 0, ABE = 0;
  PixelPos up;       //!< pixel position p(0,-1)
  PixelPos left[2];  //!< pixel positions p(-1, -1..0)

  for (i=0;i<2;i++)
  {
    getNeighbour(img->current_mb_nr, -1 ,  i-1 , IS_LUMA, &left[i]);
  }
  getNeighbour(img->current_mb_nr, 0     ,  -1 , IS_LUMA, &up);

  mb_available_up       = up.available;
  mb_available_up_left  = left[0].available;
  mb_available_left     = left[1].available;

  AR=(1.0/384)*min_rate;

  SBE = 0;

  if( (img->mb_y != (int)img->FrameHeightInMbs-1) && (img->mb_x != (int)img->PicWidthInMbs-1) && mb_available_left && mb_available_up)
  {
    for(i = 0; i < MB_BLOCK_SIZE; i++)
    {
      SBE += iabs(imgY_org[img->opix_y][img->opix_x+i] - enc_picture->imgY[img->pix_y-1][img->pix_x+i]);
      SBE += iabs(imgY_org[img->opix_y+i][img->opix_x] - enc_picture->imgY[img->pix_y+i][img->pix_x-1]);
    }
    for(i = 0; i < 8; i++)
    {
      SBE += iabs(imgUV_org[0][img->opix_c_y][img->opix_c_x+i] - enc_picture->imgUV[0][img->pix_c_y-1][img->pix_c_x+i]);
      SBE += iabs(imgUV_org[0][img->opix_c_y+i][img->opix_c_x] - enc_picture->imgUV[0][img->pix_c_y+i][img->pix_c_x-1]);
      SBE += iabs(imgUV_org[1][img->opix_c_y][img->opix_c_x+i] - enc_picture->imgUV[1][img->pix_c_y-1][img->pix_c_x+i]);
      SBE += iabs(imgUV_org[1][img->opix_c_y+i][img->opix_c_x] - enc_picture->imgUV[1][img->pix_c_y+i][img->pix_c_x-1]);
    }
    ABE = 1.0/64 * SBE;
  }
  else  // Image boundary
  {
    ABE = 0;
  }

  if(AR <= ABE)
  {
    *intra_skip = 1;
  }
}

/*!
*************************************************************************************
* \brief
*    Initialize Encoding parameters for Macroblock
*************************************************************************************
*/
void init_enc_mb_params(Macroblock* currMB, RD_PARAMS *enc_mb, int intra, int bslice)
{
  int mode;
  int l,k;

  //Setup list offset
  enc_mb->list_offset[LIST_0] = LIST_0 + currMB->list_offset;
  enc_mb->list_offset[LIST_1] = LIST_1 + currMB->list_offset;

  enc_mb->curr_mb_field = ((img->MbaffFrameFlag)&&(currMB->mb_field));
  enc_mb->best_ref[LIST_0] = 0;
  enc_mb->best_ref[LIST_1] = -1;

  // Set valid modes
  enc_mb->valid[I8MB]  = input->Transform8x8Mode;
  enc_mb->valid[I4MB]  = (input->Transform8x8Mode==2) ? 0:1;
  enc_mb->valid[I16MB] = 1;
  enc_mb->valid[IPCM]  = input->EnableIPCM;

  enc_mb->valid[0]     = (!intra );
  enc_mb->valid[1]     = (!intra && input->InterSearch16x16);
  enc_mb->valid[2]     = (!intra && input->InterSearch16x8);
  enc_mb->valid[3]     = (!intra && input->InterSearch8x16);
  enc_mb->valid[4]     = (!intra && input->InterSearch8x8);
  enc_mb->valid[5]     = (!intra && input->InterSearch8x4 && !(input->Transform8x8Mode==2));
  enc_mb->valid[6]     = (!intra && input->InterSearch4x8 && !(input->Transform8x8Mode==2));
  enc_mb->valid[7]     = (!intra && input->InterSearch4x4 && !(input->Transform8x8Mode==2));
  enc_mb->valid[P8x8]  = (enc_mb->valid[4] || enc_mb->valid[5] || enc_mb->valid[6] || enc_mb->valid[7]);
  enc_mb->valid[12]    = (img->type == SI_SLICE);

  if(img->type==SP_SLICE)
  {
    if(si_frame_indicator)
    {
      enc_mb->valid[I8MB]  = 0;
      enc_mb->valid[IPCM]  = 0;
      enc_mb->valid[0]     = 0;
      enc_mb->valid[1]     = 0;
      enc_mb->valid[2]     = 0;
      enc_mb->valid[3]     = 0;
      enc_mb->valid[4]     = 0;
      enc_mb->valid[5]     = 0;
      enc_mb->valid[6]     = 0;
      enc_mb->valid[7]     = 0;
      enc_mb->valid[P8x8]  = 0;
      enc_mb->valid[12]    = 0;
      if(check_for_SI16())
      {
        enc_mb->valid[I4MB]  = 0;
        enc_mb->valid[I16MB] = 1;
      }
      else
      {
        enc_mb->valid[I4MB]  = 1;
        enc_mb->valid[I16MB] = 0;
      }
    }
  }

  if(img->type==SP_SLICE)
  {
    if(sp2_frame_indicator)
    {
      if(check_for_SI16())
      {
        enc_mb->valid[I8MB]  = 0;
        enc_mb->valid[IPCM]  = 0;
        enc_mb->valid[0]     = 0;
        enc_mb->valid[1]     = 0;
        enc_mb->valid[2]     = 0;
        enc_mb->valid[3]     = 0;
        enc_mb->valid[4]     = 0;
        enc_mb->valid[5]     = 0;
        enc_mb->valid[6]     = 0;
        enc_mb->valid[7]     = 0;
        enc_mb->valid[P8x8]  = 0;
        enc_mb->valid[12]    = 0;
        enc_mb->valid[I4MB]  = 0;
        enc_mb->valid[I16MB] = 1;
      }
      else
      {
        enc_mb->valid[I8MB]  = 0;
        enc_mb->valid[IPCM]  = 0;
        enc_mb->valid[0]     = 0;
        enc_mb->valid[I16MB] = 0;
      }
    }
  }

  //===== SET LAGRANGE PARAMETERS =====
  // Note that these are now computed at the slice level to reduce
  // computations and cleanup code.
  if (bslice && img->nal_reference_idc)
  {
    enc_mb->lambda_md = img->lambda_md[5][img->qp];

    enc_mb->lambda_me[F_PEL] = img->lambda_me[5][img->qp][F_PEL];
    enc_mb->lambda_me[H_PEL] = img->lambda_me[5][img->qp][H_PEL];
    enc_mb->lambda_me[Q_PEL] = img->lambda_mf[5][img->qp][Q_PEL];

    enc_mb->lambda_mf[F_PEL] = img->lambda_mf[5][img->qp][F_PEL];
    enc_mb->lambda_mf[H_PEL] = img->lambda_mf[5][img->qp][H_PEL];
    enc_mb->lambda_mf[Q_PEL] = img->lambda_mf[5][img->qp][Q_PEL];

  }
  else
  {
    enc_mb->lambda_md = img->lambda_md[img->type][img->qp];

    enc_mb->lambda_me[F_PEL] = img->lambda_me[img->type][img->qp][F_PEL];
    enc_mb->lambda_me[H_PEL] = img->lambda_me[img->type][img->qp][H_PEL];
    enc_mb->lambda_me[Q_PEL] = img->lambda_me[img->type][img->qp][Q_PEL];

    enc_mb->lambda_mf[F_PEL] = img->lambda_mf[img->type][img->qp][F_PEL];
    enc_mb->lambda_mf[H_PEL] = img->lambda_mf[img->type][img->qp][H_PEL];
    enc_mb->lambda_mf[Q_PEL] = img->lambda_mf[img->type][img->qp][Q_PEL];
  }

  // Initialize bipredME decisions
  for (mode=0; mode<MAXMODE; mode++)
  {
    img->bi_pred_me[mode]=0;
  }

  if (!img->MbaffFrameFlag)
  {
    for (l = LIST_0; l < BI_PRED; l++)
    {
      for(k = 0; k < listXsize[l]; k++)
      {
        listX[l][k]->chroma_vector_adjustment= 0;
        if(img->structure == TOP_FIELD && img->structure != listX[l][k]->structure)
          listX[l][k]->chroma_vector_adjustment = -2;
        if(img->structure == BOTTOM_FIELD && img->structure != listX[l][k]->structure)
          listX[l][k]->chroma_vector_adjustment = 2;
      }
    }
  }
  else
  {
    if (enc_mb->curr_mb_field)
    {
      for (l = enc_mb->list_offset[LIST_0]; l <= enc_mb->list_offset[LIST_1]; l++)
      {
        for(k = 0; k < listXsize[l]; k++)
        {
          listX[l][k]->chroma_vector_adjustment= 0;
          if(img->current_mb_nr % 2 == 0 && listX[l][k]->structure == BOTTOM_FIELD)
            listX[l][k]->chroma_vector_adjustment = -2;
          if(img->current_mb_nr % 2 == 1 && listX[l][k]->structure == TOP_FIELD)
            listX[l][k]->chroma_vector_adjustment = 2;
        }
      }
    }
    else
    {
      for (l = enc_mb->list_offset[LIST_0]; l <= enc_mb->list_offset[LIST_1]; l++)
      {
        for(k = 0; k < listXsize[l]; k++)
          listX[l][k]->chroma_vector_adjustment= 0;
      }
    }
  }
}

/*!
*************************************************************************************
* \brief
*    computation of prediction list (including biprediction) cost
*************************************************************************************
*/
void list_prediction_cost(int list, int block, int mode, RD_PARAMS enc_mb, int bmcost[5], signed char best_ref[2])
{
  short ref;
  int mcost;
  int cur_list = list < BI_PRED ? enc_mb.list_offset[list] : enc_mb.list_offset[LIST_0];

  //--- get cost and reference frame for forward prediction ---

  if (list < BI_PRED)
  {
    for (ref=0; ref < listXsize[cur_list]; ref++)
    {
      if (!img->checkref || list || ref==0 || (input->RestrictRef && CheckReliabilityOfRef (block, list, ref, mode)))
      {
        // limit the number of reference frames to 1 when switching SP frames are used
        if((!input->sp2_frame_indicator && !input->sp_output_indicator)||
          ((input->sp2_frame_indicator || input->sp_output_indicator) && (img->type!=P_SLICE && img->type!=SP_SLICE))||
          ((input->sp2_frame_indicator || input->sp_output_indicator) && ((img->type==P_SLICE || img->type==SP_SLICE) &&(ref==0))))
        {
          mcost  = (input->rdopt
            ? REF_COST (enc_mb.lambda_mf[Q_PEL], ref, cur_list)
            : (int) (2 * enc_mb.lambda_me[Q_PEL] * imin(ref, 1)));

          mcost += motion_cost[mode][list][ref][block];
          if (mcost < bmcost[list])
          {
            bmcost[list]   = mcost;
            best_ref[list] = (signed char)ref;
          }
        }
      }
    }
  }
  else if (list == BI_PRED)
  {
    if (active_pps->weighted_bipred_idc == 1)
    {
      int weight_sum = wbp_weight[0][(int) best_ref[LIST_0]][(int) best_ref[LIST_1]][0] + wbp_weight[1][(int) best_ref[LIST_0]][(int) best_ref[LIST_1]][0];
      if (weight_sum < -128 ||  weight_sum > 127)
      {
        bmcost[list] = INT_MAX;
      }
      else
      {
        bmcost[list]  = (input->rdopt
          ? (REF_COST  (enc_mb.lambda_mf[Q_PEL], (short)best_ref[LIST_0], cur_list)
          +  REF_COST  (enc_mb.lambda_mf[Q_PEL], (short)best_ref[LIST_1], cur_list + LIST_1))
          : (int) (2 * (enc_mb.lambda_me[Q_PEL] * (imin((short)best_ref[LIST_0], 1) + imin((short)best_ref[LIST_1], 1)))));
        bmcost[list] += BIDPartitionCost (mode, block, (short)best_ref[LIST_0], (short)best_ref[LIST_1], enc_mb.lambda_mf[Q_PEL]);
      }
    }
    else
    {
      bmcost[list]  = (input->rdopt
        ? (REF_COST  (enc_mb.lambda_mf[Q_PEL], (short)best_ref[LIST_0], cur_list)
        +  REF_COST  (enc_mb.lambda_mf[Q_PEL], (short)best_ref[LIST_1], cur_list + LIST_1))
        : (int) (2 * (enc_mb.lambda_me[Q_PEL] * (imin((short)best_ref[LIST_0], 1) + imin((short)best_ref[LIST_1], 1)))));
      bmcost[list] += BIDPartitionCost (mode, block, (short)best_ref[LIST_0], (short)best_ref[LIST_1], enc_mb.lambda_mf[Q_PEL]);
    }
  }
  else
  {
    bmcost[list]  = (input->rdopt
      ? (REF_COST (enc_mb.lambda_mf[Q_PEL], 0, cur_list)
      +  REF_COST (enc_mb.lambda_mf[Q_PEL], 0, cur_list + LIST_1))
      : (int) (4 * enc_mb.lambda_me[Q_PEL]));
    bmcost[list] += BPredPartitionCost(mode, block, 0, 0, enc_mb.lambda_mf[Q_PEL], !(list&1));
  }
}

int compute_ref_cost(RD_PARAMS enc_mb, int ref, int list)
{
  return WEIGHTED_COST(enc_mb.lambda_mf[Q_PEL],((listXsize[enc_mb.list_offset[list]] <= 1)? 0:refbits[ref]));
}

/*!
*************************************************************************************
* \brief
*    Determination of prediction list based on simple distortion computation
*************************************************************************************
*/
void determine_prediction_list(int mode, int bmcost[5], signed char best_ref[2], signed char *best_pdir, int *cost, short *bi_pred_me)
{
  if ((!input->BiPredMotionEstimation) || (mode != 1))
  {
    //--- get prediction direction ----
    if  (bmcost[LIST_0] <= bmcost[LIST_1]
      && bmcost[LIST_0] <= bmcost[BI_PRED])
    {
      *best_pdir = 0;
      *cost += bmcost[LIST_0];
    }
    else if (bmcost[LIST_1] <= bmcost[LIST_0]
      &&     bmcost[LIST_1] <= bmcost[BI_PRED])
    {
      *best_pdir = 1;
      *cost += bmcost[LIST_1];
    }
    else
    {
      *best_pdir = 2;
      *cost += bmcost[BI_PRED];
    }
  }
  else
  {
    img->bi_pred_me[mode]=0;
    *bi_pred_me = 0;
    //--- get prediction direction ----
    if  (bmcost[LIST_0] <= bmcost[LIST_1]
      && bmcost[LIST_0] <= bmcost[BI_PRED]
      && bmcost[LIST_0] <= bmcost[BI_PRED_L0]
      && bmcost[LIST_0] <= bmcost[BI_PRED_L1])
    {
      *best_pdir = 0;
      *cost += bmcost[LIST_0];
      //best_ref[LIST_1] = 0;
    }
    else if (bmcost[LIST_1] <= bmcost[LIST_0]
      &&     bmcost[LIST_1] <= bmcost[BI_PRED]
      &&     bmcost[LIST_1] <= bmcost[BI_PRED_L0]
      &&     bmcost[LIST_1] <= bmcost[BI_PRED_L1])
    {
      *best_pdir = 1;
      *cost += bmcost[LIST_1];
      //best_ref[LIST_0] = 0;
    }
    else if (bmcost[BI_PRED] <= bmcost[LIST_0]
      &&     bmcost[BI_PRED] <= bmcost[LIST_1]
      &&     bmcost[BI_PRED] <= bmcost[BI_PRED_L0]
      &&     bmcost[BI_PRED] <= bmcost[BI_PRED_L1])
    {
      *best_pdir = 2;
      *cost += bmcost[BI_PRED];
      //best_ref[LIST_1] = 0;
    }
    else if (bmcost[BI_PRED_L0] <= bmcost[LIST_0]
      &&     bmcost[BI_PRED_L0] <= bmcost[LIST_1]
      &&     bmcost[BI_PRED_L0] <= bmcost[BI_PRED]
      &&     bmcost[BI_PRED_L0] <= bmcost[BI_PRED_L1])
    {
      *best_pdir = 2;
      *cost += bmcost[BI_PRED_L0];
      *bi_pred_me = 1;
      img->bi_pred_me[mode]=1;
      best_ref[LIST_1] = 0;
      best_ref[LIST_0] = 0;
    }
    else
    {
      *best_pdir = 2;
      *cost += bmcost[BI_PRED_L1];
      *bi_pred_me = 2;
      best_ref[LIST_1] = 0;
      best_ref[LIST_0] = 0;
      img->bi_pred_me[mode]=2;
    }
  }
}

/*!
*************************************************************************************
* \brief
*    RD decision process
*************************************************************************************
*/
void compute_mode_RD_cost(int mode,
                          Macroblock *currMB,
                          RD_PARAMS enc_mb,
                          double *min_rdcost,
                          double *min_rate,
                          int i16mode,
                          short bslice,
                          short *inter_skip)
{
  //--- transform size ---
  currMB->luma_transform_size_8x8_flag = input->Transform8x8Mode==2
    ?  (mode >= 1 && mode <= 3)
    || (mode == 0 && bslice && active_sps->direct_8x8_inference_flag)
    || ((mode == P8x8) && (enc_mb.valid[4]))
    :  0;
  //store_coding_state (cs_cm); // RD
  SetModesAndRefframeForBlocks (mode);

  // Encode with coefficients
  img->NoResidueDirect = 0;

  if ((input->FastCrIntraDecision )
    || (currMB->c_ipred_mode == DC_PRED_8 || (IS_INTRA(currMB) )))
  {
    while(1)
    {
      if (RDCost_for_macroblocks (enc_mb.lambda_md, mode, min_rdcost, min_rate, i16mode))
      {
        //Rate control
        if (input->RCEnable)
        {
          if(mode == P8x8)
            rc_store_diff(img->opix_x,img->opix_y,
            currMB->luma_transform_size_8x8_flag == 1 ? tr8x8.mpr8x8 : tr4x4.mpr8x8);
          else
            rc_store_diff(img->opix_x, img->opix_y, pred);
        }
        store_macroblock_parameters (mode);

        if(input->rdopt == 2 && mode == 0 && input->EarlySkipEnable)
        {
          // check transform quantized coeff.
          if(currMB->cbp == 0)
            *inter_skip = 1;
        }
      }

      // Go through transform modes.
      // Note that if currMB->cbp is 0 one could choose to skip 8x8 mode
      // although this could be due to deadzoning decisions.
      //if (input->Transform8x8Mode==1 && currMB->cbp!=0)
      if (input->Transform8x8Mode==1)
      {
        //=========== try mb_types 1,2,3 with 8x8 transform ===========
        if ((mode >= 1 && mode <= 3) && currMB->luma_transform_size_8x8_flag == 0)
        {
          //try with 8x8 transform size
          currMB->luma_transform_size_8x8_flag = 1;
          continue;
        }
        //=========== try DIRECT-MODE with 8x8 transform ===========
        else if (mode == 0 && bslice && active_sps->direct_8x8_inference_flag && currMB->luma_transform_size_8x8_flag == 0)
        {
          //try with 8x8 transform size
          currMB->luma_transform_size_8x8_flag = 1;
          continue;
        }
        //=========== try mb_type P8x8 for mode 4 with 4x4/8x8 transform ===========
        else if ((mode == P8x8) && (enc_mb.valid[4]) && (currMB->luma_transform_size_8x8_flag == 0))
        {
          currMB->luma_transform_size_8x8_flag = 1; //check 8x8 partition for transform size 8x8
          continue;
        }
        else
        {
          currMB->luma_transform_size_8x8_flag = 0;
          break;
        }
      }
      else
        break;
    }

    // Encode with no coefficients. Currently only for direct. This could be extended to all other modes as in example.
    //if (mode < P8x8 && (*inter_skip == 0) && enc_mb.valid[mode] && currMB->cbp && (currMB->cbp&15) != 15 && !input->nobskip)
    if ( bslice && mode == 0 && (*inter_skip == 0) && enc_mb.valid[mode]
    && currMB->cbp && (currMB->cbp&15) != 15 && !input->nobskip)
    {
      img->NoResidueDirect = 1;
      if (RDCost_for_macroblocks (enc_mb.lambda_md, mode, min_rdcost, min_rate, i16mode))
      {
        //Rate control
        if (input->RCEnable)
          rc_store_diff(img->opix_x,img->opix_y,pred);

        store_macroblock_parameters (mode);
      }
    }
  }
}


/*!
*************************************************************************************
* \brief
*    Mode Decision for an 8x8 sub-macroblock
*************************************************************************************
*/
void submacroblock_mode_decision(RD_PARAMS enc_mb,
                                 RD_8x8DATA *dataTr,
                                 Macroblock *currMB,
                                 int ***cofACtr,
                                 int *have_direct,
                                 short bslice,
                                 int block,
                                 int *cost_direct,
                                 int *cost,
                                 int *cost8x8_direct,
                                 int transform8x8)
{
  int64 curr_cbp_blk;
  double min_rdcost, rdcost = 0.0;
  int j0, i0, j1, i1;
  int i,j, k;
  int min_cost8x8, index;
  int mode;
  int direct4x4_tmp, direct8x8_tmp;
  int bmcost[5] = {INT_MAX};
  int cnt_nonz = 0;
  int dummy;
  int best_cnt_nonz = 0;
  int maxindex =  (transform8x8) ? 2 : 5;
  int pix_x, pix_y;
  int block_x, block_y;
  int lambda_mf[3];
  static int fadjust[16][16], fadjustCr[2][16][16];
  int ***fadjustTransform = transform8x8? img->fadjust8x8 : img->fadjust4x4;
  int ****fadjustTransformCr = transform8x8? img->fadjust8x8Cr : img->fadjust4x4Cr;
  int lumaAdjustIndex = transform8x8? 2 : 3;
  int chromaAdjustIndex = transform8x8? 0 : 2;
  short pdir;
  short bi_pred_me;

  signed char best_pdir = 0;
  signed char best_ref[2] = {0, -1};
#ifdef BEST_NZ_COEFF
  int best_nz_coeff[2][2];
#endif


  //--- set coordinates ---
  j0 = ((block>>1)<<3);
  j1 = (j0>>2);
  i0 = ((block&0x01)<<3);
  i1 = (i0>>2);

#ifdef BEST_NZ_COEFF
  for(j = 0; j <= 1; j++)
  {
    for(i = 0; i <= 1; i++)
      best_nz_coeff[i][j] = img->nz_coeff[img->current_mb_nr][i1 + i][j1 + j] = 0;
  }
#endif

  if (transform8x8)
    currMB->luma_transform_size_8x8_flag = 1; //switch to transform size 8x8

  //--- store coding state before coding ---
  store_coding_state (cs_cm);

  //=====  LOOP OVER POSSIBLE CODING MODES FOR 8x8 SUB-PARTITION  =====
  for (min_cost8x8=INT_MAX, min_rdcost=1e30, index=(bslice?0:1); index<maxindex; index++)
  {
    mode = b8_mode_table[index];
    *cost = 0;
    if (enc_mb.valid[mode] && (transform8x8 == 0 || mode != 0 || (mode == 0 && active_sps->direct_8x8_inference_flag)))
    {
      curr_cbp_blk = 0;

      if (mode==0)
      {
        //--- Direct Mode ---
        if (!input->rdopt )
        {
          direct4x4_tmp = 0;
          direct8x8_tmp = 0;
          direct4x4_tmp = GetDirectCost8x8 ( block, &direct8x8_tmp);

          if ((direct4x4_tmp==INT_MAX)||(*cost_direct==INT_MAX))
          {
            *cost_direct = INT_MAX;
            if (transform8x8)
              *cost8x8_direct = INT_MAX;
          }
          else
          {
            *cost_direct += direct4x4_tmp;
            if (transform8x8)
              *cost8x8_direct += direct8x8_tmp;
          }
          (*have_direct) ++;

          if (transform8x8)
          {
            switch(input->Transform8x8Mode)
            {
            case 1: // Mixture of 8x8 & 4x4 transform
              if((direct8x8_tmp < direct4x4_tmp) || !(enc_mb.valid[5] && enc_mb.valid[6] && enc_mb.valid[7]))
                *cost = direct8x8_tmp;
              else
                *cost = direct4x4_tmp;
              break;
            case 2: // 8x8 Transform only
              *cost = direct8x8_tmp;
              break;
            default: // 4x4 Transform only
              *cost = direct4x4_tmp;
              break;
            }
            if (input->Transform8x8Mode==2)
              *cost = INT_MAX;
          }
          else
          {
            *cost = direct4x4_tmp;
          }
        }

        block_x = img->block_x+(block&1)*2;
        block_y = img->block_y+(block&2);
        best_ref[LIST_0] = direct_ref_idx[LIST_0][block_y][block_x];
        best_ref[LIST_1] = direct_ref_idx[LIST_1][block_y][block_x];
        best_pdir        = direct_pdir[block_y][block_x];
      } // if (mode==0)
      else
      {
        //======= motion estimation for all reference frames ========
        //-----------------------------------------------------------
        lambda_mf[F_PEL] = (input->CtxAdptLagrangeMult == 0)
          ? enc_mb.lambda_mf[F_PEL] :(int)(enc_mb.lambda_mf[F_PEL] * lambda_mf_factor);

        lambda_mf[H_PEL] = (input->CtxAdptLagrangeMult == 0)
          ? enc_mb.lambda_mf[H_PEL] :(int)(enc_mb.lambda_mf[H_PEL] * lambda_mf_factor);

        lambda_mf[Q_PEL] = (input->CtxAdptLagrangeMult == 0)
          ? enc_mb.lambda_mf[Q_PEL] :(int)(enc_mb.lambda_mf[Q_PEL] * lambda_mf_factor);


        PartitionMotionSearch (mode, block, lambda_mf);

        //--- get cost and reference frame for LIST 0 prediction ---
        bmcost[LIST_0] = INT_MAX;
        list_prediction_cost(LIST_0, block, mode, enc_mb, bmcost, best_ref);

        //store LIST 0 reference index for every block
        block_x = img->block_x+(block&1)*2;
        block_y = img->block_y+(block&2);
        for (j = block_y; j< block_y + 2; j++)
        {
          for (i = block_x; i < block_x + 2; i++)
          {
            enc_picture->ref_idx   [LIST_0][j][i] = best_ref[LIST_0];
            enc_picture->ref_pic_id[LIST_0][j][i] =
              enc_picture->ref_pic_num[enc_mb.list_offset[LIST_0]][(short)best_ref[LIST_0]];
          }
        }

        if (bslice)
        {
          //--- get cost and reference frame for LIST 1 prediction ---
          bmcost[LIST_1] = INT_MAX;
          bmcost[BI_PRED] = INT_MAX;
          list_prediction_cost(LIST_1, block, mode, enc_mb, bmcost, best_ref);

          // Compute bipredictive cost between best list 0 and best list 1 references
          list_prediction_cost(BI_PRED, block, mode, enc_mb, bmcost, best_ref);

          //--- get prediction direction ----
          determine_prediction_list(mode, bmcost, best_ref, &best_pdir, cost, &bi_pred_me);

          //store backward reference index for every block
          for (j = block_y; j< block_y + 2; j++)
          {
            memset(&enc_picture->ref_idx[LIST_0][j][block_x], best_ref[LIST_0], 2 * sizeof(char));
            memset(&enc_picture->ref_idx[LIST_1][j][block_x], best_ref[LIST_1], 2 * sizeof(char));
          }
        } // if (bslice)
        else
        {
          best_pdir = 0;
          *cost     = bmcost[LIST_0];
        }
      } // if (mode!=0)

      if (input->rdopt)
      {
        //--- get and check rate-distortion cost ---
        rdcost = RDCost_for_8x8blocks (&cnt_nonz, &curr_cbp_blk, enc_mb.lambda_md,
          block, mode, best_pdir, best_ref[LIST_0], best_ref[LIST_1]);
      }
      else
      {
        if (*cost!=INT_MAX)
          *cost += (REF_COST (enc_mb.lambda_mf[Q_PEL], B8Mode2Value (mode, best_pdir),
          enc_mb.list_offset[(best_pdir<1?LIST_0:LIST_1)]) - 1);
      }

      //--- set variables if best mode has changed ---
      if ( ( input->rdopt && rdcost < min_rdcost)
        || (!input->rdopt && *cost < min_cost8x8))
      {
        min_cost8x8                 = *cost;
        min_rdcost                  = rdcost;
        dataTr->part8x8mode [block] = mode;
        dataTr->part8x8pdir [block] = best_pdir;
        dataTr->part8x8fwref[block] = best_ref[LIST_0];
        dataTr->part8x8bwref[block] = best_ref[LIST_1];

        img->mb_data[img->current_mb_nr].b8mode[block] = mode;

#ifdef BEST_NZ_COEFF
        for(j = 0; j <= 1; j++)
        {
          for(i = 0; i <= 1; i++)
            best_nz_coeff[i][j]= cnt_nonz ? img->nz_coeff[img->current_mb_nr][i1 + i][j1 + j] : 0;
        }
#endif

        //--- store number of nonzero coefficients ---
        best_cnt_nonz  = cnt_nonz;

        if (input->rdopt)
        {
          //--- store block cbp ---
          cbp_blk8x8    &= (~(0x33 << (((block>>1)<<3)+((block%2)<<1)))); // delete bits for block
          cbp_blk8x8    |= curr_cbp_blk;

          //--- store coefficients ---
          for (k=0; k< 4; k++)
          {
            for (j=0; j< 2; j++)
              memcpy(&cofACtr[k][j][0],&img->cofAC[block][k][j][0], 65 * sizeof(int));
          }
          //--- store reconstruction and prediction ---
          for (j=j0; j<j0+8; j++)
          {
            pix_y = img->pix_y + j;
            for (i=i0; i<i0+8; i++)
            {
              pix_x = img->pix_x + i;
              dataTr->rec_mbY8x8[j][i] = enc_picture->imgY[pix_y][pix_x];
              dataTr->mpr8x8[j][i] = img->mpr[j][i];
              if(img->type==SP_SLICE && (!si_frame_indicator))
                dataTr->lrec[j][i]=lrec[pix_y][pix_x]; // store the coefficients for primary SP slice
            }
          }
        }
        if (img->AdaptiveRounding)
        {
          for (j=j0; j<j0+8; j++)
          {
            memcpy(&fadjust[j][i0], &fadjustTransform[0][j][i0], 8 * sizeof(int));
          }

          if (input->AdaptRndChroma)
          {
            int j0_cr = (j0 * img->mb_cr_size_y) / MB_BLOCK_SIZE;
            int i0_cr = (i0 * img->mb_cr_size_x) / MB_BLOCK_SIZE;
            for (j=j0_cr; j<j0_cr+(img->mb_cr_size_y >> 1); j++)
            {
              memcpy(&fadjustCr[0][j][i0_cr], &fadjustTransformCr[0][0][j][i0_cr], (img->mb_cr_size_x >> 1) * sizeof(int));
              memcpy(&fadjustCr[1][j][i0_cr], &fadjustTransformCr[0][1][j][i0_cr], (img->mb_cr_size_x >> 1) * sizeof(int));
            }
          }
        }
        //--- store best 8x8 coding state ---
        if (block < 3)
          store_coding_state (cs_b8);
      } // if (rdcost <= min_rdcost)

      //--- re-set coding state as it was before coding with current mode was performed ---
      reset_coding_state (cs_cm);
    } // if ((enc_mb.valid[mode] && (transform8x8 == 0 || mode != 0 || (mode == 0 && active_sps->direct_8x8_inference_flag)))
  } // for (min_rdcost=1e30, index=(bslice?0:1); index<6; index++)

#ifdef BEST_NZ_COEFF
  for(j = 0; j <= 1; j++)
  {
    for(i = 0; i <= 1; i++)
      img->nz_coeff[img->current_mb_nr][i1 + i][j1 + j] = best_nz_coeff[i][j];
  }
#endif

  if (!transform8x8)
    dataTr->cost8x8 += min_cost8x8;

  if (!input->rdopt)
  {
    if (transform8x8)
    {
      dataTr->cost8x8 += min_cost8x8;
      mode = dataTr->part8x8mode[block];
      pdir = dataTr->part8x8pdir[block];
    }
    else
    {
      mode = dataTr->part8x8mode[block];
      pdir = dataTr->part8x8pdir[block];
    }
    curr_cbp_blk  = 0;
    best_cnt_nonz = LumaResidualCoding8x8 (&dummy, &curr_cbp_blk, block, pdir,
      (pdir==0||pdir==2?mode:0), (pdir==1||pdir==2?mode:0), dataTr->part8x8fwref[block], dataTr->part8x8bwref[block]);

    cbp_blk8x8   &= (~(0x33 << (((block>>1)<<3)+((block%2)<<1)))); // delete bits for block
    cbp_blk8x8   |= curr_cbp_blk;

    //--- store coefficients ---
    for (k=0; k< 4; k++)
    {
        for (j=0; j< 2; j++)
          memcpy(cofACtr[k][j],img->cofAC[block][k][j],65 * sizeof(int));
    }

    //--- store reconstruction and prediction ---
    for (j=j0; j<j0+2* BLOCK_SIZE; j++)
    {
      memcpy(&dataTr->rec_mbY8x8[j][i0], &enc_picture->imgY[img->pix_y + j][img->pix_x + i0], 2* BLOCK_SIZE * sizeof (imgpel));
      memcpy(&dataTr->mpr8x8[j][i0], &img->mpr[j][i0], 2* BLOCK_SIZE * sizeof (imgpel));
      if(img->type==SP_SLICE &&(!si_frame_indicator))
        memcpy(&dataTr->lrec[j][i0],&lrec[img->pix_y+j][img->pix_x+i0],2*BLOCK_SIZE*sizeof(int)); // store coefficients for primary SP slice
    }
  }

  //----- set cbp and count of nonzero coefficients ---
  if (best_cnt_nonz)
  {
    cbp8x8       |= (1 << block);
    cnt_nonz_8x8 += best_cnt_nonz;
  }

  if (!transform8x8)
  {
    if (block<3)
    {
      //===== re-set reconstructed block =====
      j0   = 8*(block >> 1);
      i0   = 8*(block & 0x01);
      for (j=j0; j<j0 + 2 * BLOCK_SIZE; j++)
        {
        memcpy(&enc_picture->imgY[img->pix_y + j][img->pix_x], dataTr->rec_mbY8x8[j], 2 * BLOCK_SIZE * sizeof(imgpel));
        if(img->type==SP_SLICE &&(!si_frame_indicator))
          memcpy(&lrec[img->pix_y + j][img->pix_x], dataTr->lrec[j],2*BLOCK_SIZE*sizeof(imgpel)); // reset the coefficients for SP slice
      }
    } // if (block<3)
  }
  else
  {
    //======= save motion data for 8x8 partition for transform size 8x8 ========
    StoreNewMotionVectorsBlock8x8(0, block, dataTr->part8x8mode[block], dataTr->part8x8fwref[block], dataTr->part8x8bwref[block], dataTr->part8x8pdir[block], bslice);
  }
  //===== set motion vectors and reference frames (prediction) =====
  SetRefAndMotionVectors (block, dataTr->part8x8mode[block], dataTr->part8x8pdir[block], dataTr->part8x8fwref[block], dataTr->part8x8bwref[block]);

  //===== set the coding state after current block =====
  //if (transform8x8 == 0 || block < 3)
  if (block < 3)
    reset_coding_state (cs_b8);

  if (img->AdaptiveRounding)
  {
    for (j=j0; j<j0+2 * BLOCK_SIZE; j++)
    {
      memcpy(&fadjustTransform  [lumaAdjustIndex][j][i0], &fadjust[j][i0], 2 * BLOCK_SIZE * sizeof(int));
    }

    if (input->AdaptRndChroma)
    {
      int j0_cr = (j0 * img->mb_cr_size_y) >> MB_BLOCK_SHIFT;
      int i0_cr = (i0 * img->mb_cr_size_x) >> MB_BLOCK_SHIFT;

      for (j=j0_cr; j<j0_cr+(img->mb_cr_size_y >> 1); j++)
      {
        memcpy(&fadjustTransformCr[chromaAdjustIndex][0][j][i0_cr], &fadjustCr[0][j][i0_cr], (img->mb_cr_size_x >> 1) * sizeof(int));
        memcpy(&fadjustTransformCr[chromaAdjustIndex][1][j][i0_cr], &fadjustCr[1][j][i0_cr], (img->mb_cr_size_x >> 1) * sizeof(int));
      }
    }
  }
}


/*!
*************************************************************************************
* \brief
*    Checks whether a primary SP slice macroblock was encoded as I16
*************************************************************************************
*/
int check_for_SI16()
{
  int i,j;
  for(i=img->pix_y;i<img->pix_y+MB_BLOCK_SIZE;i++)
  {
    for(j=img->pix_x;j<img->pix_x+MB_BLOCK_SIZE;j++)
      if(lrec[i][j]!=-16)
        return 0;
  }
  return 1;
}

void get_initial_mb16x16_cost()
{
  Macroblock* currMB      = &img->mb_data[img->current_mb_nr];

  if (currMB->mb_available_left && currMB->mb_available_up)
  {
    mb16x16_cost = (mb16x16_cost_frame[img->current_mb_nr - 1] +
    mb16x16_cost_frame[img->current_mb_nr - (img->width>>4)] + 1)/2.0;
  }
  else if (currMB->mb_available_left)
  {
  mb16x16_cost = mb16x16_cost_frame[img->current_mb_nr - 1];
  }
  else if (currMB->mb_available_up)
  {
    mb16x16_cost = mb16x16_cost_frame[img->current_mb_nr - (img->width>>4)];
  }
  else
  {
    mb16x16_cost = CALM_MF_FACTOR_THRESHOLD;
  }

  lambda_mf_factor = mb16x16_cost < CALM_MF_FACTOR_THRESHOLD ? 1.0 : sqrt(mb16x16_cost / (CALM_MF_FACTOR_THRESHOLD * img->lambda_mf_factor[img->type][img->qp]));
}

void adjust_mb16x16_cost(int cost)
{
  mb16x16_cost = (double) cost;
  mb16x16_cost_frame[img->current_mb_nr] = mb16x16_cost;

  lambda_mf_factor = (mb16x16_cost < CALM_MF_FACTOR_THRESHOLD)
  ? 1.0
  : sqrt(mb16x16_cost / (CALM_MF_FACTOR_THRESHOLD * img->lambda_mf_factor[img->type][img->qp]));
}
