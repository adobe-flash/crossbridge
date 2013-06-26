
/*!
 *************************************************************************************
 * \file header.c
 *
 * \brief
 *    H.264 Slice headers
 *
 *************************************************************************************
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "global.h"
#include "elements.h"
#include "defines.h"
#include "fmo.h"
#include "vlc.h"
#include "mbuffer.h"
#include "header.h"

#include "ctx_tables.h"

extern StorablePicture *dec_picture;

#if TRACE
#define SYMTRACESTRING(s) strncpy(sym.tracestring,s,TRACESTRING_SIZE)
#else
#define SYMTRACESTRING(s) // to nothing
#endif

extern int UsedBits;

static void ref_pic_list_reordering();
static void pred_weight_table();


/*!
 ************************************************************************
 * \brief
 *    calculate Ceil(Log2(uiVal))
 ************************************************************************
 */
unsigned CeilLog2( unsigned uiVal)
{
  unsigned uiTmp = uiVal-1;
  unsigned uiRet = 0;

  while( uiTmp != 0 )
  {
    uiTmp >>= 1;
    uiRet++;
  }
  return uiRet;
}


/*!
 ************************************************************************
 * \brief
 *    read the first part of the header (only the pic_parameter_set_id)
 * \return
 *    Length of the first part of the slice header (in bits)
 ************************************************************************
 */
int FirstPartOfSliceHeader()
{
  Slice *currSlice = img->currentSlice;
  int dP_nr = assignSE2partition[currSlice->dp_mode][SE_HEADER];
  DataPartition *partition = &(currSlice->partArr[dP_nr]);
  Bitstream *currStream = partition->bitstream;
  int tmp;

  UsedBits= partition->bitstream->frame_bitoffset; // was hardcoded to 31 for previous start-code. This is better.

  // Get first_mb_in_slice
  currSlice->start_mb_nr = ue_v ("SH: first_mb_in_slice", currStream);

  tmp = ue_v ("SH: slice_type", currStream);

  if (tmp>4) tmp -=5;

  img->type = currSlice->picture_type = (SliceType) tmp;

  currSlice->pic_parameter_set_id = ue_v ("SH: pic_parameter_set_id", currStream);

  return UsedBits;
}

/*!
 ************************************************************************
 * \brief
 *    read the scond part of the header (without the pic_parameter_set_id
 * \return
 *    Length of the second part of the Slice header in bits
 ************************************************************************
 */
