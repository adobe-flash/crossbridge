
/*!
 *************************************************************************************
 * \file image.c
 *
 * \brief
 *    Code one image/slice
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *     - Inge Lille-Langoy               <inge.lille-langoy@telenor.com>
 *     - Rickard Sjoberg                 <rickard.sjoberg@era.ericsson.se>
 *     - Jani Lainema                    <jani.lainema@nokia.com>
 *     - Sebastian Purreiter             <sebastian.purreiter@mch.siemens.de>
 *     - Byeong-Moon Jeon                <jeonbm@lge.com>
 *     - Yoon-Seong Soh                  <yunsung@lge.com>
 *     - Thomas Stockhammer              <stockhammer@ei.tum.de>
 *     - Detlev Marpe                    <marpe@hhi.de>
 *     - Guido Heising                   <heising@hhi.de>
 *     - Thomas Wedi                     <wedi@tnt.uni-hannover.de>
 *     - Ragip Kurceren                  <ragip.kurceren@nokia.com>
 *     - Antti Hallapuro                 <antti.hallapuro@nokia.com>
 *     - Alexis Michael Tourapis         <alexismt@ieee.org>
 *     - Athanasios Leontaris            <aleon@dolby.com>
 *************************************************************************************
 */
#include "contributors.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include "global.h"

#include "refbuf.h"
#include "mbuffer.h"
#include "img_luma.h"
#include "img_chroma.h"
#include "intrarefresh.h"
#include "fmo.h"
#include "sei.h"
#include "memalloc.h"
#include "nalu.h"
#include "ratectl.h"
#include "rc_quadratic.h"
#include "mb_access.h"
#include "output.h"
#include "cabac.h"

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
#include <sys/time.h>
int ftime(struct timeb *tp)
{ 
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  tp->time = tv.tv_sec;
  tp->millitm = tv.tv_usec / 1000;
  tp->timezone = tz.tz_minuteswest;
  tp->dstflag = tz.tz_dsttime;
}
#endif

extern pic_parameter_set_rbsp_t *PicParSet[MAXPPS];

void code_a_picture(Picture *pic);
void frame_picture (Picture *frame, int method);
void field_picture(Picture *top, Picture *bottom);

static int  writeout_picture(Picture *pic);

static int  picture_structure_decision(Picture *frame, Picture *top, Picture *bot);
static void distortion_fld (float *dis_fld_y, float *dis_fld_u, float *dis_fld_v);
static void find_snr(void);
static void find_distortion(void);

static void field_mode_buffer(int bit_field, float snr_field_y, float snr_field_u, float snr_field_v);
static void frame_mode_buffer (int bit_frame, float snr_frame_y, float snr_frame_u, float snr_frame_v);

static void init_frame(void);
static void init_field(void);

static void put_buffer_frame(void);
static void put_buffer_top(void);
static void put_buffer_bot(void);

static void copy_motion_vectors_MB(void);

static void PaddAutoCropBorders (int org_size_x, int org_size_y, int img_size_x, int img_size_y,
                                 int org_size_x_cr, int org_size_y_cr, int img_size_x_cr, int img_size_y_cr);

static void ReadOneFrame (int FrameNoInFile, int HeaderSize, int xs, int ys, int xs_cr, int ys_cr);

static void writeUnit(Bitstream* currStream ,int partition);
static void rdPictureCoding(void);

#ifdef _ADAPT_LAST_GROUP_
int *last_P_no;
int *last_P_no_frm;
int *last_P_no_fld;
#endif

static void ReportFirstframe(time_t tmp_time, time_t me_time);
static void ReportIntra(time_t tmp_time, time_t me_time);
static void ReportSP(time_t tmp_time, time_t me_time);
static void ReportP(time_t tmp_time, time_t me_time);
static void ReportB(time_t tmp_time, time_t me_time);
static void ReportNALNonVLCBits(time_t tmp_time, time_t me_time);

static int CalculateFrameNumber(void);  // Calculates the next frame number

StorablePicture *enc_picture;
StorablePicture *enc_frame_picture;
StorablePicture *enc_frame_picture2;
StorablePicture *enc_frame_picture3;
StorablePicture *enc_top_picture;
StorablePicture *enc_bottom_picture;
//Rate control
int    QP;

