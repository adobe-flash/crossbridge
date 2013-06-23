
/*!
 ***************************************************************************
 * \file transform8x8.c
 *
 * \brief
 *    8x8 transform functions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Yuri Vatis          <vatis@hhi.de>
 *    - Jan Muenster        <muenster@hhi.de>
 *
 * \date
 *    12. October 2003
 **************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include "transform8x8.h"

#define Q_BITS_8        16
#define DQ_BITS_8       6 

const int quant_coef8[6][8][8] =
{
  {
    {13107, 12222,  16777,  12222,  13107,  12222,  16777,  12222},
    {12222, 11428,  15481,  11428,  12222,  11428,  15481,  11428},
    {16777, 15481,  20972,  15481,  16777,  15481,  20972,  15481},
    {12222, 11428,  15481,  11428,  12222,  11428,  15481,  11428},
    {13107, 12222,  16777,  12222,  13107,  12222,  16777,  12222},
    {12222, 11428,  15481,  11428,  12222,  11428,  15481,  11428},
    {16777, 15481,  20972,  15481,  16777,  15481,  20972,  15481},
    {12222, 11428,  15481,  11428,  12222,  11428,  15481,  11428}
  },
  {
    {11916, 11058,  14980,  11058,  11916,  11058,  14980,  11058},
    {11058, 10826,  14290,  10826,  11058,  10826,  14290,  10826},
    {14980, 14290,  19174,  14290,  14980,  14290,  19174,  14290},
    {11058, 10826,  14290,  10826,  11058,  10826,  14290,  10826},
    {11916, 11058,  14980,  11058,  11916,  11058,  14980,  11058},
    {11058, 10826,  14290,  10826,  11058,  10826,  14290,  10826},
    {14980, 14290,  19174,  14290,  14980,  14290,  19174,  14290},
    {11058, 10826,  14290,  10826,  11058,  10826,  14290,  10826}
  },
  {
    {10082, 9675, 12710,  9675, 10082,  9675, 12710,  9675},
    {9675,  8943, 11985,  8943, 9675, 8943, 11985,  8943},
    {12710, 11985,  15978,  11985,  12710,  11985,  15978,  11985},
    {9675,  8943, 11985,  8943, 9675, 8943, 11985,  8943},
    {10082, 9675, 12710,  9675, 10082,  9675, 12710,  9675},
    {9675,  8943, 11985,  8943, 9675, 8943, 11985,  8943},
    {12710, 11985,  15978,  11985,  12710,  11985,  15978,  11985},
    {9675,  8943, 11985,  8943, 9675, 8943, 11985,  8943}
  },
  {
    {9362,  8931, 11984,  8931, 9362, 8931, 11984,  8931},
    {8931,  8228, 11259,  8228, 8931, 8228, 11259,  8228},
    {11984, 11259,  14913,  11259,  11984,  11259,  14913,  11259},
    {8931,  8228, 11259,  8228, 8931, 8228, 11259,  8228},
    {9362,  8931, 11984,  8931, 9362, 8931, 11984,  8931},
    {8931,  8228, 11259,  8228, 8931, 8228, 11259,  8228},
    {11984, 11259,  14913,  11259,  11984,  11259,  14913,  11259},
    {8931,  8228, 11259,  8228, 8931, 8228, 11259,  8228}
  },
  {
    {8192,  7740, 10486,  7740, 8192, 7740, 10486,  7740},
    {7740,  7346, 9777, 7346, 7740, 7346, 9777, 7346},
    {10486, 9777, 13159,  9777, 10486,  9777, 13159,  9777},
    {7740,  7346, 9777, 7346, 7740, 7346, 9777, 7346},
    {8192,  7740, 10486,  7740, 8192, 7740, 10486,  7740},
    {7740,  7346, 9777, 7346, 7740, 7346, 9777, 7346},
    {10486, 9777, 13159,  9777, 10486,  9777, 13159,  9777},
    {7740,  7346, 9777, 7346, 7740, 7346, 9777, 7346}
  },
  {
    {7282,  6830, 9118, 6830, 7282, 6830, 9118, 6830},
    {6830,  6428, 8640, 6428, 6830, 6428, 8640, 6428},
    {9118,  8640, 11570,  8640, 9118, 8640, 11570,  8640},
    {6830,  6428, 8640, 6428, 6830, 6428, 8640, 6428},
    {7282,  6830, 9118, 6830, 7282, 6830, 9118, 6830},
    {6830,  6428, 8640, 6428, 6830, 6428, 8640, 6428},
    {9118,  8640, 11570,  8640, 9118, 8640, 11570,  8640},
    {6830,  6428, 8640, 6428, 6830, 6428, 8640, 6428}
  }
};



const int dequant_coef8[6][8][8] =
{
  {
    {20,  19, 25, 19, 20, 19, 25, 19},
    {19,  18, 24, 18, 19, 18, 24, 18},
    {25,  24, 32, 24, 25, 24, 32, 24},
    {19,  18, 24, 18, 19, 18, 24, 18},
    {20,  19, 25, 19, 20, 19, 25, 19},
    {19,  18, 24, 18, 19, 18, 24, 18},
    {25,  24, 32, 24, 25, 24, 32, 24},
    {19,  18, 24, 18, 19, 18, 24, 18}
  },
  {
    {22,  21, 28, 21, 22, 21, 28, 21},
    {21,  19, 26, 19, 21, 19, 26, 19},
    {28,  26, 35, 26, 28, 26, 35, 26},
    {21,  19, 26, 19, 21, 19, 26, 19},
    {22,  21, 28, 21, 22, 21, 28, 21},
    {21,  19, 26, 19, 21, 19, 26, 19},
    {28,  26, 35, 26, 28, 26, 35, 26},
    {21,  19, 26, 19, 21, 19, 26, 19}
  },
  {
    {26,  24, 33, 24, 26, 24, 33, 24},
    {24,  23, 31, 23, 24, 23, 31, 23},
    {33,  31, 42, 31, 33, 31, 42, 31},
    {24,  23, 31, 23, 24, 23, 31, 23},
    {26,  24, 33, 24, 26, 24, 33, 24},
    {24,  23, 31, 23, 24, 23, 31, 23},
    {33,  31, 42, 31, 33, 31, 42, 31},
    {24,  23, 31, 23, 24, 23, 31, 23}
  },
  {
    {28,  26, 35, 26, 28, 26, 35, 26},
    {26,  25, 33, 25, 26, 25, 33, 25},
    {35,  33, 45, 33, 35, 33, 45, 33},
    {26,  25, 33, 25, 26, 25, 33, 25},
    {28,  26, 35, 26, 28, 26, 35, 26},
    {26,  25, 33, 25, 26, 25, 33, 25},
    {35,  33, 45, 33, 35, 33, 45, 33},
    {26,  25, 33, 25, 26, 25, 33, 25}
  },
  {
    {32,  30, 40, 30, 32, 30, 40, 30},
    {30,  28, 38, 28, 30, 28, 38, 28},
    {40,  38, 51, 38, 40, 38, 51, 38},
    {30,  28, 38, 28, 30, 28, 38, 28},
    {32,  30, 40, 30, 32, 30, 40, 30},
    {30,  28, 38, 28, 30, 28, 38, 28},
    {40,  38, 51, 38, 40, 38, 51, 38},
    {30,  28, 38, 28, 30, 28, 38, 28}
  },
  {
    {36,  34, 46, 34, 36, 34, 46, 34},
    {34,  32, 43, 32, 34, 32, 43, 32},
    {46,  43, 58, 43, 46, 43, 58, 43},
    {34,  32, 43, 32, 34, 32, 43, 32},
    {36,  34, 46, 34, 36, 34, 46, 34},
    {34,  32, 43, 32, 34, 32, 43, 32},
    {46,  43, 58, 43, 46, 43, 58, 43},
    {34,  32, 43, 32, 34, 32, 43, 32}
  }

};



#ifdef _NEW_8x8_ARRAYS_INCLUDED_
//! single scan pattern
const byte SNGL_SCAN8x8[64][2] = {
  {0,0}, {1,0}, {0,1}, {0,2}, {1,1}, {2,0}, {3,0}, {2,1}, {1,2}, {0,3}, {0,4}, {1,3}, {2,2}, {3,1}, {4,0}, {5,0},
  {4,1}, {3,2}, {2,3}, {1,4}, {0,5}, {0,6}, {1,5}, {2,4}, {3,3}, {4,2}, {5,1}, {6,0}, {7,0}, {6,1}, {5,2}, {4,3},
  {3,4}, {2,5}, {1,6}, {0,7}, {1,7}, {2,6}, {3,5}, {4,4}, {5,3}, {6,2}, {7,1}, {7,2}, {6,3}, {5,4}, {4,5}, {3,6},
  {2,7}, {3,7}, {4,6}, {5,5}, {6,4}, {7,3}, {7,4}, {6,5}, {5,6}, {4,7}, {5,7}, {6,6}, {7,5}, {7,6}, {6,7}, {7,7}
};

//! field scan pattern
const byte FIELD_SCAN8x8[64][2] = {
 {0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {0,3}, {0,4}, {1,2}, {2,0}, {2,1}, {1,3}, {0,5}, {0,6}, {1,4}, {2,2}, {3,0},
 {3,1}, {2,3}, {1,5}, {0,7}, {1,6}, {2,4}, {3,2}, {4,0}, {4,1}, {3,3}, {2,5}, {1,7}, {2,6}, {3,4}, {4,2}, {5,0},
 {5,1}, {4,3}, {3,5}, {2,7}, {3,6}, {4,4}, {5,2}, {6,0}, {6,1}, {5,3}, {4,5}, {3,7}, {4,6}, {5,4}, {6,2}, {7,0},
 {7,1}, {6,3}, {5,5}, {4,7}, {5,6}, {6,4}, {7,2}, {7,3}, {6,5}, {5,7}, {6,6}, {7,4}, {7,5}, {6,7}, {7,6}, {7,7}
};


//! array used to find expencive coefficients
const byte COEFF_COST8x8[64] =
{
  3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,
  1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

#endif

// Notation for comments regarding prediction and predictors.
// The pels of the 4x4 block are labelled a..p. The predictor pels above
// are labelled A..H, from the left I..P, and from above left X, as follows:
//
//  Z  A  B  C  D  E  F  G  H  I  J  K  L  M   N  O  P
//  Q  a1 b1 c1 d1 e1 f1 g1 h1
//  R  a2 b2 c2 d2 e2 f2 g2 h2
//  S  a3 b3 c3 d3 e3 f3 g3 h3
//  T  a4 b4 c4 d4 e4 f4 g4 h4
//  U  a5 b5 c5 d5 e5 f5 g5 h5
//  V  a6 b6 c6 d6 e6 f6 g6 h6
//  W  a7 b7 c7 d7 e7 f7 g7 h7
//  X  a8 b8 c8 d8 e8 f8 g8 h8


// Predictor array index definitions
#define P_Z (PredPel[0])
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
#define P_M (PredPel[13])
#define P_N (PredPel[14])
#define P_O (PredPel[15])
#define P_P (PredPel[16])
#define P_Q (PredPel[17])
#define P_R (PredPel[18])
#define P_S (PredPel[19])
#define P_T (PredPel[20])
#define P_U (PredPel[21])
#define P_V (PredPel[22])
#define P_W (PredPel[23])
#define P_X (PredPel[24])

/*!
 ************************************************************************
 * \brief
 *    Make intra 8x8 prediction according to all 9 prediction modes.
 *    The routine uses left and upper neighbouring points from
 *    previous coded blocks to do this (if available). Notice that
 *    inaccessible neighbouring points are signalled with a negative
 *    value in the predmode array .
 *
 *  \par Input:
 *     Starting point of current 8x8 block image posision
 *
 ************************************************************************
 */
