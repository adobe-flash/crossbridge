
/*!
 *************************************************************************************
 * \file cabac.c
 *
 * \brief
 *    CABAC entropy coding routines
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Detlev Marpe                    <marpe@hhi.de>
 **************************************************************************************
 */

#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "global.h"

#include "cabac.h"
#include "image.h"
#include "mb_access.h"

int last_dquant = 0;

#if TRACE
#define CABAC_TRACE if (dp->bitstream->trace_enabled) trace2out_cabac (se)
#else
  #define CABAC_TRACE
#endif

/***********************************************************************
 * L O C A L L Y   D E F I N E D   F U N C T I O N   P R O T O T Y P E S
 ***********************************************************************
 */


void unary_bin_encode(EncodingEnvironmentPtr eep_frame,
                      unsigned int symbol,
                      BiContextTypePtr ctx,
                      int ctx_offset);

void unary_bin_max_encode(EncodingEnvironmentPtr eep_frame,
                          unsigned int symbol,
                          BiContextTypePtr ctx,
                          int ctx_offset,
                          unsigned int max_symbol);

void unary_exp_golomb_level_encode( EncodingEnvironmentPtr eep_dp,
                                    unsigned int symbol,
                                    BiContextTypePtr ctx);

void unary_exp_golomb_mv_encode(EncodingEnvironmentPtr eep_dp,
                                unsigned int symbol,
                                BiContextTypePtr ctx,
                                unsigned int max_bin);


void cabac_new_slice(void)
{
  last_dquant=0;
}


/*!
 ************************************************************************
 * \brief
 *    Check for available neighbouring blocks
 *    and set pointers in current macroblock
 ************************************************************************
 */
void CheckAvailabilityOfNeighborsCABAC(void)
{
  int mb_nr = img->current_mb_nr;
  Macroblock *currMB = &img->mb_data[mb_nr];
  PixelPos up, left;

  getNeighbour(mb_nr, -1,  0, IS_LUMA, &left);
  getNeighbour(mb_nr,  0, -1, IS_LUMA, &up);

  if (up.available)
    currMB->mb_available_up = &img->mb_data[up.mb_addr];
  else
    currMB->mb_available_up = NULL;

  if (left.available)
    currMB->mb_available_left = &img->mb_data[left.mb_addr];
  else
    currMB->mb_available_left = NULL;
}

/*!
 ************************************************************************
 * \brief
 *    Allocation of contexts models for the motion info
 *    used for arithmetic encoding
 ************************************************************************
 */
MotionInfoContexts* create_contexts_MotionInfo(void)
{
  MotionInfoContexts* enco_ctx;

  enco_ctx = (MotionInfoContexts*) calloc(1, sizeof(MotionInfoContexts) );
  if( enco_ctx == NULL )
    no_mem_exit("create_contexts_MotionInfo: enco_ctx");

  return enco_ctx;
}


/*!
 ************************************************************************
 * \brief
 *    Allocates of contexts models for the texture info
 *    used for arithmetic encoding
 ************************************************************************
 */
TextureInfoContexts* create_contexts_TextureInfo(void)
{
  TextureInfoContexts*  enco_ctx;

  enco_ctx = (TextureInfoContexts*) calloc(1, sizeof(TextureInfoContexts) );
  if( enco_ctx == NULL )
    no_mem_exit("create_contexts_TextureInfo: enco_ctx");

  return enco_ctx;
}




/*!
 ************************************************************************
 * \brief
 *    Frees the memory of the contexts models
 *    used for arithmetic encoding of the motion info.
 ************************************************************************
 */
void delete_contexts_MotionInfo(MotionInfoContexts *enco_ctx)
{
  if( enco_ctx == NULL )
    return;

  free( enco_ctx );

  return;
}

/*!
 ************************************************************************
 * \brief
 *    Frees the memory of the contexts models
 *    used for arithmetic encoding of the texture info.
 ************************************************************************
 */
void delete_contexts_TextureInfo(TextureInfoContexts *enco_ctx)
{
  if( enco_ctx == NULL )
    return;

  free( enco_ctx );

  return;
}


/*!
 ***************************************************************************
 * \brief
 *    This function is used to arithmetically encode the field
 *    mode info of a given MB  in the case of mb-based frame/field decision
 ***************************************************************************
 */
void writeFieldModeInfo_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  int a,b,act_ctx;
  MotionInfoContexts *ctx         = (img->currentSlice)->mot_ctx;
  Macroblock         *currMB      = &img->mb_data[img->current_mb_nr];
  int                mb_field     = se->value1;

  a = currMB->mbAvailA ? img->mb_data[currMB->mbAddrA].mb_field : 0;
  b = currMB->mbAvailB ? img->mb_data[currMB->mbAddrB].mb_field : 0;

  act_ctx = a + b;

  biari_encode_symbol(eep_dp, (signed short) (mb_field != 0),&ctx->mb_aff_contexts[act_ctx]);

  se->context = act_ctx;

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

/*!
***************************************************************************
* \brief
*    This function is used to arithmetically encode the mb_skip_flag.
***************************************************************************
*/
void writeMB_skip_flagInfo_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  int a,b,act_ctx;
  int bframe   = (img->type==B_SLICE);
  MotionInfoContexts *ctx         = (img->currentSlice)->mot_ctx;
  Macroblock         *currMB      = &img->mb_data[img->current_mb_nr];
  int                curr_mb_type = se->value1;

  if (bframe)
  {
    if (currMB->mb_available_up == NULL)
      b = 0;
    else
      b = (currMB->mb_available_up->skip_flag==0 ? 1 : 0);
    if (currMB->mb_available_left == NULL)
      a = 0;
    else
      a = (currMB->mb_available_left->skip_flag==0 ? 1 : 0);

    act_ctx = 7 + a + b;

    if (se->value1==0 && se->value2==0) // DIRECT mode, no coefficients
      biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][act_ctx]);
    else
      biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[2][act_ctx]);

    currMB->skip_flag = (se->value1==0 && se->value2==0)?1:0;
  }
  else
  {
    if (currMB->mb_available_up == NULL)
      b = 0;
    else
      b = (( (currMB->mb_available_up)->skip_flag == 0) ? 1 : 0 );
    if (currMB->mb_available_left == NULL)
      a = 0;
    else
      a = (( (currMB->mb_available_left)->skip_flag == 0) ? 1 : 0 );

    act_ctx = a + b;

    if (curr_mb_type==0) // SKIP
      biari_encode_symbol(eep_dp, 1,&ctx->mb_type_contexts[1][act_ctx]);
    else
      biari_encode_symbol(eep_dp, 0,&ctx->mb_type_contexts[1][act_ctx]);

    currMB->skip_flag = (curr_mb_type==0)?1:0;
  }
  se->context = act_ctx;

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