void MbAffPostProc(void)
{
  imgpel temp[32][16];

  imgpel ** imgY  = enc_picture->imgY;
  imgpel ***imgUV = enc_picture->imgUV;
  int i, y, x0, y0, uv;

  if (img->yuv_format != YUV400)
  {
    for (i=0; i<(int)img->PicSizeInMbs; i+=2)
    {
      if (enc_picture->mb_field[i])
      {
        get_mb_pos(i, &x0, &y0, IS_LUMA);
        for (y=0; y<(2*MB_BLOCK_SIZE);y++)
          memcpy(&temp[y],&imgY[y0+y][x0], MB_BLOCK_SIZE * sizeof(imgpel));

        for (y=0; y<MB_BLOCK_SIZE;y++)
        {
          memcpy(&imgY[y0+(2*y)][x0],temp[y], MB_BLOCK_SIZE * sizeof(imgpel));
          memcpy(&imgY[y0+(2*y + 1)][x0],temp[y+ MB_BLOCK_SIZE], MB_BLOCK_SIZE * sizeof(imgpel));
        }

        x0 = x0 / (16/img->mb_cr_size_x);
        y0 = y0 / (16/img->mb_cr_size_y);

        for (uv=0; uv<2; uv++)
        {
          for (y=0; y<(2*img->mb_cr_size_y);y++)
            memcpy(&temp[y],&imgUV[uv][y0+y][x0], img->mb_cr_size_x * sizeof(imgpel));

          for (y=0; y<img->mb_cr_size_y;y++)
          {
            memcpy(&imgUV[uv][y0+(2*y)][x0],temp[y], img->mb_cr_size_x * sizeof(imgpel));
            memcpy(&imgUV[uv][y0+(2*y + 1)][x0],temp[y+ img->mb_cr_size_y], img->mb_cr_size_x * sizeof(imgpel));
          }
        }
      }
    }
  }
  else
  {
    for (i=0; i<(int)img->PicSizeInMbs; i+=2)
    {
      if (enc_picture->mb_field[i])
      {
        get_mb_pos(i, &x0, &y0, IS_LUMA);
        for (y=0; y<(2*MB_BLOCK_SIZE);y++)
          memcpy(&temp[y],&imgY[y0+y][x0], MB_BLOCK_SIZE * sizeof(imgpel));

        for (y=0; y<MB_BLOCK_SIZE;y++)
        {
          memcpy(&imgY[y0+(2*y)][x0],temp[y], MB_BLOCK_SIZE * sizeof(imgpel));
          memcpy(&imgY[y0+(2*y + 1)][x0],temp[y+ MB_BLOCK_SIZE], MB_BLOCK_SIZE * sizeof(imgpel));
        }
      }
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Encodes a picture
 *
 *    This is the main picture coding loop.. It is called by all this
 *    frame and field coding stuff after the img-> elements have been
 *    set up.  Not sure whether it is useful for MB-adaptive frame/field
 *    coding
 ************************************************************************
 */
void code_a_picture(Picture *pic)
{
  unsigned int NumberOfCodedMBs = 0;
  int SliceGroup = 0;
  int j;

  int intra_refresh = input->intra_period == 0 ? (IMG_NUMBER == 0) : ((IMG_NUMBER%input->intra_period) == 0);

  img->currentPicture = pic;

  img->currentPicture->idr_flag = ((!IMG_NUMBER) && (!(img->structure==BOTTOM_FIELD)))
    || (input->idr_enable && intra_refresh && (img->type == I_SLICE || img->type==SI_SLICE)&& (!(img->structure==BOTTOM_FIELD)));

  pic->no_slices = 0;
  pic->distortion_u = pic->distortion_v = pic->distortion_y = 0.0;

  RandomIntraNewPicture ();     //! Allocates forced INTRA MBs (even for fields!)

  // The slice_group_change_cycle can be changed here.
  // FmoInit() is called before coding each picture, frame or field
  img->slice_group_change_cycle=1;
  FmoInit(img, active_pps, active_sps);
  FmoStartPicture ();           //! picture level initialization of FMO

  CalculateQuantParam();
  CalculateOffsetParam();

  if(input->Transform8x8Mode)
  {
    CalculateQuant8Param();
    CalculateOffset8Param();
  }

  reset_pic_bin_count();
  img->bytes_in_picture = 0;

  while (NumberOfCodedMBs < img->PicSizeInMbs)       // loop over slices
  {
    // Encode one SLice Group
    while (!FmoSliceGroupCompletelyCoded (SliceGroup))
    {
      // Encode the current slice
      NumberOfCodedMBs += encode_one_slice (SliceGroup, pic, NumberOfCodedMBs);
      FmoSetLastMacroblockInSlice (img->current_mb_nr);
      // Proceed to next slice
      img->current_slice_nr++;
      stats->bit_slice = 0;
    }
    // Proceed to next SliceGroup
    SliceGroup++;
  }
  FmoEndPicture ();

  // Modified for Fast Mode Decision. Inchoon Choi, SungKyunKwan Univ.
  if (input->rdopt == 3 && (img->type != B_SLICE))
    for (j = 0; j < input->NoOfDecoders; j++)
      DeblockFrame (img, decs->decY_best[j], NULL);

  DeblockFrame (img, enc_picture->imgY, enc_picture->imgUV); //comment out to disable loop filter

  if (img->MbaffFrameFlag)
    MbAffPostProc();
}



/*!
 ************************************************************************
 * \brief
 *    Encodes one frame
 ************************************************************************
 */
int encode_one_frame (void)
{
  static int prev_frame_no = 0; // POC200301
  static int consecutive_non_reference_pictures = 0; // POC200301
  int        FrameNumberInFile;
  int        i, j;

#ifdef _LEAKYBUCKET_
  //extern long Bit_Buffer[20000];
  extern unsigned long total_frame_buffer;
#endif

  time_t ltime1;
  time_t ltime2;

  struct TIMEB tstruct1;
  struct TIMEB tstruct2;

  time_t tmp_time;
  int bits_frm = 0, bits_fld = 0;
  float dis_frm = 0.0, dis_frm_y = 0.0, dis_frm_u = 0.0, dis_frm_v = 0.0;
  float dis_fld = 0.0, dis_fld_y = 0.0, dis_fld_u = 0.0, dis_fld_v = 0.0;

  //Rate control
  int pic_type, bits = 0;

  me_time=0;
  img->rd_pass = 0;
  enc_frame_picture  = NULL;
  enc_frame_picture2 = NULL;
  enc_frame_picture3 = NULL;

  ftime (&tstruct1);            // start time ms
  time (&ltime1);               // start time s

  //Rate control
  img->write_macroblock = 0;
/*
  //Shankar Regunathan (Oct 2002)
  //Prepare Panscanrect SEI payload
  UpdatePanScanRectInfo ();
  //Prepare Arbitrarydata SEI Payload
  UpdateUser_data_unregistered ();
  //Prepare Registered data SEI Payload
  UpdateUser_data_registered_itu_t_t35 ();
  //Prepare RandomAccess SEI Payload
  UpdateRandomAccess ();
*/

  if (input->ResendPPS && img->number !=0)
  {
    stats->bit_ctr_parametersets_n=write_PPS(0, 0);
    stats->bit_ctr_parametersets += stats->bit_ctr_parametersets_n;
  }

  put_buffer_frame ();      // sets the pointers to the frame structures
                            // (and not to one of the field structures)
  init_frame ();
  FrameNumberInFile = CalculateFrameNumber();

  ReadOneFrame (FrameNumberInFile, input->infile_header,
                input->img_width, input->img_height, input->img_width_cr, input->img_height_cr);

  PaddAutoCropBorders (input->img_width, input->img_height, img->width, img->height,
                       input->img_width_cr, input->img_height_cr, img->width_cr, img->height_cr);

  // set parameters for direct mode and deblocking filter
  img->direct_spatial_mv_pred_flag     = input->direct_spatial_mv_pred_flag;
  img->LFDisableIdc    = input->LFDisableIdc;
  img->LFAlphaC0Offset = input->LFAlphaC0Offset;
  img->LFBetaOffset    = input->LFBetaOffset;
  img->AdaptiveRounding = input->AdaptiveRounding;
  // Following code should consider optimal coding mode. Currently also does not support
  // multiple slices per frame.
  frame_ctr[img->type]++;
  snr->frame_ctr++;

  if(img->type == SP_SLICE)
  {
    if(input->sp2_frame_indicator)
    { // switching SP frame encoding
      sp2_frame_indicator=1;
      read_SP_coefficients();
    }
  }
  else
  {
    sp2_frame_indicator=0;
  }

  if (input->PicInterlace == FIELD_CODING)
  {
    //Rate control
    if ( input->RCEnable )
      generic_RC->FieldControl=1;

    img->field_picture = 1;  // we encode fields
    field_picture (top_pic, bottom_pic);
    img->fld_flag = 1;
  }
  else
  {
    int tmpFrameQP;
    //Rate control
    if ( input->RCEnable )
      generic_RC->FieldControl=0;

    // For frame coding, turn MB level field/frame coding flag on
    if (input->MbInterlace)
      mb_adaptive = 1;

    img->field_picture = 0; // we encode a frame

    //Rate control
    if(input->RCEnable)
    {
      /*update the number of MBs in the basic unit for MB adaptive
      f/f coding*/
      if( (input->MbInterlace) && (input->basicunit < img->FrameSizeInMbs) && (img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
        img->BasicUnit = input->basicunit << 1;
      else
        img->BasicUnit = input->basicunit;

      if ( input->RDPictureDecision )
      {
        // store rate allocation quadratic...
        copy_rc_jvt( quadratic_RC_init, quadratic_RC );
        // ...and generic model
        copy_rc_generic( generic_RC_init, generic_RC );
      }

      rc_init_pict(quadratic_RC, 1,0,1, 1.0F);
      img->qp  = updateQP(quadratic_RC, 0);

      pic_type = img->type;
      QP =0;

      if( active_sps->frame_mbs_only_flag)
        generic_RC->TopFieldFlag=0;
    }

    if (input->GenerateMultiplePPS)
      active_pps = PicParSet[0];

    frame_picture (frame_pic_1, 0);

    if ((input->RDPictureIntra || img->type!=I_SLICE) && input->RDPictureDecision)
    {
      rdPictureCoding();
    }
    tmpFrameQP = img->SumFrameQP; // call it here since rdPictureCoding buffers it and may modify it

    if(img->type==SP_SLICE && si_frame_indicator==0 && input->si_frame_indicator)
    {
      // once the picture has been encoded as a primary SP frame encode as an SI frame
      si_frame_indicator=1;
      frame_picture (frame_pic_si, 0);
    }
    if(img->type==SP_SLICE && input->sp_output_indicator)
    {
      // output the transformed and quantized coefficients (useful for switching SP frames)
      output_SP_coefficients();
    }
    // For field coding, turn MB level field/frame coding flag off
    if (input->MbInterlace)
      mb_adaptive = 0;

    if (input->PicInterlace == ADAPTIVE_CODING)
    {
      //Rate control
      if ( input->RCEnable )
        generic_RC->FieldControl=1;
      img->write_macroblock = 0;
      img->bot_MB = 0;

      img->field_picture = 1;  // we encode fields
      field_picture (top_pic, bottom_pic);

      //! Note: the distortion for a field coded picture is stored in the top field
      //! the distortion values in the bottom field are dummies
      dis_fld = top_pic->distortion_y + top_pic->distortion_u + top_pic->distortion_v;
      dis_frm = frame_pic_1->distortion_y + frame_pic_1->distortion_u + frame_pic_1->distortion_v;

      if(img->rd_pass==0)
        img->fld_flag = picture_structure_decision (frame_pic_1, top_pic, bottom_pic);
      else if(img->rd_pass==1)
        img->fld_flag = picture_structure_decision (frame_pic_2, top_pic, bottom_pic);
      else
        img->fld_flag = picture_structure_decision (frame_pic_3, top_pic, bottom_pic);
      if ( img->fld_flag )
        tmpFrameQP = img->SumFrameQP;

      update_field_frame_contexts (img->fld_flag);

      //Rate control
      if ( input->RCEnable )
      {
        generic_RC->FieldFrame = !(img->fld_flag) ? 1 : 0;
      }
    }
    else
      img->fld_flag = 0;
    img->SumFrameQP = tmpFrameQP;
  }

  if (img->fld_flag)
    stats->bit_ctr_emulationprevention += stats->em_prev_bits_fld;
  else
    stats->bit_ctr_emulationprevention += stats->em_prev_bits_frm;

  if (img->type != B_SLICE)
  {
    img->pstruct_next_P = img->fld_flag;
  }

  // Here, img->structure may be either FRAME or BOTTOM FIELD depending on whether AFF coding is used
  // The picture structure decision changes really only the fld_flag

  if (img->fld_flag)            // field mode (use field when fld_flag=1 only)
  {
    field_mode_buffer (bits_fld, dis_fld_y, dis_fld_u, dis_fld_v);
    writeout_picture (top_pic);
    writeout_picture (bottom_pic);
  }
  else                          //frame mode
  {
    frame_mode_buffer (bits_frm, dis_frm_y, dis_frm_u, dis_frm_v);

    if (input->RDPictureDecision && img->rd_pass == 2)
      writeout_picture (frame_pic_3);
    else if (input->RDPictureDecision && img->rd_pass == 1)
      writeout_picture (frame_pic_2);
    else
    if(img->type==SP_SLICE && si_frame_indicator==1)
    {
      writeout_picture (frame_pic_si);
      si_frame_indicator=0;
    }
    else
      writeout_picture (frame_pic_1);
  }

  if (frame_pic_si)
    free_slice_list(frame_pic_si);
  if (frame_pic_3)
    free_slice_list(frame_pic_3);
  if (frame_pic_2)
    free_slice_list(frame_pic_2);
  if (frame_pic_1)
    free_slice_list(frame_pic_1);
  if (top_pic)
    free_slice_list(top_pic);
  if (bottom_pic)
    free_slice_list(bottom_pic);

  /*
  // Tian Dong (Sept 2002)
  // in frame mode, the newly reconstructed frame has been inserted to the mem buffer
  // and it is time to prepare the spare picture SEI payload.
  if (input->InterlaceCodingOption == FRAME_CODING
      && input->SparePictureOption && img->type != B_SLICE)
    CalculateSparePicture ();
*/

  //Rate control
  if(input->RCEnable)
  {
    bits = (int) (stats->bit_ctr - stats->bit_ctr_n);
    rc_update_pict_frame(quadratic_RC, bits);
  }

  if (input->PicInterlace == FRAME_CODING)
  {
    if (input->rdopt == 3 && img->type != B_SLICE)
      UpdateDecoders ();      // simulate packet losses and move decoded image to reference buffers

    if (input->RestrictRef)
      UpdatePixelMap ();
  }

  if (input->Verbose != 0)
    find_snr ();
  else
  {
    snr->snr_y = 0.0;
    snr->snr_u = 0.0;
    snr->snr_v = 0.0;
    snr->sse_y = 0.0;
    snr->sse_u = 0.0;
    snr->sse_v = 0.0;
  }

  // redundant pictures: save reconstruction to calculate SNR and replace reference picture
  if(input->redundant_pic_flag && key_frame)
  {
    for(i=0; i<img->width; i++)
    {
      for(j=0; j<img->height; j++)
      {
        imgY_tmp[j][i] = enc_frame_picture->imgY[j][i];
      }
    }
    for(i=0; i<img->width_cr; i++)
    {
      for(j=0; j<img->height_cr; j++)
      {
        imgUV_tmp[0][j][i] = enc_frame_picture->imgUV[0][j][i];
        imgUV_tmp[1][j][i] = enc_frame_picture->imgUV[1][j][i];
      }
    }
  }

  if(input->redundant_pic_flag && redundant_coding)
  {
    for(i=0; i<img->width; i++)
    {
      for(j=0; j<img->height; j++)
      {
        enc_frame_picture->imgY[j][i] = imgY_tmp[j][i];
      }
    }
    for(i=0; i<img->width_cr; i++)
    {
      for(j=0; j<img->height_cr; j++)
      {
        enc_frame_picture->imgUV[0][j][i] = imgUV_tmp[0][j][i];
        enc_frame_picture->imgUV[1][j][i] = imgUV_tmp[1][j][i];
      }
    }
  }

  time (&ltime2);               // end time sec
  ftime (&tstruct2);            // end time ms

  tmp_time = (ltime2 * 1000 + tstruct2.millitm) - (ltime1 * 1000 + tstruct1.millitm);
  tot_time = tot_time + tmp_time;

  if (input->PicInterlace == ADAPTIVE_CODING)
  {
    if (img->fld_flag)
    {
      // store bottom field
      store_picture_in_dpb(enc_bottom_picture);
      free_storable_picture(enc_frame_picture);
    }
    else
    {
      // replace top with frame
      replace_top_pic_with_frame(enc_frame_picture);
      free_storable_picture(enc_bottom_picture);
    }
  }
  else
  {
    if (img->fld_flag)
    {
      store_picture_in_dpb(enc_bottom_picture);
    }
    else
    {
      if (img->rd_pass==2)
      {
        store_picture_in_dpb(enc_frame_picture3);
        free_storable_picture(enc_frame_picture);
        free_storable_picture(enc_frame_picture2);
      }
      else if (img->rd_pass==1)
      {
        store_picture_in_dpb(enc_frame_picture2);
        free_storable_picture(enc_frame_picture);
        free_storable_picture(enc_frame_picture3);
      }
      else
      {
        if(input->redundant_pic_flag==0)
        {
          store_picture_in_dpb(enc_frame_picture);
          free_storable_picture(enc_frame_picture2);
          free_storable_picture(enc_frame_picture3);
        }
        else
        {
          // key picture will be stored in dpb after redundant picture is coded
          if(key_frame==0)
          {
            store_picture_in_dpb(enc_frame_picture);
            free_storable_picture(enc_frame_picture2);
            free_storable_picture(enc_frame_picture3);
          }
        }
      }
    }
  }

  img->AverageFrameQP = (img->SumFrameQP + (img->FrameSizeInMbs >> 1))/img->FrameSizeInMbs;
  if ( input->RCEnable && img->type != B_SLICE && input->basicunit < img->FrameSizeInMbs )
    quadratic_RC->CurrLastQP = img->AverageFrameQP;

#ifdef _LEAKYBUCKET_
  // Store bits used for this frame and increment counter of no. of coded frames
  Bit_Buffer[total_frame_buffer] = (int) (stats->bit_ctr - stats->bit_ctr_n);
  total_frame_buffer++;
#endif

  // POC200301: Verify that POC coding type 2 is not used if more than one consecutive
  // non-reference frame is requested or if decoding order is different from output order
  if (img->pic_order_cnt_type == 2)
  {
    if (!img->nal_reference_idc) consecutive_non_reference_pictures++;
    else consecutive_non_reference_pictures = 0;

    if (frame_no < prev_frame_no || consecutive_non_reference_pictures>1)
      error("POC type 2 cannot be applied for the coding pattern where the encoding /decoding order of pictures are different from the output order.\n", -1);
    prev_frame_no = frame_no;
  }

  if (stats->bit_ctr_parametersets_n!=0)
    ReportNALNonVLCBits(tmp_time, me_time);

  if (IMG_NUMBER == 0)
    ReportFirstframe(tmp_time,me_time);
  else
  {
    //Rate control
    if(input->RCEnable)
    {
      if((!input->PicInterlace)&&(!input->MbInterlace))
        bits=(int) (stats->bit_ctr - stats->bit_ctr_n);
      else
      {
        bits = (int)(stats->bit_ctr - (quadratic_RC->Pprev_bits)); // used for rate control update
        quadratic_RC->Pprev_bits = stats->bit_ctr;
      }
    }

    switch (img->type)
    {
    case I_SLICE:
      stats->bit_ctr_I += stats->bit_ctr - stats->bit_ctr_n;
      ReportIntra(tmp_time,me_time);
      break;
    case SP_SLICE:
      stats->bit_ctr_P += stats->bit_ctr - stats->bit_ctr_n;
      ReportSP(tmp_time,me_time);
      break;
    case B_SLICE:
      stats->bit_ctr_B += stats->bit_ctr - stats->bit_ctr_n;
      ReportB(tmp_time,me_time);
      break;
    default:      // P
      stats->bit_ctr_P += stats->bit_ctr - stats->bit_ctr_n;
      ReportP(tmp_time,me_time);
    }
  }

  if (input->Verbose == 0)
  {
    //for (i = 0; i <= (img->number & 0x0F); i++)
    //printf(".");
    //printf("                              \r");
    printf("Completed Encoding Frame %05d.\r",frame_no);
  }
  // Flush output statistics
  fflush(stdout);

  stats->bit_ctr_n = stats->bit_ctr;

  //Rate control
  if(input->RCEnable)
  {
    rc_update_pict(quadratic_RC, bits);

    // update the parameters of quadratic R-D model
    if( (img->type==P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
    {
      updateRCModel(quadratic_RC);

      if ( input->RCUpdateMode == RC_MODE_3 )
        quadratic_RC->PreviousWholeFrameMAD = ComputeFrameMAD();
    }
  }

  stats->bit_ctr_parametersets_n=0;

  if (IMG_NUMBER == 0)
    return 0;
  else
    return 1;
}


/*!
 ************************************************************************
 * \brief
 *    This function write out a picture
 * \return
 *    0 if OK,                                                         \n
 *    1 in case of error
 *
 ************************************************************************
 */
static int writeout_picture(Picture *pic)
{
  Bitstream *currStream;
  int partition, slice;
  Slice *currSlice;

  img->currentPicture=pic;

  for (slice=0; slice<pic->no_slices; slice++)
  {
    currSlice = pic->slices[slice];
    img->current_mb_nr = currSlice->start_mb_nr;
    for (partition=0; partition<currSlice->max_part_nr; partition++)
    {
      currStream = (currSlice->partArr[partition]).bitstream;
      assert (currStream->bits_to_go == 8);    //! should always be the case, the
                                               //! byte alignment is done in terminate_slice

      // write only if the partition has content
      if (currSlice->partArr[partition].bitstream->write_flag )
        writeUnit (currStream,partition);
    }           // partition loop
  }           // slice loop
  return 0;
}


void copy_params(void)
{
  enc_picture->frame_mbs_only_flag = active_sps->frame_mbs_only_flag;
  enc_picture->frame_cropping_flag = active_sps->frame_cropping_flag;
  enc_picture->chroma_format_idc   = active_sps->chroma_format_idc;

  if (active_sps->frame_cropping_flag)
  {
    enc_picture->frame_cropping_rect_left_offset=active_sps->frame_cropping_rect_left_offset;
    enc_picture->frame_cropping_rect_right_offset=active_sps->frame_cropping_rect_right_offset;
    enc_picture->frame_cropping_rect_top_offset=active_sps->frame_cropping_rect_top_offset;
    enc_picture->frame_cropping_rect_bottom_offset=active_sps->frame_cropping_rect_bottom_offset;
  }
  else
  {
    enc_picture->frame_cropping_rect_left_offset=0;
    enc_picture->frame_cropping_rect_right_offset=0;
    enc_picture->frame_cropping_rect_top_offset=0;
    enc_picture->frame_cropping_rect_bottom_offset=0;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Encodes a frame picture
 ************************************************************************
 */
void frame_picture (Picture *frame, int rd_pass)
{
  img->SumFrameQP = 0;
  img->structure = FRAME;
  img->PicSizeInMbs = img->FrameSizeInMbs;

  if (rd_pass == 2)
  {
    enc_frame_picture3  = alloc_storable_picture ((PictureStructure) img->structure, img->width, img->height, img->width_cr, img->height_cr);
    img->ThisPOC=enc_frame_picture3->poc=img->framepoc;
    enc_frame_picture3->top_poc    = img->toppoc;
    enc_frame_picture3->bottom_poc = img->bottompoc;

    enc_frame_picture3->frame_poc = img->framepoc;

    enc_frame_picture3->pic_num = img->frame_num;
    enc_frame_picture3->frame_num = img->frame_num;
    enc_frame_picture3->coded_frame = 1;

    enc_frame_picture3->MbaffFrameFlag = img->MbaffFrameFlag = (input->MbInterlace != FRAME_CODING);
    get_mb_block_pos = img->MbaffFrameFlag ? get_mb_block_pos_mbaff : get_mb_block_pos_normal;
    getNeighbour = img->MbaffFrameFlag ? getAffNeighbour : getNonAffNeighbour;

    enc_picture=enc_frame_picture3;
    copy_params();
  }
  else if (rd_pass == 1)
  {
    enc_frame_picture2  = alloc_storable_picture ((PictureStructure) img->structure, img->width, img->height, img->width_cr, img->height_cr);
    img->ThisPOC=enc_frame_picture2->poc=img->framepoc;
    enc_frame_picture2->top_poc    = img->toppoc;
    enc_frame_picture2->bottom_poc = img->bottompoc;

    enc_frame_picture2->frame_poc = img->framepoc;

    enc_frame_picture2->pic_num = img->frame_num;
    enc_frame_picture2->frame_num = img->frame_num;
    enc_frame_picture2->coded_frame = 1;

    enc_frame_picture2->MbaffFrameFlag = img->MbaffFrameFlag = (input->MbInterlace != FRAME_CODING);
    get_mb_block_pos = img->MbaffFrameFlag ? get_mb_block_pos_mbaff : get_mb_block_pos_normal;
    getNeighbour = img->MbaffFrameFlag ? getAffNeighbour : getNonAffNeighbour;

    enc_picture=enc_frame_picture2;
    copy_params();
  }
  else
  {
    enc_frame_picture  = alloc_storable_picture ((PictureStructure) img->structure, img->width, img->height, img->width_cr, img->height_cr);
    img->ThisPOC=enc_frame_picture->poc=img->framepoc;
    enc_frame_picture->top_poc    = img->toppoc;
    enc_frame_picture->bottom_poc = img->bottompoc;

    enc_frame_picture->frame_poc = img->framepoc;

    enc_frame_picture->pic_num = img->frame_num;
    enc_frame_picture->frame_num = img->frame_num;
    enc_frame_picture->coded_frame = 1;

    enc_frame_picture->MbaffFrameFlag = img->MbaffFrameFlag = (input->MbInterlace != FRAME_CODING);
    get_mb_block_pos = img->MbaffFrameFlag ? get_mb_block_pos_mbaff : get_mb_block_pos_normal;
    getNeighbour = img->MbaffFrameFlag ? getAffNeighbour : getNonAffNeighbour;

    enc_picture=enc_frame_picture;
    copy_params();
  }


  stats->em_prev_bits_frm = 0;
  stats->em_prev_bits = &stats->em_prev_bits_frm;

  img->fld_flag = 0;
  code_a_picture(frame);

  frame->bits_per_picture = 8 * ((((img->currentSlice)->partArr[0]).bitstream)->byte_pos);

  if (img->structure==FRAME)
  {
    find_distortion ();
    frame->distortion_y = snr->snr_y;
    frame->distortion_u = snr->snr_u;
    frame->distortion_v = snr->snr_v;
  }
}


/*!
 ************************************************************************
 * \brief
 *    Encodes a field picture, consisting of top and bottom field
 ************************************************************************
 */
void field_picture (Picture *top, Picture *bottom)
{
  //Rate control
  int old_pic_type;              // picture type of top field used for rate control
  int TopFieldBits;
  img->SumFrameQP = 0;

  //Rate control
  old_pic_type = img->type;

  stats->em_prev_bits_fld = 0;
  stats->em_prev_bits = &stats->em_prev_bits_fld;
  img->number *= 2;
  img->buf_cycle *= 2;
  img->height = (input->img_height+img->auto_crop_bottom) / 2;
  img->height_cr = img->height_cr_frame / 2;
  img->fld_flag = 1;
  img->PicSizeInMbs = img->FrameSizeInMbs/2;
  // Top field

  enc_top_picture  = alloc_storable_picture ((PictureStructure) img->structure, img->width, img->height, img->width_cr, img->height_cr);
  enc_top_picture->poc=img->toppoc;
  enc_top_picture->frame_poc = img->toppoc;
  enc_top_picture->pic_num = img->frame_num;
  enc_top_picture->frame_num = img->frame_num;
  enc_top_picture->coded_frame = 0;
  enc_top_picture->MbaffFrameFlag = img->MbaffFrameFlag = FALSE;
  get_mb_block_pos = get_mb_block_pos_normal;
  getNeighbour = getNonAffNeighbour;
  img->ThisPOC = img->toppoc;

  img->structure = TOP_FIELD;
  enc_picture = enc_top_picture;
  copy_params();

  put_buffer_top ();
  init_field ();
  if (img->type == B_SLICE)       //all I- and P-frames
    nextP_tr_fld--;


  img->fld_flag = 1;

  //Rate control
  if(input->RCEnable)
  {
    img->BasicUnit=input->basicunit;

    if(input->PicInterlace==FIELD_CODING)
      rc_init_pict(quadratic_RC, 0,1,1, 1.0F);
    else
      rc_init_pict(quadratic_RC, 0,1,0, 1.0F);

    img->qp  = updateQP(quadratic_RC, 1);

    generic_RC->TopFieldFlag=1;
  }

  code_a_picture(top_pic);
  enc_picture->structure = (PictureStructure) 1;

  store_picture_in_dpb(enc_top_picture);

  top->bits_per_picture = 8 * ((((img->currentSlice)->partArr[0]).bitstream)->byte_pos);

  //Rate control
  TopFieldBits=top->bits_per_picture;

  //  Bottom field
  enc_bottom_picture  = alloc_storable_picture ((PictureStructure) img->structure, img->width, img->height, img->width_cr, img->height_cr);
  enc_bottom_picture->poc=img->bottompoc;
  enc_bottom_picture->frame_poc = img->bottompoc;
  enc_bottom_picture->pic_num = img->frame_num;
  enc_bottom_picture->frame_num = img->frame_num;
  enc_bottom_picture->coded_frame = 0;
  enc_bottom_picture->MbaffFrameFlag = img->MbaffFrameFlag = FALSE;
  get_mb_block_pos = get_mb_block_pos_normal;
  getNeighbour = getNonAffNeighbour;

  img->ThisPOC = img->bottompoc;
  img->structure = BOTTOM_FIELD;
  enc_picture = enc_bottom_picture;
  copy_params();
  put_buffer_bot ();
  img->number++;

  init_field ();

  if (img->type == B_SLICE)       //all I- and P-frames
    nextP_tr_fld++;             //check once coding B field

 if (img->type == I_SLICE && input->IntraBottom!=1)
   img->type = (input->BRefPictures == 2) ? B_SLICE : P_SLICE;

  img->fld_flag = 1;

  //Rate control
  if(input->RCEnable)
  {
    quadratic_RC->bits_topfield = TopFieldBits;
    rc_init_pict(quadratic_RC, 0,0,0, 1.0F);
    img->qp  = updateQP(quadratic_RC, 0);
    generic_RC->TopFieldFlag = 0;
  }

  enc_picture->structure = (PictureStructure) 2;
  code_a_picture(bottom_pic);

  bottom->bits_per_picture = 8 * ((((img->currentSlice)->partArr[0]).bitstream)->byte_pos);

  // the distortion for a field coded frame (consisting of top and bottom field)
  // lives in the top->distortion variables, the bottom-> are dummies
  distortion_fld (&top->distortion_y, &top->distortion_u, &top->distortion_v);

}


/*!
 ************************************************************************
 * \brief
 *    Distortion Field
 ************************************************************************
 */
static void distortion_fld (float *dis_fld_y, float *dis_fld_u, float *dis_fld_v)
{

  img->number /= 2;
  img->buf_cycle /= 2;
  img->height = (input->img_height+img->auto_crop_bottom);
  img->height_cr = img->height_cr_frame;

  combine_field ();

  imgY_org = imgY_org_frm;
  imgUV_org = imgUV_org_frm;

  find_distortion ();   // find snr from original frame picture

  *dis_fld_y = snr->snr_y;
  *dis_fld_u = snr->snr_u;
  *dis_fld_v = snr->snr_v;
}


/*!
 ************************************************************************
 * \brief
 *    Picture Structure Decision
 ************************************************************************
 */
static int picture_structure_decision (Picture *frame, Picture *top, Picture *bot)
{
  double lambda_picture;
  int bframe = (img->type == B_SLICE);
  float snr_frame, snr_field;
  int bit_frame, bit_field;

  lambda_picture = 0.68 * pow (2, img->bitdepth_lambda_scale + ((img->qp - SHIFT_QP) / 3.0)) * (bframe ? 1 : 1);

  snr_frame = frame->distortion_y + frame->distortion_u + frame->distortion_v;
  //! all distrortions of a field picture are accumulated in the top field
  snr_field = top->distortion_y + top->distortion_u + top->distortion_v;
  bit_field = top->bits_per_picture + bot->bits_per_picture;
  bit_frame = frame->bits_per_picture;
  return decide_fld_frame (snr_frame, snr_field, bit_field, bit_frame, lambda_picture);
}


/*!
 ************************************************************************
 * \brief
 *    Field Mode Buffer
 ************************************************************************
 */
static void field_mode_buffer (int bit_field, float snr_field_y, float snr_field_u, float snr_field_v)
{
  put_buffer_frame ();

  snr->snr_y = snr_field_y;
  snr->snr_u = snr_field_u;
  snr->snr_v = snr_field_v;
}


/*!
 ************************************************************************
 * \brief
 *    Frame Mode Buffer
 ************************************************************************
 */
static void frame_mode_buffer (int bit_frame, float snr_frame_y, float snr_frame_u, float snr_frame_v)
{
  put_buffer_frame ();

  if ((input->PicInterlace != FRAME_CODING)||(input->MbInterlace != FRAME_CODING))
  {
    img->height = img->height / 2;
    img->height_cr = img->height_cr / 2;
    img->number *= 2;

    put_buffer_top ();

    img->number++;
    put_buffer_bot ();

    img->number /= 2;         // reset the img->number to field
    img->height = (input->img_height+img->auto_crop_bottom);
    img->height_cr = img->height_cr_frame;

    snr->snr_y = snr_frame_y;
    snr->snr_u = snr_frame_u;
    snr->snr_v = snr_frame_v;
    put_buffer_frame ();

  }
}


/*!
 ************************************************************************
 * \brief
 *    mmco initializations should go here
 ************************************************************************
 */
static void init_dec_ref_pic_marking_buffer(void)
{
  img->dec_ref_pic_marking_buffer=NULL;
}


/*!
 ************************************************************************
 * \brief
 *    Initializes the parameters for a new frame
 ************************************************************************
 */
static void init_frame (void)
{
  int i;
  int prevP_no, nextP_no;

  last_P_no = last_P_no_frm;

  img->current_mb_nr = 0;
  img->current_slice_nr = 0;
  stats->bit_slice = 0;

  img->mb_y = img->mb_x = 0;
  img->block_y = img->pix_y = img->pix_c_y = 0;
  img->block_x = img->pix_x = img->block_c_x = img->pix_c_x = 0;

  // The 'slice_nr' of each macroblock is set to -1 here, to guarantee the correct encoding
  // with FMO (if no FMO, encoding is correct without following assignment),
  // for which MBs may not be encoded with scan order
  for(i=0;i< ((int) (img->FrameSizeInMbs));i++)
    img->mb_data[i].slice_nr=-1;

  if (img->b_frame_to_code == 0)
  {
    img->tr = start_tr_in_this_IGOP + IMG_NUMBER * (input->jumpd + 1);

    img->imgtr_last_P_frm = img->imgtr_next_P_frm;
    img->imgtr_next_P_frm = img->tr;

#ifdef _ADAPT_LAST_GROUP_
    if (input->last_frame && img->number + 1 == input->no_frames)
      img->tr = input->last_frame;
#endif

    if (IMG_NUMBER != 0 && input->successive_Bframe != 0)     // B pictures to encode
      nextP_tr_frm = img->tr;

    //Rate control
    if(!input->RCEnable)                  // without using rate control
    {
      if (img->type == I_SLICE)
      {
#ifdef _CHANGE_QP_
        //QP oscillation for secondary SP frames
        if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
          ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
          img->qp = input->qp02;
        else
#endif
          img->qp = input->qp0;   // set quant. parameter for I-frame
      }
      else
      {
#ifdef _CHANGE_QP_
        //QP oscillation for secondary SP frames
        if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
          ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
          img->qp = input->qpN2 + (img->nal_reference_idc ? 0 : input->DispPQPOffset);
        else
#endif
          img->qp = input->qpN + (img->nal_reference_idc ? 0 : input->DispPQPOffset);

        if (img->type == SP_SLICE)
        {
          if ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ))
          {
            img->qp = input->qpN2-(input->qpN-input->qpsp);
            img->qpsp = input->qpN2-(input->qpN-input->qpsp_pred);
          }
          else
          {
            img->qp = input->qpsp;
            img->qpsp = input->qpsp_pred;
          }
        }
      }
    }

    img->mb_y_intra = img->mb_y_upd;  //  img->mb_y_intra indicates which GOB to intra code for this frame

    if (input->intra_upd > 0) // if error robustness, find next GOB to update
    {
      img->mb_y_upd = (IMG_NUMBER / input->intra_upd) % (img->height / MB_BLOCK_SIZE);
    }
  }
  else
  {
    img->p_interval = input->jumpd + 1;
    prevP_no = start_tr_in_this_IGOP + (IMG_NUMBER - 1) * img->p_interval;
    nextP_no = start_tr_in_this_IGOP + (IMG_NUMBER) * img->p_interval;

#ifdef _ADAPT_LAST_GROUP_
    last_P_no[0] = prevP_no;
    for (i = 1; i < img->buf_cycle; i++)
      last_P_no[i] = last_P_no[i - 1] - img->p_interval;

    if (input->last_frame && img->number + 1 == input->no_frames)
    {
      nextP_no = input->last_frame;
      img->p_interval = nextP_no - prevP_no;
    }
#endif

    img->b_interval =
      ((double) (input->jumpd + 1) / (input->successive_Bframe + 1.0) );

    if (input->HierarchicalCoding == 3)
      img->b_interval = 1.0;

    if (input->HierarchicalCoding)
      img->tr = prevP_no + (int) (img->b_interval  * (double) (1 + gop_structure[img->b_frame_to_code - 1].display_no));      // from prev_P
    else
      img->tr = prevP_no + (int) (img->b_interval * (double) img->b_frame_to_code);      // from prev_P


    if (img->tr >= nextP_no)
      img->tr = nextP_no - 1;
    //Rate control
    if(!input->RCEnable && input->HierarchicalCoding == 0)                  // without using rate control
    {
#ifdef _CHANGE_QP_
      //QP oscillation for secondary SP frames
      if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
        ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
      {
        img->qp = input->qpB2;
      }
      else
#endif
      {
        img->qp = input->qpB;
      }

      if (img->nal_reference_idc)
      {
#ifdef _CHANGE_QP_
        //QP oscillation for secondary SP frames
        if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
          ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
        {
          img->qp = iClip3(-img->bitdepth_luma_qp_scale,51,input->qpB2 + input->qpBRS2Offset);
        }
        else
#endif
        {
          img->qp = iClip3(-img->bitdepth_luma_qp_scale,51,input->qpB + input->qpBRSOffset);
        }
      }
    }
    else if (!input->RCEnable && input->HierarchicalCoding !=0)
    {
      // Note that _CHANGE_QP_ does not anymore work for gop_structure. Needs to be fixed
      img->qp =  gop_structure[img->b_frame_to_code - 1].slice_qp;
    }
  }
  img->qp_scaled = img->qp + img->bitdepth_luma_qp_scale;

  UpdateSubseqInfo (img->layer);        // Tian Dong (Sept 2002)
  UpdateSceneInformation (FALSE, 0, 0, -1); // JVT-D099, scene information SEI, nothing included by default

  //! Commented out by StW, needs fixing in SEI.h to keep the trace file clean
  //  PrepareAggregationSEIMessage ();

  img->no_output_of_prior_pics_flag = 0;
  img->long_term_reference_flag = 0;

  init_dec_ref_pic_marking_buffer();
}

/*!
 ************************************************************************
 * \brief
 *    Initializes the parameters for a new field
 ************************************************************************
 */
static void init_field (void)
{
  int i;
  int prevP_no, nextP_no;

  last_P_no = last_P_no_fld;

  img->current_mb_nr = 0;
  img->current_slice_nr = 0;
  stats->bit_slice = 0;

  input->jumpd *= 2;
  input->successive_Bframe *= 2;
  img->number /= 2;
  img->buf_cycle /= 2;

  img->mb_y = img->mb_x = 0;
  img->block_y = img->pix_y = img->pix_c_y = 0; // define vertical positions
  img->block_x = img->pix_x = img->block_c_x = img->pix_c_x = 0;        // define horizontal positions

  if (!img->b_frame_to_code)
  {
    img->tr = img->number * (input->jumpd + 2) + img->fld_type;

    if (!img->fld_type)
    {
      img->imgtr_last_P_fld = img->imgtr_next_P_fld;
      img->imgtr_next_P_fld = img->tr;
    }

#ifdef _ADAPT_LAST_GROUP_
    if (input->last_frame && img->number + 1 == input->no_frames)
      img->tr = input->last_frame;
#endif
    if (img->number != 0 && input->successive_Bframe != 0)    // B pictures to encode
      nextP_tr_fld = img->tr;

      //Rate control
    if(!input->RCEnable)                  // without using rate control
    {
      if (img->type == I_SLICE)
      {
#ifdef _CHANGE_QP_
        //QP oscillation for secondary SP frames
        if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
          ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
          img->qp = input->qp02;
        else
#endif
          img->qp = input->qp0;   // set quant. parameter for I-frame
      }
      else
      {
#ifdef _CHANGE_QP_
        //QP oscillation for secondary SP frames
        if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
          ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
          img->qp = input->qpN2 + (img->nal_reference_idc ? 0 : input->DispPQPOffset);
        else
#endif
          img->qp = input->qpN + (img->nal_reference_idc ? 0 : input->DispPQPOffset);
        if (img->type == SP_SLICE)
        {
          if ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ))
          {
            img->qp = input->qpN2-(input->qpN-input->qpsp);
            img->qpsp = input->qpN2-(input->qpN-input->qpsp_pred);
          }
          else
          {
            img->qp = input->qpsp;
            img->qpsp = input->qpsp_pred;
          }
        }
      }
    }
    img->mb_y_intra = img->mb_y_upd;  //  img->mb_y_intra indicates which GOB to intra code for this frame

    if (input->intra_upd > 0) // if error robustness, find next GOB to update
    {
      img->mb_y_upd =
        (img->number / input->intra_upd) % (img->width / MB_BLOCK_SIZE);
    }
  }
  else
  {
    img->p_interval = input->jumpd + 2;
    prevP_no = (img->number - 1) * img->p_interval + img->fld_type;
    nextP_no = img->number * img->p_interval + img->fld_type;
#ifdef _ADAPT_LAST_GROUP_
    if (!img->fld_type)       // top field
    {
      last_P_no[0] = prevP_no + 1;
      last_P_no[1] = prevP_no;
      for (i = 1; i <= img->buf_cycle; i++)
      {
        last_P_no[2 * i] = last_P_no[2 * i - 2] - img->p_interval;
        last_P_no[2 * i + 1] = last_P_no[2 * i - 1] - img->p_interval;
      }
    }
    else                      // bottom field
    {
      last_P_no[0] = nextP_no - 1;
      last_P_no[1] = prevP_no;
      for (i = 1; i <= img->buf_cycle; i++)
      {
        last_P_no[2 * i] = last_P_no[2 * i - 2] - img->p_interval;
        last_P_no[2 * i + 1] = last_P_no[2 * i - 1] - img->p_interval;
      }
    }

    if (input->last_frame && img->number + 1 == input->no_frames)
    {
      nextP_no = input->last_frame;
      img->p_interval = nextP_no - prevP_no;
    }
#endif
    img->b_interval =
      ((double) (input->jumpd + 1) / (input->successive_Bframe + 1.0) );

    if (input->HierarchicalCoding == 3)
      img->b_interval = 1.0;

    if (input->HierarchicalCoding)
      img->tr = prevP_no + (int) ((img->b_interval + 1.0) * (double) (1 + gop_structure[img->b_frame_to_code - 1].display_no));      // from prev_P
    else
      img->tr = prevP_no + (int) ((img->b_interval + 1.0) * (double) img->b_frame_to_code);      // from prev_P


    if (img->tr >= nextP_no)
      img->tr = nextP_no - 1; // ?????
    //Rate control
    if(!input->RCEnable && input->HierarchicalCoding == 0)                  // without using rate control
    {
#ifdef _CHANGE_QP_
      //QP oscillation for secondary SP frames
      if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
        ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
      {
        img->qp = input->qpB2;
      }
      else
#endif
        img->qp = input->qpB;
      if (img->nal_reference_idc)
      {
#ifdef _CHANGE_QP_
        //QP oscillation for secondary SP frames
        if ((input->qp2start > 0 && img->tr >= input->qp2start && input->sp2_frame_indicator==0)||
          ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ) && (input->sp2_frame_indicator==1)))
        {
          img->qp = iClip3(-img->bitdepth_luma_qp_scale,51,input->qpB2 + input->qpBRS2Offset);
        }
        else
#endif
          img->qp = iClip3(-img->bitdepth_luma_qp_scale,51,input->qpB + input->qpBRSOffset);

      }
    }
    else if (!input->RCEnable && input->HierarchicalCoding != 0)
    {
      img->qp =  gop_structure[img->b_frame_to_code - 1].slice_qp;
    }
  }
  img->qp_scaled = img->qp + img->bitdepth_luma_qp_scale;
  input->jumpd /= 2;
  input->successive_Bframe /= 2;
  img->buf_cycle *= 2;
  img->number = 2 * img->number + img->fld_type;
}



/*!
 ************************************************************************
 * \brief
 *    Upsample 4 times, store them in out4x.  Color is simply copied
 *
 * \par Input:
 *    srcy, srcu, srcv, out4y, out4u, out4v
 *
 * \par Side Effects_
 *    Uses (writes) img4Y_tmp.  This should be moved to a static variable
 *    in this module
 ************************************************************************/
void UnifiedOneForthPix (StorablePicture *s)
{
  int ypadded_size = s->size_y + 2 * IMG_PAD_SIZE;
  int xpadded_size = s->size_x + 2 * IMG_PAD_SIZE;

  // don't upsample twice
  if (s->imgY_sub)
    return;
  // Y component
  get_mem4Dpel (&(s->imgY_sub), 4, 4, ypadded_size, xpadded_size);
  if (NULL == s->imgY_sub)
    no_mem_exit("alloc_storable_picture: s->imgY_sub");

  if ( input->ChromaMCBuffer )
  {
    // UV components
    if ( img->yuv_format != YUV400 )
    {
      if ( img->yuv_format == YUV420 )
      {
        get_mem5Dpel (&(s->imgUV_sub), 2, 8, 8, ypadded_size/2, xpadded_size/2);
      }
      else if ( img->yuv_format == YUV422 )
      {
        get_mem5Dpel (&(s->imgUV_sub), 2, 4, 8, ypadded_size, xpadded_size/2);
      }
      else
      { // YUV444
        get_mem5Dpel (&(s->imgUV_sub), 2, 4, 4, ypadded_size, xpadded_size);
      }
    }
  }

  // derive the subpixel images for first component
  getSubImagesLuma ( s );
  // and the sub-images for U and V
  if ( img->yuv_format != YUV400 && input->ChromaMCBuffer )
    getSubImagesChroma( s );
}


/*!
 ************************************************************************
 * \brief
 *    Find SNR for all three components
 ************************************************************************
 */
static void find_snr (void)
{
  int i, j;
  int64 diff_y=0, diff_u=0, diff_v=0;
  int impix;
  int impix_cr;
  unsigned int max_pix_value_sqd = img->max_imgpel_value * img->max_imgpel_value;
  unsigned int max_pix_value_sqd_uv = img->max_imgpel_value_uv * img->max_imgpel_value_uv;

  //  Calculate  PSNR for Y, U and V.

  //     Luma.
  impix = input->img_height * input->img_width;
  impix_cr = input->img_height_cr * input->img_width_cr;

  if (img->fld_flag != 0)
  {

    diff_y = 0;
    for (i = 0; i < input->img_width; ++i)
    {
      for (j = 0; j < input->img_height; ++j)
      {
        diff_y += img->quad[imgY_org[j][i] - imgY_com[j][i]];
      }
    }

    if (img->yuv_format != YUV400)
    {
      //     Chroma.
      diff_u = 0;
      diff_v = 0;

      for (i = 0; i < input->img_width_cr; i++)
      {
        for (j = 0; j < input->img_height_cr; j++)
        {
          diff_u += img->quad[imgUV_org[0][j][i] - imgUV_com[0][j][i]];
          diff_v += img->quad[imgUV_org[1][j][i] - imgUV_com[1][j][i]];
        }
      }
    }
  }
  else
  {
    imgY_org  = imgY_org_frm;
    imgUV_org = imgUV_org_frm;

    if(input->PicInterlace==ADAPTIVE_CODING)
    {
      enc_picture = enc_frame_picture;
    }

    diff_y = 0;
    for (i = 0; i < input->img_width; ++i)
    {
      for (j = 0; j < input->img_height; ++j)
      {
        diff_y += img->quad[imgY_org[j][i] - enc_picture->imgY[j][i]];
      }
    }

    if (img->yuv_format != YUV400)
    {
      //     Chroma.
      diff_u = 0;
      diff_v = 0;

      for (i = 0; i < input->img_width_cr; i++)
      {
        for (j = 0; j < input->img_height_cr; j++)
        {
          diff_u += img->quad[imgUV_org[0][j][i] - enc_picture->imgUV[0][j][i]];
          diff_v += img->quad[imgUV_org[1][j][i] - enc_picture->imgUV[1][j][i]];
        }
      }
    }
  }
  snr->sse_y = (float)diff_y;
  snr->sse_u = (float)diff_u;
  snr->sse_v = (float)diff_v;

#if ZEROSNR
  if (diff_y == 0)
    diff_y = 1;
  if (diff_u == 0)
    diff_u = 1;
  if (diff_v == 0)
    diff_v = 1;
#endif

  //  Collecting SNR statistics
  if (diff_y != 0)
  {
    snr->snr_y = (float) (10 * log10 (max_pix_value_sqd * (double)((double) impix    / diff_y)));         // luma snr for current frame
    if (img->yuv_format != YUV400)
    {
      snr->snr_u = (float) (10 * log10 (max_pix_value_sqd_uv * (double)((double) impix_cr / diff_u)));   // u croma snr for current frame, 1/4 of luma samples
      snr->snr_v = (float) (10 * log10 (max_pix_value_sqd_uv * (double)((double) impix_cr / diff_v)));   // v croma snr for current frame, 1/4 of luma samples
    }
    else
    {
      snr->snr_u = 0.0;
      snr->snr_v = 0.0;
    }
  }


  if (img->number == 0)
  {
    snr->snr_y1 = snr->snr_y;  // keep luma snr for first frame
    snr->snr_u1 = snr->snr_u;  // keep croma u snr for first frame
    snr->snr_v1 = snr->snr_v;  // keep croma v snr for first frame
    snr->snr_ya = snr->snr_y1;
    snr->snr_ua = snr->snr_u1;
    snr->snr_va = snr->snr_v1;
    // sse stats
    snr->msse_y = snr->sse_y;
    snr->msse_u = snr->sse_u;
    snr->msse_v = snr->sse_v;
    for (i=0; i<5; i++)
    {
      snr->snr_yt[i] = 0.0;
      snr->snr_ut[i] = 0.0;
      snr->snr_vt[i] = 0.0;
    }
  }
  else
  {
    //int total_frames = img->number + frame_ctr[B_SLICE];
    int total_frames = snr->frame_ctr - 1;

    snr->snr_ya = (float) (snr->snr_ya * total_frames + snr->snr_y) / (total_frames + 1); // average snr luma for all frames inc. first
    snr->snr_ua = (float) (snr->snr_ua * total_frames + snr->snr_u) / (total_frames + 1); // average snr u croma for all frames inc. first
    snr->snr_va = (float) (snr->snr_va * total_frames + snr->snr_v) / (total_frames + 1); // average snr v croma for all frames inc. first
    snr->msse_y = (float) (snr->msse_y * total_frames + snr->sse_y) / (total_frames + 1); // average sse luma for all frames inc. first
    snr->msse_u = (float) (snr->msse_u * total_frames + snr->sse_u) / (total_frames + 1); // average sse u croma for all frames inc. first
    snr->msse_v = (float) (snr->msse_v * total_frames + snr->sse_v) / (total_frames + 1); // average sse v croma for all frames inc. first
  }

  snr->snr_yt[img->type] = (float) (snr->snr_yt[img->type] * (frame_ctr[img->type] - 1) + snr->snr_y) / ( frame_ctr[img->type] );  // average luma snr for img->type coded frames
  snr->snr_ut[img->type] = (float) (snr->snr_ut[img->type] * (frame_ctr[img->type] - 1) + snr->snr_u) / ( frame_ctr[img->type] );  // average chroma u snr for img->type coded frames
  snr->snr_vt[img->type] = (float) (snr->snr_vt[img->type] * (frame_ctr[img->type] - 1) + snr->snr_v) / ( frame_ctr[img->type] );  // average chroma v snr for img->type coded frames
}

/*!
 ************************************************************************
 * \brief
 *    Find distortion for all three components
 ************************************************************************
 */
static void find_distortion (void)
{
  int i, j;
  int64 diff_y, diff_u, diff_v;
  int impix;

  //  Calculate  PSNR for Y, U and V.

  //     Luma.
  impix = input->img_height * input->img_width;

  if (img->structure!=FRAME)
  {

    diff_y = 0;
    for (i = 0; i < input->img_width; ++i)
    {
      for (j = 0; j < input->img_height; ++j)
      {
        diff_y += img->quad[imgY_org[j][i] - imgY_com[j][i]];
      }
    }

    diff_u = 0;
    diff_v = 0;

    if (img->yuv_format != YUV400)
    {
      //     Chroma.
      for (i = 0; i < input->img_width_cr; i++)
      {
        for (j = 0; j < input->img_height_cr; j++)
        {
          diff_u += img->quad[imgUV_org[0][j][i] - imgUV_com[0][j][i]];
          diff_v += img->quad[imgUV_org[1][j][i] - imgUV_com[1][j][i]];
        }
      }
    }
  }
  else
  {
    imgY_org  = imgY_org_frm;
    imgUV_org = imgUV_org_frm;

    diff_y = 0;
    for (i = 0; i < input->img_width; ++i)
    {
      for (j = 0; j < input->img_height; ++j)
      {
        diff_y += img->quad[imgY_org[j][i] - enc_picture->imgY[j][i]];
      }
    }

    diff_u = 0;
    diff_v = 0;

    if (img->yuv_format != YUV400)
    {
      //     Chroma.
      for (i = 0; i < input->img_width_cr; i++)
      {
        for (j = 0; j < input->img_height_cr; j++)
        {
          diff_u += img->quad[imgUV_org[0][j][i] - enc_picture->imgUV[0][j][i]];
          diff_v += img->quad[imgUV_org[1][j][i] - enc_picture->imgUV[1][j][i]];
        }
      }
    }
  }
  // Calculate real PSNR at find_snr_avg()
  snr->snr_y = (float) diff_y;
  snr->snr_u = (float) diff_u;
  snr->snr_v = (float) diff_v;
}


  /*!
 ************************************************************************
 * \brief
 *    Just a placebo
 ************************************************************************
 */
Boolean dummy_slice_too_big (int bits_slice)
{
  return FALSE;
}


/*!
***************************************************************************
// For MB level field/frame coding
***************************************************************************
*/
void copy_rdopt_data (int bot_block)
{
  int mb_nr = img->current_mb_nr;
  Macroblock *currMB = &img->mb_data[mb_nr];
  int i, j, k;

  int bframe = (img->type == B_SLICE);
  int mode;
  int b8mode, b8pdir;
  int block_y;

  int list_offset = currMB->list_offset;

  mode                = rdopt->mode;
  currMB->mb_type     = rdopt->mb_type;    // copy mb_type
  currMB->cbp         = rdopt->cbp;        // copy cbp
  currMB->cbp_blk     = rdopt->cbp_blk;    // copy cbp_blk
  currMB->bi_pred_me  = rdopt->bi_pred_me; // copy biprediction
  img->i16offset      = rdopt->i16offset;

  currMB->prev_qp=rdopt->prev_qp;
  currMB->prev_delta_qp=rdopt->prev_delta_qp;
  currMB->prev_cbp = rdopt->prev_cbp;
  currMB->delta_qp = rdopt->delta_qp;

  currMB->qp = rdopt->qp;
  set_chroma_qp(currMB);

  currMB->c_ipred_mode = rdopt->c_ipred_mode;

  for (i = 0; i < 4+img->num_blk8x8_uv; i++)
  {
    for (j = 0; j < 4; j++)
      for (k = 0; k < 2; k++)
        memcpy(img->cofAC[i][j][k],rdopt->cofAC[i][j][k], 65 * sizeof(int));
  }

  for (i = 0; i < 3; i++)
  {
    for (k = 0; k < 2; k++)
      memcpy(img->cofDC[i][k],rdopt->cofDC[i][k], 18 * sizeof(int));
  }

  for (j = 0; j < BLOCK_MULTIPLE; j++)
  {
    block_y = img->block_y + j;
    memcpy(&enc_picture->ref_idx[LIST_0][block_y][img->block_x], rdopt->refar[LIST_0][j], BLOCK_MULTIPLE * sizeof(char));
    for (i = 0; i < BLOCK_MULTIPLE; i++)
      enc_picture->ref_pic_id [LIST_0][block_y][img->block_x + i] =
      enc_picture->ref_pic_num[LIST_0 + list_offset][(short)enc_picture->ref_idx[LIST_0][block_y][img->block_x+i]];
  }
  if (bframe)
  {
    for (j = 0; j < BLOCK_MULTIPLE; j++)
    {
      block_y = img->block_y + j;
      memcpy(&enc_picture->ref_idx[LIST_1][block_y][img->block_x], rdopt->refar[LIST_1][j], BLOCK_MULTIPLE * sizeof(char));
      for (i = 0; i < BLOCK_MULTIPLE; i++)
        enc_picture->ref_pic_id [LIST_1][block_y][img->block_x + i] =
        enc_picture->ref_pic_num[LIST_1 + list_offset][(short)enc_picture->ref_idx[LIST_1][block_y][img->block_x+i]];
    }
  }

  //===== reconstruction values =====
  for (j = 0; j < MB_BLOCK_SIZE; j++)
    memcpy(&enc_picture->imgY[img->pix_y + j][img->pix_x],rdopt->rec_mbY[j], MB_BLOCK_SIZE * sizeof(imgpel));

  if (img->yuv_format != YUV400)
  {
    for (j = 0; j < img->mb_cr_size_y; j++)
    {
      memcpy(&enc_picture->imgUV[0][img->pix_c_y + j][img->pix_c_x],rdopt->rec_mbU[j], img->mb_cr_size_x * sizeof(imgpel));
      memcpy(&enc_picture->imgUV[1][img->pix_c_y + j][img->pix_c_x],rdopt->rec_mbV[j], img->mb_cr_size_x * sizeof(imgpel));
    }
  }

  memcpy(currMB->b8mode,rdopt->b8mode, 4 * sizeof(int));
  memcpy(currMB->b8pdir,rdopt->b8pdir, 4 * sizeof(int));

  currMB->luma_transform_size_8x8_flag = rdopt->luma_transform_size_8x8_flag;

  //==== intra prediction modes ====
  if (mode == P8x8)
  {
    memcpy(currMB->intra_pred_modes,rdopt->intra_pred_modes, MB_BLOCK_PARTITIONS * sizeof(char));
    for (j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
      memcpy(&img->ipredmode[j][img->block_x],&rdopt->ipredmode[j][img->block_x], BLOCK_MULTIPLE * sizeof(char));
  }
  else if (mode != I4MB && mode != I8MB)
  {
    memset(currMB->intra_pred_modes,DC_PRED, MB_BLOCK_PARTITIONS * sizeof(char));
    for (j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
      memset(&img->ipredmode[j][img->block_x],DC_PRED, BLOCK_MULTIPLE * sizeof(char));
  }
  else if (mode == I4MB || mode == I8MB)
  {
    memcpy(currMB->intra_pred_modes,rdopt->intra_pred_modes, MB_BLOCK_PARTITIONS * sizeof(char));
    memcpy(currMB->intra_pred_modes8x8,rdopt->intra_pred_modes8x8, MB_BLOCK_PARTITIONS * sizeof(char));
    for (j = img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++) {
      memcpy(&img->ipredmode[j][img->block_x],&rdopt->ipredmode[j][img->block_x], BLOCK_MULTIPLE * sizeof(char));
    }
  }

  if (img->MbaffFrameFlag)
  {
    // motion vectors
    copy_motion_vectors_MB ();

    if (!IS_INTRA(currMB))
    {
      for (j = 0; j < 4; j++)
        for (i = 0; i < 4; i++)
        {
          b8mode = currMB->b8mode[i/2+2*(j/2)];
          b8pdir = currMB->b8pdir[i/2+2*(j/2)];

          if (b8pdir!=1)
          {
            enc_picture->mv[LIST_0][j+img->block_y][i+img->block_x][0] = rdopt->all_mv[j][i][LIST_0][(short)rdopt->refar[LIST_0][j][i]][b8mode][0];
            enc_picture->mv[LIST_0][j+img->block_y][i+img->block_x][1] = rdopt->all_mv[j][i][LIST_0][(short)rdopt->refar[LIST_0][j][i]][b8mode][1];
          }
          else
          {
            enc_picture->mv[LIST_0][j+img->block_y][i+img->block_x][0] = 0;
            enc_picture->mv[LIST_0][j+img->block_y][i+img->block_x][1] = 0;
          }
          if (bframe)
          {
            if (b8pdir!=0)
            {
              enc_picture->mv[LIST_1][j+img->block_y][i+img->block_x][0] = rdopt->all_mv[j][i][LIST_1][(short)rdopt->refar[LIST_1][j][i]][b8mode][0];
              enc_picture->mv[LIST_1][j+img->block_y][i+img->block_x][1] = rdopt->all_mv[j][i][LIST_1][(short)rdopt->refar[LIST_1][j][i]][b8mode][1];
            }
            else
            {
              enc_picture->mv[LIST_1][j+img->block_y][i+img->block_x][0] = 0;
              enc_picture->mv[LIST_1][j+img->block_y][i+img->block_x][1] = 0;
            }
          }
        }
    }
    else
    {
      for (j = 0; j < 4; j++)
        memset(enc_picture->mv[LIST_0][j+img->block_y][img->block_x], 0, 2 * BLOCK_MULTIPLE * sizeof(short));
      if (bframe)
      {
        for (j = 0; j < 4; j++)
          memset(enc_picture->mv[LIST_1][j+img->block_y][img->block_x], 0, 2 * BLOCK_MULTIPLE * sizeof(short));
      }
    }
  }
}                             // end of copy_rdopt_data

static void copy_motion_vectors_MB (void)
{
  int i,j,k,l;

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      for (k = 0; k < img->max_num_references; k++)
      {
        for (l = 0; l < 9; l++)
        {
          img->all_mv[j][i][LIST_0][k][l][0] = rdopt->all_mv[j][i][LIST_0][k][l][0];
          img->all_mv[j][i][LIST_0][k][l][1] = rdopt->all_mv[j][i][LIST_0][k][l][1];

          img->all_mv[j][i][LIST_1][k][l][0] = rdopt->all_mv[j][i][LIST_1][k][l][0];
          img->all_mv[j][i][LIST_1][k][l][1] = rdopt->all_mv[j][i][LIST_1][k][l][1];

          img->pred_mv[j][i][LIST_0][k][l][0] = rdopt->pred_mv[j][i][LIST_0][k][l][0];
          img->pred_mv[j][i][LIST_0][k][l][1] = rdopt->pred_mv[j][i][LIST_0][k][l][1];

          img->pred_mv[j][i][LIST_1][k][l][0] = rdopt->pred_mv[j][i][LIST_1][k][l][0];
          img->pred_mv[j][i][LIST_1][k][l][1] = rdopt->pred_mv[j][i][LIST_1][k][l][1];
        }
      }
    }
  }
}


static void ReportNALNonVLCBits(time_t tmp_time, time_t me_time)
{

  //! Need to add type (i.e. SPS, PPS, SEI etc).
  if (input->Verbose != 0)
  printf ("%04d(NVB)%8d \n", frame_no, 0);
}
static void ReportFirstframe(time_t tmp_time, time_t me_time)
{
  //Rate control
  int bits;

  if (input->Verbose == 1)
  {
    printf ("%04d(IDR)%8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
      frame_no, 0,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
  }
  else if (input->Verbose == 2)
  {
    printf ("%04d(IDR)%8d %1d %2d %7.3f %7.3f %7.3f %9d %7d    %3s %5d   %2d %2d  %d   %d\n",
      frame_no,0,0,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", intras, img->num_ref_idx_l0_active, img->num_ref_idx_l1_active,img->rd_pass, img->nal_reference_idc);
  }
  //Rate control
  if(input->RCEnable)
  {
    if((!input->PicInterlace) && (!input->MbInterlace))
      bits = (int) (stats->bit_ctr - stats->bit_ctr_n); // used for rate control update
    else
    {
      bits = (int)(stats->bit_ctr - quadratic_RC->Iprev_bits); // used for rate control update
      quadratic_RC->Iprev_bits = stats->bit_ctr;
    }
  }

  stats->bit_ctr_I = stats->bit_ctr;
  stats->bit_ctr = 0;

}


static void ReportIntra(time_t tmp_time, time_t me_time)
{
  if (input->Verbose == 1)
  {
   if (img->currentPicture->idr_flag == 1)
      printf ("%04d(IDR)%8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
      frame_no, 0,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
    else
      printf ("%04d(I)  %8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
      frame_no, 0,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
  }
  else if (input->Verbose == 2)
  {
    if (img->currentPicture->idr_flag == 1)
      printf ("%04d(IDR)%8d %1d %2d %7.3f %7.3f %7.3f %9d %7d    %3s %5d   %2d %2d  %d   %d\n",
      frame_no, 0, 0,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", intras, img->num_ref_idx_l0_active, img->num_ref_idx_l1_active,img->rd_pass, img->nal_reference_idc);
    else
      printf ("%04d(I)  %8d %1d %2d %7.3f %7.3f %7.3f %9d %7d    %3s %5d   %2d %2d  %d   %d\n",
      frame_no, 0, 0,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", intras, img->num_ref_idx_l0_active, img->num_ref_idx_l1_active,img->rd_pass, img->nal_reference_idc);
  }
}

static void ReportSP(time_t tmp_time, time_t me_time)
{
  if (input->Verbose == 1)
  {
    printf ("%04d(SP) %8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
      frame_no, 0,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
  }
  else if (input->Verbose == 2)
  {

    printf ("%04d(SP) %8d %1d %2d %7.3f %7.3f %7.3f %9d %7d    %3s %5d   %2d %2d  %d   %d\n",
      frame_no, 0, active_pps->weighted_pred_flag,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", intras, img->num_ref_idx_l0_active, img->num_ref_idx_l1_active,img->rd_pass, img->nal_reference_idc);
  }
}

static void ReportB(time_t tmp_time, time_t me_time)
{
  if (input->Verbose == 1)
  {
    printf ("%04d(B)  %8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
    frame_no, 0,
    img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
    img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
  }
  else if (input->Verbose == 2)
  {
    printf ("%04d(B)  %8d %1d %2d %7.3f %7.3f %7.3f %9d %7d    %3s %5d %1d %2d %2d  %d   %d\n",
    frame_no, 0, active_pps->weighted_bipred_idc,
    img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
    img->fld_flag ? "FLD" : "FRM",intras,img->direct_spatial_mv_pred_flag, img->num_ref_idx_l0_active, img->num_ref_idx_l1_active,img->rd_pass, img->nal_reference_idc);
  }
}


static void ReportP(time_t tmp_time, time_t me_time)
{
  if (input->Verbose == 1)
  {
    if(input->redundant_pic_flag==0)
    {
      printf ("%04d(P)  %8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
        frame_no, 0,
        img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
        img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
    }
    else
    {
      if(!redundant_coding)
      {
        printf ("%04d(P)  %8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
          frame_no, 0,
          img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
          img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
      }
      else
      { // report a redundant picture.
        printf ("    (R)  %8d   %2d %7.3f %7.3f %7.3f %9d %7d    %3s    %d\n",
          0,
          img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
          img->fld_flag ? "FLD" : "FRM", img->nal_reference_idc);
      }
    }
  }
  else if (input->Verbose == 2)
  {
    printf ("%04d(P)  %8d %1d %2d %7.3f %7.3f %7.3f %9d %7d    %3s %5d   %2d %2d  %d   %d\n",
      frame_no, 0, active_pps->weighted_pred_flag,
      img->AverageFrameQP, snr->snr_y, snr->snr_u, snr->snr_v, 0, 0,
      img->fld_flag ? "FLD" : "FRM", intras, img->num_ref_idx_l0_active, img->num_ref_idx_l1_active, img->rd_pass, img->nal_reference_idc);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Padding of automatically added border for picture sizes that are not
 *     multiples of macroblock/macroblock pair size
 *
 * \param org_size_x
 *    original image horizontal size (luma)
 * \param org_size_y
 *    original image vertical size (luma)
 * \param img_size_x
 *    coded image horizontal size (luma)
 * \param img_size_y
 *    code image vertical size (luma)
 * \param org_size_x_cr
 *    original image horizontal size (chroma)
 * \param org_size_y_cr
 *    original image vertical size (chroma)
 * \param img_size_x_cr
 *    coded image horizontal size (chroma)
 * \param img_size_y_cr
 *    code image vertical size (chroma)
 ************************************************************************
 */
static void PaddAutoCropBorders (int org_size_x, int org_size_y, int img_size_x, int img_size_y,
                                 int org_size_x_cr, int org_size_y_cr, int img_size_x_cr, int img_size_y_cr)
{
  int x, y;

  //padding right border
  for (y=0; y<org_size_y; y++)
    for (x=org_size_x; x<img_size_x; x++)
      imgY_org_frm [y][x] = imgY_org_frm [y][x-1];

  //padding bottom border
  for (y=org_size_y; y<img_size_y; y++)
    for (x=0; x<img_size_x; x++)
      imgY_org_frm [y][x] = imgY_org_frm [y-1][x];


  if (img->yuv_format != YUV400)
  {
    //padding right border
    for (y=0; y<org_size_y_cr; y++)
      for (x=org_size_x_cr; x<img_size_x_cr; x++)
      {
        imgUV_org_frm [0][y][x] = imgUV_org_frm [0][y][x-1];
        imgUV_org_frm [1][y][x] = imgUV_org_frm [1][y][x-1];
      }

    //padding bottom border
    for (y=org_size_y_cr; y<img_size_y_cr; y++)
      for (x=0; x<img_size_x_cr; x++)
      {
        imgUV_org_frm [0][y][x] = imgUV_org_frm [0][y-1][x];
        imgUV_org_frm [1][y][x] = imgUV_org_frm [1][y-1][x];
      }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Calculates the absolute frame number in the source file out
 *    of various variables in img-> and input->
 * \return
 *    frame number in the file to be read
 * \par side effects
 *    global variable frame_no updated -- dunno, for what this one is necessary
 ************************************************************************
 */
static int CalculateFrameNumber(void)
{
  if (img->b_frame_to_code)
  {
    if (input->HierarchicalCoding)
      frame_no = start_tr_in_this_IGOP + (IMG_NUMBER - 1) * (input->jumpd + 1) + (int) (img->b_interval * (double) (1 + gop_structure[img->b_frame_to_code - 1].display_no));
    else
      frame_no = start_tr_in_this_IGOP + (IMG_NUMBER - 1) * (input->jumpd + 1) + (int) (img->b_interval * (double) img->b_frame_to_code);
  }
  else
    {
      frame_no = start_tr_in_this_IGOP + IMG_NUMBER * (input->jumpd + 1);
#ifdef _ADAPT_LAST_GROUP_
      if (input->last_frame && img->number + 1 == input->no_frames)
        frame_no = input->last_frame;
#endif
    }

  return frame_no;
}


/*!
 ************************************************************************
 * \brief
 *    Convert file read buffer to source picture structure
 ************************************************************************
 */
void buf2img ( imgpel** imgX,           //!< Pointer to image plane
               unsigned char* buf,      //!< Buffer for file output
               int size_x,              //!< horizontal size of picture
               int size_y,              //!< vertical size of picture
               int symbol_size_in_bytes //!< number of bytes in file used for one pixel
               )
{
  int i,j;

  unsigned short tmp16, ui16;
  unsigned long  tmp32, ui32;

  if (symbol_size_in_bytes> sizeof(imgpel))
  {
    error ("Source picture has higher bit depth than imgpel data type. Please recompile with larger data type for imgpel.", 500);
  }

  if (( sizeof(char) == sizeof (imgpel)) && ( sizeof(char) == symbol_size_in_bytes))
  {
    // imgpel == pixel_in_file == 1 byte -> simple copy
    for(j=0;j<size_y;j++)
      memcpy(imgX[j], buf+j*size_x, size_x);
  }
  else
  {
    // sizeof (imgpel) > sizeof(char)
    if (testEndian())
    {
      // big endian
      switch (symbol_size_in_bytes)
      {
      case 1:
        {
          for(j=0;j<size_y;j++)
            for(i=0;i<size_x;i++)
            {
              imgX[j][i]= buf[i+j*size_x];
            }
          break;
        }
      case 2:
        {
          for(j=0;j<size_y;j++)
            for(i=0;i<size_x;i++)
            {
              memcpy(&tmp16, buf+((i+j*size_x)*2), 2);
              ui16  = (tmp16 >> 8) | ((tmp16&0xFF)<<8);
              imgX[j][i] = (imgpel) ui16;
            }
          break;
        }
      case 4:
        {
          for(j=0;j<size_y;j++)
            for(i=0;i<size_x;i++)
            {
              memcpy(&tmp32, buf+((i+j*size_x)*4), 4);
              ui32  = ((tmp32&0xFF00)<<8) | ((tmp32&0xFF)<<24) | ((tmp32&0xFF0000)>>8) | ((tmp32&0xFF000000)>>24);
              imgX[j][i] = (imgpel) ui32;
            }
        }
      default:
        {
           error ("reading only from formats of 8, 16 or 32 bit allowed on big endian architecture", 500);
           break;
        }
      }
    }
    else
    {
      // little endian
      for (j=0; j < size_y; j++)
        for (i=0; i < size_x; i++)
        {
          imgX[j][i]=0;
          memcpy(&(imgX[j][i]), buf +((i+j*size_x)*symbol_size_in_bytes), symbol_size_in_bytes);
        }
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Reads one new frame from file
 *
 * \param FrameNoInFile
 *    Frame number in the source file
 * \param HeaderSize
 *    Number of bytes in the source file to be skipped
 * \param xs
 *    horizontal size of frame in pixels
 * \param ys
 *    vertical size of frame in pixels
 * \param xs_cr
 *    horizontal chroma size of frame in pixels
 * \param ys_cr
 *    vertical chroma size of frame in pixels
 ************************************************************************
 */
static void ReadOneFrame (int FrameNoInFile, int HeaderSize, int xs, int ys, int xs_cr, int ys_cr)
{
  unsigned int symbol_size_in_bytes = img->pic_unit_size_on_disk/8;

  const int imgsize_y = xs*ys;
  const int imgsize_uv = xs_cr*ys_cr;

  const int bytes_y = imgsize_y * symbol_size_in_bytes;
  const int bytes_uv = imgsize_uv * symbol_size_in_bytes;

  const int64 framesize_in_bytes = bytes_y + 2*bytes_uv;
  unsigned char *buf;

  Boolean rgb_input = (Boolean) (input->rgb_input_flag==1 && input->yuv_format==3);

  assert (p_in != -1);

  // KS: this buffer should actually be allocated only once, but this is still much faster than the previous version
  if (NULL==(buf = malloc (xs*ys * symbol_size_in_bytes))) no_mem_exit("ReadOneFrame: buf");

  // skip Header
  if (lseek (p_in, HeaderSize, SEEK_SET) != HeaderSize)
  {
    error ("ReadOneFrame: cannot fseek to (Header size) in p_in", -1);
  }

  // skip starting frames
  if (lseek (p_in, framesize_in_bytes * input->start_frame, SEEK_CUR) == -1)
  {
    snprintf(errortext, ET_SIZE, "ReadOneFrame: cannot advance file pointer in p_in beyond frame %d\n", input->start_frame);
    error (errortext,-1);
  }

  // seek to current frame
  if (lseek (p_in, framesize_in_bytes * FrameNoInFile, SEEK_CUR) == -1)
  {
    snprintf(errortext, ET_SIZE, "ReadOneFrame: cannot advance file pointer in p_in beyond frame %d\n", input->start_frame + FrameNoInFile);
    error (errortext,-1);
  }

  // Here we are at the correct position for the source frame in the file.  Now
  // read it.
  if (img->pic_unit_size_on_disk%8 == 0)
  {
    if(rgb_input)
      lseek (p_in, framesize_in_bytes/3, SEEK_CUR);

    if (read(p_in, buf, bytes_y) != bytes_y)
    {
      printf ("ReadOneFrame: cannot read %d bytes from input file, unexpected EOF?, exiting", bytes_y);
      report_stats_on_error();
      exit (-1);
    }

    buf2img(imgY_org_frm, buf, xs, ys, symbol_size_in_bytes);
//#define _DEBUG_BITDEPTH_
#ifdef _DEBUG_BITDEPTH_
    {
      int i,j;
      for (j=0; j < ys; j++)
      {
        for (i=0; i < xs; i++)
        {
          imgY_org_frm[j][i]= (imgpel) (imgY_org_frm[j][i] & ((1<<img->bitdepth_luma ) - 1));
        }
      }
    }
#endif
    if (img->yuv_format != YUV400)
    {
      if (read(p_in, buf, bytes_uv) != bytes_uv)
      {
        printf ("ReadOneFrame: cannot read %d bytes from input file, unexpected EOF?, exiting", bytes_y);
        report_stats_on_error();
        exit (-1);
      }
      buf2img(imgUV_org_frm[0], buf, xs_cr, ys_cr, symbol_size_in_bytes);

      if(rgb_input)
        lseek (p_in, -framesize_in_bytes, SEEK_CUR);

      if (read(p_in, buf, bytes_uv) != bytes_uv)
      {
        printf ("ReadOneFrame: cannot read %d bytes from input file, unexpected EOF?, exiting", bytes_y);
        report_stats_on_error();
        exit (-1);
      }
      buf2img(imgUV_org_frm[1], buf, xs_cr, ys_cr, symbol_size_in_bytes);

#ifdef _DEBUG_BITDEPTH_
      {
        int i,j;

        for (j=0; j < ys_cr; j++)
        {
          for (i=0; i < xs_cr; i++)
          {
            imgUV_org_frm[0][j][i]=(imgpel) (imgUV_org_frm[0][j][i] & ((1<<img->bitdepth_chroma ) - 1));
            imgUV_org_frm[1][j][i]=(imgpel) (imgUV_org_frm[1][j][i] & ((1<<img->bitdepth_chroma ) - 1));
          }
        }
      }
#endif

      if(rgb_input)
        lseek (p_in, framesize_in_bytes*2/3, SEEK_CUR);
    }
  }
  else
  {
    printf ("ReadOneFrame (NOT IMPLEMENTED): pic unit size on disk must be divided by 8");
    exit (-1);
  }
  free (buf);

}


/*!
 ************************************************************************
 * \brief
 *    point to frame coding variables
 ************************************************************************
 */
static void put_buffer_frame(void)
{
  imgY_org  = imgY_org_frm;
  imgUV_org = imgUV_org_frm;
}

/*!
 ************************************************************************
 * \brief
 *    point to top field coding variables
 ************************************************************************
 */
static void put_buffer_top(void)
{
  img->fld_type = 0;

  imgY_org = imgY_org_top;
  imgUV_org = imgUV_org_top;
}

/*!
 ************************************************************************
 * \brief
 *    point to bottom field coding variables
 ************************************************************************
 */
static void put_buffer_bot(void)
{
  img->fld_type = 1;

  imgY_org = imgY_org_bot;
  imgUV_org = imgUV_org_bot;
}

/*!
 ************************************************************************
 * \brief
 *    Writes a NAL unit of a partition or slice
 ************************************************************************
 */

static void writeUnit(Bitstream* currStream, int partition)
{
  const int buffer_size = 500 + img->FrameSizeInMbs * (128 + 256 * img->bitdepth_luma + 512 * img->bitdepth_chroma);
                                                          // KS: this is approx. max. allowed code picture size
  NALU_t *nalu;
  assert (currStream->bits_to_go == 8);
  nalu = AllocNALU(buffer_size);
  nalu->startcodeprefix_len = 1+ (img->current_mb_nr == 0 && partition == 0 ?ZEROBYTES_SHORTSTARTCODE+1:ZEROBYTES_SHORTSTARTCODE);
//printf ("nalu->startcodeprefix_len %d\n", nalu->startcodeprefix_len);
  nalu->len = currStream->byte_pos +1;            // add one for the first byte of the NALU
//printf ("nalu->len %d\n", nalu->len);
  memcpy (&nalu->buf[1], currStream->streamBuffer, nalu->len-1);
  if (img->currentPicture->idr_flag)
  {
    nalu->nal_unit_type = NALU_TYPE_IDR;
    nalu->nal_reference_idc = NALU_PRIORITY_HIGHEST;
  }
  else if (img->type == B_SLICE)
  {
    //different nal header for different partitions
    if(input->partition_mode == 0)
    {
    nalu->nal_unit_type = NALU_TYPE_SLICE;
    }
    else
    {
      nalu->nal_unit_type = NALU_TYPE_DPA +  partition;
    }

    if (img->nal_reference_idc !=0)
    {
      nalu->nal_reference_idc = NALU_PRIORITY_HIGH;
    }
    else
    {
      nalu->nal_reference_idc = NALU_PRIORITY_DISPOSABLE;
    }
  }
  else   // non-b frame, non IDR slice
  {
    //different nal header for different partitions
    if(input->partition_mode == 0)
    {
     nalu->nal_unit_type = NALU_TYPE_SLICE;
    }
    else
    {
     nalu->nal_unit_type = NALU_TYPE_DPA +  partition;
    }
    if (img->nal_reference_idc !=0)
    {
      nalu->nal_reference_idc = NALU_PRIORITY_HIGH;
    }
    else
    {
      nalu->nal_reference_idc = NALU_PRIORITY_DISPOSABLE;
    }
  }
  nalu->forbidden_bit = 0;
  stats->bit_ctr += WriteNALU (nalu);

  FreeNALU(nalu);
}

/*!
 ************************************************************************
 * \brief
 *    performs multi-pass encoding of same picture using different
 *    coding conditions
 ************************************************************************
 */

static void rdPictureCoding(void)
{
  int second_qp = img->qp, rd_qp = img->qp;
  int previntras = intras;
  int prevtype = img->type;
  int skip_encode = 0;
  pic_parameter_set_rbsp_t *sec_pps;
  int tmpFrameQP = img->SumFrameQP;

  if ( input->RCEnable )
  {
    // save the current RC model as preliminary best
    copy_rc_jvt( quadratic_RC_best, quadratic_RC );
    copy_rc_generic( generic_RC_best, generic_RC );
  }

  if (img->type!=I_SLICE && input->GenerateMultiplePPS)
  {
    if (img->type==P_SLICE)
    {
      if (test_wp_P_slice(0) == 1)
      {
        active_pps = PicParSet[1];
      }
      else
      {
        skip_encode = input->RDPSliceWeightOnly;
        active_pps = PicParSet[0];
        if (!img->AdaptiveRounding)
          img->qp-=1;
      }
    }
    else
    {
      active_pps = PicParSet[2];
    }
  }
  else
  {
    if (!img->AdaptiveRounding)
      img->qp-=1;
  }

  sec_pps = active_pps;
  second_qp = img->qp;

  img->write_macroblock = 0;

  if (skip_encode)
  {
    img->rd_pass = 0;
    enc_frame_picture2 = NULL;
  }
  else
  {
    if(input->RCEnable)
    {
      // re-store the initial RC model
      copy_rc_jvt( quadratic_RC, quadratic_RC_init );
      copy_rc_generic( generic_RC, generic_RC_init );

      rc_init_pict(quadratic_RC, 1,0,1, input->GenerateMultiplePPS ? 1.0F : 0.85F );

      img->qp  = updateQP(quadratic_RC, 0);
    }

    frame_picture (frame_pic_2,1);
    img->rd_pass=picture_coding_decision(frame_pic_1, frame_pic_2, rd_qp);
  }
  //      update_rd_picture_contexts (img->rd_pass);
  if (img->rd_pass==0)
  {
    enc_picture=enc_frame_picture;
    if (img->type!=I_SLICE && input->GenerateMultiplePPS)
    {
      img->qp=rd_qp;
      active_pps = PicParSet[0];
    }
    else
    {
      img->qp=rd_qp;
    }
    intras = previntras;
    frame_pic = frame_pic_1;
  }
  else
  {
    previntras = intras;
    frame_pic = frame_pic_2;
    tmpFrameQP = img->SumFrameQP;

    if(input->RCEnable)
    {
      copy_rc_jvt( quadratic_RC_best, quadratic_RC );
      copy_rc_generic( generic_RC_best, generic_RC );
    }
  }
  // Final Encoding pass - note that we should
  // make this more flexible in a later version.

  if (img->type!=I_SLICE )
  {
    skip_encode = 0;
    img->qp    = rd_qp;

    if (img->type == P_SLICE && (intras * 100 )/img->FrameSizeInMbs >=75)
    {
      img->type=I_SLICE;
      active_pps = PicParSet[0];
    }
    else if (img->type==P_SLICE)
    {
      if (input->GenerateMultiplePPS)
      {
        if (test_wp_P_slice(1) == 1)
        {
          active_pps = PicParSet[1];
        }
        else if (input->RDPSliceBTest && active_sps->profile_idc != 66)
        {
          img->type = B_SLICE;
          active_pps = PicParSet[0];
        }
        else
        {
          skip_encode = input->RDPSliceWeightOnly;
          active_pps = PicParSet[0];
          if (!img->AdaptiveRounding)
            img->qp+=1;
        }
      }
    }
    else
    {
      if (input->GenerateMultiplePPS && test_wp_B_slice(0) == 1)
      {
        active_pps = PicParSet[1];
      }
      else
      {
        skip_encode = input->RDBSliceWeightOnly;
        img->qp = rd_qp + (img->nal_reference_idc ? - 1 : 1);
      }
    }
  }
  else
  {
    active_pps = PicParSet[0];
    if (!img->AdaptiveRounding)
      img->qp    = (rd_qp + 1);
  }


  img->write_macroblock = 0;

  if (skip_encode)
  {
    enc_frame_picture3 = NULL;
    img->qp = rd_qp;
  }
  else
  {
    if(input->RCEnable)
    {
      // re-store the initial RC model
      copy_rc_jvt( quadratic_RC, quadratic_RC_init );
      copy_rc_generic( generic_RC, generic_RC_init );

      rc_init_pict(quadratic_RC, 1,0,1, input->GenerateMultiplePPS ? 1.0F : 1.15F );

      img->qp = updateQP(quadratic_RC, 0);
    }

    frame_picture (frame_pic_3,2);

    if (img->rd_pass==0)
      img->rd_pass  = 2*picture_coding_decision(frame_pic_1, frame_pic_3, rd_qp);
    else
      img->rd_pass +=   picture_coding_decision(frame_pic_2, frame_pic_3, rd_qp);

    if ( input->RCEnable && img->rd_pass == 2 )
    {
      copy_rc_jvt( quadratic_RC_best, quadratic_RC );
      copy_rc_generic( generic_RC_best, generic_RC );
    }
    if ( img->rd_pass == 2 )
      tmpFrameQP = img->SumFrameQP;
  }

  //update_rd_picture_contexts (img->rd_pass);
  if (img->rd_pass==0)
  {
    enc_picture = enc_frame_picture;
    img->type   = prevtype;
    active_pps  = PicParSet[0];
    img->qp     = rd_qp;
    intras      = previntras;
  }
  else if (img->rd_pass==1)
  {
    enc_picture = enc_frame_picture2;
    img->type   = prevtype;
    active_pps  = sec_pps;
    img->qp     = second_qp;
    intras      = previntras;
  }
  if ( input->RCEnable )
  {
    copy_rc_jvt( quadratic_RC, quadratic_RC_best );
    copy_rc_generic( generic_RC, generic_RC_best );
  }
  img->SumFrameQP = tmpFrameQP;
}

/*!
*************************************************************************************
* Brief
*     Output SP frames coefficients
*************************************************************************************
*/
void output_SP_coefficients()
{
  int i,k;
  FILE *SP_coeff_file;
  if(number_sp2_frames==0)
  {
    if ((SP_coeff_file = fopen(input->sp_output_filename,"wb")) == NULL)
    {
      printf ("Fatal: cannot open SP output file '%s', exit (-1)\n", input->sp_output_filename);
      exit (-1);
    }
    number_sp2_frames++;
  }
  else
  {
    if ((SP_coeff_file = fopen(input->sp_output_filename,"ab")) == NULL)
    {
      printf ("Fatal: cannot open SP output file '%s', exit (-1)\n", input->sp_output_filename);
      exit (-1);
    }
  }

  for(i=0;i<img->height;i++)
  {
    fwrite(lrec[i],sizeof(int),img->width,SP_coeff_file);
  }
  for(k=0;k<2;k++)
  {
    for(i=0;i<img->height_cr;i++)
    {
      fwrite(lrec_uv[k][i],sizeof(int),img->width_cr,SP_coeff_file);
    }
  }
  fclose(SP_coeff_file);
}

/*!
*************************************************************************************
* Brief
*     Read SP frames coefficients
*************************************************************************************
*/
void read_SP_coefficients()
{
  int i,k;
  FILE *SP_coeff_file;

  if ( (input->qp2start > 0) && ( ( (img->tr ) % (2*input->qp2start) ) >=input->qp2start ))
  {
    if ((SP_coeff_file = fopen(input->sp2_input_filename1,"rb")) == NULL)
    {
      printf ("Fatal: cannot open SP input file '%s', exit (-1)\n", input->sp2_input_filename2);
      exit (-1);
    }
  }
  else
  {
    if ((SP_coeff_file = fopen(input->sp2_input_filename2,"rb")) == NULL)
    {
      printf ("Fatal: cannot open SP input file '%s', exit (-1)\n", input->sp2_input_filename1);
      exit (-1);
    }
  }

  if (0 != fseek (SP_coeff_file, img->size * 3/2*number_sp2_frames*sizeof(int), SEEK_SET))
  {
    printf ("Fatal: cannot seek in SP input file, exit (-1)\n");
    exit (-1);
  }
  number_sp2_frames++;

  for(i=0;i<img->height;i++)
  {
    if(img->width!=(int)fread(lrec[i],sizeof(int),img->width,SP_coeff_file))
    {
      printf ("Fatal: cannot read in SP input file, exit (-1)\n");
      exit (-1);
    }
  }

  for(k=0;k<2;k++)
  {
    for(i=0;i<img->height_cr;i++)
    {
      if(img->width_cr!=(int)fread(lrec_uv[k][i],sizeof(int),img->width_cr,SP_coeff_file))
      {
        printf ("Fatal: cannot read in SP input file, exit (-1)\n");
        exit (-1);
      }
    }
  }
  fclose(SP_coeff_file);
}