int intrapred8x8( struct img_par *img,  //!< image parameters
                  int b8)

{
  int i,j;
  int s0;
  imgpel PredPel[25];  // array of predictor pels
  imgpel **imgY = dec_picture->imgY;  // For MB level frame/field coding tools -- set default to imgY

  int mb_nr=img->current_mb_nr;

  PixelPos pix_a[8];
  PixelPos pix_b, pix_c, pix_d;

  int block_available_up;
  int block_available_left;
  int block_available_up_left;
  int block_available_up_right;
  int img_block_x = (img->mb_x)*4 + 2*(b8%2);
  int img_block_y = (img->mb_y)*4 + 2*(b8/2);
  int ioff = (b8%2)*8;
  int joff = (b8/2)*8;
  int jpos0 = joff    , jpos1 = joff + 1, jpos2 = joff + 2, jpos3 = joff + 3;
  int jpos4 = joff + 4, jpos5 = joff + 5, jpos6 = joff + 6, jpos7 = joff + 7;
  int ipos0 = ioff    , ipos1 = ioff + 1, ipos2 = ioff + 2, ipos3 = ioff + 3;
  int ipos4 = ioff + 4, ipos5 = ioff + 5, ipos6 = ioff + 6, ipos7 = ioff + 7;
  int jpos, ipos;
  imgpel *pred_pels;

  byte predmode = img->ipredmode[img_block_y][img_block_x];

  for (i=0;i<8;i++)
  {
    getNeighbour(mb_nr, ioff -1 , joff +i , IS_LUMA, &pix_a[i]);
  }

  getNeighbour(mb_nr, ioff    , joff -1 , IS_LUMA, &pix_b);
  getNeighbour(mb_nr, ioff +8 , joff -1 , IS_LUMA, &pix_c);
  getNeighbour(mb_nr, ioff -1 , joff -1 , IS_LUMA, &pix_d);

  pix_c.available = pix_c.available &&!(ioff == 8 && joff == 8);

  if (active_pps->constrained_intra_pred_flag)
  {
    for (i=0, block_available_left=1; i<8;i++)
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

//  *left_available = block_available_left;
//  *up_available   = block_available_up;
//  *all_available  = block_available_up && block_available_left && block_available_up_left;

  // form predictor pels
  // form predictor pels
  if (block_available_up)
  {
    pred_pels = &imgY[pix_b.pos_y][pix_b.pos_x];
    P_A = pred_pels[0];
    P_B = pred_pels[1];
    P_C = pred_pels[2];
    P_D = pred_pels[3];
    P_E = pred_pels[4];
    P_F = pred_pels[5];
    P_G = pred_pels[6];
    P_H = pred_pels[7];
  }
  else
  {
    P_A = P_B = P_C = P_D = P_E = P_F = P_G = P_H = img->dc_pred_value_luma;
  }

  if (block_available_up_right)
  {
    pred_pels = &imgY[pix_c.pos_y][pix_c.pos_x];
    P_I = pred_pels[0];
    P_J = pred_pels[1];
    P_K = pred_pels[2];
    P_L = pred_pels[3];
    P_M = pred_pels[4];
    P_N = pred_pels[5];
    P_O = pred_pels[6];
    P_P = pred_pels[7];

  }
  else
  {
    P_I = P_J = P_K = P_L = P_M = P_N = P_O = P_P = P_H;
  }

  if (block_available_left)
  {
    P_Q = imgY[pix_a[0].pos_y][pix_a[0].pos_x];
    P_R = imgY[pix_a[1].pos_y][pix_a[1].pos_x];
    P_S = imgY[pix_a[2].pos_y][pix_a[2].pos_x];
    P_T = imgY[pix_a[3].pos_y][pix_a[3].pos_x];
    P_U = imgY[pix_a[4].pos_y][pix_a[4].pos_x];
    P_V = imgY[pix_a[5].pos_y][pix_a[5].pos_x];
    P_W = imgY[pix_a[6].pos_y][pix_a[6].pos_x];
    P_X = imgY[pix_a[7].pos_y][pix_a[7].pos_x];
  }
  else
  {
    P_Q = P_R = P_S = P_T = P_U = P_V = P_W = P_X = img->dc_pred_value_luma;
  }

  if (block_available_up_left)
  {
    P_Z = imgY[pix_d.pos_y][pix_d.pos_x];
  }
  else
  {
    P_Z = img->dc_pred_value_luma;
  }

  LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);

//img->mpr[y][x]
  switch(predmode)
  {
  case DC_PRED:
    s0 = 0;
    if (block_available_up && block_available_left)
    {
      // no edge
      s0 = (P_A + P_B + P_C + P_D + P_E + P_F + P_G + P_H + P_Q + P_R + P_S + P_T + P_U + P_V + P_W + P_X + 8) >> 4;
    }
    else if (!block_available_up && block_available_left)
    {
      // upper edge
      s0 = (P_Q + P_R + P_S + P_T + P_U + P_V + P_W + P_X + 4) >> 3;
    }
    else if (block_available_up && !block_available_left)
    {
      // left edge
      s0 = (P_A + P_B + P_C + P_D + P_E + P_F + P_G + P_H + 4) >> 3;
    }
    else //if (!block_available_up && !block_available_left)
    {
      // top left corner, nothing to predict from
      s0 = img->dc_pred_value_luma;
    }
    for(j = joff; j < joff + 2*BLOCK_SIZE; j++)
      for(i = ioff; i < ioff + 2*BLOCK_SIZE; i++)
        img->mpr[j][i] = (imgpel) s0;
    break;

  case VERT_PRED:
    if (!block_available_up)
      printf ("warning: Intra_8x8_Vertical prediction mode not allowed at mb %d\n",img->current_mb_nr);

    for (i=0; i < 2*BLOCK_SIZE; i++)
    {
      ipos = i+ioff;
      img->mpr[jpos0][ipos] =
      img->mpr[jpos1][ipos] =
      img->mpr[jpos2][ipos] =
      img->mpr[jpos3][ipos] =
      img->mpr[jpos4][ipos] =
      img->mpr[jpos5][ipos] =
      img->mpr[jpos6][ipos] =
      img->mpr[jpos7][ipos] = (imgpel) (&P_A)[i];
    }
    break;
  case HOR_PRED:
    if (!block_available_left)
      printf ("warning: Intra_8x8_Horizontal prediction mode not allowed at mb %d\n",img->current_mb_nr);

    for (j=0; j < 2*BLOCK_SIZE; j++)
    {
      jpos = j + joff;
      img->mpr[jpos][ipos0]  =
      img->mpr[jpos][ipos1]  =
      img->mpr[jpos][ipos2]  =
      img->mpr[jpos][ipos3]  =
      img->mpr[jpos][ipos4]  =
      img->mpr[jpos][ipos5]  =
      img->mpr[jpos][ipos6]  =
      img->mpr[jpos][ipos7]  = (imgpel) (&P_Q)[j];
    }
    break;

  case DIAG_DOWN_LEFT_PRED:
    if (!block_available_up)
      printf ("warning: Intra_8x8_Diagonal_Down_Left prediction mode not allowed at mb %d\n",img->current_mb_nr);
    // Mode DIAG_DOWN_LEFT_PRED
    img->mpr[jpos0][ipos0] = (imgpel) ((P_A + P_C + 2*(P_B) + 2) >> 2);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos0][ipos1] = (imgpel) ((P_B + P_D + 2*(P_C) + 2) >> 2);
    img->mpr[jpos2][ipos0] =
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos0][ipos2] = (imgpel) ((P_C + P_E + 2*(P_D) + 2) >> 2);
    img->mpr[jpos3][ipos0] =
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos0][ipos3] = (imgpel) ((P_D + P_F + 2*(P_E) + 2) >> 2);
    img->mpr[jpos4][ipos0] =
    img->mpr[jpos3][ipos1] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos0][ipos4] = (imgpel) ((P_E + P_G + 2*(P_F) + 2) >> 2);
    img->mpr[jpos5][ipos0] =
    img->mpr[jpos4][ipos1] =
    img->mpr[jpos3][ipos2] =
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos1][ipos4] =
    img->mpr[jpos0][ipos5] = (imgpel) ((P_F + P_H + 2*(P_G) + 2) >> 2);
    img->mpr[jpos6][ipos0] =
    img->mpr[jpos5][ipos1] =
    img->mpr[jpos4][ipos2] =
    img->mpr[jpos3][ipos3] =
    img->mpr[jpos2][ipos4] =
    img->mpr[jpos1][ipos5] =
    img->mpr[jpos0][ipos6] = (imgpel) ((P_G + P_I + 2*(P_H) + 2) >> 2);
    img->mpr[jpos7][ipos0] =
    img->mpr[jpos6][ipos1] =
    img->mpr[jpos5][ipos2] =
    img->mpr[jpos4][ipos3] =
    img->mpr[jpos3][ipos4] =
    img->mpr[jpos2][ipos5] =
    img->mpr[jpos1][ipos6] =
    img->mpr[jpos0][ipos7] = (imgpel) ((P_H + P_J + 2*(P_I) + 2) >> 2);
    img->mpr[jpos7][ipos1] =
    img->mpr[jpos6][ipos2] =
    img->mpr[jpos5][ipos3] =
    img->mpr[jpos4][ipos4] =
    img->mpr[jpos3][ipos5] =
    img->mpr[jpos2][ipos6] =
    img->mpr[jpos1][ipos7] = (imgpel) ((P_I + P_K + 2*(P_J) + 2) >> 2);
    img->mpr[jpos7][ipos2] =
    img->mpr[jpos6][ipos3] =
    img->mpr[jpos5][ipos4] =
    img->mpr[jpos4][ipos5] =
    img->mpr[jpos3][ipos6] =
    img->mpr[jpos2][ipos7] = (imgpel) ((P_J + P_L + 2*(P_K) + 2) >> 2);
    img->mpr[jpos7][ipos3] =
    img->mpr[jpos6][ipos4] =
    img->mpr[jpos5][ipos5] =
    img->mpr[jpos4][ipos6] =
    img->mpr[jpos3][ipos7] = (imgpel) ((P_K + P_M + 2*(P_L) + 2) >> 2);
    img->mpr[jpos7][ipos4] =
    img->mpr[jpos6][ipos5] =
    img->mpr[jpos5][ipos6] =
    img->mpr[jpos4][ipos7] = (imgpel) ((P_L + P_N + 2*(P_M) + 2) >> 2);
    img->mpr[jpos7][ipos5] =
    img->mpr[jpos6][ipos6] =
    img->mpr[jpos5][ipos7] = (imgpel) ((P_M + P_O + 2*(P_N) + 2) >> 2);
    img->mpr[jpos7][ipos6] =
    img->mpr[jpos6][ipos7] = (imgpel) ((P_N + P_P + 2*(P_O) + 2) >> 2);
    img->mpr[jpos7][ipos7] = (imgpel) ((P_O + 3*(P_P) + 2) >> 2);
    break;

  case VERT_LEFT_PRED:
    if (!block_available_up)
      printf ("warning: Intra_4x4_Vertical_Left prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] = (imgpel) ((P_A + P_B + 1) >> 1);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos2][ipos0] = (imgpel) ((P_B + P_C + 1) >> 1);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos4][ipos0] = (imgpel) ((P_C + P_D + 1) >> 1);
    img->mpr[jpos0][ipos3] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos4][ipos1] =
    img->mpr[jpos6][ipos0] = (imgpel) ((P_D + P_E + 1) >> 1);
    img->mpr[jpos0][ipos4] =
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos4][ipos2] =
    img->mpr[jpos6][ipos1] = (imgpel) ((P_E + P_F + 1) >> 1);
    img->mpr[jpos0][ipos5] =
    img->mpr[jpos2][ipos4] =
    img->mpr[jpos4][ipos3] =
    img->mpr[jpos6][ipos2] = (imgpel) ((P_F + P_G + 1) >> 1);
    img->mpr[jpos0][ipos6] =
    img->mpr[jpos2][ipos5] =
    img->mpr[jpos4][ipos4] =
    img->mpr[jpos6][ipos3] = (imgpel) ((P_G + P_H + 1) >> 1);
    img->mpr[jpos0][ipos7] =
    img->mpr[jpos2][ipos6] =
    img->mpr[jpos4][ipos5] =
    img->mpr[jpos6][ipos4] = (imgpel) ((P_H + P_I + 1) >> 1);
    img->mpr[jpos2][ipos7] =
    img->mpr[jpos4][ipos6] =
    img->mpr[jpos6][ipos5] = (imgpel) ((P_I + P_J + 1) >> 1);
    img->mpr[jpos4][ipos7] =
    img->mpr[jpos6][ipos6] = (imgpel) ((P_J + P_K + 1) >> 1);
    img->mpr[jpos6][ipos7] = (imgpel) ((P_K + P_L + 1) >> 1);
    img->mpr[jpos1][ipos0] = (imgpel) ((P_A + P_C + 2*P_B + 2) >> 2);
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos3][ipos0] = (imgpel) ((P_B + P_D + 2*P_C + 2) >> 2);
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos3][ipos1] =
    img->mpr[jpos5][ipos0] = (imgpel) ((P_C + P_E + 2*P_D + 2) >> 2);
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos3][ipos2] =
    img->mpr[jpos5][ipos1] =
    img->mpr[jpos7][ipos0] = (imgpel) ((P_D + P_F + 2*P_E + 2) >> 2);
    img->mpr[jpos1][ipos4] =
    img->mpr[jpos3][ipos3] =
    img->mpr[jpos5][ipos2] =
    img->mpr[jpos7][ipos1] = (imgpel) ((P_E + P_G + 2*P_F + 2) >> 2);
    img->mpr[jpos1][ipos5] =
    img->mpr[jpos3][ipos4] =
    img->mpr[jpos5][ipos3] =
    img->mpr[jpos7][ipos2] = (imgpel) ((P_F + P_H + 2*P_G + 2) >> 2);
    img->mpr[jpos1][ipos6] =
    img->mpr[jpos3][ipos5] =
    img->mpr[jpos5][ipos4] =
    img->mpr[jpos7][ipos3] = (imgpel) ((P_G + P_I + 2*P_H + 2) >> 2);
    img->mpr[jpos1][ipos7] =
    img->mpr[jpos3][ipos6] =
    img->mpr[jpos5][ipos5] =
    img->mpr[jpos7][ipos4] = (imgpel) ((P_H + P_J + 2*P_I + 2) >> 2);
    img->mpr[jpos3][ipos7] =
    img->mpr[jpos5][ipos6] =
    img->mpr[jpos7][ipos5] = (imgpel) ((P_I + P_K + 2*P_J + 2) >> 2);
    img->mpr[jpos5][ipos7] =
    img->mpr[jpos7][ipos6] = (imgpel) ((P_J + P_L + 2*P_K + 2) >> 2);
    img->mpr[jpos7][ipos7] = (imgpel) ((P_K + P_M + 2*P_L + 2) >> 2);
    break;


  case DIAG_DOWN_RIGHT_PRED:
    if ((!block_available_up)||(!block_available_left)||(!block_available_up_left))
      printf ("warning: Intra_8x8_Diagonal_Down_Right prediction mode not allowed at mb %d\n",img->current_mb_nr);

    // Mode DIAG_DOWN_RIGHT_PRED
    img->mpr[jpos7][ipos0] = (imgpel) ((P_X + P_V + 2*(P_W) + 2) >> 2);
    img->mpr[jpos6][ipos0] =
    img->mpr[jpos7][ipos1] = (imgpel) ((P_W + P_U + 2*(P_V) + 2) >> 2);
    img->mpr[jpos5][ipos0] =
    img->mpr[jpos6][ipos1] =
    img->mpr[jpos7][ipos2] = (imgpel) ((P_V + P_T + 2*(P_U) + 2) >> 2);
    img->mpr[jpos4][ipos0] =
    img->mpr[jpos5][ipos1] =
    img->mpr[jpos6][ipos2] =
    img->mpr[jpos7][ipos3] = (imgpel) ((P_U + P_S + 2*(P_T) + 2) >> 2);
    img->mpr[jpos3][ipos0] =
    img->mpr[jpos4][ipos1] =
    img->mpr[jpos5][ipos2] =
    img->mpr[jpos6][ipos3] =
    img->mpr[jpos7][ipos4] = (imgpel) ((P_T + P_R + 2*(P_S) + 2) >> 2);
    img->mpr[jpos2][ipos0] =
    img->mpr[jpos3][ipos1] =
    img->mpr[jpos4][ipos2] =
    img->mpr[jpos5][ipos3] =
    img->mpr[jpos6][ipos4] =
    img->mpr[jpos7][ipos5] = (imgpel) ((P_S + P_Q + 2*(P_R) + 2) >> 2);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos3][ipos2] =
    img->mpr[jpos4][ipos3] =
    img->mpr[jpos5][ipos4] =
    img->mpr[jpos6][ipos5] =
    img->mpr[jpos7][ipos6] = (imgpel) ((P_R + P_Z + 2*(P_Q) + 2) >> 2);
    img->mpr[jpos0][ipos0] =
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos3][ipos3] =
    img->mpr[jpos4][ipos4] =
    img->mpr[jpos5][ipos5] =
    img->mpr[jpos6][ipos6] =
    img->mpr[jpos7][ipos7] = (imgpel) ((P_Q + P_A + 2*(P_Z) + 2) >> 2);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos3][ipos4] =
    img->mpr[jpos4][ipos5] =
    img->mpr[jpos5][ipos6] =
    img->mpr[jpos6][ipos7] = (imgpel) ((P_Z + P_B + 2*(P_A) + 2) >> 2);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos2][ipos4] =
    img->mpr[jpos3][ipos5] =
    img->mpr[jpos4][ipos6] =
    img->mpr[jpos5][ipos7] = (imgpel) ((P_A + P_C + 2*(P_B) + 2) >> 2);
    img->mpr[jpos0][ipos3] =
    img->mpr[jpos1][ipos4] =
    img->mpr[jpos2][ipos5] =
    img->mpr[jpos3][ipos6] =
    img->mpr[jpos4][ipos7] = (imgpel) ((P_B + P_D + 2*(P_C) + 2) >> 2);
    img->mpr[jpos0][ipos4] =
    img->mpr[jpos1][ipos5] =
    img->mpr[jpos2][ipos6] =
    img->mpr[jpos3][ipos7] = (imgpel) ((P_C + P_E + 2*(P_D) + 2) >> 2);
    img->mpr[jpos0][ipos5] =
    img->mpr[jpos1][ipos6] =
    img->mpr[jpos2][ipos7] = (imgpel) ((P_D + P_F + 2*(P_E) + 2) >> 2);
    img->mpr[jpos0][ipos6] =
    img->mpr[jpos1][ipos7] = (imgpel) ((P_E + P_G + 2*(P_F) + 2) >> 2);
    img->mpr[jpos0][ipos7] = (imgpel) ((P_F + P_H + 2*(P_G) + 2) >> 2);
    break;

  case  VERT_RIGHT_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
    if ((!block_available_up)||(!block_available_left)||(!block_available_up_left))
      printf ("warning: Intra_8x8_Vertical_Right prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] =
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos4][ipos2] =
    img->mpr[jpos6][ipos3] = (imgpel) ((P_Z + P_A + 1) >> 1);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos4][ipos3] =
    img->mpr[jpos6][ipos4] = (imgpel) ((P_A + P_B + 1) >> 1);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos4][ipos4] =
    img->mpr[jpos6][ipos5] = (imgpel) ((P_B + P_C + 1) >> 1);
    img->mpr[jpos0][ipos3] =
    img->mpr[jpos2][ipos4] =
    img->mpr[jpos4][ipos5] =
    img->mpr[jpos6][ipos6] = (imgpel) ((P_C + P_D + 1) >> 1);
    img->mpr[jpos0][ipos4] =
    img->mpr[jpos2][ipos5] =
    img->mpr[jpos4][ipos6] =
    img->mpr[jpos6][ipos7] = (imgpel) ((P_D + P_E + 1) >> 1);
    img->mpr[jpos0][ipos5] =
    img->mpr[jpos2][ipos6] =
    img->mpr[jpos4][ipos7] = (imgpel) ((P_E + P_F + 1) >> 1);
    img->mpr[jpos0][ipos6] =
    img->mpr[jpos2][ipos7] = (imgpel) ((P_F + P_G + 1) >> 1);
    img->mpr[jpos0][ipos7] = (imgpel) ((P_G + P_H + 1) >> 1);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos3][ipos1] =
    img->mpr[jpos5][ipos2] =
    img->mpr[jpos7][ipos3] = (imgpel) ((P_Q + P_A + 2*P_Z + 2) >> 2);
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos3][ipos2] =
    img->mpr[jpos5][ipos3] =
    img->mpr[jpos7][ipos4] = (imgpel) ((P_Z + P_B + 2*P_A + 2) >> 2);
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos3][ipos3] =
    img->mpr[jpos5][ipos4] =
    img->mpr[jpos7][ipos5] = (imgpel) ((P_A + P_C + 2*P_B + 2) >> 2);
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos3][ipos4] =
    img->mpr[jpos5][ipos5] =
    img->mpr[jpos7][ipos6] = (imgpel) ((P_B + P_D + 2*P_C + 2) >> 2);
    img->mpr[jpos1][ipos4] =
    img->mpr[jpos3][ipos5] =
    img->mpr[jpos5][ipos6] =
    img->mpr[jpos7][ipos7] = (imgpel) ((P_C + P_E + 2*P_D + 2) >> 2);
    img->mpr[jpos1][ipos5] =
    img->mpr[jpos3][ipos6] =
    img->mpr[jpos5][ipos7] = (imgpel) ((P_D + P_F + 2*P_E + 2) >> 2);
    img->mpr[jpos1][ipos6] =
    img->mpr[jpos3][ipos7] = (imgpel) ((P_E + P_G + 2*P_F + 2) >> 2);
    img->mpr[jpos1][ipos7] = (imgpel) ((P_F + P_H + 2*P_G + 2) >> 2);
    img->mpr[jpos2][ipos0] =
    img->mpr[jpos4][ipos1] =
    img->mpr[jpos6][ipos2] = (imgpel) ((P_R + P_Z + 2*P_Q + 2) >> 2);
    img->mpr[jpos3][ipos0] =
    img->mpr[jpos5][ipos1] =
    img->mpr[jpos7][ipos2] = (imgpel) ((P_S + P_Q + 2*P_R + 2) >> 2);
    img->mpr[jpos4][ipos0] =
    img->mpr[jpos6][ipos1] = (imgpel) ((P_T + P_R + 2*P_S + 2) >> 2);
    img->mpr[jpos5][ipos0] =
    img->mpr[jpos7][ipos1] = (imgpel) ((P_U + P_S + 2*P_T + 2) >> 2);
    img->mpr[jpos6][ipos0] = (imgpel) ((P_V + P_T + 2*P_U + 2) >> 2);
    img->mpr[jpos7][ipos0] = (imgpel) ((P_W + P_U + 2*P_V + 2) >> 2);
    break;

  case  HOR_DOWN_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
    if ((!block_available_up)||(!block_available_left)||(!block_available_up_left))
      printf ("warning: Intra_8x8_Horizontal_Down prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] =
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos2][ipos4] =
    img->mpr[jpos3][ipos6] = (imgpel) ((P_Q + P_Z + 1) >> 1);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos3][ipos4] =
    img->mpr[jpos4][ipos6] = (imgpel) ((P_R + P_Q + 1) >> 1);
    img->mpr[jpos2][ipos0] =
    img->mpr[jpos3][ipos2] =
    img->mpr[jpos4][ipos4] =
    img->mpr[jpos5][ipos6] = (imgpel) ((P_S + P_R + 1) >> 1);
    img->mpr[jpos3][ipos0] =
    img->mpr[jpos4][ipos2] =
    img->mpr[jpos5][ipos4] =
    img->mpr[jpos6][ipos6] = (imgpel) ((P_T + P_S + 1) >> 1);
    img->mpr[jpos4][ipos0] =
    img->mpr[jpos5][ipos2] =
    img->mpr[jpos6][ipos4] =
    img->mpr[jpos7][ipos6] = (imgpel) ((P_U + P_T + 1) >> 1);
    img->mpr[jpos5][ipos0] =
    img->mpr[jpos6][ipos2] =
    img->mpr[jpos7][ipos4] = (imgpel) ((P_V + P_U + 1) >> 1);
    img->mpr[jpos6][ipos0] =
    img->mpr[jpos7][ipos2] = (imgpel) ((P_W + P_V + 1) >> 1);
    img->mpr[jpos7][ipos0] = (imgpel) ((P_X + P_W + 1) >> 1);
    img->mpr[jpos0][ipos1] =
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos2][ipos5] =
    img->mpr[jpos3][ipos7] = (imgpel) ((P_Q + P_A + 2*P_Z + 2) >> 2);
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos3][ipos5] =
    img->mpr[jpos4][ipos7] = (imgpel) ((P_Z + P_R + 2*P_Q + 2) >> 2);
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos3][ipos3] =
    img->mpr[jpos4][ipos5] =
    img->mpr[jpos5][ipos7] = (imgpel) ((P_Q + P_S + 2*P_R + 2) >> 2);
    img->mpr[jpos3][ipos1] =
    img->mpr[jpos4][ipos3] =
    img->mpr[jpos5][ipos5] =
    img->mpr[jpos6][ipos7] = (imgpel) ((P_R + P_T + 2*P_S + 2) >> 2);
    img->mpr[jpos4][ipos1] =
    img->mpr[jpos5][ipos3] =
    img->mpr[jpos6][ipos5] =
    img->mpr[jpos7][ipos7] = (imgpel) ((P_S + P_U + 2*P_T + 2) >> 2);
    img->mpr[jpos5][ipos1] =
    img->mpr[jpos6][ipos3] =
    img->mpr[jpos7][ipos5] = (imgpel) ((P_T + P_V + 2*P_U + 2) >> 2);
    img->mpr[jpos6][ipos1] =
    img->mpr[jpos7][ipos3] = (imgpel) ((P_U + P_W + 2*P_V + 2) >> 2);
    img->mpr[jpos7][ipos1] = (imgpel) ((P_V + P_X + 2*P_W + 2) >> 2);
    img->mpr[jpos0][ipos2] =
    img->mpr[jpos1][ipos4] =
    img->mpr[jpos2][ipos6] = (imgpel) ((P_Z + P_B + 2*P_A + 2) >> 2);
    img->mpr[jpos0][ipos3] =
    img->mpr[jpos1][ipos5] =
    img->mpr[jpos2][ipos7] = (imgpel) ((P_A + P_C + 2*P_B + 2) >> 2);
    img->mpr[jpos0][ipos4] =
    img->mpr[jpos1][ipos6] = (imgpel) ((P_B + P_D + 2*P_C + 2) >> 2);
    img->mpr[jpos0][ipos5] =
    img->mpr[jpos1][ipos7] = (imgpel) ((P_C + P_E + 2*P_D + 2) >> 2);
    img->mpr[jpos0][ipos6] = (imgpel) ((P_D + P_F + 2*P_E + 2) >> 2);
    img->mpr[jpos0][ipos7] = (imgpel) ((P_E + P_G + 2*P_F + 2) >> 2);
    break;

  case  HOR_UP_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
    if (!block_available_left)
      printf ("warning: Intra_8x8_Horizontal_Up prediction mode not allowed at mb %d\n",img->current_mb_nr);

    img->mpr[jpos0][ipos0] = (imgpel) ((P_Q + P_R + 1) >> 1);
    img->mpr[jpos1][ipos0] =
    img->mpr[jpos0][ipos2] = (imgpel) ((P_R + P_S + 1) >> 1);
    img->mpr[jpos2][ipos0] =
    img->mpr[jpos1][ipos2] =
    img->mpr[jpos0][ipos4] = (imgpel) ((P_S + P_T + 1) >> 1);
    img->mpr[jpos3][ipos0] =
    img->mpr[jpos2][ipos2] =
    img->mpr[jpos1][ipos4] =
    img->mpr[jpos0][ipos6] = (imgpel) ((P_T + P_U + 1) >> 1);
    img->mpr[jpos4][ipos0] =
    img->mpr[jpos3][ipos2] =
    img->mpr[jpos2][ipos4] =
    img->mpr[jpos1][ipos6] = (imgpel) ((P_U + P_V + 1) >> 1);
    img->mpr[jpos5][ipos0] =
    img->mpr[jpos4][ipos2] =
    img->mpr[jpos3][ipos4] =
    img->mpr[jpos2][ipos6] = (imgpel) ((P_V + P_W + 1) >> 1);
    img->mpr[jpos6][ipos0] =
    img->mpr[jpos5][ipos2] =
    img->mpr[jpos4][ipos4] =
    img->mpr[jpos3][ipos6] = (imgpel) ((P_W + P_X + 1) >> 1);
    img->mpr[jpos4][ipos6] =
    img->mpr[jpos4][ipos7] =
    img->mpr[jpos5][ipos4] =
    img->mpr[jpos5][ipos5] =
    img->mpr[jpos5][ipos6] =
    img->mpr[jpos5][ipos7] =
    img->mpr[jpos6][ipos2] =
    img->mpr[jpos6][ipos3] =
    img->mpr[jpos6][ipos4] =
    img->mpr[jpos6][ipos5] =
    img->mpr[jpos6][ipos6] =
    img->mpr[jpos6][ipos7] =
    img->mpr[jpos7][ipos0] =
    img->mpr[jpos7][ipos1] =
    img->mpr[jpos7][ipos2] =
    img->mpr[jpos7][ipos3] =
    img->mpr[jpos7][ipos4] =
    img->mpr[jpos7][ipos5] =
    img->mpr[jpos7][ipos6] =
    img->mpr[jpos7][ipos7] = (imgpel) P_X;
    img->mpr[jpos6][ipos1] =
    img->mpr[jpos5][ipos3] =
    img->mpr[jpos4][ipos5] =
    img->mpr[jpos3][ipos7] = (imgpel) ((P_W + 3*P_X + 2) >> 2);
    img->mpr[jpos5][ipos1] =
    img->mpr[jpos4][ipos3] =
    img->mpr[jpos3][ipos5] =
    img->mpr[jpos2][ipos7] = (imgpel) ((P_X + P_V + 2*P_W + 2) >> 2);
    img->mpr[jpos4][ipos1] =
    img->mpr[jpos3][ipos3] =
    img->mpr[jpos2][ipos5] =
    img->mpr[jpos1][ipos7] = (imgpel) ((P_W + P_U + 2*P_V + 2) >> 2);
    img->mpr[jpos3][ipos1] =
    img->mpr[jpos2][ipos3] =
    img->mpr[jpos1][ipos5] =
    img->mpr[jpos0][ipos7] = (imgpel) ((P_V + P_T + 2*P_U + 2) >> 2);
    img->mpr[jpos2][ipos1] =
    img->mpr[jpos1][ipos3] =
    img->mpr[jpos0][ipos5] = (imgpel) ((P_U + P_S + 2*P_T + 2) >> 2);
    img->mpr[jpos1][ipos1] =
    img->mpr[jpos0][ipos3] = (imgpel) ((P_T + P_R + 2*P_S + 2) >> 2);
    img->mpr[jpos0][ipos1] = (imgpel) ((P_S + P_Q + 2*P_R + 2) >> 2);
    break;

  default:
    printf("Error: illegal intra_4x4 prediction mode: %d\n",predmode);
    return SEARCH_SYNC;
    break;
  }
  return DECODING_OK;
}



