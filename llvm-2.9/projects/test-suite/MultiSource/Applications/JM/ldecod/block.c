
/*!
 ***********************************************************************
 *  \file
 *      block.c
 *
 *  \brief
 *      Block functions
 *
 *  \author
 *      Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Inge Lille-Langoy          <inge.lille-langoy@telenor.com>
 *      - Rickard Sjoberg            <rickard.sjoberg@era.ericsson.se>
 ***********************************************************************
 */

#include "contributors.h"

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "block.h"
#include "image.h"
#include "mb_access.h"


#define Q_BITS          15

static const int quant_coef[6][4][4] = {
  {{13107, 8066,13107, 8066},{ 8066, 5243, 8066, 5243},{13107, 8066,13107, 8066},{ 8066, 5243, 8066, 5243}},
  {{11916, 7490,11916, 7490},{ 7490, 4660, 7490, 4660},{11916, 7490,11916, 7490},{ 7490, 4660, 7490, 4660}},
  {{10082, 6554,10082, 6554},{ 6554, 4194, 6554, 4194},{10082, 6554,10082, 6554},{ 6554, 4194, 6554, 4194}},
  {{ 9362, 5825, 9362, 5825},{ 5825, 3647, 5825, 3647},{ 9362, 5825, 9362, 5825},{ 5825, 3647, 5825, 3647}},
  {{ 8192, 5243, 8192, 5243},{ 5243, 3355, 5243, 3355},{ 8192, 5243, 8192, 5243},{ 5243, 3355, 5243, 3355}},
  {{ 7282, 4559, 7282, 4559},{ 4559, 2893, 4559, 2893},{ 7282, 4559, 7282, 4559},{ 4559, 2893, 4559, 2893}}
};
static const int A[4][4] = {
  { 16, 20, 16, 20},
  { 20, 25, 20, 25},
  { 16, 20, 16, 20},
  { 20, 25, 20, 25}
};

int quant_intra_default[16] = {
 6,13,20,28,
13,20,28,32,
20,28,32,37,
28,32,37,42
};

int quant_inter_default[16] = {
10,14,20,24,
14,20,24,27,
20,24,27,30,
24,27,30,34
};

int quant8_intra_default[64] = {
 6,10,13,16,18,23,25,27,
10,11,16,18,23,25,27,29,
13,16,18,23,25,27,29,31,
16,18,23,25,27,29,31,33,
18,23,25,27,29,31,33,36,
23,25,27,29,31,33,36,38,
25,27,29,31,33,36,38,40,
27,29,31,33,36,38,40,42
};

int quant8_inter_default[64] = {
 9,13,15,17,19,21,22,24,
13,13,17,19,21,22,24,25,
15,17,19,21,22,24,25,27,
17,19,21,22,24,25,27,28,
19,21,22,24,25,27,28,30,
21,22,24,25,27,28,30,32,
22,24,25,27,28,30,32,33,
24,25,27,28,30,32,33,35
};

int quant_org[16] = { //to be use if no q matrix is chosen
16,16,16,16,
16,16,16,16,
16,16,16,16,
16,16,16,16
};

int quant8_org[64] = { //to be use if no q matrix is chosen
16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16,
16,16,16,16,16,16,16,16
};

// Notation for comments regarding prediction and predictors.
// The pels of the 4x4 block are labelled a..p. The predictor pels above
// are labelled A..H, from the left I..L, and from above left X, as follows:
//
//  X A B C D E F G H
//  I a b c d
//  J e f g h
//  K i j k l
//  L m n o p
//

// Predictor array index definitions
#define P_X (PredPel[0])
#define P_A (PredPel[1])
#define P_B (PredPel[2])
#define P_C (PredPel[3])
#define P_D (PredPel[4])
#define P_E (PredPel[5])
#define P_F (PredPel[6])
#define P_G (PredPel[7])
#define P_H (PredPel[8])
#define P_I (PredPel[9])
#define P_J (PredPel[10])
#define P_K (PredPel[11])
#define P_L (PredPel[12])

/*!
 ***********************************************************************
 * \brief
 *    makes and returns 4x4 blocks with all 5 intra prediction modes
 *
 * \return
 *    DECODING_OK   decoding of intraprediction mode was sucessfull            \n
 *    SEARCH_SYNC   search next sync element as errors while decoding occured
 ***********************************************************************
 */

