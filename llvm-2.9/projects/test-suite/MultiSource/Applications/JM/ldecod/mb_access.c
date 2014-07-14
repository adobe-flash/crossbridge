
/*!
 *************************************************************************************
 * \file mb_access.c
 *
 * \brief
 *    Functions for macroblock neighborhoods
 *
 *  \author
 *      Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Sühring          <suehring@hhi.de>
 *************************************************************************************
 */
#include <assert.h>

#include "global.h"
#include "mbuffer.h"
#include "mb_access.h"

extern StorablePicture *dec_picture;

/*!
 ************************************************************************
 * \brief
 *    returns 1 if the macroblock at the given address is available
 ************************************************************************
 */
int mb_is_available(int mbAddr, int currMbAddr)
{
  if ((mbAddr < 0) || (mbAddr > ((int)dec_picture->PicSizeInMbs - 1)))
    return 0;

  // the following line checks both: slice number and if the mb has been decoded
  if (!img->DeblockCall)
  {
    if (img->mb_data[mbAddr].slice_nr != img->mb_data[currMbAddr].slice_nr)
      return 0;
  }

  return 1;
}


/*!
 ************************************************************************
 * \brief
 *    Checks the availability of neighboring macroblocks of
 *    the current macroblock for prediction and context determination;
 ************************************************************************
 */
void CheckAvailabilityOfNeighbors(void)
{
  const int mb_nr = img->current_mb_nr;
  Macroblock *currMB = &img->mb_data[mb_nr];

  // mark all neighbors as unavailable
  currMB->mb_available_up   = NULL;
  currMB->mb_available_left = NULL;

  if (dec_picture->MbaffFrameFlag)
  {
    int cur_mb_pair = mb_nr >> 1;
    currMB->mbAddrA = 2 * (cur_mb_pair - 1);
    currMB->mbAddrB = 2 * (cur_mb_pair - dec_picture->PicWidthInMbs);
    currMB->mbAddrC = 2 * (cur_mb_pair - dec_picture->PicWidthInMbs + 1);
    currMB->mbAddrD = 2 * (cur_mb_pair - dec_picture->PicWidthInMbs - 1);

    currMB->mbAvailA = mb_is_available(currMB->mbAddrA, mb_nr) && ((PicPos[ cur_mb_pair     ][0])!=0);
    currMB->mbAvailB = mb_is_available(currMB->mbAddrB, mb_nr);
    currMB->mbAvailC = mb_is_available(currMB->mbAddrC, mb_nr) && ((PicPos[ cur_mb_pair + 1 ][0])!=0);
    currMB->mbAvailD = mb_is_available(currMB->mbAddrD, mb_nr) && ((PicPos[ cur_mb_pair     ][0])!=0);
  }
  else
  {
    currMB->mbAddrA = mb_nr - 1;
    currMB->mbAddrB = mb_nr - dec_picture->PicWidthInMbs;
    currMB->mbAddrC = mb_nr - dec_picture->PicWidthInMbs + 1;
    currMB->mbAddrD = mb_nr - dec_picture->PicWidthInMbs - 1;

    currMB->mbAvailA = mb_is_available(currMB->mbAddrA, mb_nr) && ((PicPos[ mb_nr    ][0])!=0);
    currMB->mbAvailB = mb_is_available(currMB->mbAddrB, mb_nr);
    currMB->mbAvailC = mb_is_available(currMB->mbAddrC, mb_nr) && ((PicPos[ mb_nr + 1][0])!=0);
    currMB->mbAvailD = mb_is_available(currMB->mbAddrD, mb_nr) && ((PicPos[ mb_nr    ][0])!=0);
  }
}


/*!
 ************************************************************************
 * \brief
 *    returns the x and y macroblock coordinates for a given MbAddress
 ************************************************************************
 */
void get_mb_block_pos_normal (int mb_addr, int *x, int*y)
{
  *x = PicPos[ mb_addr ][0];
  *y = PicPos[ mb_addr ][1];
}

/*!
 ************************************************************************
 * \brief
 *    returns the x and y macroblock coordinates for a given MbAddress
 *    for mbaff type slices
 ************************************************************************
 */
void get_mb_block_pos_mbaff (int mb_addr, int *x, int*y)
{
  *x =  PicPos[mb_addr>>1][0];
  *y = (PicPos[mb_addr>>1][1] << 1) + (mb_addr & 0x01);
}

/*!
 ************************************************************************
 * \brief
 *    returns the x and y sample coordinates for a given MbAddress
 ************************************************************************
 */
