
/*!
*************************************************************************************
* \file me_distortion.c
*
* \brief
*    Motion estimation error calculation functions
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*      - Alexis Michael Tourapis <alexis.tourapis@dolby.com>
*      - Athanasios Leontaris    <aleon@dolby.com>
*
*************************************************************************************
*/

#include "contributors.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "global.h"
#include "image.h"
#include "memalloc.h"
#include "mb_access.h"
#include "refbuf.h"

#include "me_distortion.h"

extern unsigned int *byte_abs;

// Define Global Parameters
// Luma
imgpel *(*get_line[2]) (imgpel****, int, int);
imgpel *(*get_line1[2]) (imgpel****, int, int);
imgpel *(*get_line2[2]) (imgpel****, int, int);
// Chroma
imgpel *(*get_crline[2]) (imgpel****, int, int);
imgpel *(*get_crline1[2]) (imgpel****, int, int);
imgpel *(*get_crline2[2]) (imgpel****, int, int);
// Access method (fast/safe or unconstrained)
int ref_access_method;
int bipred1_access_method;
int bipred2_access_method;

SubImageContainer ref_pic_sub;
SubImageContainer ref_pic1_sub;
SubImageContainer ref_pic2_sub;

short weight1, weight2, offsetBi;
short weight1_cr[2], weight2_cr[2], offsetBi_cr[2];
int weight_luma, weight_cr[2], offset_luma, offset_cr[2];
short img_width, img_height;
int test8x8transform;
int ChromaMEEnable;

// temp storage of pixel difference values prior to applying Hadamard Transform (4x4 or 8x8)
static int diff[MB_PIXELS];
// Hadamard related arrays
static int m[16], d[16];
static int m1[8][8], m2[8][8], m3[8][8];
static imgpel *src_line, *ref_line, *ref1_line, *ref2_line;

int (*computeUniPred[6])(imgpel* , int , int , int , int , int);
int (*computeBiPred) (imgpel* , int , int , int , int , int, int , int);
int (*computeBiPred1[3])(imgpel* , int , int , int , int , int, int , int);
int (*computeBiPred2[3])(imgpel* , int , int , int , int , int, int , int);

/*!
 ***********************************************************************
 * \brief
 *    Calculate SA(T)D
 ***********************************************************************
 */
int distortion4x4(int* diff)
{
  int distortion = 0, k, *byte_sse;

  switch(input->ModeDecisionMetric)
  {
  case ERROR_SAD:
    for (k = 0; k < 16; k++)
    {
      distortion += byte_abs [diff [k]];
    }
    break;
  case ERROR_SSE:
    byte_sse = img->quad;
    for (k = 0; k < 16; k++)
    {
      distortion += byte_sse [diff [k]];
    }
    break;
  case ERROR_SATD :
  default:
    distortion = HadamardSAD4x4( diff );
    break;
  }

  return distortion;
}

/*!
 ***********************************************************************
 * \brief
 *    Calculate SA(T)D for 8x8
 ***********************************************************************
 */
int distortion8x8(int* diff)
{
  int distortion = 0, k, *byte_sse;

  switch(input->ModeDecisionMetric)
  {
  case ERROR_SAD:
    for (k = 0; k < 64; k++)
    {
      distortion += byte_abs [diff [k]];
    }
    break;
  case ERROR_SSE:
    byte_sse = img->quad;
    for (k = 0; k < 64; k++)
    {
      distortion += byte_sse [diff [k]];
    }
    break;
  case ERROR_SATD :
  default:
    distortion = HadamardSAD8x8( diff );
    break;
  }

  return distortion;
}

/*!
 ***********************************************************************
 * \brief
 *    Calculate SA(T)D for 8x8
 ***********************************************************************
 */
int HadamardMB (int c_diff[MB_PIXELS], int blocktype)
{
  int sad=0;

  switch(blocktype)
  {
    //16x16
  case 1:
    sad  = HadamardSAD8x8( c_diff     );
    sad += HadamardSAD8x8(&c_diff[ 64]);
    sad += HadamardSAD8x8(&c_diff[128]);
    sad += HadamardSAD8x8(&c_diff[192]);
    break;
    //16x8 8x16
  case 2:
  case 3:
    sad  = HadamardSAD8x8( c_diff    );
    sad += HadamardSAD8x8(&c_diff[64]);
    break;
    //8x8
  case 4:
    sad  = HadamardSAD8x8(c_diff);
    break;
    //8x4 4x8
  default:
    sad=-1;
    break;
  }

  return sad;
}

