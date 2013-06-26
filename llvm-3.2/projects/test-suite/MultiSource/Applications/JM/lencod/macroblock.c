
/*!
 *************************************************************************************
 * \file macroblock.c
 *
 * \brief
 *    Process one macroblock
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Inge Lille-Langoy               <inge.lille-langoy@telenor.com>
 *    - Rickard Sjoberg                 <rickard.sjoberg@era.ericsson.se>
 *    - Jani Lainema                    <jani.lainema@nokia.com>
 *    - Sebastian Purreiter             <sebastian.purreiter@mch.siemens.de>
 *    - Detlev Marpe                    <marpe@hhi.de>
 *    - Thomas Wedi                     <wedi@tnt.uni-hannover.de>
 *    - Ragip Kurceren                  <ragip.kurceren@nokia.com>
 *    - Alexis Michael Tourapis         <alexismt@ieee.org>
 *************************************************************************************
 */

#include "contributors.h"

#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <memory.h>

#include "global.h"

#include "elements.h"
#include "macroblock.h"
#include "refbuf.h"
#include "fmo.h"
#include "vlc.h"
#include "image.h"
#include "mb_access.h"
#include "ratectl.h"              // header file for rate control
#include "rc_quadratic.h"
#include "cabac.h"
#include "transform8x8.h"
#include "me_fullsearch.h"
#include "symbol.h"

#if TRACE
#define TRACE_SE(trace,str)  snprintf(trace,TRACESTRING_SIZE,str)
#else
#define TRACE_SE(trace,str)
#endif

extern const byte QP_SCALE_CR[52] ;

//Rate control
int        predict_error,dq;
extern int delta_qp_mbaff[2][2],delta_qp_mbaff[2][2];
extern int qp_mbaff[2][2],qp_mbaff[2][2];

// function pointer for different ways of obtaining chroma interpolation
static void (*OneComponentChromaPrediction4x4) (imgpel* , int , int , short****** , int , short , int , int );
static void OneComponentChromaPrediction4x4_regenerate (imgpel* , int , int , short****** , int , short , int , int );
static void OneComponentChromaPrediction4x4_retrieve (imgpel* , int , int , short****** , int , short , int , int );

static int  slice_too_big(int rlc_bits);

static int  writeChromaIntraPredMode (void);
static int  writeMotionInfo2NAL      (void);
static int  writeChromaCoeff         (void);
static int  writeCBPandLumaCoeff     (void);

extern int *mvbits;

extern int QP2QUANT[40];
extern int ver_offset[4][8][4];
extern int hor_offset[4][8][4];

static int diff  [16];
static int diff64[64];
static const unsigned char subblk_offset_x[3][8][4] =
{
  { {0, 4, 0, 4},
    {0, 4, 0, 4},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}, },

  { {0, 4, 0, 4},
    {0, 4, 0, 4},
    {0, 4, 0, 4},
    {0, 4, 0, 4},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}, },

  { {0, 4, 0, 4},
    {8,12, 8,12},
    {0, 4, 0, 4},
    {8,12, 8,12},
    {0, 4, 0, 4},
    {8,12, 8,12},
    {0, 4, 0, 4},
    {8,12, 8,12}  }
};

static const unsigned char subblk_offset_y[3][8][4] =
{
  { {0, 0, 4, 4},
    {0, 0, 4, 4},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}, },

  { {0, 0, 4, 4},
    {8, 8,12,12},
    {0, 0, 4, 4},
    {8, 8,12,12},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0} },

 { {0, 0, 4, 4},
   {0, 0, 4, 4},
   {8, 8,12,12},
   {8, 8,12,12},
   {0, 0, 4, 4},
   {0, 0, 4, 4},
   {8, 8,12,12},
   {8, 8,12,12} }
};


 /*!
 ************************************************************************
 * \brief
 *    updates the coordinates for the next macroblock to be processed
 *
 * \param mb_addr
 *    macroblock address in scan order
 ************************************************************************
 */
void set_MB_parameters (int mb_addr)
{
  img->current_mb_nr = mb_addr;

  get_mb_block_pos(mb_addr, &img->mb_x, &img->mb_y);

  img->block_x = img->mb_x << 2;
  img->block_y = img->mb_y << 2;

  img->pix_x   = img->block_x << 2;
  img->pix_y   = img->block_y << 2;

  img->opix_x   = img->pix_x;

  if (img->MbaffFrameFlag)
  {
    if (img->mb_data[mb_addr].mb_field)
    {
      imgY_org  = (mb_addr % 2) ? imgY_org_bot  : imgY_org_top;
      imgUV_org = (mb_addr % 2) ? imgUV_org_bot : imgUV_org_top;
      img->opix_y   = (img->mb_y >> 1 ) << 4;
      img->mb_data[mb_addr].list_offset = (mb_addr % 2) ? 4 : 2;
    }
    else
    {
      imgY_org  = imgY_org_frm;
      imgUV_org = imgUV_org_frm;
      img->opix_y   = img->block_y << 2;
      img->mb_data[mb_addr].list_offset = 0;
    }
  }
  else
  {
    img->opix_y   = img->block_y << 2;
    img->mb_data[mb_addr].list_offset = 0;
  }

  if (img->yuv_format != YUV400)
  {
    img->pix_c_x = (img->mb_cr_size_x * img->pix_x) >> 4;
    img->pix_c_y = (img->mb_cr_size_y * img->pix_y) >> 4;

    img->opix_c_x = (img->mb_cr_size_x * img->opix_x) >> 4;
    img->opix_c_y = (img->mb_cr_size_y * img->opix_y) >> 4;
  }
  //  printf ("set_MB_parameters: mb %d,  mb_x %d,  mb_y %d\n", mb_addr, img->mb_x, img->mb_y);
}


/*!
 ************************************************************************
 * \brief
 *    updates the coordinates and statistics parameter for the
 *    next macroblock
 ************************************************************************
 */
void proceed2nextMacroblock(void)
{
#if TRACE
  int use_bitstream_backing = (input->slice_mode == FIXED_RATE || input->slice_mode == CALLBACK);
#endif
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];
  int*        bitCount = currMB->bitcounter;
  int i;

  if (bitCount[BITS_TOTAL_MB] > img->max_bitCount)
    printf("Warning!!! Number of bits (%d) of macroblock_layer() data seems to exceed defined limit (%d).\n", bitCount[BITS_TOTAL_MB],img->max_bitCount);

  // Update the statistics
  stats->bit_use_mb_type[img->type]      += bitCount[BITS_MB_MODE];
  stats->bit_use_coeffY[img->type]       += bitCount[BITS_COEFF_Y_MB] ;
  stats->tmp_bit_use_cbp[img->type]      += bitCount[BITS_CBP_MB];
  stats->bit_use_coeffC[img->type]       += bitCount[BITS_COEFF_UV_MB];
  stats->bit_use_delta_quant[img->type]  += bitCount[BITS_DELTA_QUANT_MB];

  if (IS_INTRA(currMB))
  {
    ++stats->intra_chroma_mode[currMB->c_ipred_mode];

    if ((currMB->cbp&15) != 0)
    {
      if (currMB->luma_transform_size_8x8_flag)
        ++stats->mode_use_transform_8x8[img->type][currMB->mb_type];
      else
        ++stats->mode_use_transform_4x4[img->type][currMB->mb_type];
    }
  }

   ++stats->mode_use[img->type][currMB->mb_type];
   stats->bit_use_mode[img->type][currMB->mb_type]+= bitCount[BITS_INTER_MB];

   if (img->type != I_SLICE)
   {
     if (currMB->mb_type == P8x8)
     {
       for(i=0;i<4;i++)
       {
         if (currMB->b8mode[i] > 0)
           ++stats->mode_use[img->type][currMB->b8mode[i]];
         else
           ++stats->b8_mode_0_use[img->type][currMB->luma_transform_size_8x8_flag];

         if (currMB->b8mode[i]==4)
         {
           if ((currMB->luma_transform_size_8x8_flag && (currMB->cbp&15) != 0) || input->Transform8x8Mode == 2)
             ++stats->mode_use_transform_8x8[img->type][4];
           else
             ++stats->mode_use_transform_4x4[img->type][4];
         }
       }
     }
     else if (currMB->mb_type >= 0 && currMB->mb_type <=3 && ((currMB->cbp&15) != 0))
     {
       if (currMB->luma_transform_size_8x8_flag)
         ++stats->mode_use_transform_8x8[img->type][currMB->mb_type];
       else
         ++stats->mode_use_transform_4x4[img->type][currMB->mb_type];
     }
   }

  // Statistics
  if ((img->type == P_SLICE)||(img->type==SP_SLICE) )
  {
    ++stats->quant0;
    stats->quant1 += currMB->qp;      // to find average quant for inter frames
  }
}

/*!
************************************************************************
* \brief
*    updates chroma QP according to luma QP and bit depth
************************************************************************
*/
void set_chroma_qp(Macroblock *currMB)
{
  int i;
  for (i=0; i<2; i++)
  {
    currMB->qpc[i] = iClip3 ( -img->bitdepth_chroma_qp_scale, 51, currMB->qp + img->chroma_qp_offset[i] );
    currMB->qpc[i] = currMB->qpc[i] < 0 ? currMB->qpc[i] : QP_SCALE_CR[currMB->qpc[i]];
  }
}

/*!
 ************************************************************************
 * \brief
 *    initializes the current macroblock
 *
 * \param mb_addr
 *    macroblock address in scan order
 * \param mb_field
 *    true for field macroblock coding
 ************************************************************************
 */
void start_macroblock(int mb_addr, int mb_field)
{
  int i,j,l;
  int use_bitstream_backing = (input->slice_mode == FIXED_RATE || input->slice_mode == CALLBACK);
  Macroblock *currMB = &img->mb_data[mb_addr];
  Slice *curr_slice = img->currentSlice;
  DataPartition *dataPart;
  Bitstream *currStream;
  int prev_mb;

  currMB->mb_field = mb_field;

  enc_picture->mb_field[mb_addr] = mb_field;
  currMB->is_field_mode = (img->field_picture || ( img->MbaffFrameFlag && currMB->mb_field));

  set_MB_parameters (mb_addr);

  prev_mb = FmoGetPreviousMBNr(img->current_mb_nr);

  if(use_bitstream_backing)
  {
    if ((!input->MbInterlace)||((mb_addr&1)==0)) // KS: MB AFF -> store stream positions for
                                               //       first macroblock only
    {
      // Keep the current state of the bitstreams
      if(!img->cod_counter)
      {
        for (i=0; i<curr_slice->max_part_nr; i++)
        {
          dataPart = &(curr_slice->partArr[i]);
          currStream = dataPart->bitstream;
          currStream->stored_bits_to_go = currStream->bits_to_go;
          currStream->stored_byte_pos   = currStream->byte_pos;
          currStream->stored_byte_buf   = currStream->byte_buf;
          stats->stored_bit_slice       = stats->bit_slice;

          if (input->symbol_mode ==CABAC)
          {
            dataPart->ee_recode = dataPart->ee_cabac;
          }
        }
      }
    }
  }

  // Save the slice number of this macroblock. When the macroblock below
  // is coded it will use this to decide if prediction for above is possible
  currMB->slice_nr = img->current_slice_nr;

  // Initialize delta qp change from last macroblock. Feature may be used for future rate control
  // Rate control
  currMB->qpsp       = img->qpsp;
  if(input->RCEnable)
  {
    if (prev_mb > -1)
    {
      if ( input->MbInterlace == ADAPTIVE_CODING && !img->bot_MB && currMB->mb_field )
      {
        currMB->qp = img->qp = img->mb_data[prev_mb].qp;
      }
      currMB->prev_qp = img->mb_data[prev_mb].qp;
      if (img->mb_data[prev_mb].slice_nr == img->current_slice_nr)
      {
        currMB->prev_delta_qp = img->mb_data[prev_mb].delta_qp;
      }
      else
      {
        currMB->prev_delta_qp = 0;
      }
    }
    else
    {
      currMB->prev_qp = curr_slice->qp;
      currMB->prev_delta_qp = 0;
    }
    // frame layer rate control
    if(input->basicunit==img->FrameSizeInMbs)
    {
      currMB->delta_qp = 0;
      currMB->qp       = img->qp;
    }
    // basic unit layer rate control
    else
    {
      // each I or B frame has only one QP
      if( ((img->type == I_SLICE || img->type == B_SLICE) && input->RCUpdateMode != RC_MODE_1 ) || (!IMG_NUMBER) )
      {
        currMB->delta_qp = 0;
        currMB->qp       = img->qp;
      }
      else if( img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1 )
      {
        if (!img->write_macroblock) //write macroblock
        {
          if (prev_mb < 0) //first macroblock (of slice)
          {
            // Initialize delta qp change from last macroblock. Feature may be used for future rate control
            currMB->delta_qp = 0;
            currMB->qp       = img->qp;
            delta_qp_mbaff[currMB->mb_field][img->bot_MB] = currMB->delta_qp;
            qp_mbaff      [currMB->mb_field][img->bot_MB] = currMB->qp;
          }
          else
          {
            if (!((input->MbInterlace) && img->bot_MB)) //top macroblock
            {
              if (img->mb_data[prev_mb].prev_cbp == 1)
              {
                currMB->delta_qp = 0;
                currMB->qp       = img->qp;
              }
              else
              {
                currMB->qp = img->mb_data[prev_mb].prev_qp;
                currMB->delta_qp = currMB->qp - img->mb_data[prev_mb].qp;
                img->qp = currMB->qp;
              }
              delta_qp_mbaff[currMB->mb_field][img->bot_MB] = currMB->delta_qp;
              qp_mbaff      [currMB->mb_field][img->bot_MB] = currMB->qp;
            }
            else //bottom macroblock
            {
              // Initialize delta qp change from last macroblock. Feature may be used for future rate control
              currMB->delta_qp = 0;
              currMB->qp       = img->qp;       // needed in loop filter (even if constant QP is used)
            }
          }
        }
        else
        {
          if (!img->bot_MB) //write top macroblock
          {
            if (img->write_mbaff_frame)
            {
              currMB->delta_qp = delta_qp_mbaff[0][img->bot_MB];
              img->qp = currMB->qp =   qp_mbaff[0][img->bot_MB];
              //set_chroma_qp(currMB);
            }
            else
            {
              if (prev_mb < 0) //first macroblock (of slice)
              {
                // Initialize delta qp change from last macroblock. Feature may be used for future rate control
                currMB->delta_qp = 0;
                currMB->qp       = img->qp;
                delta_qp_mbaff[currMB->mb_field][img->bot_MB] = currMB->delta_qp;
                qp_mbaff      [currMB->mb_field][img->bot_MB] = currMB->qp;
              }
              else
              {
                currMB->delta_qp = delta_qp_mbaff[1][img->bot_MB];
                img->qp = currMB->qp =   qp_mbaff[1][img->bot_MB];
                //set_chroma_qp(currMB);
              }
            }
          }
          else //write bottom macroblock
          {
            currMB->delta_qp = 0;
            currMB->qp = img->qp;
            set_chroma_qp(currMB);
          }
        }

        // compute the quantization parameter for each basic unit of P frame
        if (!img->write_macroblock)
        {
          if(!((input->MbInterlace) && img->bot_MB))
          {
            if((img->NumberofCodedMacroBlocks>0) && (img->NumberofCodedMacroBlocks%img->BasicUnit==0))
            {
              // frame coding
              if(active_sps->frame_mbs_only_flag)
              {
                updateRCModel(quadratic_RC);
                img->BasicUnitQP=updateQP(quadratic_RC, generic_RC->TopFieldFlag);
              }
              // picture adaptive field/frame coding
              else if((input->PicInterlace!=FRAME_CODING)&&(!input->MbInterlace)&&(generic_RC->NoGranularFieldRC==0))
              {
                updateRCModel(quadratic_RC);
                img->BasicUnitQP=updateQP(quadratic_RC, generic_RC->TopFieldFlag);
              }
              // mb adaptive f/f coding, field coding
              else if((input->MbInterlace))
              {
                updateRCModel(quadratic_RC);
                img->BasicUnitQP=updateQP(quadratic_RC, generic_RC->TopFieldFlag);
              }
            }

            if(img->current_mb_nr==0)
              img->BasicUnitQP=img->qp;

            currMB->predict_qp = iClip3(currMB->qp - img->min_qp_delta, currMB->qp + img->max_qp_delta, img->BasicUnitQP);

            dq = currMB->delta_qp + currMB->predict_qp - currMB->qp;
            if(dq < -img->min_qp_delta)
            {
              dq = -img->min_qp_delta;
              predict_error = dq-currMB->delta_qp;
              img->qp = img->qp+predict_error;
              currMB->delta_qp = -img->min_qp_delta;
            }
            else if(dq > img->max_qp_delta)
            {
              dq = img->max_qp_delta;
              predict_error = dq - currMB->delta_qp;
              img->qp = img->qp + predict_error;
              currMB->delta_qp = img->max_qp_delta;
            }
            else
            {
              currMB->delta_qp = dq;
              predict_error=currMB->predict_qp-currMB->qp;
              img->qp = currMB->predict_qp;
            }
            currMB->qp =  img->qp;
            if (input->MbInterlace)
            {
              delta_qp_mbaff[currMB->mb_field][img->bot_MB] = currMB->delta_qp;
              qp_mbaff      [currMB->mb_field][img->bot_MB] = currMB->qp;
            }
            currMB->predict_error=predict_error;
          }
          else
            currMB->prev_qp=img->qp;
        }
      }
    }
  }
  else
  {
    Slice* currSlice = img->currentSlice;

    if (prev_mb>-1)
    {
      currMB->prev_qp = img->mb_data[prev_mb].qp;
      currMB->prev_delta_qp = (img->mb_data[prev_mb].slice_nr == img->current_slice_nr) ? img->mb_data[prev_mb].delta_qp : 0;
    }
    else
    {
      currMB->prev_qp = currSlice->qp;
      currMB->prev_delta_qp = 0;
    }

    currMB->qp = currSlice->qp ;

    currMB->delta_qp = currMB->qp - currMB->prev_qp;
    delta_qp_mbaff[currMB->mb_field][img->bot_MB] = currMB->delta_qp;
    qp_mbaff      [currMB->mb_field][img->bot_MB] = currMB->qp;
  }
  img->qp_scaled = img->qp + img->bitdepth_luma_qp_scale;

  set_chroma_qp (currMB);

  // loop filter parameter
  if (active_pps->deblocking_filter_control_present_flag)
  {
    currMB->LFDisableIdc    = img->LFDisableIdc;
    currMB->LFAlphaC0Offset = img->LFAlphaC0Offset;
    currMB->LFBetaOffset    = img->LFBetaOffset;
  }
  else
  {
    currMB->LFDisableIdc    = 0;
    currMB->LFAlphaC0Offset = 0;
    currMB->LFBetaOffset    = 0;
  }

  // If MB is next to a slice boundary, mark neighboring blocks unavailable for prediction
  CheckAvailabilityOfNeighbors();

  if (input->symbol_mode == CABAC)
    CheckAvailabilityOfNeighborsCABAC();

  // Reset vectors and reference indices
  for (l=0; l<2; l++)
  {
    for (j=img->block_y; j < img->block_y + BLOCK_MULTIPLE; j++)
    {
      memset(&enc_picture->ref_idx[l][j][img->block_x], -1, BLOCK_MULTIPLE * sizeof(char));
      memset(enc_picture->mv [l][j][img->block_x], 0, 2 * BLOCK_MULTIPLE * sizeof(short));
      for (i=img->block_x; i < img->block_x + BLOCK_MULTIPLE; i++)
        enc_picture->ref_pic_id[l][j][i]= -1;
    }
  }

  // Reset syntax element entries in MB struct
  currMB->mb_type      = 0;
  currMB->cbp_blk      = 0;
  currMB->cbp          = 0;
  currMB->cbp_bits     = 0;
  currMB->c_ipred_mode = DC_PRED_8;

  memset (currMB->mvd, 0, BLOCK_CONTEXT * sizeof(int));
  memset (currMB->intra_pred_modes, DC_PRED, MB_BLOCK_PARTITIONS * sizeof(char)); // changing this to char would allow us to use memset
  memset (currMB->intra_pred_modes8x8, DC_PRED, MB_BLOCK_PARTITIONS * sizeof(char));

  //initialize the whole MB as INTRA coded
  //Blocks are set to notINTRA in write_one_macroblock
  if (input->UseConstrainedIntraPred)
  {
    img->intra_block[img->current_mb_nr] = 1;
  }

  // Initialize bitcounters for this macroblock
  if(prev_mb < 0) // No slice header to account for
  {
    currMB->bitcounter[BITS_HEADER] = 0;
  }
  else if (currMB->slice_nr == img->mb_data[prev_mb].slice_nr) // current MB belongs to the
  // same slice as the last MB
  {
    currMB->bitcounter[BITS_HEADER] = 0;
  }

  currMB->bitcounter[BITS_MB_MODE       ] = 0;
  currMB->bitcounter[BITS_COEFF_Y_MB    ] = 0;
  currMB->bitcounter[BITS_INTER_MB      ] = 0;
  currMB->bitcounter[BITS_CBP_MB        ] = 0;
  currMB->bitcounter[BITS_DELTA_QUANT_MB] = 0;
  currMB->bitcounter[BITS_COEFF_UV_MB   ] = 0;

  if(input->SearchMode == FAST_FULL_SEARCH)
    ResetFastFullIntegerSearch ();

  // disable writing of trace file
#if TRACE
  curr_slice->partArr[0].bitstream->trace_enabled = FALSE;
  if (input->partition_mode)
  {
    curr_slice->partArr[1].bitstream->trace_enabled = FALSE;
    curr_slice->partArr[2].bitstream->trace_enabled = FALSE;
  }
#endif
}

