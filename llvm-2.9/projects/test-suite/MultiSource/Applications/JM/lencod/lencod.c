
/*!
 ***********************************************************************
 *  \mainpage
 *     This is the H.264/AVC encoder reference software. For detailed documentation
 *     see the comments in each file.
 *
 *  \author
 *     The main contributors are listed in contributors.h
 *
 *  \version
 *     JM 12.1 (FRExt)
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 */
/*!
 *  \file
 *     lencod.c
 *  \brief
 *     H.264/AVC reference encoder project main()
 *  \author
 *   Main contributors (see contributors.h for copyright, address and affiliation details)
 *   - Inge Lille-Langoy               <inge.lille-langoy@telenor.com>
 *   - Rickard Sjoberg                 <rickard.sjoberg@era.ericsson.se>
 *   - Stephan Wenger                  <stewe@cs.tu-berlin.de>
 *   - Jani Lainema                    <jani.lainema@nokia.com>
 *   - Byeong-Moon Jeon                <jeonbm@lge.com>
 *   - Yoon-Seong Soh                  <yunsung@lge.com>
 *   - Thomas Stockhammer              <stockhammer@ei.tum.de>
 *   - Detlev Marpe                    <marpe@hhi.de>
 *   - Guido Heising                   <heising@hhi.de>
 *   - Valeri George                   <george@hhi.de>
 *   - Karsten Suehring                <suehring@hhi.de>
 *   - Alexis Michael Tourapis         <alexismt@ieee.org>
 ***********************************************************************
 */

#include "contributors.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>
#include "global.h"

#include "configfile.h"
#include "leaky_bucket.h"
#include "memalloc.h"
#include "intrarefresh.h"
#include "fmo.h"
#include "sei.h"
#include "parset.h"
#include "image.h"
#include "output.h"

#include "me_epzs.h"
#include "me_umhex.h"
#include "me_umhexsmp.h"

#include "ratectl.h"
#include "rc_quadratic.h"
#include "explicit_gop.h"

#define JM      "12 (FRExt)"
#define VERSION "12.1"
#define EXT_VERSION "(FRExt)"

static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

InputParameters inputs,      *input = &inputs;
ImageParameters images,      *img   = &images;
StatParameters  statistics,  *stats = &statistics;
SNRParameters   snrs,        *snr   = &snrs;
Decoders decoders, *decs=&decoders;

static void information_init(void);

#ifdef _ADAPT_LAST_GROUP_
int initial_Bframes = 0;
#endif

Boolean In2ndIGOP = FALSE;
int    start_frame_no_in_this_IGOP = 0;
int    start_tr_in_this_IGOP = 0;
int    FirstFrameIn2ndIGOP=0;
int    cabac_encoding = 0;
int    frame_statistic_start;
extern ColocatedParams *Co_located;
extern double *mb16x16_cost_frame;
static char DistortionType[3][20] = {"SAD","SSE","Hadamard SAD"};

void Init_Motion_Search_Module (void);
void Clear_Motion_Search_Module (void);
void report_frame_statistic(void);
void SetLevelIndices(void);
void chroma_mc_setup(void);

void init_stats(void)
{
  stats->successive_Bframe = input->successive_Bframe;
  stats->bit_ctr_I = 0;
  stats->bit_ctr_P = 0;
  stats->bit_ctr_B = 0;
  snr->snr_y = 0.0;
  snr->snr_u = 0.0;
  snr->snr_v = 0.0;
  snr->snr_y1 = 0.0;
  snr->snr_u1 = 0.0;
  snr->snr_v1 = 0.0;
  snr->snr_ya = 0.0;
  snr->snr_ua = 0.0;
  snr->snr_va = 0.0;
  snr->sse_y  = 0.0;
  snr->sse_u  = 0.0;
  snr->sse_v  = 0.0;
  snr->msse_y = 0.0;
  snr->msse_u = 0.0;
  snr->msse_v = 0.0;
  snr->frame_ctr = 0;
}
/*!
 ***********************************************************************
 * \brief
 *    Main function for encoder.
 * \param argc
 *    number of command line arguments
 * \param argv
 *    command line arguments
 * \return
 *    exit code
 ***********************************************************************
 */
int main(int argc,char **argv)
{
  int M,N,n,np,nb;           //Rate control
  int primary_disp = 0;
  giRDOpt_B8OnlyFlag = 0;

  p_dec = p_in = -1;

  p_stat = p_log = p_trace = NULL;

  frame_statistic_start = 1;

  Configure (argc, argv);

  Init_QMatrix();

  Init_QOffsetMatrix();

  AllocNalPayloadBuffer();

  init_poc();
  GenerateParameterSets();
  SetLevelIndices();

  init_img();
  frame_pic_1= malloc_picture();

  if (input->RDPictureDecision)
  {
    frame_pic_2 = malloc_picture();
    frame_pic_3 = malloc_picture();
  }

  if (input->si_frame_indicator)
  {
    si_frame_indicator=0; //indicates whether the frame is SP or SI
    number_sp2_frames=0;

    frame_pic_si = malloc_picture();//picture buffer for the encoded SI picture
    //allocation of lrec and lrec_uv for SI picture
    get_mem2Dint (&lrec, img->height, img->width);
    get_mem3Dint (&lrec_uv, 2, img->height, img->width);
  }

  if (input->PicInterlace != FRAME_CODING)
  {
    top_pic = malloc_picture();
    bottom_pic = malloc_picture();
  }
  init_rdopt ();
#ifdef _LEAKYBUCKET_
  Bit_Buffer = malloc((input->no_frames * (input->successive_Bframe + 1) + 1) * sizeof(long));
#endif

  if (input->HierarchicalCoding )
  {
    init_gop_structure();
    if (input->successive_Bframe && input->HierarchicalCoding == 3)
    {
      interpret_gop_structure();
    }
    else
    {
      create_hierarchy();
    }
  }

  dpb.init_done = 0;
  init_dpb();
  init_out_buffer();
  init_stats();

  enc_picture = enc_frame_picture = enc_top_picture = enc_bottom_picture = NULL;

  init_global_buffers();

  create_context_memory ();

  Init_Motion_Search_Module ();

  information_init();

  //Rate control
  if (input->RCEnable)
     rc_init_seq(quadratic_RC);

  if(input->SearchMode == UM_HEX)
    UMHEX_DefineThreshold();

  // Init frame type counter. Only supports single slice per frame.
  memset(frame_ctr, 0, 5 * sizeof(int));

  img->last_valid_reference = 0;
  tot_time=0;                 // time for total encoding session

#ifdef _ADAPT_LAST_GROUP_
  if (input->last_frame > 0)
    input->no_frames = 1 + (input->last_frame + input->jumpd) / (input->jumpd + 1);
  initial_Bframes = input->successive_Bframe;
#endif

  PatchInputNoFrames();

  // Write sequence header (with parameter sets)
  stats->bit_ctr_parametersets = 0;
  stats->bit_slice = start_sequence();
  stats->bit_ctr_parametersets += stats->bit_ctr_parametersets_n;
  start_frame_no_in_this_IGOP = 0;

  if ( input->ChromaMCBuffer )
    chroma_mc_setup();

  for (img->number=0; img->number < input->no_frames; img->number++)
  {
    //img->nal_reference_idc = 1;
    if (input->intra_period)
      img->nal_reference_idc = ((IMG_NUMBER % input->intra_period) && input->DisposableP) ? (img->number + 1)% 2 : 1;
    else
      img->nal_reference_idc = (img->number && input->DisposableP) ? (img->number + 1)% 2 : 1;

    //much of this can go in init_frame() or init_field()?
    //poc for this frame or field
    img->toppoc = (input->intra_period && input->idr_enable ? IMG_NUMBER % input->intra_period : IMG_NUMBER) * (2*(input->jumpd+1));

    if ((input->PicInterlace==FRAME_CODING)&&(input->MbInterlace==FRAME_CODING))
      img->bottompoc = img->toppoc;     //progressive
    else
      img->bottompoc = img->toppoc+1;   //hard coded

    img->framepoc = imin (img->toppoc, img->bottompoc);

    //frame_num for this frame
    //if (input->BRefPictures== 0 || input->successive_Bframe == 0 || img-> number < 2)
    if ((input->BRefPictures != 1 &&  input->HierarchicalCoding == 0) || input->successive_Bframe == 0 || img-> number < 2)// ||  input->HierarchicalCoding == 0)
    {
      if (input->intra_period && input->idr_enable)
      {
        img->frame_num =  ((IMG_NUMBER - primary_disp)  % input->intra_period ) % (1 << (log2_max_frame_num_minus4 + 4));
        if (IMG_NUMBER % input->intra_period  == 0)
        {
          img->frame_num = 0;
          primary_disp   = 0;
        }
      }
      else
      img->frame_num = (IMG_NUMBER - primary_disp) % (1 << (log2_max_frame_num_minus4 + 4));

    }
    else
    {
      //img->frame_num ++;
      if (input->intra_period && input->idr_enable)
      {
        if (0== (img->number % input->intra_period))
        {
          img->frame_num=0;
          primary_disp   = 0;
        }
      }
      img->frame_num %= (1 << (log2_max_frame_num_minus4 + 4));
    }

    //the following is sent in the slice header
    img->delta_pic_order_cnt[0]=0;

    if (input->BRefPictures == 1)
    {
      if (img->number)
      {
        img->delta_pic_order_cnt[0]=+2 * input->successive_Bframe;
      }
    }

    SetImgType();

    if (input->ResendSPS == 1 && img->type == I_SLICE && img->number != 0)
    {
      stats->bit_slice = rewrite_paramsets();
      stats->bit_ctr_parametersets += stats->bit_ctr_parametersets_n;
    }


#ifdef _ADAPT_LAST_GROUP_
    if (input->successive_Bframe && input->last_frame && IMG_NUMBER+1 == input->no_frames)
    {
      int bi = (int)((float)(input->jumpd+1)/(input->successive_Bframe+1.0)+0.499999);

      input->successive_Bframe = (input->last_frame-(img->number-1)*(input->jumpd+1))/bi-1;

      //about to code the last ref frame, adjust delta poc
      img->delta_pic_order_cnt[0]= -2*(initial_Bframes - input->successive_Bframe);
      img->toppoc += img->delta_pic_order_cnt[0];
      img->bottompoc += img->delta_pic_order_cnt[0];
      img->framepoc = imin (img->toppoc, img->bottompoc);
    }
#endif

     //Rate control
    if (img->type == I_SLICE && ((input->RCUpdateMode != RC_MODE_1 && input->RCUpdateMode != RC_MODE_3) || (!IMG_NUMBER) ) )
    {
      if(input->RCEnable)
      {
        if (input->intra_period == 0)
        {
          n = input->no_frames + (input->no_frames - 1) * input->successive_Bframe;

          /* number of P frames */
          np = input->no_frames-1;

          /* number of B frames */
          nb = (input->no_frames - 1) * input->successive_Bframe;
        }
        else if ( input->RCUpdateMode != RC_MODE_1 && input->RCUpdateMode != RC_MODE_3 )
        {
          N = input->intra_period*(input->successive_Bframe+1);
          M = input->successive_Bframe+1;
          n = (img->number==0) ? N - ( M - 1) : N;

          /* last GOP may contain less frames */
          if(img->number/input->intra_period >= input->no_frames / input->intra_period)
          {
            if (img->number != 0)
              n = (input->no_frames - img->number) + (input->no_frames - img->number - 1) * input->successive_Bframe + input->successive_Bframe;
            else
              n = input->no_frames  + (input->no_frames - 1) * input->successive_Bframe;
          }

          /* number of P frames */
          if (img->number == 0)
            np = (n + 2 * (M - 1)) / M - 1; /* first GOP */
          else
            np = (n + (M - 1)) / M - 1;

          /* number of B frames */
          nb = n - np - 1;
        }
        else // applies RC to I and B slices
        {
          np = input->no_frames - 1; // includes I and P slices/frames except the very first IDR I_SLICE
          nb = (input->no_frames - 1) * input->successive_Bframe;
        }
        rc_init_GOP(quadratic_RC,np,nb);
      }
    }


    // which layer the image belonged to?
    if ( IMG_NUMBER % (input->NumFramesInELSubSeq+1) == 0 )
      img->layer = 0;
    else
      img->layer = 1;

    // redundant frame initialization and allocation
    if(input->redundant_pic_flag)
    {
      Init_redundant_frame();
      Set_redundant_frame();
    }

    encode_one_frame(); // encode one I- or P-frame

    // if key frame is encoded, encode one redundant frame
    if(input->redundant_pic_flag && key_frame)
    {
      encode_one_redundant_frame();
    }

    if (img->type == I_SLICE && input->EnableOpenGOP)
      img->last_valid_reference = img->ThisPOC;

    if (input->ReportFrameStats)
      report_frame_statistic();

    if (img->nal_reference_idc == 0)
    {
      primary_disp ++;
      img->frame_num -= 1;
      img->frame_num %= (1 << (log2_max_frame_num_minus4 + 4));
    }
    encode_enhancement_layer();

    process_2nd_IGOP();
  }
  // terminate sequence
  terminate_sequence();

  flush_dpb();

  close(p_in);
  if (-1!=p_dec)
    close(p_dec);
  if (p_trace)
    fclose(p_trace);

  Clear_Motion_Search_Module ();

  RandomIntraUninit();
  FmoUninit();

  if (input->HierarchicalCoding)
    clear_gop_structure ();

  // free structure for rd-opt. mode decision
  clear_rdopt ();

#ifdef _LEAKYBUCKET_
  calc_buffer();
#endif

  // report everything
  report();

#ifdef _LEAKYBUCKET_
  free(Bit_Buffer);
#endif
  free_picture (frame_pic_1);

  if (input->RDPictureDecision)
  {
    free_picture (frame_pic_2);
    free_picture (frame_pic_3);
  }

  // Deallocation of SI picture related memory
  if (input->si_frame_indicator)
  {
    free_picture (frame_pic_si);
    //deallocation of lrec and lrec_uv for SI frames
    free_mem2Dint (lrec);
    free_mem3Dint (lrec_uv,2);
  }

  if (top_pic)
    free_picture (top_pic);
  if (bottom_pic)
    free_picture (bottom_pic);

  free_dpb();
  free_colocated(Co_located);
  uninit_out_buffer();

  free_global_buffers();

  // free image mem
  free_img ();
  free_context_memory ();
  FreeNalPayloadBuffer();
  FreeParameterSets();
  return 0;
}
/*!
 ***********************************************************************
 * \brief
 *    Terminates and reports statistics on error.
 *
 ***********************************************************************
 */
