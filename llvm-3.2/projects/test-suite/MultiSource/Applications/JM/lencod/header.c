
/*!
 *************************************************************************************
 * \file header.c
 *
 * \brief
 *    H.264 Slice and Sequence headers
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger                  <stewe@cs.tu-berlin.de>
 *      - Karsten Suehring                <suehring@hhi.de>
 *************************************************************************************
 */

#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"

#include "elements.h"
#include "header.h"
#include "rtp.h"
#include "mbuffer.h"
#include "defines.h"
#include "vlc.h"
#include "parset.h"

// A little trick to avoid those horrible #if TRACE all over the source code
#if TRACE
#define SYMTRACESTRING(s) strncpy(sym.tracestring,s,TRACESTRING_SIZE)
#else
#define SYMTRACESTRING(s) // do nothing
#endif

int * assignSE2partition[2] ;
int assignSE2partition_NoDP[SE_MAX_ELEMENTS] =
  {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int assignSE2partition_DP[SE_MAX_ELEMENTS] =
  // 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
  {  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0 } ;

static int ref_pic_list_reordering(Bitstream *bitstream);
static int dec_ref_pic_marking    (Bitstream *bitstream);
static int pred_weight_table      (Bitstream *bitstream);

/*!
 ********************************************************************************************
 * \brief
 *    Write a slice header
 *
 * \return
 *    number of bits used
 ********************************************************************************************
*/
int SliceHeader()
{
  int dP_nr = assignSE2partition[input->partition_mode][SE_HEADER];
  Bitstream *bitstream = img->currentSlice->partArr[dP_nr].bitstream;
  Slice* currSlice = img->currentSlice;
  int len = 0;
  unsigned int field_pic_flag = 0, bottom_field_flag = 0;

  int num_bits_slice_group_change_cycle;
  float numtmp;

  if (img->MbaffFrameFlag)
    len  = ue_v("SH: first_mb_in_slice", img->current_mb_nr >> 1,   bitstream);
  else
    len  = ue_v("SH: first_mb_in_slice", img->current_mb_nr,   bitstream);

  len += ue_v("SH: slice_type",        get_picture_type (),   bitstream);

  len += ue_v("SH: pic_parameter_set_id" , active_pps->pic_parameter_set_id ,bitstream);

  len += u_v (log2_max_frame_num_minus4 + 4,"SH: frame_num", img->frame_num, bitstream);

  if (!active_sps->frame_mbs_only_flag)
  {
    // field_pic_flag    u(1)
    field_pic_flag = (img->structure ==TOP_FIELD || img->structure ==BOTTOM_FIELD)?1:0;
    assert( field_pic_flag == img->fld_flag );
    len += u_1("SH: field_pic_flag", field_pic_flag, bitstream);

    if (field_pic_flag)
    {
      //bottom_field_flag     u(1)
      bottom_field_flag = (img->structure == BOTTOM_FIELD)?1:0;
      len += u_1("SH: bottom_field_flag" , bottom_field_flag ,bitstream);
    }
  }

  if (img->currentPicture->idr_flag)
  {
    // idr_pic_id
    len += ue_v ("SH: idr_pic_id", (img->number % 2), bitstream);
  }

  if (img->pic_order_cnt_type == 0)
  {
    if (active_sps->frame_mbs_only_flag)
    {
      img->pic_order_cnt_lsb = (img->toppoc & ~((((unsigned int)(-1)) << (log2_max_pic_order_cnt_lsb_minus4+4))) );
    }
    else
    {
      if (!field_pic_flag || img->structure == TOP_FIELD)
        img->pic_order_cnt_lsb = (img->toppoc & ~((((unsigned int)(-1)) << (log2_max_pic_order_cnt_lsb_minus4+4))) );
      else if ( img->structure == BOTTOM_FIELD )
        img->pic_order_cnt_lsb = (img->bottompoc & ~((((unsigned int)(-1)) << (log2_max_pic_order_cnt_lsb_minus4+4))) );
    }

    len += u_v (log2_max_pic_order_cnt_lsb_minus4+4, "SH: pic_order_cnt_lsb", img->pic_order_cnt_lsb, bitstream);

    if (img->pic_order_present_flag && !field_pic_flag)
    {
      len += se_v ("SH: delta_pic_order_cnt_bottom", img->delta_pic_order_cnt_bottom, bitstream);
    }
  }
  if (img->pic_order_cnt_type == 1 && !img->delta_pic_order_always_zero_flag)
  {
    len += se_v ("SH: delta_pic_order_cnt[0]", img->delta_pic_order_cnt[0], bitstream);

    if (img->pic_order_present_flag && !field_pic_flag)
    {
      len += se_v ("SH: delta_pic_order_cnt[1]", img->delta_pic_order_cnt[1], bitstream);
    }
  }

  if (active_pps->redundant_pic_cnt_present_flag)
  {
    len += ue_v ("SH: redundant_pic_cnt", img->redundant_pic_cnt, bitstream);
  }

  // Direct Mode Type selection for B pictures
  if (img->type==B_SLICE)
  {
    len +=  u_1 ("SH: direct_spatial_mv_pred_flag", img->direct_spatial_mv_pred_flag, bitstream);
  }

  if ((img->type == P_SLICE) || (img->type == B_SLICE) || (img->type==SP_SLICE))
  {
    int override_flag;
    if ((img->type == P_SLICE) || (img->type==SP_SLICE))
    {
      override_flag = (img->num_ref_idx_l0_active != (active_pps->num_ref_idx_l0_active_minus1 +1)) ? 1 : 0;
    }
    else
    {
      override_flag = ((img->num_ref_idx_l0_active != (active_pps->num_ref_idx_l0_active_minus1 +1))
                      || (img->num_ref_idx_l1_active != (active_pps->num_ref_idx_l1_active_minus1 +1))) ? 1 : 0;
    }

    len +=  u_1 ("SH: num_ref_idx_active_override_flag", override_flag, bitstream);

    if (override_flag)
    {
      len += ue_v ("SH: num_ref_idx_l0_active_minus1", img->num_ref_idx_l0_active-1, bitstream);
      if (img->type==B_SLICE)
      {
        len += ue_v ("SH: num_ref_idx_l1_active_minus1", img->num_ref_idx_l1_active-1, bitstream);
      }
    }

  }
  len += ref_pic_list_reordering(bitstream);

  if (((img->type == P_SLICE || img->type == SP_SLICE) && active_pps->weighted_pred_flag) ||
     ((img->type == B_SLICE) && active_pps->weighted_bipred_idc == 1))
  {
    len += pred_weight_table(bitstream);
  }

  if (img->nal_reference_idc)
    len += dec_ref_pic_marking(bitstream);

  if(input->symbol_mode==CABAC && img->type!=I_SLICE /*&& img->type!=SI_IMG*/)
  {
    len += ue_v("SH: cabac_init_idc", img->model_number, bitstream);
  }

  len += se_v("SH: slice_qp_delta", (currSlice->qp - 26 - active_pps->pic_init_qp_minus26), bitstream);

  if (img->type==SP_SLICE /*|| img->type==SI_SLICE*/)
  {
    if (img->type==SP_SLICE) // Switch Flag only for SP pictures
    {
      len += u_1 ("SH: sp_for_switch_flag", (si_frame_indicator || sp2_frame_indicator), bitstream);   // 1 for switching SP, 0 for normal SP
    }
    len += se_v ("SH: slice_qs_delta", (img->qpsp - 26), bitstream );
  }

  if (active_pps->deblocking_filter_control_present_flag)
  {
    len += ue_v("SH: disable_deblocking_filter_idc",img->LFDisableIdc, bitstream);  // Turn loop filter on/off on slice basis

    if (img->LFDisableIdc!=1)
    {
      len += se_v ("SH: slice_alpha_c0_offset_div2", img->LFAlphaC0Offset / 2, bitstream);

      len += se_v ("SH: slice_beta_offset_div2", img->LFBetaOffset / 2, bitstream);
    }
  }


  if ( active_pps->num_slice_groups_minus1>0 &&
    active_pps->slice_group_map_type>=3 && active_pps->slice_group_map_type<=5)
  {
    numtmp=img->PicHeightInMapUnits*img->PicWidthInMbs/(float)(active_pps->slice_group_change_rate_minus1+1)+1;
    num_bits_slice_group_change_cycle = (int)ceil(log(numtmp)/log(2));

    //! img->slice_group_change_cycle can be changed before calling FmoInit()
    len += u_v (num_bits_slice_group_change_cycle, "SH: slice_group_change_cycle", img->slice_group_change_cycle, bitstream);
  }

  // NOTE: The following syntax element is actually part
  //        Slice data bitstream A RBSP syntax

  if(input->partition_mode&&!img->currentPicture->idr_flag)
  {
    len += ue_v("DPA: slice_id", img->current_slice_nr, bitstream);
  }

  return len;
}

/*!
 ********************************************************************************************
 * \brief
 *    writes the ref_pic_list_reordering syntax
 *    based on content of according fields in img structure
 *
 * \return
 *    number of bits used
 ********************************************************************************************
*/
static int ref_pic_list_reordering(Bitstream *bitstream)
{
  Slice *currSlice = img->currentSlice;

  int i, len=0;

  // RPLR for redundant pictures
  if(input->redundant_pic_flag && redundant_coding)
  {
    currSlice->ref_pic_list_reordering_flag_l0 = 1;
    currSlice->reordering_of_pic_nums_idc_l0[0] = 0;
    currSlice->reordering_of_pic_nums_idc_l0[1] = 3;
    currSlice->abs_diff_pic_num_minus1_l0[0] = redundant_ref_idx - 1;
    currSlice->long_term_pic_idx_l0[0] = 0;
    reorder_ref_pic_list( listX[LIST_0], &listXsize[LIST_0],
                          img->num_ref_idx_l0_active-1,
                          currSlice->reordering_of_pic_nums_idc_l0,
                          currSlice->abs_diff_pic_num_minus1_l0,
                          currSlice->long_term_pic_idx_l0);
  }

  if ((img->type!=I_SLICE) /*&&(img->type!=SI_IMG)*/ )
  {
    len += u_1 ("SH: ref_pic_list_reordering_flag_l0", currSlice->ref_pic_list_reordering_flag_l0, bitstream);
    if (currSlice->ref_pic_list_reordering_flag_l0)
    {
      i=-1;
      do
      {
        i++;
        len += ue_v ("SH: reordering_of_pic_nums_idc", currSlice->reordering_of_pic_nums_idc_l0[i], bitstream);
        if (currSlice->reordering_of_pic_nums_idc_l0[i]==0 ||
            currSlice->reordering_of_pic_nums_idc_l0[i]==1)
        {
          len += ue_v ("SH: abs_diff_pic_num_minus1_l0", currSlice->abs_diff_pic_num_minus1_l0[i], bitstream);
        }
        else
        {
          if (currSlice->reordering_of_pic_nums_idc_l0[i]==2)
          {
            len += ue_v ("SH: long_term_pic_idx_l0", currSlice->long_term_pic_idx_l0[i], bitstream);
          }
        }

      } while (currSlice->reordering_of_pic_nums_idc_l0[i] != 3);
    }
  }

  if (img->type==B_SLICE)
  {
    len += u_1 ("SH: ref_pic_list_reordering_flag_l1", currSlice->ref_pic_list_reordering_flag_l1, bitstream);
    if (currSlice->ref_pic_list_reordering_flag_l1)
    {
      i=-1;
      do
      {
        i++;
        len += ue_v ("SH: remapping_of_pic_num_idc", currSlice->reordering_of_pic_nums_idc_l1[i], bitstream);
        if (currSlice->reordering_of_pic_nums_idc_l1[i]==0 ||
            currSlice->reordering_of_pic_nums_idc_l1[i]==1)
        {
          len += ue_v ("SH: abs_diff_pic_num_minus1_l1", currSlice->abs_diff_pic_num_minus1_l1[i], bitstream);
        }
        else
        {
          if (currSlice->reordering_of_pic_nums_idc_l1[i]==2)
          {
            len += ue_v ("SH: long_term_pic_idx_l1", currSlice->long_term_pic_idx_l1[i], bitstream);
          }
        }
      } while (currSlice->reordering_of_pic_nums_idc_l1[i] != 3);
    }
  }

  return len;
}


/*!
 ************************************************************************
 * \brief
 *    write the memory management control operations
 *
 * \return
 *    number of bits used
 ************************************************************************
 */
static int dec_ref_pic_marking(Bitstream *bitstream)
{
  DecRefPicMarking_t *tmp_drpm;

  int val, len=0;

  if (img->currentPicture->idr_flag)
  {
    len += u_1("SH: no_output_of_prior_pics_flag", img->no_output_of_prior_pics_flag, bitstream);
    len += u_1("SH: long_term_reference_flag", img->long_term_reference_flag, bitstream);
  }
  else
  {
    img->adaptive_ref_pic_buffering_flag = (img->dec_ref_pic_marking_buffer!=NULL);

    len += u_1("SH: adaptive_ref_pic_buffering_flag", img->adaptive_ref_pic_buffering_flag, bitstream);

    if (img->adaptive_ref_pic_buffering_flag)
    {
      tmp_drpm = img->dec_ref_pic_marking_buffer;
      // write Memory Management Control Operation
      do
      {
        if (tmp_drpm==NULL) error ("Error encoding MMCO commands", 500);

        val = tmp_drpm->memory_management_control_operation;
        len += ue_v("SH: memory_management_control_operation", val, bitstream);

        if ((val==1)||(val==3))
        {
          len += 1 + ue_v("SH: difference_of_pic_nums_minus1", tmp_drpm->difference_of_pic_nums_minus1, bitstream);
        }
        if (val==2)
        {
          len+= ue_v("SH: long_term_pic_num", tmp_drpm->long_term_pic_num, bitstream);
        }
        if ((val==3)||(val==6))
        {
          len+= ue_v("SH: long_term_frame_idx", tmp_drpm->long_term_frame_idx, bitstream);
        }
        if (val==4)
        {
          len += ue_v("SH: max_long_term_pic_idx_plus1", tmp_drpm->max_long_term_frame_idx_plus1, bitstream);
        }

        tmp_drpm=tmp_drpm->Next;

      } while (val != 0);

    }
  }
  return len;
}


/*!
 ************************************************************************
 * \brief
 *    write prediction weight table
 *
 * \return
 *    number of bits used
 ************************************************************************
 */
static int pred_weight_table(Bitstream *bitstream)
{
  int len = 0;
  int i,j;

  len += ue_v("SH: luma_log_weight_denom", luma_log_weight_denom, bitstream);

  if ( 0 != active_sps->chroma_format_idc)
  {
    len += ue_v("SH: chroma_log_weight_denom", chroma_log_weight_denom, bitstream);
  }

  for (i=0; i< img->num_ref_idx_l0_active; i++)
  {
    if ( (wp_weight[0][i][0] != 1<<luma_log_weight_denom) || (wp_offset[0][i][0] != 0) )
    {
      len += u_1 ("SH: luma_weight_flag_l0", 1, bitstream);

      len += se_v ("SH: luma_weight_l0", wp_weight[0][i][0], bitstream);

      len += se_v ("SH: luma_offset_l0", wp_offset[0][i][0], bitstream);
    }
    else
    {
        len += u_1 ("SH: luma_weight_flag_l0", 0, bitstream);
    }

    if (active_sps->chroma_format_idc!=0)
    {
      if ( (wp_weight[0][i][1] != 1<<chroma_log_weight_denom) || (wp_offset[0][i][1] != 0) ||
        (wp_weight[0][i][2] != 1<<chroma_log_weight_denom) || (wp_offset[0][i][2] != 0)  )
      {
        len += u_1 ("chroma_weight_flag_l0", 1, bitstream);
        for (j=1; j<3; j++)
        {
          len += se_v ("chroma_weight_l0", wp_weight[0][i][j] ,bitstream);

          len += se_v ("chroma_offset_l0", wp_offset[0][i][j] ,bitstream);
        }
      }
      else
      {
        len += u_1 ("chroma_weight_flag_l0", 0, bitstream);
      }
    }
  }

  if (img->type == B_SLICE)
  {
    for (i=0; i< img->num_ref_idx_l1_active; i++)
    {
      if ( (wp_weight[1][i][0] != 1<<luma_log_weight_denom) || (wp_offset[1][i][0] != 0) )
      {
        len += u_1 ("SH: luma_weight_flag_l1", 1, bitstream);

        len += se_v ("SH: luma_weight_l1", wp_weight[1][i][0], bitstream);

        len += se_v ("SH: luma_offset_l1", wp_offset[1][i][0], bitstream);
      }
      else
      {
        len += u_1 ("SH: luma_weight_flag_l1", 0, bitstream);
      }

      if (active_sps->chroma_format_idc!=0)
      {
        if ( (wp_weight[1][i][1] != 1<<chroma_log_weight_denom) || (wp_offset[1][i][1] != 0) ||
          (wp_weight[1][i][2] != 1<<chroma_log_weight_denom) || (wp_offset[1][i][2] != 0) )
        {
          len += u_1 ("chroma_weight_flag_l1", 1, bitstream);
          for (j=1; j<3; j++)
          {
            len += se_v ("chroma_weight_l1", wp_weight[1][i][j] ,bitstream);
            len += se_v ("chroma_offset_l1", wp_offset[1][i][j] ,bitstream);
          }
        }
        else
        {
          len += u_1 ("chroma_weight_flag_l1", 0, bitstream);
        }
      }
    }
  }
  return len;
}


/*!
 ************************************************************************
 * \brief
 *    Selects picture type and codes it to symbol
 *
 * \return
 *    symbol value for picture type
 ************************************************************************
 */
int get_picture_type()
{
  // set this value to zero for transmission without signaling
  // that the whole picture has the same slice type
  int same_slicetype_for_whole_frame = 5;

  switch (img->type)
  {
  case I_SLICE:
    return 2 + same_slicetype_for_whole_frame;
    break;
  case P_SLICE:
    return 0 + same_slicetype_for_whole_frame;
    break;
  case B_SLICE:
    return 1 + same_slicetype_for_whole_frame;
    break;
  case SP_SLICE:
    return 3 + same_slicetype_for_whole_frame;
    break;
  default:
    error("Picture Type not supported!",1);
    break;
  }

  return 0;
}



/*!
 *****************************************************************************
 *
 * \brief
 *    int Partition_BC_Header () write the Partition type B, C header
 *
 * \return
 *    Number of bits used by the partition header
 *
 * \par Parameters
 *    PartNo: Partition Number to which the header should be written
 *
 * \par Side effects
 *    Partition header as per VCEG-N72r2 is written into the appropriate
 *    partition bit buffer
 *
 * \par Limitations/Shortcomings/Tweaks
 *    The current code does not support the change of picture parameters within
 *    one coded sequence, hence there is only one parameter set necessary.  This
 *    is hard coded to zero.
 *
 * \date
 *    October 24, 2001
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/
int Partition_BC_Header(int PartNo)
{
  DataPartition *partition = &((img->currentSlice)->partArr[PartNo]);
  SyntaxElement sym;

  assert (PartNo > 0 && PartNo < img->currentSlice->max_part_nr);

  sym.type = SE_HEADER;         // This will be true for all symbols generated here
  sym.value2  = 0;

  SYMTRACESTRING("RTP-PH: Slice ID");
  sym.value1 = img->current_slice_nr;
  writeSE_UVLC (&sym, partition);

  return sym.len;
}