void get_mb_pos (int mb_addr, int *x, int*y, int is_chroma)
{
  get_mb_block_pos(mb_addr, x, y);

  (*x) *= img->mb_size[is_chroma][0];
  (*y) *= img->mb_size[is_chroma][1];
}


/*!
 ************************************************************************
 * \brief
 *    get neighbouring positions for non-aff coding
 * \param curr_mb_nr
 *   current macroblock number (decoding order)
 * \param xN
 *    input x position
 * \param yN
 *    input y position
 * \param luma
 *    1 if luma coding, 0 for chroma
 * \param pix
 *    returns position informations
 ************************************************************************
 */
void getNonAffNeighbour(unsigned int curr_mb_nr, int xN, int yN, int is_chroma, PixelPos *pix)
{
  Macroblock *currMb = &img->mb_data[curr_mb_nr];
  int maxW = img->mb_size[is_chroma][0], maxH = img->mb_size[is_chroma][1];
/*
  if (!is_chroma)
  {
    maxW = 16;
    maxH = 16;
  }
  else
  {
    assert(dec_picture->chroma_format_idc != 0);
    maxW = img->mb_cr_size_x;
    maxH = img->mb_cr_size_y;
  }
*/

  if ((xN<0))
  {
    if (yN<0)
    {
      pix->mb_addr   = currMb->mbAddrD;
      pix->available = currMb->mbAvailD;
    }
    else if (yN<maxH)
    {
      pix->mb_addr  = currMb->mbAddrA;
      pix->available = currMb->mbAvailA;
    }
    else
      pix->available = FALSE;
  }
  else if (xN<maxW)
  {
    if (yN<0)
    {
      pix->mb_addr  = currMb->mbAddrB;
      pix->available = currMb->mbAvailB;
    }
    else if (yN<maxH)
    {
      pix->mb_addr  = curr_mb_nr;
      pix->available = TRUE;
    }
    else
    {
      pix->available = FALSE;
    }
  }
  else if ((xN>=maxW)&&(yN<0))
  {
    pix->mb_addr  = currMb->mbAddrC;
    pix->available = currMb->mbAvailC;
  }
  else
  {
    pix->available = FALSE;
  }

  if (pix->available || img->DeblockCall)
  {
    int *CurPos = PicPos[ pix->mb_addr ];

    pix->x = xN & (maxW - 1);
    pix->y = yN & (maxH - 1);
    pix->pos_x = CurPos[0] * maxW + pix->x;
    pix->pos_y = CurPos[1] * maxH + pix->y;
  }
}

/*!
 ************************************************************************
 * \brief
 *    get neighbouring positions for aff coding
 * \param curr_mb_nr
 *   current macroblock number (decoding order)
 * \param xN
 *    input x position
 * \param yN
 *    input y position
 * \param luma
 *    1 if luma coding, 0 for chroma
 * \param pix
 *    returns position informations
 ************************************************************************
 */