/*!
***************************************************************************
* \brief
*    This function is used to arithmetically encode the macroblock
*    intra_pred_size flag info of a given MB.
***************************************************************************
*/

void writeMB_transform_size_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  int a, b;
  int act_ctx = 0;
  int act_sym;

  MotionInfoContexts *ctx         = (img->currentSlice)->mot_ctx;
  Macroblock         *currMB      = &img->mb_data[img->current_mb_nr];


  b = (currMB->mb_available_up == NULL) ? 0 : currMB->mb_available_up->luma_transform_size_8x8_flag;
  a = (currMB->mb_available_left == NULL) ? 0 :currMB->mb_available_left->luma_transform_size_8x8_flag;

  act_ctx     = a + b;
  act_sym     = currMB->luma_transform_size_8x8_flag;
  se->context = act_ctx; // store context
  biari_encode_symbol(eep_dp, (signed short) (act_sym != 0), ctx->transform_size_contexts + act_ctx );

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

/*!
 ***************************************************************************
 * \brief
 *    This function is used to arithmetically encode the macroblock
 *    type info of a given MB.
 ***************************************************************************
 */

void writeMB_typeInfo_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  int a, b;
  int act_ctx = 0;
  int act_sym;
  signed short csym;
  int bframe   = (img->type==B_SLICE);
  int mode_sym = 0;
  int mode16x16;


  MotionInfoContexts *ctx         = (img->currentSlice)->mot_ctx;
  Macroblock         *currMB      = &img->mb_data[img->current_mb_nr];
  int                curr_mb_type = se->value1;

  if(img->type == I_SLICE)  // INTRA-frame
  {
    if (currMB->mb_available_up == NULL)
      b = 0;
    else
      b = ((currMB->mb_available_up->mb_type != I4MB &&  currMB->mb_available_up->mb_type != I8MB) ? 1 : 0 );

    if (currMB->mb_available_left == NULL)
      a = 0;
    else
      a = ((currMB->mb_available_left->mb_type != I4MB &&  currMB->mb_available_left->mb_type != I8MB) ? 1 : 0 );

    act_ctx     = a + b;
    act_sym     = curr_mb_type;
    se->context = act_ctx; // store context

    if (act_sym==0) // 4x4 Intra
    {
      biari_encode_symbol(eep_dp, 0, ctx->mb_type_contexts[0] + act_ctx );
    }
    else if( act_sym == 25 ) // PCM-MODE
    {
      biari_encode_symbol(eep_dp, 1, ctx->mb_type_contexts[0] + act_ctx );
      biari_encode_symbol_final(eep_dp, 1);
    }
    else // 16x16 Intra
    {
      biari_encode_symbol(eep_dp, 1, ctx->mb_type_contexts[0] + act_ctx );

      biari_encode_symbol_final(eep_dp, 0);

      mode_sym = act_sym-1; // Values in the range of 0...23
      act_ctx  = 4;
      act_sym  = mode_sym/12;
      biari_encode_symbol(eep_dp, (signed short) act_sym, ctx->mb_type_contexts[0] + act_ctx ); // coding of AC/no AC
      mode_sym = mode_sym % 12;
      act_sym  = mode_sym / 4; // coding of cbp: 0,1,2
      act_ctx  = 5;
      if (act_sym==0)
      {
        biari_encode_symbol(eep_dp, 0, ctx->mb_type_contexts[0] + act_ctx );
      }
      else
      {
        biari_encode_symbol(eep_dp, 1, ctx->mb_type_contexts[0] + act_ctx );
        act_ctx=6;
        biari_encode_symbol(eep_dp, (signed short) (act_sym!=1), ctx->mb_type_contexts[0] + act_ctx );
      }
      mode_sym = mode_sym & 0x03; // coding of I pred-mode: 0,1,2,3
      act_sym  = mode_sym >> 1;
      act_ctx  = 7;
      biari_encode_symbol(eep_dp, (signed short) act_sym, ctx->mb_type_contexts[0] + act_ctx );
      act_ctx  = 8;
      act_sym  = mode_sym & 0x01;
      biari_encode_symbol(eep_dp, (signed short) act_sym, ctx->mb_type_contexts[0] + act_ctx );
    }
  }
  else // INTER
  {

    if (bframe)
    {
      if (currMB->mb_available_up == NULL)
        b = 0;
      else
        b = ((currMB->mb_available_up->mb_type != 0) ? 1 : 0 );

      if (currMB->mb_available_left == NULL)
        a = 0;
      else
        a = ((currMB->mb_available_left->mb_type != 0) ? 1 : 0 );
      act_ctx = a + b;
      se->context = act_ctx; // store context
    }
    act_sym = curr_mb_type;

    if (act_sym>=(mode16x16=(bframe?24:7)))
    {
      mode_sym = act_sym-mode16x16;
      act_sym  = mode16x16; // 16x16 mode info
    }

    if (!bframe)
    {
      switch (act_sym)
      {
      case 0:
        break;
      case 1:
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][4]);
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][5]);
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][6]);
        break;
      case 2:
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][4]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[1][5]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[1][7]);
        break;
      case 3:
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][4]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[1][5]);
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][7]);
        break;
      case 4:
      case 5:
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][4]);
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][5]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[1][6]);
        break;
      case 6:
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[1][4]);
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[1][7]);
        break;
      case 7:
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[1][4]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[1][7]);
        break;
      default:
        printf ("Unsupported MB-MODE in writeMB_typeInfo_CABAC!\n");
        exit (1);
      }
    }
    else //===== B-FRAMES =====
    {
      if (act_sym==0)
      {
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[2][act_ctx]);
      }
      else if (act_sym<=2)
      {
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][act_ctx]);
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[2][4]);
        csym = (act_sym-1 != 0);
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
      }
      else if (act_sym<=10)
      {
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][act_ctx]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][4]);
        biari_encode_symbol (eep_dp, 0, &ctx->mb_type_contexts[2][5]);
        csym=(((act_sym-3)>>2)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
        csym=(((act_sym-3)>>1)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
        csym=((act_sym-3)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
      }
      else if (act_sym==11 || act_sym==22)
      {
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][act_ctx]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][4]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][5]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][6]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][6]);
        csym = (act_sym != 11);
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
      }
      else
      {
        if (act_sym > 22) act_sym--;
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][act_ctx]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][4]);
        biari_encode_symbol (eep_dp, 1, &ctx->mb_type_contexts[2][5]);
        csym=(((act_sym-12)>>3)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
        csym=(((act_sym-12)>>2)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
        csym=(((act_sym-12)>>1)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
        csym=((act_sym-12)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->mb_type_contexts[2][6]);
        if (act_sym >=22) act_sym++;
      }
    }

    if(act_sym==mode16x16) // additional info for 16x16 Intra-mode
    {
      if( mode_sym==25 )
      {
        biari_encode_symbol_final(eep_dp, 1 );
        dp->bitstream->write_flag = 1;
        se->len = (arienco_bits_written(eep_dp) - curr_len);
        CABAC_TRACE;
        return;
      }
      biari_encode_symbol_final(eep_dp, 0 );

      act_ctx = 8;
      act_sym = mode_sym/12;
      biari_encode_symbol(eep_dp, (signed short) act_sym, ctx->mb_type_contexts[1] + act_ctx ); // coding of AC/no AC
      mode_sym = mode_sym % 12;

      act_sym = mode_sym / 4; // coding of cbp: 0,1,2
      act_ctx = 9;
      if (act_sym==0)
      {
        biari_encode_symbol(eep_dp, 0, ctx->mb_type_contexts[1] + act_ctx );
      }
      else
      {
        biari_encode_symbol(eep_dp, 1, ctx->mb_type_contexts[1] + act_ctx );
        biari_encode_symbol(eep_dp, (signed short) (act_sym!=1), ctx->mb_type_contexts[1] + act_ctx );
      }

      mode_sym = mode_sym % 4; // coding of I pred-mode: 0,1,2,3
      act_ctx  = 10;
      act_sym  = mode_sym/2;
      biari_encode_symbol(eep_dp, (signed short) act_sym, ctx->mb_type_contexts[1] + act_ctx );
      act_sym  = mode_sym%2;
      biari_encode_symbol(eep_dp, (signed short) act_sym, ctx->mb_type_contexts[1] + act_ctx );
    }
  }
  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}


