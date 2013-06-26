/*!
 ***************************************************************************
 * \file
 *    img_luma.h
 *
 * \author
 *    Athanasios Leontaris           <aleon@dolby.com>
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *
 * \date
 *    4. October 2006
 *
 * \brief
 *    Headerfile for luma interpolation functions
 **************************************************************************
 */

#ifndef _IMG_LUMA_H_
#define _IMG_LUMA_H_

void getSubImagesLuma       ( StorablePicture *s );
void getHorSubImageSixTap   ( StorablePicture *s, int dst_y, int dst_x, int src_y, int src_x );
void getVerSubImageSixTap   ( StorablePicture *s, int dst_y, int dst_x, int src_y, int src_x, int use_stored_int );
void getHorSubImageBiLinear ( StorablePicture *s, int dst_y, int dst_x, int src_y_l, int src_x_l, int src_y_r, int src_x_r, int offset );
void getVerSubImageBiLinear ( StorablePicture *s, int dst_y, int dst_x, int src_y_l, int src_x_l, int src_y_r, int src_x_r, int offset );
void getDiagSubImageBiLinear( StorablePicture *s, int dst_y, int dst_x, int src_y_l, int src_x_l, int src_y_r, int src_x_r, int offset_y_l, int offset_x_l,
                              int offset_y_r, int offset_x_r );

#endif // _IMG_LUMA_H_
