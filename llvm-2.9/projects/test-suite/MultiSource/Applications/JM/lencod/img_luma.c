
/*!
*************************************************************************************
* \file img_luma.c
*
* \brief
*    Luma interpolation functions
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
#include <memory.h>
#include <limits.h>

#include "global.h"
#include "image.h"
#include "img_luma.h"

const int ONE_FOURTH_TAP[2][3] =
{
  {20, -5, 1},  // AVC Interpolation taps
  {20,-4, 0},   // Experimental - not valid
};

/*!
 ************************************************************************
 * \brief
 *    Creates the 4x4 = 16 images that contain quarter-pel samples
 *    sub-sampled at different spatial orientations;
 *    enables more efficient implementation
 *
 * \param s
 *    pointer to StorablePicture structure
 s************************************************************************
 */
void getSubImagesLuma( StorablePicture *s )
{
  int i, j;
  int jj, jpad;

  imgpel  **imgY = s->imgY;
  int size_x_minus1 = s->size_x - 1;
  int size_y_minus1 = s->size_y - 1;

  imgpel *wBufDst, *wBufSrc;

  //  0  1  2  3
  //  4  5  6  7
  //  8  9 10 11
  // 12 13 14 15

  //// INTEGER PEL POSITIONS ////

  // sub-image 0 [0][0]
  // simply copy the integer pels
  for (j = -IMG_PAD_SIZE; j < s->size_y + IMG_PAD_SIZE; j++)
  {
    jj = iClip3(0, size_y_minus1, j);
    jpad = j + IMG_PAD_SIZE;
    wBufDst = &( s->imgY_sub[0][0][jpad][IMG_PAD_SIZE] );
    wBufSrc = imgY[jj];
    // left IMG_PAD_SIZE
    for (i = -IMG_PAD_SIZE; i < 0; i++)
    {
      wBufDst[i] = wBufSrc[0];
    }
    // right IMG_PAD_SIZE
    for (i = s->size_x; i < s->size_x + IMG_PAD_SIZE; i++)
    {
      wBufDst[i] = wBufSrc[size_x_minus1];
    }
    // center 0-(s->size_x)
    memcpy(wBufDst, wBufSrc, s->size_x * sizeof(imgpel));
  }

  //// HALF-PEL POSITIONS: SIX-TAP FILTER ////

  // sub-image 2 [0][2]
  // HOR interpolate (six-tap) sub-image [0][0]
  getHorSubImageSixTap( s, 0, 2, 0, 0 );

  // sub-image 8 [2][0]
  // VER interpolate (six-tap) sub-image [0][0]
  getVerSubImageSixTap( s, 2, 0, 0, 0, 0 );

  // sub-image 10 [2][2]
  // VER interpolate (six-tap) sub-image [0][2]
  getVerSubImageSixTap( s, 2, 2, 0, 2, 1 );

  //// QUARTER-PEL POSITIONS: BI-LINEAR INTERPOLATION ////

  // sub-image 1 [0][1]
  getHorSubImageBiLinear( s, 0, 1, 0, 0, 0, 2,  0 );
  // sub-image 3 [0][3]
  getHorSubImageBiLinear( s, 0, 3, 0, 2, 0, 0,  1 );
  // sub-image 9 [2][1]
  getHorSubImageBiLinear( s, 2, 1, 2, 0, 2, 2,  0 );
  // sub-image 11 [0][3]
  getHorSubImageBiLinear( s, 2, 3, 2, 2, 2, 0,  1 );

  // sub-image 4 [1][0]
  getVerSubImageBiLinear( s, 1, 0, 0, 0, 2, 0,  0 );
  // sub-image 6 [1][2]
  getVerSubImageBiLinear( s, 1, 2, 0, 2, 2, 2,  0 );

  // sub-image 12 [3][0]
  getVerSubImageBiLinear( s, 3, 0, 2, 0, 0, 0,  1 );
  // sub-image 14 [3][2]
  getVerSubImageBiLinear( s, 3, 2, 2, 2, 0, 2,  1 );

  // sub-image 5 [1][1]
  getDiagSubImageBiLinear( s, 1, 1, 0, 2, 2, 0,  0, 0, 0, 0 );
  // sub-image 7 [1][3]
  getDiagSubImageBiLinear( s, 1, 3, 0, 2, 2, 0,  0, 0, 0, 1 );
  // sub-image 13 [3][1]
  getDiagSubImageBiLinear( s, 3, 1, 2, 0, 0, 2,  0, 0, 1, 0 );
  // sub-image 15 [3][3]
  getDiagSubImageBiLinear( s, 3, 3, 0, 2, 2, 0,  1, 0, 0, 1 );
}

