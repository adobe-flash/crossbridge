
/*!
 ***************************************************************************
 * \file md_low.c
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
static imgpel temp_imgY[16][16]; // to temp store the Y data for 8x8 transform


/*!
*************************************************************************************
* \brief
*    Mode Decision for a macroblock
*************************************************************************************
*/
void encode_one_macroblock_low ()
{

  int         block, mode, i, j, k, dummy, MEPos;
  signed char best_pdir;
  RD_PARAMS   enc_mb;
  signed char best_ref[2] = {0, -1};
  int         bmcost[5] = {INT_MAX};
  int         cost=0;
  int         min_cost = INT_MAX, cost_direct=0, have_direct=0, i16mode=0;
  int         intra1 = 0;
  int         temp_cpb = 0;
  int         best_transform_flag = 0;
  int         cost8x8_direct = 0;
  short       islice      = (short) (img->type==I_SLICE);
  short       bslice      = (short) (img->type==B_SLICE);
  short       pslice      = (short) ((img->type==P_SLICE) || (img->type==SP_SLICE));
  short       intra       = (short) (islice || (pslice && img->mb_y==img->mb_y_upd && img->mb_y_upd!=img->mb_y_intra));
  int         lambda_mf[3];
  int         pix_x, pix_y;
  Macroblock* currMB      = &img->mb_data[img->current_mb_nr];
  int         prev_mb_nr  = FmoGetPreviousMBNr(img->current_mb_nr);
  Macroblock* prevMB      = (prev_mb_nr >= 0) ? &img->mb_data[prev_mb_nr]:NULL ;

  signed char **ipredmodes = img->ipredmode;
  short   *allmvs = img->all_mv[0][0][0][0][0];
  int     ****i4p;  //for non-RD-opt. mode

  int tmp_8x8_flag, tmp_no_mbpart;
  // Fast Mode Decision
  short inter_skip = 0;


  if(input->SearchMode == UM_HEX)
  {
    UMHEX_decide_intrabk_SAD();
  }
  else if (input->SearchMode == UM_HEX_SIMPLE)
  {
    smpUMHEX_decide_intrabk_SAD();
  }

  intra |= RandomIntra (img->current_mb_nr);    // Forced Pseudo-Random Intra

  //===== Setup Macroblock encoding parameters =====
  init_enc_mb_params(currMB, &enc_mb, intra, bslice);


  // reset chroma intra predictor to default
  currMB->c_ipred_mode = DC_PRED_8;

  //=====   S T O R E   C O D I N G   S T A T E   =====
  //---------------------------------------------------
  store_coding_state (cs_cm);

  if (!intra)
  {
    //===== set direct motion vectors =====
    best_mode = 1;
    if (bslice)
    {
      Get_Direct_Motion_Vectors ();
    }

    if (input->CtxAdptLagrangeMult == 1)
    {
      get_initial_mb16x16_cost();
    }

    //===== MOTION ESTIMATION FOR 16x16, 16x8, 8x16 BLOCKS =====
    for (min_cost=INT_MAX, mode=1; mode<4; mode++)
    {
      bi_pred_me = 0;
      img->bi_pred_me[mode]=0;
      if (enc_mb.valid[mode] && !inter_skip)
      {
        for (cost=0, block=0; block<(mode==1?1:2); block++)
        {
          for (MEPos=0; MEPos<3; MEPos++)
          {
            lambda_mf[MEPos] = input->CtxAdptLagrangeMult == 0 ? enc_mb.lambda_mf[MEPos] : (int)(enc_mb.lambda_mf[MEPos] * sqrt(lambda_mf_factor));
          }
          PartitionMotionSearch (mode, block, lambda_mf);

          //--- set 4x4 block indizes (for getting MV) ---
          j = (block==1 && mode==2 ? 2 : 0);
          i = (block==1 && mode==3 ? 2 : 0);

          //--- get cost and reference frame for List 0 prediction ---
          bmcost[LIST_0] = INT_MAX;
          list_prediction_cost(LIST_0, block, mode, enc_mb, bmcost, best_ref);

          if (bslice)
          {
            //--- get cost and reference frame for List 1 prediction ---
            bmcost[LIST_1] = INT_MAX;
            list_prediction_cost(LIST_1, block, mode, enc_mb, bmcost, best_ref);

            // Compute bipredictive cost between best list 0 and best list 1 references
            list_prediction_cost(BI_PRED, block, mode, enc_mb, bmcost, best_ref);

            // Finally, if mode 16x16, compute cost for bipredictive ME vectore
            if (input->BiPredMotionEstimation && mode == 1)
            {
              list_prediction_cost(BI_PRED_L0, block, mode, enc_mb, bmcost, 0);
              list_prediction_cost(BI_PRED_L1, block, mode, enc_mb, bmcost, 0);
            }
            else
            {
              bmcost[BI_PRED_L0] = INT_MAX;
              bmcost[BI_PRED_L1] = INT_MAX;
            }

            // Determine prediction list based on mode cost
            determine_prediction_list(mode, bmcost, best_ref, &best_pdir, &cost, &bi_pred_me);
          }
          else // if (bslice)
          {
            best_pdir  = 0;
            cost      += bmcost[LIST_0];
          }

          assign_enc_picture_params(mode, best_pdir, block, enc_mb.list_offset[LIST_0], best_ref[LIST_0], best_ref[LIST_1], bslice);

          //----- set reference frame and direction parameters -----
          if (mode==3)
          {
            best8x8fwref [3][block  ] = best8x8fwref [3][  block+2] = best_ref[LIST_0];
            best8x8pdir  [3][block  ] = best8x8pdir  [3][  block+2] = best_pdir;
            best8x8bwref [3][block  ] = best8x8bwref [3][  block+2] = best_ref[LIST_1];
          }
          else if (mode==2)
          {
            best8x8fwref [2][2*block] = best8x8fwref [2][2*block+1] = best_ref[LIST_0];
            best8x8pdir  [2][2*block] = best8x8pdir  [2][2*block+1] = best_pdir;
            best8x8bwref [2][2*block] = best8x8bwref [2][2*block+1] = best_ref[LIST_1];
          }
          else
          {
            memset(&best8x8fwref [1][0], best_ref[LIST_0], 4 * sizeof(char));
            memset(&best8x8bwref [1][0], best_ref[LIST_1], 4 * sizeof(char));
            best8x8pdir  [1][0] = best8x8pdir  [1][1] = best8x8pdir  [1][2] = best8x8pdir  [1][3] = best_pdir;
          }

          //--- set reference frames and motion vectors ---
          if (mode>1 && block==0)
            SetRefAndMotionVectors (block, mode, best_pdir, best_ref[LIST_0], best_ref[LIST_1]);
        } // for (block=0; block<(mode==1?1:2); block++)

        currMB->luma_transform_size_8x8_flag = 0;
        if (input->Transform8x8Mode) //for inter rd-off, set 8x8 to do 8x8 transform
        {
          SetModesAndRefframeForBlocks(mode);
          currMB->luma_transform_size_8x8_flag = TransformDecision(-1, &cost);
        }

        if ((!inter_skip) && (cost < min_cost))
        {
          best_mode = (short) mode;
          min_cost  = cost;
          best_transform_flag = currMB->luma_transform_size_8x8_flag;

          if (input->CtxAdptLagrangeMult == 1)
          {
            adjust_mb16x16_cost(cost);
          }
        }
      } // if (enc_mb.valid[mode])
    } // for (mode=1; mode<4; mode++)

    if ((!inter_skip) && enc_mb.valid[P8x8])
    {
      giRDOpt_B8OnlyFlag = 1;

      tr8x8.cost8x8 = INT_MAX;
      tr4x4.cost8x8 = INT_MAX;
      //===== store coding state of macroblock =====
      store_coding_state (cs_mb);

      currMB->all_blk_8x8 = -1;

      if (input->Transform8x8Mode)
      {
        tr8x8.cost8x8 = 0;
        //===========================================================
        // Check 8x8 partition with transform size 8x8
        //===========================================================
        //=====  LOOP OVER 8x8 SUB-PARTITIONS  (Motion Estimation & Mode Decision) =====
        for (cost_direct=cbp8x8=cbp_blk8x8=cnt_nonz_8x8=0, block=0; block<4; block++)
        {
          submacroblock_mode_decision(enc_mb, &tr8x8, currMB, cofAC_8x8ts[block],
            &have_direct, bslice, block, &cost_direct, &cost, &cost8x8_direct, 1);
          best8x8mode       [block] = tr8x8.part8x8mode [block];
          best8x8pdir [P8x8][block] = tr8x8.part8x8pdir [block];
          best8x8fwref[P8x8][block] = tr8x8.part8x8fwref[block];
          best8x8bwref[P8x8][block] = tr8x8.part8x8bwref[block];
        }

        // following params could be added in RD_8x8DATA structure
        cbp8_8x8ts      = cbp8x8;
        cbp_blk8_8x8ts  = cbp_blk8x8;
        cnt_nonz8_8x8ts = cnt_nonz_8x8;
        currMB->luma_transform_size_8x8_flag = 0; //switch to 4x4 transform size

        //--- re-set coding state (as it was before 8x8 block coding) ---
        //reset_coding_state (cs_mb);
      }// if (input->Transform8x8Mode)


      if (input->Transform8x8Mode != 2)
      {
        tr4x4.cost8x8 = 0;
        //=================================================================
        // Check 8x8, 8x4, 4x8 and 4x4 partitions with transform size 4x4
        //=================================================================
        //=====  LOOP OVER 8x8 SUB-PARTITIONS  (Motion Estimation & Mode Decision) =====
        for (cost_direct=cbp8x8=cbp_blk8x8=cnt_nonz_8x8=0, block=0; block<4; block++)
        {
          submacroblock_mode_decision(enc_mb, &tr4x4, currMB, cofAC8x8[block],
            &have_direct, bslice, block, &cost_direct, &cost, &cost8x8_direct, 0);

          best8x8mode       [block] = tr4x4.part8x8mode [block];
          best8x8pdir [P8x8][block] = tr4x4.part8x8pdir [block];
          best8x8fwref[P8x8][block] = tr4x4.part8x8fwref[block];
          best8x8bwref[P8x8][block] = tr4x4.part8x8bwref[block];
        }
        //--- re-set coding state (as it was before 8x8 block coding) ---
        // reset_coding_state (cs_mb);
      }// if (input->Transform8x8Mode != 2)

      //--- re-set coding state (as it was before 8x8 block coding) ---
      reset_coding_state (cs_mb);


      // This is not enabled yet since mpr has reverse order.
      if (input->RCEnable)
        rc_store_diff(img->opix_x,img->opix_y,img->mpr);

      //check cost for P8x8 for non-rdopt mode
      if (tr4x4.cost8x8 < min_cost || tr8x8.cost8x8 < min_cost)
      {
        best_mode = P8x8;
        if (input->Transform8x8Mode == 2)
        {
          min_cost = tr8x8.cost8x8;
          currMB->luma_transform_size_8x8_flag=1;
        }
        else if (input->Transform8x8Mode)
        {
          if (tr8x8.cost8x8 < tr4x4.cost8x8)
          {
            min_cost = tr8x8.cost8x8;
            currMB->luma_transform_size_8x8_flag=1;
          }
          else if(tr4x4.cost8x8 < tr8x8.cost8x8)
          {
            min_cost = tr4x4.cost8x8;
            currMB->luma_transform_size_8x8_flag=0;
          }
          else
          {
            if (GetBestTransformP8x8() == 0)
            {
              min_cost = tr4x4.cost8x8;
              currMB->luma_transform_size_8x8_flag=0;
            }
            else
            {
              min_cost = tr8x8.cost8x8;
              currMB->luma_transform_size_8x8_flag=1;
            }
          }
        }
        else
        {
          min_cost = tr4x4.cost8x8;
          currMB->luma_transform_size_8x8_flag=0;
        }
      }// if ((tr4x4.cost8x8 < min_cost || tr8x8.cost8x8 < min_cost))
      giRDOpt_B8OnlyFlag = 0;
    }
    else // if (enc_mb.valid[P8x8])
    {
      tr4x4.cost8x8 = INT_MAX;
    }

    // Find a motion vector for the Skip mode
    if(pslice)
      FindSkipModeMotionVector ();
  }
  else // if (!intra)
  {
    min_cost = INT_MAX;
  }

  //========= C H O O S E   B E S T   M A C R O B L O C K   M O D E =========
  //-------------------------------------------------------------------------
  tmp_8x8_flag = currMB->luma_transform_size_8x8_flag;  //save 8x8_flag
  tmp_no_mbpart = currMB->NoMbPartLessThan8x8Flag;      //save no-part-less

  if (img->yuv_format != YUV400)
    // precompute all chroma intra prediction modes
    IntraChromaPrediction(NULL, NULL, NULL);

  if (enc_mb.valid[0] && bslice) // check DIRECT MODE
  {
    if(have_direct)
    {
      switch(input->Transform8x8Mode)
      {
      case 1: // Mixture of 8x8 & 4x4 transform
        cost = ((cost8x8_direct < cost_direct) || !(enc_mb.valid[5] && enc_mb.valid[6] && enc_mb.valid[7]))
          ? cost8x8_direct : cost_direct;
        break;
      case 2: // 8x8 Transform only
        cost = cost8x8_direct;
        break;
      default: // 4x4 Transform only
        cost = cost_direct;
        break;
      }
    }
    else
    { //!have_direct
      cost = GetDirectCostMB ();
    }
    if (cost!=INT_MAX)
    {
      cost -= (int)floor(16*enc_mb.lambda_md+0.4999);
    }

    if (cost <= min_cost)
    {
      if(active_sps->direct_8x8_inference_flag && input->Transform8x8Mode)
      {
        if(input->Transform8x8Mode==2)
          currMB->luma_transform_size_8x8_flag=1;
        else
        {
          if(cost8x8_direct < cost_direct)
            currMB->luma_transform_size_8x8_flag=1;
          else
            currMB->luma_transform_size_8x8_flag=0;
        }
      }
      else
        currMB->luma_transform_size_8x8_flag=0;

      //Rate control
      if (input->RCEnable)
        rc_store_diff(img->opix_x,img->opix_y,img->mpr);

      min_cost  = cost;
      best_mode = 0;
      tmp_8x8_flag = currMB->luma_transform_size_8x8_flag;
    }
    else
    {
      currMB->luma_transform_size_8x8_flag = tmp_8x8_flag; // restore if not best
      currMB->NoMbPartLessThan8x8Flag = tmp_no_mbpart; // restore if not best
    }
  }

  if (enc_mb.valid[I8MB]) // check INTRA8x8
  {
    currMB->luma_transform_size_8x8_flag = 1; // at this point cost will ALWAYS be less than min_cost

    currMB->mb_type = I8MB;
    temp_cpb = Mode_Decision_for_new_Intra8x8Macroblock (enc_mb.lambda_md, &cost);

    if (cost <= min_cost)
    {
      currMB->cbp = temp_cpb;

      //coeffs
      if (input->Transform8x8Mode != 2)
      {
        i4p=cofAC; cofAC=img->cofAC; img->cofAC=i4p;
      }

      for(j=0; j<MB_BLOCK_SIZE; j++)
      {
        pix_y = img->pix_y + j;
        for(i=0; i<MB_BLOCK_SIZE; i++)
        {
          pix_x = img->pix_x + i;
          temp_imgY[j][i] = enc_picture->imgY[pix_y][pix_x];
        }
      }

      //Rate control
      if (input->RCEnable)
        rc_store_diff(img->opix_x,img->opix_y,img->mpr);

      min_cost  = cost;
      best_mode = I8MB;
      tmp_8x8_flag = currMB->luma_transform_size_8x8_flag;
    }
    else
      currMB->luma_transform_size_8x8_flag = tmp_8x8_flag; // restore if not best
  }

  if (enc_mb.valid[I4MB]) // check INTRA4x4
  {
    currMB->luma_transform_size_8x8_flag = 0;
    currMB->mb_type = I4MB;
    temp_cpb = Mode_Decision_for_Intra4x4Macroblock (enc_mb.lambda_md, &cost);

    if (cost <= min_cost)
    {
      currMB->cbp = temp_cpb;

      //Rate control
      if (input->RCEnable)
        rc_store_diff(img->opix_x,img->opix_y,img->mpr);

      min_cost  = cost;
      best_mode = I4MB;
      tmp_8x8_flag = currMB->luma_transform_size_8x8_flag;
    }
    else
    {
      currMB->luma_transform_size_8x8_flag = tmp_8x8_flag; // restore if not best
      //coeffs
      i4p=cofAC; cofAC=img->cofAC; img->cofAC=i4p;
    }
  }
  if (enc_mb.valid[I16MB]) // check INTRA16x16
  {
    currMB->luma_transform_size_8x8_flag = 0;
    intrapred_luma_16x16 ();
    cost = find_sad_16x16 (&i16mode);

    if (cost < min_cost)
    {
      //Rate control
      // should this access opix or pix?
      if (input->RCEnable)
        rc_store_diff(img->opix_x,img->opix_y,img->mprr_2[i16mode]);

      best_mode   = I16MB;
      currMB->cbp = dct_luma_16x16 (i16mode);

    }
    else
    {
      currMB->luma_transform_size_8x8_flag = tmp_8x8_flag; // restore
      currMB->NoMbPartLessThan8x8Flag = tmp_no_mbpart;     // restore
    }
  }

  intra1 = IS_INTRA(currMB);

  //=====  S E T   F I N A L   M A C R O B L O C K   P A R A M E T E R S ======
  //---------------------------------------------------------------------------
  {
    //===== set parameters for chosen mode =====
    SetModesAndRefframeForBlocks (best_mode);

    if (best_mode==P8x8)
    {
      if (currMB->luma_transform_size_8x8_flag && (cbp8_8x8ts == 0) && input->Transform8x8Mode != 2)
        currMB->luma_transform_size_8x8_flag = 0;

      SetCoeffAndReconstruction8x8 (currMB);

      memset(currMB->intra_pred_modes, DC_PRED, MB_BLOCK_PARTITIONS * sizeof(char));
      for (k=0, j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
        memset(&ipredmodes[j][img->block_x], DC_PRED, BLOCK_MULTIPLE * sizeof(char));
    }
    else
    {
      //===== set parameters for chosen mode =====
      if (best_mode == I8MB)
      {
        memcpy(currMB->intra_pred_modes,currMB->intra_pred_modes8x8, MB_BLOCK_PARTITIONS * sizeof(char));
        for(j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
          memcpy(&img->ipredmode[j][img->block_x],&img->ipredmode8x8[j][img->block_x], BLOCK_MULTIPLE * sizeof(char));

        //--- restore reconstruction for 8x8 transform ---
        for(j=0; j<MB_BLOCK_SIZE; j++)
        {
          memcpy(&enc_picture->imgY[img->pix_y + j][img->pix_x],temp_imgY[j], MB_BLOCK_SIZE * sizeof(imgpel));
        }
      }

      if ((best_mode!=I4MB)&&(best_mode != I8MB))
      {
        memset(currMB->intra_pred_modes,DC_PRED, MB_BLOCK_PARTITIONS * sizeof(char));
        for(j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
          memset(&ipredmodes[j][img->block_x],DC_PRED, BLOCK_MULTIPLE * sizeof(char));

        if (best_mode!=I16MB)
        {
          if((best_mode>=1) && (best_mode<=3))
            currMB->luma_transform_size_8x8_flag = best_transform_flag;
          LumaResidualCoding ();

          if((currMB->cbp==0)&&(best_mode==0))
            currMB->luma_transform_size_8x8_flag = 0;

          //Rate control
          if (input->RCEnable)
            rc_store_diff(img->opix_x,img->opix_y,img->mpr);
        }
      }
    }
    //check luma cbp for transform size flag
    if (((currMB->cbp&15) == 0) && !(IS_OLDINTRA(currMB) || currMB->mb_type == I8MB))
      currMB->luma_transform_size_8x8_flag = 0;

    // precompute all chroma intra prediction modes
    if (img->yuv_format != YUV400)
      IntraChromaPrediction(NULL, NULL, NULL);

    img->i16offset = 0;
    dummy = 0;

    if (img->yuv_format!=YUV400)
      ChromaResidualCoding (&dummy);

    if (best_mode==I16MB)
    {
      img->i16offset = I16Offset  (currMB->cbp, i16mode);
    }

    SetMotionVectorsMB (currMB, bslice);

    //===== check for SKIP mode =====
    if ((pslice) && best_mode==1 && currMB->cbp==0 &&
      enc_picture->ref_idx[LIST_0][img->block_y][img->block_x]    == 0 &&
      enc_picture->mv     [LIST_0][img->block_y][img->block_x][0] == allmvs[0] &&
      enc_picture->mv     [LIST_0][img->block_y][img->block_x][1] == allmvs[1])
    {
      currMB->mb_type = currMB->b8mode[0] = currMB->b8mode[1] = currMB->b8mode[2] = currMB->b8mode[3] = 0;
      currMB->luma_transform_size_8x8_flag = 0;
    }

    if(img->MbaffFrameFlag)
      set_mbaff_parameters();
  }

  // Rate control
  if(input->RCEnable)
    update_rc(currMB, best_mode);

  rdopt->min_rdcost = min_cost;

  if ( (img->MbaffFrameFlag)
    && (img->current_mb_nr%2)
    && (currMB->mb_type ? 0:((bslice) ? !currMB->cbp:1))  // bottom is skip
    && (prevMB->mb_type ? 0:((bslice) ? !prevMB->cbp:1))
    && !(field_flag_inference() == enc_mb.curr_mb_field)) // top is skip
  {
    rdopt->min_rdcost = 1e30;  // don't allow coding of a MB pair as skip if wrong inference
  }

  //===== Decide if this MB will restrict the reference frames =====
  if (input->RestrictRef)
    update_refresh_map(intra, intra1, currMB);

  if(input->SearchMode == UM_HEX)
  {
    UMHEX_skip_intrabk_SAD(best_mode, listXsize[enc_mb.list_offset[LIST_0]]);
  }
  else if(input->SearchMode == UM_HEX_SIMPLE)
  {
    smpUMHEX_skip_intrabk_SAD(best_mode, listXsize[enc_mb.list_offset[LIST_0]]);
  }

  //--- constrain intra prediction ---
  if(input->UseConstrainedIntraPred && (img->type==P_SLICE || img->type==B_SLICE))
  {
    img->intra_block[img->current_mb_nr] = IS_INTRA(currMB);
  }
}