int intrapred( struct img_par *img,  //!< image parameters
               int ioff,             //!< pixel offset X within MB
               int joff,             //!< pixel offset Y within MB
               int img_block_x,      //!< location of block X, multiples of 4
               int img_block_y)      //!< location of block Y, multiples of 4
{
  int i,j;
  int s0;
  int img_y,img_x;
  imgpel PredPel[13];  // array of predictor pels

  imgpel **imgY = dec_picture->imgY;

  PixelPos pix_a[4];
  PixelPos pix_b, pix_c, pix_d;

  int block_available_up;
  int block_available_left;
  int block_available_up_left;
  int block_available_up_right;

  int mb_nr=img->current_mb_nr;

  byte predmode = img->ipredmode[img_block_y][img_block_x];
  int jpos0 = joff, jpos1 = joff + 1, jpos2 = joff + 2, jpos3 = joff + 3;
  int ipos0 = ioff, ipos1 = ioff + 1, ipos2 = ioff + 2, ipos3 = ioff + 3;


  img_x=img_block_x*4;
  img_y=img_block_y*4;

  for (i=0;i<4;i++)
  {
    getNeighbour(mb_nr, ioff -1 , joff +i , IS_LUMA, &pix_a[i]);
  }

  getNeighbour(mb_nr, ioff    , joff -1 , IS_LUMA, &pix_b);
  getNeighbour(mb_nr, ioff +4 , joff -1 , IS_LUMA, &pix_c);
  getNeighbour(mb_nr, ioff -1 , joff -1 , IS_LUMA, &pix_d);

  pix_c.available = pix_c.available && !((ioff==4) && ((joff==4)||(joff==12)));

  if (active_pps->constrained_intra_pred_flag)
  {
    for (i=0, block_available_left=1; i<4;i++)
      block_available_left  &= pix_a[i].available ? img->intra_block[pix_a[i].mb_addr]: 0;
    block_available_up       = pix_b.available ? img->intra_block [pix_b.mb_addr] : 0;
    block_available_up_right = pix_c.available ? img->intra_block [pix_c.mb_addr] : 0;
    block_available_up_left  = pix_d.available ? img->intra_block [pix_d.mb_addr] : 0;
  }
  else
  {
    block_available_left     = pix_a[0].available;
    block_available_up       = pix_b.available;
    block_available_up_right = pix_c.available;
    block_available_up_left  = pix_d.available;
  }

  // form predictor pels
  if (block_available_up)
  {
    P_A = imgY[pix_b.pos_y][pix_b.pos_x+0];
    P_B = imgY[pix_b.pos_y][pix_b.pos_x+1];
    P_C = imgY[pix_b.pos_y][pix_b.pos_x+2];
    P_D = imgY[pix_b.pos_y][pix_b.pos_x+3];

  }
  else
  {
    P_A = P_B = P_C = P_D = img->dc_pred_value_luma;
  }

  if (block_available_up_right)
  {
    P_E = imgY[pix_c.pos_y][pix_c.pos_x+0];
    P_F = imgY[pix_c.pos_y][pix_c.pos_x+1];
    P_G = imgY[pix_c.pos_y][pix_c.pos_x+2];
    P_H = imgY[pix_c.pos_y][pix_c.pos_x+3];
  }
  else
  {
    P_E = P_F = P_G = P_H = P_D;
  }

  if (block_available_left)
  {
    P_I = imgY[pix_a[0].pos_y][pix_a[0].pos_x];
    P_J = imgY[pix_a[1].pos_y][pix_a[1].pos_x];
    P_K = imgY[pix_a[2].pos_y][pix_a[2].pos_x];
    P_L = imgY[pix_a[3].pos_y][pix_a[3].pos_x];
  }
  else
  {
    P_I = P_J = P_K = P_L = img->dc_pred_value_luma;
  }

  if (block_available_up_left)
  {
    P_X = imgY[pix_d.pos_y][pix_d.pos_x];
  }
  else
  {
    P_X = img->dc_pred_value_luma;
  }


  switch (predmode)
  {
  case DC_PRED:                         /* DC prediction */

    s0 = 0;
    if (block_available_up && block_available_left)
    {
      // no edge
      s0 = (P_A + P_B + P_C + P_D + P_I + P_J + P_K + P_L + 4)/(2*BLOCK_SIZE);
    }
    else if (!block_available_up && block_available_left)
    {
      // upper edge
      s0 = (P_I + P_J + P_K + P_L + 2)/BLOCK_SIZE;
    }
    else if (block_available_up && !block_available_left)
    {
      // left edge
      s0 = (P_A + P_B + P_C + P_D + 2)/BLOCK_SIZE;
    }
    else //if (!block_available_up && !block_available_left)
    {
      // top left corner, nothing to predict from
      s0 = img->dc_pred_value_luma;
    }

    for (j=0; j < BLOCK_SIZE; j++)
    {
      for (i=0; i < BLOCK_SIZE; i++)
      {
        // store DC prediction
        img->mpr[j+joff][i+ioff] = (imgpel) s0;
      }
    }
    break;

  case VERT_PRED:                       /* vertical prediction from block above */
    if (!block_available_up)
      printf ("warning: Intra_4x4_Vertical prediction mode not allowed at mb %d\n", (int) img->current_mb_nr);

    for(j=0;j<BLOCK_SIZE;j++)
      for(i=0;i<BLOCK_SIZE;i++)
        img->mpr[j+joff][i+ioff]=imgY[pix_b.pos_y][pix_b.pos_x+i];/* store predicted 4x4 block */
    break;

  case HOR_PRED:                        /* horizontal prediction from left block */
    if (!block_available_left)
      printf ("warning: Intra_4x4_Horizontal prediction mode not allowed at mb %d\n",(int) img->current_mb_nr);

    for(j=0;j<BLOCK_SIZE;j++)
      for(i=0;i<BLOCK_SIZE;i++)
        img->mpr[j+joff][i+ioff]=imgY[pix_a[j].pos_y][pix_a[j].pos_x]; /* store predicted 4x4 block */
    break;

  case DIAG_DOWN_RIGHT_PRED:
    if ((!block_available_up)||(!block_available_left)||(!block_available_up_left))
      printf ("warning: Intra_4x4_Diagonal_Down_Right prediction mode not allowed at mb %d\n",(int) img->current_mb_nr);

    img->mpr[jpos3][ipos0] = (imgpel) ((P_L + 2*P_K + P_J + 2) >> 2);
    img->mpr[jpos2][ipos0] =
    img->mpr[jpos3][ipos1] = (imgpel) ((P_K + 2*P_J + P_I + 2) >> 2);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos3][ipos2] = (imgpel) ((P_J + 2*P_I + P_X + 2) >> 2);
    img->mpr[jpos0][ipos0] =
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos3][ipos3] = (imgpel) ((P_I + 2*P_X + P_A + 2) >> 2);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos2][ipos3] = (imgpel) ((P_X + 2*P_A + P_B + 2) >> 2);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos1][ipos3] = (imgpel) ((P_A + 2*P_B + P_C + 2) >> 2);
    img->mpr[jpos0][ipos3] = (imgpel) ((P_B + 2*P_C + P_D + 2) >> 2);
    break;

  case DIAG_DOWN_LEFT_PRED:
    if (!block_available_up)
      printf ("warning: Intra_4x4_Diagonal_Down_Left prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] = (imgpel) ((P_A + P_C + 2*(P_B) + 2) >> 2);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos1][ipos0] = (imgpel) ((P_B + P_D + 2*(P_C) + 2) >> 2);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos2][ipos0] = (imgpel) ((P_C + P_E + 2*(P_D) + 2) >> 2);
    img->mpr[jpos0][ipos3] =
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos3][ipos0] = (imgpel) ((P_D + P_F + 2*(P_E) + 2) >> 2);
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos3][ipos1] = (imgpel) ((P_E + P_G + 2*(P_F) + 2) >> 2);
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos3][ipos2] = (imgpel) ((P_F + P_H + 2*(P_G) + 2) >> 2);
    img->mpr[jpos3][ipos3] = (imgpel) ((P_G + 3*(P_H) + 2) >> 2);
    break;

  case  VERT_RIGHT_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
    if ((!block_available_up)||(!block_available_left)||(!block_available_up_left))
      printf ("warning: Intra_4x4_Vertical_Right prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] =
    img->mpr[jpos2][ipos1] = (imgpel) ((P_X + P_A + 1) >> 1);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos2][ipos2] = (imgpel) ((P_A + P_B + 1) >> 1);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos2][ipos3] = (imgpel) ((P_B + P_C + 1) >> 1);
    img->mpr[jpos0][ipos3] = (imgpel) ((P_C + P_D + 1) >> 1);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos3][ipos1] = (imgpel) ((P_I + 2*P_X + P_A + 2) >> 2);
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos3][ipos2] = (imgpel) ((P_X + 2*P_A + P_B + 2) >> 2);
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos3][ipos3] = (imgpel) ((P_A + 2*P_B + P_C + 2) >> 2);
    img->mpr[jpos1][ipos3] = (imgpel) ((P_B + 2*P_C + P_D + 2) >> 2);
    img->mpr[jpos2][ipos0] = (imgpel) ((P_X + 2*P_I + P_J + 2) >> 2);
    img->mpr[jpos3][ipos0] = (imgpel) ((P_I + 2*P_J + P_K + 2) >> 2);
    break;

  case  VERT_LEFT_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
    if (!block_available_up)
      printf ("warning: Intra_4x4_Vertical_Left prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] = (imgpel) ((P_A + P_B + 1) >> 1);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos2][ipos0] = (imgpel) ((P_B + P_C + 1) >> 1);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos2][ipos1] = (imgpel) ((P_C + P_D + 1) >> 1);
    img->mpr[jpos0][ipos3] =
    img->mpr[jpos2][ipos2] = (imgpel) ((P_D + P_E + 1) >> 1);
    img->mpr[jpos2][ipos3] = (imgpel) ((P_E + P_F + 1) >> 1);
    img->mpr[jpos1][ipos0] = (imgpel) ((P_A + 2*P_B + P_C + 2) >> 2);
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos3][ipos0] = (imgpel) ((P_B + 2*P_C + P_D + 2) >> 2);
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos3][ipos1] = (imgpel) ((P_C + 2*P_D + P_E + 2) >> 2);
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos3][ipos2] = (imgpel) ((P_D + 2*P_E + P_F + 2) >> 2);
    img->mpr[jpos3][ipos3] = (imgpel) ((P_E + 2*P_F + P_G + 2) >> 2);
    break;

  case  HOR_UP_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
    if (!block_available_left)
      printf ("warning: Intra_4x4_Horizontal_Up prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] = (imgpel) ((P_I + P_J + 1) >> 1);
    img->mpr[jpos0][ipos1] = (imgpel) ((P_I + 2*P_J + P_K + 2) >> 2);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos1][ipos0] = (imgpel) ((P_J + P_K + 1) >> 1);
    img->mpr[jpos0][ipos3] =
    img->mpr[jpos1][ipos1] = (imgpel) ((P_J + 2*P_K + P_L + 2) >> 2);
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos2][ipos0] = (imgpel) ((P_K + P_L + 1) >> 1);
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos2][ipos1] = (imgpel) ((P_K + 2*P_L + P_L + 2) >> 2);
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos3][ipos1] =
    img->mpr[jpos3][ipos0] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos3][ipos2] =
    img->mpr[jpos3][ipos3] = (imgpel) P_L;
    break;

  case  HOR_DOWN_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
    if ((!block_available_up)||(!block_available_left)||(!block_available_up_left))
      printf ("warning: Intra_4x4_Horizontal_Down prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] =
    img->mpr[jpos1][ipos2] = (imgpel) ((P_X + P_I + 1) >> 1);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos1][ipos3] = (imgpel) ((P_I + 2*P_X + P_A + 2) >> 2);
    img->mpr[jpos0][ipos2] = (imgpel) ((P_X + 2*P_A + P_B + 2) >> 2);
    img->mpr[jpos0][ipos3] = (imgpel) ((P_A + 2*P_B + P_C + 2) >> 2);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos2][ipos2] = (imgpel) ((P_I + P_J + 1) >> 1);
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos2][ipos3] = (imgpel) ((P_X + 2*P_I + P_J + 2) >> 2);
    img->mpr[jpos2][ipos0] =
    img->mpr[jpos3][ipos2] = (imgpel) ((P_J + P_K + 1) >> 1);
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos3][ipos3] = (imgpel) ((P_I + 2*P_J + P_K + 2) >> 2);
    img->mpr[jpos3][ipos0] = (imgpel) ((P_K + P_L + 1) >> 1);
    img->mpr[jpos3][ipos1] = (imgpel) ((P_J + 2*P_K + P_L + 2) >> 2);
    break;

  default:
    printf("Error: illegal intra_4x4 prediction mode: %d\n",predmode);
    return SEARCH_SYNC;
    break;
  }

  return DECODING_OK;
}