/*!
***********************************************************************
* \brief
*    Calculate 4x4 Hadamard-Transformed SAD
***********************************************************************
*/
int HadamardSAD4x4 (int* diff)
{
  int k, satd = 0;

  /*===== hadamard transform =====*/
  m[ 0] = diff[ 0] + diff[12];
  m[ 1] = diff[ 1] + diff[13];
  m[ 2] = diff[ 2] + diff[14];
  m[ 3] = diff[ 3] + diff[15];
  m[ 4] = diff[ 4] + diff[ 8];
  m[ 5] = diff[ 5] + diff[ 9];
  m[ 6] = diff[ 6] + diff[10];
  m[ 7] = diff[ 7] + diff[11];
  m[ 8] = diff[ 4] - diff[ 8];
  m[ 9] = diff[ 5] - diff[ 9];
  m[10] = diff[ 6] - diff[10];
  m[11] = diff[ 7] - diff[11];
  m[12] = diff[ 0] - diff[12];
  m[13] = diff[ 1] - diff[13];
  m[14] = diff[ 2] - diff[14];
  m[15] = diff[ 3] - diff[15];

  d[ 0] = m[ 0] + m[ 4];
  d[ 1] = m[ 1] + m[ 5];
  d[ 2] = m[ 2] + m[ 6];
  d[ 3] = m[ 3] + m[ 7];
  d[ 4] = m[ 8] + m[12];
  d[ 5] = m[ 9] + m[13];
  d[ 6] = m[10] + m[14];
  d[ 7] = m[11] + m[15];
  d[ 8] = m[ 0] - m[ 4];
  d[ 9] = m[ 1] - m[ 5];
  d[10] = m[ 2] - m[ 6];
  d[11] = m[ 3] - m[ 7];
  d[12] = m[12] - m[ 8];
  d[13] = m[13] - m[ 9];
  d[14] = m[14] - m[10];
  d[15] = m[15] - m[11];

  m[ 0] = d[ 0] + d[ 3];
  m[ 1] = d[ 1] + d[ 2];
  m[ 2] = d[ 1] - d[ 2];
  m[ 3] = d[ 0] - d[ 3];
  m[ 4] = d[ 4] + d[ 7];
  m[ 5] = d[ 5] + d[ 6];
  m[ 6] = d[ 5] - d[ 6];
  m[ 7] = d[ 4] - d[ 7];
  m[ 8] = d[ 8] + d[11];
  m[ 9] = d[ 9] + d[10];
  m[10] = d[ 9] - d[10];
  m[11] = d[ 8] - d[11];
  m[12] = d[12] + d[15];
  m[13] = d[13] + d[14];
  m[14] = d[13] - d[14];
  m[15] = d[12] - d[15];

  d[ 0] = m[ 0] + m[ 1];
  d[ 1] = m[ 0] - m[ 1];
  d[ 2] = m[ 2] + m[ 3];
  d[ 3] = m[ 3] - m[ 2];
  d[ 4] = m[ 4] + m[ 5];
  d[ 5] = m[ 4] - m[ 5];
  d[ 6] = m[ 6] + m[ 7];
  d[ 7] = m[ 7] - m[ 6];
  d[ 8] = m[ 8] + m[ 9];
  d[ 9] = m[ 8] - m[ 9];
  d[10] = m[10] + m[11];
  d[11] = m[11] - m[10];
  d[12] = m[12] + m[13];
  d[13] = m[12] - m[13];
  d[14] = m[14] + m[15];
  d[15] = m[15] - m[14];

  //===== sum up =====
  // Table lookup is faster than abs macro
  for (k=0; k<16; ++k)
  {
    satd += byte_abs [d [k]];
  }


  return ((satd+1)>>1);
}