void report_stats_on_error(void)
{
  input->no_frames=img->number;
  terminate_sequence();

  flush_dpb();

  close(p_in);
  if (-1!=p_dec)
    close(p_dec);

  if (p_trace)
    fclose(p_trace);

  Clear_Motion_Search_Module ();

  RandomIntraUninit();
  FmoUninit();

  if (input->HierarchicalCoding)
    clear_gop_structure ();

  // free structure for rd-opt. mode decision
  clear_rdopt ();

#ifdef _LEAKYBUCKET_
  calc_buffer();
#endif

  if (input->ReportFrameStats)
    report_frame_statistic();

  // report everything
  report();

  free_picture (frame_pic_1);
  if (top_pic)
    free_picture (top_pic);
  if (bottom_pic)
    free_picture (bottom_pic);

  free_dpb();
  free_colocated(Co_located);
  uninit_out_buffer();

  free_global_buffers();

  // free image mem
  free_img ();
  free_context_memory ();
  FreeNalPayloadBuffer();
  FreeParameterSets();
}

/*!
 ***********************************************************************
 * \brief
 *    Initializes the POC structure with appropriate parameters.
 *
 ***********************************************************************
 */
void init_poc()
{
  //the following should probably go in sequence parameters
  // frame poc's increase by 2, field poc's by 1

  img->pic_order_cnt_type=input->pic_order_cnt_type;

  img->delta_pic_order_always_zero_flag = FALSE;
  img->num_ref_frames_in_pic_order_cnt_cycle= 1;

  if (input->BRefPictures == 1)
  {
    img->offset_for_non_ref_pic  =   0;
    img->offset_for_ref_frame[0] =   2;
  }
  else
  {
    img->offset_for_non_ref_pic  =  -2*(input->successive_Bframe);
    img->offset_for_ref_frame[0] =   2*(input->successive_Bframe+1);
  }

  if ((input->PicInterlace==FRAME_CODING)&&(input->MbInterlace==FRAME_CODING))
    img->offset_for_top_to_bottom_field=0;
  else
    img->offset_for_top_to_bottom_field=1;

  if ((input->PicInterlace==FRAME_CODING)&&(input->MbInterlace==FRAME_CODING))
  {
    img->pic_order_present_flag=FALSE;
    img->delta_pic_order_cnt_bottom = 0;
  }
  else
  {
    img->pic_order_present_flag=TRUE;
    img->delta_pic_order_cnt_bottom = 1;
  }
}


/*!
 ***********************************************************************
 * \brief
 *    Initializes the img->nz_coeff
 * \par Input:
 *    none
 * \par  Output:
 *    none
 * \ side effects
 *    sets omg->nz_coef[][][][] to -1
 ***********************************************************************
 */
void CAVLC_init(void)
{
  unsigned int i, k, l;

  for (i=0;i < img->PicSizeInMbs; i++)
    for (k=0;k<4;k++)
      for (l=0;l < (4 + (unsigned int)img->num_blk8x8_uv);l++)
        img->nz_coeff[i][k][l]=0;
}


/*!
 ***********************************************************************
 * \brief
 *    Initializes the Image structure with appropriate parameters.
 * \par Input:
 *    Input Parameters struct inp_par *inp
 * \par  Output:
 *    Image Parameters struct img_par *img
 ***********************************************************************
 */
void init_img()
{
  int i;
  int byte_abs_range;

  static int mb_width_cr[4] = {0,8, 8,16};
  static int mb_height_cr[4]= {0,8,16,16};

  img->yuv_format = input->yuv_format;

  //pel bitdepth init
  img->bitdepth_luma   = input->BitDepthLuma;

  img->bitdepth_luma_qp_scale   = 6*(img->bitdepth_luma   - 8);
  img->bitdepth_lambda_scale    = 2*(img->bitdepth_luma   - 8);

  img->dc_pred_value_luma   = 1<<(img->bitdepth_luma - 1);
  img->max_imgpel_value = (1<<img->bitdepth_luma) - 1;
  img->mb_size[0][0] = img->mb_size[0][1] = MB_BLOCK_SIZE;

  if (img->yuv_format != YUV400)
  {
    img->bitdepth_chroma      = input->BitDepthChroma;
    img->dc_pred_value_chroma = 1<<(img->bitdepth_chroma - 1);
    img->max_imgpel_value_uv  = (1<<img->bitdepth_chroma) - 1;
    img->num_blk8x8_uv        = (1<<img->yuv_format)&(~(0x1));
    img->num_cdc_coeff        = img->num_blk8x8_uv<<1;
    img->mb_size[1][0] = img->mb_size[2][0] = img->mb_cr_size_x = (img->yuv_format==YUV420 || img->yuv_format==YUV422)? 8:16;
    img->mb_size[1][1] = img->mb_size[2][1] = img->mb_cr_size_y = (img->yuv_format==YUV444 || img->yuv_format==YUV422)? 16:8;

    img->bitdepth_chroma_qp_scale = 6*(img->bitdepth_chroma - 8);

    img->chroma_qp_offset[0] = active_pps->cb_qp_index_offset;
    img->chroma_qp_offset[1] = active_pps->cr_qp_index_offset;
  }
  else
  {
    img->bitdepth_chroma     = 0;
    img->max_imgpel_value_uv = 0;
    img->num_blk8x8_uv       = 0;
    img->num_cdc_coeff       = 0;
    img->mb_size[1][0] = img->mb_size[2][0] = img->mb_cr_size_x        = 0;
    img->mb_size[1][1] = img->mb_size[2][1] = img->mb_cr_size_y        = 0;

    img->bitdepth_chroma_qp_scale = 0;
    img->bitdepth_chroma_qp_scale = 0;

    img->chroma_qp_offset[0] = 0;
    img->chroma_qp_offset[1] = 0;
  }

  if((img->bitdepth_luma > img->bitdepth_chroma) || img->yuv_format == YUV400)
    img->pic_unit_size_on_disk = (img->bitdepth_luma > 8)? 16:8;
  else
    img->pic_unit_size_on_disk = (img->bitdepth_chroma > 8)? 16:8;

  img->max_bitCount =  128 + 256 * img->bitdepth_luma + 2 * img->mb_cr_size_y * img->mb_cr_size_x * img->bitdepth_chroma;

  img->max_qp_delta = (25 + (img->bitdepth_luma_qp_scale>>1));
  img->min_qp_delta = img->max_qp_delta + 1;

  img->num_ref_frames = active_sps->num_ref_frames;
  img->max_num_references   = active_sps->frame_mbs_only_flag ? active_sps->num_ref_frames : 2 * active_sps->num_ref_frames;

  img->buf_cycle = input->num_ref_frames;

  img->DeblockCall = 0;

  img->framerate=(float) input->FrameRate;   // The basic frame rate (of the original sequence)

  get_mem_mv (&(img->pred_mv));
  get_mem_mv (&(img->all_mv));

  if (input->BiPredMotionEstimation)
  {
    get_mem_mv (&(img->bipred_mv1));
    get_mem_mv (&(img->bipred_mv2));
  }

  get_mem_ACcoeff (&(img->cofAC));
  get_mem_DCcoeff (&(img->cofDC));

  if (input->AdaptiveRounding)
  {
    get_mem3Dint(&(img->fadjust4x4), 4, MB_BLOCK_SIZE, MB_BLOCK_SIZE);
    get_mem3Dint(&(img->fadjust8x8), 3, MB_BLOCK_SIZE, MB_BLOCK_SIZE);
    get_mem4Dint(&(img->fadjust4x4Cr), 4, 2, img->mb_cr_size_y, img->mb_cr_size_x);
    get_mem4Dint(&(img->fadjust8x8Cr), 1, 2, img->mb_cr_size_y, img->mb_cr_size_x);
  }

  if(input->MbInterlace)
  {
    get_mem_mv (&(rddata_top_frame_mb.pred_mv));
    get_mem_mv (&(rddata_top_frame_mb.all_mv));

    get_mem_mv (&(rddata_bot_frame_mb.pred_mv));
    get_mem_mv (&(rddata_bot_frame_mb.all_mv));

    get_mem_ACcoeff (&(rddata_top_frame_mb.cofAC));
    get_mem_DCcoeff (&(rddata_top_frame_mb.cofDC));

    get_mem_ACcoeff (&(rddata_bot_frame_mb.cofAC));
    get_mem_DCcoeff (&(rddata_bot_frame_mb.cofDC));

    if ( input->MbInterlace != FRAME_MB_PAIR_CODING )
    {
      get_mem_mv (&(rddata_top_field_mb.pred_mv));
      get_mem_mv (&(rddata_top_field_mb.all_mv));

      get_mem_mv (&(rddata_bot_field_mb.pred_mv));
      get_mem_mv (&(rddata_bot_field_mb.all_mv));

      get_mem_ACcoeff (&(rddata_top_field_mb.cofAC));
      get_mem_DCcoeff (&(rddata_top_field_mb.cofDC));

      get_mem_ACcoeff (&(rddata_bot_field_mb.cofAC));
      get_mem_DCcoeff (&(rddata_bot_field_mb.cofDC));
    }
  }

  if(img->max_imgpel_value > img->max_imgpel_value_uv)
    byte_abs_range = (img->max_imgpel_value + 1) * 2;
  else
    byte_abs_range = (img->max_imgpel_value_uv + 1) * 2;

  if ((img->quad = (int*)calloc (byte_abs_range, sizeof(int))) == NULL)
    no_mem_exit ("init_img: img->quad");
  img->quad+=byte_abs_range/2;
  for (i=0; i < byte_abs_range/2; ++i)
  {
    img->quad[i]=img->quad[-i]=i*i;
  }

  img->width    = (input->img_width+img->auto_crop_right);
  img->height   = (input->img_height+img->auto_crop_bottom);
  img->width_blk  = img->width / BLOCK_SIZE;
  img->height_blk = img->height / BLOCK_SIZE;

  img->width_padded =  img->width + 2 * IMG_PAD_SIZE;
  img->height_padded = img->height + 2 * IMG_PAD_SIZE;

  if (img->yuv_format != YUV400)
  {
    img->width_cr = img->width/(16/mb_width_cr[img->yuv_format]);
    img->height_cr= img->height/(16/mb_height_cr[img->yuv_format]);

    input->img_width_cr  = input->img_width/(16/mb_width_cr[img->yuv_format]);
    input->img_height_cr = input->img_height/(16/mb_height_cr[img->yuv_format]);
  }
  else
  {
    img->width_cr = 0;
    img->height_cr= 0;

    input->img_width_cr  = 0;
    input->img_height_cr = 0;
  }
  img->height_cr_frame = img->height_cr;

  img->size = img->width * img->height;
  img->size_cr = img->width_cr * img->height_cr;

  img->PicWidthInMbs    = img->width/MB_BLOCK_SIZE;
  img->FrameHeightInMbs = img->height/MB_BLOCK_SIZE;
  img->FrameSizeInMbs   = img->PicWidthInMbs * img->FrameHeightInMbs;

  img->PicHeightInMapUnits = ( active_sps->frame_mbs_only_flag ? img->FrameHeightInMbs : img->FrameHeightInMbs/2 );

  if(((img->mb_data) = (Macroblock *) calloc(img->FrameSizeInMbs,sizeof(Macroblock))) == NULL)
    no_mem_exit("init_img: img->mb_data");

  if(input->UseConstrainedIntraPred)
  {
    if(((img->intra_block) = (int*)calloc(img->FrameSizeInMbs,sizeof(int))) == NULL)
      no_mem_exit("init_img: img->intra_block");
  }

  if (input->CtxAdptLagrangeMult == 1)
  {
    if ((mb16x16_cost_frame = (double*)calloc(img->FrameSizeInMbs, sizeof(double))) == NULL)
    {
      no_mem_exit("init mb16x16_cost_frame");
    }
  }
  get_mem2D((byte***)&(img->ipredmode), img->height_blk, img->width_blk);        //need two extra rows at right and bottom
  get_mem2D((byte***)&(img->ipredmode8x8), img->height_blk, img->width_blk);     // help storage for ipredmode 8x8, inserted by YV
  memset(&img->ipredmode[0][0],-1, img->height_blk * img->width_blk *sizeof(char));
  memset(&img->ipredmode8x8[0][0],-1, img->height_blk * img->width_blk *sizeof(char));

  get_mem2D((byte***)&(rddata_top_frame_mb.ipredmode), img->height_blk, img->width_blk);

  if(input->MbInterlace)
  {
    get_mem2D((byte***)&(rddata_bot_frame_mb.ipredmode), img->height_blk, img->width_blk);
    get_mem2D((byte***)&(rddata_top_field_mb.ipredmode), img->height_blk, img->width_blk);
    get_mem2D((byte***)&(rddata_bot_field_mb.ipredmode), img->height_blk, img->width_blk);
  }
  // CAVLC mem
  get_mem3Dint(&(img->nz_coeff), img->FrameSizeInMbs, 4, 4+img->num_blk8x8_uv);


  get_mem2Ddb_offset(&(img->lambda_md), 10, 52 + img->bitdepth_luma_qp_scale,img->bitdepth_luma_qp_scale);
  get_mem3Ddb_offset (&(img->lambda_me), 10, 52 + img->bitdepth_luma_qp_scale, 3, img->bitdepth_luma_qp_scale);
  get_mem3Dint_offset(&(img->lambda_mf), 10, 52 + img->bitdepth_luma_qp_scale, 3, img->bitdepth_luma_qp_scale);

  if (input->CtxAdptLagrangeMult == 1)
  {
    get_mem2Ddb_offset(&(img->lambda_mf_factor), 10, 52 + img->bitdepth_luma_qp_scale,img->bitdepth_luma_qp_scale);
  }

  //get_mem2Ddouble(&(img->lambda_md), 10, 52 + img->bitdepth_luma_qp_scale);

  CAVLC_init();


  img->GopLevels = (input->successive_Bframe) ? 1 : 0;
  img->mb_y_upd=0;

  RandomIntraInit (img->PicWidthInMbs, img->FrameHeightInMbs, input->RandomIntraMBRefresh);

  InitSEIMessages();  // Tian Dong (Sept 2002)

  // Initialize filtering parameters. If sending parameters, the offsets are
  // multiplied by 2 since inputs are taken in "div 2" format.
  // If not sending parameters, all fields are cleared
  if (input->LFSendParameters)
  {
    input->LFAlphaC0Offset <<= 1;
    input->LFBetaOffset <<= 1;
  }
  else
  {
    input->LFDisableIdc = 0;
    input->LFAlphaC0Offset = 0;
    input->LFBetaOffset = 0;
  }
}