/*!
 ************************************************************************
 * \brief
 *    Does _horizontal_ interpolation using the SIX TAP filters
 *
 * \param s
 *    pointer to StorablePicture structure
 * \param dst_x
 *    horizontal index to sub-image being generated
 * \param dst_y
 *    vertical index to sub-image being generated
 * \param src_x
 *    horizontal index to source sub-image
 * \param src_y
 *    vertical index to source sub-image
 * \param store_int
 *    store shifted integer version of picture to temporary array for
 *    increased fidelity during application of the six tap filter
 ************************************************************************
 */
void getHorSubImageSixTap( StorablePicture *s, int dst_y, int dst_x, int src_y, int src_x )
{
  int is;
  int jpad;
  int ipad;
  int ypadded_size = s->size_y + 2 * IMG_PAD_SIZE;
  int xpadded_size = s->size_x + 2 * IMG_PAD_SIZE;
  int maxx = xpadded_size - 1;

  imgpel *wBufSrc, *wBufDst;
  int *iBufDst;
  int tap0 = ONE_FOURTH_TAP[0][0];
  int tap1 = ONE_FOURTH_TAP[0][1];
  int tap2 = ONE_FOURTH_TAP[0][2];

  for (jpad = 0; jpad < ypadded_size; jpad++)
  {
    wBufSrc = s->imgY_sub[src_y][src_x][jpad];
    wBufDst = s->imgY_sub[dst_y][dst_x][jpad];
    iBufDst = imgY_sub_tmp[jpad];

    // left padded area
    for (ipad = 0; ipad < 2; ipad++)
    {
      is =
        (tap0 * (wBufSrc[ipad]               + wBufSrc[ipad + 1]) +
        tap1  * (wBufSrc[imax (0, ipad - 1)] + wBufSrc[ipad + 2]) +
        tap2  * (wBufSrc[imax (0, ipad - 2)] + wBufSrc[ipad + 3]));

      wBufDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 5 ) );
      iBufDst[ipad] =  is;
    }
    // center
    for (ipad = 2; ipad < xpadded_size - 3; ipad++)
    {
      is =
        (tap0 * (wBufSrc[ipad]     + wBufSrc[ipad + 1]) +
        tap1  * (wBufSrc[ipad - 1] + wBufSrc[ipad + 2]) +
        tap2  * (wBufSrc[ipad - 2] + wBufSrc[ipad + 3]));

      wBufDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 5 ) );
      iBufDst[ipad] =  is;
    }
    // right padded area
    for (ipad = xpadded_size - 3; ipad < xpadded_size; ipad++)
    {
      is =
        (tap0 * (wBufSrc[ipad]     + wBufSrc[imin (maxx, ipad + 1)]) +
        tap1  * (wBufSrc[ipad - 1] + wBufSrc[imin (maxx, ipad + 2)]) +
        tap2  * (wBufSrc[ipad - 2] + wBufSrc[imin (maxx, ipad + 3)]));

      wBufDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 5 ) );
      iBufDst[ipad] =  is;
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Does _vertical_ interpolation using the SIX TAP filters
 *
 * \param s
 *    pointer to StorablePicture structure
 * \param dst_x
 *    horizontal index to sub-image being generated
 * \param dst_y
 *    vertical index to sub-image being generated
 * \param src_x
 *    horizontal index to source sub-image
 * \param src_y
 *    vertical index to source sub-image
 * \param use_stored_int
 *    use stored shifted integer version of picture to temporary array for
 *    increased fidelity during application of the six tap filter
 ************************************************************************
 */