/*!
***********************************************************************
* \brief
*    Calculate 8x8 Hadamard-Transformed SAD
***********************************************************************
*/
int HadamardSAD8x8 (int* diff)
{
  int i, j, jj, sad=0;

  //horizontal
  for (j=0; j < 8; j++)
  {
    jj = j << 3;
    m2[j][0] = diff[jj  ] + diff[jj+4];
    m2[j][1] = diff[jj+1] + diff[jj+5];
    m2[j][2] = diff[jj+2] + diff[jj+6];
    m2[j][3] = diff[jj+3] + diff[jj+7];
    m2[j][4] = diff[jj  ] - diff[jj+4];
    m2[j][5] = diff[jj+1] - diff[jj+5];
    m2[j][6] = diff[jj+2] - diff[jj+6];
    m2[j][7] = diff[jj+3] - diff[jj+7];

    m1[j][0] = m2[j][0] + m2[j][2];
    m1[j][1] = m2[j][1] + m2[j][3];
    m1[j][2] = m2[j][0] - m2[j][2];
    m1[j][3] = m2[j][1] - m2[j][3];
    m1[j][4] = m2[j][4] + m2[j][6];
    m1[j][5] = m2[j][5] + m2[j][7];
    m1[j][6] = m2[j][4] - m2[j][6];
    m1[j][7] = m2[j][5] - m2[j][7];

    m2[j][0] = m1[j][0] + m1[j][1];
    m2[j][1] = m1[j][0] - m1[j][1];
    m2[j][2] = m1[j][2] + m1[j][3];
    m2[j][3] = m1[j][2] - m1[j][3];
    m2[j][4] = m1[j][4] + m1[j][5];
    m2[j][5] = m1[j][4] - m1[j][5];
    m2[j][6] = m1[j][6] + m1[j][7];
    m2[j][7] = m1[j][6] - m1[j][7];
  }

  //vertical
  for (i=0; i < 8; i++)
  {
    m3[0][i] = m2[0][i] + m2[4][i];
    m3[1][i] = m2[1][i] + m2[5][i];
    m3[2][i] = m2[2][i] + m2[6][i];
    m3[3][i] = m2[3][i] + m2[7][i];
    m3[4][i] = m2[0][i] - m2[4][i];
    m3[5][i] = m2[1][i] - m2[5][i];
    m3[6][i] = m2[2][i] - m2[6][i];
    m3[7][i] = m2[3][i] - m2[7][i];

    m1[0][i] = m3[0][i] + m3[2][i];
    m1[1][i] = m3[1][i] + m3[3][i];
    m1[2][i] = m3[0][i] - m3[2][i];
    m1[3][i] = m3[1][i] - m3[3][i];
    m1[4][i] = m3[4][i] + m3[6][i];
    m1[5][i] = m3[5][i] + m3[7][i];
    m1[6][i] = m3[4][i] - m3[6][i];
    m1[7][i] = m3[5][i] - m3[7][i];

    m2[0][i] = m1[0][i] + m1[1][i];
    m2[1][i] = m1[0][i] - m1[1][i];
    m2[2][i] = m1[2][i] + m1[3][i];
    m2[3][i] = m1[2][i] - m1[3][i];
    m2[4][i] = m1[4][i] + m1[5][i];
    m2[5][i] = m1[4][i] - m1[5][i];
    m2[6][i] = m1[6][i] + m1[7][i];
    m2[7][i] = m1[6][i] - m1[7][i];
  }
  for (j=0; j < 8; j++)
    for (i=0; i < 8; i++)
      sad += iabs (m2[j][i]);

  return ((sad+2)>>2);
}

/*!
************************************************************************
* \brief
*    SAD computation
************************************************************************
*/
int computeSAD(imgpel* src_pic,
               int blocksize_y,
               int blocksize_x,
               int min_mcost,
               int cand_x,
               int cand_y)
{
  int mcost = 0;
  int y,x4;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line = src_pic;
  ref_line = get_line[ref_access_method] (ref_pic_sub.luma, cand_y, cand_x);
  for (y=0; y<blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      mcost += byte_abs[ *src_line++ - *ref_line++ ];
      mcost += byte_abs[ *src_line++ - *ref_line++ ];
      mcost += byte_abs[ *src_line++ - *ref_line++ ];
      mcost += byte_abs[ *src_line++ - *ref_line++ ];
    }
    if (mcost >= min_mcost) return mcost;
    ref_line += pad_size_x;
  }
  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k < 2; k++)
    {
      src_line = src_pic + (256 << k);
      ref_line = get_crline[ref_access_method] ( ref_pic_sub.crcb[k], cand_y, cand_x);
      for (y=0; y<blocksize_y_c; y++)
      {
        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          mcost += byte_abs[ *src_line++ - *ref_line++ ];
          mcost += byte_abs[ *src_line++ - *ref_line++ ];
        }
        if (mcost >= min_mcost) return mcost;
        ref_line += cr_pad_size_x;
      }
    }
  }

  return mcost;
}

