
/*!
 *************************************************************************************
 * \file biariencode.c
 *
 * \brief
 *    Routines for binary arithmetic encoding
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Detlev Marpe                    <marpe@hhi.de>
 *    - Gabi Blaettermann               <blaetter@hhi.de>
 *************************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "global.h"
#include "biariencode.h"

int binCount = 0;

/*!
 ************************************************************************
 * Macro for writing bytes of code
 ***********************************************************************
 */

#define put_byte() { \
                     Ecodestrm[(*Ecodestrm_len)++] = Ebuffer; \
                     Ebits_to_go = 8; \
                     while (eep->C > 7) { \
                       eep->C-=8; \
                       eep->E++; \
                     } \
                    }

#define put_one_bit(b) { \
                         Ebuffer <<= 1; Ebuffer |= (b); \
                         if (--Ebits_to_go == 0) \
                           put_byte(); \
                       }

#define put_one_bit_plus_outstanding(b) { \
                                          put_one_bit(b); \
                                          while (Ebits_to_follow > 0) \
                                          { \
                                            Ebits_to_follow--; \
                                            put_one_bit(!(b)); \
                                          } \
                                         }

int pic_bin_count;

void reset_pic_bin_count(void)
{
  pic_bin_count = 0;
}

int get_pic_bin_count(void)
{
  return pic_bin_count;
}



/*!
 ************************************************************************
 * \brief
 *    Allocates memory for the EncodingEnvironment struct
 ************************************************************************
 */
EncodingEnvironmentPtr arienco_create_encoding_environment(void)
{
  EncodingEnvironmentPtr eep;

  if ( (eep = (EncodingEnvironmentPtr) calloc(1,sizeof(EncodingEnvironment))) == NULL)
    no_mem_exit("arienco_create_encoding_environment: eep");

  return eep;
}



/*!
 ************************************************************************
 * \brief
 *    Frees memory of the EncodingEnvironment struct
 ************************************************************************
 */
void arienco_delete_encoding_environment(EncodingEnvironmentPtr eep)
{
  if (eep == NULL)
  {
    snprintf(errortext, ET_SIZE, "Error freeing eep (NULL pointer)");
    error (errortext, 200);
  }
  else
    free(eep);
}



/*!
 ************************************************************************
 * \brief
 *    Initializes the EncodingEnvironment for the arithmetic coder
 ************************************************************************
 */
void arienco_start_encoding(EncodingEnvironmentPtr eep,
                            unsigned char *code_buffer,
                            int *code_len )
{
  Elow = 0;
  Ebits_to_follow = 0;
  Ebuffer = 0;
  Ebits_to_go = 9; // to swallow first redundant bit

  Ecodestrm = code_buffer;
  Ecodestrm_len = code_len;

  Erange = HALF-2;

  eep->C = 0;
  eep->E = 0;

}

/*!
 ************************************************************************
 * \brief
 *    Returns the number of currently written bits
 ************************************************************************
 */
int arienco_bits_written(EncodingEnvironmentPtr eep)
{
   return (8 * (*Ecodestrm_len) + Ebits_to_follow + 8  - Ebits_to_go);
}


/*!
 ************************************************************************
 * \brief
 *    Terminates the arithmetic codeword, writes stop bit and stuffing bytes (if any)
 ************************************************************************
 */
void arienco_done_encoding(EncodingEnvironmentPtr eep)
{
  put_one_bit_plus_outstanding((unsigned char) ((Elow >> (B_BITS-1)) & 1));
  put_one_bit((unsigned char) (Elow >> (B_BITS-2))&1);
  put_one_bit((unsigned char) 1);

  stats->bit_use_stuffingBits[img->type]+=(8-Ebits_to_go);

  while (Ebits_to_go != 8)
    put_one_bit(0);

  pic_bin_count += eep->E*8 + eep->C; // no of processed bins
}

extern int cabac_encoding;

/*!
 ************************************************************************
 * \brief
 *    Actually arithmetic encoding of one binary symbol by using
 *    the probability estimate of its associated context model
 ************************************************************************
 */