void getVerSubImageSixTap( StorablePicture *s, int dst_y, int dst_x, int src_y, int src_x, int use_stored_int )
{
  int is;
  int jpad;
  int jlow1, jlow2, jhigh1, jhigh2, jhigh3;
  int ipad;
  int ypadded_size = s->size_y + 2 * IMG_PAD_SIZE;
  int xpadded_size = s->size_x + 2 * IMG_PAD_SIZE;
  int maxy = ypadded_size - 1;

  imgpel **wxBufSrc, **wxBufDst, *wxLineDst;
  int tap0 = ONE_FOURTH_TAP[0][0];
  int tap1 = ONE_FOURTH_TAP[0][1];
  int tap2 = ONE_FOURTH_TAP[0][2];

  wxBufSrc = s->imgY_sub[src_y][src_x];
  wxBufDst = s->imgY_sub[dst_y][dst_x];

  if ( !use_stored_int ) { // causes code expansion but is better since we avoid too many
    // branches within the j loop
    // top
    for (jpad = 0; jpad < 2; jpad++)
    {
      wxLineDst = wxBufDst[jpad];
      jlow1  = imax (0, jpad - 1);
      jlow2  = imax (0, jpad - 2);
      jhigh1 = jpad + 1;
      jhigh2 = jpad + 2;
      jhigh3 = jpad + 3;
      for (ipad = 0; ipad < xpadded_size; ipad++)
      {
        is =
          (tap0 * (wxBufSrc[jpad ][ipad] + wxBufSrc[jhigh1][ipad]) +
          tap1 *  (wxBufSrc[jlow1][ipad] + wxBufSrc[jhigh2][ipad]) +
          tap2 *  (wxBufSrc[jlow2][ipad] + wxBufSrc[jhigh3][ipad]));

        wxLineDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 5 ) );
      }
    }
    // center
    for (jpad = 2; jpad < ypadded_size - 3; jpad++)
    {
      wxLineDst = wxBufDst[jpad];
      jlow1  = jpad - 1;
      jlow2  = jpad - 2;
      jhigh1 = jpad + 1;
      jhigh2 = jpad + 2;
      jhigh3 = jpad + 3;
      for (ipad = 0; ipad < xpadded_size; ipad++)
      {
        is =
          (tap0 * (wxBufSrc[jpad ][ipad] + wxBufSrc[jhigh1][ipad]) +
          tap1 *  (wxBufSrc[jlow1][ipad] + wxBufSrc[jhigh2][ipad]) +
          tap2 *  (wxBufSrc[jlow2][ipad] + wxBufSrc[jhigh3][ipad]));

        wxLineDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 5 ) );
      }
    }

    // bottom
    for (jpad = ypadded_size - 3; jpad < ypadded_size; jpad++)
    {
      wxLineDst = wxBufDst[jpad];
      jlow1  = jpad - 1;
      jlow2  = jpad - 2;
      jhigh1 = imin (maxy, jpad + 1);
      jhigh2 = imin (maxy, jpad + 2);
      jhigh3 = imin (maxy, jpad + 3);
      for (ipad = 0; ipad < xpadded_size; ipad++)
      {
        is =
          (tap0 * (wxBufSrc[jpad ][ipad] + wxBufSrc[jhigh1][ipad]) +
          tap1 *  (wxBufSrc[jlow1][ipad] + wxBufSrc[jhigh2][ipad]) +
          tap2 *  (wxBufSrc[jlow2][ipad] + wxBufSrc[jhigh3][ipad]));

        wxLineDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 5 ) );
      }
    }
  }
  else
  {
    // top
    for (jpad = 0; jpad < 2; jpad++)
    {
      wxLineDst = wxBufDst[jpad];
      jlow1  = imax (0, jpad - 1);
      jlow2  = imax (0, jpad - 2);
      jhigh1 = jpad + 1;
      jhigh2 = jpad + 2;
      jhigh3 = jpad + 3;

      for (ipad = 0; ipad < xpadded_size; ipad++)
      {
        is =
          (tap0 * (imgY_sub_tmp[jpad ][ipad] + imgY_sub_tmp[jhigh1][ipad]) +
          tap1 *  (imgY_sub_tmp[jlow1][ipad] + imgY_sub_tmp[jhigh2][ipad]) +
          tap2 *  (imgY_sub_tmp[jlow2][ipad] + imgY_sub_tmp[jhigh3][ipad]));

        wxLineDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 10 ) );
      }
    }

    // center
    for (jpad = 2; jpad < ypadded_size - 3; jpad++)
    {
      wxLineDst = wxBufDst[jpad];
      jlow1  = jpad - 1;
      jlow2  = jpad - 2;
      jhigh1 = jpad + 1;
      jhigh2 = jpad + 2;
      jhigh3 = jpad + 3;
      for (ipad = 0; ipad < xpadded_size; ipad++)
      {
        is =
          (tap0 * (imgY_sub_tmp[jpad ][ipad] + imgY_sub_tmp[jhigh1][ipad]) +
          tap1 *  (imgY_sub_tmp[jlow1][ipad] + imgY_sub_tmp[jhigh2][ipad]) +
          tap2 *  (imgY_sub_tmp[jlow2][ipad] + imgY_sub_tmp[jhigh3][ipad]));

        wxLineDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 10 ) );
      }
    }

    // bottom
    for (jpad = ypadded_size - 3; jpad < ypadded_size; jpad++)
    {
      wxLineDst = wxBufDst[jpad];
      jlow1  = jpad - 1;
      jlow2  = jpad - 2;
      jhigh1 = imin (maxy, jpad + 1);
      jhigh2 = imin (maxy, jpad + 2);
      jhigh3 = imin (maxy, jpad + 3);
      for (ipad = 0; ipad < xpadded_size; ipad++)
      {
        is =
          (tap0 * (imgY_sub_tmp[jpad ][ipad] + imgY_sub_tmp[jhigh1][ipad]) +
          tap1 *  (imgY_sub_tmp[jlow1][ipad] + imgY_sub_tmp[jhigh2][ipad]) +
          tap2 *  (imgY_sub_tmp[jlow2][ipad] + imgY_sub_tmp[jhigh3][ipad]));

        wxLineDst[ipad] = (imgpel) iClip3 (0, img->max_imgpel_value, rshift_rnd_sf( is, 10 ) );
      }
    }

  }
}

