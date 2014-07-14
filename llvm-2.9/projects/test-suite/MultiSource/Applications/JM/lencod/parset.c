
/*!
 **************************************************************************************
 * \file
 *    parset.c
 * \brief
 *    Picture and Sequence Parameter set generation and handling
 *  \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 *
 **************************************************************************************
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

#include "global.h"

#include "contributors.h"
#include "mbuffer.h"
#include "parset.h"
#include "vlc.h"

// Local helpers
static int IdentifyProfile(void);
static int IdentifyLevel(void);
static int GenerateVUISequenceParameters(Bitstream *bitstream);

extern ColocatedParams *Co_located;

pic_parameter_set_rbsp_t *PicParSet[MAXPPS];

static const byte ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

static const byte ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
   12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
   35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
   58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};


/*!
 *************************************************************************************
 * \brief
 *    generates a sequence and picture parameter set and stores these in global
 *    active_sps and active_pps
 *
 * \return
 *    A NALU containing the Sequence ParameterSet
 *
 *************************************************************************************
*/
void GenerateParameterSets (void)
{
  int i;
  seq_parameter_set_rbsp_t *sps = NULL;

  sps = AllocSPS();

  for (i=0; i<MAXPPS; i++)
  {
    PicParSet[i] = NULL;
  }


  GenerateSequenceParameterSet(sps, 0);

  if (input->GenerateMultiplePPS)
  {
    PicParSet[0] = AllocPPS();
    PicParSet[1] = AllocPPS();
    PicParSet[2] = AllocPPS();

    if (sps->profile_idc >= FREXT_HP)
    {
      GeneratePictureParameterSet( PicParSet[0], sps, 0, 0, 0, input->cb_qp_index_offset, input->cr_qp_index_offset);
      GeneratePictureParameterSet( PicParSet[1], sps, 1, 1, 1, input->cb_qp_index_offset, input->cr_qp_index_offset);
      GeneratePictureParameterSet( PicParSet[2], sps, 2, 1, 2, input->cb_qp_index_offset, input->cr_qp_index_offset);

    }
    else
    {
      GeneratePictureParameterSet( PicParSet[0], sps, 0, 0, 0, input->chroma_qp_index_offset, 0);
      GeneratePictureParameterSet( PicParSet[1], sps, 1, 1, 1, input->chroma_qp_index_offset, 0);
      GeneratePictureParameterSet( PicParSet[2], sps, 2, 1, 2, input->chroma_qp_index_offset, 0);
    }
  }
  else
  {
    PicParSet[0] = AllocPPS();
    if (sps->profile_idc >= FREXT_HP)
      GeneratePictureParameterSet( PicParSet[0], sps, 0, input->WeightedPrediction, input->WeightedBiprediction,
                                   input->cb_qp_index_offset, input->cr_qp_index_offset);
    else
      GeneratePictureParameterSet( PicParSet[0], sps, 0, input->WeightedPrediction, input->WeightedBiprediction,
                                   input->chroma_qp_index_offset, 0);

  }

  active_sps = sps;
  active_pps = PicParSet[0];
}

/*!
*************************************************************************************
* \brief
*    frees global parameter sets active_sps and active_pps
*
* \return
*    A NALU containing the Sequence ParameterSet
*
*************************************************************************************
*/
void FreeParameterSets (void)
{
  int i;
  for (i=0; i<MAXPPS; i++)
  {
    if ( NULL != PicParSet[i])
    {
      FreePPS(PicParSet[i]);
      PicParSet[i] = NULL;
    }
  }
  FreeSPS (active_sps);
}

/*!
*************************************************************************************
* \brief
*    int GenerateSeq_parameter_set_NALU (void);
*
* \note
*    Uses the global variables through GenerateSequenceParameterSet()
*    and GeneratePictureParameterSet
*
* \return
*    A NALU containing the Sequence ParameterSet
*
*************************************************************************************
*/

NALU_t *GenerateSeq_parameter_set_NALU (void)
{
  NALU_t *n = AllocNALU(64000);
  int RBSPlen = 0;
  int NALUlen;
  byte rbsp[MAXRBSPSIZE];

  RBSPlen = GenerateSeq_parameter_set_rbsp (active_sps, rbsp);
  NALUlen = RBSPtoNALU (rbsp, n, RBSPlen, NALU_TYPE_SPS, NALU_PRIORITY_HIGHEST, 0, 1);
  n->startcodeprefix_len = 4;

  return n;
}


/*!
*************************************************************************************
* \brief
*    NALU_t *GeneratePic_parameter_set_NALU (int PPS_id);
*
* \note
*    Uses the global variables through GenerateSequenceParameterSet()
*    and GeneratePictureParameterSet
*
* \return
*    A NALU containing the Picture Parameter Set
*
*************************************************************************************
*/

NALU_t *GeneratePic_parameter_set_NALU(int PPS_id)
{
  NALU_t *n = AllocNALU(64000);
  int RBSPlen = 0;
  int NALUlen;
  byte rbsp[MAXRBSPSIZE];

  RBSPlen = GeneratePic_parameter_set_rbsp (PicParSet[PPS_id], rbsp);
  NALUlen = RBSPtoNALU (rbsp, n, RBSPlen, NALU_TYPE_PPS, NALU_PRIORITY_HIGHEST, 0, 1);
  n->startcodeprefix_len = 4;

  return n;
}