/*!
 ***********************************************************************
 * \return
 *    best SAD
 ***********************************************************************
 */
int intrapred_luma_16x16(struct img_par *img, //!< image parameters
                         int predmode)        //!< prediction mode
{
  int s0=0,s1,s2;

  int i,j;

  int ih,iv;
  int ib,ic,iaa;

  imgpel **imgY=dec_picture->imgY;

  int mb_nr=img->current_mb_nr;

  PixelPos up;          //!< pixel position p(0,-1)
  PixelPos left[17];    //!< pixel positions p(-1, -1..15)

  int up_avail, left_avail, left_up_avail;

  s1=s2=0;

  for (i=0;i<17;i++)
  {
    getNeighbour(mb_nr, -1 ,  i-1 , IS_LUMA, &left[i]);
  }
  getNeighbour(mb_nr, 0     ,  -1 , IS_LUMA, &up);

  if (!active_pps->constrained_intra_pred_flag)
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

  switch (predmode)
  {
  case VERT_PRED_16:                       // vertical prediction from block above
    if (!up_avail)
      error ("invalid 16x16 intra pred Mode VERT_PRED_16",500);
    for(j=0;j<MB_BLOCK_SIZE;j++)
      for(i=0;i<MB_BLOCK_SIZE;i++)
        img->mpr[j][i]=imgY[up.pos_y][up.pos_x+i];// store predicted 16x16 block
    break;

  case HOR_PRED_16:                        // horizontal prediction from left block
    if (!left_avail)
      error ("invalid 16x16 intra pred Mode HOR_PRED_16",500);
    for(j=0;j<MB_BLOCK_SIZE;j++)
      for(i=0;i<MB_BLOCK_SIZE;i++)
        img->mpr[j][i]=imgY[left[j+1].pos_y][left[j+1].pos_x]; // store predicted 16x16 block
    break;

  case DC_PRED_16:                         // DC prediction
    s1=s2=0;
    for (i=0; i < MB_BLOCK_SIZE; i++)
    {
      if (up_avail)
        s1 += imgY[up.pos_y][up.pos_x+i];    // sum hor pix
      if (left_avail)
        s2 += imgY[left[i+1].pos_y][left[i+1].pos_x];    // sum vert pix
    }
    if (up_avail && left_avail)
      s0=(s1+s2+16)>>5;       // no edge
    if (!up_avail && left_avail)
      s0=(s2+8)>>4;              // upper edge
    if (up_avail && !left_avail)
      s0=(s1+8)>>4;              // left edge
    if (!up_avail && !left_avail)
      s0=img->dc_pred_value_luma;                            // top left corner, nothing to predict from
    for(i=0;i<MB_BLOCK_SIZE;i++)
      for(j=0;j<MB_BLOCK_SIZE;j++)
      {
        img->mpr[j][i]=(imgpel) s0;
      }
    break;
  case PLANE_16:// 16 bit integer plan pred
    if (!up_avail || !left_up_avail  || !left_avail)
      error ("invalid 16x16 intra pred Mode PLANE_16",500);

    ih=0;
    iv=0;
    for (i=1;i<9;i++)
    {
      if (i<8)
        ih += i*(imgY[up.pos_y][up.pos_x+7+i] - imgY[up.pos_y][up.pos_x+7-i]);
      else
        ih += i*(imgY[up.pos_y][up.pos_x+7+i] - imgY[left[0].pos_y][left[0].pos_x]);

      iv += i*(imgY[left[8+i].pos_y][left[8+i].pos_x] - imgY[left[8-i].pos_y][left[8-i].pos_x]);
    }
    ib=(5*ih+32)>>6;
    ic=(5*iv+32)>>6;

    iaa=16*(imgY[up.pos_y][up.pos_x+15]+imgY[left[16].pos_y][left[16].pos_x]);
    for (j=0;j< MB_BLOCK_SIZE;j++)
    {
      for (i=0;i< MB_BLOCK_SIZE;i++)
      {
        img->mpr[j][i]=(imgpel) iClip1(img->max_imgpel_value,((iaa+(i-7)*ib +(j-7)*ic + 16)>>5));
      }
    }// store plane prediction
    break;

  default:
    {                                    // indication of fault in bitstream,exit
      printf("illegal 16x16 intra prediction mode input: %d\n",predmode);
      return SEARCH_SYNC;
    }
  }

  return DECODING_OK;
}