/*!
 ***************************************************************************
 * \brief
 *    This function is used to arithmetically encode the 8x8 block
 *    type info
 ***************************************************************************
 */
void writeB8_typeInfo_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  int act_ctx;
  int act_sym;
  signed short csym;
  int bframe=(img->type==B_SLICE);

  MotionInfoContexts *ctx = (img->currentSlice)->mot_ctx;

  act_sym = se->value1;
  act_ctx = 0;

  if (!bframe)
  {
    switch (act_sym)
    {
    case 0:
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[0][1]);
      break;
    case 1:
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[0][1]);
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[0][3]);
      break;
    case 2:
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[0][1]);
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[0][3]);
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[0][4]);
      break;
    case 3:
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[0][1]);
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[0][3]);
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[0][4]);
      break;
    }
  }
  else //===== B-FRAME =====
  {
    if (act_sym==0)
    {
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[1][0]);
      dp->bitstream->write_flag = 1;
      se->len = (arienco_bits_written(eep_dp) - curr_len);
      CABAC_TRACE;
      return;
    }
    else
    {
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[1][0]);
      act_sym--;
    }
    if (act_sym<2)
    {
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[1][1]);
      biari_encode_symbol (eep_dp, (signed short) (act_sym!=0), &ctx->b8_type_contexts[1][3]);
    }
    else if (act_sym<6)
    {
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[1][1]);
      biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[1][2]);
      csym=(((act_sym-2)>>1)&0x01) != 0;
      biari_encode_symbol (eep_dp, csym, &ctx->b8_type_contexts[1][3]);
      csym=((act_sym-2)&0x01) != 0;
      biari_encode_symbol (eep_dp, csym, &ctx->b8_type_contexts[1][3]);
    }
    else
    {
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[1][1]);
      biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[1][2]);
      csym=(((act_sym-6)>>2)&0x01);
      if (csym)
      {
        biari_encode_symbol (eep_dp, 1, &ctx->b8_type_contexts[1][3]);
        csym=((act_sym-6)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->b8_type_contexts[1][3]);
      }
      else
      {
        biari_encode_symbol (eep_dp, 0, &ctx->b8_type_contexts[1][3]);
        csym=(((act_sym-6)>>1)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->b8_type_contexts[1][3]);
        csym=((act_sym-6)&0x01) != 0;
        biari_encode_symbol (eep_dp, csym, &ctx->b8_type_contexts[1][3]);
      }
    }
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

/*!
 ****************************************************************************
 * \brief
 *    This function is used to arithmetically encode a pair of
 *    intra prediction modes of a given MB.
 ****************************************************************************
 */