/*!
 ***********************************************************************
 * \brief
 *    Free the Image structures
 * \par Input:
 *    Image Parameters struct img_par *img
 ***********************************************************************
 */
void free_img ()
{
  CloseSEIMessages(); // Tian Dong (Sept 2002)
  free_mem_mv (img->pred_mv);
  free_mem_mv (img->all_mv);

  if (input->BiPredMotionEstimation)
  {
    free_mem_mv (img->bipred_mv1);
    free_mem_mv (img->bipred_mv2);
  }

  free_mem_ACcoeff (img->cofAC);
  free_mem_DCcoeff (img->cofDC);

  if (input->AdaptiveRounding)
  {
    free_mem3Dint(img->fadjust4x4, 4);
    free_mem3Dint(img->fadjust8x8, 3);
    free_mem4Dint(img->fadjust4x4Cr, 4, 2);
    free_mem4Dint(img->fadjust8x8Cr, 1, 2);
  }


  if(input->MbInterlace)
  {
    free_mem_mv (rddata_top_frame_mb.pred_mv);
    free_mem_mv (rddata_top_frame_mb.all_mv);

    free_mem_mv (rddata_bot_frame_mb.pred_mv);
    free_mem_mv (rddata_bot_frame_mb.all_mv);

    free_mem_ACcoeff (rddata_top_frame_mb.cofAC);
    free_mem_DCcoeff (rddata_top_frame_mb.cofDC);

    free_mem_ACcoeff (rddata_bot_frame_mb.cofAC);
    free_mem_DCcoeff (rddata_bot_frame_mb.cofDC);

    if ( input->MbInterlace != FRAME_MB_PAIR_CODING )
    {
      free_mem_mv (rddata_top_field_mb.pred_mv);
      free_mem_mv (rddata_top_field_mb.all_mv);

      free_mem_mv (rddata_bot_field_mb.pred_mv);
      free_mem_mv (rddata_bot_field_mb.all_mv);

      free_mem_ACcoeff (rddata_top_field_mb.cofAC);
      free_mem_DCcoeff (rddata_top_field_mb.cofDC);

      free_mem_ACcoeff (rddata_bot_field_mb.cofAC);
      free_mem_DCcoeff (rddata_bot_field_mb.cofDC);
    }
  }

  if(img->max_imgpel_value > img->max_imgpel_value_uv)
    free (img->quad-(img->max_imgpel_value + 1));
  else
    free (img->quad-(img->max_imgpel_value_uv + 1));

  if(input->MbInterlace)
  {
    free_mem2D((byte**)rddata_bot_frame_mb.ipredmode);
    free_mem2D((byte**)rddata_top_field_mb.ipredmode);
    free_mem2D((byte**)rddata_bot_field_mb.ipredmode);
  }
}


/*!
 ************************************************************************
 * \brief
 *    Allocates the picture structure along with its dependent
 *    data structures
 * \return
 *    Pointer to a Picture
 ************************************************************************
 */

Picture *malloc_picture()
{
  Picture *pic;
  if ((pic = calloc (1, sizeof (Picture))) == NULL) no_mem_exit ("malloc_picture: Picture structure");
  //! Note: slice structures are allocated as needed in code_a_picture
  return pic;
}

/*!
 ************************************************************************
 * \brief
 *    Frees a picture
 * \param
 *    pic: POinter to a Picture to be freed
 ************************************************************************
 */


void free_picture(Picture *pic)
{
  if (pic != NULL)
  {
    free_slice_list(pic);
    free (pic);
  }
}


/*!
 ************************************************************************
 * \brief
 *    Reports frame statistical data to a stats file
 ************************************************************************
 */
void report_frame_statistic()
{
  FILE *p_stat_frm = NULL;
  static int64   last_mode_use[NUM_PIC_TYPE][MAXMODE];
  static int   last_b8_mode_0[NUM_PIC_TYPE][2];
  static int   last_mode_chroma_use[4];
  static int64   last_bit_ctr_n = 0;
  int i;
  char name[20];
  int bitcounter;

#ifndef WIN32
  time_t now;
  struct tm *l_time;
  char string[1000];
#else
  char timebuf[128];
#endif


  // write to log file
  if ((p_stat_frm=fopen("stat_frame.dat","r"))==0)                      // check if file exist
  {
    if ((p_stat_frm=fopen("stat_frame.dat","a"))==NULL)            // append new statistic at the end
    {
      snprintf(errortext, ET_SIZE, "Error open file %s  \n","stat_frame.dat.dat");
      error(errortext, 500);
    }
    else                                            // Create header for new log file
    {
      fprintf(p_stat_frm," --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n");
      fprintf(p_stat_frm,"|            Encoder statistics. This file is generated during first encoding session, new sessions will be appended                                                                                                                                                                                                                                                                                                                                                              |\n");
      fprintf(p_stat_frm," --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n");
    }
  }
  else
  {
    fclose (p_stat_frm);
    if ((p_stat_frm=fopen("stat_frame.dat","a"))==NULL)            // File exist,just open for appending
    {
      snprintf(errortext, ET_SIZE, "Error open file %s  \n","stat_frame.dat.dat");
      error(errortext, 500);
    }
  }

  if (frame_statistic_start)
  {
    fprintf(p_stat_frm,"|  ver       | Date  | Time  |    Sequence        |Frm | QP |P/MbInt|   Bits   |  SNRY  |  SNRU  |  SNRV  |  I4  |  I8  | I16  | IC0  | IC1  | IC2  | IC3  | PI4  | PI8  | PI16 |  P0  |  P1  |  P2  |  P3  | P1*8*| P1*4*| P2*8*| P2*4*| P3*8*| P3*4*|  P8  | P8:4 | P4*8*| P4*4*| P8:5 | P8:6 | P8:7 | BI4  | BI8  | BI16 |  B0  |  B1  |  B2  |  B3  | B0*8*| B0*4*| B1*8*| B1*4*| B2*8*| B2*4*| B3*8*| B3*4*|  B8  | B8:0 |B80*8*|B80*4*| B8:4 | B4*8*| B4*4*| B8:5 | B8:6 | B8:7 |\n");
    fprintf(p_stat_frm," ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n");
  }

  //report
  fprintf(p_stat_frm,"|%4s/%s", VERSION, EXT_VERSION);

#ifdef WIN32
  _strdate( timebuf );
  fprintf(p_stat_frm,"| %1.5s |",timebuf );

  _strtime( timebuf);
  fprintf(p_stat_frm," % 1.5s |",timebuf);
#else
  now = time ((time_t *) NULL); // Get the system time and put it into 'now' as 'calender time'
  time (&now);
  l_time = localtime (&now);
  strftime (string, sizeof string, "%d-%b-%Y", l_time);
  fprintf(p_stat_frm,"| %1.5s |",string );

  strftime (string, sizeof string, "%H:%M:%S", l_time);
  fprintf(p_stat_frm," %1.5s |",string );
#endif

  for (i=0;i<20;i++)
    name[i]=input->infile[i + imax(0,(int) (strlen(input->infile)-20))]; // write last part of path, max 20 chars
  fprintf(p_stat_frm,"%20.20s|",name);

  fprintf(p_stat_frm,"%3d |",frame_no);

  fprintf(p_stat_frm,"%3d |",img->qp);

  fprintf(p_stat_frm,"  %d/%d  |",input->PicInterlace, input->MbInterlace);


  if (img->number == 0 && img->frame_num == 0)
  {
    bitcounter = (int) stats->bit_ctr_I;
  }
  else
  {
    bitcounter = (int) (stats->bit_ctr_n - last_bit_ctr_n);
    last_bit_ctr_n = stats->bit_ctr_n;
  }

  //report bitrate
  fprintf(p_stat_frm, " %9d|", bitcounter);

  //report snr's
  fprintf(p_stat_frm, " %2.4f| %2.4f| %2.4f|", snr->snr_y, snr->snr_u, snr->snr_v);

  //report modes
  //I-Modes
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[I_SLICE][I4MB] - last_mode_use[I_SLICE][I4MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[I_SLICE][I8MB] - last_mode_use[I_SLICE][I8MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[I_SLICE][I16MB] - last_mode_use[I_SLICE][I16MB]);

  //chroma intra mode
  fprintf(p_stat_frm, " %5d|",stats->intra_chroma_mode[0] - last_mode_chroma_use[0]);
  fprintf(p_stat_frm, " %5d|",stats->intra_chroma_mode[1] - last_mode_chroma_use[1]);
  fprintf(p_stat_frm, " %5d|",stats->intra_chroma_mode[2] - last_mode_chroma_use[2]);
  fprintf(p_stat_frm, " %5d|",stats->intra_chroma_mode[3] - last_mode_chroma_use[3]);

  //P-Modes
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][I4MB] - last_mode_use[P_SLICE][I4MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][I8MB] - last_mode_use[P_SLICE][I8MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][I16MB] - last_mode_use[P_SLICE][I16MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][0   ] - last_mode_use[P_SLICE][0   ]);

  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][1   ] - last_mode_use[P_SLICE][1   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][2   ] - last_mode_use[P_SLICE][2   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][3   ] - last_mode_use[P_SLICE][3   ]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[0][1]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[0][1]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[0][2]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[0][2]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[0][3]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[0][3]);

  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][P8x8] - last_mode_use[P_SLICE][P8x8]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][4   ] - last_mode_use[P_SLICE][4   ]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[0][4]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[0][4]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][5   ] - last_mode_use[P_SLICE][5   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][6   ] - last_mode_use[P_SLICE][6   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[P_SLICE][7   ] - last_mode_use[P_SLICE][7   ]);

  //B-Modes
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][I4MB] - last_mode_use[B_SLICE][I4MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][I8MB] - last_mode_use[B_SLICE][I8MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][I16MB] - last_mode_use[B_SLICE][I16MB]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][0   ] - last_mode_use[B_SLICE][0   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][1   ] - last_mode_use[B_SLICE][1   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][2   ] - last_mode_use[B_SLICE][2   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][3   ] - last_mode_use[B_SLICE][3   ]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[1][0]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[1][0]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[1][1]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[1][1]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[1][2]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[1][2]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[1][3]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[1][3]);

  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][P8x8] - last_mode_use[B_SLICE][P8x8]);
  fprintf(p_stat_frm, " %d|",(stats->b8_mode_0_use [B_SLICE][0]+stats->b8_mode_0_use [B_SLICE][1]) - (last_b8_mode_0[B_SLICE][0]+last_b8_mode_0[B_SLICE][1]));
  fprintf(p_stat_frm, " %5d|",stats->b8_mode_0_use [B_SLICE][1] - last_b8_mode_0[B_SLICE][1]);
  fprintf(p_stat_frm, " %5d|",stats->b8_mode_0_use [B_SLICE][0] - last_b8_mode_0[B_SLICE][0]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][4   ] - last_mode_use[B_SLICE][4   ]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_8x8[1][4]);
  fprintf(p_stat_frm, " %5d|",stats->mode_use_transform_4x4[1][4]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][5   ] - last_mode_use[B_SLICE][5   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][6   ] - last_mode_use[B_SLICE][6   ]);
  fprintf(p_stat_frm, " %5" FORMAT_OFF_T  "|",stats->mode_use[B_SLICE][7   ] - last_mode_use[B_SLICE][7   ]);

  fprintf(p_stat_frm, "\n");

  //save the last results
  memcpy(last_mode_use[I_SLICE],stats->mode_use[I_SLICE], MAXMODE *  sizeof(int64));
  memcpy(last_mode_use[P_SLICE],stats->mode_use[P_SLICE], MAXMODE *  sizeof(int64));
  memcpy(last_mode_use[B_SLICE],stats->mode_use[B_SLICE], MAXMODE *  sizeof(int64));
  memset(stats->mode_use_transform_8x8,0, 2 * MAXMODE *  sizeof(int));
  memset(stats->mode_use_transform_4x4,0, 2 * MAXMODE *  sizeof(int));
  memcpy(last_b8_mode_0[B_SLICE],stats->b8_mode_0_use[B_SLICE], 2 *  sizeof(int));
  memcpy(last_mode_chroma_use,stats->intra_chroma_mode, 4 *  sizeof(int));

  frame_statistic_start = 0;
  fclose(p_stat_frm);
}