/*!
************************************************************************
* \brief
*    SAD computation for weighted samples
************************************************************************
*/
int computeSADWP(imgpel* src_pic,
               int blocksize_y,
               int blocksize_x,
               int min_mcost,
               int cand_x,
               int cand_y)
{
  int mcost = 0;
  int y,x4;
  int weighted_pel;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line = src_pic;
  ref_line = get_line[ref_access_method] (ref_pic_sub.luma, cand_y, cand_x);
  for (y=0; y<blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_abs[ *src_line++ -  weighted_pel ];
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_abs[ *src_line++ -  weighted_pel ];
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_abs[ *src_line++ -  weighted_pel ];
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_abs[ *src_line++ -  weighted_pel ];
    }
    if (mcost >= min_mcost) return mcost;
    ref_line += pad_size_x;
  }
  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k < 2; k++)
    {
      src_line = src_pic + (256 << k);
      ref_line = get_crline[ref_access_method] ( ref_pic_sub.crcb[k], cand_y, cand_x);
      for (y=0; y<blocksize_y_c; y++)
      {
        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *ref_line++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
          mcost += byte_abs[ *src_line++ -  weighted_pel ];
          weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *ref_line++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
          mcost += byte_abs[ *src_line++ -  weighted_pel ];
        }
        if (mcost >= min_mcost) return mcost;
        ref_line += cr_pad_size_x;
      }
    }
  }

  return mcost;
}

/*!
************************************************************************
* \brief
*    BiPred SAD computation (no weights)
************************************************************************
*/
int computeBiPredSAD1(imgpel* src_pic,
                      int blocksize_y,
                      int blocksize_x,
                      int min_mcost,
                      int cand_x1, int cand_y1,
                      int cand_x2, int cand_y2)
{
  int mcost = 0;
  int bi_diff;
  int y,x4;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line   = src_pic;
  ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, cand_y2, cand_x2);
  ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, cand_y1, cand_x1);

  for (y = 0; y < blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_abs[bi_diff];
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_abs[bi_diff];
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_abs[bi_diff];
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_abs[bi_diff];
    }
    if (mcost >= min_mcost) return mcost;
    ref2_line += pad_size_x;
    ref1_line += pad_size_x;
  }

  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c  = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k<2; k++)
    {
      src_line = src_pic + (256 << k);
      ref2_line = get_crline[bipred2_access_method] ( ref_pic2_sub.crcb[k], cand_y2, cand_x2);
      ref1_line = get_crline[bipred1_access_method] ( ref_pic1_sub.crcb[k], cand_y1, cand_x1);

      for (y=0; y<blocksize_y_c; y++)
      {
        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          mcost += byte_abs[bi_diff];
          bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          mcost += byte_abs[bi_diff];
        }

        if (mcost >= min_mcost) return mcost;
        ref2_line += cr_pad_size_x;
        ref1_line += cr_pad_size_x;
      }
    }
  }
  return mcost;
}

/*!
************************************************************************
* \brief
*    BiPred SAD computation (with weights)
************************************************************************
*/
int computeBiPredSAD2(imgpel* src_pic,
                      int blocksize_y,
                      int blocksize_x,
                      int min_mcost,
                      int cand_x1, int cand_y1,
                      int cand_x2, int cand_y2)
{
  int mcost = 0;
  int bi_diff;
  int denom = luma_log_weight_denom + 1;
  int lround = 2 * wp_luma_round;
  int y,x4;
  int weighted_pel, pixel1, pixel2;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line   = src_pic;
  ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, cand_y2, cand_x2);
  ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, cand_y1, cand_x1);

  for (y=0; y<blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += byte_abs[bi_diff];

      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += byte_abs[bi_diff];

      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += byte_abs[bi_diff];

      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += byte_abs[bi_diff];
    }
    if (mcost >= min_mcost) return mcost;
    ref2_line += pad_size_x;
    ref1_line += pad_size_x;
  }

  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k<2; k++)
    {
      src_line = src_pic + (256 << k);
      ref2_line = get_crline[bipred2_access_method] ( ref_pic2_sub.crcb[k], cand_y2, cand_x2);
      ref1_line = get_crline[bipred1_access_method] ( ref_pic1_sub.crcb[k], cand_y1, cand_x1);

      for (y=0; y<blocksize_y_c; y++)
      {
        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          pixel1 = weight1_cr[k] * (*ref1_line++);
          pixel2 = weight2_cr[k] * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value_uv, ((pixel1 + pixel2 + lround) >> denom) + offsetBi_cr[k]);
          bi_diff = (*src_line++) - weighted_pel;
          mcost += byte_abs[bi_diff];

          pixel1 = weight1_cr[k] * (*ref1_line++);
          pixel2 = weight2_cr[k] * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value_uv, ((pixel1 + pixel2 + lround) >> denom) + offsetBi_cr[k]);
          bi_diff = (*src_line++) - weighted_pel;
          mcost += byte_abs[bi_diff];
        }
        if (mcost >= min_mcost) return mcost;
        ref2_line += cr_pad_size_x;
        ref1_line += cr_pad_size_x;
      }
    }
  }
  return mcost;
}