void writeIntraPredMode_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  TextureInfoContexts *ctx = img->currentSlice->tex_ctx;

  // use_most_probable_mode
  if (se->value1 == -1)
    biari_encode_symbol(eep_dp, 1, ctx->ipr_contexts);
  else
  {
    biari_encode_symbol(eep_dp, 0, ctx->ipr_contexts);

    // remaining_mode_selector
    biari_encode_symbol(eep_dp,(signed short)( se->value1 & 0x1    ), ctx->ipr_contexts+1);
    biari_encode_symbol(eep_dp,(signed short)((se->value1 & 0x2)>>1), ctx->ipr_contexts+1);
    biari_encode_symbol(eep_dp,(signed short)((se->value1 & 0x4)>>2), ctx->ipr_contexts+1);
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

/*!
 ****************************************************************************
 * \brief
 *    This function is used to arithmetically encode the reference
 *    parameter of a given MB.
 ****************************************************************************
 */
void writeRefFrame_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  int   mb_nr = img->current_mb_nr;
  MotionInfoContexts  *ctx    = img->currentSlice->mot_ctx;
  Macroblock          *currMB = &img->mb_data[mb_nr];
  int                 addctx  = 0;

  int   a, b;
  int   act_ctx;
  int   act_sym;
  signed char** refframe_array = enc_picture->ref_idx[se->value2];

  int bslice = (img->type==B_SLICE);

  int   b8a, b8b;

  PixelPos block_a, block_b;

  getLuma4x4Neighbour(mb_nr, (img->subblock_x << 2) - 1, (img->subblock_y << 2), &block_a);
  getLuma4x4Neighbour(mb_nr, (img->subblock_x << 2),     (img->subblock_y << 2) - 1, &block_b);

  b8a=((block_a.x >> 1) & 0x01)+2*((block_a.y >> 1) & 0x01);
  b8b=((block_b.x >> 1) & 0x01)+2*((block_b.y >> 1) & 0x01);


  if (!block_b.available)
    b=0;
  //else if (IS_DIRECT(&img->mb_data[block_b.mb_addr]) || (img->mb_data[block_b.mb_addr].b8mode[b8b]==0 && bslice))
  else if ((IS_DIRECT(&img->mb_data[block_b.mb_addr]) && !giRDOpt_B8OnlyFlag) || (img->mb_data[block_b.mb_addr].b8mode[b8b]==0 && bslice))
    b=0;
  else
  {
    if (img->MbaffFrameFlag && (currMB->mb_field == 0) && (img->mb_data[block_b.mb_addr].mb_field == 1))
      b = (refframe_array[block_b.pos_y][block_b.pos_x] > 1 ? 1 : 0);
    else
      b = (refframe_array[block_b.pos_y][block_b.pos_x] > 0 ? 1 : 0);
  }

  if (!block_a.available)
    a=0;
  // else if (IS_DIRECT(&img->mb_data[block_a.mb_addr]) || (img->mb_data[block_a.mb_addr].b8mode[b8a]==0 && bslice))
  else if ((IS_DIRECT(&img->mb_data[block_a.mb_addr]) && !giRDOpt_B8OnlyFlag) || (img->mb_data[block_a.mb_addr].b8mode[b8a]==0 && bslice))
    a=0;
  else
  {
    if (img->MbaffFrameFlag && (currMB->mb_field == 0) && (img->mb_data[block_a.mb_addr].mb_field == 1))
      a = (refframe_array[block_a.pos_y][block_a.pos_x] > 1 ? 1 : 0);
    else
      a = (refframe_array[block_a.pos_y][block_a.pos_x] > 0 ? 1 : 0);
  }

  act_ctx     = a + 2*b;
  se->context = act_ctx; // store context
  act_sym     = se->value1;

  if (act_sym==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx->ref_no_contexts[addctx] + act_ctx );
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx->ref_no_contexts[addctx] + act_ctx);
    act_sym--;
    act_ctx=4;
    unary_bin_encode(eep_dp, act_sym,ctx->ref_no_contexts[addctx]+act_ctx,1);
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

/*!
 ****************************************************************************
 * \brief
 *    This function is used to arithmetically encode the coded
 *    block pattern of a given delta quant.
 ****************************************************************************
 */
void writeDquant_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);

  MotionInfoContexts *ctx = img->currentSlice->mot_ctx;

  int act_ctx;
  int act_sym;
  int dquant = se->value1;
  int sign=0;

  Macroblock     *currMB       = &img->mb_data[img->current_mb_nr];

  last_dquant=currMB->prev_delta_qp;

  if (dquant <= 0)
    sign = 1;
  act_sym = iabs(dquant) << 1;

  act_sym += sign;
  act_sym --;

  act_ctx = ( (last_dquant != 0) ? 1 : 0);

  if (act_sym==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx->delta_qp_contexts + act_ctx );
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx->delta_qp_contexts + act_ctx);
    act_ctx=2;
    act_sym--;
    unary_bin_encode(eep_dp, act_sym,ctx->delta_qp_contexts+act_ctx,1);
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

/*!
 ****************************************************************************
 * \brief
 *    This function is used to arithmetically encode the motion
 *    vector data of a B-frame MB.
 ****************************************************************************
 */
void writeMVD_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  int i = (img->subblock_x << 2);
  int j = (img->subblock_y << 2);
  int a, b;
  int act_ctx;
  int act_sym;
  int mv_pred_res;
  int mv_local_err;
  int mv_sign;
  int list_idx = se->value2 & 0x01;
  int k = (se->value2>>1); // MVD component
  int mb_nr = img->current_mb_nr;

  PixelPos block_a, block_b;

  MotionInfoContexts  *ctx    = img->currentSlice->mot_ctx;
  Macroblock          *currMB = &img->mb_data[mb_nr];

  getLuma4x4Neighbour(mb_nr, i - 1, j, &block_a);
  getLuma4x4Neighbour(mb_nr, i    , j - 1, &block_b);

  if (block_b.available)
  {
    b = iabs(img->mb_data[block_b.mb_addr].mvd[list_idx][block_b.y][block_b.x][k]);
    if (img->MbaffFrameFlag && (k==1))
    {
      if ((currMB->mb_field==0) && (img->mb_data[block_b.mb_addr].mb_field==1))
        b *= 2;
      else if ((currMB->mb_field==1) && (img->mb_data[block_b.mb_addr].mb_field==0))
        b /= 2;
    }
  }
  else
    b=0;

  if (block_a.available)
  {
    a = iabs(img->mb_data[block_a.mb_addr].mvd[list_idx][block_a.y][block_a.x][k]);
    if (img->MbaffFrameFlag && (k==1))
    {
      if ((currMB->mb_field==0) && (img->mb_data[block_a.mb_addr].mb_field==1))
        a *= 2;
      else if ((currMB->mb_field==1) && (img->mb_data[block_a.mb_addr].mb_field==0))
        a /= 2;
    }
  }
  else
    a = 0;

  if ((mv_local_err=a+b)<3)
    act_ctx = 5*k;
  else
  {
    if (mv_local_err>32)
      act_ctx=5*k+3;
    else
      act_ctx=5*k+2;
  }

  mv_pred_res = se->value1;
  se->context = act_ctx;

  act_sym = iabs(mv_pred_res);

  if (act_sym == 0)
    biari_encode_symbol(eep_dp, 0, &ctx->mv_res_contexts[0][act_ctx] );
  else
  {
    biari_encode_symbol(eep_dp, 1, &ctx->mv_res_contexts[0][act_ctx] );
    act_sym--;
    act_ctx=5*k;
    unary_exp_golomb_mv_encode(eep_dp,act_sym,ctx->mv_res_contexts[1]+act_ctx,3);
    mv_sign = (mv_pred_res<0) ? 1: 0;
    biari_encode_symbol_eq_prob(eep_dp, (signed short) mv_sign);
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}