/*!
 ************************************************************************
 * \brief
 *    Reports the gathered information to appropriate outputs
 * \par Input:
 *    struct inp_par *inp,                                            \n
 *    struct img_par *img,                                            \n
 *    struct stat_par *stats,                                          \n
 *    struct stat_par *stats                                           \n
 *
 * \par Output:
 *    None
 ************************************************************************
 */
void report()
{
  int64 bit_use[NUM_PIC_TYPE][2] ;
  int i,j;
  char name[30];
  int64 total_bits;
  float frame_rate;
  double mean_motion_info_bit_use[2] = {0.0};

#ifndef WIN32
  time_t now;
  struct tm *l_time;
  char string[1000];
#else
  char timebuf[128];
#endif
  bit_use[I_SLICE][0] = frame_ctr[I_SLICE];
  bit_use[P_SLICE][0] = imax(frame_ctr[P_SLICE] + frame_ctr[SP_SLICE],1);
  bit_use[B_SLICE][0] = frame_ctr[B_SLICE];

  //  Accumulate bit usage for inter and intra frames
  for (j=0;j<NUM_PIC_TYPE;j++)
  {
    bit_use[j][1] = 0;
  }

  for (j=0;j<NUM_PIC_TYPE;j++)
  {
    for(i=0; i<MAXMODE; i++)
      bit_use[j][1] += stats->bit_use_mode[j][i];

    bit_use[j][1]+=stats->bit_use_header[j];
    bit_use[j][1]+=stats->bit_use_mb_type[j];
    bit_use[j][1]+=stats->tmp_bit_use_cbp[j];
    bit_use[j][1]+=stats->bit_use_coeffY[j];
    bit_use[j][1]+=stats->bit_use_coeffC[j];
    bit_use[j][1]+=stats->bit_use_delta_quant[j];
    bit_use[j][1]+=stats->bit_use_stuffingBits[j];
  }

  frame_rate = (img->framerate *(float)(stats->successive_Bframe + 1)) / (float) (input->jumpd+1);

//! Currently adding NVB bits on P rate. Maybe additional stats info should be created instead and added in log file
  stats->bitrate_I = (stats->bit_ctr_I)*(frame_rate)/(float) (input->no_frames + frame_ctr[B_SLICE]);
  stats->bitrate_P = (stats->bit_ctr_P)*(frame_rate)/(float) (input->no_frames + frame_ctr[B_SLICE]);
  stats->bitrate_B = (stats->bit_ctr_B)*(frame_rate)/(float) (input->no_frames + frame_ctr[B_SLICE]);

  fprintf(stdout,"-------------------------------------------------------------------------------\n");
  if (input->Verbose != 0)
  {
    fprintf(stdout,  " Freq. for encoded bitstream       : %1.0f\n",img->framerate/(float)(input->jumpd+1));
    for (i=0; i<3; i++)
    {
      fprintf(stdout," ME Metric for Refinement Level %1d  : %s\n",i,DistortionType[input->MEErrorMetric[i]]);
    }
    fprintf(stdout," Mode Decision Metric              : %s\n",DistortionType[input->ModeDecisionMetric]);

    switch ( input->ChromaMEEnable )
    {
    case 1:
      fprintf(stdout," Motion Estimation for components  : YCbCr\n");
      break;
    default:
      fprintf(stdout," Motion Estimation for components  : Y\n");
      break;
    }

    fprintf(stdout,  " Image format                      : %dx%d\n",input->img_width,input->img_height);

    if(input->intra_upd)
      fprintf(stdout," Error robustness                  : On\n");
    else
      fprintf(stdout," Error robustness                  : Off\n");
    fprintf(stdout,  " Search range                      : %d\n",input->search_range);

    fprintf(stdout,  " Total number of references        : %d\n",input->num_ref_frames);
    fprintf(stdout,  " References for P slices           : %d\n",input->P_List0_refs? input->P_List0_refs:input->num_ref_frames);
    if(stats->successive_Bframe != 0)
    {
      fprintf(stdout," List0 references for B slices     : %d\n",input->B_List0_refs? input->B_List0_refs:input->num_ref_frames);
      fprintf(stdout," List1 references for B slices     : %d\n",input->B_List1_refs? input->B_List1_refs:input->num_ref_frames);
    }


    // B pictures
    fprintf(stdout,  " Sequence type                     :" );

    if(stats->successive_Bframe>0 && input->HierarchicalCoding)
    {
      fprintf(stdout, " Hierarchy (QP: I %d, P %d, B %d) \n",
        input->qp0, input->qpN, input->qpB);
    }
    else if(stats->successive_Bframe>0)
    {
      char seqtype[80];
      int i,j;

      strcpy (seqtype,"I");

      for (j=0;j<2;j++)
      {
        for (i=0;i<stats->successive_Bframe;i++)
        {
          if (input->BRefPictures)
            strncat(seqtype,"-RB", imax(0, (int) (79-strlen(seqtype))));
          else
            strncat(seqtype,"-B", imax(0, (int) (79-strlen(seqtype))));
        }
        strncat(seqtype,"-P", imax(0, (int) (79-strlen(seqtype))));
      }
      if (input->BRefPictures)
        fprintf(stdout, " %s (QP: I %d, P %d, RB %d) \n", seqtype,input->qp0, input->qpN, iClip3(0,51,input->qpB+input->qpBRSOffset));
      else
        fprintf(stdout, " %s (QP: I %d, P %d, B %d) \n", seqtype,input->qp0, input->qpN, input->qpB);
    }
    else if(stats->successive_Bframe==0 && input->sp_periodicity==0) fprintf(stdout, " IPPP (QP: I %d, P %d) \n",   input->qp0, input->qpN);

    else fprintf(stdout, " I-P-P-SP-P (QP: I %d, P %d, SP (%d, %d)) \n",  input->qp0, input->qpN, input->qpsp, input->qpsp_pred);

    // report on entropy coding  method
    if (input->symbol_mode == UVLC)
      fprintf(stdout," Entropy coding method             : CAVLC\n");
    else
      fprintf(stdout," Entropy coding method             : CABAC\n");

    fprintf(stdout,  " Profile/Level IDC                 : (%d,%d)\n",input->ProfileIDC,input->LevelIDC);

  if (input->SearchMode==UM_HEX)
    fprintf(stdout,  " Motion Estimation Scheme          : HEX\n");
  else if (input->SearchMode==UM_HEX_SIMPLE)
    fprintf(stdout,  " Motion Estimation Scheme          : SHEX\n");
   else if (input->SearchMode == EPZS)
   {
     fprintf(stdout,  " Motion Estimation Scheme          : EPZS\n");
     EPZSOutputStats(stdout, 0);
   }
  else if (input->SearchMode == FAST_FULL_SEARCH)
    fprintf(stdout,  " Motion Estimation Scheme          : Fast Full Search\n");
  else
    fprintf(stdout,  " Motion Estimation Scheme          : Full Search\n");



#ifdef _FULL_SEARCH_RANGE_
    if (input->full_search == 2)
      fprintf(stdout," Search range restrictions         : none\n");
    else if (input->full_search == 1)
      fprintf(stdout," Search range restrictions         : older reference frames\n");
    else
      fprintf(stdout," Search range restrictions         : smaller blocks and older reference frames\n");
#endif

    if (input->rdopt)
      fprintf(stdout," RD-optimized mode decision        : used\n");
    else
      fprintf(stdout," RD-optimized mode decision        : not used\n");

    switch(input->partition_mode)
    {
    case PAR_DP_1:
      fprintf(stdout," Data Partitioning Mode            : 1 partition \n");
      break;
    case PAR_DP_3:
      fprintf(stdout," Data Partitioning Mode            : 3 partitions \n");
      break;
    default:
      fprintf(stdout," Data Partitioning Mode            : not supported\n");
      break;
    }

    switch(input->of_mode)
    {
    case PAR_OF_ANNEXB:
      fprintf(stdout," Output File Format                : H.264 Bit Stream File Format \n");
      break;
    case PAR_OF_RTP:
      fprintf(stdout," Output File Format                : RTP Packet File Format \n");
      break;
    default:
      fprintf(stdout," Output File Format                : not supported\n");
      break;
    }
}

  fprintf(stdout,"------------------ Average data all frames  -----------------------------------\n");
  if (input->Verbose != 0)
  {
    int  impix = input->img_height * input->img_width;
    int  impix_cr = input->img_height_cr * input->img_width_cr;
    unsigned int max_pix_value_sqd = img->max_imgpel_value * img->max_imgpel_value;
    unsigned int max_pix_value_sqd_uv = img->max_imgpel_value_uv * img->max_imgpel_value_uv;
    float csnr_y = (float) (10 * log10 (max_pix_value_sqd *
      (double)((double) impix / (snr->msse_y == 0.0? 1.0 : snr->msse_y))));
    float csnr_u = (float) (10 * log10 (max_pix_value_sqd_uv *
      (double)((double) impix_cr / (snr->msse_u == 0.0? 1.0 : snr->msse_u))));
    float csnr_v = (float) (10 * log10 (max_pix_value_sqd_uv *
      (double)((double) impix_cr / (snr->msse_v == 0.0? 1.0 : snr->msse_v))));

    fprintf(stdout," PSNR Y(dB)                        : %5.2f\n",snr->snr_ya);
    fprintf(stdout," PSNR U(dB)                        : %5.2f\n",snr->snr_ua);
    fprintf(stdout," PSNR V(dB)                        : %5.2f\n",snr->snr_va);
    fprintf(stdout," cSNR Y(dB)                        : %5.2f (%5.2f)\n",csnr_y,snr->msse_y/impix);
    fprintf(stdout," cSNR U(dB)                        : %5.2f (%5.2f)\n",csnr_u,snr->msse_u/impix_cr);
    fprintf(stdout," cSNR V(dB)                        : %5.2f (%5.2f)\n",csnr_v,snr->msse_v/impix_cr);
  }

  if(frame_ctr[B_SLICE]!=0)
  {
    fprintf(stdout, " Total bits                        : %" FORMAT_OFF_T  " (I %" FORMAT_OFF_T  ", P %" FORMAT_OFF_T  ", B %" FORMAT_OFF_T  " NVB %d) \n",
      total_bits=stats->bit_ctr_P + stats->bit_ctr_I + stats->bit_ctr_B + stats->bit_ctr_parametersets,
      stats->bit_ctr_I, stats->bit_ctr_P, stats->bit_ctr_B, stats->bit_ctr_parametersets);

    frame_rate = (img->framerate *(float)(stats->successive_Bframe + 1)) / (float) (input->jumpd+1);
//    stats->bitrate= ((float) total_bits * frame_rate)/((float) (input->no_frames + frame_ctr[B_SLICE]));
    stats->bitrate= ((float) total_bits * frame_rate)/((float)(frame_ctr[I_SLICE] + frame_ctr[P_SLICE] + frame_ctr[B_SLICE]));

    fprintf(stdout, " Bit rate (kbit/s)  @ %2.2f Hz     : %5.2f\n", frame_rate, stats->bitrate/1000);

  }
  else if (input->sp_periodicity==0)
  {
    fprintf(stdout, " Total bits                        : %" FORMAT_OFF_T  " (I %" FORMAT_OFF_T  ", P %" FORMAT_OFF_T  ", NVB %d) \n",
      total_bits=stats->bit_ctr_P + stats->bit_ctr_I + stats->bit_ctr_parametersets, stats->bit_ctr_I, stats->bit_ctr_P, stats->bit_ctr_parametersets);


    frame_rate = img->framerate / ( (float) (input->jumpd + 1) );
    stats->bitrate= ((float) total_bits * frame_rate)/((float) input->no_frames );

    fprintf(stdout, " Bit rate (kbit/s)  @ %2.2f Hz     : %5.2f\n", frame_rate, stats->bitrate/1000);
  }
  else
  {
    fprintf(stdout, " Total bits                        : %" FORMAT_OFF_T  " (I %" FORMAT_OFF_T  ", P %" FORMAT_OFF_T  ", NVB %d) \n",
      total_bits=stats->bit_ctr_P + stats->bit_ctr_I + stats->bit_ctr_parametersets, stats->bit_ctr_I, stats->bit_ctr_P, stats->bit_ctr_parametersets);


    frame_rate = img->framerate / ( (float) (input->jumpd + 1) );
    stats->bitrate= ((float) total_bits * frame_rate)/((float) input->no_frames );

    fprintf(stdout, " Bit rate (kbit/s)  @ %2.2f Hz     : %5.2f\n", frame_rate, stats->bitrate/1000);
  }

  fprintf(stdout, " Bits to avoid Startcode Emulation : %d \n", stats->bit_ctr_emulationprevention);
  fprintf(stdout, " Bits for parameter sets           : %d \n", stats->bit_ctr_parametersets);

  fprintf(stdout,"-------------------------------------------------------------------------------\n");
  fprintf(stdout,"Exit JM %s encoder ver %s ", JM, VERSION);
  fprintf(stdout,"\n");

  // status file
  if ((p_stat=fopen("stats.dat","wt"))==0)
  {
    snprintf(errortext, ET_SIZE, "Error open file %s", "stats.dat");
    error(errortext, 500);
  }
  fprintf(p_stat," -------------------------------------------------------------- \n");
  fprintf(p_stat,"  This file contains statistics for the last encoded sequence   \n");
  fprintf(p_stat," -------------------------------------------------------------- \n");
  fprintf(p_stat,   " Sequence                     : %s\n",input->infile);
  fprintf(p_stat,   " No.of coded pictures         : %4d\n",input->no_frames+frame_ctr[B_SLICE]);
  fprintf(p_stat,   " Freq. for encoded bitstream  : %4.0f\n",frame_rate);

  fprintf(p_stat,   " I Slice Bitrate(kb/s)        : %6.2f\n", stats->bitrate_I/1000);
  fprintf(p_stat,   " P Slice Bitrate(kb/s)        : %6.2f\n", stats->bitrate_P/1000);
  // B pictures
  if(stats->successive_Bframe != 0)
    fprintf(p_stat,   " B Slice Bitrate(kb/s)        : %6.2f\n", stats->bitrate_B/1000);
  fprintf(p_stat,   " Total Bitrate(kb/s)          : %6.2f\n", stats->bitrate/1000);

  for (i=0; i<3; i++)
  {
    fprintf(p_stat," ME Metric for Refinement Level %1d : %s\n",i,DistortionType[input->MEErrorMetric[i]]);
  }
  fprintf(p_stat," Mode Decision Metric              : %s\n",DistortionType[input->ModeDecisionMetric]);

  switch ( input->ChromaMEEnable )
  {
  case 1:
    fprintf(p_stat," Motion Estimation for components  : YCbCr\n");
    break;
  default:
    fprintf(p_stat," Motion Estimation for components  : Y\n");
    break;
  }

  fprintf(p_stat,  " Image format                 : %dx%d\n",input->img_width,input->img_height);

  if(input->intra_upd)
    fprintf(p_stat," Error robustness             : On\n");
  else
    fprintf(p_stat," Error robustness             : Off\n");

  fprintf(p_stat,  " Search range                 : %d\n",input->search_range);

  fprintf(p_stat,   " Total number of references   : %d\n",input->num_ref_frames);
  fprintf(p_stat,   " References for P slices      : %d\n",input->P_List0_refs? input->P_List0_refs:input->num_ref_frames);
  if(stats->successive_Bframe != 0)
  {
    fprintf(p_stat, " List0 refs for B slices      : %d\n",input->B_List0_refs? input->B_List0_refs:input->num_ref_frames);
    fprintf(p_stat, " List1 refs for B slices      : %d\n",input->B_List1_refs? input->B_List1_refs:input->num_ref_frames);
  }

  if (input->symbol_mode == UVLC)
    fprintf(p_stat,   " Entropy coding method        : CAVLC\n");
  else
    fprintf(p_stat,   " Entropy coding method        : CABAC\n");

    fprintf(p_stat,   " Profile/Level IDC            : (%d,%d)\n",input->ProfileIDC,input->LevelIDC);
  if(input->MbInterlace)
    fprintf(p_stat, " MB Field Coding : On \n");

  if (input->SearchMode == EPZS)
    EPZSOutputStats(p_stat, 1);

#ifdef _FULL_SEARCH_RANGE_
  if (input->full_search == 2)
    fprintf(p_stat," Search range restrictions    : none\n");
  else if (input->full_search == 1)
    fprintf(p_stat," Search range restrictions    : older reference frames\n");
  else
    fprintf(p_stat," Search range restrictions    : smaller blocks and older reference frames\n");
#endif
  if (input->rdopt)
    fprintf(p_stat," RD-optimized mode decision   : used\n");
  else
    fprintf(p_stat," RD-optimized mode decision   : not used\n");

  fprintf(p_stat," ---------------------|----------------|---------------|\n");
  fprintf(p_stat,"     Item             |     Intra      |   All frames  |\n");
  fprintf(p_stat," ---------------------|----------------|---------------|\n");
  fprintf(p_stat," SNR Y(dB)            |");
  fprintf(p_stat," %5.2f          |",snr->snr_y1);
  fprintf(p_stat," %5.2f         |\n",snr->snr_ya);
  fprintf(p_stat," SNR U/V (dB)         |");
  fprintf(p_stat," %5.2f/%5.2f    |",snr->snr_u1,snr->snr_v1);
  fprintf(p_stat," %5.2f/%5.2f   |\n",snr->snr_ua,snr->snr_va);

  // QUANT.
  fprintf(p_stat," Average quant        |");
  fprintf(p_stat," %5d          |",iabs(input->qp0));
  fprintf(p_stat," %5.2f         |\n",(float)stats->quant1/dmax(1.0,(float)stats->quant0));

  fprintf(p_stat,"\n ---------------------|----------------|---------------|---------------|\n");
  fprintf(p_stat,"     SNR              |        I       |       P       |       B       |\n");
  fprintf(p_stat," ---------------------|----------------|---------------|---------------|\n");
  fprintf(p_stat," SNR Y(dB)            |      %5.3f    |     %5.3f    |     %5.3f    |\n",
                snr->snr_yt[I_SLICE],snr->snr_yt[P_SLICE],snr->snr_yt[B_SLICE]);
  fprintf(p_stat," SNR U(dB)            |      %5.3f    |     %5.3f    |     %5.3f    |\n",
                snr->snr_ut[I_SLICE],snr->snr_ut[P_SLICE],snr->snr_ut[B_SLICE]);
  fprintf(p_stat," SNR V(dB)            |      %5.3f    |     %5.3f    |     %5.3f    |\n",
                snr->snr_vt[I_SLICE],snr->snr_vt[P_SLICE],snr->snr_vt[B_SLICE]);


  // MODE
  fprintf(p_stat,"\n ---------------------|----------------|\n");
  fprintf(p_stat,"   Intra              |   Mode used    |\n");
  fprintf(p_stat," ---------------------|----------------|\n");

  fprintf(p_stat," Mode 0  intra 4x4    |  %5" FORMAT_OFF_T  "         |\n",stats->mode_use[I_SLICE][I4MB ]);
  fprintf(p_stat," Mode 1  intra 8x8    |  %5" FORMAT_OFF_T  "         |\n",stats->mode_use[I_SLICE][I8MB ]);
  fprintf(p_stat," Mode 2+ intra 16x16  |  %5" FORMAT_OFF_T  "         |\n",stats->mode_use[I_SLICE][I16MB]);
  fprintf(p_stat," Mode    intra IPCM   |  %5" FORMAT_OFF_T  "         |\n",stats->mode_use[I_SLICE][IPCM ]);

  fprintf(p_stat,"\n ---------------------|----------------|-----------------|\n");
  fprintf(p_stat,"   Inter              |   Mode used    | MotionInfo bits |\n");
  fprintf(p_stat," ---------------------|----------------|-----------------|");
  fprintf(p_stat,"\n Mode  0  (copy)      |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[P_SLICE][0   ],(double)stats->bit_use_mode[P_SLICE][0   ]/(double)bit_use[P_SLICE][0]);
  fprintf(p_stat,"\n Mode  1  (16x16)     |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[P_SLICE][1   ],(double)stats->bit_use_mode[P_SLICE][1   ]/(double)bit_use[P_SLICE][0]);
  fprintf(p_stat,"\n Mode  2  (16x8)      |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[P_SLICE][2   ],(double)stats->bit_use_mode[P_SLICE][2   ]/(double)bit_use[P_SLICE][0]);
  fprintf(p_stat,"\n Mode  3  (8x16)      |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[P_SLICE][3   ],(double)stats->bit_use_mode[P_SLICE][3   ]/(double)bit_use[P_SLICE][0]);
  fprintf(p_stat,"\n Mode  4  (8x8)       |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[P_SLICE][P8x8],(double)stats->bit_use_mode[P_SLICE][P8x8]/(double)bit_use[P_SLICE][0]);
  fprintf(p_stat,"\n Mode  5  intra 4x4   |  %5" FORMAT_OFF_T  "         |-----------------|",stats->mode_use[P_SLICE][I4MB]);
  fprintf(p_stat,"\n Mode  6  intra 8x8   |  %5" FORMAT_OFF_T  "         |",stats->mode_use[P_SLICE][I8MB]);
  fprintf(p_stat,"\n Mode  7+ intra 16x16 |  %5" FORMAT_OFF_T  "         |",stats->mode_use[P_SLICE][I16MB]);
  fprintf(p_stat,"\n Mode     intra IPCM  |  %5" FORMAT_OFF_T  "         |",stats->mode_use[P_SLICE][IPCM ]);
  mean_motion_info_bit_use[0] = (double)(stats->bit_use_mode[P_SLICE][0] + stats->bit_use_mode[P_SLICE][1] + stats->bit_use_mode[P_SLICE][2]
                                      + stats->bit_use_mode[P_SLICE][3] + stats->bit_use_mode[P_SLICE][P8x8])/(double) bit_use[P_SLICE][0];

  // B pictures
  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
  {

    fprintf(p_stat,"\n\n ---------------------|----------------|-----------------|\n");
    fprintf(p_stat,"   B frame            |   Mode used    | MotionInfo bits |\n");
    fprintf(p_stat," ---------------------|----------------|-----------------|");
    fprintf(p_stat,"\n Mode  0  (copy)      |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[B_SLICE][0   ],(double)stats->bit_use_mode[B_SLICE][0   ]/(double)frame_ctr[B_SLICE]);
    fprintf(p_stat,"\n Mode  1  (16x16)     |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[B_SLICE][1   ],(double)stats->bit_use_mode[B_SLICE][1   ]/(double)frame_ctr[B_SLICE]);
    fprintf(p_stat,"\n Mode  2  (16x8)      |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[B_SLICE][2   ],(double)stats->bit_use_mode[B_SLICE][2   ]/(double)frame_ctr[B_SLICE]);
    fprintf(p_stat,"\n Mode  3  (8x16)      |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[B_SLICE][3   ],(double)stats->bit_use_mode[B_SLICE][3   ]/(double)frame_ctr[B_SLICE]);
    fprintf(p_stat,"\n Mode  4  (8x8)       |  %5" FORMAT_OFF_T  "         |    %8.2f     |",stats->mode_use[B_SLICE][P8x8],(double)stats->bit_use_mode[B_SLICE][P8x8]/(double)frame_ctr[B_SLICE]);
    fprintf(p_stat,"\n Mode  5  intra 4x4   |  %5" FORMAT_OFF_T  "         |-----------------|",stats->mode_use[B_SLICE][I4MB]);
    fprintf(p_stat,"\n Mode  6  intra 8x8   |  %5" FORMAT_OFF_T  "         |",stats->mode_use[B_SLICE][I8MB]);
    fprintf(p_stat,"\n Mode  7+ intra 16x16 |  %5" FORMAT_OFF_T  "         |",stats->mode_use[B_SLICE][I16MB]);
    fprintf(p_stat,"\n Mode     intra IPCM  |  %5" FORMAT_OFF_T  "         |",stats->mode_use[B_SLICE][IPCM ]);
    mean_motion_info_bit_use[1] = (double)(stats->bit_use_mode[B_SLICE][0] + stats->bit_use_mode[B_SLICE][1] + stats->bit_use_mode[B_SLICE][2]
                                      + stats->bit_use_mode[B_SLICE][3] + stats->bit_use_mode[B_SLICE][P8x8])/(double) frame_ctr[B_SLICE];
  }

  fprintf(p_stat,"\n\n ---------------------|----------------|----------------|----------------|\n");
  fprintf(p_stat,"  Bit usage:          |      Intra     |      Inter     |    B frame     |\n");
  fprintf(p_stat," ---------------------|----------------|----------------|----------------|\n");

  fprintf(p_stat," Header               |");
  fprintf(p_stat," %10.2f     |",(float) stats->bit_use_header[I_SLICE]/bit_use[I_SLICE][0]);
  fprintf(p_stat," %10.2f     |",(float) stats->bit_use_header[P_SLICE]/bit_use[P_SLICE][0]);
  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," %10.2f     |",(float) stats->bit_use_header[B_SLICE]/frame_ctr[B_SLICE]);
  else fprintf(p_stat," %10.2f     |", 0.);
  fprintf(p_stat,"\n");

  fprintf(p_stat," Mode                 |");
  fprintf(p_stat," %10.2f     |",(float)stats->bit_use_mb_type[I_SLICE]/bit_use[I_SLICE][0]);
  fprintf(p_stat," %10.2f     |",(float)stats->bit_use_mb_type[P_SLICE]/bit_use[P_SLICE][0]);
  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," %10.2f     |",(float)stats->bit_use_mb_type[B_SLICE]/frame_ctr[B_SLICE]);
  else fprintf(p_stat," %10.2f     |", 0.);
  fprintf(p_stat,"\n");

  fprintf(p_stat," Motion Info          |");
  fprintf(p_stat,"        ./.     |");
  fprintf(p_stat," %10.2f     |",mean_motion_info_bit_use[0]);
  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," %10.2f     |",mean_motion_info_bit_use[1]);
  else fprintf(p_stat," %10.2f     |", 0.);
  fprintf(p_stat,"\n");

  fprintf(p_stat," CBP Y/C              |");
  fprintf(p_stat," %10.2f     |", (float)stats->tmp_bit_use_cbp[I_SLICE]/bit_use[I_SLICE][0]);
  fprintf(p_stat," %10.2f     |", (float)stats->tmp_bit_use_cbp[P_SLICE]/bit_use[P_SLICE][0]);
  if(stats->successive_Bframe!=0 && bit_use[B_SLICE][0]!=0)
    fprintf(p_stat," %10.2f     |", (float)stats->tmp_bit_use_cbp[B_SLICE]/bit_use[B_SLICE][0]);
  else fprintf(p_stat," %10.2f     |", 0.);
  fprintf(p_stat,"\n");

  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," Coeffs. Y            | %10.2f     | %10.2f     | %10.2f     |\n",
    (float)stats->bit_use_coeffY[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_coeffY[P_SLICE]/bit_use[P_SLICE][0], (float)stats->bit_use_coeffY[B_SLICE]/frame_ctr[B_SLICE]);
  else
    fprintf(p_stat," Coeffs. Y            | %10.2f     | %10.2f     | %10.2f     |\n",
      (float)stats->bit_use_coeffY[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_coeffY[P_SLICE]/(float)bit_use[P_SLICE][0], 0.);

  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," Coeffs. C            | %10.2f     | %10.2f     | %10.2f     |\n",
      (float)stats->bit_use_coeffC[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_coeffC[P_SLICE]/bit_use[P_SLICE][0], (float)stats->bit_use_coeffC[B_SLICE]/frame_ctr[B_SLICE]);
  else
    fprintf(p_stat," Coeffs. C            | %10.2f     | %10.2f     | %10.2f     |\n",
      (float)stats->bit_use_coeffC[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_coeffC[P_SLICE]/bit_use[P_SLICE][0], 0.);

  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," Delta quant          | %10.2f     | %10.2f     | %10.2f     |\n",
      (float)stats->bit_use_delta_quant[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_delta_quant[P_SLICE]/bit_use[P_SLICE][0], (float)stats->bit_use_delta_quant[B_SLICE]/frame_ctr[B_SLICE]);
  else
    fprintf(p_stat," Delta quant          | %10.2f     | %10.2f     | %10.2f     |\n",
      (float)stats->bit_use_delta_quant[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_delta_quant[P_SLICE]/bit_use[P_SLICE][0], 0.);

  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," Stuffing Bits        | %10.2f     | %10.2f     | %10.2f     |\n",
      (float)stats->bit_use_stuffingBits[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_stuffingBits[P_SLICE]/bit_use[P_SLICE][0], (float)stats->bit_use_stuffingBits[B_SLICE]/frame_ctr[B_SLICE]);
  else
    fprintf(p_stat," Stuffing Bits        | %10.2f     | %10.2f     | %10.2f     |\n",
      (float)stats->bit_use_stuffingBits[I_SLICE]/bit_use[I_SLICE][0], (float)stats->bit_use_stuffingBits[P_SLICE]/bit_use[P_SLICE][0], 0.);



  fprintf(p_stat," ---------------------|----------------|----------------|----------------|\n");

  fprintf(p_stat," average bits/frame   |");

  fprintf(p_stat," %10.2f     |", (float) bit_use[I_SLICE][1]/(float) bit_use[I_SLICE][0] );
  fprintf(p_stat," %10.2f     |", (float) bit_use[P_SLICE][1]/(float) bit_use[P_SLICE][0] );

  if(stats->successive_Bframe!=0 && frame_ctr[B_SLICE]!=0)
    fprintf(p_stat," %10.2f     |", (float) bit_use[B_SLICE][1]/ (float) frame_ctr[B_SLICE] );
  else fprintf(p_stat," %10.2f     |", 0.);

  fprintf(p_stat,"\n");
  fprintf(p_stat," ---------------------|----------------|----------------|----------------|\n");

  fclose(p_stat);

  // write to log file
  if ((p_log=fopen("log.dat","r"))==0)                      // check if file exist
  {
    if ((p_log=fopen("log.dat","a"))==NULL)            // append new statistic at the end
    {
      snprintf(errortext, ET_SIZE, "Error open file %s  \n","log.dat");
      error(errortext, 500);
    }
    else                                            // Create header for new log file
    {
      fprintf(p_log," ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ \n");
      fprintf(p_log,"|                   Encoder statistics. This file is generated during first encoding session, new sessions will be appended                                                                                                                                                                            |\n");
      fprintf(p_log," ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ \n");
      fprintf(p_log,"|    ver    | Date  | Time  |         Sequence             | #Img |P/MbInt| QPI| QPP| QPB| Format  |Iperiod| #B | FMES | Hdmd | S.R |#Ref | Freq |Coding|RD-opt|Intra upd|8x8Tr| SNRY 1| SNRU 1| SNRV 1| SNRY N| SNRU N| SNRV N|#Bitr I|#Bitr P|#Bitr B|#Bitr IPB|     Total Time   |      Me Time     |\n");
      fprintf(p_log," ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ \n");
    }
  }
  else
  {
    fclose (p_log);
    if ((p_log=fopen("log.dat","a"))==NULL)            // File exist,just open for appending
    {
      snprintf(errortext, ET_SIZE, "Error open file %s  \n","log.dat");
      error(errortext, 500);
    }
  }
  fprintf(p_log,"|%s/%-4s", VERSION, EXT_VERSION);

#ifdef WIN32
  _strdate( timebuf );
  fprintf(p_log,"| %1.5s |",timebuf );

  _strtime( timebuf);
  fprintf(p_log," % 1.5s |",timebuf);
#else
  now = time ((time_t *) NULL); // Get the system time and put it into 'now' as 'calender time'
  time (&now);
  l_time = localtime (&now);
  strftime (string, sizeof string, "%d-%b-%Y", l_time);
  fprintf(p_log,"| %1.5s |",string );

  strftime (string, sizeof string, "%H:%M:%S", l_time);
  fprintf(p_log," %1.5s |",string );
#endif

  for (i=0;i<30;i++)
    name[i]=input->infile[i + imax(0,((int) strlen(input->infile))-30)]; // write last part of path, max 20 chars
  fprintf(p_log,"%30.30s|",name);

  fprintf(p_log,"%5d |",input->no_frames);
  fprintf(p_log,"  %d/%d  |",input->PicInterlace, input->MbInterlace);
  fprintf(p_log," %-3d|",input->qp0);
  fprintf(p_log," %-3d|",input->qpN);
  fprintf(p_log," %-3d|",input->qpB);

  fprintf(p_log,"%4dx%-4d|",input->img_width,input->img_height);

  fprintf(p_log,"  %3d  |",input->intra_period);
  fprintf(p_log,"%3d |",stats->successive_Bframe);

  if (input->SearchMode == UM_HEX)
    fprintf(p_log,"  HEX |");
  else if (input->SearchMode == UM_HEX_SIMPLE)
    fprintf(p_log," SHEX |");
  else if (input->SearchMode == EPZS)
    fprintf(p_log," EPZS |");
  else if (input->SearchMode == FAST_FULL_SEARCH)
    fprintf(p_log,"  FFS |");
  else
    fprintf(p_log,"  FS  |");

  fprintf(p_log,"  %1d%1d%1d |", input->MEErrorMetric[F_PEL], input->MEErrorMetric[H_PEL], input->MEErrorMetric[Q_PEL]);

  fprintf(p_log," %3d |",input->search_range );

  fprintf(p_log," %2d  |",input->num_ref_frames);

  fprintf(p_log," %5.2f|",(img->framerate *(float) (stats->successive_Bframe + 1)) / (float)(input->jumpd+1));

  if (input->symbol_mode == UVLC)
    fprintf(p_log," CAVLC|");
  else
    fprintf(p_log," CABAC|");

  fprintf(p_log,"   %d  |",input->rdopt);

  if (input->intra_upd==1)
    fprintf(p_log,"   ON    |");
  else
    fprintf(p_log,"   OFF   |");

  fprintf(p_log,"  %d  |",input->Transform8x8Mode);

  fprintf(p_log,"%7.3f|",snr->snr_y1);
  fprintf(p_log,"%7.3f|",snr->snr_u1);
  fprintf(p_log,"%7.3f|",snr->snr_v1);
  fprintf(p_log,"%7.3f|",snr->snr_ya);
  fprintf(p_log,"%7.3f|",snr->snr_ua);
  fprintf(p_log,"%7.3f|",snr->snr_va);
/*
  fprintf(p_log,"%-5.3f|",snr->snr_yt[I_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_ut[I_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_vt[I_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_yt[P_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_ut[P_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_vt[P_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_yt[B_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_ut[B_SLICE]);
  fprintf(p_log,"%-5.3f|",snr->snr_vt[B_SLICE]);
*/
  fprintf(p_log,"%7.0f|",stats->bitrate_I);
  fprintf(p_log,"%7.0f|",stats->bitrate_P);
  fprintf(p_log,"%7.0f|",stats->bitrate_B);
  fprintf(p_log,"%9.0f|",stats->bitrate);

  fprintf(p_log,"   %12d   |", (int)tot_time);
  fprintf(p_log,"   %12d   |", (int)me_tot_time);
  fprintf(p_log,"\n");

  fclose(p_log);

  p_log=fopen("data.txt","a");

  if(stats->successive_Bframe != 0 && frame_ctr[B_SLICE] != 0) // B picture used
  {
    fprintf(p_log, "%3d %2d %2d %2.2f %2.2f %2.2f %5" FORMAT_OFF_T  " "
          "%2.2f %2.2f %2.2f %5d "
        "%2.2f %2.2f %2.2f %5" FORMAT_OFF_T  " %5" FORMAT_OFF_T  " %.3f\n",
        input->no_frames, input->qp0, input->qpN,
        snr->snr_y1,
        snr->snr_u1,
        snr->snr_v1,
        stats->bit_ctr_I,
        0.0,
        0.0,
        0.0,
        0,
        snr->snr_ya,
        snr->snr_ua,
        snr->snr_va,
        (stats->bit_ctr_I+stats->bit_ctr)/(input->no_frames+frame_ctr[B_SLICE]),
        stats->bit_ctr_B/frame_ctr[B_SLICE],
        (double)0.001*tot_time/(input->no_frames+frame_ctr[B_SLICE]));
  }
  else
  {
    if (input->no_frames!=0)
    fprintf(p_log, "%3d %2d %2d %2.2f %2.2f %2.2f %5" FORMAT_OFF_T  " "
          "%2.2f %2.2f %2.2f %5d "
        "%2.2f %2.2f %2.2f %5" FORMAT_OFF_T  " %5d %.3f\n",
        input->no_frames, input->qp0, input->qpN,
        snr->snr_y1,
        snr->snr_u1,
        snr->snr_v1,
        stats->bit_ctr_I,
        0.0,
        0.0,
        0.0,
        0,
        snr->snr_ya,
        snr->snr_ua,
        snr->snr_va,
        (stats->bit_ctr_I+stats->bit_ctr)/input->no_frames,
        0,
        (double)0.001*tot_time/input->no_frames);
  }

  fclose(p_log);

  if (input->ReportFrameStats)
  {
    if ((p_log=fopen("stat_frame.dat","a"))==NULL)            // append new statistic at the end
    {
      snprintf(errortext, ET_SIZE, "Error open file %s  \n","stat_frame.dat.dat");
      //    error(errortext, 500);
    }
    else
    {
      fprintf(p_log," --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n");
      fclose(p_log);
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Prints the header of the protocol.
 * \par Input:
 *    struct inp_par *inp
 * \par Output:
 *    none
 ************************************************************************
 */
void information_init(void)
{
  char yuv_types[4][10]= {"YUV 4:0:0","YUV 4:2:0","YUV 4:2:2","YUV 4:4:4"};
  if (input->Verbose == 0 || input->Verbose  == 1)
    printf("------------------------------- JM %s %s --------------------------------\n",VERSION, EXT_VERSION);
  else
    printf("------------------------------- JM %s %s ------------------------------------------\n",VERSION, EXT_VERSION);
  printf(" Input YUV file                    : %s \n",mybasename(input->infile));
  printf(" Output H.264 bitstream            : %s \n",input->outfile);
  if (p_dec != -1)
    printf(" Output YUV file                   : %s \n",input->ReconFile);
  printf(" YUV Format                        : %s \n", &yuv_types[img->yuv_format][0]);//img->yuv_format==YUV422?"YUV 4:2:2":(img->yuv_format==YUV444)?"YUV 4:4:4":"YUV 4:2:0");
  printf(" Frames to be encoded I-P/B        : %d/%d\n", input->no_frames, (input->successive_Bframe*(input->no_frames-1)));
  printf(" PicInterlace / MbInterlace        : %d/%d\n", input->PicInterlace, input->MbInterlace);
  printf(" Transform8x8Mode                  : %d\n", input->Transform8x8Mode);

  switch (input->Verbose)
  {
    case 1:
      printf("-------------------------------------------------------------------------------\n");
      printf("  Frame  Bit/pic    QP   SnrY    SnrU    SnrV    Time(ms) MET(ms) Frm/Fld Ref  \n");
      printf("-------------------------------------------------------------------------------\n");
      break;
    case 2:
      printf("---------------------------------------------------------------------------------------------\n");
      printf("  Frame  Bit/pic WP QP   SnrY    SnrU    SnrV    Time(ms) MET(ms) Frm/Fld   I D L0 L1 RDP Ref\n");
      printf("---------------------------------------------------------------------------------------------\n");
      break;
    case 0:
    default:
      printf("-------------------------------------------------------------------------------\n");
      printf("\nEncoding. Please Wait.\n\n");
      break;
  }
}

/*!
 ************************************************************************
 * \brief
 *    memory allocation for original picture buffers
 ************************************************************************
 */
int init_orig_buffers(void)
{
  int memory_size = 0;

  // allocate memory for reference frame buffers: imgY_org_frm, imgUV_org_frm
  memory_size += get_mem2Dpel(&imgY_org_frm, img->height, img->width);

  if (img->yuv_format != YUV400)
    memory_size += get_mem3Dpel(&imgUV_org_frm, 2, img->height_cr, img->width_cr);


  if(!active_sps->frame_mbs_only_flag)
  {
    // allocate memory for reference frame buffers: imgY_org, imgUV_org
    init_top_bot_planes(imgY_org_frm, img->height, img->width, &imgY_org_top, &imgY_org_bot);

    if (img->yuv_format != YUV400)
    {
      if(((imgUV_org_top) = (imgpel***)calloc(2,sizeof(imgpel**))) == NULL)
        no_mem_exit("init_global_buffers: imgUV_org_top");
      if(((imgUV_org_bot) = (imgpel***)calloc(2,sizeof(imgpel**))) == NULL)
        no_mem_exit("init_global_buffers: imgUV_org_bot");

      memory_size += 4*(sizeof(imgpel**));

      memory_size += init_top_bot_planes(imgUV_org_frm[0], img->height_cr, img->width_cr, &(imgUV_org_top[0]), &(imgUV_org_bot[0]));
      memory_size += init_top_bot_planes(imgUV_org_frm[1], img->height_cr, img->width_cr, &(imgUV_org_top[1]), &(imgUV_org_bot[1]));
    }
  }
  return memory_size;
}

/*!
 ************************************************************************
 * \brief
 *    Dynamic memory allocation of frame size related global buffers
 *    buffers are defined in global.h, allocated memory must be freed in
 *    void free_global_buffers()
 * \par Input:
 *    Input Parameters struct inp_par *inp,                            \n
 *    Image Parameters struct img_par *img
 * \return Number of allocated bytes
 ************************************************************************
 */
int init_global_buffers(void)
{
  int j,memory_size=0;
#ifdef _ADAPT_LAST_GROUP_
  extern int *last_P_no_frm;
  extern int *last_P_no_fld;

  if ((last_P_no_frm = (int*)malloc(2*img->max_num_references*sizeof(int))) == NULL)
    no_mem_exit("init_global_buffers: last_P_no");
  if(!active_sps->frame_mbs_only_flag)
    if ((last_P_no_fld = (int*)malloc(4*img->max_num_references*sizeof(int))) == NULL)
      no_mem_exit("init_global_buffers: last_P_no");
#endif

  memory_size += init_orig_buffers();

  memory_size += get_mem2Dint(&PicPos,img->FrameSizeInMbs + 1,2);

  for (j=0;j< (int) img->FrameSizeInMbs + 1;j++)
  {
    PicPos[j][0] = (j % img->PicWidthInMbs);
    PicPos[j][1] = (j / img->PicWidthInMbs);
  }

  if (input->WeightedPrediction || input->WeightedBiprediction || input->GenerateMultiplePPS)
  {
    // Currently only use up to 20 references. Need to use different indicator such as maximum num of references in list
    memory_size += get_mem3Dint(&wp_weight,6,MAX_REFERENCE_PICTURES,3);
    memory_size += get_mem3Dint(&wp_offset,6,MAX_REFERENCE_PICTURES,3);

    memory_size += get_mem4Dint(&wbp_weight, 6, MAX_REFERENCE_PICTURES, MAX_REFERENCE_PICTURES, 3);
  }

  // allocate memory for reference frames of each block: refFrArr

  if(input->successive_Bframe!=0 || input->BRefPictures> 0)
  {
    memory_size += get_mem3D((byte ****)(void*)(&direct_ref_idx), 2, img->height_blk, img->width_blk);
    memory_size += get_mem2D((byte ***)(void*)&direct_pdir, img->height_blk, img->width_blk);
  }

  if (input->rdopt==3)
  {
    memory_size += get_mem2Dint(&decs->resY, MB_BLOCK_SIZE, MB_BLOCK_SIZE);
    if ((decs->decref = (imgpel****) calloc(input->NoOfDecoders,sizeof(imgpel***))) == NULL)
      no_mem_exit("init_global_buffers: decref");
    for (j=0 ; j<input->NoOfDecoders; j++)
    {
      memory_size += get_mem3Dpel(&decs->decref[j], img->max_num_references+1, img->height, img->width);
    }
    memory_size += get_mem2Dpel(&decs->RefBlock, BLOCK_SIZE,BLOCK_SIZE);
    memory_size += get_mem3Dpel(&decs->decY, input->NoOfDecoders, img->height, img->width);
    memory_size += get_mem3Dpel(&decs->decY_best, input->NoOfDecoders, img->height, img->width);
    memory_size += get_mem2D(&decs->status_map, img->FrameHeightInMbs, img->PicWidthInMbs);
    memory_size += get_mem2D(&decs->dec_mb_mode,img->FrameHeightInMbs, img->PicWidthInMbs);
  }
  if (input->RestrictRef)
  {
    memory_size += get_mem2D(&pixel_map, img->height,img->width);
    memory_size += get_mem2D(&refresh_map, img->height/8,img->width/8);
  }

  if(!active_sps->frame_mbs_only_flag)
  {
    memory_size += get_mem2Dpel(&imgY_com, img->height, img->width);

    if (img->yuv_format != YUV400)
    {
      memory_size += get_mem3Dpel(&imgUV_com, 2, img->height_cr, img->width_cr);
    }
  }

  // allocate and set memory relating to motion estimation
  if (input->SearchMode == UM_HEX)
  {
    memory_size += UMHEX_get_mem();
  }
  else if (input->SearchMode == UM_HEX_SIMPLE)
  {
    smpUMHEX_init();
    memory_size += smpUMHEX_get_mem();
  }
  else if (input->SearchMode == EPZS)
    memory_size += EPZSInit();


  if (input->RCEnable)
  {
    generic_alloc( &generic_RC );
    rc_alloc( &quadratic_RC );
    // RDPictureDecision
    if ( input->RDPictureDecision || input->MbInterlace == ADAPTIVE_CODING )
    {
      // INIT
      generic_alloc( &generic_RC_init );
      rc_alloc( &quadratic_RC_init );
      // BEST
      generic_alloc( &generic_RC_best );
      rc_alloc( &quadratic_RC_best );
    }
  }

  if(input->redundant_pic_flag)
  {
    memory_size += get_mem2Dpel(&imgY_tmp, img->height, input->img_width);
    memory_size += get_mem2Dpel(&imgUV_tmp[0], input->img_height/2, input->img_width/2);
    memory_size += get_mem2Dpel(&imgUV_tmp[1], input->img_height/2, input->img_width/2);
  }

  memory_size += get_mem2Dint (&imgY_sub_tmp, img->height_padded, img->width_padded);
  img_padded_size_x = (img->width + 2 * IMG_PAD_SIZE);
  img_cr_padded_size_x = (img->width_cr + 2 * img_pad_size_uv_x);

  return (memory_size);
}


/*!
 ************************************************************************
 * \brief
 *    Free allocated memory of original picture buffers
 ************************************************************************
 */
void free_orig_planes(void)
{
  free_mem2Dpel(imgY_org_frm);      // free ref frame buffers

  if (img->yuv_format != YUV400)
    free_mem3Dpel(imgUV_org_frm, 2);


  if(!active_sps->frame_mbs_only_flag)
  {
    free_top_bot_planes(imgY_org_top, imgY_org_bot);

    if (img->yuv_format != YUV400)
    {
      free_top_bot_planes(imgUV_org_top[0], imgUV_org_bot[0]);
      free_top_bot_planes(imgUV_org_top[1], imgUV_org_bot[1]);
      free (imgUV_org_top);
      free (imgUV_org_bot);
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Free allocated memory of frame size related global buffers
 *    buffers are defined in global.h, allocated memory is allocated in
 *    int get_mem4global_buffers()
 * \par Input:
 *    Input Parameters struct inp_par *inp,                             \n
 *    Image Parameters struct img_par *img
 * \par Output:
 *    none
 ************************************************************************
 */
void free_global_buffers(void)
{
  int  i,j;

#ifdef _ADAPT_LAST_GROUP_
  extern int *last_P_no_frm;
  extern int *last_P_no_fld;
  free (last_P_no_frm);
  free (last_P_no_fld);
#endif

  free_orig_planes();
  // free lookup memory which helps avoid divides with PicWidthInMbs
  free_mem2Dint(PicPos);
  // Free Qmatrices and offsets
  free_QMatrix();
  free_QOffsets();

  if (input->WeightedPrediction || input->WeightedBiprediction || input->GenerateMultiplePPS)
  {
    free_mem3Dint(wp_weight,6);
    free_mem3Dint(wp_offset,6);
    free_mem4Dint(wbp_weight,6,MAX_REFERENCE_PICTURES);
  }

  if(stats->successive_Bframe!=0 || input->BRefPictures> 0)
  {
    free_mem3D((byte ***)direct_ref_idx,2);
    free_mem2D((byte **) direct_pdir);
  } // end if B frame

  if (imgY_sub_tmp) // free temp quarter pel frame buffers
  {
    free_mem2Dint (imgY_sub_tmp);
    imgY_sub_tmp=NULL;
  }

  // free mem, allocated in init_img()
  // free intra pred mode buffer for blocks
  free_mem2D((byte**)img->ipredmode);
  free_mem2D((byte**)img->ipredmode8x8);
  free(img->mb_data);

  free_mem2D((byte**)rddata_top_frame_mb.ipredmode);

  if(input->UseConstrainedIntraPred)
  {
    free (img->intra_block);
  }

  if (input->CtxAdptLagrangeMult == 1)
  {
    free(mb16x16_cost_frame);
  }

  if (input->rdopt==3)
  {
    free(decs->resY[0]);
    free(decs->resY);
    free(decs->RefBlock[0]);
    free(decs->RefBlock);
    for (j=0; j<input->NoOfDecoders; j++)
    {
      free(decs->decY[j][0]);
      free(decs->decY[j]);
      free(decs->decY_best[j][0]);
      free(decs->decY_best[j]);
      for (i=0; i<img->max_num_references+1; i++)
      {
        free(decs->decref[j][i][0]);
        free(decs->decref[j][i]);
      }
      free(decs->decref[j]);
    }
    free(decs->decY);
    free(decs->decY_best);
    free(decs->decref);
    free(decs->status_map[0]);
    free(decs->status_map);
    free(decs->dec_mb_mode[0]);
    free(decs->dec_mb_mode);
  }
  if (input->RestrictRef)
  {
    free(pixel_map[0]);
    free(pixel_map);
    free(refresh_map[0]);
    free(refresh_map);
  }

  if(!active_sps->frame_mbs_only_flag)
  {
    free_mem2Dpel(imgY_com);
    if (img->yuv_format != YUV400)
    {
      free_mem3Dpel(imgUV_com,2);
    }
  }

  free_mem3Dint(img->nz_coeff, img->FrameSizeInMbs);

  free_mem2Ddb_offset (img->lambda_md, img->bitdepth_luma_qp_scale);
  free_mem3Ddb_offset (img->lambda_me, 10, 52 + img->bitdepth_luma_qp_scale, img->bitdepth_luma_qp_scale);
  free_mem3Dint_offset(img->lambda_mf, 10, 52 + img->bitdepth_luma_qp_scale, img->bitdepth_luma_qp_scale);

  if (input->CtxAdptLagrangeMult == 1)
  {
    free_mem2Ddb_offset(img->lambda_mf_factor,img->bitdepth_luma_qp_scale);
  }

  if(input->SearchMode == UM_HEX)
  {
    UMHEX_free_mem();
  }
  else if (input->SearchMode == UM_HEX_SIMPLE)
  {
    smpUMHEX_free_mem();
  }
  else if (input->SearchMode == EPZS)
  {
    EPZSDelete();
  }


  if (input->RCEnable)
  {
    generic_free( &generic_RC );
    rc_free( &quadratic_RC );
    // RDPictureDecision
    if ( input->RDPictureDecision || input->MbInterlace == ADAPTIVE_CODING )
    {
      // INIT
      generic_free( &generic_RC_init );
      rc_free( &quadratic_RC_init );
      // BEST
      generic_free( &generic_RC_best );
      rc_free( &quadratic_RC_best );
    }
  }

  if(input->redundant_pic_flag)
  {
    free_mem2Dpel(imgY_tmp);
    free_mem2Dpel(imgUV_tmp[0]);
    free_mem2Dpel(imgUV_tmp[1]);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Allocate memory for mv
 * \par Input:
 *    Image Parameters struct img_par *img                             \n
 *    int****** mv
 * \return memory size in bytes
 ************************************************************************
 */
int get_mem_mv (short ******* mv)
{
  int i, j, k, l, m;

  if ((*mv = (short******)calloc(4,sizeof(short*****))) == NULL)
    no_mem_exit ("get_mem_mv: mv");
  for (i=0; i<4; i++)
  {
    if (((*mv)[i] = (short*****)calloc(4,sizeof(short****))) == NULL)
      no_mem_exit ("get_mem_mv: mv");
    for (j=0; j<4; j++)
    {
      if (((*mv)[i][j] = (short****)calloc(2,sizeof(short***))) == NULL)
        no_mem_exit ("get_mem_mv: mv");
      for (k=0; k<2; k++)
      {
        if (((*mv)[i][j][k] = (short***)calloc(img->max_num_references,sizeof(short**))) == NULL)
          no_mem_exit ("get_mem_mv: mv");
        for (l=0; l<img->max_num_references; l++)
        {
          if (((*mv)[i][j][k][l] = (short**)calloc(9,sizeof(short*))) == NULL)
            no_mem_exit ("get_mem_mv: mv");
          if (((*mv)[i][j][k][l][0] = (short*)calloc(2*9,sizeof(short))) == NULL)
            no_mem_exit ("get_mem_mv: mv");
          for (m=1; m<9; m++)
            (*mv)[i][j][k][l][m] = (*mv)[i][j][k][l][m - 1] + 2;
        }
      }
    }
  }
  return 4*4*img->max_num_references*9*2*sizeof(short);
}


/*!
 ************************************************************************
 * \brief
 *    Free memory from mv
 * \par Input:
 *    int****** mv
 ************************************************************************
 */
void free_mem_mv (short****** mv)
{
  int i, j, k, l;

  for (i=0; i<4; i++)
  {
    for (j=0; j<4; j++)
    {
      for (k=0; k<2; k++)
      {
        for (l=0; l<img->max_num_references; l++)
        {
          free (mv[i][j][k][l][0]);
          free (mv[i][j][k][l]);
        }
        free (mv[i][j][k]);
      }
      free (mv[i][j]);
    }
    free (mv[i]);
  }
  free (mv);
}


/*!
 ************************************************************************
 * \brief
 *    Allocate memory for AC coefficients
 ************************************************************************
 */
int get_mem_ACcoeff (int***** cofAC)
{
  int i, j, k;
  int num_blk8x8 = 4 + img->num_blk8x8_uv;

  if ((*cofAC = (int****)calloc (num_blk8x8, sizeof(int***))) == NULL)              no_mem_exit ("get_mem_ACcoeff: cofAC");
  for (k=0; k<num_blk8x8; k++)
  {
    if (((*cofAC)[k] = (int***)calloc (4, sizeof(int**))) == NULL)         no_mem_exit ("get_mem_ACcoeff: cofAC");
    for (j=0; j<4; j++)
    {
      if (((*cofAC)[k][j] = (int**)calloc (2, sizeof(int*))) == NULL)      no_mem_exit ("get_mem_ACcoeff: cofAC");
      for (i=0; i<2; i++)
      {
        if (((*cofAC)[k][j][i] = (int*)calloc (65, sizeof(int))) == NULL)  no_mem_exit ("get_mem_ACcoeff: cofAC"); // 18->65 for ABT
      }
    }
  }
  return num_blk8x8*4*2*65*sizeof(int);// 18->65 for ABT
}

/*!
 ************************************************************************
 * \brief
 *    Allocate memory for DC coefficients
 ************************************************************************
 */
int get_mem_DCcoeff (int**** cofDC)
{
  int j, k;

  if ((*cofDC = (int***)calloc (3, sizeof(int**))) == NULL)           no_mem_exit ("get_mem_DCcoeff: cofDC");
  for (k=0; k<3; k++)
  {
    if (((*cofDC)[k] = (int**)calloc (2, sizeof(int*))) == NULL)      no_mem_exit ("get_mem_DCcoeff: cofDC");
    for (j=0; j<2; j++)
    {
      if (((*cofDC)[k][j] = (int*)calloc (65, sizeof(int))) == NULL)  no_mem_exit ("get_mem_DCcoeff: cofDC"); // 18->65 for ABT
    }
  }
  return 3*2*65*sizeof(int); // 18->65 for ABT
}


/*!
 ************************************************************************
 * \brief
 *    Free memory of AC coefficients
 ************************************************************************
 */
void free_mem_ACcoeff (int**** cofAC)
{
  int i, j, k;

  for (k=0; k<4+img->num_blk8x8_uv; k++)
  {
    for (i=0; i<4; i++)
    {
      for (j=0; j<2; j++)
      {
        free (cofAC[k][i][j]);
      }
      free (cofAC[k][i]);
    }
    free (cofAC[k]);
  }
  free (cofAC);
}

/*!
 ************************************************************************
 * \brief
 *    Free memory of DC coefficients
 ************************************************************************
 */
void free_mem_DCcoeff (int*** cofDC)
{
  int i, j;

  for (j=0; j<3; j++)
  {
    for (i=0; i<2; i++)
    {
      free (cofDC[j][i]);
    }
    free (cofDC[j]);
  }
  free (cofDC);
}


/*!
 ************************************************************************
 * \brief
 *    form frame picture from two field pictures
 ************************************************************************
 */
void combine_field(void)
{
  int i;

  for (i=0; i<img->height / 2; i++)
  {
    memcpy(imgY_com[i*2], enc_top_picture->imgY[i], img->width*sizeof(imgpel));     // top field
    memcpy(imgY_com[i*2 + 1], enc_bottom_picture->imgY[i], img->width*sizeof(imgpel)); // bottom field
  }

  if (img->yuv_format != YUV400)
  {
    for (i=0; i<img->height_cr / 2; i++)
    {
      memcpy(imgUV_com[0][i*2],     enc_top_picture->imgUV[0][i],    img->width_cr*sizeof(imgpel));
      memcpy(imgUV_com[0][i*2 + 1], enc_bottom_picture->imgUV[0][i], img->width_cr*sizeof(imgpel));
      memcpy(imgUV_com[1][i*2],     enc_top_picture->imgUV[1][i],    img->width_cr*sizeof(imgpel));
      memcpy(imgUV_com[1][i*2 + 1], enc_bottom_picture->imgUV[1][i], img->width_cr*sizeof(imgpel));
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    RD decision of frame and field coding
 ************************************************************************
 */
int decide_fld_frame(float snr_frame_Y, float snr_field_Y, int bit_field, int bit_frame, double lambda_picture)
{
  double cost_frame, cost_field;

  cost_frame = bit_frame * lambda_picture + snr_frame_Y;
  cost_field = bit_field * lambda_picture + snr_field_Y;

  if (cost_field > cost_frame)
    return (0);
  else
    return (1);
}

/*!
 ************************************************************************
 * \brief
 *    Do some initialization work for encoding the 2nd IGOP
 ************************************************************************
 */
void process_2nd_IGOP(void)
{
  Boolean FirstIGOPFinished = FALSE;
  if ( img->number == input->no_frames-1 )
    FirstIGOPFinished = TRUE;
  if (input->NumFrameIn2ndIGOP==0) return;
  if (!FirstIGOPFinished || In2ndIGOP) return;
  In2ndIGOP = TRUE;

//  img->number = -1;
  start_frame_no_in_this_IGOP = input->no_frames;
  start_tr_in_this_IGOP = (input->no_frames-1)*(input->jumpd+1) +1;
  input->no_frames = input->no_frames + input->NumFrameIn2ndIGOP;

/*  reset_buffers();

  frm->picbuf_short[0]->used=0;
  frm->picbuf_short[0]->picID=-1;
  frm->picbuf_short[0]->lt_picID=-1;
  frm->short_used = 0; */
}


/*!
 ************************************************************************
 * \brief
 *    Set the image type for I,P and SP pictures (not B!)
 ************************************************************************
 */
void SetImgType(void)
{
  int intra_refresh = input->intra_period == 0 ? (IMG_NUMBER == 0) : ((IMG_NUMBER%input->intra_period) == 0);

  if (intra_refresh)
  {
    img->type = I_SLICE;        // set image type for first image to I-frame
  }
  else
  {
    img->type = input->sp_periodicity && ((IMG_NUMBER % input->sp_periodicity) ==0) ? SP_SLICE : ((input->BRefPictures == 2) ? B_SLICE : P_SLICE);
  }
}


void SetLevelIndices(void)
{
  switch(active_sps->level_idc)
  {
  case 9:
    img->LevelIndex=1;
    break;
  case 10:
    img->LevelIndex=0;
    break;
  case 11:
    if ((active_sps->profile_idc < FREXT_HP)&&(active_sps->constrained_set3_flag == 0))
      img->LevelIndex=2;
    else
      img->LevelIndex=1;
    break;
  case 12:
    img->LevelIndex=3;
    break;
  case 13:
    img->LevelIndex=4;
    break;
  case 20:
    img->LevelIndex=5;
    break;
  case 21:
    img->LevelIndex=6;
    break;
  case 22:
    img->LevelIndex=7;
    break;
  case 30:
    img->LevelIndex=8;
    break;
  case 31:
    img->LevelIndex=9;
    break;
  case 32:
    img->LevelIndex=10;
    break;
  case 40:
    img->LevelIndex=11;
    break;
  case 41:
    img->LevelIndex=12;
    break;
  case 42:
    if (active_sps->profile_idc <= 88)
      img->LevelIndex=13;
    else
      img->LevelIndex=14;
    break;
  case 50:
    img->LevelIndex=15;
    break;
  case 51:
    img->LevelIndex=16;
    break;
  default:
    fprintf ( stderr, "Warning: unknown LevelIDC, using maximum level 5.1 \n" );
    img->LevelIndex=16;
    break;
  }
}

/*!
 ************************************************************************
 * \brief
 *    initialize key frames and corresponding redundant frames.
 ************************************************************************
 */
void Init_redundant_frame()
{
  if(input->redundant_pic_flag)
  {
    if(input->successive_Bframe)
    {
      error("B frame not supported when redundant picture used!",100);
    }

    if(input->PicInterlace)
    {
      error("Interlace not supported when redundant picture used!",100);
    }

    if(input->num_ref_frames<input->PrimaryGOPLength)
    {
      error("NumberReferenceFrames must be no less than PrimaryGOPLength",100);
    }

    if((1<<input->NumRedundantHierarchy)>input->PrimaryGOPLength)
    {
      error("PrimaryGOPLength must be greater than 2^NumRedundantHeirarchy",100);
    }

    if(input->Verbose!=1)
    {
      error("Redundant slices not supported when Verbose!=1",100);
    }
  }

  key_frame = 0;
  redundant_coding = 0;
  img->redundant_pic_cnt = 0;
  frameNuminGOP = img->number % input->PrimaryGOPLength;
  if(img->number == 0)
  {
    frameNuminGOP = -1;
  }
}

/*!
 ************************************************************************
 * \brief
 *    allocate redundant frames in a primary GOP.
 ************************************************************************
 */
void Set_redundant_frame()
{
  int GOPlength = input->PrimaryGOPLength;

  //start frame of GOP
  if(frameNuminGOP == 0)
  {
    redundant_coding = 0;
    key_frame = 1;
    redundant_ref_idx = GOPlength;
  }

  //1/2 position
  if(input->NumRedundantHierarchy>0)
  {
    if(frameNuminGOP == GOPlength/2)
    {
      redundant_coding = 0;
      key_frame = 1;
      redundant_ref_idx = GOPlength/2;
    }
  }

  //1/4, 3/4 position
  if(input->NumRedundantHierarchy>1)
  {
    if(frameNuminGOP == GOPlength/4 || frameNuminGOP == GOPlength*3/4)
    {
      redundant_coding = 0;
      key_frame = 1;
      redundant_ref_idx = GOPlength/4;
    }
  }

  //1/8, 3/8, 5/8, 7/8 position
  if(input->NumRedundantHierarchy>2)
  {
    if(frameNuminGOP == GOPlength/8 || frameNuminGOP == GOPlength*3/8
      || frameNuminGOP == GOPlength*5/8 || frameNuminGOP == GOPlength*7/8)
    {
      redundant_coding = 0;
      key_frame = 1;
      redundant_ref_idx = GOPlength/8;
    }
  }

  //1/16, 3/16, 5/16, 7/16, 9/16, 11/16, 13/16 position
  if(input->NumRedundantHierarchy>3)
  {
    if(frameNuminGOP == GOPlength/16 || frameNuminGOP == GOPlength*3/16
      || frameNuminGOP == GOPlength*5/16 || frameNuminGOP == GOPlength*7/16
      || frameNuminGOP == GOPlength*9/16 || frameNuminGOP == GOPlength*11/16
      || frameNuminGOP == GOPlength*13/16)
    {
      redundant_coding = 0;
      key_frame = 1;
      redundant_ref_idx = GOPlength/16;
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    encode on redundant frame.
 ************************************************************************
 */
void encode_one_redundant_frame()
{
  key_frame = 0;
  redundant_coding = 1;
  img->redundant_pic_cnt = 1;

  if(img->type == I_SLICE)
  {
    img->type = P_SLICE;
  }

  encode_one_frame();
}

/*!
 ************************************************************************
 * \brief
 *    Setup Chroma MC Variables
 ************************************************************************
 */
void chroma_mc_setup(void)
{
  // initialize global variables used for chroma interpolation and buffering
  if ( img->yuv_format == YUV420 )
  {
    img_pad_size_uv_x = IMG_PAD_SIZE >> 1;
    img_pad_size_uv_y = IMG_PAD_SIZE >> 1;
    chroma_mask_mv_y = 7;
    chroma_mask_mv_x = 7;
    chroma_shift_x = 3;
    chroma_shift_y = 3;
  }
  else if ( img->yuv_format == YUV422 )
  {
    img_pad_size_uv_x = IMG_PAD_SIZE >> 1;
    img_pad_size_uv_y = IMG_PAD_SIZE;
    chroma_mask_mv_y = 3;
    chroma_mask_mv_x = 7;
    chroma_shift_y = 2;
    chroma_shift_x = 3;
  }
  else
  { // YUV444
    img_pad_size_uv_x = IMG_PAD_SIZE;
    img_pad_size_uv_y = IMG_PAD_SIZE;
    chroma_mask_mv_y = 3;
    chroma_mask_mv_x = 3;
    chroma_shift_y = 2;
    chroma_shift_x = 2;
  }
  shift_cr_y = chroma_shift_y - 2;
  shift_cr_x = chroma_shift_x - 1;

}