/*!
 ************************************************************************
 * \brief
 *    Does _horizontal_ interpolation using the BiLinear filter
 *
 * \param s
 *    pointer to StorablePicture structure
 * \param dst_x
 *    horizontal index to sub-image being generated
 * \param dst_y
 *    vertical index to sub-image being generated
 * \param src_x_l
 *    horizontal index to "LEFT" source sub-image
 * \param src_y_l
 *    vertical index to "LEFT" source sub-image
 * \param src_x_r
 *    horizontal index to "RIGHT" source sub-image
 * \param src_y_r
 *    vertical index to "RIGHT" source sub-image
 * \param offset
 *    offset (either +0 or +1) for RIGHT sub-image HOR coordinate
 ************************************************************************
 */
void getHorSubImageBiLinear( StorablePicture *s, int dst_y, int dst_x, int src_y_l, int src_x_l, int src_y_r, int src_x_r, int offset )
{
  int jpad;
  int ipad;
  int ypadded_size = s->size_y + 2 * IMG_PAD_SIZE;
  int xpadded_size = s->size_x + 2 * IMG_PAD_SIZE;
  int maxx = xpadded_size - 1;

  imgpel *wBufSrcL, *wBufSrcR, *wBufDst;
  int xpadded_size_left = maxx - offset;

  for (jpad = 0; jpad < ypadded_size; jpad++)
  {
    wBufSrcL = s->imgY_sub[src_y_l][src_x_l][jpad];
    wBufSrcR = s->imgY_sub[src_y_r][src_x_r][jpad];
    wBufDst = s->imgY_sub[dst_y][dst_x][jpad];

    // left padded area + center
    for (ipad = 0; ipad < xpadded_size_left; ipad++)
    {
      wBufDst[ipad] = (imgpel)
        rshift_rnd_sf( wBufSrcL[ipad] + wBufSrcR[ipad + offset], 1 );
    }
    // right padded area
    for (ipad = xpadded_size_left; ipad < xpadded_size; ipad++)
    {
      wBufDst[ipad] = (imgpel)
        rshift_rnd_sf( wBufSrcL[ipad] + wBufSrcR[maxx], 1 );
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Does _vertical_ interpolation using the BiLinear filter
 *
 * \param s
 *    pointer to StorablePicture structure
 * \param dst_x
 *    horizontal index to sub-image being generated
 * \param dst_y
 *    vertical index to sub-image being generated
 * \param src_x_l
 *    horizontal index to "TOP" source sub-image
 * \param src_y_l
 *    vertical index to "TOP" source sub-image
 * \param src_x_r
 *    horizontal index to "BOTTOM" source sub-image
 * \param src_y_r
 *    vertical index to "BOTTOM" source sub-image
 * \param offset
 *    offset (either +0 or +1) for BOTTOM sub-image VER coordinate
 ************************************************************************
 */
void getVerSubImageBiLinear( StorablePicture *s, int dst_y, int dst_x, int src_y_l, int src_x_l, int src_y_r, int src_x_r, int offset )
{
  int jpad;
  int ipad;
  int ypadded_size = s->size_y + 2 * IMG_PAD_SIZE;
  int xpadded_size = s->size_x + 2 * IMG_PAD_SIZE;
  int maxy = ypadded_size - 1;

  imgpel *wBufSrcL, *wBufSrcR, *wBufDst;
  int ypadded_size_top = maxy - offset;

  // top
  for (jpad = 0; jpad < ypadded_size_top; jpad++)
  {
    wBufSrcL = s->imgY_sub[src_y_l][src_x_l][jpad];
    wBufDst  = s->imgY_sub[dst_y][dst_x][jpad];
    wBufSrcR = s->imgY_sub[src_y_r][src_x_r][jpad + offset];

    for (ipad = 0; ipad < xpadded_size; ipad++)
    {
      wBufDst[ipad] = (imgpel)
        rshift_rnd_sf(wBufSrcL[ipad] + wBufSrcR[ipad], 1);
    }
  }
  // bottom
  for (jpad = ypadded_size_top; jpad < ypadded_size; jpad++)
  {
    wBufSrcL = s->imgY_sub[src_y_l][src_x_l][jpad];
    wBufDst  = s->imgY_sub[dst_y  ][dst_x  ][jpad];
    wBufSrcR = s->imgY_sub[src_y_r][src_x_r][maxy];

    for (ipad = 0; ipad < xpadded_size; ipad++)
    {
      wBufDst[ipad] = (imgpel)
        rshift_rnd_sf(wBufSrcL[ipad] + wBufSrcR[ipad], 1);
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Does _diagonal_ interpolation using the BiLinear filter
 *
 * \param s
 *    pointer to StorablePicture structure
 * \param dst_x
 *    horizontal index to sub-image being generated
 * \param dst_y
 *    vertical index to sub-image being generated
 * \param src_x_l
 *    horizontal index to "TOP" source sub-image
 * \param src_y_l
 *    vertical index to "TOP" source sub-image
 * \param src_x_r
 *    horizontal index to "BOTTOM" source sub-image
 * \param src_y_r
 *    vertical index to "BOTTOM" source sub-image
 * \param offset_y_l
 *    Y offset (either +0 or +1) for TOP sub-image coordinate
 * \param offset_x_l
 *    X offset (either +0 or +1) for TOP sub-image coordinate
 * \param offset_y_r
 *    Y offset (either +0 or +1) for BOTTOM sub-image coordinate
 * \param offset_x_r
 *    X offset (either +0 or +1) for BOTTOM sub-image coordinate
 ************************************************************************
 */
void getDiagSubImageBiLinear( StorablePicture *s, int dst_y, int dst_x, int src_y_l, int src_x_l, int src_y_r, int src_x_r, int offset_y_l, int offset_x_l,
                             int offset_y_r, int offset_x_r )
{
  int jpad;
  int ipad;
  int ypadded_size = s->size_y + 2 * IMG_PAD_SIZE;
  int xpadded_size = s->size_x + 2 * IMG_PAD_SIZE;
  int maxx = xpadded_size - 1;
  int maxy = ypadded_size - 1;

  imgpel *wBufSrcL, *wBufSrcR, *wBufDst;
  // -1 explanation: offsets can be maximally one so let's assume the worst and avoid too many checks
  int ypadded_size_top = ypadded_size - IMG_PAD_SIZE - 1;

  for (jpad = 0; jpad < ypadded_size_top; jpad++)
  {
    wBufSrcL = s->imgY_sub[src_y_l][src_x_l][jpad + offset_y_l];
    wBufSrcR = s->imgY_sub[src_y_r][src_x_r][jpad + offset_y_r];
    wBufDst = s->imgY_sub[dst_y][dst_x][jpad];

    for (ipad = 0; ipad < xpadded_size; ipad++)
    {
      wBufDst[ipad] = (imgpel)
        rshift_rnd_sf(wBufSrcL[imin (maxx, ipad + offset_x_l)] +
        wBufSrcR[imin (maxx, ipad + offset_x_r)], 1);
    }
  }
  for (jpad = ypadded_size_top; jpad < ypadded_size; jpad++)
  {
    wBufSrcL = s->imgY_sub[src_y_l][src_x_l][imin (maxy, jpad + offset_y_l)];
    wBufSrcR = s->imgY_sub[src_y_r][src_x_r][imin (maxy, jpad + offset_y_r)];
    wBufDst = s->imgY_sub[dst_y][dst_x][jpad];

    for (ipad = 0; ipad < xpadded_size; ipad++)
    {
      wBufDst[ipad] = (imgpel)
        rshift_rnd_sf(wBufSrcL[imin (maxx, ipad + offset_x_l)] +
        wBufSrcR[imin (maxx, ipad + offset_x_r)], 1);
    }
  }
}