/*!
 ****************************************************************************
 * \brief
 *    This function is used to arithmetically encode the chroma
 *    intra prediction mode of an 8x8 block
 ****************************************************************************
 */
void writeCIPredMode_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  TextureInfoContexts *ctx     = img->currentSlice->tex_ctx;
  Macroblock          *currMB  = &img->mb_data[img->current_mb_nr];
  int                 act_ctx,a,b;
  int                 act_sym  = se->value1;

  if (currMB->mb_available_up == NULL) b = 0;
  else  b = ( ((currMB->mb_available_up)->c_ipred_mode != 0) ? 1 : 0);

  if (currMB->mb_available_left == NULL) a = 0;
  else  a = ( ((currMB->mb_available_left)->c_ipred_mode != 0) ? 1 : 0);

  act_ctx = a+b;

  if (act_sym==0)
    biari_encode_symbol(eep_dp, 0, ctx->cipr_contexts + act_ctx );
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx->cipr_contexts + act_ctx );
    unary_bin_max_encode(eep_dp,(unsigned int) (act_sym-1),ctx->cipr_contexts+3,0,2);
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}


/*!
 ****************************************************************************
 * \brief
 *    This function is used to arithmetically encode the coded
 *    block pattern of an 8x8 block
 ****************************************************************************
 */
void writeCBP_BIT_CABAC (int b8, int bit, int cbp, Macroblock* currMB, int inter, EncodingEnvironmentPtr eep_dp)
{
  PixelPos block_a;
  int a, b;

  int mb_x=(b8 & 0x01)<<1;
  int mb_y=(b8 >> 1)<<1;

  if (mb_y == 0)
  {
    if (currMB->mb_available_up == NULL)
      b = 0;
    else
    {
      if((currMB->mb_available_up)->mb_type==IPCM)
        b=0;
      else
        b = (( ((currMB->mb_available_up)->cbp & (1<<(2+(mb_x>>1)))) == 0) ? 1 : 0);   //VG-ADD
    }

  }
  else
    b = ( ((cbp & (1<<(mb_x/2))) == 0) ? 1: 0);

  if (mb_x == 0)
  {
    getLuma4x4Neighbour(img->current_mb_nr, (mb_x << 2) - 1, (mb_y << 2), &block_a);
    if (block_a.available)
    {
      {
        if(img->mb_data[block_a.mb_addr].mb_type==IPCM)
          a=0;
        else
          a = (( (img->mb_data[block_a.mb_addr].cbp & (1<<(2*(block_a.y>>1)+1))) == 0) ? 1 : 0); //VG-ADD
      }

    }
    else
      a=0;
  }
  else
    a = ( ((cbp & (1<<mb_y)) == 0) ? 1: 0);

  //===== WRITE BIT =====
  biari_encode_symbol (eep_dp, (signed short) bit,
    img->currentSlice->tex_ctx->cbp_contexts[0] + a+2*b);
}

/*!
****************************************************************************
* \brief
*    This function is used to arithmetically encode the coded
*    block pattern of a macroblock
****************************************************************************
*/
void writeCBP_CABAC(SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  TextureInfoContexts *ctx = img->currentSlice->tex_ctx;
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];

  int a, b;
  int curr_cbp_ctx, curr_cbp_idx;
  int cbp = se->value1; // symbol to encode
  int cbp_bit;
  int b8;

  for (b8=0; b8<4; b8++)
  {
    curr_cbp_idx = (currMB->b8mode[b8] == IBLOCK ? 0 : 1);
    writeCBP_BIT_CABAC (b8, cbp&(1<<b8), cbp, currMB, curr_cbp_idx, eep_dp);
  }

  if (img->yuv_format != YUV400)
  {
    // coding of chroma part
    b = 0;
    if (currMB->mb_available_up != NULL)
    {
      if((currMB->mb_available_up)->mb_type==IPCM)
        b=1;
      else
        b = ((currMB->mb_available_up)->cbp > 15) ? 1 : 0;
    }


    a = 0;
    if (currMB->mb_available_left != NULL)
    {
      if((currMB->mb_available_left)->mb_type==IPCM)
        a=1;
      else
        a = ((currMB->mb_available_left)->cbp > 15) ? 1 : 0;
    }

    curr_cbp_ctx = a+2*b;
    cbp_bit = (cbp > 15 ) ? 1 : 0;
    biari_encode_symbol(eep_dp, (signed short) cbp_bit, ctx->cbp_contexts[1] + curr_cbp_ctx );

    if (cbp > 15)
    {
      b = 0;
      if (currMB->mb_available_up != NULL)
      {
        if((currMB->mb_available_up)->mb_type==IPCM)
          b=1;
        else
          if ((currMB->mb_available_up)->cbp > 15)
            b = (( ((currMB->mb_available_up)->cbp >> 4) == 2) ? 1 : 0);
      }


      a = 0;
      if (currMB->mb_available_left != NULL)
      {
        if((currMB->mb_available_left)->mb_type==IPCM)
          a=1;
        else
          if ((currMB->mb_available_left)->cbp > 15)
            a = (( ((currMB->mb_available_left)->cbp >> 4) == 2) ? 1 : 0);
      }

      curr_cbp_ctx = a+2*b;
      cbp_bit = ((cbp>>4) == 2) ? 1 : 0;
      biari_encode_symbol(eep_dp, (signed short) cbp_bit, ctx->cbp_contexts[2] + curr_cbp_ctx );
    }
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}