/*!
 ************************************************************************
 * \brief
 *    terminates processing of the current macroblock depending
 *    on the chosen slice mode
 ************************************************************************
 */
void terminate_macroblock( Boolean *end_of_slice,      //!< returns true for last macroblock of a slice, otherwise false
                           Boolean *recode_macroblock  //!< returns true if max. slice size is exceeded an macroblock must be recoded in next slice
                           )
{
  int i;
  Slice *currSlice = img->currentSlice;
  Macroblock    *currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement se;
  int *partMap = assignSE2partition[input->partition_mode];
  DataPartition *dataPart;
  Bitstream *currStream;
  int rlc_bits=0;
  int use_bitstream_backing = (input->slice_mode == FIXED_RATE || input->slice_mode == CALLBACK);
  int new_slice;
  static int skip = FALSE;


  // if previous mb in the same slice group has different slice number as the current, it's the
  // the start of new slice
  new_slice=0;
  if ( (img->current_mb_nr==0) || (FmoGetPreviousMBNr(img->current_mb_nr)<0) )
    new_slice=1;
  else if( img->mb_data[FmoGetPreviousMBNr(img->current_mb_nr)].slice_nr != img->current_slice_nr )
    new_slice=1;

  *recode_macroblock=FALSE;

  switch(input->slice_mode)
  {
  case NO_SLICES:
    currSlice->num_mb++;
    *recode_macroblock = FALSE;
    if ((currSlice->num_mb) == (int)img->PicSizeInMbs) // maximum number of MBs reached
      *end_of_slice = TRUE;

    // if it's end of current slice group, slice ends too
    *end_of_slice = (Boolean) (*end_of_slice | (img->current_mb_nr == FmoGetLastCodedMBOfSliceGroup (FmoMB2SliceGroup (img->current_mb_nr))));

    break;
  case FIXED_MB:
    // For slice mode one, check if a new slice boundary follows
    currSlice->num_mb++;
    *recode_macroblock = FALSE;
    //! Check end-of-slice group condition first
    *end_of_slice = (Boolean) (img->current_mb_nr == FmoGetLastCodedMBOfSliceGroup (FmoMB2SliceGroup (img->current_mb_nr)));
    //! Now check maximum # of MBs in slice
    *end_of_slice = (Boolean) (*end_of_slice | (currSlice->num_mb >= input->slice_argument));

    break;

    // For slice modes two and three, check if coding of this macroblock
    // resulted in too many bits for this slice. If so, indicate slice
    // boundary before this macroblock and code the macroblock again
  case FIXED_RATE:
    // in case of skip MBs check if there is a slice boundary
    // only for UVLC (img->cod_counter is always 0 in case of CABAC)
    if(img->cod_counter)
    {
      // write out the skip MBs to know how many bits we need for the RLC
      se.value1 = img->cod_counter;
      se.value2 = 0;
      se.type = SE_MBTYPE;
      dataPart = &(currSlice->partArr[partMap[se.type]]);

      TRACE_SE (se.tracestring, "mb_skip_run");
      writeSE_UVLC(&se, dataPart);
      rlc_bits=se.len;

      currStream = dataPart->bitstream;
      // save the bitstream as it would be if we write the skip MBs
      currStream->bits_to_go_skip  = currStream->bits_to_go;
      currStream->byte_pos_skip    = currStream->byte_pos;
      currStream->byte_buf_skip    = currStream->byte_buf;
      // restore the bitstream
      currStream->bits_to_go = currStream->stored_bits_to_go;
      currStream->byte_pos = currStream->stored_byte_pos;
      currStream->byte_buf = currStream->stored_byte_buf;
      skip = TRUE;
    }
    //! Check if the last coded macroblock fits into the size of the slice
    //! But only if this is not the first macroblock of this slice
    if (!new_slice)
    {
      if(slice_too_big(rlc_bits))
      {
        *recode_macroblock = TRUE;
        *end_of_slice = TRUE;
      }
      else if(!img->cod_counter)
        skip = FALSE;
    }
    // maximum number of MBs

    // check if current slice group is finished
    if ((*recode_macroblock == FALSE) && (img->current_mb_nr == FmoGetLastCodedMBOfSliceGroup (FmoMB2SliceGroup (img->current_mb_nr))))
    {
      *end_of_slice = TRUE;
      if(!img->cod_counter)
        skip = FALSE;
    }

    //! (first MB OR first MB in a slice) AND bigger that maximum size of slice
    if (new_slice && slice_too_big(rlc_bits))
    {
      *end_of_slice = TRUE;
      if(!img->cod_counter)
        skip = FALSE;
    }
    if (!*recode_macroblock)
      currSlice->num_mb++;
    break;

  case  CALLBACK:
    if (img->current_mb_nr > 0 && !new_slice)
    {
      if (currSlice->slice_too_big(rlc_bits))
      {
        *recode_macroblock = TRUE;
        *end_of_slice = TRUE;
      }
    }

    if ( (*recode_macroblock == FALSE) && (img->current_mb_nr == FmoGetLastCodedMBOfSliceGroup (FmoMB2SliceGroup (img->current_mb_nr))))
      *end_of_slice = TRUE;
    break;

  default:
    snprintf(errortext, ET_SIZE, "Slice Mode %d not supported", input->slice_mode);
    error(errortext, 600);
  }

  if (*recode_macroblock == TRUE)
  {
    // Restore everything
    for (i=0; i<currSlice->max_part_nr; i++)
    {
      dataPart = &(currSlice->partArr[i]);
      currStream = dataPart->bitstream;
      currStream->bits_to_go = currStream->stored_bits_to_go;
      currStream->byte_pos  = currStream->stored_byte_pos;
      currStream->byte_buf  = currStream->stored_byte_buf;
      stats->bit_slice      = stats->stored_bit_slice;

      if (input->symbol_mode == CABAC)
      {
        dataPart->ee_cabac = dataPart->ee_recode;
      }
    }
  }

  if (input->symbol_mode == UVLC)
  {
    // Skip MBs at the end of this slice
    dataPart = &(currSlice->partArr[partMap[SE_MBTYPE]]);
    if(*end_of_slice == TRUE  && skip == TRUE)
    {
      // only for Slice Mode 2 or 3
      // If we still have to write the skip, let's do it!
      if(img->cod_counter && *recode_macroblock == TRUE) // MB that did not fit in this slice
      {
        // If recoding is true and we have had skip,
        // we have to reduce the counter in case of recoding
        img->cod_counter--;
        if(img->cod_counter)
        {
          se.value1 = img->cod_counter;
          se.value2 = 0;
          se.type = SE_MBTYPE;
#if TRACE
          snprintf(se.tracestring, TRACESTRING_SIZE, "Final MB runlength = %3d",img->cod_counter);
#endif
          writeSE_UVLC(&se, dataPart);
          rlc_bits=se.len;
          currMB->bitcounter[BITS_MB_MODE]+=rlc_bits;
          img->cod_counter = 0;
        }
      }
      else //! MB that did not fit in this slice anymore is not a Skip MB
      {
        currStream = dataPart->bitstream;
        // update the bitstream
        currStream->bits_to_go = currStream->bits_to_go_skip;
        currStream->byte_pos  = currStream->byte_pos_skip;
        currStream->byte_buf  = currStream->byte_buf_skip;

        // update the statistics
        img->cod_counter = 0;
        skip = FALSE;
      }
    }

    // Skip MBs at the end of this slice for Slice Mode 0 or 1
    if(*end_of_slice == TRUE && img->cod_counter && !use_bitstream_backing)
    {
      se.value1 = img->cod_counter;
      se.value2 = 0;
      se.type = SE_MBTYPE;

      TRACE_SE (se.tracestring, "mb_skip_run");
      writeSE_UVLC(&se, dataPart);

      rlc_bits=se.len;
      currMB->bitcounter[BITS_MB_MODE]+=rlc_bits;
      img->cod_counter = 0;
    }
  }
}

/*!
 *****************************************************************************
 *
 * \brief
 *    For Slice Mode 2: Checks if one partition of one slice exceeds the
 *    allowed size
 *
 * \return
 *    FALSE if all Partitions of this slice are smaller than the allowed size
 *    TRUE is at least one Partition exceeds the limit
 *
 * \par Side effects
 *    none
 *
 * \date
 *    4 November 2001
 *
 * \author
 *    Tobias Oelbaum      drehvial@gmx.net
 *****************************************************************************/

 int slice_too_big(int rlc_bits)
 {
   Slice *currSlice = img->currentSlice;
   DataPartition *dataPart;
   Bitstream *currStream;
   EncodingEnvironmentPtr eep;
   int i;
   int size_in_bytes;

   //! UVLC
   if (input->symbol_mode == UVLC)
   {
     for (i=0; i<currSlice->max_part_nr; i++)
     {
       dataPart = &(currSlice->partArr[i]);
       currStream = dataPart->bitstream;
       size_in_bytes = currStream->byte_pos /*- currStream->tmp_byte_pos*/;

       if (currStream->bits_to_go < 8)
         size_in_bytes++;
       if (currStream->bits_to_go < rlc_bits)
         size_in_bytes++;
       if(size_in_bytes > input->slice_argument)
         return TRUE;
     }
   }

   //! CABAC
   if (input->symbol_mode ==CABAC)
   {
     for (i=0; i<currSlice->max_part_nr; i++)
     {
        dataPart= &(currSlice->partArr[i]);
        eep = &(dataPart->ee_cabac);

       if( arienco_bits_written(eep) > (input->slice_argument*8))
          return TRUE;
     }
   }
   return FALSE;
 }

/*!
 ************************************************************************
 * \brief
 *    Predict one component of a 4x4 Luma block
 ************************************************************************
 */
void OneComponentLumaPrediction4x4 ( imgpel*   mpred,          //!< array of prediction values (row by row)
                                     int    pic_pix_x,      //!< absolute horizontal coordinate of 4x4 block
                                     int    pic_pix_y,      //!< absolute vertical   coordinate of 4x4 block
                                     short* mv,             //!< motion vector
                                     short  ref,            //!< reference frame
                                     StorablePicture **list //!< reference picture list
                                     )
{
  int     j;
  imgpel *ref_line;

  width_pad  = list[ref]->size_x_pad;
  height_pad = list[ref]->size_y_pad;

  ref_line = UMVLine4X (list[ref]->imgY_sub, pic_pix_y + mv[1], pic_pix_x + mv[0]);
  
  for (j = 0; j < BLOCK_SIZE; j++) 
  {
    memcpy(mpred, ref_line, BLOCK_SIZE * sizeof(imgpel));
    ref_line += img_padded_size_x;
    mpred += BLOCK_SIZE;
  }  
}


/*!
 ************************************************************************
 * \brief
 *    Predict one 4x4 Luma block
 ************************************************************************
 */