void intrapred_chroma(struct img_par *img, int uv)
{
  int i,j, ii, jj, ioff, joff;

  imgpel ***imgUV = dec_picture->imgUV;

  int js[4][4];

  int pred;
  int ih, iv, ib, ic, iaa;

  int      b8, b4;
  int      yuv = dec_picture->chroma_format_idc - 1;
  int      blk_x, blk_y;
  int      block_pos[3][4][4]= //[yuv][b8][b4]
  {
    { {0, 1, 2, 3},{0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0}},
    { {0, 1, 2, 3},{2, 3, 2, 3},{0, 0, 0, 0},{0, 0, 0, 0}},
    { {0, 1, 2, 3},{1, 1, 3, 3},{2, 3, 2, 3},{3, 3, 3, 3}}
  };
  int      s0, s1, s2, s3;

  int mb_nr=img->current_mb_nr;
  Macroblock *currMB = &img->mb_data[img->current_mb_nr];

  PixelPos up;        //!< pixel position  p(0,-1)
  PixelPos left[17];  //!< pixel positions p(-1, -1..16)

  int up_avail, left_avail[2], left_up_avail;

  int cr_MB_x = img->mb_cr_size_x;
  int cr_MB_y = img->mb_cr_size_y;

  for (i=0;i<cr_MB_y+1;i++)
  {
    getNeighbour(mb_nr, -1, i-1, IS_CHROMA, &left[i]);
  }

  getNeighbour(mb_nr, 0, -1, IS_CHROMA, &up);

  if (!active_pps->constrained_intra_pred_flag)
  {
    up_avail      = up.available;
    left_avail[0] = left_avail[1] = left[1].available;
    left_up_avail = left[0].available;
  }
  else
  {
    up_avail = up.available ? img->intra_block[up.mb_addr] : 0;
    for (i=0, left_avail[0]=1; i<cr_MB_y/2;i++)
      left_avail[0]  &= left[i+1].available ? img->intra_block[left[i+1].mb_addr]: 0;
    for (i=cr_MB_y/2, left_avail[1]=1; i<cr_MB_y;i++)
      left_avail[1]  &= left[i+1].available ? img->intra_block[left[i+1].mb_addr]: 0;
    left_up_avail = left[0].available ? img->intra_block[left[0].mb_addr]: 0;
  }


  if (currMB->c_ipred_mode == DC_PRED_8)
  {
    // DC prediction
    for(b8=0; b8<img->num_blk8x8_uv/2;b8++)
    {
      for (b4=0; b4<4; b4++)
      {
        blk_y = subblk_offset_y[yuv][b8][b4] + 1;
        blk_x = subblk_offset_x[yuv][b8][b4];

        s0=s1=s2=s3=0;
        js[b8][b4]=img->dc_pred_value_chroma;

        //===== get prediction value =====
        switch (block_pos[yuv][b8][b4])
        {
        case 0:  //===== TOP LEFT =====
          if      (up_avail)       for (i=blk_x;i<(blk_x+4);i++)  s0 += imgUV[uv][up.pos_y][up.pos_x + i];
          if      (left_avail[0])  for (i=blk_y;i<(blk_y+4);i++)  s2 += imgUV[uv][left[i].pos_y][left[i].pos_x];
          if      (up_avail && left_avail[0])         js[b8][b4]  = (s0+s2+4) >> 3;
          else if (up_avail)                          js[b8][b4]  = (s0   +2) >> 2;
          else if (left_avail[0])                     js[b8][b4]  = (s2   +2) >> 2;
          break;
        case 1: //===== TOP RIGHT =====
          if      (up_avail)       for (i=blk_x;i<(blk_x+4);i++)  s1 += imgUV[uv][up.pos_y][up.pos_x + i];
          else if (left_avail[0])  for (i=blk_y;i<(blk_y+4);i++)  s2 += imgUV[uv][left[i].pos_y][left[i].pos_x];
          if      (up_avail)                          js[b8][b4]  = (s1   +2) >> 2;
          else if (left_avail[0])                     js[b8][b4]  = (s2   +2) >> 2;
          break;
        case 2: //===== BOTTOM LEFT =====
          if      (left_avail[1])  for (i=blk_y;i<(blk_y+4);i++)  s3 += imgUV[uv][left[i].pos_y][left[i].pos_x];
          else if (up_avail)       for (i=blk_x;i<(blk_x+4);i++)  s0 += imgUV[uv][up.pos_y][up.pos_x + i];
          if      (left_avail[1])                     js[b8][b4]  = (s3   +2) >> 2;
          else if (up_avail)                          js[b8][b4]  = (s0   +2) >> 2;
          break;
        case 3: //===== BOTTOM RIGHT =====
          if      (up_avail)       for (i=blk_x;i<(blk_x+4);i++)  s1 += imgUV[uv][up.pos_y][up.pos_x + i];
          if      (left_avail[1])  for (i=blk_y;i<(blk_y+4);i++)  s3 += imgUV[uv][left[i].pos_y][left[i].pos_x];
          if      (up_avail && left_avail[1])         js[b8][b4]  = (s1+s3+4) >> 3;
          else if (up_avail)                          js[b8][b4]  = (s1   +2) >> 2;
          else if (left_avail[1])                     js[b8][b4]  = (s3   +2) >> 2;
          break;
        }
      }
    }
  }
  if (PLANE_8 == currMB->c_ipred_mode)
  {
    // plane prediction
    if (!left_up_avail || !left_avail[0] || !left_avail[1] || !up_avail)
      error("unexpected PLANE_8 chroma intra prediction mode",-1);

    ih = cr_MB_x/2*(imgUV[uv][up.pos_y][up.pos_x+cr_MB_x-1] - imgUV[uv][left[0].pos_y][left[0].pos_x]);
    for (i=0;i<cr_MB_x/2-1;i++)
      ih += (i+1)*(imgUV[uv][up.pos_y][up.pos_x+cr_MB_x/2  +i] -
      imgUV[uv][up.pos_y][up.pos_x+cr_MB_x/2-2-i]);

    iv = cr_MB_y/2*(imgUV[uv][left[cr_MB_y].pos_y][left[cr_MB_y].pos_x] - imgUV[uv][left[0].pos_y][left[0].pos_x]);
    for (i=0;i<cr_MB_y/2-1;i++)
      iv += (i+1)*(imgUV[uv][left[cr_MB_y/2+1+i].pos_y][left[cr_MB_y/2+1+i].pos_x] -
      imgUV[uv][left[cr_MB_y/2-1-i].pos_y][left[cr_MB_y/2-1-i].pos_x]);

    ib= ((cr_MB_x == 8?17:5)*ih+2*cr_MB_x)>>(cr_MB_x == 8?5:6);
    ic= ((cr_MB_y == 8?17:5)*iv+2*cr_MB_y)>>(cr_MB_y == 8?5:6);

    iaa=16*(imgUV[uv][left[cr_MB_y].pos_y][left[cr_MB_y].pos_x] +
            imgUV[uv][up.pos_y][up.pos_x+cr_MB_x-1]);

    for (j=0; j<cr_MB_y; j++)
      for (i=0; i<cr_MB_x; i++)
        img->mpr[j][i]=(imgpel) iClip1(img->max_imgpel_value_uv,((iaa+(i-cr_MB_x/2+1)*ib+(j-cr_MB_y/2+1)*ic+16)>>5));
  }
  else
  {
    switch (currMB->c_ipred_mode)
    {
    case DC_PRED_8:
      for (b8=0;b8<img->num_blk8x8_uv/2;b8++)
      {
        for (b4=0;b4<4;b4++)
        {
          joff = subblk_offset_y[yuv][b8][b4];
          ioff = subblk_offset_x[yuv][b8][b4];
          for (jj=joff; jj<joff + BLOCK_SIZE; jj++)
          for (ii=ioff; ii<ioff + BLOCK_SIZE; ii++)
          {
            img->mpr[jj][ii]=(imgpel) js[b8][b4];
          }
        }
      }
      break;
    case HOR_PRED_8:
      if (!left_avail[0] || !left_avail[1])
        error("unexpected HOR_PRED_8 chroma intra prediction mode",-1);

      for (j=0;j<2;j++)
      {
        joff=j*cr_MB_y/2;
        for(i=0;i<2;i++)
        {
          ioff=i*cr_MB_x/2;
          for (jj=joff; jj<joff + cr_MB_y/2; jj++)
          {
            pred = imgUV[uv][left[1+jj].pos_y][left[1+jj].pos_x];
            for (ii=ioff; ii<ioff + cr_MB_x/2; ii++)
              img->mpr[jj][ii]=(imgpel) pred;
          }
        }
      }
      break;
    case VERT_PRED_8:
      if (!up_avail)
        error("unexpected VERT_PRED_8 chroma intra prediction mode",-1);

      for (j=0;j<2;j++)
      {
        joff=j*cr_MB_y/2;
        for(i=0;i<2;i++)
        {
          ioff=i*cr_MB_x/2;
          for (ii=ioff; ii<ioff + cr_MB_x/2; ii++)
          {
            pred = imgUV[uv][up.pos_y][up.pos_x+ii];
            for (jj=joff; jj<joff + cr_MB_y/2; jj++)
              img->mpr[jj][ii]=(imgpel) pred;
          }
        }
      }
      break;
    default:
      error("illegal chroma intra prediction mode", 600);
      break;
    }
  }
}