void getAffNeighbour(unsigned int curr_mb_nr, int xN, int yN, int is_chroma, PixelPos *pix)
{
  Macroblock *currMb = &img->mb_data[curr_mb_nr];
  int maxW, maxH;
  int yM = -1;

/*
  if (!is_chroma)
  {
    maxW = 16;
    maxH = 16;
  }
  else
  {
    assert(dec_picture->chroma_format_idc != 0);
    maxW = img->mb_cr_size_x;
    maxH = img->mb_cr_size_y;
  }
*/
  maxW = img->mb_size[is_chroma][0];
  maxH = img->mb_size[is_chroma][1];

  // initialize to "not available"
  pix->available = FALSE;

  if(yN > (maxH - 1))
  {
    return;
  }
  if (xN > (maxW - 1) && yN >= 0 && yN < maxH)
  {
    return;
  }

  if (xN < 0)
  {
    if (yN < 0)
    {
      if(!currMb->mb_field)
      {
        // frame
        if ((curr_mb_nr & 0x01) == 0)
        {
          // top
          pix->mb_addr   = currMb->mbAddrD  + 1;
          pix->available = currMb->mbAvailD;
          yM = yN;
        }
        else
        {
          // bottom
          pix->mb_addr   = currMb->mbAddrA;
          pix->available = currMb->mbAvailA;
          if (currMb->mbAvailA)
          {
            if(!img->mb_data[currMb->mbAddrA].mb_field)
            {
               yM = yN;
            }
            else
            {
              (pix->mb_addr)++;
               yM = (yN + maxH) >> 1;
            }
          }
        }
      }
      else
      {
        // field
        if ((curr_mb_nr & 0x01) == 0)
        {
          // top
          pix->mb_addr   = currMb->mbAddrD;
          pix->available = currMb->mbAvailD;
          if (currMb->mbAvailD)
          {
            if(!img->mb_data[currMb->mbAddrD].mb_field)
            {
              (pix->mb_addr)++;
               yM = 2 * yN;
            }
            else
            {
               yM = yN;
            }
          }
        }
        else
        {
          // bottom
          pix->mb_addr   = currMb->mbAddrD+1;
          pix->available = currMb->mbAvailD;
          yM = yN;
        }
      }
    }
    else
    { // xN < 0 && yN >= 0
      if (yN >= 0 && yN <maxH)
      {
        if (!currMb->mb_field)
        {
          // frame
          if ((curr_mb_nr & 0x01) == 0)
          {
            // top
            pix->mb_addr   = currMb->mbAddrA;
            pix->available = currMb->mbAvailA;
            if (currMb->mbAvailA)
            {
              if(!img->mb_data[currMb->mbAddrA].mb_field)
              {
                 yM = yN;
              }
              else
              {
                if ((yN & 0x01) == 0)
                {
                   yM = yN>> 1;
                }
                else
                {
                  (pix->mb_addr)++;
                   yM = yN>> 1;
                }
              }
            }
          }
          else
          {
            // bottom
            pix->mb_addr   = currMb->mbAddrA;
            pix->available = currMb->mbAvailA;
            if (currMb->mbAvailA)
            {
              if(!img->mb_data[currMb->mbAddrA].mb_field)
              {
                (pix->mb_addr)++;
                 yM = yN;
              }
              else
              {
                if ((yN & 0x01) == 0)
                {
                   yM = (yN + maxH) >> 1;
                }
                else
                {
                  (pix->mb_addr)++;
                   yM = (yN + maxH) >> 1;
                }
              }
            }
          }
        }
        else
        {
          // field
          if ((curr_mb_nr & 0x01) == 0)
          {
            // top
            pix->mb_addr  = currMb->mbAddrA;
            pix->available = currMb->mbAvailA;
            if (currMb->mbAvailA)
            {
              if(!img->mb_data[currMb->mbAddrA].mb_field)
              {
                if (yN < (maxH >> 1))
                {
                   yM = yN << 1;
                }
                else
                {
                  (pix->mb_addr)++;
                   yM = (yN << 1 ) - maxH;
                }
              }
              else
              {
                 yM = yN;
              }
            }
          }
          else
          {
            // bottom
            pix->mb_addr  = currMb->mbAddrA;
            pix->available = currMb->mbAvailA;
            if (currMb->mbAvailA)
            {
              if(!img->mb_data[currMb->mbAddrA].mb_field)
              {
                if (yN < (maxH >> 1))
                {
                  yM = (yN << 1) + 1;
                }
                else
                {
                  (pix->mb_addr)++;
                   yM = (yN << 1 ) + 1 - maxH;
                }
              }
              else
              {
                (pix->mb_addr)++;
                 yM = yN;
              }
            }
          }
        }
      }
    }
  }
  else
  { // xN >= 0
    if (xN >= 0 && xN < maxW)
    {
      if (yN<0)
      {
        if (!currMb->mb_field)
        {
          //frame
          if ((curr_mb_nr & 0x01) == 0)
          {
            //top
            pix->mb_addr  = currMb->mbAddrB;
            // for the deblocker if the current MB is a frame and the one above is a field
            // then the neighbor is the top MB of the pair
            if (currMb->mbAvailB)
            {
              if (!(img->DeblockCall == 1 && (img->mb_data[currMb->mbAddrB]).mb_field))
                pix->mb_addr  += 1;
            }

            pix->available = currMb->mbAvailB;
            yM = yN;
          }
          else
          {
            // bottom
            pix->mb_addr   = curr_mb_nr - 1;
            pix->available = TRUE;
            yM = yN;
          }
        }
        else
        {
          // field
          if ((curr_mb_nr & 0x01) == 0)
          {
            // top
            pix->mb_addr   = currMb->mbAddrB;
            pix->available = currMb->mbAvailB;
            if (currMb->mbAvailB)
            {
              if(!img->mb_data[currMb->mbAddrB].mb_field)
              {
                (pix->mb_addr)++;
                 yM = 2* yN;
              }
              else
              {
                 yM = yN;
              }
            }
          }
          else
          {
            // bottom
            pix->mb_addr   = currMb->mbAddrB + 1;
            pix->available = currMb->mbAvailB;
            yM = yN;
          }
        }
      }
      else
      {
        // yN >=0
        // for the deblocker if this is the extra edge then do this special stuff
        if (yN == 0 && img->DeblockCall == 2)
        {
          pix->mb_addr  = currMb->mbAddrB + 1;
          pix->available = TRUE;
          yM = yN - 1;
        }

        else if ((yN >= 0) && (yN <maxH))
        {
          pix->mb_addr   = curr_mb_nr;
          pix->available = TRUE;
          yM = yN;
        }
      }
    }
    else
    { // xN >= maxW
      if(yN < 0)
      {
        if (!currMb->mb_field)
        {
          // frame
          if ((curr_mb_nr & 0x01) == 0)
          {
            // top
            pix->mb_addr  = currMb->mbAddrC + 1;
            pix->available = currMb->mbAvailC;
            yM = yN;
          }
          else
          {
            // bottom
            pix->available = FALSE;
          }
        }
        else
        {
          // field
          if ((curr_mb_nr & 0x01) == 0)
          {
            // top
            pix->mb_addr   = currMb->mbAddrC;
            pix->available = currMb->mbAvailC;
            if (currMb->mbAvailC)
            {
              if(!img->mb_data[currMb->mbAddrC].mb_field)
              {
                (pix->mb_addr)++;
                 yM = 2* yN;
              }
              else
              {
                yM = yN;
              }
            }
          }
          else
          {
            // bottom
            pix->mb_addr   = currMb->mbAddrC + 1;
            pix->available = currMb->mbAvailC;
            yM = yN;
          }
        }
      }
    }
  }
  if (pix->available || img->DeblockCall)
  {
    pix->x = xN & (maxW - 1);
    pix->y = yM & (maxH - 1);
    get_mb_pos(pix->mb_addr, &(pix->pos_x), &(pix->pos_y), is_chroma);
    pix->pos_x += pix->x;
    pix->pos_y += pix->y;
  }
}