/*!
 *************************************************************************************
 * \brief
 *    Prefiltering for Intra8x8 prediction
 *************************************************************************************
 */
void LowPassForIntra8x8Pred(imgpel *PredPel, int block_up_left, int block_up, int block_left)
{
  int i;
  imgpel LoopArray[25];

  memcpy(&LoopArray[0], &PredPel[0], 25 * sizeof(imgpel));

  if(block_up)
  {
    if(block_up_left)
    {
      LoopArray[1] = (((&P_Z)[0] + ((&P_Z)[1]<<1) + (&P_Z)[2] + 2)>>2);
    }
    else
      LoopArray[1] = (((&P_Z)[1] + ((&P_Z)[1]<<1) + (&P_Z)[2] + 2)>>2);


    for(i = 2; i <16; i++)
    {
      LoopArray[i] = (((&P_Z)[i-1] + ((&P_Z)[i]<<1) + (&P_Z)[i+1] + 2)>>2);
    }
    LoopArray[16] = ((P_P + (P_P<<1) + P_O + 2)>>2);
  }

  if(block_up_left)
  {

    if(block_up && block_left)
    {
      LoopArray[0] = ((P_Q + (P_Z<<1) + P_A +2)>>2);
    }
    else
    {
      if(block_up)
        LoopArray[0] = ((P_Z + (P_Z<<1) + P_A +2)>>2);
      else
        if(block_left)
          LoopArray[0] = ((P_Z + (P_Z<<1) + P_Q +2)>>2);
    }

  }

  if(block_left)
  {
    if(block_up_left)
      LoopArray[17] = ((P_Z + (P_Q<<1) + P_R + 2)>>2);
    else
      LoopArray[17] = ((P_Q + (P_Q<<1) + P_R + 2)>>2);

    for(i = 18; i <24; i++)
    {
      LoopArray[i] = (((&P_Z)[i-1] + ((&P_Z)[i]<<1) + (&P_Z)[i+1] + 2)>>2);
    }
    LoopArray[24] = ((P_W + (P_X<<1) + P_X + 2)>>2);
  }

  memcpy(&PredPel[0], &LoopArray[0], 25 * sizeof(imgpel));
}