/*!
 ***********************************************************************
 * \brief
 *    Inverse 4x4 transformation, transforms cof to m7
 ***********************************************************************
 */
void itrans(struct img_par *img, //!< image parameters
            int ioff,            //!< index to 4x4 block
            int joff,            //!<
            int i0,              //!<
            int j0,
            int chroma)
{
  int i,j;
  int m5[4];
  int m6[4];

  Boolean lossless_qpprime = (Boolean) ((img->qp + img->bitdepth_luma_qp_scale)==0 && img->lossless_qpprime_flag==1);
  int max_imgpel_value = chroma ? img->max_imgpel_value_uv : img->max_imgpel_value;

  if (!lossless_qpprime)
  {
    // horizontal
    for (j=0;j<BLOCK_SIZE;j++)
    {
      memcpy(&m5[0],&img->cof[i0][j0][j][0], BLOCK_SIZE * sizeof(int));

      m6[0] =  m5[0] + m5[2];
      m6[1] =  m5[0] - m5[2];
      m6[2] = (m5[1] >> 1) - m5[3];
      m6[3] =  m5[1] + (m5[3] >> 1);

      img->m7[j][0] = m6[0] + m6[3];
      img->m7[j][3] = m6[0] - m6[3];
      img->m7[j][1] = m6[1] + m6[2];
      img->m7[j][2] = m6[1] - m6[2];
    }
    // vertical
    for (i=0;i<BLOCK_SIZE;i++)
    {
      int ipos = i+ioff;

      m5[0]=img->m7[0][i];
      m5[1]=img->m7[1][i];
      m5[2]=img->m7[2][i];
      m5[3]=img->m7[3][i];

      m6[0] =  m5[0] + m5[2];
      m6[1] =  m5[0] - m5[2];
      m6[2] = (m5[1]>>1) - m5[3];
      m6[3] =  m5[1] + (m5[3]>>1);

      img->m7[0][i] = iClip1(max_imgpel_value, rshift_rnd_sf((m6[0] + m6[3] + ((long)img->mpr[    joff][ipos] << DQ_BITS)), DQ_BITS));
      img->m7[1][i] = iClip1(max_imgpel_value, rshift_rnd_sf((m6[1] + m6[2] + ((long)img->mpr[1 + joff][ipos] << DQ_BITS)), DQ_BITS));
      img->m7[2][i] = iClip1(max_imgpel_value, rshift_rnd_sf((m6[1] - m6[2] + ((long)img->mpr[2 + joff][ipos] << DQ_BITS)), DQ_BITS));
      img->m7[3][i] = iClip1(max_imgpel_value, rshift_rnd_sf((m6[0] - m6[3] + ((long)img->mpr[3 + joff][ipos] << DQ_BITS)), DQ_BITS));
    }
  }
  else
  {
      for (j=0;j<BLOCK_SIZE;j++)
        for (i=0;i<BLOCK_SIZE ;i++)
        img->m7[j][i] = iClip1(max_imgpel_value, (img->cof[i0][j0][j][i]+(long)img->mpr[j+joff][i+ioff]));
  }
}

/*!
 ************************************************************************
 * \brief
 *    For mapping the q-matrix to the active id and calculate quantisation values
 *
 * \param pps
 *    Picture parameter set
 * \param sps
 *    Sequence parameter set
 *
 ************************************************************************
 */
void AssignQuantParam(pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps)
{
  int i;

  if(!pps->pic_scaling_matrix_present_flag && !sps->seq_scaling_matrix_present_flag)
  {
    for(i=0; i<8; i++)
      qmatrix[i] = (i<6) ? quant_org:quant8_org;
  }
  else
  {
    if(sps->seq_scaling_matrix_present_flag) // check sps first
    {
      for(i=0; i<8; i++)
      {
        if(i<6)
        {
          if(!sps->seq_scaling_list_present_flag[i]) // fall-back rule A
          {
            if((i==0) || (i==3))
              qmatrix[i] = (i==0) ? quant_intra_default:quant_inter_default;
            else
              qmatrix[i] = qmatrix[i-1];
          }
          else
          {
            if(sps->UseDefaultScalingMatrix4x4Flag[i])
              qmatrix[i] = (i<3) ? quant_intra_default:quant_inter_default;
            else
              qmatrix[i] = sps->ScalingList4x4[i];
          }
        }
        else
        {
          if(!sps->seq_scaling_list_present_flag[i] || sps->UseDefaultScalingMatrix8x8Flag[i-6]) // fall-back rule A
            qmatrix[i] = (i==6) ? quant8_intra_default:quant8_inter_default;
          else
            qmatrix[i] = sps->ScalingList8x8[i-6];
        }
      }
    }

    if(pps->pic_scaling_matrix_present_flag) // then check pps
    {
      for(i=0; i<8; i++)
      {
        if(i<6)
        {
          if(!pps->pic_scaling_list_present_flag[i]) // fall-back rule B
          {
            if((i==0) || (i==3))
            {
              if(!sps->seq_scaling_matrix_present_flag)
                qmatrix[i] = (i==0) ? quant_intra_default:quant_inter_default;
            }
            else
              qmatrix[i] = qmatrix[i-1];
          }
          else
          {
            if(pps->UseDefaultScalingMatrix4x4Flag[i])
              qmatrix[i] = (i<3) ? quant_intra_default:quant_inter_default;
            else
              qmatrix[i] = pps->ScalingList4x4[i];
          }
        }
        else
        {
          if(!pps->pic_scaling_list_present_flag[i]) // fall-back rule B
          {
            if(!sps->seq_scaling_matrix_present_flag)
              qmatrix[i] = (i==6) ? quant8_intra_default:quant8_inter_default;
          }
          else if(pps->UseDefaultScalingMatrix8x8Flag[i-6])
            qmatrix[i] = (i==6) ? quant8_intra_default:quant8_inter_default;
          else
            qmatrix[i] = pps->ScalingList8x8[i-6];
        }
      }
    }
  }

  CalculateQuantParam();
  if(pps->transform_8x8_mode_flag)
    CalculateQuant8Param();
}