/*!
 ************************************************************************
 * \brief
 *    GenerateSequenceParameterSet: extracts info from global variables and
 *    generates sequence parameter set structure
 *
 * \par
 *    Function reads all kinds of values from several global variables,
 *    including input-> and image-> and fills in the sps.  Many
 *    values are current hard-coded to defaults.
 *
 ************************************************************************
 */

void GenerateSequenceParameterSet( seq_parameter_set_rbsp_t *sps, //!< Sequence Parameter Set to be filled
                                   int SPS_id                     //!< SPS ID
                                   )
{
  unsigned i;
  int SubWidthC  [4]= { 1, 2, 2, 1};
  int SubHeightC [4]= { 1, 2, 1, 1};

  int frext_profile = ((IdentifyProfile()==FREXT_HP) ||
                      (IdentifyProfile()==FREXT_Hi10P) ||
                      (IdentifyProfile()==FREXT_Hi422) ||
                      (IdentifyProfile()==FREXT_Hi444));

  // *************************************************************************
  // Sequence Parameter Set
  // *************************************************************************
  assert (sps != NULL);
  // Profile and Level should be calculated using the info from the config
  // file.  Calculation is hidden in IndetifyProfile() and IdentifyLevel()
  sps->profile_idc = IdentifyProfile();
  sps->level_idc = IdentifyLevel();

  // needs to be set according to profile
  sps->constrained_set0_flag = FALSE;
  sps->constrained_set1_flag = FALSE;
  sps->constrained_set2_flag = FALSE;

  if ( (sps->level_idc == 9) && (sps->profile_idc < FREXT_HP) ) // Level 1.b
  {
    sps->constrained_set3_flag = TRUE;
    sps->level_idc = 11;
  }
  else
  {
    sps->constrained_set3_flag = FALSE;
  }

  // Parameter Set ID hard coded to zero
  sps->seq_parameter_set_id = 0;

  // Fidelity Range Extensions stuff
  sps->bit_depth_luma_minus8   = input->BitDepthLuma - 8;
  sps->bit_depth_chroma_minus8 = input->BitDepthChroma - 8;
  img->lossless_qpprime_flag = input->lossless_qpprime_y_zero_flag & (sps->profile_idc==FREXT_Hi444);

  //! POC stuff:
  //! The following values are hard-coded in init_poc().  Apparently,
  //! the poc implementation covers only a subset of the poc functionality.
  //! Here, the same subset is implemented.  Changes in the POC stuff have
  //! also to be reflected here
  sps->log2_max_frame_num_minus4 = log2_max_frame_num_minus4;
  sps->log2_max_pic_order_cnt_lsb_minus4 = log2_max_pic_order_cnt_lsb_minus4;

  sps->pic_order_cnt_type = input->pic_order_cnt_type;
  sps->num_ref_frames_in_pic_order_cnt_cycle = img->num_ref_frames_in_pic_order_cnt_cycle;
  sps->delta_pic_order_always_zero_flag = img->delta_pic_order_always_zero_flag;
  sps->offset_for_non_ref_pic = img->offset_for_non_ref_pic;
  sps->offset_for_top_to_bottom_field = img->offset_for_top_to_bottom_field;

  for (i=0; i<img->num_ref_frames_in_pic_order_cnt_cycle; i++)
  {
    sps->offset_for_ref_frame[i] = img->offset_for_ref_frame[i];
  }
  // End of POC stuff

  // Number of Reference Frames
  sps->num_ref_frames = input->num_ref_frames;

  //required_frame_num_update_behaviour_flag hardcoded to zero
  sps->gaps_in_frame_num_value_allowed_flag = FALSE;    // double check

  sps->frame_mbs_only_flag = (Boolean) !(input->PicInterlace || input->MbInterlace);

  // Picture size, finally a simple one :-)
  sps->pic_width_in_mbs_minus1 = ((input->img_width+img->auto_crop_right)/16) -1;
  sps->pic_height_in_map_units_minus1 = (((input->img_height+img->auto_crop_bottom)/16)/ (2 - sps->frame_mbs_only_flag)) - 1;

  // a couple of flags, simple
  sps->mb_adaptive_frame_field_flag = (Boolean) (FRAME_CODING != input->MbInterlace);
  sps->direct_8x8_inference_flag = (Boolean) input->directInferenceFlag;

  // Sequence VUI not implemented, signalled as not present
  sps->vui_parameters_present_flag = (Boolean) ((input->rgb_input_flag && input->yuv_format==3)|| input->Generate_SEIVUI);

  sps->chroma_format_idc = input->yuv_format;

  // This should be moved somewhere else.
  {
    int PicWidthInMbs, PicHeightInMapUnits, FrameHeightInMbs;
    int width, height;
    PicWidthInMbs = (sps->pic_width_in_mbs_minus1 +1);
    PicHeightInMapUnits = (sps->pic_height_in_map_units_minus1 +1);
    FrameHeightInMbs = ( 2 - sps->frame_mbs_only_flag ) * PicHeightInMapUnits;

    width = PicWidthInMbs * MB_BLOCK_SIZE;
    height = FrameHeightInMbs * MB_BLOCK_SIZE;

    Co_located = alloc_colocated (width, height,sps->mb_adaptive_frame_field_flag);

  }

  // Fidelity Range Extensions stuff
  if(frext_profile)
  {

    sps->seq_scaling_matrix_present_flag = (Boolean) (input->ScalingMatrixPresentFlag&1);
    for(i=0; i<8; i++)
    {
      if(i<6)
        sps->seq_scaling_list_present_flag[i] = (input->ScalingListPresentFlag[i]&1);
      else
      {
        if(input->Transform8x8Mode)
          sps->seq_scaling_list_present_flag[i] = (input->ScalingListPresentFlag[i]&1);
        else
          sps->seq_scaling_list_present_flag[i] = 0;
      }
    }
  }
  else
  {
    sps->seq_scaling_matrix_present_flag = FALSE;
    for(i=0; i<8; i++)
      sps->seq_scaling_list_present_flag[i] = 0;

  }


  if (img->auto_crop_right || img->auto_crop_bottom)
  {
    sps->frame_cropping_flag = TRUE;
    sps->frame_cropping_rect_left_offset=0;
    sps->frame_cropping_rect_top_offset=0;
    sps->frame_cropping_rect_right_offset=  (img->auto_crop_right / SubWidthC[sps->chroma_format_idc]);
    sps->frame_cropping_rect_bottom_offset= (img->auto_crop_bottom / (SubHeightC[sps->chroma_format_idc] * (2 - sps->frame_mbs_only_flag)));
    if (img->auto_crop_right % SubWidthC[sps->chroma_format_idc])
    {
      error("automatic frame cropping (width) not possible",500);
    }
    if (img->auto_crop_bottom % (SubHeightC[sps->chroma_format_idc] * (2 - sps->frame_mbs_only_flag)))
    {
      error("automatic frame cropping (height) not possible",500);
    }
  }
  else
  {
    sps->frame_cropping_flag = FALSE;
  }
}