static const int maxpos       [] = {16, 15, 64, 32, 32, 16,  4, 15,  8, 16};
static const int c1isdc       [] = { 1,  0,  1,  1,  1,  1,  1,  0,  1,  1};

static const int type2ctx_bcbp[] = { 0,  1,  2,  2,  3,  4,  5,  6,  5,  5}; // 7
static const int type2ctx_map [] = { 0,  1,  2,  3,  4,  5,  6,  7,  6,  6}; // 8
static const int type2ctx_last[] = { 0,  1,  2,  3,  4,  5,  6,  7,  6,  6}; // 8
static const int type2ctx_one [] = { 0,  1,  2,  3,  3,  4,  5,  6,  5,  5}; // 7
static const int type2ctx_abs [] = { 0,  1,  2,  3,  3,  4,  5,  6,  5,  5}; // 7
static const int max_c2       [] = { 4,  4,  4,  4,  4,  4,  3,  4,  3,  3}; // 9



/*!
 ****************************************************************************
 * \brief
 *    Write CBP4-BIT
 ****************************************************************************
 */
void write_and_store_CBP_block_bit (Macroblock* currMB, EncodingEnvironmentPtr eep_dp, int type, int cbp_bit)
{
#define BIT_SET(x,n)  ((int)(((x)&((int64)1<<(n)))>>(n)))

  int y_ac        = (type==LUMA_16AC || type==LUMA_8x8 || type==LUMA_8x4 || type==LUMA_4x8 || type==LUMA_4x4);
  int y_dc        = (type==LUMA_16DC);
  int u_ac        = (type==CHROMA_AC && !img->is_v_block);
  int v_ac        = (type==CHROMA_AC &&  img->is_v_block);
  int chroma_dc   = (type==CHROMA_DC || type==CHROMA_DC_2x4 || type==CHROMA_DC_4x4);
  int u_dc        = (chroma_dc && !img->is_v_block);
  int v_dc        = (chroma_dc &&  img->is_v_block);
  int j           = (y_ac || u_ac || v_ac ? img->subblock_y : 0);
  int i           = (y_ac || u_ac || v_ac ? img->subblock_x : 0);
  int bit         = (y_dc ? 0 : y_ac ? 1 : u_dc ? 17 : v_dc ? 18 : u_ac ? 19 : 23);
  int default_bit = (img->is_intra_block ? 1 : 0);
  int upper_bit   = default_bit;
  int left_bit    = default_bit;
  int ctx;

  int bit_pos_a   = 0;
  int bit_pos_b   = 0;

  PixelPos block_a, block_b;

  if (y_ac || y_dc)
  {
    getLuma4x4Neighbour(img->current_mb_nr, (i<<2) - 1, (j << 2),     &block_a);
    getLuma4x4Neighbour(img->current_mb_nr, (i<<2),     (j << 2) -1, &block_b);
    if (y_ac)
    {
      if (block_a.available)
        bit_pos_a = 4*block_a.y + block_a.x;
      if (block_b.available)
        bit_pos_b = 4*block_b.y + block_b.x;
    }
  }
  else
  {
    getChroma4x4Neighbour(img->current_mb_nr, (i<<2)-1, (j<<2),  &block_a);
    getChroma4x4Neighbour(img->current_mb_nr, (i<<2),   (j<<2)-1,&block_b);
    if (u_ac||v_ac)
    {
      if (block_a.available)
        bit_pos_a = 4*block_a.y + block_a.x;
      if (block_b.available)
        bit_pos_b = 4*block_b.y + block_b.x;
    }
  }

  bit = (y_dc ? 0 : y_ac ? 1+4*j+i : u_dc ? 17 : v_dc ? 18 : u_ac ? 19+4*j+i : 35+4*j+i);
  //--- set bits for current block ---
  if (cbp_bit)
  {
    if (type==LUMA_8x8)
    {
      currMB->cbp_bits   |= ((int64)1<< bit   );
      currMB->cbp_bits   |= ((int64)1<<(bit+1));
      currMB->cbp_bits   |= ((int64)1<<(bit+4));
      currMB->cbp_bits   |= ((int64)1<<(bit+5));
    }
    else if (type==LUMA_8x4)
    {
      currMB->cbp_bits   |= ((int64)1<< bit   );
      currMB->cbp_bits   |= ((int64)1<<(bit+1));
    }
    else if (type==LUMA_4x8)
    {
      currMB->cbp_bits   |= ((int64)1<< bit   );
      currMB->cbp_bits   |= ((int64)1<<(bit+4));
    }
    else
    {
      currMB->cbp_bits   |= ((int64)1<<bit);
    }
  }

  bit = (y_dc ? 0 : y_ac ? 1 : u_dc ? 17 : v_dc ? 18 : u_ac ? 19 : 35);
  if (type!=LUMA_8x8)
  {
    if (block_b.available)
    {
      if(img->mb_data[block_b.mb_addr].mb_type==IPCM)
        upper_bit=1;
      else
        upper_bit = BIT_SET(img->mb_data[block_b.mb_addr].cbp_bits,bit+bit_pos_b);
    }


    if (block_a.available)
    {
      if(img->mb_data[block_a.mb_addr].mb_type==IPCM)
        left_bit=1;
      else
        left_bit = BIT_SET(img->mb_data[block_a.mb_addr].cbp_bits,bit+bit_pos_a);
    }

    ctx = 2*upper_bit+left_bit;

    //===== encode symbol =====
    biari_encode_symbol (eep_dp, (short)cbp_bit, img->currentSlice->tex_ctx->bcbp_contexts[type2ctx_bcbp[type]]+ctx);
  }
}




//===== position -> ctx for MAP =====
//--- zig-zag scan ----
static const int  pos2ctx_map8x8 [] = { 0,  1,  2,  3,  4,  5,  5,  4,  4,  3,  3,  4,  4,  4,  5,  5,
                                        4,  4,  4,  4,  3,  3,  6,  7,  7,  7,  8,  9, 10,  9,  8,  7,
                                        7,  6, 11, 12, 13, 11,  6,  7,  8,  9, 14, 10,  9,  8,  6, 11,
                                       12, 13, 11,  6,  9, 14, 10,  9, 11, 12, 13, 11 ,14, 10, 12, 14}; // 15 CTX