/*!
 ************************************************************************
 * \brief
 *    For calculating the quantisation values at frame level
 *
 ************************************************************************
 */
void CalculateQuantParam()
{
  int i, j, k, temp;

  for(k=0; k<6; k++)
    for(j=0; j<4; j++)
      for(i=0; i<4; i++)
      {
        temp = (i<<2)+j;
        InvLevelScale4x4Luma_Intra[k][i][j]      = dequant_coef[k][j][i]*qmatrix[0][temp];
        InvLevelScale4x4Chroma_Intra[0][k][i][j] = dequant_coef[k][j][i]*qmatrix[1][temp];
        InvLevelScale4x4Chroma_Intra[1][k][i][j] = dequant_coef[k][j][i]*qmatrix[2][temp];

        InvLevelScale4x4Luma_Inter[k][i][j]      = dequant_coef[k][j][i]*qmatrix[3][temp];
        InvLevelScale4x4Chroma_Inter[0][k][i][j] = dequant_coef[k][j][i]*qmatrix[4][temp];
        InvLevelScale4x4Chroma_Inter[1][k][i][j] = dequant_coef[k][j][i]*qmatrix[5][temp];
      }
}

/*!
 ***********************************************************************
 * \brief
 *    Luma DC inverse transform
 ***********************************************************************
 */
void itrans_2(struct img_par *img) //!< image parameters
{
  int i,j;
  int M5[4];
  int M6[4];

  int qp_per = (img->qp + img->bitdepth_luma_qp_scale - MIN_QP)/6;
  int qp_rem = (img->qp + img->bitdepth_luma_qp_scale - MIN_QP)%6;

  // horizontal
  for (j=0;j<4;j++)
  {
    M5[0]=img->cof[0][j][0][0];
    M5[1]=img->cof[1][j][0][0];
    M5[2]=img->cof[2][j][0][0];
    M5[3]=img->cof[3][j][0][0];

    M6[0]=M5[0]+M5[2];
    M6[1]=M5[0]-M5[2];
    M6[2]=M5[1]-M5[3];
    M6[3]=M5[1]+M5[3];

    img->cof[0][j][0][0] = M6[0]+M6[3];
    img->cof[1][j][0][0] = M6[1]+M6[2];
    img->cof[2][j][0][0] = M6[1]-M6[2];
    img->cof[3][j][0][0] = M6[0]-M6[3];
  }

  // vertical
  for (i=0;i<4;i++)
  {
    M5[0]=img->cof[i][0][0][0];
    M5[1]=img->cof[i][1][0][0];
    M5[2]=img->cof[i][2][0][0];
    M5[3]=img->cof[i][3][0][0];

    M6[0]=M5[0]+M5[2];
    M6[1]=M5[0]-M5[2];
    M6[2]=M5[1]-M5[3];
    M6[3]=M5[1]+M5[3];

    img->cof[i][0][0][0] = rshift_rnd((((M6[0]+M6[3])*InvLevelScale4x4Luma_Intra[qp_rem][0][0]) << qp_per), 6);
    img->cof[i][1][0][0] = rshift_rnd((((M6[1]+M6[2])*InvLevelScale4x4Luma_Intra[qp_rem][0][0]) << qp_per), 6);
    img->cof[i][2][0][0] = rshift_rnd((((M6[1]-M6[2])*InvLevelScale4x4Luma_Intra[qp_rem][0][0]) << qp_per), 6);
    img->cof[i][3][0][0] = rshift_rnd((((M6[0]-M6[3])*InvLevelScale4x4Luma_Intra[qp_rem][0][0]) << qp_per), 6);
  }
}


void itrans_sp(struct img_par *img,  //!< image parameters
               int ioff,             //!< index to 4x4 block
               int joff,             //!<
               int i0,               //!<
               int j0)               //!<
{
  int i,j,i1,j1;
  int m5[4];
  int m6[4];
  int predicted_block[BLOCK_SIZE][BLOCK_SIZE],ilev;

  int qp_per = (img->qp-MIN_QP)/6;
  int qp_rem = (img->qp-MIN_QP)%6;
  int q_bits    = Q_BITS+qp_per;

  int qp_per_sp = (img->qpsp-MIN_QP)/6;
  int qp_rem_sp = (img->qpsp-MIN_QP)%6;
  int q_bits_sp = Q_BITS+qp_per_sp;
  int qp_const2 = (1<<q_bits_sp)/2;  //sp_pred
  if (img->type == SI_SLICE) //ES modified
  {
    qp_per = (img->qpsp-MIN_QP)/6;
    qp_rem = (img->qpsp-MIN_QP)%6;
    q_bits = Q_BITS+qp_per;
  }

  for (j=0; j< BLOCK_SIZE; j++)
  for (i=0; i< BLOCK_SIZE; i++)
      predicted_block[i][j]=img->mpr[j+joff][i+ioff];
  for (j=0; j < BLOCK_SIZE; j++)
  {
    for (i=0; i < 2; i++)
    {
      i1=3-i;
      m5[i]=predicted_block[i][j]+predicted_block[i1][j];
      m5[i1]=predicted_block[i][j]-predicted_block[i1][j];
    }
    predicted_block[0][j]=(m5[0]+m5[1]);
    predicted_block[2][j]=(m5[0]-m5[1]);
    predicted_block[1][j]=m5[3]*2+m5[2];
    predicted_block[3][j]=m5[3]-m5[2]*2;
  }

  //  Vertival transform

  for (i=0; i < BLOCK_SIZE; i++)
  {
    for (j=0; j < 2; j++)
    {
      j1=3-j;
      m5[j]=predicted_block[i][j]+predicted_block[i][j1];
      m5[j1]=predicted_block[i][j]-predicted_block[i][j1];
    }
    predicted_block[i][0]=(m5[0]+m5[1]);
    predicted_block[i][2]=(m5[0]-m5[1]);
    predicted_block[i][1]=m5[3]*2+m5[2];
    predicted_block[i][3]=m5[3]-m5[2]*2;
  }

  for (j=0;j<BLOCK_SIZE;j++)
  for (i=0;i<BLOCK_SIZE;i++)
  {
    // recovering coefficient since they are already dequantized earlier
    img->cof[i0][j0][j][i]=(img->cof[i0][j0][j][i] >> qp_per) / dequant_coef[qp_rem][i][j];
    if(img->sp_switch || img->type==SI_SLICE)  //M.W. patched for SI
    {
      ilev=(iabs(predicted_block[i][j]) * quant_coef[qp_rem_sp][i][j] + qp_const2) >> q_bits_sp; //ES added
      ilev= isignab(ilev,predicted_block[i][j])+ img->cof[i0][j0][j][i];                           //ES added
      img->cof[i0][j0][j][i] = isignab(iabs(ilev) * dequant_coef[qp_rem_sp][i][j] << qp_per_sp ,ilev) ; //ES added
    }                                                                                             //ES added
    else
    {                                                                                          //ES added
      ilev=((img->cof[i0][j0][j][i]*dequant_coef[qp_rem][i][j]*A[i][j]<< qp_per) >>6)+predicted_block[i][j] ;
      img->cof[i0][j0][j][i]=isignab((iabs(ilev) * quant_coef[qp_rem_sp][i][j] + qp_const2) >> q_bits_sp, ilev) * dequant_coef[qp_rem_sp][i][j] << qp_per_sp;
    }
  }
  // horizontal
  for (j=0;j<BLOCK_SIZE;j++)
  {
    for (i=0;i<BLOCK_SIZE;i++)
    {
      m5[i]=img->cof[i0][j0][j][i];
    }
    m6[0]=(m5[0]+m5[2]);
    m6[1]=(m5[0]-m5[2]);
    m6[2]=(m5[1]>>1)-m5[3];
    m6[3]=m5[1]+(m5[3]>>1);

    for (i=0;i<2;i++)
    {
      i1=3-i;
      img->m7[j][i]=m6[i]+m6[i1];
      img->m7[j][i1]=m6[i]-m6[i1];
    }
  }
  // vertical
  for (i=0;i<BLOCK_SIZE;i++)
  {
    for (j=0;j<BLOCK_SIZE;j++)
      m5[j]=img->m7[j][i];

    m6[0]=(m5[0]+m5[2]);
    m6[1]=(m5[0]-m5[2]);
    m6[2]=(m5[1]>>1)-m5[3];
    m6[3]=m5[1]+(m5[3]>>1);

    for (j=0;j<2;j++)
    {
      j1=3-j;
      img->m7[j][i] =iClip1(img->max_imgpel_value,rshift_rnd_sf((m6[j]+m6[j1]),DQ_BITS));
      img->m7[j1][i]=iClip1(img->max_imgpel_value,rshift_rnd_sf((m6[j]-m6[j1]),DQ_BITS));
    }
  }
}