/*!
 ************************************************************************
 * \brief
 *    GeneratePictureParameterSet:
 *    Generates a Picture Parameter Set structure
 *
 * \par
 *    Regarding the QP
 *    The previous software versions coded the absolute QP only in the
 *    slice header.  This is kept, and the offset in the PPS is coded
 *    even if we could save bits by intelligently using this field.
 *
 ************************************************************************
 */

void GeneratePictureParameterSet( pic_parameter_set_rbsp_t *pps, //!< Picture Parameter Set to be filled
                                  seq_parameter_set_rbsp_t *sps, //!< used Sequence Parameter Set
                                  int PPS_id,                    //!< PPS ID
                                  int WeightedPrediction,        //!< value of weighted_pred_flag
                                  int WeightedBiprediction,      //!< value of weighted_bipred_idc
                                  int cb_qp_index_offset,        //!< value of cb_qp_index_offset
                                  int cr_qp_index_offset         //!< value of cr_qp_index_offset
                                  )
{
  unsigned i;

  int frext_profile = ((IdentifyProfile()==FREXT_HP) ||
                      (IdentifyProfile()==FREXT_Hi10P) ||
                      (IdentifyProfile()==FREXT_Hi422) ||
                      (IdentifyProfile()==FREXT_Hi444));

  // *************************************************************************
  // Picture Parameter Set
  // *************************************************************************

  pps->seq_parameter_set_id = sps->seq_parameter_set_id;
  pps->pic_parameter_set_id = PPS_id;
  pps->entropy_coding_mode_flag = (input->symbol_mode==UVLC ? FALSE : TRUE);

  // Fidelity Range Extensions stuff
  if(frext_profile)
  {
    pps->transform_8x8_mode_flag = (input->Transform8x8Mode ? TRUE:FALSE);
    pps->pic_scaling_matrix_present_flag = (Boolean) ((input->ScalingMatrixPresentFlag&2)>>1);
    for(i=0; i<8; i++)
    {
      if(i<6)
        pps->pic_scaling_list_present_flag[i] = (input->ScalingListPresentFlag[i]&2)>>1;
      else
      {
        if(pps->transform_8x8_mode_flag)
          pps->pic_scaling_list_present_flag[i] = (input->ScalingListPresentFlag[i]&2)>>1;
        else
          pps->pic_scaling_list_present_flag[i] = 0;
      }
    }
  }
  else
  {
    pps->pic_scaling_matrix_present_flag = FALSE;
    for(i=0; i<8; i++)
      pps->pic_scaling_list_present_flag[i] = 0;

    pps->transform_8x8_mode_flag = FALSE;
    input->Transform8x8Mode = 0;
  }

  // JVT-Fxxx (by Stephan Wenger, make this flag unconditional
  pps->pic_order_present_flag = img->pic_order_present_flag;


  // Begin FMO stuff
  pps->num_slice_groups_minus1 = input->num_slice_groups_minus1;


  //! Following set the parameter for different slice group types
  if (pps->num_slice_groups_minus1 > 0)
  {
     if ((pps->slice_group_id = calloc ((sps->pic_height_in_map_units_minus1+1)*(sps->pic_width_in_mbs_minus1+1), sizeof(byte))) == NULL)
       no_mem_exit ("GeneratePictureParameterSet: slice_group_id");

    switch (input->slice_group_map_type)
    {
    case 0:
      pps->slice_group_map_type = 0;
      for(i=0; i<=pps->num_slice_groups_minus1; i++)
      {
        pps->run_length_minus1[i]=input->run_length_minus1[i];
      }
      break;
    case 1:
      pps->slice_group_map_type = 1;
      break;
    case 2:
      // i loops from 0 to num_slice_groups_minus1-1, because no info for background needed
      pps->slice_group_map_type = 2;
      for(i=0; i<pps->num_slice_groups_minus1; i++)
      {
        pps->top_left[i] = input->top_left[i];
        pps->bottom_right[i] = input->bottom_right[i];
      }
     break;
    case 3:
    case 4:
    case 5:
      pps->slice_group_map_type = input->slice_group_map_type;
      pps->slice_group_change_direction_flag = (Boolean) input->slice_group_change_direction_flag;
      pps->slice_group_change_rate_minus1 = input->slice_group_change_rate_minus1;
      break;
    case 6:
      pps->slice_group_map_type = 6;
      pps->pic_size_in_map_units_minus1 =
        (((input->img_height+img->auto_crop_bottom)/MB_BLOCK_SIZE)/(2-sps->frame_mbs_only_flag))
        *((input->img_width+img->auto_crop_right)/MB_BLOCK_SIZE) -1;

      for (i=0;i<=pps->pic_size_in_map_units_minus1; i++)
        pps->slice_group_id[i] = input->slice_group_id[i];

      break;
    default:
      printf ("Parset.c: slice_group_map_type invalid, default\n");
      assert (0==1);
    }
  }
// End FMO stuff

  pps->num_ref_idx_l0_active_minus1 = sps->frame_mbs_only_flag ? (sps->num_ref_frames-1) : (2 * sps->num_ref_frames - 1) ;   // set defaults
  pps->num_ref_idx_l1_active_minus1 = sps->frame_mbs_only_flag ? (sps->num_ref_frames-1) : (2 * sps->num_ref_frames - 1) ;   // set defaults

  pps->weighted_pred_flag = (Boolean) WeightedPrediction;
  pps->weighted_bipred_idc = WeightedBiprediction;

  pps->pic_init_qp_minus26 = 0;         // hard coded to zero, QP lives in the slice header
  pps->pic_init_qs_minus26 = 0;

  pps->chroma_qp_index_offset = cb_qp_index_offset;
  if (frext_profile)
  {
    pps->cb_qp_index_offset     = cb_qp_index_offset;
    pps->cr_qp_index_offset     = cr_qp_index_offset;
  }
  else
    pps->cb_qp_index_offset = pps->cr_qp_index_offset = pps->chroma_qp_index_offset;

  pps->deblocking_filter_control_present_flag = (Boolean) input->LFSendParameters;
  pps->constrained_intra_pred_flag = (Boolean) input->UseConstrainedIntraPred;

  // if redundant slice is in use.
  pps->redundant_pic_cnt_present_flag = (Boolean) input->redundant_pic_flag;
}