/*!
 ************************************************************************
 * \brief
 *    SAD computation _with_ Hadamard Transform
 ************************************************************************
*/
int computeSATD(imgpel* src_pic,
                int blocksize_y,
                int blocksize_x,
                int min_mcost,
                int cand_x,
                int cand_y)
{
  int mcost = 0;
  int y, x, y4, *d;
  int pad_size_x, src_size_x, src_size_mul;
  imgpel *src_tmp = src_pic;

  if ( !test8x8transform )
  { // 4x4 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE;
    src_size_x = blocksize_x - BLOCK_SIZE;
    src_size_mul = blocksize_x * BLOCK_SIZE;
    for (y = cand_y; y < cand_y + (blocksize_y<<2); y += (BLOCK_SIZE_SP))
    {
      for (x=0; x<blocksize_x; x += BLOCK_SIZE)
      {
        d    = diff;
        ref_line = get_line[ref_access_method] (ref_pic_sub.luma, y, cand_x + (x<<2));
        src_line = src_tmp + x;
        for (y4 = 0; y4 < BLOCK_SIZE; y4++ )
        {
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;

          ref_line += pad_size_x;
          src_line += src_size_x;
        }
        if ((mcost += HadamardSAD4x4 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  else
  { // 8x8 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE8x8;
    src_size_x = (blocksize_x - BLOCK_SIZE8x8);
    src_size_mul = blocksize_x * BLOCK_SIZE8x8;
    for (y = cand_y; y < cand_y + (blocksize_y<<2); y += (BLOCK_SIZE8x8_SP) )
    {
      for (x=0; x<blocksize_x; x += BLOCK_SIZE8x8 )
      {
        d = diff;
        ref_line  = get_line[ref_access_method] (ref_pic_sub.luma, y, cand_x + (x<<2));
        src_line = src_tmp + x;
        for (y4 = 0; y4 < BLOCK_SIZE8x8; y4++ )
        {
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;
          *d++ = *src_line++ - *ref_line++ ;

          ref_line += pad_size_x;
          src_line += src_size_x;
        }
        if ((mcost += HadamardSAD8x8 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  return mcost;
}

/*!
 ************************************************************************
 * \brief
 *    SAD computation of weighted samples _with_ Hadamard Transform
 ************************************************************************
*/
int computeSATDWP(imgpel* src_pic,
                int blocksize_y,
                int blocksize_x,
                int min_mcost,
                int cand_x,
                int cand_y)
{
  int mcost = 0;
  int y, x, y4, *d;
  int weighted_pel;
  int pad_size_x, src_size_x, src_size_mul;
  imgpel *src_tmp = src_pic;

  if ( !test8x8transform )
  { // 4x4 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE;
    src_size_x = (blocksize_x - BLOCK_SIZE);
    src_size_mul = blocksize_x * BLOCK_SIZE;
    for (y = cand_y; y < cand_y + (blocksize_y<<2); y += (BLOCK_SIZE_SP))
    {
      for (x=0; x<blocksize_x; x += BLOCK_SIZE)
      {
        d    = diff;
        ref_line = get_line[ref_access_method] (ref_pic_sub.luma, y, cand_x + (x<<2));
        src_line = src_tmp + x;
        for (y4 = 0; y4 < BLOCK_SIZE; y4++ )
        {
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;

          ref_line += pad_size_x;
          src_line += src_size_x;
        }
        if ((mcost += HadamardSAD4x4 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  else
  { // 8x8 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE8x8;
    src_size_x = (blocksize_x - BLOCK_SIZE8x8);
    src_size_mul = blocksize_x * BLOCK_SIZE8x8;
    for (y = cand_y; y < cand_y + (blocksize_y<<2); y += (BLOCK_SIZE8x8_SP) )
    {
      for (x=0; x<blocksize_x; x += BLOCK_SIZE8x8 )
      {
        d = diff;
        ref_line  = get_line[ref_access_method] (ref_pic_sub.luma, y, cand_x + (x<<2));
        src_line = src_tmp + x;
        for (y4 = 0; y4 < BLOCK_SIZE8x8; y4++ )
        {
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          *d++ = *src_line++ - weighted_pel;

          ref_line += pad_size_x;
          src_line += src_size_x;
        }
        if ((mcost += HadamardSAD8x8 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  return mcost;
}

/*!
 ************************************************************************
 * \brief
 *    BiPred (w/o weights) SATD computation
 ************************************************************************
*/
int computeBiPredSATD1(imgpel* src_pic,
                       int blocksize_y,
                       int blocksize_x,
                       int min_mcost,
                       int cand_x1, int cand_y1,
                       int cand_x2, int cand_y2)
{
  int mcost = 0;
  int y, x, y4, *d;
  int pad_size_x, src_size_x, src_size_mul;
  imgpel *src_tmp = src_pic;

  if ( !test8x8transform )
  { // 4x4 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE;
    src_size_x = (blocksize_x - BLOCK_SIZE);
    src_size_mul = blocksize_x * BLOCK_SIZE;
    for (y=0; y<(blocksize_y<<2); y += (BLOCK_SIZE_SP))
    {
      for (x=0; x<blocksize_x; x += BLOCK_SIZE)
      {
        d    = diff;
        src_line   = src_tmp + x;
        ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, cand_y2 + y, cand_x2 + (x<<2));
        ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, cand_y1 + y, cand_x1 + (x<<2));
        for (y4 = 0; y4 < BLOCK_SIZE; y4++ )
        {
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);

          ref1_line += pad_size_x;
          ref2_line += pad_size_x;
          src_line  += src_size_x;
        }
        if ((mcost += HadamardSAD4x4 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  else
  { // 8x8 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE8x8;
    src_size_x = (blocksize_x - BLOCK_SIZE8x8);
    src_size_mul = blocksize_x * BLOCK_SIZE8x8;
    for (y=0; y<blocksize_y; y += BLOCK_SIZE8x8 )
    {
      int y_pos2 = cand_y2 + (y<<2);
      int y_pos1 = cand_y1 + (y<<2);
      for (x=0; x<blocksize_x; x += BLOCK_SIZE8x8 )
      {
        d = diff;
        src_line   = src_tmp + x;
        ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, y_pos2, cand_x2 + (x<<2));
        ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, y_pos1, cand_x1 + (x<<2));
        for (y4 = 0; y4 < BLOCK_SIZE8x8; y4++ )
        {
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          *d++ = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);

          ref1_line += pad_size_x;
          ref2_line += pad_size_x;
          src_line += src_size_x;
        }
        if ((mcost += HadamardSAD8x8 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  return mcost;
}

/*!
************************************************************************
* \brief
*    BiPred (w/ weights) SATD computation
************************************************************************
*/
int computeBiPredSATD2(imgpel* src_pic,
                       int blocksize_y,
                       int blocksize_x,
                       int min_mcost,
                       int cand_x1, int cand_y1,
                       int cand_x2, int cand_y2)
{
  int mcost = 0;
  int y, x, y4, *d;
  int weighted_pel, pixel1, pixel2;
  int denom = luma_log_weight_denom + 1;
  int lround = 2 * wp_luma_round;
  int pad_size_x, src_size_x, src_size_mul;
  imgpel *src_tmp = src_pic;

  if ( !test8x8transform )
  { // 4x4 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE;
    src_size_x = (blocksize_x - BLOCK_SIZE);
    src_size_mul = blocksize_x * BLOCK_SIZE;
    for (y=0; y<(blocksize_y<<2); y += BLOCK_SIZE_SP)
    {
      for (x=0; x<blocksize_x; x += BLOCK_SIZE)
      {
        d    = diff;
        src_line   = src_tmp + x;
        ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, cand_y2 + y, cand_x2 + (x<<2));
        ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, cand_y1 + y, cand_x1 + (x<<2));
        for (y4 = 0; y4 < BLOCK_SIZE; y4++ )
        {
          // 0
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 1
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 2
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 3
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;

          ref1_line += pad_size_x;
          ref2_line += pad_size_x;
          src_line  += src_size_x;
        }
        if ((mcost += HadamardSAD4x4 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  else
  { // 8x8 TRANSFORM
    pad_size_x = img_padded_size_x - BLOCK_SIZE8x8;
    src_size_x = (blocksize_x - BLOCK_SIZE8x8);
    src_size_mul = blocksize_x * BLOCK_SIZE8x8;
    for (y=0; y<blocksize_y; y += BLOCK_SIZE8x8 )
    {
      int y_pos2 = cand_y2 + (y<<2);
      int y_pos1 = cand_y1 + (y<<2);
      for (x=0; x<blocksize_x; x += BLOCK_SIZE8x8 )
      {
        d = diff;
        src_line   = src_tmp + x;
        ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, y_pos2, cand_x2 + (x<<2));
        ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, y_pos1, cand_x1 + (x<<2));
        for (y4 = 0; y4 < BLOCK_SIZE8x8; y4++ )
        {
          // 0
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 1
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 2
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 3
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 4
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 5
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 6
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line++) - weighted_pel;
          // 7
          pixel1 = weight1 * (*ref1_line++);
          pixel2 = weight2 * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
          *d++ =  (*src_line) - weighted_pel;

          ref1_line += pad_size_x;
          ref2_line += pad_size_x;
          src_line  += src_size_x;
        }
        if ((mcost += HadamardSAD8x8 (diff)) > min_mcost) return mcost;
      }
      src_tmp += src_size_mul;
    }
  }
  return mcost;
}

/*!
************************************************************************
* \brief
*    SSE computation
************************************************************************
*/

int computeSSE(imgpel* src_pic,
               int blocksize_y,
               int blocksize_x,
               int min_mcost,
               int cand_x,
               int cand_y)
{
  int mcost = 0;
  int y,x4;
  int *byte_sse = img->quad;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line = src_pic;
  ref_line = get_line[ref_access_method] (ref_pic_sub.luma, cand_y, cand_x);
  for (y=0; y<blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      mcost += byte_sse[ *src_line++ - *ref_line++ ];
      mcost += byte_sse[ *src_line++ - *ref_line++ ];
      mcost += byte_sse[ *src_line++ - *ref_line++ ];
      mcost += byte_sse[ *src_line++ - *ref_line++ ];
    }
    if (mcost >= min_mcost) return mcost;
    ref_line += pad_size_x;
  }

  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k<2; k++)
    {
      src_line = src_pic + (256 << k);
      ref_line = get_crline[ref_access_method] ( ref_pic_sub.crcb[k], cand_y, cand_x);
      for (y=0; y<blocksize_y_c; y++)
      {
        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          mcost += byte_sse[ *src_line++ - *ref_line++ ];
          mcost += byte_sse[ *src_line++ - *ref_line++ ];
        }
        if (mcost >= min_mcost) return mcost;
        ref_line += cr_pad_size_x;
      }
    }
  }

  return mcost;
}


/*!
************************************************************************
* \brief
*    SSE computation of weighted samples
************************************************************************
*/

int computeSSEWP(imgpel* src_pic,
               int blocksize_y,
               int blocksize_x,
               int min_mcost,
               int cand_x,
               int cand_y)
{
  int mcost = 0;
  int y,x4;
  int weighted_pel;
  int *byte_sse = img->quad;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line = src_pic;
  ref_line = get_line[ref_access_method] (ref_pic_sub.luma, cand_y, cand_x);
  for (y=0; y<blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_sse[ *src_line++ - weighted_pel ];
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_sse[ *src_line++ - weighted_pel ];
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_sse[ *src_line++ - weighted_pel ];
      weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *ref_line++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
      mcost += byte_sse[ *src_line++ - weighted_pel ];
    }
    if (mcost >= min_mcost) return mcost;
    ref_line += pad_size_x;
  }

  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    // These could be made global to reduce computations
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k<2; k++)
    {
      src_line = src_pic + (256 << k);
      ref_line = get_crline[ref_access_method] ( ref_pic_sub.crcb[k], cand_y, cand_x);
      for (y=0; y<blocksize_y_c; y++)
      {

        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *ref_line++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
          mcost += byte_sse[ *src_line++ - weighted_pel ];
          weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *ref_line++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
          mcost += byte_sse[ *src_line++ - weighted_pel ];
        }
        if (mcost >= min_mcost) return mcost;
        ref_line += cr_pad_size_x;
      }
    }
  }

  return mcost;
}

/*!
************************************************************************
* \brief
*    BiPred SSE computation (no weights)
************************************************************************
*/
int computeBiPredSSE1(imgpel* src_pic,
                      int blocksize_y,
                      int blocksize_x,
                      int min_mcost,
                      int cand_x1, int cand_y1,
                      int cand_x2, int cand_y2)
{
  int mcost = 0;
  int bi_diff;
  int y,x4;
  int *byte_sse = img->quad;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line   = src_pic;
  ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, cand_y2, cand_x2);
  ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, cand_y1, cand_x1);

  for (y = 0; y < blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_sse[bi_diff];
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_sse[bi_diff];
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_sse[bi_diff];
      bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
      mcost += byte_sse[bi_diff];
    }

    if (mcost >= min_mcost) return mcost;
    ref2_line += pad_size_x;
    ref1_line += pad_size_x;
  }

  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k<2; k++)
    {
      src_line = src_pic + (256 << k);
      ref2_line = get_crline[bipred2_access_method] ( ref_pic2_sub.crcb[k], cand_y2, cand_x2);
      ref1_line = get_crline[bipred1_access_method] ( ref_pic1_sub.crcb[k], cand_y1, cand_x1);

      for (y=0; y<blocksize_y_c; y++)
      {
        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          mcost += byte_sse[bi_diff];
          bi_diff = (*src_line++) - ((*ref1_line++ + *ref2_line++ + 1)>>1);
          mcost += byte_sse[bi_diff];
        }
        if (mcost >= min_mcost) return mcost;
        ref2_line += cr_pad_size_x;
        ref1_line += cr_pad_size_x;
      }
    }
  }

  return mcost;
}


/*!
************************************************************************
* \brief
*    BiPred SSE computation (with weights)
************************************************************************
*/
int computeBiPredSSE2(imgpel* src_pic,
                      int blocksize_y,
                      int blocksize_x,
                      int min_mcost,
                      int cand_x1, int cand_y1,
                      int cand_x2, int cand_y2)
{
  int mcost = 0;
  int bi_diff;
  int denom = luma_log_weight_denom + 1;
  int lround = 2 * wp_luma_round;
  int y,x4;
  int weighted_pel, pixel1, pixel2;
  int pad_size_x = img_padded_size_x - blocksize_x;

  src_line   = src_pic;
  ref2_line  = get_line[bipred2_access_method] (ref_pic2_sub.luma, cand_y2, cand_x2);
  ref1_line  = get_line[bipred1_access_method] (ref_pic1_sub.luma, cand_y1, cand_x1);
  for (y=0; y<blocksize_y; y++)
  {
    for (x4 = 0; x4 < blocksize_x; x4+=4)
    {
      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += bi_diff * bi_diff;

      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += bi_diff * bi_diff;

      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += bi_diff * bi_diff;

      pixel1 = weight1 * (*ref1_line++);
      pixel2 = weight2 * (*ref2_line++);
      weighted_pel =  iClip1( img->max_imgpel_value, ((pixel1 + pixel2 + lround) >> denom) + offsetBi);
      bi_diff = (*src_line++) - weighted_pel;
      mcost += bi_diff * bi_diff;
    }
    if (mcost >= min_mcost) return mcost;
    ref2_line += pad_size_x;
    ref1_line += pad_size_x;
  }

  if ( ChromaMEEnable ) {
    // calculate chroma conribution to motion compensation error
    int blocksize_x_c2 = blocksize_x >> shift_cr_x;
    int blocksize_y_c = blocksize_y >> shift_cr_y;
    int cr_pad_size_x = img_cr_padded_size_x - blocksize_x_c2;
    int k;

    for (k=0; k<2; k++)
    {
      src_line = src_pic + (256 << k);
      ref2_line = get_crline[bipred2_access_method] ( ref_pic2_sub.crcb[k], cand_y2, cand_x2);
      ref1_line = get_crline[bipred1_access_method] ( ref_pic1_sub.crcb[k], cand_y1, cand_x1);

      for (y=0; y<blocksize_y_c; y++)
      {
        for (x4 = 0; x4 < blocksize_x_c2; x4++)
        {
          pixel1 = weight1_cr[k] * (*ref1_line++);
          pixel2 = weight2_cr[k] * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value_uv, ((pixel1 + pixel2 + lround) >> denom) + offsetBi_cr[k]);
          bi_diff = (*src_line++) - weighted_pel;
          mcost += bi_diff * bi_diff;

          pixel1 = weight1_cr[k] * (*ref1_line++);
          pixel2 = weight2_cr[k] * (*ref2_line++);
          weighted_pel =  iClip1( img->max_imgpel_value_uv, ((pixel1 + pixel2 + lround) >> denom) + offsetBi_cr[k]);
          bi_diff = (*src_line++) - weighted_pel;
          mcost += bi_diff * bi_diff;
        }
        if (mcost >= min_mcost) return mcost;
        ref2_line += cr_pad_size_x;
        ref1_line += cr_pad_size_x;
      }
    }
  }

  return mcost;
}