/*!
 ************************************************************************
 * \brief
 *    get neighbouring positions. MB AFF is automatically used from img structure
 * \param curr_mb_nr
 *   current macroblock number (decoding order)
 * \param xN
 *    input x position
 * \param yN
 *    input y position
 * \param luma
 *    1 if luma coding, 0 for chroma
 * \param pix
 *    returns position informations
 ************************************************************************
 */
/*
void getNeighbour(int curr_mb_nr, int xN, int yN, int is_chroma, PixelPos *pix)
{
  if (curr_mb_nr<0)
    error ("getNeighbour: invalid macroblock number", 100);

  if (dec_picture->MbaffFrameFlag)
    getAffNeighbour(curr_mb_nr, xN, yN, is_chroma, pix);
  else
    getNonAffNeighbour(curr_mb_nr, xN, yN, is_chroma, pix);
}
*/

/*!
 ************************************************************************
 * \brief
 *    get neighbouring  get neighbouring 4x4 luma block
 * \param curr_mb_nr
 *   current macroblock number (decoding order)
 * \param block_x
 *    input x block position
 * \param block_y
 *    input y block position
 * \param rel_x
 *    relative x position of neighbor
 * \param rel_y
 *    relative y position of neighbor
 * \param pix
 *    returns position informations
 ************************************************************************
 */
void getLuma4x4Neighbour (int curr_mb_nr, int block_x, int block_y, PixelPos *pix)
{
  getNeighbour(curr_mb_nr, block_x, block_y, IS_LUMA, pix);

  if (pix->available)
  {
    pix->x >>= 2;
    pix->y >>= 2;
    pix->pos_x >>= 2;
    pix->pos_y >>= 2;
  }
}


/*!
 ************************************************************************
 * \brief
 *    get neighbouring 4x4 chroma block
 * \param curr_mb_nr
 *   current macroblock number (decoding order)
 * \param block_x
 *    input x block position
 * \param block_y
 *    input y block position
 * \param rel_x
 *    relative x position of neighbor
 * \param rel_y
 *    relative y position of neighbor
 * \param pix
 *    returns position informations
 ************************************************************************
 */
void getChroma4x4Neighbour (int curr_mb_nr, int block_x, int block_y, PixelPos *pix)
{
  getNeighbour(curr_mb_nr, block_x, block_y, IS_CHROMA, pix);

  if (pix->available)
  {
    pix->x >>= 2;
    pix->y >>= 2;
    pix->pos_x >>= 2;
    pix->pos_y >>= 2;
  }
}