/*!
 ***********************************************************************
 * \brief
 *    The routine performs transform,quantization,inverse transform, adds the diff.
 *    to the prediction and writes the result to the decoded luma frame. Includes the
 *    RD constrained quantization also.
 *
 * \par Input:
 *    block_x,block_y: Block position inside a macro block (0,4,8,12).
 *
 * \par Output:
 *    nonzero: 0 if no levels are nonzero.  1 if there are nonzero levels. \n
 *    coeff_cost: Counter for nonzero coefficients, used to discard expencive levels.
 ************************************************************************
 */
void copyblock_sp(struct img_par *img,int block_x,int block_y)
{
  int i,j,i1,j1,m5[4],m6[4];

  int predicted_block[BLOCK_SIZE][BLOCK_SIZE];
  int qp_per = (img->qpsp-MIN_QP)/6;
  int qp_rem = (img->qpsp-MIN_QP)%6;
  int q_bits    = Q_BITS+qp_per;
  int qp_const2=(1<<q_bits)/2;  //sp_pred


  //  Horizontal transform
  for (j=0; j< BLOCK_SIZE; j++)
  for (i=0; i< BLOCK_SIZE; i++)
    predicted_block[i][j]=img->mpr[j+block_y][i+block_x];

  for (j=0; j < BLOCK_SIZE; j++)
  {
    for (i=0; i < 2; i++)
    {
      i1=3-i;
      m5[i]=predicted_block[i][j]+predicted_block[i1][j];
      m5[i1]=predicted_block[i][j]-predicted_block[i1][j];
    }
    predicted_block[0][j]=(m5[0]+m5[1]);
    predicted_block[2][j]=(m5[0]-m5[1]);
    predicted_block[1][j]=m5[3]*2+m5[2];
    predicted_block[3][j]=m5[3]-m5[2]*2;
  }

  //  Vertival transform

  for (i=0; i < BLOCK_SIZE; i++)
  {
    for (j=0; j < 2; j++)
    {
      j1=3-j;
      m5[j]=predicted_block[i][j]+predicted_block[i][j1];
      m5[j1]=predicted_block[i][j]-predicted_block[i][j1];
    }
    predicted_block[i][0]=(m5[0]+m5[1]);
    predicted_block[i][2]=(m5[0]-m5[1]);
    predicted_block[i][1]=m5[3]*2+m5[2];
    predicted_block[i][3]=m5[3]-m5[2]*2;
  }

  // Quant
  for (j=0;j < BLOCK_SIZE; j++)
  for (i=0; i < BLOCK_SIZE; i++)
    img->m7[j][i]=isignab((iabs(predicted_block[i][j])* quant_coef[qp_rem][i][j]+qp_const2)>> q_bits,predicted_block[i][j])*dequant_coef[qp_rem][i][j]<<qp_per;

  //     IDCT.
  //     horizontal

  for (j=0;j<BLOCK_SIZE;j++)
  {
    for (i=0;i<BLOCK_SIZE;i++)
    {
      m5[i]=img->m7[j][i];
    }
    m6[0]=(m5[0]+m5[2]);
    m6[1]=(m5[0]-m5[2]);
    m6[2]=(m5[1]>>1)-m5[3];
    m6[3]=m5[1]+(m5[3]>>1);

    for (i=0;i<2;i++)
    {
      i1=3-i;
      img->m7[j][i]=m6[i]+m6[i1];
      img->m7[j][i1]=m6[i]-m6[i1];
    }
  }
  // vertical
  for (i=0;i<BLOCK_SIZE;i++)
  {
    for (j=0;j<BLOCK_SIZE;j++)
      m5[j]=img->m7[j][i];

    m6[0]=(m5[0]+m5[2]);
    m6[1]=(m5[0]-m5[2]);
    m6[2]=(m5[1]>>1)-m5[3];
    m6[3]=m5[1]+(m5[3]>>1);

    for (j=0;j<2;j++)
    {
      j1=3-j;
      img->m7[j][i] =iClip1(img->max_imgpel_value,rshift_rnd_sf((m6[j]+m6[j1]),DQ_BITS));
      img->m7[j1][i]=iClip1(img->max_imgpel_value,rshift_rnd_sf((m6[j]-m6[j1]),DQ_BITS));
    }
  }

  //  Decoded block moved to frame memory

  for (j=0; j < BLOCK_SIZE; j++)
    for (i=0; i < BLOCK_SIZE; i++)
      dec_picture->imgY[img->pix_y+block_y+j][img->pix_x+block_x+i]=(imgpel) img->m7[j][i];

}