void biari_encode_symbol(EncodingEnvironmentPtr eep, signed short symbol, BiContextTypePtr bi_ct )
{
  register unsigned int range = Erange;
  register unsigned int low = Elow;
  unsigned int rLPS = rLPS_table_64x4[bi_ct->state][(range>>6) & 3];

#if (2==TRACE)
  if (cabac_encoding)
    fprintf(p_trace, "%d  0x%04x  %d  %d\n", binCount++, Erange , bi_ct->state, bi_ct->MPS );
#endif

  range -= rLPS;
  bi_ct->count += cabac_encoding;

  /* covers all cases where code does not bother to shift down symbol to be
   * either 0 or 1, e.g. in some cases for cbp, mb_Type etc the code simply
   * masks off the bit position and passes in the resulting value */
  symbol = (short) (symbol != 0);

  if (symbol != bi_ct->MPS)
  {
    low += range;
    range = rLPS;

    if (!bi_ct->state)
      bi_ct->MPS = (unsigned char) (bi_ct->MPS ^ 0x01);               // switch LPS if necessary
    bi_ct->state = AC_next_state_LPS_64[bi_ct->state]; // next state
  }
  else
    bi_ct->state = AC_next_state_MPS_64[bi_ct->state]; // next state

  /* renormalisation */
  while (range < QUARTER)
  {
    if (low >= HALF)
    {
      put_one_bit_plus_outstanding(1);
      low -= HALF;
    }
    else if (low < QUARTER)
    {
      put_one_bit_plus_outstanding(0);
    }
    else
    {
      Ebits_to_follow++;
      low -= QUARTER;
    }
    low <<= 1;
    range <<= 1;
  }
  Erange = range;
  Elow = low;
  eep->C++;
}

/*!
 ************************************************************************
 * \brief
 *    Arithmetic encoding of one binary symbol assuming
 *    a fixed prob. distribution with p(symbol) = 0.5
 ************************************************************************
 */
void biari_encode_symbol_eq_prob(EncodingEnvironmentPtr eep, signed short symbol)
{
  register unsigned int low = (Elow<<1);

#if (2==TRACE)
  extern int cabac_encoding;
  if (cabac_encoding)
    fprintf(p_trace, "%d  0x%04x\n", binCount++, Erange );
#endif

  if (symbol != 0)
    low += Erange;

  /* renormalisation as for biari_encode_symbol;
     note that low has already been doubled */
  if (low >= ONE)
  {
    put_one_bit_plus_outstanding(1);
    low -= ONE;
  }
  else
    if (low < HALF)
    {
      put_one_bit_plus_outstanding(0);
    }
    else
    {
      Ebits_to_follow++;
      low -= HALF;
    }
    Elow = low;
    eep->C++;
}

/*!
 ************************************************************************
 * \brief
 *    Arithmetic encoding for last symbol before termination
 ************************************************************************
 */
void biari_encode_symbol_final(EncodingEnvironmentPtr eep, signed short symbol)
{
  register unsigned int range = Erange-2;
  register unsigned int low = Elow;

#if (2==TRACE)
  extern int cabac_encoding;
  if (cabac_encoding)
    fprintf(p_trace, "%d  0x%04x\n", binCount++, Erange);
#endif

  if (symbol) {
    low += range;
    range = 2;
  }

  while (range < QUARTER)
  {
    if (low >= HALF)
    {
      put_one_bit_plus_outstanding(1);
      low -= HALF;
    }
    else
      if (low < QUARTER)
      {
        put_one_bit_plus_outstanding(0);
      }
      else
      {
        Ebits_to_follow++;
        low -= QUARTER;
      }
      low <<= 1;
      range <<= 1;
  }
  Erange = range;
  Elow = low;
  eep->C++;
}


/*!
 ************************************************************************
 * \brief
 *    Initializes a given context with some pre-defined probability state
 ************************************************************************
 */
void biari_init_context (BiContextTypePtr ctx, const int* ini)
{
  int pstate = iClip3 ( 1, 126, ((ini[0]* imax(0, img->currentSlice->qp)) >> 4) + ini[1]);

  if ( pstate >= 64 )
  {
    ctx->state  = (unsigned short) (pstate - 64);
    ctx->MPS    = 1;
  }
  else
  {
    ctx->state  = (unsigned short) (63 - pstate);
    ctx->MPS    = 0;
  }

  ctx->count = 0;
}