int RestOfSliceHeader()
{
  Slice *currSlice = img->currentSlice;
  int dP_nr = assignSE2partition[currSlice->dp_mode][SE_HEADER];
  DataPartition *partition = &(currSlice->partArr[dP_nr]);
  Bitstream *currStream = partition->bitstream;

  int val, len;

  img->frame_num = u_v (active_sps->log2_max_frame_num_minus4 + 4, "SH: frame_num", currStream);

  /* Tian Dong: frame_num gap processing, if found */
  if (img->idr_flag)
  {
    img->pre_frame_num = img->frame_num;
    // picture error concealment
    img->last_ref_pic_poc = 0;
    assert(img->frame_num == 0);
  }

  if (active_sps->frame_mbs_only_flag)
  {
    img->structure = FRAME;
    img->field_pic_flag=0;
  }
  else
  {
    // field_pic_flag   u(1)
    img->field_pic_flag = u_1("SH: field_pic_flag", currStream);
    if (img->field_pic_flag)
    {
      // bottom_field_flag  u(1)
      img->bottom_field_flag = u_1("SH: bottom_field_flag", currStream);

      img->structure = img->bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD;
    }
    else
    {
      img->structure = FRAME;
      img->bottom_field_flag=0;
    }
  }

  currSlice->structure = (PictureStructure) img->structure;

  img->MbaffFrameFlag=(active_sps->mb_adaptive_frame_field_flag && (img->field_pic_flag==0));

  if (img->structure == FRAME       ) assert (img->field_pic_flag == 0);
  if (img->structure == TOP_FIELD   ) assert (img->field_pic_flag == 1 && img->bottom_field_flag == 0);
  if (img->structure == BOTTOM_FIELD) assert (img->field_pic_flag == 1 && img->bottom_field_flag == 1);

  if (img->idr_flag)
  {
    img->idr_pic_id = ue_v("SH: idr_pic_id", currStream);
  }

  if (active_sps->pic_order_cnt_type == 0)
  {
    img->pic_order_cnt_lsb = u_v(active_sps->log2_max_pic_order_cnt_lsb_minus4 + 4, "SH: pic_order_cnt_lsb", currStream);
    if( active_pps->pic_order_present_flag  ==  1 &&  !img->field_pic_flag )
      img->delta_pic_order_cnt_bottom = se_v("SH: delta_pic_order_cnt_bottom", currStream);
    else
      img->delta_pic_order_cnt_bottom = 0;
  }
  if( active_sps->pic_order_cnt_type == 1 && !active_sps->delta_pic_order_always_zero_flag )
  {
    img->delta_pic_order_cnt[ 0 ] = se_v("SH: delta_pic_order_cnt[0]", currStream);
    if( active_pps->pic_order_present_flag  ==  1  &&  !img->field_pic_flag )
      img->delta_pic_order_cnt[ 1 ] = se_v("SH: delta_pic_order_cnt[1]", currStream);
  }else
  {
    if (active_sps->pic_order_cnt_type == 1)
    {
      img->delta_pic_order_cnt[ 0 ] = 0;
      img->delta_pic_order_cnt[ 1 ] = 0;
    }
  }

  //! redundant_pic_cnt is missing here
  if (active_pps->redundant_pic_cnt_present_flag)
  {
    img->redundant_pic_cnt = ue_v ("SH: redundant_pic_cnt", currStream);
  }

  if(img->type==B_SLICE)
  {
    img->direct_spatial_mv_pred_flag = u_1 ("SH: direct_spatial_mv_pred_flag", currStream);
  }

  img->num_ref_idx_l0_active = active_pps->num_ref_idx_l0_active_minus1 + 1;
  img->num_ref_idx_l1_active = active_pps->num_ref_idx_l1_active_minus1 + 1;

  if(img->type==P_SLICE || img->type == SP_SLICE || img->type==B_SLICE)
  {
    val = u_1 ("SH: num_ref_idx_override_flag", currStream);
    if (val)
    {
      img->num_ref_idx_l0_active = 1 + ue_v ("SH: num_ref_idx_l0_active_minus1", currStream);

      if(img->type==B_SLICE)
      {
        img->num_ref_idx_l1_active = 1 + ue_v ("SH: num_ref_idx_l1_active_minus1", currStream);
      }
    }
  }
  if (img->type!=B_SLICE)
  {
    img->num_ref_idx_l1_active = 0;
  }

  ref_pic_list_reordering();

  img->apply_weights = ((active_pps->weighted_pred_flag && (currSlice->picture_type == P_SLICE || currSlice->picture_type == SP_SLICE) )
          || ((active_pps->weighted_bipred_idc > 0 ) && (currSlice->picture_type == B_SLICE)));

  if ((active_pps->weighted_pred_flag&&(img->type==P_SLICE|| img->type == SP_SLICE))||
      (active_pps->weighted_bipred_idc==1 && (img->type==B_SLICE)))
  {
    pred_weight_table();
  }

  if (img->nal_reference_idc)
    dec_ref_pic_marking(currStream);

  if (active_pps->entropy_coding_mode_flag && img->type!=I_SLICE && img->type!=SI_SLICE)
  {
    img->model_number = ue_v("SH: cabac_init_idc", currStream);
  }
  else
  {
    img->model_number = 0;
  }

  val = se_v("SH: slice_qp_delta", currStream);
  currSlice->qp = img->qp = 26 + active_pps->pic_init_qp_minus26 + val;
  if ((img->qp < -img->bitdepth_luma_qp_scale) || (img->qp > 51))
    error ("slice_qp_delta makes slice_qp_y out of range", 500);


  currSlice->slice_qp_delta = val;

  if(img->type==SP_SLICE || img->type == SI_SLICE)
  {
    if(img->type==SP_SLICE)
    {
      img->sp_switch = u_1 ("SH: sp_for_switch_flag", currStream);
    }
    val = se_v("SH: slice_qs_delta", currStream);
    img->qpsp = 26 + active_pps->pic_init_qs_minus26 + val;
    if ((img->qpsp < 0) || (img->qpsp > 51))
      error ("slice_qs_delta makes slice_qs_y out of range", 500);
  }

  if (active_pps->deblocking_filter_control_present_flag)
  {
    currSlice->LFDisableIdc = ue_v ("SH: disable_deblocking_filter_idc", currStream);

    if (currSlice->LFDisableIdc!=1)
    {
      currSlice->LFAlphaC0Offset = 2 * se_v("SH: slice_alpha_c0_offset_div2", currStream);
      currSlice->LFBetaOffset = 2 * se_v("SH: slice_beta_offset_div2", currStream);
    }
    else
    {
      currSlice->LFAlphaC0Offset = currSlice->LFBetaOffset = 0;
    }
  }
  else
  {
    currSlice->LFDisableIdc = currSlice->LFAlphaC0Offset = currSlice->LFBetaOffset = 0;
  }

  if (active_pps->num_slice_groups_minus1>0 && active_pps->slice_group_map_type>=3 &&
      active_pps->slice_group_map_type<=5)
  {
    len = (active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1)/
          (active_pps->slice_group_change_rate_minus1+1);
    if (((active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1))%
          (active_pps->slice_group_change_rate_minus1+1))
          len +=1;

    len = CeilLog2(len+1);

    img->slice_group_change_cycle = u_v (len, "SH: slice_group_change_cycle", currStream);
  }
  img->PicHeightInMbs = img->FrameHeightInMbs / ( 1 + img->field_pic_flag );
  img->PicSizeInMbs   = img->PicWidthInMbs * img->PicHeightInMbs;
  img->FrameSizeInMbs = img->PicWidthInMbs * img->FrameHeightInMbs;

  return UsedBits;
}