/*!
 *************************************************************************************
 * \brief
 *    syntax for scaling list matrix values
 *
 * \param scalingListinput
 *    input scaling list
 * \param scalingList
 *    scaling list to be used
 * \param sizeOfScalingList
 *    size of the scaling list
 * \param UseDefaultScalingMatrix
 *    usage of default Scaling Matrix
 * \param bitstream
 *    target bitstream for writing syntax
 *
 * \return
 *    size of the RBSP in bytes
 *
 *************************************************************************************
 */
int Scaling_List(short *scalingListinput, short *scalingList, int sizeOfScalingList, short *UseDefaultScalingMatrix, Bitstream *bitstream)
{
  int j, scanj;
  int len=0;
  int delta_scale, lastScale, nextScale;

  lastScale = 8;
  nextScale = 8;

  for(j=0; j<sizeOfScalingList; j++)
  {
    scanj = (sizeOfScalingList==16) ? ZZ_SCAN[j]:ZZ_SCAN8[j];

    if(nextScale!=0)
    {
      delta_scale = scalingListinput[scanj]-lastScale; // Calculate delta from the scalingList data from the input file
      if(delta_scale>127)
        delta_scale=delta_scale-256;
      else if(delta_scale<-128)
        delta_scale=delta_scale+256;

      len+=se_v ("   : delta_sl   ",                      delta_scale,                       bitstream);
      nextScale = scalingListinput[scanj];
      *UseDefaultScalingMatrix|=(scanj==0 && nextScale==0); // Check first matrix value for zero
    }

    scalingList[scanj] = (short) ((nextScale==0) ? lastScale:nextScale); // Update the actual scalingList matrix with the correct values
    lastScale = scalingList[scanj];
  }

  return len;
}


/*!
 *************************************************************************************
 * \brief
 *    int GenerateSeq_parameter_set_rbsp (seq_parameter_set_rbsp_t *sps, char *rbsp);
 *
 * \param sps
 *    sequence parameter structure
 * \param rbsp
 *    buffer to be filled with the rbsp, size should be at least MAXIMUMPARSETRBSPSIZE
 *
 * \return
 *    size of the RBSP in bytes
 *
 * \note
 *    Sequence Parameter VUI function is called, but the function implements
 *    an exit (-1)
 *************************************************************************************
 */