void itrans_sp_chroma(struct img_par *img,int ll)
{
  int i,j,i1,j2,ilev,n2,n1,j1,mb_y;
  int m5[BLOCK_SIZE];
  int predicted_chroma_block[MB_BLOCK_SIZE/2][MB_BLOCK_SIZE/2],mp1[BLOCK_SIZE];
  int qp_per,qp_rem,q_bits;
  int qp_per_sp,qp_rem_sp,q_bits_sp,qp_const2;

  qp_per    = ((img->qp<0?img->qp:QP_SCALE_CR[img->qp])-MIN_QP)/6;
  qp_rem    = ((img->qp<0?img->qp:QP_SCALE_CR[img->qp])-MIN_QP)%6;
  q_bits    = Q_BITS+qp_per;

  qp_per_sp    = ((img->qpsp<0?img->qpsp:QP_SCALE_CR[img->qpsp])-MIN_QP)/6;
  qp_rem_sp    = ((img->qpsp<0?img->qpsp:QP_SCALE_CR[img->qpsp])-MIN_QP)%6;
  q_bits_sp    = Q_BITS+qp_per_sp;
  qp_const2=(1<<q_bits_sp)/2;  //sp_pred

  if (img->type == SI_SLICE)
  {
    qp_per    = ((img->qpsp < 0 ? img->qpsp : QP_SCALE_CR[img->qpsp]) - MIN_QP) / 6;
    qp_rem    = ((img->qpsp < 0 ? img->qpsp : QP_SCALE_CR[img->qpsp]) - MIN_QP) % 6;
    q_bits    = Q_BITS + qp_per;
  }

  for (j=0; j < MB_BLOCK_SIZE/2; j++)
  for (i=0; i < MB_BLOCK_SIZE/2; i++)
  {
    predicted_chroma_block[i][j]=img->mpr[j][i];
    img->mpr[j][i]=0;
  }
  for (n2=0; n2 <= BLOCK_SIZE; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 <= BLOCK_SIZE; n1 += BLOCK_SIZE)
    {
      //  Horizontal transform.
      for (j=0; j < BLOCK_SIZE; j++)
      {
        mb_y=n2+j;
        for (i=0; i < 2; i++)
        {
          i1=3-i;
          m5[i]=predicted_chroma_block[i+n1][mb_y]+predicted_chroma_block[i1+n1][mb_y];
          m5[i1]=predicted_chroma_block[i+n1][mb_y]-predicted_chroma_block[i1+n1][mb_y];
        }
        predicted_chroma_block[n1][mb_y]  =(m5[0]+m5[1]);
        predicted_chroma_block[n1+2][mb_y]=(m5[0]-m5[1]);
        predicted_chroma_block[n1+1][mb_y]=m5[3]*2+m5[2];
        predicted_chroma_block[n1+3][mb_y]=m5[3]-m5[2]*2;
      }

      //  Vertical transform.

      for (i=0; i < BLOCK_SIZE; i++)
      {
        j1=n1+i;
        for (j=0; j < 2; j++)
        {
          j2=3-j;
          m5[j]=predicted_chroma_block[j1][n2+j]+predicted_chroma_block[j1][n2+j2];
          m5[j2]=predicted_chroma_block[j1][n2+j]-predicted_chroma_block[j1][n2+j2];
        }
        predicted_chroma_block[j1][n2+0]=(m5[0]+m5[1]);
        predicted_chroma_block[j1][n2+2]=(m5[0]-m5[1]);
        predicted_chroma_block[j1][n2+1]=m5[3]*2+m5[2];
        predicted_chroma_block[j1][n2+3]=m5[3]-m5[2]*2;
      }
    }
  }

  //     2X2 transform of DC coeffs.
  mp1[0]=(predicted_chroma_block[0][0]+predicted_chroma_block[4][0]+predicted_chroma_block[0][4]+predicted_chroma_block[4][4]);
  mp1[1]=(predicted_chroma_block[0][0]-predicted_chroma_block[4][0]+predicted_chroma_block[0][4]-predicted_chroma_block[4][4]);
  mp1[2]=(predicted_chroma_block[0][0]+predicted_chroma_block[4][0]-predicted_chroma_block[0][4]-predicted_chroma_block[4][4]);
  mp1[3]=(predicted_chroma_block[0][0]-predicted_chroma_block[4][0]-predicted_chroma_block[0][4]+predicted_chroma_block[4][4]);

  for (n1=0; n1 < 2; n1 ++)
  for (n2=0; n2 < 2; n2 ++)
  {
    if (img->sp_switch || img->type==SI_SLICE)  //M.W. patched for SI
    {
      //quantization fo predicted block
      ilev=(iabs (mp1[n1+n2*2]) * quant_coef[qp_rem_sp][0][0] + 2 * qp_const2) >> (q_bits_sp + 1);
      //addition
      ilev=img->cof[n1+ll][4+n2][0][0]+isignab(ilev,mp1[n1+n2*2]);
      //dequantization
      mp1[n1+n2*2] =ilev*dequant_coef[qp_rem_sp][0][0]<<qp_per_sp;
    }
    else
    {
      ilev=((img->cof[n1+ll][4+n2][0][0]*dequant_coef[qp_rem][0][0]*A[0][0]<< qp_per) >>5)+mp1[n1+n2*2] ;
      mp1[n1+n2*2]=isignab((iabs(ilev)* quant_coef[qp_rem_sp][0][0]+ 2 * qp_const2)>> (q_bits_sp+1),ilev)*dequant_coef[qp_rem_sp][0][0]<<qp_per_sp;
    }
  }


  for (n2=0; n2 < 2; n2 ++)
  for (n1=0; n1 < 2; n1 ++)
  for (i=0;i< BLOCK_SIZE; i++)
  for (j=0;j< BLOCK_SIZE; j++)
  {
  // recovering coefficient since they are already dequantized earlier
    img->cof[n1+ll][4+n2][j][i] = (img->cof[n1+ll][4+n2][j][i] >> qp_per) / dequant_coef[qp_rem][i][j];

    if (img->sp_switch || img->type==SI_SLICE)  //M.W. patched for SI
    {
      //quantization of the predicted block
      ilev =  (iabs(predicted_chroma_block[n1*BLOCK_SIZE+i][n2*BLOCK_SIZE+j]) * quant_coef[qp_rem_sp][i][j] + qp_const2) >> q_bits_sp;
      //addition of the residual
      ilev = isignab(ilev,predicted_chroma_block[n1*BLOCK_SIZE+i][n2*BLOCK_SIZE+j]) + img->cof[n1+ll][4+n2][j][i];
      // Inverse quantization
      img->cof[n1+ll][4+n2][j][i] = ilev * dequant_coef[qp_rem_sp][i][j] << qp_per_sp  ;
    }
    else
    {
      //dequantization and addition of the predicted block
      ilev=((img->cof[n1+ll][4+n2][j][i]*dequant_coef[qp_rem][i][j]*A[i][j]<< qp_per) >>6)+predicted_chroma_block[n1*BLOCK_SIZE+i][n2*BLOCK_SIZE+j] ;
      //quantization and dequantization
      img->cof[n1+ll][4+n2][j][i] = isignab((iabs(ilev) * quant_coef[qp_rem_sp][i][j] + qp_const2)>> q_bits_sp,ilev)*dequant_coef[qp_rem_sp][i][j]<<qp_per_sp;
    }
  }
  img->cof[0+ll][4][0][0]=(mp1[0]+mp1[1]+mp1[2]+mp1[3])>>1;
  img->cof[1+ll][4][0][0]=(mp1[0]-mp1[1]+mp1[2]-mp1[3])>>1;
  img->cof[0+ll][5][0][0]=(mp1[0]+mp1[1]-mp1[2]-mp1[3])>>1;
  img->cof[1+ll][5][0][0]=(mp1[0]-mp1[1]-mp1[2]+mp1[3])>>1;
}