/*!
 ***********************************************************************
 * \brief
 *    Inverse 8x8 transformation
 ***********************************************************************
 */
void itrans8x8(struct img_par *img, //!< image parameters
              int ioff,            //!< index to 4x4 block
              int joff)            //!<
{
  int i,j;
  int m6[8][8];
  Boolean lossless_qpprime = (Boolean) ((img->qp + img->bitdepth_luma_qp_scale)==0 && img->lossless_qpprime_flag==1);
  static int ipos;
  static int a[8], b[8];

  if(lossless_qpprime)
  {
    for( j = joff; j < joff + 8; j++)
    {
      for( i = ioff; i < ioff + 8; i++)
        img->m7[j][i] = iClip1(img->max_imgpel_value, (img->m7[j][i]+(long)img->mpr[j][i]));
    }
  }
  else
  {
    int *m7;
    for( i = 0; i < 8; i++)
    {
      m7 = &img->m7[joff + i][ioff];
      a[0] =  m7[0] + m7[4];
      a[4] =  m7[0] - m7[4];
      a[2] = (m7[2]>>1) - m7[6];
      a[6] =  m7[2] + (m7[6]>>1);

      b[0] = a[0] + a[6];
      b[2] = a[4] + a[2];
      b[4] = a[4] - a[2];
      b[6] = a[0] - a[6];

      a[1] = -m7[3] + m7[5] - m7[7] - (m7[7]>>1);
      a[3] =  m7[1] + m7[7] - m7[3] - (m7[3]>>1);
      a[5] = -m7[1] + m7[7] + m7[5] + (m7[5]>>1);
      a[7] =  m7[3] + m7[5] + m7[1] + (m7[1]>>1);

      b[1] =   a[1] + (a[7]>>2);
      b[7] = -(a[1]>>2) + a[7];
      b[3] =   a[3] + (a[5]>>2);
      b[5] =  (a[3]>>2) - a[5];

      m6[0][i] = b[0] + b[7];
      m6[1][i] = b[2] + b[5];
      m6[2][i] = b[4] + b[3];
      m6[3][i] = b[6] + b[1];
      m6[4][i] = b[6] - b[1];
      m6[5][i] = b[4] - b[3];
      m6[6][i] = b[2] - b[5];
      m6[7][i] = b[0] - b[7];
    }
    for( i = 0; i < 8; i++)
    {
      ipos = ioff + i;
      m7 = m6[i]; 
      a[0] = m7[0] + m7[4];
      a[4] = m7[0] - m7[4];
      a[2] = (m7[2]>>1) - m7[6];
      a[6] =  m7[2] + (m7[6]>>1);

      b[0] = a[0] + a[6];
      b[2] = a[4] + a[2];
      b[4] = a[4] - a[2];
      b[6] = a[0] - a[6];

      a[1] = -m7[3] + m7[5] - m7[7] - (m7[7]>>1);
      a[3] =  m7[1] + m7[7] - m7[3] - (m7[3]>>1);
      a[5] = -m7[1] + m7[7] + m7[5] + (m7[5]>>1);
      a[7] =  m7[3] + m7[5] + m7[1] + (m7[1]>>1);

      b[1] =   a[1] + (a[7]>>2);
      b[7] = -(a[1]>>2) + a[7];
      b[3] =   a[3] + (a[5]>>2);
      b[5] =  (a[3]>>2) - a[5];

      img->m7[joff + 0][ipos] = b[0] + b[7];
      img->m7[joff + 1][ipos] = b[2] + b[5];
      img->m7[joff + 2][ipos] = b[4] + b[3];
      img->m7[joff + 3][ipos] = b[6] + b[1];
      img->m7[joff + 4][ipos] = b[6] - b[1];
      img->m7[joff + 5][ipos] = b[4] - b[3];
      img->m7[joff + 6][ipos] = b[2] - b[5];
      img->m7[joff + 7][ipos] = b[0] - b[7];
    }

    for( j = joff; j < joff + 8; j++)
    {
      for( i = ioff; i < ioff + 8; i++)
        img->m7[j][i] = iClip1(img->max_imgpel_value, 
        rshift_rnd_sf((img->m7[j][i]+((long)img->mpr[j][i] << DQ_BITS_8)),DQ_BITS_8));
    }
  }
}