/*!
 ************************************************************************
 * \brief
 *    read the reference picture reordering information
 ************************************************************************
 */
static void ref_pic_list_reordering()
{
  Slice *currSlice = img->currentSlice;
  int dP_nr = assignSE2partition[currSlice->dp_mode][SE_HEADER];
  DataPartition *partition = &(currSlice->partArr[dP_nr]);
  Bitstream *currStream = partition->bitstream;
  int i, val;

  alloc_ref_pic_list_reordering_buffer(currSlice);

  if (img->type!=I_SLICE && img->type!=SI_SLICE)
  {
    val = currSlice->ref_pic_list_reordering_flag_l0 = u_1 ("SH: ref_pic_list_reordering_flag_l0", currStream);

    if (val)
    {
      i=0;
      do
      {
        val = currSlice->reordering_of_pic_nums_idc_l0[i] = ue_v("SH: reordering_of_pic_nums_idc_l0", currStream);
        if (val==0 || val==1)
        {
          currSlice->abs_diff_pic_num_minus1_l0[i] = ue_v("SH: abs_diff_pic_num_minus1_l0", currStream);
        }
        else
        {
          if (val==2)
          {
            currSlice->long_term_pic_idx_l0[i] = ue_v("SH: long_term_pic_idx_l0", currStream);
          }
        }
        i++;
        // assert (i>img->num_ref_idx_l0_active);
      } while (val != 3);
    }
  }

  if (img->type==B_SLICE)
  {
    val = currSlice->ref_pic_list_reordering_flag_l1 = u_1 ("SH: ref_pic_list_reordering_flag_l1", currStream);

    if (val)
    {
      i=0;
      do
      {
        val = currSlice->reordering_of_pic_nums_idc_l1[i] = ue_v("SH: reordering_of_pic_nums_idc_l1", currStream);
        if (val==0 || val==1)
        {
          currSlice->abs_diff_pic_num_minus1_l1[i] = ue_v("SH: abs_diff_pic_num_minus1_l1", currStream);
        }
        else
        {
          if (val==2)
          {
            currSlice->long_term_pic_idx_l1[i] = ue_v("SH: long_term_pic_idx_l1", currStream);
          }
        }
        i++;
        // assert (i>img->num_ref_idx_l1_active);
      } while (val != 3);
    }
  }

  // set reference index of redundant slices.
  if(img->redundant_pic_cnt)
  {
    redundant_slice_ref_idx = currSlice->abs_diff_pic_num_minus1_l0[0] + 1;
  }
}