int GenerateSeq_parameter_set_rbsp (seq_parameter_set_rbsp_t *sps, byte *rbsp)
{
  Bitstream *bitstream;
  int len = 0, LenInBytes;
  unsigned i;

  assert (rbsp != NULL);

  if ((bitstream=calloc(1, sizeof(Bitstream)))==NULL) no_mem_exit("SeqParameterSet:bitstream");

  // .. and use the rbsp provided (or allocated above) for the data
  bitstream->streamBuffer = rbsp;
  bitstream->bits_to_go = 8;

  len+=u_v  (8, "SPS: profile_idc",                             sps->profile_idc,                               bitstream);

  len+=u_1  ("SPS: constrained_set0_flag",                      sps->constrained_set0_flag,    bitstream);
  len+=u_1  ("SPS: constrained_set1_flag",                      sps->constrained_set1_flag,    bitstream);
  len+=u_1  ("SPS: constrained_set2_flag",                      sps->constrained_set2_flag,    bitstream);
  len+=u_1  ("SPS: constrained_set3_flag",                      sps->constrained_set3_flag,    bitstream);
  len+=u_v  (4, "SPS: reserved_zero_4bits",                     0,                             bitstream);

  len+=u_v  (8, "SPS: level_idc",                               sps->level_idc,                                 bitstream);

  len+=ue_v ("SPS: seq_parameter_set_id",                    sps->seq_parameter_set_id,                      bitstream);

  // Fidelity Range Extensions stuff
  if((sps->profile_idc==FREXT_HP) ||
     (sps->profile_idc==FREXT_Hi10P) ||
     (sps->profile_idc==FREXT_Hi422) ||
     (sps->profile_idc==FREXT_Hi444))
  {
    len+=ue_v ("SPS: chroma_format_idc",                        sps->chroma_format_idc,                          bitstream);
    if(img->yuv_format == 3)
      len+=u_1  ("SPS: residue_transform_flag",                 0,                                               bitstream);
    len+=ue_v ("SPS: bit_depth_luma_minus8",                    sps->bit_depth_luma_minus8,                      bitstream);
    len+=ue_v ("SPS: bit_depth_chroma_minus8",                  sps->bit_depth_chroma_minus8,                    bitstream);
    len+=u_1  ("SPS: lossless_qpprime_y_zero_flag",             img->lossless_qpprime_flag,                      bitstream);
    //other chroma info to be added in the future

    len+=u_1 ("SPS: seq_scaling_matrix_present_flag",           sps->seq_scaling_matrix_present_flag,            bitstream);

    if(sps->seq_scaling_matrix_present_flag)
    {
      for(i=0; i<8; i++)
      {
        len+=u_1 ("SPS: seq_scaling_list_present_flag",         sps->seq_scaling_list_present_flag[i],           bitstream);
        if(sps->seq_scaling_list_present_flag[i])
        {
          if(i<6)
            len+=Scaling_List(ScalingList4x4input[i], ScalingList4x4[i], 16, &UseDefaultScalingMatrix4x4Flag[i], bitstream);
          else
            len+=Scaling_List(ScalingList8x8input[i-6], ScalingList8x8[i-6], 64, &UseDefaultScalingMatrix8x8Flag[i-6], bitstream);
        }
      }
    }
  }

  len+=ue_v ("SPS: log2_max_frame_num_minus4",               sps->log2_max_frame_num_minus4,                 bitstream);
  len+=ue_v ("SPS: pic_order_cnt_type",                      sps->pic_order_cnt_type,                        bitstream);

  if (sps->pic_order_cnt_type == 0)
    len+=ue_v ("SPS: log2_max_pic_order_cnt_lsb_minus4",     sps->log2_max_pic_order_cnt_lsb_minus4,         bitstream);
  else if (sps->pic_order_cnt_type == 1)
  {
    len+=u_1  ("SPS: delta_pic_order_always_zero_flag",        sps->delta_pic_order_always_zero_flag,          bitstream);
    len+=se_v ("SPS: offset_for_non_ref_pic",                  sps->offset_for_non_ref_pic,                    bitstream);
    len+=se_v ("SPS: offset_for_top_to_bottom_field",          sps->offset_for_top_to_bottom_field,            bitstream);
    len+=ue_v ("SPS: num_ref_frames_in_pic_order_cnt_cycle",   sps->num_ref_frames_in_pic_order_cnt_cycle,     bitstream);
    for (i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
      len+=se_v ("SPS: offset_for_ref_frame",                  sps->offset_for_ref_frame[i],                      bitstream);
  }
  len+=ue_v ("SPS: num_ref_frames",                          sps->num_ref_frames,                            bitstream);
  len+=u_1  ("SPS: gaps_in_frame_num_value_allowed_flag",    sps->gaps_in_frame_num_value_allowed_flag,      bitstream);
  len+=ue_v ("SPS: pic_width_in_mbs_minus1",                 sps->pic_width_in_mbs_minus1,                   bitstream);
  len+=ue_v ("SPS: pic_height_in_map_units_minus1",          sps->pic_height_in_map_units_minus1,            bitstream);
  len+=u_1  ("SPS: frame_mbs_only_flag",                     sps->frame_mbs_only_flag,                       bitstream);
  if (!sps->frame_mbs_only_flag)
  {
    len+=u_1  ("SPS: mb_adaptive_frame_field_flag",            sps->mb_adaptive_frame_field_flag,              bitstream);
  }
  len+=u_1  ("SPS: direct_8x8_inference_flag",               sps->direct_8x8_inference_flag,                 bitstream);

  len+=u_1  ("SPS: frame_cropping_flag",                      sps->frame_cropping_flag,                       bitstream);
  if (sps->frame_cropping_flag)
  {
    len+=ue_v ("SPS: frame_cropping_rect_left_offset",          sps->frame_cropping_rect_left_offset,           bitstream);
    len+=ue_v ("SPS: frame_cropping_rect_right_offset",         sps->frame_cropping_rect_right_offset,          bitstream);
    len+=ue_v ("SPS: frame_cropping_rect_top_offset",           sps->frame_cropping_rect_top_offset,            bitstream);
    len+=ue_v ("SPS: frame_cropping_rect_bottom_offset",        sps->frame_cropping_rect_bottom_offset,         bitstream);
  }

  len+=u_1  ("SPS: vui_parameters_present_flag",             sps->vui_parameters_present_flag,               bitstream);

  if (sps->vui_parameters_present_flag)
    len+=GenerateVUISequenceParameters(bitstream);    // currently a dummy, asserting

  SODBtoRBSP(bitstream);     // copies the last couple of bits into the byte buffer

  LenInBytes=bitstream->byte_pos;

  free (bitstream);

  return LenInBytes;
}


/*!
 ***********************************************************************************************
 * \brief
 *    int GeneratePic_parameter_set_rbsp (pic_parameter_set_rbsp_t *sps, char *rbsp);
 *
 * \param pps
 *    picture parameter structure
 * \param rbsp
 *    buffer to be filled with the rbsp, size should be at least MAXIMUMPARSETRBSPSIZE
 *
 * \return
 *    size of the RBSP in bytes, negative in case of an error
 *
 * \note
 *    Picture Parameter VUI function is called, but the function implements
 *    an exit (-1)
 ************************************************************************************************
 */

int GeneratePic_parameter_set_rbsp (pic_parameter_set_rbsp_t *pps, byte *rbsp)
{
  Bitstream *bitstream;
  int len = 0, LenInBytes;
  unsigned i;
  unsigned NumberBitsPerSliceGroupId;
  int profile_idc;

  assert (rbsp != NULL);

  if ((bitstream=calloc(1, sizeof(Bitstream)))==NULL) no_mem_exit("PicParameterSet:bitstream");

  // .. and use the rbsp provided (or allocated above) for the data
  bitstream->streamBuffer = rbsp;
  bitstream->bits_to_go = 8;

  pps->pic_order_present_flag = img->pic_order_present_flag;

  len+=ue_v ("PPS: pic_parameter_set_id",                    pps->pic_parameter_set_id,                      bitstream);
  len+=ue_v ("PPS: seq_parameter_set_id",                    pps->seq_parameter_set_id,                      bitstream);
  len+=u_1  ("PPS: entropy_coding_mode_flag",                pps->entropy_coding_mode_flag,                  bitstream);
  len+=u_1  ("PPS: pic_order_present_flag",                  pps->pic_order_present_flag,                    bitstream);
  len+=ue_v ("PPS: num_slice_groups_minus1",                 pps->num_slice_groups_minus1,                   bitstream);

  // FMO stuff
  if(pps->num_slice_groups_minus1 > 0 )
  {
    len+=ue_v ("PPS: slice_group_map_type",                 pps->slice_group_map_type,                   bitstream);
    if (pps->slice_group_map_type == 0)
      for (i=0; i<=pps->num_slice_groups_minus1; i++)
        len+=ue_v ("PPS: run_length_minus1[i]",                           pps->run_length_minus1[i],                             bitstream);
    else if (pps->slice_group_map_type==2)
      for (i=0; i<pps->num_slice_groups_minus1; i++)
      {

        len+=ue_v ("PPS: top_left[i]",                          pps->top_left[i],                           bitstream);
        len+=ue_v ("PPS: bottom_right[i]",                      pps->bottom_right[i],                       bitstream);
      }
    else if (pps->slice_group_map_type == 3 ||
             pps->slice_group_map_type == 4 ||
             pps->slice_group_map_type == 5)
    {
      len+=u_1  ("PPS: slice_group_change_direction_flag",         pps->slice_group_change_direction_flag,         bitstream);
      len+=ue_v ("PPS: slice_group_change_rate_minus1",            pps->slice_group_change_rate_minus1,            bitstream);
    }
    else if (pps->slice_group_map_type == 6)
    {
      if (pps->num_slice_groups_minus1>=4)
        NumberBitsPerSliceGroupId=3;
      else if (pps->num_slice_groups_minus1>=2)
        NumberBitsPerSliceGroupId=2;
      else if (pps->num_slice_groups_minus1>=1)
        NumberBitsPerSliceGroupId=1;
      else
        NumberBitsPerSliceGroupId=0;

      len+=ue_v ("PPS: pic_size_in_map_units_minus1",                       pps->pic_size_in_map_units_minus1,             bitstream);
      for(i=0; i<=pps->pic_size_in_map_units_minus1; i++)
        len+= u_v  (NumberBitsPerSliceGroupId, "PPS: >slice_group_id[i]",   pps->slice_group_id[i],                        bitstream);
    }
  }
  // End of FMO stuff

  len+=ue_v ("PPS: num_ref_idx_l0_active_minus1",             pps->num_ref_idx_l0_active_minus1,              bitstream);
  len+=ue_v ("PPS: num_ref_idx_l1_active_minus1",             pps->num_ref_idx_l1_active_minus1,              bitstream);
  len+=u_1  ("PPS: weighted_pred_flag",                       pps->weighted_pred_flag,                        bitstream);
  len+=u_v  (2, "PPS: weighted_bipred_idc",                   pps->weighted_bipred_idc,                       bitstream);
  len+=se_v ("PPS: pic_init_qp_minus26",                      pps->pic_init_qp_minus26,                       bitstream);
  len+=se_v ("PPS: pic_init_qs_minus26",                      pps->pic_init_qs_minus26,                       bitstream);

  profile_idc = IdentifyProfile();
  if((profile_idc==FREXT_HP) ||
     (profile_idc==FREXT_Hi10P) ||
     (profile_idc==FREXT_Hi422) ||
     (profile_idc==FREXT_Hi444))
    len+=se_v ("PPS: chroma_qp_index_offset",                 pps->cb_qp_index_offset,                        bitstream);
  else
    len+=se_v ("PPS: chroma_qp_index_offset",                 pps->chroma_qp_index_offset,                    bitstream);

  len+=u_1  ("PPS: deblocking_filter_control_present_flag",   pps->deblocking_filter_control_present_flag,    bitstream);
  len+=u_1  ("PPS: constrained_intra_pred_flag",              pps->constrained_intra_pred_flag,               bitstream);
  len+=u_1  ("PPS: redundant_pic_cnt_present_flag",           pps->redundant_pic_cnt_present_flag,            bitstream);

  // Fidelity Range Extensions stuff
  if((profile_idc==FREXT_HP) ||
     (profile_idc==FREXT_Hi10P) ||
     (profile_idc==FREXT_Hi422) ||
     (profile_idc==FREXT_Hi444))
  {
    len+=u_1  ("PPS: transform_8x8_mode_flag",                pps->transform_8x8_mode_flag,                   bitstream);

    len+=u_1  ("PPS: pic_scaling_matrix_present_flag",        pps->pic_scaling_matrix_present_flag,           bitstream);

    if(pps->pic_scaling_matrix_present_flag)
    {
      for(i=0; i<(6+((unsigned)pps->transform_8x8_mode_flag<<1)); i++)
      {
        len+=u_1  ("PPS: pic_scaling_list_present_flag",      pps->pic_scaling_list_present_flag[i],          bitstream);

        if(pps->pic_scaling_list_present_flag[i])
        {
          if(i<6)
            len+=Scaling_List(ScalingList4x4input[i], ScalingList4x4[i], 16, &UseDefaultScalingMatrix4x4Flag[i], bitstream);
          else
            len+=Scaling_List(ScalingList8x8input[i-6], ScalingList8x8[i-6], 64, &UseDefaultScalingMatrix8x8Flag[i-6], bitstream);
        }
      }
    }
    len+=se_v ("PPS: second_chroma_qp_index_offset",          pps->cr_qp_index_offset,                        bitstream);
  }

  SODBtoRBSP(bitstream);     // copies the last couple of bits into the byte buffer

  LenInBytes=bitstream->byte_pos;

  // Get rid of the helper structures
  free (bitstream);

  return LenInBytes;
}



/*!
 *************************************************************************************
 * \brief
 *    Returns the Profile
 *
 * \return
 *    Profile according to Annex A
 *
 * \note
 *    Function is currently a dummy.  Should "calculate" the profile from those
 *    config file parameters.  E.g.
 *
 *    Profile = Baseline;
 *    if (CABAC Used || Interlace used) Profile=Main;
 *    if (!Cabac Used) && (Bframes | SPframes) Profile = Streaming;
 *
 *************************************************************************************
 */
int IdentifyProfile(void)
{
  return input->ProfileIDC;
}

/*!
 *************************************************************************************
 * \brief
 *    Returns the Level
 *
 * \return
 *    Level according to Annex A
 *
 * \note
 *    This function is currently a dummy, but should calculate the level out of
 *    the config file parameters (primarily the picture size)
 *************************************************************************************
 */
int IdentifyLevel(void)
{
  return input->LevelIDC;
}


/*!
 *************************************************************************************
 * \brief
 *    Function body for VUI Parameter generation (to be done)
 *
 * \return
 *    exits with error message
 *************************************************************************************
 */
static int GenerateVUISequenceParameters(Bitstream *bitstream)
{
  int len=0;

  // special case to signal the RGB format
  if(input->rgb_input_flag && input->yuv_format==3)
  {
    //still pretty much a dummy VUI
    printf   ("VUI: writing Sequence Parameter VUI to signal RGB format\n");
    len+=u_1 ("VUI: aspect_ratio_info_present_flag", 0, bitstream);
    len+=u_1 ("VUI: overscan_info_present_flag", 0, bitstream);
    len+=u_1 ("VUI: video_signal_type_present_flag", 1, bitstream);
    len+=u_v (3, "VUI: video format", 2, bitstream);
    len+=u_1 ("VUI: video_full_range_flag", 1, bitstream);
    len+=u_1 ("VUI: color_description_present_flag", 1, bitstream);
    len+=u_v (8, "VUI: colour primaries", 2, bitstream);
    len+=u_v (8, "VUI: transfer characteristics", 2, bitstream);
    len+=u_v (8, "VUI: matrix coefficients", 0, bitstream);
    len+=u_1 ("VUI: chroma_loc_info_present_flag", 0, bitstream);
    len+=u_1 ("VUI: timing_info_present_flag", 0, bitstream);
    len+=u_1 ("VUI: nal_hrd_parameters_present_flag", 0, bitstream);
    len+=u_1 ("VUI: vcl_hrd_parameters_present_flag", 0, bitstream);
    len+=u_1 ("VUI: pic_struc_present_flag", 0, bitstream);
    len+=u_1 ("VUI: bitstream_restriction_flag", 0, bitstream);

    return len;
  }
  else if (input->Generate_SEIVUI)
  {
      int bitstream_restriction_flag = 0;
      int timing_info_present_flag = 0;
      int aspect_ratio_info_present_flag = 0;
      len+=u_1 ("VUI: aspect_ratio_info_present_flag", 0, bitstream);
      if (aspect_ratio_info_present_flag)
      {
        len+=u_v (8,"VUI: aspect_ratio_idc", 1, bitstream);
      }
      len+=u_1 ("VUI: overscan_info_present_flag", 0, bitstream);
      len+=u_1 ("VUI: video_signal_type_present_flag", 0, bitstream);
      len+=u_1 ("VUI: chroma_loc_info_present_flag", 0, bitstream);
      len+=u_1 ("VUI: timing_info_present_flag", timing_info_present_flag, bitstream);
            // timing parameters
      if (timing_info_present_flag)
      {
        len+=u_v (32,"VUI: num_units_in_tick", 416667, bitstream);
        len+=u_v (32,"VUI: time_scale", 20000000, bitstream);
        len+=u_1 ("VUI: fixed_frame_rate_flag", 1, bitstream);
      }
      // end of timing parameters
      len+=u_1 ("VUI: nal_hrd_parameters_present_flag", 0, bitstream);
      len+=u_1 ("VUI: vcl_hrd_parameters_present_flag", 0, bitstream);
      len+=u_1 ("VUI: pic_struc_present_flag", 0, bitstream);

      len+=u_1 ("VUI: bitstream_restriction_flag", bitstream_restriction_flag, bitstream);
      if (bitstream_restriction_flag)
      {
        len+=u_1 ("VUI: motion_vectors_over_pic_boundaries_flag", 1, bitstream);
        len+=ue_v ("VUI: max_bytes_per_pic_denom", 0, bitstream);
        len+=ue_v ("VUI: max_bits_per_mb_denom", 0, bitstream);
        len+=ue_v ("VUI: log2_max_mv_length_horizontal", 11, bitstream);
        len+=ue_v ("VUI: log2_max_mv_length_vertical", 11, bitstream);
        len+=ue_v ("VUI: num_reorder_frames", 3, bitstream);
        len+=ue_v ("VUI: max_dec_frame_buffering", 4, bitstream);
      }
    }
  else
  {
    printf ("Sequence Parameter VUI not yet implemented, this should never happen, exit\n");
    exit (-1);
  }

  return 1;
}

/*!
 *************************************************************************************
 * \brief
 *    Function body for SEI message NALU generation
 *
 * \return
 *    A NALU containing the SEI messages
 *
 *************************************************************************************
 */
NALU_t *GenerateSEImessage_NALU()
{
  NALU_t *n = AllocNALU(64000);
  int RBSPlen = 0;
  int NALUlen;
  byte rbsp[MAXRBSPSIZE];

  RBSPlen = GenerateSEImessage_rbsp (NORMAL_SEI, rbsp);
  NALUlen = RBSPtoNALU (rbsp, n, RBSPlen, NALU_TYPE_SEI, NALU_PRIORITY_DISPOSABLE, 0, 1);
  n->startcodeprefix_len = 4;

  return n;
}


/*!
 *************************************************************************************
 * \brief
 *    int GenerateSEImessage_rbsp (int, bufferingperiod_information_struct*, char*)
 *
 *
 * \return
 *    size of the RBSP in bytes, negative in case of an error
 *
 * \note
 *************************************************************************************
 */
int GenerateSEImessage_rbsp (int id, byte *rbsp)
{
  Bitstream *bitstream;

  int len = 0, LenInBytes;
  assert (rbsp != NULL);

  if ((bitstream=calloc(1, sizeof(Bitstream)))==NULL) no_mem_exit("SeqParameterSet:bitstream");

  // .. and use the rbsp provided (or allocated above) for the data
  bitstream->streamBuffer = rbsp;
  bitstream->bits_to_go = 8;

  {
    char sei_message[500];
    char uuid_message[9] = "RandomMSG"; // This is supposed to be Random
    unsigned int i, message_size = strlen(input->SEIMessageText);
    struct TIMEB tstruct;
    ftime( &tstruct);    // start time ms

    if (message_size == 0)
    {
      message_size = 13;
      strncpy(sei_message,"Empty Message",message_size);
    }
    else
      strncpy(sei_message,input->SEIMessageText,message_size);

    len+=u_v (8,"SEI: last_payload_type_byte", 5, bitstream);
    message_size += 17;
    while (message_size > 254)
    {
      len+=u_v (8,"SEI: ff_byte",255, bitstream);
      message_size -= 255;
    }
    len+=u_v (8,"SEI: last_payload_size_byte",message_size, bitstream);

    // Lets randomize uuid based on time
    len+=u_v (32,"SEI: uuid_iso_iec_11578",(int) tstruct.timezone, bitstream);
    len+=u_v (32,"SEI: uuid_iso_iec_11578",(int) tstruct.time*1000+tstruct.millitm, bitstream);
    len+=u_v (32,"SEI: uuid_iso_iec_11578",(int) (uuid_message[0] << 24) + (uuid_message[1] << 16)  + (uuid_message[2] << 8) + (uuid_message[3] << 0), bitstream);
    len+=u_v (32,"SEI: uuid_iso_iec_11578",(int) (uuid_message[4] << 24) + (uuid_message[5] << 16)  + (uuid_message[6] << 8) + (uuid_message[7] << 0), bitstream);
    for (i = 0; i < strlen(sei_message); i++)
      len+=u_v (8,"SEI: user_data_payload_byte",sei_message[i], bitstream);

    len+=u_v (8,"SEI: user_data_payload_byte",   0, bitstream);
  }
  SODBtoRBSP(bitstream);     // copies the last couple of bits into the byte buffer

  LenInBytes=bitstream->byte_pos;

  free(bitstream);
  return LenInBytes;
}
