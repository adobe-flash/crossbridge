
/*!
*************************************************************************************
* \file img_chroma.c
*
* \brief
*    Chroma interpolation functions
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*      - Athanasios Leontaris    <aleon@dolby.com>
*      - Alexis Michael Tourapis <alexis.tourapis@dolby.com>
*
*************************************************************************************
*/

#include "contributors.h"

#include <stdlib.h>
#include <limits.h>

#include "global.h"
#include "image.h"
#include "img_luma.h"

/*!
 ************************************************************************
 * \brief
 *    Creates the 16 (YUV444), 32 (YUV422), or 64 (YUV420) sub-images that
 *    contain quarter-pel samples sub-sampled at different
 *    spatial orientationss;
 *      enables more efficient implementation
 *
 * \param s
 *    pointer to StorablePicture structure
 s************************************************************************
 */
void getSubImagesChroma( StorablePicture *s )
{
  int jpad, ipad, i, j, jj, ii_plus_one, jj_plus_one, uv, k, l;
  int weight00, weight01, weight10, weight11;
  int ypadded_size;
  int xpadded_size;
  int subimages_y, subimages_x, subx, suby;
  int maxx, maxy;
  int size_x_minus1;
  int size_y_minus1;
  // multiplier factor for index to account for UV sampling ratios
  int mul_x, mul_y;
  int mm, kk;

  imgpel *wBufSrc0, *wBufSrc1, *wBufDst;

  size_x_minus1 = s->size_x_cr - 1;
  size_y_minus1 = s->size_y_cr - 1;

  if ( img->yuv_format == YUV420 ) {
    subimages_x = 8;
    subimages_y = 8;
    mul_x = mul_y = 1;
  }
  else if ( img->yuv_format == YUV422 ) {
    subimages_x = 8;
    subimages_y = 4;
    mul_y = 2;
    mul_x = 1;
  }
  else { // YUV444
    subimages_x = 4;
    subimages_y = 4;
    mul_x = mul_y = 2;
  }

  xpadded_size = s->size_x_cr + 2*img_pad_size_uv_x;
  ypadded_size = s->size_y_cr + 2*img_pad_size_uv_y;

  maxx = xpadded_size - 1;
  maxy = ypadded_size - 1;

  // U or V
  for ( uv = 0; uv < 2; uv++ )
  {
    for ( suby = 0, k = 0; suby < subimages_y; suby++, k += mul_y )
    {
      int m = (8 - k);
      mm = m * 8;
      kk = k * 8;
      for ( subx = 0, l = 0; subx < subimages_x; subx++, l += mul_x )
      {
        weight00 = m * (8-l);
        weight01 = m * l;
        weight10 = k * (8-l);
        weight11 = k * l;
        for (j = -img_pad_size_uv_y, jpad = 0; j < ypadded_size - img_pad_size_uv_y; j++, jpad++)
        {
          jj = iClip3(0,size_y_minus1, j);
          jj_plus_one = iClip3(0,size_y_minus1, j + 1);

          wBufDst = &( s->imgUV_sub[uv][suby][subx][jpad][0] );
          wBufSrc0 = s->imgUV[uv][jj         ];
          wBufSrc1 = s->imgUV[uv][jj_plus_one];

          for (i = -img_pad_size_uv_x, ipad = 0; i < 0; i++, ipad++)
          {
            wBufDst[ipad] = (imgpel) rshift_rnd_sf(
              mm * wBufSrc0[0] + kk * wBufSrc1[0], 6 );
          }

          for (i = 0; i < size_x_minus1; i++, ipad++)
          {
            ii_plus_one = i + 1;

            wBufDst[ipad] = (imgpel) rshift_rnd_sf(
              weight00 * wBufSrc0[i] + weight01 * wBufSrc0[ii_plus_one] +
              weight10 * wBufSrc1[i] + weight11 * wBufSrc1[ii_plus_one], 6 );
          }
          for (i = size_x_minus1; i < xpadded_size - img_pad_size_uv_x; i++, ipad++)
          {
            wBufDst[ipad] = (imgpel) rshift_rnd_sf(
              mm * wBufSrc0[size_x_minus1] + kk * wBufSrc1[size_x_minus1], 6 );
          }
        }
      }
    }
  }
}