static const int  pos2ctx_map8x4 [] = { 0,  1,  2,  3,  4,  5,  7,  8,  9, 10, 11,  9,  8,  6,  7,  8,
                                        9, 10, 11,  9,  8,  6, 12,  8,  9, 10, 11,  9, 13, 13, 14, 14}; // 15 CTX
static const int  pos2ctx_map4x4 [] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14}; // 15 CTX
static const int  pos2ctx_map2x4c[] = { 0,  0,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2}; // 15 CTX
static const int  pos2ctx_map4x4c[] = { 0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2}; // 15 CTX
static const int* pos2ctx_map    [] = {pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map8x8, pos2ctx_map8x4,
                                       pos2ctx_map8x4, pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map4x4,
                                       pos2ctx_map2x4c, pos2ctx_map4x4c};

//--- interlace scan ----
//Taken from ABT
static const int  pos2ctx_map8x8i[] = { 0,  1,  1,  2,  2,  3,  3,  4,  5,  6,  7,  7,  7,  8,  4,  5,
                                        6,  9, 10, 10,  8, 11, 12, 11,  9,  9, 10, 10,  8, 11, 12, 11,
                                        9,  9, 10, 10,  8, 11, 12, 11,  9,  9, 10, 10,  8, 13, 13,  9,
                                        9, 10, 10,  8, 13, 13,  9,  9, 10, 10, 14, 14, 14, 14, 14, 14}; // 15 CTX

static const int  pos2ctx_map8x4i[] = { 0,  1,  2,  3,  4,  5,  6,  3,  4,  5,  6,  3,  4,  7,  6,  8,
                                        9,  7,  6,  8,  9, 10, 11, 12, 12, 10, 11, 13, 13, 14, 14, 14}; // 15 CTX
static const int  pos2ctx_map4x8i[] = { 0,  1,  1,  1,  2,  3,  3,  4,  4,  4,  5,  6,  2,  7,  7,  8,
                                        8,  8,  5,  6,  9, 10, 10, 11, 11, 11, 12, 13, 13, 14, 14, 14}; // 15 CTX
static const int* pos2ctx_map_int[] = {pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map8x8i,pos2ctx_map8x4i,
                                       pos2ctx_map4x8i,pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map4x4,
                                       pos2ctx_map2x4c, pos2ctx_map4x4c};


//===== position -> ctx for LAST =====
static const int  pos2ctx_last8x8 [] = { 0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                                         2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                                         3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,
                                         5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8}; //  9 CTX
static const int  pos2ctx_last8x4 [] = { 0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,
                                         3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8}; //  9 CTX
static const int  pos2ctx_last4x4 [] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15}; // 15 CTX
static const int  pos2ctx_last2x4c[] = { 0,  0,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2}; // 15 CTX
static const int  pos2ctx_last4x4c[] = { 0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2}; // 15 CTX
static const int* pos2ctx_last    [] = {pos2ctx_last4x4, pos2ctx_last4x4, pos2ctx_last8x8, pos2ctx_last8x4,
                                        pos2ctx_last8x4, pos2ctx_last4x4, pos2ctx_last4x4, pos2ctx_last4x4,
                                        pos2ctx_last2x4c, pos2ctx_last4x4c};




/*!
****************************************************************************
* \brief
*    Write Significance MAP
****************************************************************************
*/
void write_significance_map (Macroblock* currMB, EncodingEnvironmentPtr eep_dp, int type, int coeff[], int coeff_ctr)
{
  int   k;
  unsigned short sig, last;
  int   k0 = 0;
  int   k1 = maxpos[type]-1;
  TextureInfoContexts*  tex_ctx = img->currentSlice->tex_ctx;

  int               fld       = ( img->structure!=FRAME || currMB->mb_field );
  BiContextTypePtr  map_ctx   = ( fld
    ? tex_ctx->fld_map_contexts[type2ctx_map [type]]
    : tex_ctx->map_contexts[type2ctx_map [type]] );
  BiContextTypePtr  last_ctx  = ( fld
    ? tex_ctx->fld_last_contexts[type2ctx_last[type]]
    : tex_ctx->last_contexts[type2ctx_last[type]] );

  if (!c1isdc[type])
  {
    k0++; k1++; coeff--;
  }

  if (!fld)
  {
    for (k=k0; k<k1; k++) // if last coeff is reached, it has to be significant
    {
      sig   = (coeff[k] != 0);
      biari_encode_symbol  (eep_dp, sig,  map_ctx + pos2ctx_map  [type][k]);
      if (sig)
      {
        last = (--coeff_ctr == 0);

        biari_encode_symbol(eep_dp, last, last_ctx + pos2ctx_last[type][k]);
        if (last) return;
      }
    }
    return;
  }
  else
  {
    for (k=k0; k<k1; k++) // if last coeff is reached, it has to be significant
    {
      sig   = (coeff[k] != 0);

      biari_encode_symbol  (eep_dp, sig,  map_ctx + pos2ctx_map_int [type][k]);
      if (sig)
      {
        last = (--coeff_ctr == 0);

        biari_encode_symbol(eep_dp, last, last_ctx + pos2ctx_last[type][k]);
        if (last) return;
      }
    }
  }
}


/*!
 ****************************************************************************
 * \brief
 *    Write Levels
 ****************************************************************************
 */
void write_significant_coefficients (Macroblock* currMB, EncodingEnvironmentPtr eep_dp, int type, int coeff[])
{
  int   i;
  int   absLevel;
  int   ctx;
  short sign;
  short greater_one;
  int   c1 = 1;
  int   c2 = 0;

  for (i=maxpos[type]-1; i>=0; i--)
  {
    if (coeff[i]!=0)
    {
      if (coeff[i]>0) {absLevel =  coeff[i];  sign = 0;}
      else            {absLevel = -coeff[i];  sign = 1;}

      greater_one = (absLevel>1);

      //--- if coefficient is one ---
      ctx = imin(c1,4);
      biari_encode_symbol (eep_dp, greater_one, img->currentSlice->tex_ctx->one_contexts[type2ctx_one[type]] + ctx);

      if (greater_one)
      {
        ctx = imin(c2, max_c2[type]);
        unary_exp_golomb_level_encode(eep_dp, absLevel-2, img->currentSlice->tex_ctx->abs_contexts[type2ctx_abs[type]] + ctx);
        c1 = 0;
        c2++;
      }
      else if (c1)
      {
        c1++;
      }
      biari_encode_symbol_eq_prob (eep_dp, sign);
    }
  }
}