/*!
 ************************************************************************
 * \brief
 *    read the weighted prediction tables
 ************************************************************************
 */
static void pred_weight_table()
{
  Slice *currSlice = img->currentSlice;
  int dP_nr = assignSE2partition[currSlice->dp_mode][SE_HEADER];
  DataPartition *partition = &(currSlice->partArr[dP_nr]);
  Bitstream *currStream = partition->bitstream;
  int luma_weight_flag_l0, luma_weight_flag_l1, chroma_weight_flag_l0, chroma_weight_flag_l1;
  int i,j;

  img->luma_log2_weight_denom = ue_v ("SH: luma_log2_weight_denom", currStream);
  img->wp_round_luma = img->luma_log2_weight_denom ? 1<<(img->luma_log2_weight_denom - 1): 0;

  if ( 0 != active_sps->chroma_format_idc)
  {
    img->chroma_log2_weight_denom = ue_v ("SH: chroma_log2_weight_denom", currStream);
    img->wp_round_chroma = img->chroma_log2_weight_denom ? 1<<(img->chroma_log2_weight_denom - 1): 0;
  }

  reset_wp_params(img);

  for (i=0; i<img->num_ref_idx_l0_active; i++)
  {
    luma_weight_flag_l0 = u_1("SH: luma_weight_flag_l0", currStream);

    if (luma_weight_flag_l0)
    {
      img->wp_weight[0][i][0] = se_v ("SH: luma_weight_l0", currStream);
      img->wp_offset[0][i][0] = se_v ("SH: luma_offset_l0", currStream);
    }
    else
    {
      img->wp_weight[0][i][0] = 1<<img->luma_log2_weight_denom;
      img->wp_offset[0][i][0] = 0;
    }

    if (active_sps->chroma_format_idc != 0)
    {
      chroma_weight_flag_l0 = u_1 ("SH: chroma_weight_flag_l0", currStream);

      for (j=1; j<3; j++)
      {
        if (chroma_weight_flag_l0)
        {
          img->wp_weight[0][i][j] = se_v("SH: chroma_weight_l0", currStream);
          img->wp_offset[0][i][j] = se_v("SH: chroma_offset_l0", currStream);
        }
        else
        {
          img->wp_weight[0][i][j] = 1<<img->chroma_log2_weight_denom;
          img->wp_offset[0][i][j] = 0;
        }
      }
    }
  }
  if ((img->type == B_SLICE) && active_pps->weighted_bipred_idc == 1)
  {
    for (i=0; i<img->num_ref_idx_l1_active; i++)
    {
      luma_weight_flag_l1 = u_1("SH: luma_weight_flag_l1", currStream);

      if (luma_weight_flag_l1)
      {
        img->wp_weight[1][i][0] = se_v ("SH: luma_weight_l1", currStream);
        img->wp_offset[1][i][0] = se_v ("SH: luma_offset_l1", currStream);
      }
      else
      {
        img->wp_weight[1][i][0] = 1<<img->luma_log2_weight_denom;
        img->wp_offset[1][i][0] = 0;
      }

      if (active_sps->chroma_format_idc != 0)
      {
        chroma_weight_flag_l1 = u_1 ("SH: chroma_weight_flag_l1", currStream);

        for (j=1; j<3; j++)
        {
          if (chroma_weight_flag_l1)
          {
            img->wp_weight[1][i][j] = se_v("SH: chroma_weight_l1", currStream);
            img->wp_offset[1][i][j] = se_v("SH: chroma_offset_l1", currStream);
          }
          else
          {
            img->wp_weight[1][i][j] = 1<<img->chroma_log2_weight_denom;
            img->wp_offset[1][i][j] = 0;
          }
        }
      }
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    read the memory control operations
 ************************************************************************
 */
void dec_ref_pic_marking(Bitstream *currStream)
{
  int val;

  DecRefPicMarking_t *tmp_drpm,*tmp_drpm2;

  // free old buffer content
  while (img->dec_ref_pic_marking_buffer)
  {
    tmp_drpm=img->dec_ref_pic_marking_buffer;

    img->dec_ref_pic_marking_buffer=tmp_drpm->Next;
    free (tmp_drpm);
  }

  if (img->idr_flag)
  {
    img->no_output_of_prior_pics_flag = u_1("SH: no_output_of_prior_pics_flag", currStream);
    img->long_term_reference_flag = u_1("SH: long_term_reference_flag", currStream);
  }
  else
  {
    img->adaptive_ref_pic_buffering_flag = u_1("SH: adaptive_ref_pic_buffering_flag", currStream);
    if (img->adaptive_ref_pic_buffering_flag)
    {
      // read Memory Management Control Operation
      do
      {
        tmp_drpm=(DecRefPicMarking_t*)calloc (1,sizeof (DecRefPicMarking_t));
        tmp_drpm->Next=NULL;

        val = tmp_drpm->memory_management_control_operation = ue_v("SH: memory_management_control_operation", currStream);

        if ((val==1)||(val==3))
        {
          tmp_drpm->difference_of_pic_nums_minus1 = ue_v("SH: difference_of_pic_nums_minus1", currStream);
        }
        if (val==2)
        {
          tmp_drpm->long_term_pic_num = ue_v("SH: long_term_pic_num", currStream);
        }

        if ((val==3)||(val==6))
        {
          tmp_drpm->long_term_frame_idx = ue_v("SH: long_term_frame_idx", currStream);
        }
        if (val==4)
        {
          tmp_drpm->max_long_term_frame_idx_plus1 = ue_v("SH: max_long_term_pic_idx_plus1", currStream);
        }

        // add command
        if (img->dec_ref_pic_marking_buffer==NULL)
        {
          img->dec_ref_pic_marking_buffer=tmp_drpm;
        }
        else
        {
          tmp_drpm2=img->dec_ref_pic_marking_buffer;
          while (tmp_drpm2->Next!=NULL) tmp_drpm2=tmp_drpm2->Next;
          tmp_drpm2->Next=tmp_drpm;
        }

      }while (val != 0);

    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    To calculate the poc values
 *        based upon JVT-F100d2
 *  POC200301: Until Jan 2003, this function will calculate the correct POC
 *    values, but the management of POCs in buffered pictures may need more work.
 * \return
 *    none
 ************************************************************************
 */
void decode_poc(struct img_par *img)
{
  int i;
  // for POC mode 0:
  unsigned int MaxPicOrderCntLsb = (1<<(active_sps->log2_max_pic_order_cnt_lsb_minus4+4));

  switch ( active_sps->pic_order_cnt_type )
  {
  case 0: // POC MODE 0
    // 1st
    if(img->idr_flag)
    {
      img->PrevPicOrderCntMsb = 0;
      img->PrevPicOrderCntLsb = 0;
    }
    else
    {
      if (img->last_has_mmco_5)
      {
        if (img->last_pic_bottom_field)
        {
          img->PrevPicOrderCntMsb = 0;
          img->PrevPicOrderCntLsb = 0;
        }
        else
        {
          img->PrevPicOrderCntMsb = 0;
          img->PrevPicOrderCntLsb = img->toppoc;
        }
      }
    }
    // Calculate the MSBs of current picture
    if( img->pic_order_cnt_lsb  <  img->PrevPicOrderCntLsb  &&
      ( img->PrevPicOrderCntLsb - img->pic_order_cnt_lsb )  >=  ( MaxPicOrderCntLsb / 2 ) )
      img->PicOrderCntMsb = img->PrevPicOrderCntMsb + MaxPicOrderCntLsb;
    else if ( img->pic_order_cnt_lsb  >  img->PrevPicOrderCntLsb  &&
      ( img->pic_order_cnt_lsb - img->PrevPicOrderCntLsb )  >  ( MaxPicOrderCntLsb / 2 ) )
      img->PicOrderCntMsb = img->PrevPicOrderCntMsb - MaxPicOrderCntLsb;
    else
      img->PicOrderCntMsb = img->PrevPicOrderCntMsb;

    // 2nd

    if(img->field_pic_flag==0)
    {           //frame pix
      img->toppoc = img->PicOrderCntMsb + img->pic_order_cnt_lsb;
      img->bottompoc = img->toppoc + img->delta_pic_order_cnt_bottom;
      img->ThisPOC = img->framepoc = (img->toppoc < img->bottompoc)? img->toppoc : img->bottompoc; // POC200301
    }
    else if (img->bottom_field_flag==0)
    {  //top field
      img->ThisPOC= img->toppoc = img->PicOrderCntMsb + img->pic_order_cnt_lsb;
      }
      else
    {  //bottom field
      img->ThisPOC= img->bottompoc = img->PicOrderCntMsb + img->pic_order_cnt_lsb;
    }
    img->framepoc=img->ThisPOC;

    if ( img->frame_num!=img->PreviousFrameNum)
      img->PreviousFrameNum=img->frame_num;

    if(img->nal_reference_idc)
    {
      img->PrevPicOrderCntLsb = img->pic_order_cnt_lsb;
      img->PrevPicOrderCntMsb = img->PicOrderCntMsb;
    }

    break;

  case 1: // POC MODE 1
    // 1st
    if(img->idr_flag)
    {
      img->FrameNumOffset=0;     //  first pix of IDRGOP,
      img->delta_pic_order_cnt[0]=0;                        //ignore first delta
      if(img->frame_num)
        error("frame_num not equal to zero in IDR picture", -1020);
    }
    else
    {
      if (img->last_has_mmco_5)
      {
        img->PreviousFrameNumOffset = 0;
        img->PreviousFrameNum = 0;
      }
      if (img->frame_num<img->PreviousFrameNum)
      {             //not first pix of IDRGOP
        img->FrameNumOffset = img->PreviousFrameNumOffset + img->MaxFrameNum;
      }
      else
      {
        img->FrameNumOffset = img->PreviousFrameNumOffset;
      }
    }

    // 2nd
    if(active_sps->num_ref_frames_in_pic_order_cnt_cycle)
      img->AbsFrameNum = img->FrameNumOffset+img->frame_num;
    else
      img->AbsFrameNum=0;
    if( (!img->nal_reference_idc) && img->AbsFrameNum>0)
      img->AbsFrameNum--;

    // 3rd
    img->ExpectedDeltaPerPicOrderCntCycle=0;

    if(active_sps->num_ref_frames_in_pic_order_cnt_cycle)
    for(i=0;i<(int) active_sps->num_ref_frames_in_pic_order_cnt_cycle;i++)
      img->ExpectedDeltaPerPicOrderCntCycle += active_sps->offset_for_ref_frame[i];

    if(img->AbsFrameNum)
    {
      img->PicOrderCntCycleCnt = (img->AbsFrameNum-1)/active_sps->num_ref_frames_in_pic_order_cnt_cycle;
      img->FrameNumInPicOrderCntCycle = (img->AbsFrameNum-1)%active_sps->num_ref_frames_in_pic_order_cnt_cycle;
      img->ExpectedPicOrderCnt = img->PicOrderCntCycleCnt*img->ExpectedDeltaPerPicOrderCntCycle;
      for(i=0;i<=(int)img->FrameNumInPicOrderCntCycle;i++)
        img->ExpectedPicOrderCnt += active_sps->offset_for_ref_frame[i];
    }
    else
      img->ExpectedPicOrderCnt=0;

    if(!img->nal_reference_idc)
      img->ExpectedPicOrderCnt += active_sps->offset_for_non_ref_pic;

    if(img->field_pic_flag==0)
    {           //frame pix
      img->toppoc = img->ExpectedPicOrderCnt + img->delta_pic_order_cnt[0];
      img->bottompoc = img->toppoc + active_sps->offset_for_top_to_bottom_field + img->delta_pic_order_cnt[1];
      img->ThisPOC = img->framepoc = (img->toppoc < img->bottompoc)? img->toppoc : img->bottompoc; // POC200301
    }
    else if (img->bottom_field_flag==0)
    {  //top field
      img->ThisPOC = img->toppoc = img->ExpectedPicOrderCnt + img->delta_pic_order_cnt[0];
    }
    else
    {  //bottom field
      img->ThisPOC = img->bottompoc = img->ExpectedPicOrderCnt + active_sps->offset_for_top_to_bottom_field + img->delta_pic_order_cnt[0];
    }
    img->framepoc=img->ThisPOC;

    img->PreviousFrameNum=img->frame_num;
    img->PreviousFrameNumOffset=img->FrameNumOffset;

    break;


  case 2: // POC MODE 2
    if(img->idr_flag) // IDR picture
    {
      img->FrameNumOffset=0;     //  first pix of IDRGOP,
      img->ThisPOC = img->framepoc = img->toppoc = img->bottompoc = 0;
      if(img->frame_num)
        error("frame_num not equal to zero in IDR picture", -1020);
    }
    else
    {
      if (img->last_has_mmco_5)
      {
        img->PreviousFrameNum = 0;
        img->PreviousFrameNumOffset = 0;
      }
      if (img->frame_num<img->PreviousFrameNum)
        img->FrameNumOffset = img->PreviousFrameNumOffset + img->MaxFrameNum;
      else
        img->FrameNumOffset = img->PreviousFrameNumOffset;


      img->AbsFrameNum = img->FrameNumOffset+img->frame_num;
      if(!img->nal_reference_idc)
        img->ThisPOC = (2*img->AbsFrameNum - 1);
      else
        img->ThisPOC = (2*img->AbsFrameNum);

      if (img->field_pic_flag==0)
        img->toppoc = img->bottompoc = img->framepoc = img->ThisPOC;
      else if (img->bottom_field_flag==0)
         img->toppoc = img->framepoc = img->ThisPOC;
      else img->bottompoc = img->framepoc = img->ThisPOC;
    }

    img->PreviousFrameNum=img->frame_num;
    img->PreviousFrameNumOffset=img->FrameNumOffset;
    break;


  default:
    //error must occurs
    assert( 1==0 );
    break;
  }
}

/*!
 ************************************************************************
 * \brief
 *    A little helper for the debugging of POC code
 * \return
 *    none
 ************************************************************************
 */
int dumppoc(struct img_par *img) {
    printf ("\nPOC locals...\n");
    printf ("toppoc                                %d\n", img->toppoc);
    printf ("bottompoc                             %d\n", img->bottompoc);
    printf ("frame_num                             %d\n", img->frame_num);
    printf ("field_pic_flag                        %d\n", img->field_pic_flag);
    printf ("bottom_field_flag                     %d\n", img->bottom_field_flag);
    printf ("POC SPS\n");
    printf ("log2_max_frame_num_minus4             %d\n", active_sps->log2_max_frame_num_minus4);         // POC200301
    printf ("log2_max_pic_order_cnt_lsb_minus4     %d\n", active_sps->log2_max_pic_order_cnt_lsb_minus4);
    printf ("pic_order_cnt_type                    %d\n", active_sps->pic_order_cnt_type);
    printf ("num_ref_frames_in_pic_order_cnt_cycle %d\n", active_sps->num_ref_frames_in_pic_order_cnt_cycle);
    printf ("delta_pic_order_always_zero_flag      %d\n", active_sps->delta_pic_order_always_zero_flag);
    printf ("offset_for_non_ref_pic                %d\n", active_sps->offset_for_non_ref_pic);
    printf ("offset_for_top_to_bottom_field        %d\n", active_sps->offset_for_top_to_bottom_field);
    printf ("offset_for_ref_frame[0]               %d\n", active_sps->offset_for_ref_frame[0]);
    printf ("offset_for_ref_frame[1]               %d\n", active_sps->offset_for_ref_frame[1]);
    printf ("POC in SLice Header\n");
    printf ("pic_order_present_flag                %d\n", active_pps->pic_order_present_flag);
    printf ("delta_pic_order_cnt[0]                %d\n", img->delta_pic_order_cnt[0]);
    printf ("delta_pic_order_cnt[1]                %d\n", img->delta_pic_order_cnt[1]);
    printf ("delta_pic_order_cnt[2]                %d\n", img->delta_pic_order_cnt[2]);
    printf ("idr_flag                              %d\n", img->idr_flag);
    printf ("MaxFrameNum                           %d\n", img->MaxFrameNum);

    return 0;
}

/*!
 ************************************************************************
 * \brief
 *    return the poc of img as per (8-1) JVT-F100d2
 *  POC200301
 ************************************************************************
 */
int picture_order(struct img_par *img)
{
  if (img->field_pic_flag==0) // is a frame
    return img->framepoc;
  else if (img->bottom_field_flag==0) // top field
    return img->toppoc;
  else // bottom field
    return img->bottompoc;
}