void LumaPrediction4x4 ( int   block_x,    //!< relative horizontal block coordinate of 4x4 block
                         int   block_y,    //!< relative vertical   block coordinate of 4x4 block
                         int   p_dir,      //!< prediction direction (0=list0, 1=list1, 2=bipred)
                         int   l0_mode,    //!< list0 prediction mode (1-7, 0=DIRECT if l1_mode=0)
                         int   l1_mode,    //!< list1 prediction mode (1-7, 0=DIRECT if l0_mode=0)
                         short l0_ref_idx, //!< reference frame for list0 prediction (-1: Intra4x4 pred. with l0_mode)
                         short l1_ref_idx  //!< reference frame for list1 prediction 
                         )
{
  static imgpel l0_pred[16];
  static imgpel l1_pred[16];

  int  i, j;
  int  block_x4  = block_x+4;
  int  block_y4  = block_y+4;
  int  pic_opix_x = ((img->opix_x + block_x) << 2) + IMG_PAD_SIZE_TIMES4;
  int  pic_opix_y = ((img->opix_y + block_y) << 2) + IMG_PAD_SIZE_TIMES4;
  int  bx        = block_x >> 2;
  int  by        = block_y >> 2;
  imgpel* l0pred     = l0_pred;
  imgpel* l1pred     = l1_pred;
  Macroblock*    currMB     = &img->mb_data[img->current_mb_nr];

  int  apply_weights = ( (active_pps->weighted_pred_flag && (img->type== P_SLICE || img->type == SP_SLICE)) ||
                         (active_pps->weighted_bipred_idc && (img->type== B_SLICE)));
  short**** mv_array = img->all_mv[by][bx];


  if (currMB->bi_pred_me && l0_ref_idx == 0 && l1_ref_idx == 0 && p_dir == 2 && l0_mode==1 && l1_mode==1)
  {
    mv_array = currMB->bi_pred_me == 1? img->bipred_mv1[by][bx] : img->bipred_mv2[by][bx];
  }

  switch (p_dir)
  {
  case 0:
    OneComponentLumaPrediction4x4 (l0_pred, pic_opix_x, pic_opix_y, mv_array[LIST_0][l0_ref_idx][l0_mode], l0_ref_idx, listX[0+currMB->list_offset]);   
    break;
  case 1:
    OneComponentLumaPrediction4x4 (l1_pred, pic_opix_x, pic_opix_y, mv_array[LIST_1][l1_ref_idx][l1_mode], l1_ref_idx, listX[1+currMB->list_offset]);   
    break;
  case 2:
    OneComponentLumaPrediction4x4 (l0_pred, pic_opix_x, pic_opix_y, mv_array[LIST_0][l0_ref_idx][l0_mode], l0_ref_idx, listX[0+currMB->list_offset]);   
    OneComponentLumaPrediction4x4 (l1_pred, pic_opix_x, pic_opix_y, mv_array[LIST_1][l1_ref_idx][l1_mode], l1_ref_idx, listX[1+currMB->list_offset]);   
    break;
  default:
    break;
  }

  if (apply_weights)
  {

    if (p_dir==2)
    {
      int wbp0 = wbp_weight[0][l0_ref_idx][l1_ref_idx][0];
      int wbp1 = wbp_weight[1][l0_ref_idx][l1_ref_idx][0];
      int offset = (wp_offset[0][l0_ref_idx][0] + wp_offset[1][l1_ref_idx][0] + 1)>>1;
      int wp_round = 2*wp_luma_round;
      int weight_denom = luma_log_weight_denom + 1;
      for   (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)  
          img->mpr[j][i] = iClip1( img->max_imgpel_value, 
          ((wbp0 * *l0pred++ + wbp1 * *l1pred++ + wp_round) >> (weight_denom)) + offset); 
    }
    else if (p_dir==0)
    {
      int wp = wp_weight[0][l0_ref_idx][0];
      int offset = wp_offset[0][l0_ref_idx][0];
      for   (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)
          img->mpr[j][i] = iClip1( img->max_imgpel_value, 
          ((wp * *l0pred++  + wp_luma_round) >> luma_log_weight_denom) + offset);
    }
    else // p_dir==1
    {
      int wp = wp_weight[1][l1_ref_idx][0];
      int offset = wp_offset[1][l1_ref_idx][0];
      for   (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)
          img->mpr[j][i] = iClip1( img->max_imgpel_value, 
          ((wp * *l1pred++  + wp_luma_round) >> luma_log_weight_denom) + offset );
    }
  }
  else
  {
    if (p_dir==2)
    {
      for   (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)
          img->mpr[j][i] = (*l0pred++ + *l1pred++ + 1) >> 1;
    }
    else if (p_dir==0)
    {
      for (j=block_y; j<block_y4; j++)
      {
        memcpy(&(img->mpr[j][block_x]), l0pred, BLOCK_SIZE * sizeof(imgpel));
        l0pred += BLOCK_SIZE;
      }
    }
    else // p_dir==1
    {
      for   (j=block_y; j<block_y4; j++)
      {
        memcpy(&(img->mpr[j][block_x]), l1pred, BLOCK_SIZE * sizeof(imgpel));
        l1pred += BLOCK_SIZE;
      }
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Predict one 4x4 Luma block
 ************************************************************************
 */
void LumaPrediction4x4Bi ( int   block_x,    //!< relative horizontal block coordinate of 4x4 block
                           int   block_y,    //!< relative vertical   block coordinate of 4x4 block
                           int   l0_mode,    //!< list0 prediction mode (1-7, 0=DIRECT if l1_mode=0)
                           int   l1_mode,    //!< list1 prediction mode (1-7, 0=DIRECT if l0_mode=0)
                           short l0_ref_idx, //!< reference frame for list0 prediction (-1: Intra4x4 pred. with l0_mode)
                           short l1_ref_idx, //!< reference frame for list1 prediction 
                           int   list        //!< current list for prediction.
                           )
{
  static imgpel l0_pred[16];
  static imgpel l1_pred[16];

  int  i, j;
  int  block_x4  = block_x+4;
  int  block_y4  = block_y+4;
  int  pic_opix_x = ((img->opix_x + block_x) << 2) + IMG_PAD_SIZE_TIMES4;
  int  pic_opix_y = ((img->opix_y + block_y) << 2) + IMG_PAD_SIZE_TIMES4;
  int  bx        = block_x >> 2;
  int  by        = block_y >> 2;
  imgpel* l0pred     = l0_pred;
  imgpel* l1pred     = l1_pred;
  Macroblock*    currMB     = &img->mb_data[img->current_mb_nr];
   
  int  apply_weights = ( (active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
                         (active_pps->weighted_bipred_idc && (img->type == B_SLICE)));  
  short   ****mv_array = list ? img->bipred_mv1[by][bx] : img->bipred_mv2[by][bx];

  OneComponentLumaPrediction4x4 (l0_pred, pic_opix_x, pic_opix_y, mv_array[LIST_0][l0_ref_idx][l0_mode], l0_ref_idx, listX[0+currMB->list_offset]);
  OneComponentLumaPrediction4x4 (l1_pred, pic_opix_x, pic_opix_y, mv_array[LIST_1][l1_ref_idx][l1_mode], l1_ref_idx, listX[1+currMB->list_offset]);

  if (apply_weights)
  {
    int wbp0 = wbp_weight[0][l0_ref_idx][l1_ref_idx][0];
    int wbp1 = wbp_weight[1][l0_ref_idx][l1_ref_idx][0];
    int offset = (wp_offset[0][l0_ref_idx][0] + wp_offset[1][l1_ref_idx][0] + 1)>>1;
    for   (j=block_y; j<block_y4; j++)
      for (i=block_x; i<block_x4; i++)
        img->mpr[j][i] = iClip1( img->max_imgpel_value,
        ((wbp0 * *l0pred++ + wbp1 * *l1pred++ + 2*wp_luma_round) >> (luma_log_weight_denom + 1)) + offset);
  }
  else
  {
    for   (j=block_y; j<block_y4; j++)
      for (i=block_x; i<block_x4; i++)
        img->mpr[j][i] = (*l0pred++ + *l1pred++ + 1) >> 1;
  }
}


/*!
 ************************************************************************
 * \brief
 *    Residual Coding of an 8x8 Luma block (not for intra)
 *
 * \return
 *    coefficient cost
 ************************************************************************
 */
int LumaResidualCoding8x8 ( int   *cbp,        //!< Output: cbp (updated according to processed 8x8 luminance block)
                            int64 *cbp_blk,    //!< Output: block cbp (updated according to processed 8x8 luminance block)
                            int   block8x8,    //!< block number of 8x8 block
                            short p_dir,       //!< prediction direction
                            int   l0_mode,     //!< list0 prediction mode (1-7, 0=DIRECT)
                            int   l1_mode,     //!< list1 prediction mode (1-7, 0=DIRECT)
                            short l0_ref_idx, //!< reference picture for list0 prediction
                            short l1_ref_idx  //!< reference picture for list0 prediction
                           )
{
  int    block_y, block_x, pic_pix_y, pic_pix_x, i, j, nonzero = 0, cbp_blk_mask;
  int    coeff_cost = 0;
  int    mb_y       = (block8x8 >> 1) << 3;
  int    mb_x       = (block8x8 & 0x01) << 3;
  int    pix_y;
  int    cbp_mask   = 1 << block8x8;
  int    bxx, byy;                   // indexing curr_blk
  int    skipped    = (l0_mode == 0 && l1_mode == 0 && (img->type != B_SLICE));
  Macroblock* currMB = &img->mb_data[img->current_mb_nr];
  //set transform size
  int    need_8x8_transform = currMB->luma_transform_size_8x8_flag;

  if ( input->ChromaMCBuffer )
    OneComponentChromaPrediction4x4 = OneComponentChromaPrediction4x4_retrieve;
  else
    OneComponentChromaPrediction4x4 = OneComponentChromaPrediction4x4_regenerate;

  //===== loop over 4x4 blocks =====
  if(!need_8x8_transform)
  {
    for (byy=0, block_y=mb_y; block_y<mb_y+8; byy+=4, block_y+=4)
    {
      pic_pix_y = img->opix_y + block_y;
    
      for (bxx=0, block_x=mb_x; block_x<mb_x+8; bxx+=4, block_x+=4)
      {
        pic_pix_x = img->opix_x + block_x;

        cbp_blk_mask = (block_x>>2) + block_y;

        //===== prediction of 4x4 block =====
        LumaPrediction4x4 (block_x, block_y, p_dir, l0_mode, l1_mode, l0_ref_idx, l1_ref_idx);

        //===== get displaced frame difference ======
        for (j=0; j<4; j++)
        {
          pix_y = pic_pix_y + j;
          for (i=0; i<4; i++)
          {
            img->m7[j][i] = imgY_org[pix_y][pic_pix_x + i] - img->mpr[j+block_y][i+block_x];
          }
        }

        //===== DCT, Quantization, inverse Quantization, IDCT, Reconstruction =====
        if ( (img->NoResidueDirect != 1 && !skipped  ) ||
          ((img->qp_scaled)==0 && img->lossless_qpprime_flag==1) )
        {
          //===== DCT, Quantization, inverse Quantization, IDCT, Reconstruction =====
          if (img->type!=SP_SLICE)
            nonzero = dct_luma   (block_x, block_y, &coeff_cost, 0);
          else if(!si_frame_indicator && !sp2_frame_indicator)
            nonzero = dct_luma_sp(block_x, block_y, &coeff_cost);// SP frame encoding
          else
            nonzero = dct_luma_sp2(block_x, block_y, &coeff_cost);//switching SP/SI encoding

          if (nonzero)
          {
            (*cbp_blk) |= (int64)1 << cbp_blk_mask;  // one bit for every 4x4 block
            (*cbp)     |= cbp_mask;           // one bit for the 4x4 blocks of an 8x8 block
          }
        }
      }
    }
  }
  else
  {
    for (byy=0, block_y=mb_y; block_y<mb_y+8; byy+=4, block_y+=4)
    {
      pic_pix_y = img->opix_y + block_y;

      for (bxx=0, block_x=mb_x; block_x<mb_x+8; bxx+=4, block_x+=4)
      {
        pic_pix_x = img->opix_x + block_x;

        cbp_blk_mask = (block_x>>2) + block_y;

        //===== prediction of 4x4 block =====
        LumaPrediction4x4 (block_x, block_y, p_dir, l0_mode, l1_mode, l0_ref_idx, l1_ref_idx);

        //===== get displaced frame difference ======
        for (j=0; j<4; j++)
        {
          pix_y = pic_pix_y + j;
          for (i=0; i<4; i++)
          {
            img->m7[j+byy][i+bxx] = imgY_org[pix_y][pic_pix_x+i] - img->mpr[j+block_y][i+block_x];
          }
        }
      }
    }
    if (img->NoResidueDirect != 1 && !skipped)
    {
      if (img->type!=SP_SLICE)
        nonzero = dct_luma8x8   (block8x8, &coeff_cost, 0);

      if (nonzero)
      {
        (*cbp_blk) |= 51 << (4*block8x8-2*(block8x8 & 0x01)); // corresponds to 110011, as if all four 4x4 blocks contain coeff, shifted to block position
        (*cbp)     |= cbp_mask;           // one bit for the 4x4 blocks of an 8x8 block
      }
    }
  }

  /*
  The purpose of the action below is to prevent that single or 'expensive' coefficients are coded.
  With 4x4 transform there is larger chance that a single coefficient in a 8x8 or 16x16 block may be nonzero.
  A single small (level=1) coefficient in a 8x8 block will cost: 3 or more bits for the coefficient,
  4 bits for EOBs for the 4x4 blocks,possibly also more bits for CBP.  Hence the total 'cost' of that single
  coefficient will typically be 10-12 bits which in a RD consideration is too much to justify the distortion improvement.
  The action below is to watch such 'single' coefficients and set the reconstructed block equal to the prediction according
  to a given criterium.  The action is taken only for inter luma blocks.

  Notice that this is a pure encoder issue and hence does not have any implication on the standard.
  coeff_cost is a parameter set in dct_luma() and accumulated for each 8x8 block.  If level=1 for a coefficient,
  coeff_cost is increased by a number depending on RUN for that coefficient.The numbers are (see also dct_luma()): 3,2,2,1,1,1,0,0,...
  when RUN equals 0,1,2,3,4,5,6, etc.
  If level >1 coeff_cost is increased by 9 (or any number above 3). The threshold is set to 3. This means for example:
  1: If there is one coefficient with (RUN,level)=(0,1) in a 8x8 block this coefficient is discarded.
  2: If there are two coefficients with (RUN,level)=(1,1) and (4,1) the coefficients are also discarded
  sum_cnt_nonz is the accumulation of coeff_cost over a whole macro block.  If sum_cnt_nonz is 5 or less for the whole MB,
  all nonzero coefficients are discarded for the MB and the reconstructed block is set equal to the prediction.
  */

  if (img->NoResidueDirect != 1 && !skipped && coeff_cost <= _LUMA_COEFF_COST_ &&
    ((img->qp_scaled)!=0 || img->lossless_qpprime_flag==0)&&
    !(img->type==SP_SLICE && (si_frame_indicator==1 || sp2_frame_indicator==1 )))// last set of conditions
    // cannot skip when perfect reconstruction is as in switching pictures or SI pictures
  {
    coeff_cost  = 0;
    (*cbp)     &=  (63 - cbp_mask);
    (*cbp_blk) &= ~(51 << (4*block8x8-2*(block8x8 & 0x01)));

    for (j=mb_y; j<mb_y+8; j++)
      memcpy(&enc_picture->imgY[img->pix_y + j][img->pix_x + mb_x], &img->mpr[j][mb_x], 2 * BLOCK_SIZE * sizeof(imgpel));

    if (img->type==SP_SLICE)
    {
      for (i=mb_x; i < mb_x+BLOCK_SIZE*2; i+=BLOCK_SIZE)
        for (j=mb_y; j < mb_y+BLOCK_SIZE*2; j+=BLOCK_SIZE)
          copyblock_sp(i,j);
    }
  }

  return coeff_cost;
}


/*!
 ************************************************************************
 * \brief
 *    Set mode parameters and reference frames for an 8x8 block
 ************************************************************************
 */
void SetModesAndRefframe (int b8, short* p_dir, int* l0_mode, int* l1_mode, short* fw_ref, short* bw_ref)
{
  Macroblock* currMB = &img->mb_data[img->current_mb_nr];
  int         j      = 2*(b8>>1);
  int         i      = 2*(b8 & 0x01);

  *l0_mode = *l1_mode = *fw_ref = *bw_ref = -1;

  *p_dir  = currMB->b8pdir[b8];

  if (img->type!=B_SLICE)
  {
    *fw_ref = enc_picture->ref_idx[LIST_0][img->block_y+j][img->block_x+i];
    *bw_ref = 0;
    *l0_mode  = currMB->b8mode[b8];
    *l1_mode  = 0;
  }
  else
  {
    if (currMB->b8pdir[b8]==-1)
    {
      *fw_ref   = -1;
      *bw_ref   = -1;
      *l0_mode  =  0;
      *l1_mode  =  0;
    }
    else if (currMB->b8pdir[b8]==0)
    {
      *fw_ref   = enc_picture->ref_idx[LIST_0][img->block_y+j][img->block_x+i];
      *bw_ref   = 0;
      *l0_mode  = currMB->b8mode[b8];
      *l1_mode  = 0;
    }
    else if (currMB->b8pdir[b8]==1)
    {
      *fw_ref   = 0;
      *bw_ref   = enc_picture->ref_idx[LIST_1][img->block_y+j][img->block_x+i];
      *l0_mode  = 0;
      *l1_mode  = currMB->b8mode[b8];
    }
    else
    {
      *fw_ref   = enc_picture->ref_idx[LIST_0][img->block_y+j][img->block_x+i];
      *bw_ref   = enc_picture->ref_idx[LIST_1][img->block_y+j][img->block_x+i];
      *l0_mode  = currMB->b8mode[b8];
      *l1_mode  = currMB->b8mode[b8];
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Residual Coding of a Luma macroblock (not for intra)
 ************************************************************************
 */
void LumaResidualCoding (void)
{
  int i,j,block8x8,b8_x,b8_y;
  int l0_mode, l1_mode;
  short p_dir, refframe;
  int sum_cnt_nonz;
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];

  currMB->cbp     = 0 ;
  currMB->cbp_blk = 0 ;
  sum_cnt_nonz    = 0 ;

  for (block8x8=0; block8x8<4; block8x8++)
  {
    short bw_ref;
    SetModesAndRefframe (block8x8, &p_dir, &l0_mode, &l1_mode, &refframe, &bw_ref);
    
    sum_cnt_nonz += LumaResidualCoding8x8 (&(currMB->cbp), &(currMB->cbp_blk), block8x8,
                                           p_dir, l0_mode, l1_mode, refframe, bw_ref);
  }

  if (sum_cnt_nonz <= _LUMA_MB_COEFF_COST_ &&
    ((img->qp_scaled)!=0 || img->lossless_qpprime_flag==0) &&
    !(img->type==SP_SLICE && (si_frame_indicator==1 || sp2_frame_indicator==1)))// modif ES added last set of conditions
    //cannot skip if SI or switching SP frame perfect reconstruction is needed
  {
     currMB->cbp     &= 0xfffff0 ;
     currMB->cbp_blk &= 0xff0000 ;
     for (j=0; j < MB_BLOCK_SIZE; j++)
       memcpy(&enc_picture->imgY[img->pix_y+j][img->pix_x], img->mpr[j], MB_BLOCK_SIZE * sizeof (imgpel));

     if (img->type==SP_SLICE)
     {
       for(block8x8=0;block8x8<4;block8x8++)
       {
         b8_x=(block8x8&1)<<3;
         b8_y=(block8x8&2)<<2;
         for (i=b8_x;i<b8_x+8;i+=4)
           for (j=b8_y;j<b8_y+8;j+=4)
             copyblock_sp(i,j);
       }
     }
   }
}


/*!
 ************************************************************************
 * \brief
 *    Makes the decision if 8x8 tranform will be used (for RD-off)
 ************************************************************************
 */
int TransformDecision (int block_check, int *cost)
{
  int    block_y, block_x, pic_pix_y, pic_pix_x, i, j, k;
  int    mb_y, mb_x, block8x8;
  int    l0_mode, l1_mode;
  short  p_dir, fw_ref, bw_ref;
  int    num_blks;
  int    cost8x8=0, cost4x4=0;
  int    *diff_ptr;

  if(block_check==-1)
  {
    block8x8=0;
    num_blks=4;
  }
  else
  {
    block8x8=block_check;
    num_blks=block_check+1;
  }

  for (; block8x8<num_blks; block8x8++)
  {
    SetModesAndRefframe (block8x8, &p_dir, &l0_mode, &l1_mode, &fw_ref, &bw_ref);

    mb_y = (block8x8 >> 1) << 3;
    mb_x = (block8x8 & 0x01) << 3;
    //===== loop over 4x4 blocks =====
    k=0;
    for (block_y=mb_y; block_y<mb_y+8; block_y+=4)
    {
      pic_pix_y = img->opix_y + block_y;
      
      for (block_x=mb_x; block_x<mb_x+8; block_x+=4)
      {
        pic_pix_x = img->opix_x + block_x;

        //===== prediction of 4x4 block =====
        LumaPrediction4x4 (block_x, block_y, p_dir, l0_mode, l1_mode, fw_ref, bw_ref);

        //===== get displaced frame difference ======
        diff_ptr=&diff64[k];
        for (j=0; j<4; j++)
        {
          for (i=0; i<4; i++, k++)
            diff64[k] = imgY_org[pic_pix_y+j][pic_pix_x+i] - img->mpr[j+block_y][i+block_x];
        }
        cost4x4 += distortion4x4 (diff_ptr);
      }
    }
    cost8x8 += distortion8x8 (diff64);
  }

  if(input->Transform8x8Mode==2) //always allow 8x8 transform
    return 1;
  else if(cost8x8<cost4x4)
    return 1;
  else
  {
    *cost = (*cost-cost8x8+cost4x4);
    return 0;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Predict (on-the-fly) one component of a chroma 4x4 block
 ************************************************************************
 */
void OneComponentChromaPrediction4x4_regenerate (
                                 imgpel*     mpred,      //!< array to store prediction values
                                 int         block_c_x,  //!< horizontal pixel coordinate of 4x4 block
                                 int         block_c_y,  //!< vertical   pixel coordinate of 4x4 block
                                 short****** mv,         //!< motion vector array
                                 int         list_idx,   //!< reference picture list
                                 short       ref,        //!< reference index
                                 int         blocktype,  //!< block type
                                 int         uv)         //!< chroma component
{
  int     i, j, ii, jj, ii0, jj0, ii1, jj1, if0, if1, jf0, jf1;
  short*  mvb;

  int     f1_x = 64/img->mb_cr_size_x;
  int     f2_x=f1_x-1;

  int     f1_y = 64/img->mb_cr_size_y;
  int     f2_y=f1_y-1;

  int     f3=f1_x*f1_y, f4=f3>>1;
  int     list_offset = img->mb_data[img->current_mb_nr].list_offset;
  int     max_y_cr = (int) (list_offset ? (img->height_cr >> 1) - 1 : img->height_cr - 1);
  int     max_x_cr = (int) (img->width_cr - 1);
  int     jjx, iix;
  int     mb_cr_y_div4 = img->mb_cr_size_y>>2;
  int     mb_cr_x_div4 = img->mb_cr_size_x>>2;
  int     jpos;

  StorablePicture **list = listX[list_idx + list_offset];

  imgpel** refimage = list[ref]->imgUV[uv];

  for (j=block_c_y; j < block_c_y + BLOCK_SIZE; j++)
  {
    jjx = j/mb_cr_y_div4;
    jpos = (j + img->opix_c_y)*f1_y;

    for (i=block_c_x; i < block_c_x + BLOCK_SIZE; i++)
    {
      iix = i/mb_cr_x_div4;
      mvb  = mv [jjx][iix][list_idx][ref][blocktype];

      ii   = (i + img->opix_c_x)*f1_x + mvb[0];
      jj   = jpos + mvb[1];

      if (active_sps->chroma_format_idc == 1)
        jj  += list[ref]->chroma_vector_adjustment;

      ii0  = iClip3 (0, max_x_cr, ii/f1_x);
      jj0  = iClip3 (0, max_y_cr, jj/f1_y);
      ii1  = iClip3 (0, max_x_cr, (ii+f2_x)/f1_x);
      jj1  = iClip3 (0, max_y_cr, (jj+f2_y)/f1_y);

      if1  = (ii&f2_x);  if0 = f1_x-if1;
      jf1  = (jj&f2_y);  jf0 = f1_y-jf1;

      *mpred++ = (if0 * jf0 * refimage[jj0][ii0] +
                  if1 * jf0 * refimage[jj0][ii1] +
                  if0 * jf1 * refimage[jj1][ii0] +
                  if1 * jf1 * refimage[jj1][ii1] + f4) / f3;
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Retrieve one component of a chroma 4x4 block from the buffer
 ************************************************************************
 */
void OneComponentChromaPrediction4x4_retrieve (imgpel*        mpred,      //!< array to store prediction values
                                 int         block_c_x,  //!< horizontal pixel coordinate of 4x4 block
                                 int         block_c_y,  //!< vertical   pixel coordinate of 4x4 block
                                 short****** mv,         //!< motion vector array
                                 int         list_idx,   //!< reference picture list
                                 short       ref,        //!< reference index
                                 int         blocktype,  //!< block type
                                 int         uv)         //!< chroma component
{
  int     j, ii, jj;
  short*  mvb;

  int     list_offset = img->mb_data[img->current_mb_nr].list_offset;

  int     jjx;
  int     right_shift_x = 4 - chroma_shift_x;
  int     right_shift_y = 4 - chroma_shift_y;
  int     jpos;

  int     pos_x1 = block_c_x >> right_shift_x;
  int     pos_x2 = (block_c_x + 2) >> right_shift_x;
  int     ipos1 = ((block_c_x + img->opix_c_x) << chroma_shift_x) + IMG_PAD_SIZE_TIMES4;
  int     ipos2 = ((block_c_x + 2 + img->opix_c_x) << chroma_shift_x) + IMG_PAD_SIZE_TIMES4;


  StorablePicture **list = listX[list_idx + list_offset];

  imgpel**** refsubimage = list[ref]->imgUV_sub[uv];
  imgpel *line_ptr;

  int jj_chroma = ((active_sps->chroma_format_idc == 1) ? list[ref]->chroma_vector_adjustment : 0) + IMG_PAD_SIZE_TIMES4;

  width_pad_cr  = list[ref]->size_x_cr_pad;
  height_pad_cr = list[ref]->size_y_cr_pad;


  for (j=block_c_y; j < block_c_y + BLOCK_SIZE; j++)
  {
    jjx = j >> right_shift_y; // translate into absolute block (luma) coordinates

    jpos = ( (j + img->opix_c_y) << chroma_shift_y ) + jj_chroma;

    mvb  = mv [jjx][pos_x1][list_idx][ref][blocktype];

    ii   = ipos1 + mvb[0];
    jj   = jpos  + mvb[1];

    line_ptr = UMVLine8X_chroma ( refsubimage, jj, ii);
    *mpred++ = *line_ptr++;
    *mpred++ = *line_ptr;

    mvb  = mv [jjx][pos_x2][list_idx][ref][blocktype];

    ii   = ipos2 + mvb[0];
    jj   = jpos  + mvb[1];

    line_ptr = UMVLine8X_chroma ( refsubimage, jj, ii);
    *mpred++ = *line_ptr++;
    *mpred++ = *line_ptr;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Predict an intra chroma 4x4 block
 ************************************************************************
 */
void IntraChromaPrediction4x4 (int  uv,       // <-- colour component
                               int  block_x,  // <-- relative horizontal block coordinate of 4x4 block
                               int  block_y)  // <-- relative vertical   block coordinate of 4x4 block
{
  int mode = img->mb_data[img->current_mb_nr].c_ipred_mode;
  int j;

  //===== prediction =====
  for (j=block_y; j<block_y+4; j++)
    memcpy(&img->mpr[j][block_x],&img->mprr_c[uv][mode][j][block_x], BLOCK_MULTIPLE * sizeof(imgpel));
}



/*!
 ************************************************************************
 * \brief
 *    Predict one chroma 4x4 block
 ************************************************************************
 */
void ChromaPrediction4x4 ( int   uv,           // <-- colour component
                           int   block_x,      // <-- relative horizontal block coordinate of 4x4 block
                           int   block_y,      // <-- relative vertical   block coordinate of 4x4 block
                           int   p_dir,        // <-- prediction direction
                           int   l0_mode,      // <-- list0  prediction mode (1-7, 0=DIRECT if l1_mode=0)
                           int   l1_mode,      // <-- list1 prediction mode (1-7, 0=DIRECT if l0_mode=0)
                           short l0_ref_idx,   // <-- reference frame for list0 prediction (if (<0) -> intra prediction)
                           short l1_ref_idx)   // <-- reference frame for list1 prediction 
{
  static imgpel l0_pred[MB_BLOCK_SIZE];
  static imgpel l1_pred[MB_BLOCK_SIZE];

  int  i, j;
  int  block_x4  = block_x+4;
  int  block_y4  = block_y+4;
  imgpel* l0pred     = l0_pred;
  imgpel* l1pred     = l1_pred;
  short****** mv_array = img->all_mv;

  Macroblock*    currMB     = &img->mb_data[img->current_mb_nr];

  int  apply_weights = ( (active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
                         (active_pps->weighted_bipred_idc && (img->type == B_SLICE)));

  if (currMB->bi_pred_me && l0_ref_idx == 0 && l1_ref_idx == 0 && p_dir == 2 && l0_mode==1 && l1_mode==1)
    mv_array = currMB->bi_pred_me == 1? img->bipred_mv1 : img->bipred_mv2 ;

  //===== INTRA PREDICTION =====
  if (p_dir==-1)
  {
    IntraChromaPrediction4x4 (uv, block_x, block_y);
    return;
  }

  //===== INTER PREDICTION =====
  if ((p_dir==0) || (p_dir==2))
  {
    (*OneComponentChromaPrediction4x4) (l0_pred, block_x, block_y, mv_array, LIST_0, l0_ref_idx, l0_mode, uv);
  }
  if ((p_dir==1) || (p_dir==2))
  {
    (*OneComponentChromaPrediction4x4) (l1_pred, block_x, block_y, mv_array, LIST_1, l1_ref_idx, l1_mode, uv);
  }

  if (apply_weights)
  {
    if (p_dir==2)
    {
      int wbp0 = wbp_weight[0][l0_ref_idx][l1_ref_idx][uv+1];
      int wbp1 = wbp_weight[1][l0_ref_idx][l1_ref_idx][uv+1];
      int offset = (wp_offset[0][l0_ref_idx][uv+1] + wp_offset[1][l1_ref_idx][uv+1] + 1)>>1;
      int wp_round = 2*wp_chroma_round;
      int weight_denom = luma_log_weight_denom + 1;


      for (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)
            img->mpr[j][i] =  iClip1( img->max_imgpel_value_uv,
            ((wbp0 * *l0pred++ + wbp1 * *l1pred++ + wp_round) >> (weight_denom)) + (offset) );
    }
    else if (p_dir==0)
    {
      int wp = wp_weight[0][l0_ref_idx][uv+1];
      int offset = wp_offset[0][l0_ref_idx][uv+1];
      for (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)
           img->mpr[j][i] = iClip1( img->max_imgpel_value_uv, (( wp * *l0pred++ + wp_chroma_round) >> chroma_log_weight_denom) +  offset);
    }
    else // (p_dir==1)
    {
      int wp = wp_weight[1][l1_ref_idx][uv+1];
      int offset = wp_offset[1][l1_ref_idx][uv+1];

      for (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)
          img->mpr[j][i] = iClip1( img->max_imgpel_value_uv, ((wp * *l1pred++ + wp_chroma_round) >> chroma_log_weight_denom) + offset);
    }
  }
  else
  {
    if (p_dir==2)
    {
      for (j=block_y; j<block_y4; j++)
        for (i=block_x; i<block_x4; i++)
          img->mpr[j][i] = (*l0pred++ + *l1pred++ + 1) >> 1;
    }
    else if (p_dir==0)
    {
      for (j=block_y; j<block_y4; j++)
      {
        memcpy(&(img->mpr[j][block_x]), l0pred, BLOCK_SIZE * sizeof(imgpel));
        l0pred += BLOCK_SIZE;
      }
    }
    else // (p_dir==1)
    {
      for (j=block_y; j<block_y4; j++)
      {
        memcpy(&(img->mpr[j][block_x]), l1pred, BLOCK_SIZE * sizeof(imgpel));
        l1pred += BLOCK_SIZE;
      }
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Chroma residual coding for an macroblock
 ************************************************************************
 */
void ChromaResidualCoding (int* cr_cbp)
{
  static const int block8x8_idx[3][4][4] =     //ADD-VG-15052004
  {
    { {0, 1, 0, 0},
      {2, 3, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
    },
    { {0, 1, 0, 0},
      {0, 1, 0, 0},
      {2, 3, 0, 0},
      {2, 3, 0, 0},
    },
    { {0, 0, 1, 1},
      {0, 0, 1, 1},
      {2, 2, 3, 3},
      {2, 2, 3, 3}
    }
  };
  int   uv, block8, block_y, block_x, j, i;
  int   l0_mode, l1_mode;
  short p_dir, refframe, bw_ref;
  int   skipped = (img->mb_data[img->current_mb_nr].mb_type == 0 && (img->type == P_SLICE || img->type == SP_SLICE));
  int   yuv = img->yuv_format - 1; //ADD-VG-15052004

  if ( input->ChromaMCBuffer )
    OneComponentChromaPrediction4x4 = OneComponentChromaPrediction4x4_retrieve;
  else
    OneComponentChromaPrediction4x4 = OneComponentChromaPrediction4x4_regenerate;

  for (*cr_cbp=0, uv=0; uv<2; uv++)
  {
    //===== prediction of chrominance blocks ===d==
    block8 = 0;
    for (block_y=0; block_y < img->mb_cr_size_y; block_y+=4)
    for (block_x=0; block_x < img->mb_cr_size_x; block_x+=4)
    {
      block8 = block8x8_idx[yuv][block_y>>2][block_x>>2];
      SetModesAndRefframe (block8, &p_dir, &l0_mode, &l1_mode, &refframe, &bw_ref);

      ChromaPrediction4x4 (uv, block_x, block_y, p_dir, l0_mode, l1_mode, refframe, bw_ref);
    }

    // ==== set chroma residue to zero for skip Mode in SP frames
    if (img->NoResidueDirect)
    {
      for (j=0; j<img->mb_cr_size_y; j++)
        memcpy(&enc_picture->imgUV[uv][img->pix_c_y+j][img->pix_c_x], img->mpr[j], img->mb_cr_size_x * sizeof(imgpel));
    }
    else if (skipped && img->type==SP_SLICE)
    {
      for (j=0; j<8; j++)
        memset(img->m7[j], 0 , 8 * sizeof(int));
    }
    else
    if (skipped)
    {
      for (j=0; j<img->mb_cr_size_y; j++)
        memcpy(&enc_picture->imgUV[uv][img->pix_c_y+j][img->pix_c_x], img->mpr[j], img->mb_cr_size_x * sizeof(imgpel));
    }
    else
    {
      for (j=0; j<img->mb_cr_size_y; j++)
        for (i=0; i<img->mb_cr_size_x; i++)
        {
          img->m7[j][i] = imgUV_org[uv][img->opix_c_y+j][img->opix_c_x+i] - img->mpr[j][i];
        }
    }

    //===== DCT, Quantization, inverse Quantization, IDCT, and Reconstruction =====
    //===== Call function for skip mode in SP frames to properly process frame ====

    if (skipped && img->type==SP_SLICE)
    {
      if(si_frame_indicator || sp2_frame_indicator)
        *cr_cbp=dct_chroma_sp2(uv,*cr_cbp);//modif ES added, calls the SI/switching SP encoding function
      else
        *cr_cbp=dct_chroma_sp(uv,*cr_cbp);
    }
    else
    {
      if (!img->NoResidueDirect && !skipped)
      {
        if (img->type!=SP_SLICE || (img->mb_data[img->current_mb_nr].mb_type==I16MB ))
        {
          //even if the block is intra it should still be treated as SP
          *cr_cbp=dct_chroma   (uv,*cr_cbp);
        }
        else
        {
          if(si_frame_indicator||sp2_frame_indicator)
            *cr_cbp=dct_chroma_sp2(uv,*cr_cbp);// SI frames or switching SP frames
          else
            *cr_cbp=dct_chroma_sp(uv,*cr_cbp);
        }
      }
    }
  }

  //===== update currMB->cbp =====
  img->mb_data[img->current_mb_nr].cbp += ((*cr_cbp)<<4);

}


/*!
 ************************************************************************
 * \brief
 *    Intra prediction of the chrminance layers of one macroblock
 ************************************************************************
 */
void IntraChromaPrediction (int *mb_up, int *mb_left, int*mb_up_left)
{

  Macroblock *currMB = &img->mb_data[img->current_mb_nr];
  int      s, s0, s1, s2, s3, i, j, k;
  imgpel**  image;
  int      block_x, block_y;
  int      mb_nr = img->current_mb_nr;
  int      mb_available_up;
  int      mb_available_left[2];
  int      mb_available_up_left;
  int      ih,iv;
  int      ib,ic,iaa;
  int      uv;
  imgpel   hline[16], vline[16];
  int      mode;
  int      best_mode = DC_PRED_8;  //just an initilaization here, should always be overwritten
  int      cost;
  int      min_cost;
  PixelPos up;        //!< pixel position  p(0,-1)
  PixelPos left[17];  //!< pixel positions p(-1, -1..15)
  int      cr_MB_x = img->mb_cr_size_x;
  int      cr_MB_y = img->mb_cr_size_y;

  int      blk_x;
  int      blk_y;
  int      b8,b4;
  int      yuv = img->yuv_format - 1;

  static const int block_pos[3][4][4]= //[yuv][b8][b4]
  {
    { {0, 1, 2, 3},{0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0}},
    { {0, 1, 2, 3},{2, 3, 2, 3},{0, 0, 0, 0},{0, 0, 0, 0}},
    { {0, 1, 2, 3},{1, 1, 3, 3},{2, 3, 2, 3},{3, 3, 3, 3}}
  };

  for (i=0;i<cr_MB_y+1;i++)
  {
    getNeighbour(mb_nr, -1 , i-1 , IS_CHROMA, &left[i]);
  }
  getNeighbour(mb_nr, 0 , -1 , IS_CHROMA, &up);


  mb_available_up                             = up.available;
  mb_available_up_left                        = left[0].available;
  mb_available_left[0] = mb_available_left[1] = left[1].available;

  if(input->UseConstrainedIntraPred)
  {
    mb_available_up = up.available ? img->intra_block[up.mb_addr] : 0;
    for (i=0, mb_available_left[0]=1; i<(cr_MB_y>>1);i++)
      mb_available_left[0]  &= left[i+1].available ? img->intra_block[left[i+1].mb_addr]: 0;
    for (i=(cr_MB_y>>1), mb_available_left[1]=1; i<cr_MB_y;i++)
      mb_available_left[1] &= left[i+1].available ? img->intra_block[left[i+1].mb_addr]: 0;
    mb_available_up_left = left[0].available ? img->intra_block[left[0].mb_addr]: 0;
  }

  if (mb_up)
    *mb_up = mb_available_up;
  if (mb_left)
    *mb_left = mb_available_left[0] && mb_available_left[1];
  if (mb_up_left)
    *mb_up_left = mb_available_up_left;


  // compute all chroma intra prediction modes for both U and V
  for (uv=0; uv<2; uv++)
  {
    image = enc_picture->imgUV[uv];

    // DC prediction
    for(b8=0; b8<img->num_blk8x8_uv >> 1;b8++)
    {
      for (b4=0; b4<4; b4++)
      {
        block_y = subblk_offset_y[yuv][b8][b4];
        block_x = subblk_offset_x[yuv][b8][b4];
        blk_x = block_x;
        blk_y = block_y + 1;

        s=img->dc_pred_value_chroma;
        s0=s1=s2=s3=0;

        //===== get prediction value =====
        switch (block_pos[yuv][b8][b4])
        {
        case 0:  //===== TOP LEFT =====
          if      (mb_available_up)       for (i=blk_x;i<(blk_x+4);i++)  s0 += image[up.pos_y][up.pos_x + i];
          if      (mb_available_left[0])  for (i=blk_y;i<(blk_y+4);i++)  s2 += image[left[i].pos_y][left[i].pos_x];
          if      (mb_available_up && mb_available_left[0])  s  = (s0+s2+4) >> 3;
          else if (mb_available_up)                          s  = (s0   +2) >> 2;
          else if (mb_available_left[0])                     s  = (s2   +2) >> 2;
          break;
        case 1: //===== TOP RIGHT =====
          if      (mb_available_up)       for (i=blk_x;i<(blk_x+4);i++)  s1 += image[up.pos_y][up.pos_x + i];
          else if (mb_available_left[0])  for (i=blk_y;i<(blk_y+4);i++)  s2 += image[left[i].pos_y][left[i].pos_x];
          if      (mb_available_up)                          s  = (s1   +2) >> 2;
          else if (mb_available_left[0])                     s  = (s2   +2) >> 2;
          break;
        case 2: //===== BOTTOM LEFT =====
          if      (mb_available_left[1])  for (i=blk_y;i<(blk_y+4);i++)  s3 += image[left[i].pos_y][left[i].pos_x];
          else if (mb_available_up)       for (i=blk_x;i<(blk_x+4);i++)  s0 += image[up.pos_y][up.pos_x + i];
          if      (mb_available_left[1])                     s  = (s3   +2) >> 2;
          else if (mb_available_up)                          s  = (s0   +2) >> 2;
          break;
        case 3: //===== BOTTOM RIGHT =====
          if      (mb_available_up)       for (i=blk_x;i<(blk_x+4);i++)  s1 += image[up.pos_y][up.pos_x + i];
          if      (mb_available_left[1])  for (i=blk_y;i<(blk_y+4);i++)  s3 += image[left[i].pos_y][left[i].pos_x];
          if      (mb_available_up && mb_available_left[1])  s  = (s1+s3+4) >> 3;
          else if (mb_available_up)                          s  = (s1   +2) >> 2;
          else if (mb_available_left[1])                     s  = (s3   +2) >> 2;
          break;
        }

        //===== prediction =====
        for (j=block_y; j<block_y+4; j++)
          for (i=block_x; i<block_x+4; i++)
          {
            img->mprr_c[uv][DC_PRED_8][j][i] = s;
          }
      }
    }

    // vertical prediction
    if (mb_available_up)
    {
      memcpy(hline,&image[up.pos_y][up.pos_x], cr_MB_x * sizeof(imgpel));
      for (j=0; j<cr_MB_y; j++)
        memcpy(img->mprr_c[uv][VERT_PRED_8][j], hline, cr_MB_x * sizeof(imgpel));
    }

    // horizontal prediction
    if (mb_available_left[0] && mb_available_left[1])
    {
      for (i=0; i<cr_MB_y; i++)
        vline[i] = image[left[i+1].pos_y][left[i+1].pos_x];
      for (i=0; i<cr_MB_x; i++)
        for (j=0; j<cr_MB_y; j++)
          img->mprr_c[uv][HOR_PRED_8][j][i] = vline[j];
    }

    // plane prediction
    if (mb_available_left[0] && mb_available_left[1] && mb_available_up && mb_available_up_left)
    {
      ih = (cr_MB_x>>1)*(hline[cr_MB_x-1] - image[left[0].pos_y][left[0].pos_x]);
      for (i=0;i<(cr_MB_x>>1)-1;i++)
        ih += (i+1)*(hline[(cr_MB_x>>1)+i] - hline[(cr_MB_x>>1)-2-i]);

      iv = (cr_MB_y>>1)*(vline[cr_MB_y-1] - image[left[0].pos_y][left[0].pos_x]);
      for (i=0;i<(cr_MB_y>>1)-1;i++)
        iv += (i+1)*(vline[(cr_MB_y>>1)+i] - vline[(cr_MB_y>>1)-2-i]);

      ib= ((cr_MB_x == 8?17:5)*ih+2*cr_MB_x)>>(cr_MB_x == 8?5:6);
      ic= ((cr_MB_y == 8?17:5)*iv+2*cr_MB_y)>>(cr_MB_y == 8?5:6);

      iaa=16*(hline[cr_MB_x-1]+vline[cr_MB_y-1]);
      for (j=0; j<cr_MB_y; j++)
        for (i=0; i<cr_MB_x; i++)
          img->mprr_c[uv][PLANE_8][j][i]= iClip3(0, img->max_imgpel_value_uv,
                                                   (iaa+(i-(cr_MB_x>>1)+1)*ib+(j-(cr_MB_y>>1)+1)*ic+16)>>5);
    }
  }

  if (!input->rdopt)      // the rd-opt part does not work correctly (see encode_one_macroblock)
  {                       // since ipredmodes could be overwritten => encoder-decoder-mismatches
    // pick lowest cost prediction mode
    min_cost = INT_MAX;
    for (i=0;i<cr_MB_y;i++)
    {
      getNeighbour(mb_nr, 0 , i, IS_CHROMA, &left[i]);
    }
    for (mode=DC_PRED_8; mode<=PLANE_8; mode++)
    {
      if ((img->type != I_SLICE || !input->IntraDisableInterOnly) && input->ChromaIntraDisable == 1 && mode!=DC_PRED_8)
        continue;

      if ((mode==VERT_PRED_8 && !mb_available_up) ||
        (mode==HOR_PRED_8 && (!mb_available_left[0] || !mb_available_left[1])) ||
        (mode==PLANE_8 && (!mb_available_left[0] || !mb_available_left[1] || !mb_available_up || !mb_available_up_left)))
        continue;

      cost = 0;
      for (uv=0; uv<2; uv++)
      {
        image = imgUV_org[uv];
        for (block_y=0; block_y<cr_MB_y; block_y+=4)
          for (block_x=0; block_x<cr_MB_x; block_x+=4)
          {
            for (k=0,j=block_y; j<block_y+4; j++)
            {
              for (i=block_x; i<block_x+4; i++,k++)
                diff[k] = image[left[j].pos_y][left[j].pos_x+i] - img->mprr_c[uv][mode][j][i];
            }
            cost += distortion4x4(diff);
          }
      }
      if (cost < min_cost)
      {
        best_mode = mode;
        min_cost = cost;
      }
    }
    currMB->c_ipred_mode = best_mode;
  }
}

/*!
 **************************************************************************************
 * \brief
 *    RD Decision for Intra prediction mode of the chrominance layers of one macroblock
 **************************************************************************************
 */
void IntraChromaRDDecision (RD_PARAMS enc_mb)
{
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];
  int      i, j, k;
  imgpel** image;
  int      block_x, block_y;
  int      mb_nr = img->current_mb_nr;
  int      mb_available_up;
  int      mb_available_left[2];
  int      mb_available_up_left;
  int      uv;
  int      mode;
  int      best_mode = DC_PRED_8;  //just an initialization here, should always be overwritten
  int      cost;
  int      min_cost;
  PixelPos up;        //!< pixel position  p(0,-1)
  PixelPos left[17];  //!< pixel positions p(-1, -1..15)
  int      cr_MB_x = img->mb_cr_size_x;
  int      cr_MB_y = img->mb_cr_size_y;

  for (i=0;i<cr_MB_y+1;i++)
  {
    getNeighbour(mb_nr, -1 , i-1 , IS_CHROMA, &left[i]);
  }
  getNeighbour(mb_nr, 0 , -1 , IS_CHROMA, &up);

  mb_available_up                             = up.available;
  mb_available_up_left                        = left[0].available;
  mb_available_left[0] = mb_available_left[1] = left[1].available;

  if(input->UseConstrainedIntraPred)
  {
    mb_available_up = up.available ? img->intra_block[up.mb_addr] : 0;
    for (i=0, mb_available_left[0]=1; i<(cr_MB_y>>1);i++)
      mb_available_left[0]  &= left[i+1].available ? img->intra_block[left[i+1].mb_addr]: 0;
    for (i=(cr_MB_y>>1), mb_available_left[1]=1; i<cr_MB_y;i++)
      mb_available_left[1] &= left[i+1].available ? img->intra_block[left[i+1].mb_addr]: 0;
    mb_available_up_left = left[0].available ? img->intra_block[left[0].mb_addr]: 0;
  }

  // pick lowest cost prediction mode
  min_cost = INT_MAX;
  for (i=0;i<cr_MB_y;i++)
  {
    getNeighbour(mb_nr, 0 , i, IS_CHROMA, &left[i]);
  }
  if ( img->MbaffFrameFlag && img->field_mode )
  {
    for (i=0;i<cr_MB_y;i++)
    {
      left[i].pos_y = left[i].pos_y >> 1;
    }
  }

  for (mode=DC_PRED_8; mode<=PLANE_8; mode++)
  {
    if ((mode==VERT_PRED_8 && !mb_available_up) ||
      (mode==HOR_PRED_8 && (!mb_available_left[0] || !mb_available_left[1])) ||
      (mode==PLANE_8 && (!mb_available_left[0] || !mb_available_left[1] || !mb_available_up || !mb_available_up_left)))
      continue;

    cost = 0;
    for (uv=0; uv<2; uv++)
    {
      image = imgUV_org[uv];
      for (block_y=0; block_y<cr_MB_y; block_y+=4)
      {
        for (block_x=0; block_x<cr_MB_x; block_x+=4)
        {
          for (k=0,j=block_y; j<block_y+4; j++)
          {
            for (i=block_x; i<block_x+4; i++,k++)
              diff[k] = image[left[j].pos_y][left[j].pos_x+i] - img->mprr_c[uv][mode][j][i];
          }
          cost += distortion4x4(diff);
        }
        if (cost > min_cost) break;
      }
      if (cost > min_cost) break;
    }

    cost += (int) (enc_mb.lambda_me[Q_PEL] * mvbits[ mode ]); // exp golomb coding cost for mode signaling

    if (cost < min_cost)
    {
      best_mode = mode;
      min_cost = cost;
    }
  }
  currMB->c_ipred_mode = best_mode;
}


/*!
 ************************************************************************
 * \brief
 *    Check if all reference frames for a macroblock are zero
 ************************************************************************
 */
int ZeroRef (Macroblock* currMB)
{
  int i,j;

  for (j=img->block_y; j<img->block_y + BLOCK_MULTIPLE; j++)
  for (i=img->block_x; i<img->block_x + BLOCK_MULTIPLE; i++)
  {
    if (enc_picture->ref_idx[LIST_0][j][i]!=0)
        return 0;
  }
  return 1;
}


/*!
 ************************************************************************
 * \brief
 *    Converts macroblock type to coding value
 ************************************************************************
 */
int MBType2Value (Macroblock* currMB)
{
  static const int dir1offset[3]    =  { 1,  2, 3};
  static const int dir2offset[3][3] = {{ 0,  4,  8},   // 1. block forward
                                       { 6,  2, 10},   // 1. block backward
                                       {12, 14, 16}};  // 1. block bi-directional

  int mbtype, pdir0, pdir1;

  if (img->type!=B_SLICE)
  {
    if      (currMB->mb_type==I8MB ||currMB->mb_type==I4MB)
      return (img->type==I_SLICE ? 0 : 6);
    else if (currMB->mb_type==I16MB) return (img->type==I_SLICE ? 0 : 6) + img->i16offset;
    else if (currMB->mb_type==IPCM) return (img->type==I_SLICE ? 25 : 31);
    else if (currMB->mb_type==P8x8)
    {
      if (input->symbol_mode==UVLC
        && ZeroRef (currMB))         return 5;
      else                           return 4;
    }
    else                             return currMB->mb_type;
  }
  else
  {
    mbtype = currMB->mb_type;
    pdir0  = currMB->b8pdir[0];
    pdir1  = currMB->b8pdir[3];

    if      (mbtype==0)       return 0;
    else if (mbtype==I4MB || mbtype==I8MB)
      return 23;
    else if (mbtype==I16MB)   return 23 + img->i16offset;
    else if (mbtype==IPCM)    return 48;
    else if (mbtype==P8x8)    return 22;
    else if (mbtype==1)       return dir1offset[pdir0];
    else if (mbtype==2)       return 4 + dir2offset[pdir0][pdir1];
    else                      return 5 + dir2offset[pdir0][pdir1];
  }
}


/*!
************************************************************************
* \brief
*    Writes 4x4 intra prediction modes for a macroblock
************************************************************************
*/
int writeIntra4x4Modes(void)
{
  int i;
  Macroblock    *currMB     = &img->mb_data[img->current_mb_nr];
  SyntaxElement se;
  int           *bitCount   = currMB->bitcounter;
  Slice         *currSlice  = img->currentSlice;
  const int     *partMap    = assignSE2partition[input->partition_mode];
  DataPartition *dataPart   = &(currSlice->partArr[partMap[SE_INTRAPREDMODE]]);

  int rate = 0;

  currMB->IntraChromaPredModeFlag = 1;

  for(i=0;i<16;i++)
  {
    se.context = i;
    se.value1  = currMB->intra_pred_modes[i];
    se.value2  = 0;

#if TRACE
    if (se.value1 < 0 )
      snprintf(se.tracestring, TRACESTRING_SIZE, "Intra 4x4 mode  = predicted (context: %d)",se.context);
    else
      snprintf(se.tracestring, TRACESTRING_SIZE, "Intra 4x4 mode  = %3d (context: %d)",se.value1,se.context);
#endif

    // set symbol type and function pointers
    se.type = SE_INTRAPREDMODE;

    // encode and update rate
    writeIntraPredMode (&se, dataPart);

    bitCount[BITS_COEFF_Y_MB]+=se.len;
    rate += se.len;
  }

  return rate;
}

/*!
************************************************************************
* \brief
*    Writes 8x8 intra prediction modes for a macroblock
************************************************************************
*/
int writeIntra8x8Modes(void)
{
  int block8x8;
  Macroblock    *currMB     = &img->mb_data[img->current_mb_nr];
  SyntaxElement se;
  int           *bitCount   = currMB->bitcounter;
  Slice         *currSlice  = img->currentSlice;
  const int     *partMap    = assignSE2partition[input->partition_mode];
  DataPartition *dataPart   = &(currSlice->partArr[partMap[SE_INTRAPREDMODE]]);

  int rate = 0;

  currMB->IntraChromaPredModeFlag = 1;

  for(block8x8=0;block8x8<4;block8x8++)
  {

    se.context = block8x8<<2;
    se.value1  = currMB->intra_pred_modes8x8[(block8x8<<2)];
    se.value2  = 0;

#if TRACE
    if (se.value1 < 0 )
      snprintf(se.tracestring, TRACESTRING_SIZE, "Intra 8x8 mode  = predicted (context: %d)",se.context);
    else
      snprintf(se.tracestring, TRACESTRING_SIZE, "Intra 8x8 mode  = %3d (context: %d)",se.value1,se.context);
#endif

    // set symbol type and function pointers
    se.type = SE_INTRAPREDMODE;

    // encode and update rate
    writeIntraPredMode (&se, dataPart);

    bitCount[BITS_COEFF_Y_MB]+=se.len;
    rate += se.len;
  }

  return rate;
}

int writeIntraModes(void)
{
  switch (img->mb_data[img->current_mb_nr].mb_type)
  {
  case I4MB:
    return writeIntra4x4Modes();
    break;
  case I8MB:
    return writeIntra8x8Modes();
    break;
  default:
    return 0;
    break;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Converts 8x8 block type to coding value
 ************************************************************************
 */
int B8Mode2Value (int b8mode, int b8pdir)
{
  static const int b8start[8] = {0,0,0,0, 1, 4, 5, 10};
  static const int b8inc  [8] = {0,0,0,0, 1, 2, 2, 1};

  if (img->type!=B_SLICE)
  {
    return (b8mode-4);
  }
  else
  {
    return b8start[b8mode] + b8inc[b8mode] * b8pdir;
  }
}

/*!
************************************************************************
* \brief
*    Codes macroblock header
* \param rdopt
*    true for calls during RD-optimization
* \param coeff_rate
*    bitrate of Luma and Chroma coeff
************************************************************************
*/
int writeMBLayer (int rdopt, int *coeff_rate)
{
  int             i,j;
  int             mb_nr      = img->current_mb_nr;
  int             prev_mb_nr = FmoGetPreviousMBNr(img->current_mb_nr);
  Macroblock*     currMB     = &img->mb_data[mb_nr];
  Macroblock*     prevMB     = mb_nr ? (&img->mb_data[prev_mb_nr]) : NULL;
  SyntaxElement   se;
  int*            bitCount   = currMB->bitcounter;
  Slice*          currSlice  = img->currentSlice;
  DataPartition*  dataPart;
  const int*      partMap    = assignSE2partition[input->partition_mode];
  int             no_bits    = 0;
  int             skip       = currMB->mb_type ? 0:((img->type == B_SLICE) ? !currMB->cbp:1);
  int             mb_type;
  int             prevMbSkipped = 0;
  int             mb_field_tmp;
  Macroblock      *topMB = NULL;

  int             WriteFrameFieldMBInHeader = 0;

  if (img->MbaffFrameFlag)
  {
    if (0==(mb_nr & 0x01))
    {
      WriteFrameFieldMBInHeader = 1; // top field

      prevMbSkipped = 0;
    }
    else
    {
      if (prevMB->mb_type ? 0:((img->type == B_SLICE) ? !prevMB->cbp:1))
      {
        WriteFrameFieldMBInHeader = 1; // bottom, if top was skipped
      }

      topMB= &img->mb_data[prev_mb_nr];
      prevMbSkipped = topMB->skip_flag;
    }
  }
  currMB->IntraChromaPredModeFlag = IS_INTRA(currMB);

  // choose the appropriate data partition
  dataPart = &(currSlice->partArr[partMap[SE_MBTYPE]]);

  if(img->type == I_SLICE)
  {
    //========= write mb_aff (I_SLICE) =========
    if(WriteFrameFieldMBInHeader)
    {
      se.value1 = currMB->mb_field;
      se.value2 = 0;
      se.type   = SE_MBTYPE;

      TRACE_SE (se.tracestring, "mb_field_decoding_flag");
      writeFieldModeInfo(&se, dataPart);

      bitCount[BITS_MB_MODE] += se.len;
      no_bits                += se.len;
    }

    //========= write mb_type (I_SLICE) =========
    se.value1  = MBType2Value (currMB);
    se.value2  = 0;
    se.type    = SE_MBTYPE;

#if TRACE
    snprintf(se.tracestring, TRACESTRING_SIZE,   "mb_type (I_SLICE) (%2d,%2d) = %3d",img->mb_x, img->mb_y, currMB->mb_type);
#endif
    writeMB_typeInfo (&se, dataPart);

    bitCount[BITS_MB_MODE] += se.len;
    no_bits                += se.len;
  }
  // not I_SLICE, CABAC
  else if (input->symbol_mode == CABAC)
  {
    if (img->MbaffFrameFlag && ((img->current_mb_nr & 0x01) == 0||prevMbSkipped))
    {
      mb_field_tmp = currMB->mb_field;
      currMB->mb_field = field_flag_inference();
      CheckAvailabilityOfNeighborsCABAC();
      currMB->mb_field = mb_field_tmp;
    }

    //========= write mb_skip_flag (CABAC) =========
    mb_type    = MBType2Value (currMB);
    se.value1  = mb_type;
    se.value2  = currMB->cbp;
    se.type    = SE_MBTYPE;

    TRACE_SE (se.tracestring, "mb_skip_flag");
    writeMB_skip_flagInfo_CABAC(&se, dataPart);

    bitCount[BITS_MB_MODE] += se.len;
    no_bits                += se.len;

    CheckAvailabilityOfNeighborsCABAC();

    //========= write mb_aff (CABAC) =========
    if(img->MbaffFrameFlag && !skip) // check for copy mode
    {
      if(WriteFrameFieldMBInHeader)
      {
        se.value1 = currMB->mb_field;
        se.value2 = 0;
        se.type   =  SE_MBTYPE;

        TRACE_SE(se.tracestring, "mb_field_decoding_flag");
        writeFieldModeInfo(&se, dataPart);

        bitCount[BITS_MB_MODE] += se.len;
        no_bits                += se.len;
      }
    }

    //========= write mb_type (CABAC) =========
    if (currMB->mb_type != 0 || ((img->type == B_SLICE) && currMB->cbp != 0))
    {
      se.value1  = mb_type;
      se.value2  = 0;
      se.type    = SE_MBTYPE;

#if TRACE
      if (img->type == B_SLICE)
        snprintf(se.tracestring, TRACESTRING_SIZE, "mb_type (B_SLICE) (%2d,%2d) = %3d",img->mb_x, img->mb_y, currMB->mb_type);
      else
        snprintf(se.tracestring, TRACESTRING_SIZE, "mb_type (P_SLICE) (%2d,%2d) = %3d",img->mb_x, img->mb_y, currMB->mb_type);
#endif
      writeMB_typeInfo( &se, dataPart);

      bitCount[BITS_MB_MODE] += se.len;
      no_bits                += se.len;
    }
  }
  // VLC not intra
  else if (currMB->mb_type != 0 || ((img->type == B_SLICE) && currMB->cbp != 0))
  {
    //===== Run Length Coding: Non-Skipped macroblock =====
    se.value1  = img->cod_counter;
    se.value2  = 0;
    se.type    = SE_MBTYPE;

    TRACE_SE (se.tracestring, "mb_skip_run");
    writeSE_UVLC(&se, dataPart);

    bitCount[BITS_MB_MODE] += se.len;
    no_bits                += se.len;

    // Reset cod counter
    img->cod_counter = 0;

    // write mb_aff
    if(img->MbaffFrameFlag && !skip) // check for copy mode
    {
      if(WriteFrameFieldMBInHeader)
      {
        se.value1 = currMB->mb_field;
        se.type   =  SE_MBTYPE;

        TRACE_SE(se.tracestring, "mb_field_decoding_flag");
        writeSE_Flag (&se, dataPart);

        bitCount[BITS_MB_MODE] += se.len;
        no_bits                += se.len;
      }
    }
    // Put out mb mode
    se.value1  = MBType2Value (currMB);

    if (img->type != B_SLICE)
    {
      se.value1--;
    }
    se.type    = SE_MBTYPE;
    se.value2  = 0;

#if TRACE
    if (img->type == B_SLICE)
      snprintf(se.tracestring, TRACESTRING_SIZE, "mb_type (B_SLICE) (%2d,%2d) = %3d",img->mb_x, img->mb_y, currMB->mb_type);
    else
      snprintf(se.tracestring, TRACESTRING_SIZE, "mb_type (P_SLICE) (%2d,%2d) = %3d",img->mb_x, img->mb_y, currMB->mb_type);
#endif
    writeMB_typeInfo(&se, dataPart);

    bitCount[BITS_MB_MODE] += se.len;
    no_bits                += se.len;
  }
  else
  {
    //Run Length Coding: Skipped macroblock
    img->cod_counter++;

    currMB->skip_flag = 1;
    // CAVLC
    for (j=0; j < (4 + img->num_blk8x8_uv); j++)
      for (i=0; i < 4; i++)
        img->nz_coeff [img->current_mb_nr][i][j]=0;


    if(FmoGetNextMBNr(img->current_mb_nr) == -1 && img->cod_counter>0)
    {
      // Put out run
      se.value1  = img->cod_counter;
      se.value2  = 0;
      se.type    = SE_MBTYPE;

      TRACE_SE(se.tracestring, "mb_skip_run");
      writeSE_UVLC(&se, dataPart);

      bitCount[BITS_MB_MODE] += se.len;
      no_bits                += se.len;

      // Reset cod counter
      img->cod_counter = 0;
    }
  }

  //init NoMbPartLessThan8x8Flag
  currMB->NoMbPartLessThan8x8Flag = (IS_DIRECT(currMB) && !(active_sps->direct_8x8_inference_flag))? 0: 1;

  if (currMB->mb_type == IPCM)
  {
    int jj, uv;
    if (input->symbol_mode == CABAC)
    {
      int len;
      EncodingEnvironmentPtr eep = &dataPart->ee_cabac;
      len = arienco_bits_written(eep);
      arienco_done_encoding(eep); // This pads to byte
      len = arienco_bits_written(eep) - len;
      no_bits += len;
      // Now restart the encoder
      arienco_start_encoding(eep, dataPart->bitstream->streamBuffer, &(dataPart->bitstream->byte_pos));
      reset_pic_bin_count();
    }
    if (dataPart->bitstream->bits_to_go < 8)
    {
      // This will only happen in the CAVLC case, CABAC is already padded
      se.type  = SE_MBTYPE;
      se.len   = dataPart->bitstream->bits_to_go;
      no_bits += se.len;
      bitCount[BITS_COEFF_Y_MB]+= se.len;
      se.bitpattern = 0;
#if TRACE
      snprintf(se.tracestring, TRACESTRING_SIZE, "pcm_alignment_zero_bits = %d", se.len);
#endif
      writeSE_Fix(&se, dataPart);
    }
    for (j=0;j<MB_BLOCK_SIZE;j++)
    {
      jj = img->pix_y+j;
      for (i=0;i<MB_BLOCK_SIZE;i++)
      {
        se.len   = img->bitdepth_luma;
        se.type  = SE_MBTYPE;
        no_bits += se.len;
        se.bitpattern = enc_picture->imgY[jj][img->pix_x+i];
        se.value1 = se.bitpattern;
        bitCount[BITS_COEFF_Y_MB]+=se.len;
#if TRACE
        snprintf(se.tracestring, TRACESTRING_SIZE, "pcm_sample_luma (%d %d) = %d", j,i,se.bitpattern);
#endif
        writeSE_Fix(&se, dataPart);
      }
    }
    if (img->yuv_format != YUV400)
    {
      for (uv = 0; uv < 2; uv ++)
      {
        for (j=0;j<img->mb_cr_size_y;j++)
        {
          jj = img->pix_c_y+j;
          for (i=0;i<img->mb_cr_size_x;i++)
          {
            se.len   = img->bitdepth_chroma;
            se.type  = SE_MBTYPE;
            no_bits += se.len;
            se.bitpattern = enc_picture->imgUV[uv][jj][img->pix_c_x+i];
            se.value1 = se.bitpattern;
            bitCount[BITS_COEFF_UV_MB]+=se.len;
#if TRACE
            snprintf(se.tracestring, TRACESTRING_SIZE, "pcm_sample_chroma (%s) (%d %d) = %d", uv?"v":"u", j,i,se.bitpattern);
#endif
            writeSE_Fix(&se, dataPart);
          }
        }
      }
    }
    return no_bits;
  }

  //===== BITS FOR 8x8 SUB-PARTITION MODES =====
  if (IS_P8x8 (currMB))
  {
    dataPart = &(currSlice->partArr[partMap[SE_MBTYPE]]);

    for (i=0; i<4; i++)
    {
      se.value1  = B8Mode2Value (currMB->b8mode[i], currMB->b8pdir[i]);
      se.value2  = 0;
      se.type    = SE_MBTYPE;
#if TRACE
      snprintf(se.tracestring, TRACESTRING_SIZE, "8x8 mode/pdir(%2d) = %3d/%d", i, currMB->b8mode[i], currMB->b8pdir[i]);
#endif
      writeB8_typeInfo (&se, dataPart);
      bitCount[BITS_MB_MODE]+= se.len;
      no_bits               += se.len;

      //set NoMbPartLessThan8x8Flag for P8x8 mode
      currMB->NoMbPartLessThan8x8Flag &= (currMB->b8mode[i]==0 && active_sps->direct_8x8_inference_flag) ||
                                         (currMB->b8mode[i]==4);
    }
    no_bits += writeMotionInfo2NAL  ();
  }

  //============= Transform size flag for INTRA MBs =============
  //-------------------------------------------------------------
  //transform size flag for INTRA_4x4 and INTRA_8x8 modes
  if ((currMB->mb_type == I8MB || currMB->mb_type == I4MB) && input->Transform8x8Mode)
  {
    se.value1 = currMB->luma_transform_size_8x8_flag;
    se.type   = SE_MBTYPE;

#if TRACE
    snprintf(se.tracestring, TRACESTRING_SIZE, "transform_size_8x8_flag = %3d", currMB->luma_transform_size_8x8_flag);
#endif
    writeMB_transform_size(&se, dataPart);

    bitCount[BITS_MB_MODE] += se.len;
    no_bits                += se.len;
  }


 //===== BITS FOR INTRA PREDICTION MODES ====
  no_bits += writeIntraModes();
  //===== BITS FOR CHROMA INTRA PREDICTION MODE ====
  if (currMB->IntraChromaPredModeFlag && img->yuv_format != YUV400)
    no_bits += writeChromaIntraPredMode();
  else if(!rdopt) //GB CHROMA !!!!!
    currMB->c_ipred_mode = DC_PRED_8; //setting c_ipred_mode to default is not the right place here
                                      //resetting in rdopt.c (but where ??)
                                      //with cabac and bframes maybe it could crash without this default
                                      //since cabac needs the right neighborhood for the later MBs

  //----- motion information -----
  if (currMB->mb_type !=0 && currMB->mb_type !=P8x8)
  {
    no_bits  += writeMotionInfo2NAL  ();
  }

  if ((currMB->mb_type!=0) || (img->type==B_SLICE && (currMB->cbp!=0)))
  {
    *coeff_rate = writeCBPandLumaCoeff ();
    if (img->yuv_format != YUV400)
      *coeff_rate  += writeChromaCoeff ();

    no_bits  += *coeff_rate;
  }

  return no_bits;
}

void write_terminating_bit (short bit)
{
  DataPartition*          dataPart;
  const int*              partMap   = assignSE2partition[input->partition_mode];
  EncodingEnvironmentPtr  eep_dp;

  //--- write non-slice termination symbol if the macroblock is not the first one in its slice ---
  dataPart = &(img->currentSlice->partArr[partMap[SE_MBTYPE]]);
  dataPart->bitstream->write_flag = 1;
  eep_dp                          = &(dataPart->ee_cabac);

  biari_encode_symbol_final(eep_dp, bit);
#if TRACE
  fprintf (p_trace, "      CABAC terminating bit = %d\n",bit);
#endif

}


/*!
 ************************************************************************
 * \brief
 *    Write chroma intra prediction mode.
 ************************************************************************
 */
int writeChromaIntraPredMode(void)
{
  Macroblock*     currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement   se;
  Slice*          currSlice = img->currentSlice;
  int*            bitCount  = currMB->bitcounter;
  const int*      partMap   = assignSE2partition[input->partition_mode];
  int             rate      = 0;
  DataPartition*  dataPart;

  //===== BITS FOR CHROMA INTRA PREDICTION MODES

  se.value1 = currMB->c_ipred_mode;
  se.value2 = 0;
  se.type = SE_INTRAPREDMODE;
  dataPart = &(currSlice->partArr[partMap[SE_INTRAPREDMODE]]);

  TRACE_SE(se.tracestring, "intra_chroma_pred_mode");
  writeCIPredMode(&se, dataPart);

  bitCount[BITS_COEFF_UV_MB] += se.len;
  rate                       += se.len;

  return rate;
}


/*!
 ************************************************************************
 * \brief
*    Set global last_dquant according to macroblock delta qp
************************************************************************
*/

extern int last_dquant;

void set_last_dquant(void)
{
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];

  if ((IS_INTERMV (currMB) || IS_INTRA (currMB))
    || ((img->type==B_SLICE)  && currMB->cbp != 0))
  {
    // non-skip
    last_dquant = currMB->delta_qp;
  }
  else
  {
    // skip
    last_dquant = 0;
  }
}


/*!
 ************************************************************************
 * \brief
 *    Passes the chosen syntax elements to the NAL
 ************************************************************************
 */
void write_one_macroblock (int eos_bit)
{
  Macroblock* currMB   = &img->mb_data[img->current_mb_nr];
  int*        bitCount = currMB->bitcounter;
  int i;

  extern int cabac_encoding;

  // enable writing of trace file
#if TRACE
  Slice *curr_slice = img->currentSlice;
  curr_slice->partArr[0].bitstream->trace_enabled = TRUE;
  if (input->partition_mode)
  {
    curr_slice->partArr[1].bitstream->trace_enabled = TRUE;
    curr_slice->partArr[2].bitstream->trace_enabled = TRUE;
  }
#endif

  img->SumFrameQP += currMB->qp;

  //--- constrain intra prediction ---
  if(input->UseConstrainedIntraPred && (img->type==P_SLICE || img->type==B_SLICE))
  {
      img->intra_block[img->current_mb_nr] = IS_INTRA(currMB);
  }

  //===== init and update number of intra macroblocks =====
  if (img->current_mb_nr==0)
    intras=0;

  if (IS_INTRA(currMB))
    intras++;

  //--- write non-slice termination symbol if the macroblock is not the first one in its slice ---
  if (input->symbol_mode==CABAC && img->current_mb_nr!=img->currentSlice->start_mb_nr && eos_bit)
  {
    write_terminating_bit (0);
  }

#if TRACE
  // trace: write macroblock header
  if (p_trace)
  {
    fprintf(p_trace, "\n*********** Pic: %i (I/P) MB: %i Slice: %i **********\n\n", frame_no, img->current_mb_nr, img->current_slice_nr);
  }
#endif

  cabac_encoding = 1;

  //--- write macroblock ---
  writeMBLayer (0, &i);  // i is temporary

  if (!((currMB->mb_type !=0 ) || ((img->type==B_SLICE) && currMB->cbp != 0) ))
  {
    for (i=0; i < 4; i++)
      memset(img->nz_coeff [img->current_mb_nr][i], 0, (4 + img->num_blk8x8_uv) * sizeof(int));  // CAVLC
  }

  set_last_dquant();

  //--- set total bit-counter ---
  bitCount[BITS_TOTAL_MB] = bitCount[BITS_MB_MODE]  + bitCount[BITS_COEFF_Y_MB]
                          + bitCount[BITS_INTER_MB] + bitCount[BITS_CBP_MB]
                          + bitCount[BITS_DELTA_QUANT_MB] + bitCount[BITS_COEFF_UV_MB];

  if ( input->RCEnable )
  {
    //Rate control
    img->NumberofMBHeaderBits= bitCount[BITS_MB_MODE] + bitCount[BITS_INTER_MB]
    + bitCount[BITS_CBP_MB]  + bitCount[BITS_DELTA_QUANT_MB];
    img->NumberofMBTextureBits= bitCount[BITS_COEFF_Y_MB]+ bitCount[BITS_COEFF_UV_MB];

    generic_RC->NumberofTextureBits +=img->NumberofMBTextureBits;
    generic_RC->NumberofHeaderBits +=img->NumberofMBHeaderBits;
    /*basic unit layer rate control*/
    if(img->BasicUnit < img->FrameSizeInMbs)
    {
      generic_RC->NumberofBasicUnitHeaderBits  += img->NumberofMBHeaderBits;
      generic_RC->NumberofBasicUnitTextureBits += img->NumberofMBTextureBits;
    }
  }
  /*record the total number of MBs*/
  img->NumberofCodedMacroBlocks++;

  stats->bit_slice += bitCount[BITS_TOTAL_MB];

  cabac_encoding = 0;
}


/*!
 ************************************************************************
 * \brief
 *    Codes the reference frame
 ************************************************************************
 */
int writeReferenceFrame (int mode, int i, int j, int fwd_flag, int  ref)
{
  Macroblock*     currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement   se;
  Slice*          currSlice = img->currentSlice;
  int*            bitCount  = currMB->bitcounter;
  const int*      partMap   = assignSE2partition[input->partition_mode];
  int             rate      = 0;
  DataPartition*  dataPart  = &(currSlice->partArr[partMap[SE_REFFRAME]]);
  int             list       = ( fwd_flag ? LIST_0 + currMB->list_offset: LIST_1 + currMB->list_offset);

  se.value1  = ref;
  se.type    = SE_REFFRAME;
  se.value2 = (fwd_flag)? LIST_0: LIST_1;

  img->subblock_x = i; // position used for context determination
  img->subblock_y = j; // position used for context determination

#if TRACE
  if (fwd_flag)
    snprintf(se.tracestring, TRACESTRING_SIZE, "ref_idx_l0 = %d", se.value1);
  else
    snprintf(se.tracestring, TRACESTRING_SIZE, "ref_idx_l1 = %d", se.value1);
#endif

  writeRefFrame[list](&se, dataPart);

  bitCount[BITS_INTER_MB] += se.len;
  rate                    += se.len;

  return rate;
}


/*!
 ************************************************************************
 * \brief
 *    Writes motion vectors of an 8x8 block
 ************************************************************************
 */
int writeMotionVector8x8 (int  i0,
                          int  j0,
                          int  i1,
                          int  j1,
                          int  refframe,
                          int  list_idx,
                          int  mv_mode)
{
  int            i, j, k, l, m;
  int            curr_mvd;
  DataPartition* dataPart;

  int            rate       = 0;
  int            step_h     = input->part_size[mv_mode][0];
  int            step_v     = input->part_size[mv_mode][1];
  Macroblock*    currMB     = &img->mb_data[img->current_mb_nr];
  SyntaxElement  se;
  Slice*         currSlice  = img->currentSlice;
  int*           bitCount   = currMB->bitcounter;
  const int*     partMap    = assignSE2partition[input->partition_mode];
  int            refindex   = refframe;

  short******    all_mv     = img->all_mv;
  short******    pred_mv    = img->pred_mv;

  if (currMB->bi_pred_me && currMB->b8pdir[0]==2 && mv_mode == 1 && refindex == 0)
    all_mv = currMB->bi_pred_me == 1? img->bipred_mv1 : img->bipred_mv2 ;

  for (j=j0; j<j1; j+=step_v)
  {
    for (i=i0; i<i1; i+=step_h)
    {
      for (k=0; k<2; k++)
      {
        curr_mvd = all_mv[j][i][list_idx][refindex][mv_mode][k] - pred_mv[j][i][list_idx][refindex][mv_mode][k];

        //--- store (oversampled) mvd ---
        for (l=0; l < step_v; l++)
          for (m=0; m < step_h; m++)
          {
            currMB->mvd[list_idx][j+l][i+m][k] = curr_mvd;
          }
        dataPart = &(currSlice->partArr[partMap[SE_MVD]]);
        img->subblock_x = i; // position used for context determination
        img->subblock_y = j; // position used for context determination
        se.value1 = curr_mvd;
        se.value2  = 2*k+list_idx; // identifies the component and the direction; only used for context determination
        se.type   = SE_MVD;

#if TRACE
        if (!list_idx)
          snprintf(se.tracestring, TRACESTRING_SIZE, "mvd_l0 (%d) = %3d  (org_mv %3d pred_mv %3d)",k, curr_mvd, all_mv[j][i][list_idx][refindex][mv_mode][k], pred_mv[j][i][list_idx][refindex][mv_mode][k]);
        else
          snprintf(se.tracestring, TRACESTRING_SIZE, "mvd_l1 (%d) = %3d  (org_mv %3d pred_mv %3d)",k, curr_mvd, all_mv[j][i][list_idx][refindex][mv_mode][k], pred_mv[j][i][list_idx][refindex][mv_mode][k]);
#endif
        writeMVD (&se, dataPart);

        bitCount[BITS_INTER_MB] += se.len;
        rate                    += se.len;
      }
    }
  }
  return rate;
}


/*!
 ************************************************************************
 * \brief
 *    Writes motion info
 ************************************************************************
 */
int writeMotionInfo2NAL (void)
{
  int k, j0, i0, refframe;
  int jj;
  Macroblock*     currMB    = &img->mb_data[img->current_mb_nr];
  int             no_bits   = 0;
  int   bframe          = (img->type==B_SLICE);
  int   step_h0         = (input->blc_size[IS_P8x8(currMB) ? 4 : currMB->mb_type][0] >> 2);
  int   step_v0         = (input->blc_size[IS_P8x8(currMB) ? 4 : currMB->mb_type][1] >> 2);

  //=== If multiple ref. frames, write reference frame for the MB ===
  if (IS_INTERMV (currMB))
  {
    // if UVLC is turned on, a 8x8 macroblock with all ref=0 in a P-frame is signalled in macroblock mode
    if (!IS_P8x8 (currMB) || !ZeroRef (currMB) || input->symbol_mode==CABAC || bframe)
    {
      for (j0=0; j0<4; j0+=step_v0)
      {
        jj = img->block_y+j0;
        for (i0=0; i0<4; i0+=step_h0)
        {
          k=j0+(i0 >> 1);

          if ((currMB->b8pdir[k]==0 || currMB->b8pdir[k]==2) && currMB->b8mode[k]!=0)//has forward vector
          {
            no_bits += writeReferenceFrame (currMB->b8mode[k], i0, j0, 1, enc_picture->ref_idx[LIST_0][jj][img->block_x+i0]);
          }
        }
      }
      for (j0=0; j0<4; j0+=step_v0)
      {
        jj = img->block_y+j0;
        for (i0=0; i0<4; i0+=step_h0)
        {
          k=j0+(i0 >> 1);
          if ((currMB->b8pdir[k]==1 || currMB->b8pdir[k]==2) && currMB->b8mode[k]!=0)//has backward vector
          {
            no_bits += writeReferenceFrame (currMB->b8mode[k], i0, j0, 0, enc_picture->ref_idx[LIST_1][jj][img->block_x+i0]);
          }
        }
      }
    }
  }

  //===== write forward motion vectors =====
  if (IS_INTERMV (currMB))
  {
    for (j0=0; j0<4; j0+=step_v0)
    {
      jj = img->block_y+j0;
      for (i0=0; i0<4; i0+=step_h0)
      {
        k=j0+(i0 >> 1);
        if ((currMB->b8pdir[k]==0 || currMB->b8pdir[k]==2) && currMB->b8mode[k]!=0)//has forward vector
        {
          refframe  = enc_picture->ref_idx[LIST_0][jj][img->block_x+i0];
          no_bits  += writeMotionVector8x8 (i0, j0, i0+step_h0, j0+step_v0, refframe, LIST_0, currMB->b8mode[k]);
        }
      }
    }
  }


  //===== write backward motion vectors =====
  if (IS_INTERMV (currMB) && bframe)
  {
    for (j0=0; j0<4; j0+=step_v0)
    {
      jj = img->block_y+j0;
      for (i0=0; i0<4; i0+=step_h0)
      {
        k=j0+(i0 >> 1);
        if ((currMB->b8pdir[k]==1 || currMB->b8pdir[k]==2) && currMB->b8mode[k]!=0)//has backward vector
        {
          refframe  = enc_picture->ref_idx[LIST_1][jj][img->block_x+i0];
          no_bits  += writeMotionVector8x8 (i0, j0, i0+step_h0, j0+step_v0, refframe, LIST_1, currMB->b8mode[k]);
        }
      }
    }
  }
  return no_bits;
}



/*!
 ************************************************************************
 * \brief
 *    Writes chrominance coefficients
 ************************************************************************
 */
int writeChromaCoeff (void)
{
  int             rate      = 0;
  Macroblock*     currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement   se;
  int*            bitCount  = currMB->bitcounter;
  Slice*          currSlice = img->currentSlice;
  const int*      partMap   = assignSE2partition[input->partition_mode];
  int             cbp       = currMB->cbp;
  DataPartition*  dataPart;

  int   level, run;
  int   k, uv;
  int   b8, b4, param;
  int*  ACLevel;
  int*  ACRun;
  int*  DCLevel;
  int*  DCRun;

  static const int   chroma_dc_context[3]={CHROMA_DC, CHROMA_DC_2x4, CHROMA_DC_4x4};
  int   yuv = img->yuv_format - 1;

  static const unsigned char chroma_ac_param[3][8][4] =
  {
   {{ 4, 20,  5, 21},
    {36, 52, 37, 53},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0}},
   {{ 4, 20,  5, 21},
    { 6, 22,  7, 23},
    {36, 52, 37, 53},
    {38, 54, 39, 55},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0},
    { 0,  0,  0,  0}},
   {{ 4, 20,  5, 21},
    {36, 52, 37, 53},
    { 6, 22,  7, 23},
    {38, 54, 39, 55},
    { 8, 24,  9, 25},
    {40, 56, 41, 57},
    {10, 26, 11, 27},
    {42, 58, 43, 59}}
  };

  //=====
  //=====   D C - C O E F F I C I E N T S
  //=====
  if (cbp > 15)  // check if any chroma bits in coded block pattern is set
  {
    for (uv=0; uv < 2; uv++)
    {
      if (input->symbol_mode == UVLC)
      {
        param = uv;
        rate += writeCoeff4x4_CAVLC (CHROMA_DC, 0, 0, param);
        // CAVLC
      }
      else
      {

        DCLevel = img->cofDC[uv+1][0];
        DCRun   = img->cofDC[uv+1][1];

        level=1;
        for (k=0; k <= img->num_cdc_coeff && level != 0; ++k)
        {
          level = se.value1 = DCLevel[k]; // level
          run   = se.value2 = DCRun  [k]; // run

          se.context         = chroma_dc_context[yuv];
          se.type             = (IS_INTRA(currMB) ? SE_CHR_DC_INTRA : SE_CHR_DC_INTER);
          img->is_intra_block =  IS_INTRA(currMB);
          img->is_v_block     = uv;

          // choose the appropriate data partition
          dataPart = &(currSlice->partArr[partMap[se.type]]);
#if TRACE
          snprintf(se.tracestring, TRACESTRING_SIZE, "DC Chroma %2d: level =%3d run =%2d",k, level, run);
#endif
          writeRunLevel_CABAC(&se, dataPart);

          bitCount[BITS_COEFF_UV_MB] += se.len;
          rate                       += se.len;
        }
      }
    }
  }

  //=====
  //=====   A C - C O E F F I C I E N T S
  //=====
  uv=-1;
  if (cbp >> 4 == 2) // check if chroma bits in coded block pattern = 10b
  {
    for (b8=4; b8 < (4+img->num_blk8x8_uv); b8++)
    for (b4=0; b4 < 4; b4++)
    {
      if (input->symbol_mode == UVLC)
      {
        param = chroma_ac_param[yuv][b8-4][b4];
        rate += writeCoeff4x4_CAVLC (CHROMA_AC, b8, b4, param);
        // CAVLC
      }
      else
      {

        ACLevel = img->cofAC[b8][b4][0];
        ACRun   = img->cofAC[b8][b4][1];

        level=1;
        uv++;

        img->subblock_y = subblk_offset_y[yuv][b8-4][b4]>>2;
        img->subblock_x = subblk_offset_x[yuv][b8-4][b4]>>2;

        for (k=0; k < 16 && level != 0; k++)
        {
          level = se.value1 = ACLevel[k]; // level
          run   = se.value2 = ACRun  [k]; // run

          se.context          = CHROMA_AC;
          se.type             = (IS_INTRA(currMB) ? SE_CHR_AC_INTRA : SE_CHR_AC_INTER);
          img->is_intra_block =  IS_INTRA(currMB);
          img->is_v_block     = (uv>=(img->num_blk8x8_uv<<1));

          // choose the appropriate data partition
          dataPart = &(currSlice->partArr[partMap[se.type]]);
#if TRACE
          snprintf(se.tracestring, TRACESTRING_SIZE, "AC Chroma %2d: level =%3d run =%2d",k, level, run);
#endif
          writeRunLevel_CABAC(&se, dataPart);
          bitCount[BITS_COEFF_UV_MB] += se.len;
          rate                       += se.len;
        }
      }
    }
  }

  return rate;
}



/*!
 ************************************************************************
 * \brief
 *    Writes Luma coeff of an 4x4 block
 ************************************************************************
 */
int writeLumaCoeff4x4_CABAC (int b8, int b4, int intra4x4mode)
{
  int             rate      = 0;
  Macroblock*     currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement   se;
  Slice*          currSlice = img->currentSlice;
  const int*      partMap   = assignSE2partition[input->partition_mode];
  int*            bitCount  = currMB->bitcounter;
  DataPartition*  dataPart;

  int   level, run;
  int   k;
  int*  ACLevel = img->cofAC[b8][b4][0];
  int*  ACRun   = img->cofAC[b8][b4][1];

  img->subblock_x = ((b8&0x1)==0) ? (((b4&0x1)==0)? 0: 1) : (((b4&0x1)==0)? 2: 3); // horiz. position for coeff_count context
  img->subblock_y = (b8<2)        ? ((b4<2)       ? 0: 1) : ((b4<2)       ? 2: 3); // vert.  position for coeff_count context

  level=1; // get inside loop
  for(k=0; k<=16 && level !=0; k++)
  {
    level = se.value1 = ACLevel[k]; // level
    run   = se.value2 = ACRun  [k]; // run

    //currSE->writing = writeRunLevel_CABAC;

    se.context     = LUMA_4x4;
    se.type        = (k==0
      ? (intra4x4mode ? SE_LUM_DC_INTRA : SE_LUM_DC_INTER)
      : (intra4x4mode ? SE_LUM_AC_INTRA : SE_LUM_AC_INTER));
    img->is_intra_block = intra4x4mode;

    // choose the appropriate data partition
    dataPart = &(currSlice->partArr[partMap[se.type]]);
#if TRACE
    snprintf(se.tracestring, TRACESTRING_SIZE, "Luma sng(%2d) level =%3d run =%2d", k, level,run);
#endif
    writeRunLevel_CABAC(&se, dataPart);
    bitCount[BITS_COEFF_Y_MB] += se.len;
    rate                      += se.len;
  }
  return rate;
}

/*!
 ************************************************************************
 * \brief
 *    Writes Luma coeff of an 8x8 block
 ************************************************************************
 */
int writeLumaCoeff8x8_CABAC (int b8, int intra_mode)
{
  int             rate      = 0;
  Macroblock*     currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement   se;
  Slice*          currSlice = img->currentSlice;
  const int*      partMap   = assignSE2partition[input->partition_mode];
  int*            bitCount  = currMB->bitcounter;
  DataPartition*  dataPart;

  int   level, run;
  int   k;
  int*  ACLevel = img->cofAC[b8][0][0];
  int*  ACRun   = img->cofAC[b8][0][1];

  img->subblock_x = ((b8&0x1)==0)?0:2;  // horiz. position for coeff_count context
  img->subblock_y = (b8<2)?0:2;     // vert.  position for coeff_count context


  level=1; // get inside loop
  for(k=0; k<=64 && level !=0; k++)
  {
    level = se.value1 = ACLevel[k]; // level
    run   = se.value2 = ACRun  [k]; // run

    se.context     = LUMA_8x8;
    se.type        = (k==0
      ? (intra_mode ? SE_LUM_DC_INTRA : SE_LUM_DC_INTER)
      : (intra_mode ? SE_LUM_AC_INTRA : SE_LUM_AC_INTER));
    img->is_intra_block = intra_mode;

    // choose the appropriate data partition
    dataPart = &(currSlice->partArr[partMap[img->type != B_SLICE ? se.type : SE_BFRAME]]);

#if TRACE
    snprintf(se.tracestring, TRACESTRING_SIZE, "Luma8x8 sng(%2d) level =%3d run =%2d", k, level,run);
#endif
    writeRunLevel_CABAC(&se, dataPart);
    bitCount[BITS_COEFF_Y_MB] += se.len;
    rate                      += se.len;
  }
  return rate;
}

/*!
************************************************************************
* \brief
*    Writes Luma Coeff of an 8x8 block
************************************************************************
*/
int writeLumaCoeff8x8 (int block8x8, int block_mode, int transform_size_flag)
{
  int  block4x4, rate = 0;
  int intra4x4mode = (block_mode==IBLOCK);

  if (block_mode == I8MB)
    assert(transform_size_flag == 1);


  if((!transform_size_flag) || input->symbol_mode == UVLC) // allow here if 4x4 or UVLC
  {
    if (input->symbol_mode == UVLC )
    {
      for (block4x4=0; block4x4<4; block4x4++)
        rate += writeCoeff4x4_CAVLC (LUMA, block8x8, block4x4, (transform_size_flag)?(block_mode==I8MB):intra4x4mode);// CAVLC, pass new intra
    }
    else
    {
      for (block4x4=0; block4x4<4; block4x4++)
        rate += writeLumaCoeff4x4_CABAC (block8x8, block4x4, intra4x4mode);
    }
  }
  else
    rate += writeLumaCoeff8x8_CABAC (block8x8, (block_mode == I8MB));

  return rate;
}

/*!
 ************************************************************************
 * \brief
 *    Writes CBP, DQUANT, and Luma Coefficients of an macroblock
 ************************************************************************
 */
int writeCBPandLumaCoeff (void)
{
  int             mb_x, mb_y, i, j, k;
  int             level, run;
  int             rate      = 0;
  Macroblock*     currMB    = &img->mb_data[img->current_mb_nr];
  int*            bitCount  = currMB->bitcounter;
  SyntaxElement   se;
  Slice*          currSlice = img->currentSlice;
  const int*      partMap   = assignSE2partition[input->partition_mode];
  int             cbp       = currMB->cbp;
  DataPartition*  dataPart;
  int             need_transform_size_flag;   //ADD-VG-24062004

  int   b8, b4;
  int*  DCLevel = img->cofDC[0][0];
  int*  DCRun   = img->cofDC[0][1];
  int*  ACLevel;
  int*  ACRun;

  if (!IS_NEWINTRA (currMB))
  {
    //=====   C B P   =====
    //---------------------
    se.value1 = cbp;
    se.type   = SE_CBP;

    // choose the appropriate data partition
    dataPart = &(currSlice->partArr[partMap[se.type]]);

#if TRACE
    snprintf(se.tracestring, TRACESTRING_SIZE, "CBP (%2d,%2d) = %3d",img->mb_x, img->mb_y, cbp);
#endif
    writeCBP (&se, dataPart);

    bitCount[BITS_CBP_MB] += se.len;
    rate                  += se.len;

    //============= Transform Size Flag for INTER MBs =============
    //-------------------------------------------------------------
    need_transform_size_flag = (((currMB->mb_type >= 1 && currMB->mb_type <= 3)||
                                (IS_DIRECT(currMB) && active_sps->direct_8x8_inference_flag) ||
                                (currMB->NoMbPartLessThan8x8Flag))
                                && currMB->mb_type != I8MB && currMB->mb_type != I4MB
                                && (currMB->cbp&15)
                                && input->Transform8x8Mode);

    if (need_transform_size_flag)
    {
      se.value1 = currMB->luma_transform_size_8x8_flag;
      se.type   = SE_MBTYPE;

#if TRACE
      snprintf(se.tracestring, TRACESTRING_SIZE, "transform_size_8x8_flag = %3d", currMB->luma_transform_size_8x8_flag);
#endif
      writeMB_transform_size(&se, dataPart);

      bitCount[BITS_MB_MODE] += se.len;
      rate                   += se.len;
    }
  }

  //=====   DQUANT   =====
  //----------------------
  if (cbp!=0 || IS_NEWINTRA (currMB))
  {
    se.value1 = currMB->delta_qp;
    se.type = SE_DELTA_QUANT;

    // choose the appropriate data partition
    dataPart = &(img->currentSlice->partArr[partMap[se.type]]);
#if TRACE
    snprintf(se.tracestring, TRACESTRING_SIZE, "Delta QP (%2d,%2d) = %3d",img->mb_x, img->mb_y, currMB->delta_qp);
#endif
    writeDquant (&se, dataPart);
    bitCount[BITS_DELTA_QUANT_MB] += se.len;
    rate                          += se.len;
  }

  for (i=0; i < 4; i++)
    memset(img->nz_coeff [img->current_mb_nr][i], 0, (4 + img->num_blk8x8_uv) * sizeof(int));

  if (!IS_NEWINTRA (currMB))
  {
    //=====  L U M I N A N C E   =====
    //--------------------------------
    for (i=0; i<4; i++)  if (cbp & (1<<i))
    {
      rate += writeLumaCoeff8x8 (i, currMB->b8mode[i], currMB->luma_transform_size_8x8_flag);
    }
  }
  else
  {
    //=====  L U M I N A N C E   f o r   1 6 x 1 6   =====
    //----------------------------------------------------
    // DC coeffs
    if (input->symbol_mode == UVLC)
    {
      rate += writeCoeff4x4_CAVLC (LUMA_INTRA16x16DC, 0, 0, 0);  // CAVLC
    }
    else
    {
      level=1; // get inside loop
      img->is_intra_block = TRUE;
      for (k=0; k<=16 && level!=0; k++)
      {
        level = se.value1 = DCLevel[k]; // level
        run   = se.value2 = DCRun  [k]; // run

        se.context = LUMA_16DC;
        se.type    = SE_LUM_DC_INTRA;   // element is of type DC

        // choose the appropriate data partition
        dataPart = &(currSlice->partArr[partMap[se.type]]);

#if TRACE
        snprintf(se.tracestring, TRACESTRING_SIZE, "DC luma 16x16 sng(%2d) level =%3d run =%2d", k, level, run);
#endif
        writeRunLevel_CABAC(&se, dataPart);
        bitCount[BITS_COEFF_Y_MB] += se.len;
        rate                      += se.len;
      }
    }

    // AC coeffs
    if (cbp & 15)
    {
      for (mb_y=0; mb_y < 4; mb_y += 2)
      for (mb_x=0; mb_x < 4; mb_x += 2)
      for (j=mb_y; j < mb_y+2; j++)
      for (i=mb_x; i < mb_x+2; i++)
      {
        b8      = 2*(j >> 1) + (i >> 1);
        b4      = 2*(j & 0x01) + (i & 0x01);
        if (input->symbol_mode == UVLC)
        {
          rate += writeCoeff4x4_CAVLC (LUMA_INTRA16x16AC, b8, b4, 0);  // CAVLC
        }
        else
        {
          ACLevel = img->cofAC[b8][b4][0];
          ACRun   = img->cofAC[b8][b4][1];

          img->subblock_y = j;
          img->subblock_x = i;

          level=1; // get inside loop
          img->is_intra_block = TRUE;
          for (k=0;k<16 && level !=0;k++)
          {
            level = se.value1 = ACLevel[k]; // level
            run   = se.value2 = ACRun  [k]; // run

            se.context = LUMA_16AC;
            se.type    = SE_LUM_AC_INTRA;   // element is of type AC

            // choose the appropriate data partition
            dataPart = &(currSlice->partArr[partMap[se.type]]);
#if TRACE
            snprintf(se.tracestring, TRACESTRING_SIZE, "AC luma 16x16 sng(%2d) level =%3d run =%2d", k, level, run);
#endif
            writeRunLevel_CABAC(&se, dataPart);
            bitCount[BITS_COEFF_Y_MB] += se.len;
            rate                      += se.len;
          }
        }
      }
    }
  }

  return rate;
}


/*!
 ************************************************************************
 * \brief
 *    Get the Prediction from the Neighboring Blocks for Number of Nonzero Coefficients
 *
 *    Luma Blocks
 ************************************************************************
 */
int predict_nnz(int i,int j)
{
  PixelPos pix;

  int pred_nnz = 0;
  int cnt      = 0;
  int mb_nr    = img->current_mb_nr;
  Macroblock *currMB = &(img->mb_data[mb_nr]);

  // left block
  getLuma4x4Neighbour(mb_nr, (i << 2) - 1, (j << 2), &pix);

  if (IS_INTRA(currMB) && pix.available && active_pps->constrained_intra_pred_flag && ((input->partition_mode != 0) && !img->currentPicture->idr_flag))
  {
    pix.available &= img->intra_block[pix.mb_addr];
    if (!pix.available)
      cnt++;
  }

  if (pix.available)
  {
    pred_nnz = img->nz_coeff [pix.mb_addr ][pix.x][pix.y];
    cnt++;
  }

  // top block
  getLuma4x4Neighbour(mb_nr, (i<<2), (j<<2) - 1, &pix);

  if (IS_INTRA(currMB) && pix.available && active_pps->constrained_intra_pred_flag && ((input->partition_mode != 0) && !img->currentPicture->idr_flag))
  {
    pix.available &= img->intra_block[pix.mb_addr];
    if (!pix.available)
      cnt++;
  }

  if (pix.available)
  {
    pred_nnz += img->nz_coeff [pix.mb_addr ][pix.x][pix.y];
    cnt++;
  }

  if (cnt==2)
  {
    pred_nnz++;
    pred_nnz>>=1;
  }

  return pred_nnz;
}


/*!
 ************************************************************************
 * \brief
 *    Get the Prediction from the Neighboring Blocks for Number of Nonzero Coefficients
 *
 *    Chroma Blocks
 ************************************************************************
 */
int predict_nnz_chroma(int i,int j)
{
  PixelPos pix;

  int pred_nnz = 0;
  int cnt      = 0;
  int mb_nr    = img->current_mb_nr;
  static const int j_off_tab [12] = {0,0,0,0,4,4,4,4,8,8,8,8};
  int j_off = j_off_tab[j];
  Macroblock *currMB = &(img->mb_data[mb_nr]);

  if (img->yuv_format != YUV444)
  {
    //YUV420 and YUV422
    // left block
    getChroma4x4Neighbour(mb_nr, ((i & 0x01)<<2) - 1, ((j-4)<<2), &pix);

    if (IS_INTRA(currMB) && pix.available && active_pps->constrained_intra_pred_flag && ((input->partition_mode != 0) && !img->currentPicture->idr_flag))
    {
      pix.available &= img->intra_block[pix.mb_addr];
      if (!pix.available)
        cnt++;
    }

    if (pix.available)
    {
      pred_nnz = img->nz_coeff [pix.mb_addr ][2 * (i >> 1) + pix.x][4 + pix.y];
      cnt++;
    }

    // top block
    getChroma4x4Neighbour(mb_nr, ((i & 0x01)<<2), ((j-4)<<2) -1,  &pix);

    if (IS_INTRA(currMB) && pix.available && active_pps->constrained_intra_pred_flag && ((input->partition_mode != 0) && !img->currentPicture->idr_flag))
    {
      pix.available &= img->intra_block[pix.mb_addr];
      if (!pix.available)
        cnt++;
    }

    if (pix.available)
    {
      pred_nnz += img->nz_coeff [pix.mb_addr ][2 * (i >> 1) + pix.x][4 + pix.y];
      cnt++;
    }
  }
  else
  {
    //YUV444
    // left block
    getChroma4x4Neighbour(mb_nr, (i<<2) -1, ((j-j_off)<<2), &pix);

    if (IS_INTRA(currMB) && pix.available && active_pps->constrained_intra_pred_flag && ((input->partition_mode != 0) && !img->currentPicture->idr_flag))
    {
      pix.available &= img->intra_block[pix.mb_addr];
      cnt--;
    }

    if (pix.available)
    {
      pred_nnz = img->nz_coeff [pix.mb_addr ][pix.x][j_off + pix.y];
      cnt++;
    }

    // top block
    getChroma4x4Neighbour(mb_nr, (i<<2), ((j-j_off)<<2)-1, &pix);

    if (IS_INTRA(currMB) && pix.available && active_pps->constrained_intra_pred_flag && ((input->partition_mode != 0) && !img->currentPicture->idr_flag))
    {
      pix.available &= img->intra_block[pix.mb_addr];
      cnt--;
    }

    if (pix.available)
    {
      pred_nnz += img->nz_coeff [pix.mb_addr ][pix.x][j_off + pix.y];
      cnt++;
    }

  }

  if (cnt==2)
  {
    pred_nnz++;
    pred_nnz>>=1;
  }

  return pred_nnz;
}



/*!
 ************************************************************************
 * \brief
 *    Writes coeff of an 4x4 block (CAVLC)
 *
 * \author
 *    Karl Lillevold <karll@real.com>
 *    contributions by James Au <james@ubvideo.com>
 ************************************************************************
 */

int writeCoeff4x4_CAVLC (int block_type, int b8, int b4, int param)
{
  int           no_bits    = 0;
  Macroblock    *currMB    = &img->mb_data[img->current_mb_nr];
  SyntaxElement se;
  int           *bitCount  = currMB->bitcounter;
  Slice         *currSlice = img->currentSlice;
  DataPartition *dataPart;
  int           *partMap   = assignSE2partition[input->partition_mode];

  int k,level = 1,run,vlcnum;
  int numcoeff = 0, lastcoeff = 0, numtrailingones = 0; 
  int numones = 0, totzeros = 0, zerosleft, numcoef;
  int numcoeff_vlc;
  int code, level_two_or_higher;
  int dptype = 0, bitcounttype = 0;
  int nnz, max_coeff_num = 0, cdc=0, cac=0;
  int subblock_x, subblock_y;
  char type[15];

  static const int incVlc[] = {0,3,6,12,24,48,32768};  // maximum vlc = 6


  int*  pLevel = NULL;
  int*  pRun = NULL;

  switch (block_type)
  {
  case LUMA:
    max_coeff_num = 16;
    bitcounttype = BITS_COEFF_Y_MB;

    pLevel = img->cofAC[b8][b4][0];
    pRun   = img->cofAC[b8][b4][1];

    sprintf(type, "%s", "Luma");
    if (IS_INTRA (currMB))
    {
      dptype = SE_LUM_AC_INTRA;
    }
    else
    {
      dptype = SE_LUM_AC_INTER;
    }
    break;
  case LUMA_INTRA16x16DC:
    max_coeff_num = 16;
    bitcounttype = BITS_COEFF_Y_MB;

    pLevel = img->cofDC[0][0];
    pRun   = img->cofDC[0][1];

    sprintf(type, "%s", "Lum16DC");
    dptype = SE_LUM_DC_INTRA;
    break;
  case LUMA_INTRA16x16AC:
    max_coeff_num = 15;
    bitcounttype = BITS_COEFF_Y_MB;

    pLevel = img->cofAC[b8][b4][0];
    pRun   = img->cofAC[b8][b4][1];

    sprintf(type, "%s", "Lum16AC");
    dptype = SE_LUM_AC_INTRA;
    break;

  case CHROMA_DC:
    max_coeff_num = img->num_cdc_coeff;
    bitcounttype = BITS_COEFF_UV_MB;
    cdc = 1;

    pLevel = img->cofDC[param+1][0];
    pRun   = img->cofDC[param+1][1];

    sprintf(type, "%s", "ChrDC");
    if (IS_INTRA (currMB))
    {
      dptype = SE_CHR_DC_INTRA;
    }
    else
    {
      dptype = SE_CHR_DC_INTER;
    }
    break;
  case CHROMA_AC:
    max_coeff_num = 15;
    bitcounttype = BITS_COEFF_UV_MB;
    cac = 1;

    pLevel = img->cofAC[b8][b4][0];
    pRun   = img->cofAC[b8][b4][1];

    sprintf(type, "%s", "ChrAC");
    if (IS_INTRA (currMB))
    {
      dptype = SE_CHR_AC_INTRA;
    }
    else
    {
      dptype = SE_CHR_AC_INTER;
    }
    break;
  default:
    error("writeCoeff4x4_CAVLC: Invalid block type", 600);
    break;
  }

  dataPart = &(currSlice->partArr[partMap[dptype]]);

  for(k = 0; (k <= ((cdc)?img->num_cdc_coeff:16))&& level !=0; k++)
  {
    level = pLevel[k]; // level
    run   = pRun[k];   // run

    if (level)
    {
      if (run)
        totzeros += run;
      if (iabs(level) == 1)
      {
        numtrailingones ++;
        numones ++;
        if (numtrailingones > 3)
        {
          numtrailingones = 3; /* clip to 3 */
        }
      }
      else
      {
        numtrailingones = 0;
      }
      numcoeff ++;
      lastcoeff = k;
    }
  }

  if (!cdc)
  {
    if (!cac)
    {
      // luma
      subblock_x = ((b8&0x1)==0)?(((b4&0x1)==0)?0:1):(((b4&0x1)==0)?2:3);
        // horiz. position for coeff_count context
      subblock_y = (b8<2)?((b4<2)?0:1):((b4<2)?2:3);
        // vert.  position for coeff_count context
      nnz = predict_nnz(subblock_x,subblock_y);
    }
    else
    {
      // chroma AC
      subblock_x = param >> 4;
      subblock_y = param & 15;
      nnz = predict_nnz_chroma(subblock_x,subblock_y);
    }

    img->nz_coeff [img->current_mb_nr ][subblock_x][subblock_y] = numcoeff;


    if (nnz < 2)
    {
      numcoeff_vlc = 0;
    }
    else if (nnz < 4)
    {
      numcoeff_vlc = 1;
    }
    else if (nnz < 8)
    {
      numcoeff_vlc = 2;
    }
    else
    {
      numcoeff_vlc = 3;
    }

  }
  else
  {
    // chroma DC (has its own VLC)
    // numcoeff_vlc not relevant
    numcoeff_vlc = 0;

    subblock_x = param;
    subblock_y = param;
  }

  se.type  = dptype;

  se.value1 = numcoeff;
  se.value2 = numtrailingones;
  se.len    = numcoeff_vlc; /* use len to pass vlcnum */

#if TRACE
  snprintf(se.tracestring,
    TRACESTRING_SIZE, "%s # c & tr.1s(%d,%d) vlc=%d #c=%d #t1=%d",
    type, subblock_x, subblock_y, numcoeff_vlc, numcoeff, numtrailingones);
#endif

  if (!cdc)
    writeSyntaxElement_NumCoeffTrailingOnes(&se, dataPart);
  else
    writeSyntaxElement_NumCoeffTrailingOnesChromaDC(&se, dataPart);

  bitCount[bitcounttype]+=se.len;
  no_bits               +=se.len;

  if (!numcoeff)
    return no_bits;

  if (numcoeff)
  {
    code = 0;
    for (k = lastcoeff; k > lastcoeff-numtrailingones; k--)
    {
      level = pLevel[k]; // level
      if (iabs(level) > 1)
      {
        printf("ERROR: level > 1\n");
        exit(-1);
      }
      code <<= 1;
      if (level < 0)
      {
        code |= 0x1;
      }
    }

    if (numtrailingones)
    {
      se.type  = dptype;

      se.value2 = numtrailingones;
      se.value1 = code;

#if TRACE
      snprintf(se.tracestring,
        TRACESTRING_SIZE, "%s trailing ones sign (%d,%d)",
        type, subblock_x, subblock_y);
#endif

      writeSyntaxElement_VLC (&se, dataPart);
      bitCount[bitcounttype]+=se.len;
      no_bits               +=se.len;

    }

    // encode levels
    level_two_or_higher = (numcoeff > 3 && numtrailingones == 3) ? 0 : 1;

    vlcnum = (numcoeff > 10 && numtrailingones < 3) ? 1 : 0;

    for (k = lastcoeff - numtrailingones; k >= 0; k--)
    {
      level = pLevel[k]; // level

      se.value1 = level;
      se.type  = dptype;

  #if TRACE
        snprintf(se.tracestring,
          TRACESTRING_SIZE, "%s lev (%d,%d) k=%d vlc=%d lev=%3d",
            type, subblock_x, subblock_y, k, vlcnum, level);
  #endif

          if (level_two_or_higher)
          {
            if (se.value1 > 0)
              se.value1 --;
            else
              se.value1 ++;
            level_two_or_higher = 0;
          }

      //    encode level
      if (vlcnum == 0)
        writeSyntaxElement_Level_VLC1(&se, dataPart, active_sps->profile_idc);
      else
        writeSyntaxElement_Level_VLCN(&se, vlcnum, dataPart, active_sps->profile_idc);

      // update VLC table
      if (iabs(level)>incVlc[vlcnum])
        vlcnum++;

      if (k == lastcoeff - numtrailingones && iabs(level)>3)
        vlcnum = 2;

      bitCount[bitcounttype]+=se.len;
      no_bits               +=se.len;
    }

    // encode total zeroes
    if (numcoeff < max_coeff_num)
    {

      se.type  = dptype;
      se.value1 = totzeros;

      vlcnum = numcoeff-1;

      se.len = vlcnum;

#if TRACE
      snprintf(se.tracestring,
        TRACESTRING_SIZE, "%s totalrun (%d,%d) vlc=%d totzeros=%3d",
          type, subblock_x, subblock_y, vlcnum, totzeros);
#endif
      if (!cdc)
        writeSyntaxElement_TotalZeros(&se, dataPart);
      else
        writeSyntaxElement_TotalZerosChromaDC(&se, dataPart);

      bitCount[bitcounttype]+=se.len;
      no_bits               +=se.len;
    }

    // encode run before each coefficient
    zerosleft = totzeros;
    numcoef = numcoeff;
    for (k = lastcoeff; k >= 0; k--)
    {
      run = pRun[k]; // run

      se.value1 = run;
      se.type  = dptype;

      // for last coeff, run is remaining totzeros
      // when zerosleft is zero, remaining coeffs have 0 run
      if (numcoeff <= 1 || !zerosleft)
        break;

      if (numcoef > 1 && zerosleft)
      {

        vlcnum = zerosleft - 1;
        if (vlcnum > RUNBEFORE_NUM-1)
          vlcnum = RUNBEFORE_NUM-1;

        se.len = vlcnum;

#if TRACE
        snprintf(se.tracestring,
          TRACESTRING_SIZE, "%s run (%d,%d) k=%d vlc=%d run=%2d",
            type, subblock_x, subblock_y, k, vlcnum, run);
#endif

        writeSyntaxElement_Run(&se, dataPart);

        bitCount[bitcounttype]+=se.len;
        no_bits               +=se.len;

        zerosleft -= run;
        numcoef --;
      }
    }
  }

  return no_bits;
}

/*!
 ************************************************************************
 * \brief
 *    Find best 16x16 based intra mode
 *
 * \par Input:
 *    Image parameters, pointer to best 16x16 intra mode
 *
 * \par Output:
 *    best 16x16 based SAD
 ************************************************************************/
int find_sad_16x16(int *intra_mode)
{
  int current_intra_sad_2,best_intra_sad2;
  int M1[16][16],M0[4][4][4][4],M3[4],M4[4][4];

  int i,j,k;
  int ii,jj;
  int mb_nr = img->current_mb_nr;

  PixelPos up;          //!< pixel position p(0,-1)
  PixelPos left[17];    //!< pixel positions p(-1, -1..15)

  int up_avail, left_avail, left_up_avail;

  for (i=0;i<17;i++)
  {
    getNeighbour(mb_nr, -1 ,  i-1 , IS_LUMA, &left[i]);
  }

  getNeighbour(mb_nr, 0     ,  -1 , IS_LUMA, &up);

  if (!(input->UseConstrainedIntraPred))
  {
    up_avail   = up.available;
    left_avail = left[1].available;
    left_up_avail = left[0].available;
  }
  else
  {
    up_avail      = up.available ? img->intra_block[up.mb_addr] : 0;
    for (i=1, left_avail=1; i<17;i++)
      left_avail  &= left[i].available ? img->intra_block[left[i].mb_addr]: 0;
    left_up_avail = left[0].available ? img->intra_block[left[0].mb_addr]: 0;
  }

  best_intra_sad2=MAX_VALUE;
  *intra_mode = DC_PRED_16;

  for (k=0;k<4;k++)
  {
    if (input->IntraDisableInterOnly == 0 || img->type != I_SLICE)
    {
      if (input->Intra16x16ParDisable && (k==VERT_PRED_16||k==HOR_PRED_16))
        continue;

      if (input->Intra16x16PlaneDisable && k==PLANE_16)
        continue;
    }
    //check if there are neighbours to predict from
    if ((k==0 && !up_avail) || (k==1 && !left_avail) || (k==3 && (!left_avail || !up_avail || !left_up_avail)))
    {
      ; // edge, do nothing
    }
    else
    {
      for (j=0;j<16;j++)
      {
        for (i=0;i<16;i++)
        {
          M1[j][i]=imgY_org[img->opix_y+j][img->opix_x+i]-img->mprr_2[k][j][i];
          M0[i & 0x03][i >> 2][j & 0x03][j >> 2]=M1[j][i];
        }
      }
      current_intra_sad_2=0;              // no SAD start handicap here
      for (jj=0;jj<4;jj++)
      {
        for (ii=0;ii<4;ii++)
        {
          for (j=0;j<4;j++)
          {
            M3[0]=M0[0][ii][j][jj]+M0[3][ii][j][jj];
            M3[1]=M0[1][ii][j][jj]+M0[2][ii][j][jj];
            M3[2]=M0[1][ii][j][jj]-M0[2][ii][j][jj];
            M3[3]=M0[0][ii][j][jj]-M0[3][ii][j][jj];

            M0[0][ii][j][jj]=M3[0]+M3[1];
            M0[2][ii][j][jj]=M3[0]-M3[1];
            M0[1][ii][j][jj]=M3[2]+M3[3];
            M0[3][ii][j][jj]=M3[3]-M3[2];
          }

          for (i=0;i<4;i++)
          {
            M3[0]=M0[i][ii][0][jj]+M0[i][ii][3][jj];
            M3[1]=M0[i][ii][1][jj]+M0[i][ii][2][jj];
            M3[2]=M0[i][ii][1][jj]-M0[i][ii][2][jj];
            M3[3]=M0[i][ii][0][jj]-M0[i][ii][3][jj];

            M0[i][ii][0][jj]=M3[0]+M3[1];
            M0[i][ii][2][jj]=M3[0]-M3[1];
            M0[i][ii][1][jj]=M3[2]+M3[3];
            M0[i][ii][3][jj]=M3[3]-M3[2];
            for (j=0;j<4;j++)
              if ((i+j)!=0)
                current_intra_sad_2 += iabs(M0[i][ii][j][jj]);
          }
        }
      }

      for (j=0;j<4;j++)
        for (i=0;i<4;i++)
          M4[j][i]=M0[0][i][0][j]/4;

        // Hadamard of DC koeff
        for (j=0;j<4;j++)
        {
          M3[0]=M4[j][0]+M4[j][3];
          M3[1]=M4[j][1]+M4[j][2];
          M3[2]=M4[j][1]-M4[j][2];
          M3[3]=M4[j][0]-M4[j][3];

          M4[j][0]=M3[0]+M3[1];
          M4[j][2]=M3[0]-M3[1];
          M4[j][1]=M3[2]+M3[3];
          M4[j][3]=M3[3]-M3[2];
        }

        for (i=0;i<4;i++)
        {
          M3[0]=M4[0][i]+M4[3][i];
          M3[1]=M4[1][i]+M4[2][i];
          M3[2]=M4[1][i]-M4[2][i];
          M3[3]=M4[0][i]-M4[3][i];

          M4[0][i]=M3[0]+M3[1];
          M4[2][i]=M3[0]-M3[1];
          M4[1][i]=M3[2]+M3[3];
          M4[3][i]=M3[3]-M3[2];

          for (j=0;j<4;j++)
            current_intra_sad_2 += iabs(M4[j][i]);
        }
        if(current_intra_sad_2 < best_intra_sad2)
        {
          best_intra_sad2=current_intra_sad_2;
          *intra_mode = k; // update best intra mode

        }
    }
  }
  best_intra_sad2 = best_intra_sad2/2;

  return best_intra_sad2;

}