/*!
 ****************************************************************************
 * \brief
 *    Write Block-Transform Coefficients
 ****************************************************************************
 */
void writeRunLevel_CABAC (SyntaxElement *se, DataPartition *dp)
{
  EncodingEnvironmentPtr eep_dp = &(dp->ee_cabac);
  int curr_len = arienco_bits_written(eep_dp);
  static int  coeff[64];
  static int  coeff_ctr = 0;
  static int  pos       = 0;

  //--- accumulate run-level information ---
  if (se->value1 != 0)
  {
    pos += se->value2;
    coeff[pos++] = se->value1;
    coeff_ctr++;
    //return;
  }
  else
  {
    Macroblock* currMB    = &img->mb_data[img->current_mb_nr];
    //===== encode CBP-BIT =====
    if (coeff_ctr>0)
    {
      write_and_store_CBP_block_bit  (currMB, eep_dp, se->context, 1);
      //===== encode significance map =====
      write_significance_map         (currMB, eep_dp, se->context, coeff, coeff_ctr);
      //===== encode significant coefficients =====
      write_significant_coefficients (currMB, eep_dp, se->context, coeff);
    }
    else
      write_and_store_CBP_block_bit  (currMB, eep_dp, se->context, 0);

    //--- reset counters ---
    pos = coeff_ctr = 0;
    memset(coeff, 0 , 64 * sizeof(int));
  }

  dp->bitstream->write_flag = 1;
  se->len = (arienco_bits_written(eep_dp) - curr_len);
  CABAC_TRACE;
  return;
}




/*!
 ************************************************************************
 * \brief
 *    Unary binarization and encoding of a symbol by using
 *    one or two distinct models for the first two and all
 *    remaining bins
*
************************************************************************/
void unary_bin_encode(EncodingEnvironmentPtr eep_dp,
                      unsigned int symbol,
                      BiContextTypePtr ctx,
                      int ctx_offset)
{
  unsigned int l;
  BiContextTypePtr ictx;

  if (symbol==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx );
    return;
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx );
    l = symbol;
    ictx = ctx+ctx_offset;
    while ((--l)>0)
      biari_encode_symbol(eep_dp, 1, ictx);
    biari_encode_symbol(eep_dp, 0, ictx);
  }
  return;
}

/*!
 ************************************************************************
 * \brief
 *    Unary binarization and encoding of a symbol by using
 *    one or two distinct models for the first two and all
 *    remaining bins; no terminating "0" for max_symbol
 *    (finite symbol alphabet)
 ************************************************************************
 */
void unary_bin_max_encode(EncodingEnvironmentPtr eep_dp,
                          unsigned int symbol,
                          BiContextTypePtr ctx,
                          int ctx_offset,
                          unsigned int max_symbol)
{
  unsigned int l;
  BiContextTypePtr ictx;

  if (symbol==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx );
    return;
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx );
    l=symbol;
    ictx=ctx+ctx_offset;
    while ((--l)>0)
      biari_encode_symbol(eep_dp, 1, ictx);
    if (symbol<max_symbol)
      biari_encode_symbol(eep_dp, 0, ictx);
  }
  return;
}



/*!
 ************************************************************************
 * \brief
 *    Exp Golomb binarization and encoding
 ************************************************************************
 */
void exp_golomb_encode_eq_prob( EncodingEnvironmentPtr eep_dp,
                                unsigned int symbol,
                                int k)
{
  while(1)
  {
    if (symbol >= (unsigned int)(1<<k))
    {
      biari_encode_symbol_eq_prob(eep_dp, 1);   //first unary part
      symbol = symbol - (1<<k);
      k++;
    }
    else
    {
      biari_encode_symbol_eq_prob(eep_dp, 0);   //now terminated zero of unary part
      while (k--)                               //next binary part
        biari_encode_symbol_eq_prob(eep_dp, (signed short)((symbol>>k)&1));
      break;
    }
  }

  return;
}

/*!
 ************************************************************************
 * \brief
 *    Exp-Golomb for Level Encoding
*
************************************************************************/
void unary_exp_golomb_level_encode( EncodingEnvironmentPtr eep_dp,
                                    unsigned int symbol,
                                    BiContextTypePtr ctx)
{
  unsigned int l,k;
  unsigned int exp_start = 13; // 15-2 : 0,1 level decision always sent

  if (symbol==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx );
    return;
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx );
    l=symbol;
    k=1;
    while (((--l)>0) && (++k <= exp_start))
      biari_encode_symbol(eep_dp, 1, ctx);
    if (symbol < exp_start) biari_encode_symbol(eep_dp, 0, ctx);
    else exp_golomb_encode_eq_prob(eep_dp,symbol-exp_start,0);
  }
  return;
}



/*!
 ************************************************************************
 * \brief
 *    Exp-Golomb for MV Encoding
*
************************************************************************/
void unary_exp_golomb_mv_encode(EncodingEnvironmentPtr eep_dp,
                                unsigned int symbol,
                                BiContextTypePtr ctx,
                                unsigned int max_bin)
{
  unsigned int l,k;
  unsigned int bin=1;
  BiContextTypePtr ictx=ctx;
  unsigned int exp_start = 8; // 9-1 : 0 mvd decision always sent

  if (symbol==0)
  {
    biari_encode_symbol(eep_dp, 0, ictx );
    return;
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ictx );
    l=symbol;
    k=1;
    ictx++;
    while (((--l)>0) && (++k <= exp_start))
    {
      biari_encode_symbol(eep_dp, 1, ictx  );
      if ((++bin)==2) ictx++;
      if (bin==max_bin) ictx++;
    }
    if (symbol < exp_start) biari_encode_symbol(eep_dp, 0, ictx);
    else exp_golomb_encode_eq_prob(eep_dp,symbol-exp_start,3);
  }
  return;
}

