/*!
 ************************************************************************
 * \file  sei.c
 *
 * \brief
 *    Functions to implement SEI messages
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Dong Tian        <tian@cs.tut.fi>
 *    - Karsten Suehring <suehring@hhi.de>
 ************************************************************************
 */

#include "contributors.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "global.h"
#include "memalloc.h"
#include "sei.h"
#include "vlc.h"
#include "header.h"
#include "mbuffer.h"
#include "parset.h"

extern int UsedBits;

extern seq_parameter_set_rbsp_t SeqParSet[MAXSPS];


// #define PRINT_BUFFERING_PERIOD_INFO    // uncomment to print buffering period SEI info
// #define PRINT_PCITURE_TIMING_INFO      // uncomment to print picture timing SEI info
// #define WRITE_MAP_IMAGE                // uncomment to write spare picture map
// #define PRINT_SUBSEQUENCE_INFO         // uncomment to print sub-sequence SEI info
// #define PRINT_SUBSEQUENCE_LAYER_CHAR   // uncomment to print sub-sequence layer characteristics SEI info
// #define PRINT_SUBSEQUENCE_CHAR         // uncomment to print sub-sequence characteristics SEI info
// #define PRINT_SCENE_INFORMATION        // uncomment to print scene information SEI info
// #define PRINT_PAN_SCAN_RECT            // uncomment to print pan-scan rectangle SEI info
// #define PRINT_RECOVERY_POINT            // uncomment to print random access point SEI info
// #define PRINT_FILLER_PAYLOAD_INFO      // uncomment to print filler payload SEI info
// #define PRINT_DEC_REF_PIC_MARKING      // uncomment to print decoded picture buffer management repetition SEI info
// #define PRINT_RESERVED_INFO            // uncomment to print reserved SEI info
// #define PRINT_USER_DATA_UNREGISTERED_INFO          // uncomment to print unregistered user data SEI info
// #define PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO  // uncomment to print ITU-T T.35 user data SEI info
// #define PRINT_FULL_FRAME_FREEZE_INFO               // uncomment to print full-frame freeze SEI info
// #define PRINT_FULL_FRAME_FREEZE_RELEASE_INFO       // uncomment to print full-frame freeze release SEI info
// #define PRINT_FULL_FRAME_SNAPSHOT_INFO             // uncomment to print full-frame snapshot SEI info
// #define PRINT_PROGRESSIVE_REFINEMENT_END_INFO      // uncomment to print Progressive refinement segment start SEI info
// #define PRINT_PROGRESSIVE_REFINEMENT_END_INFO      // uncomment to print Progressive refinement segment end SEI info
// #define PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO    // uncomment to print Motion-constrained slice group set SEI info
// #define PRINT_FILM_GRAIN_CHARACTERISTICS_INFO      // uncomment to print Film grain characteristics SEI info
// #define PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO // uncomment to print deblocking filter display preference SEI info
// #define PRINT_STEREO_VIDEO_INFO_INFO               // uncomment to print stero video SEI info

/*!
 ************************************************************************
 *  \brief
 *     Interpret the SEI rbsp
 *  \param msg
 *     a pointer that point to the sei message.
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void InterpretSEIMessage(byte* msg, int size, ImageParameters *img)
{
  int payload_type = 0;
  int payload_size = 0;
  int offset = 1;
  byte tmp_byte;
  do
  {
    // sei_message();
    payload_type = 0;
    tmp_byte = msg[offset++];
    while (tmp_byte == 0xFF)
    {
      payload_type += 255;
      tmp_byte = msg[offset++];
    }
    payload_type += tmp_byte;   // this is the last byte

    payload_size = 0;
    tmp_byte = msg[offset++];
    while (tmp_byte == 0xFF)
    {
      payload_size += 255;
      tmp_byte = msg[offset++];
    }
    payload_size += tmp_byte;   // this is the last byte

    switch ( payload_type )     // sei_payload( type, size );
    {
    case  SEI_BUFFERING_PERIOD:
      interpret_buffering_period_info( msg+offset, payload_size, img );
      break;
    case  SEI_PIC_TIMING:
      interpret_picture_timing_info( msg+offset, payload_size, img );
      break;
    case  SEI_PAN_SCAN_RECT:
      interpret_pan_scan_rect_info( msg+offset, payload_size, img );
      break;
    case  SEI_FILLER_PAYLOAD:
      interpret_filler_payload_info( msg+offset, payload_size, img );
      break;
    case  SEI_USER_DATA_REGISTERED_ITU_T_T35:
      interpret_user_data_registered_itu_t_t35_info( msg+offset, payload_size, img );
      break;
    case  SEI_USER_DATA_UNREGISTERED:
      interpret_user_data_unregistered_info( msg+offset, payload_size, img );
      break;
    case  SEI_RECOVERY_POINT:
      interpret_recovery_point_info( msg+offset, payload_size, img );
      break;
    case  SEI_DEC_REF_PIC_MARKING_REPETITION:
      interpret_dec_ref_pic_marking_repetition_info( msg+offset, payload_size, img );
      break;
    case  SEI_SPARE_PIC:
      interpret_spare_pic( msg+offset, payload_size, img );
      break;
    case  SEI_SCENE_INFO:
      interpret_scene_information( msg+offset, payload_size, img );
      break;
    case  SEI_SUB_SEQ_INFO:
      interpret_subsequence_info( msg+offset, payload_size, img );
      break;
    case  SEI_SUB_SEQ_LAYER_CHARACTERISTICS:
      interpret_subsequence_layer_characteristics_info( msg+offset, payload_size, img );
      break;
    case  SEI_SUB_SEQ_CHARACTERISTICS:
      interpret_subsequence_characteristics_info( msg+offset, payload_size, img );
      break;
    case  SEI_FULL_FRAME_FREEZE:
      interpret_full_frame_freeze_info( msg+offset, payload_size, img );
      break;
    case  SEI_FULL_FRAME_FREEZE_RELEASE:
      interpret_full_frame_freeze_release_info( msg+offset, payload_size, img );
      break;
    case  SEI_FULL_FRAME_SNAPSHOT:
      interpret_full_frame_snapshot_info( msg+offset, payload_size, img );
      break;
    case  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START:
      interpret_progressive_refinement_end_info( msg+offset, payload_size, img );
      break;
    case  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END:
      interpret_progressive_refinement_end_info( msg+offset, payload_size, img );
      break;
    case  SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET:
      interpret_motion_constrained_slice_group_set_info( msg+offset, payload_size, img );
    case  SEI_FILM_GRAIN_CHARACTERISTICS:
      interpret_film_grain_characteristics_info ( msg+offset, payload_size, img );
      break;
    case  SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE:
      interpret_deblocking_filter_display_preference_info ( msg+offset, payload_size, img );
      break;
    case  SEI_STEREO_VIDEO_INFO:
      interpret_stereo_video_info_info ( msg+offset, payload_size, img );
      break;
    default:
      interpret_reserved_info( msg+offset, payload_size, img );
      break;
    }
    offset += payload_size;

  } while( msg[offset] != 0x80 );    // more_rbsp_data()  msg[offset] != 0x80
  // ignore the trailing bits rbsp_trailing_bits();
  assert(msg[offset] == 0x80);      // this is the trailing bits
  assert( offset+1 == size );
}


/*!
************************************************************************
*  \brief
*     Interpret the spare picture SEI message
*  \param payload
*     a pointer that point to the sei payload
*  \param size
*     the size of the sei message
*  \param img
*     the image pointer
*
************************************************************************
*/
void interpret_spare_pic( byte* payload, int size, ImageParameters *img )
{
  int i,x,y;
  Bitstream* buf;
  int bit0, bit1, bitc, no_bit0;
  int target_frame_num = 0;
  int num_spare_pics;
  int delta_spare_frame_num, CandidateSpareFrameNum, SpareFrameNum = 0;
  int ref_area_indicator;

  int m, n, left, right, top, bottom,directx, directy;
  byte ***map;

#ifdef WRITE_MAP_IMAGE
  int symbol_size_in_bytes = img->pic_unit_bitsize_on_disk/8;
  int  j, k, i0, j0, tmp, kk;
  char filename[20] = "map_dec.yuv";
  FILE *fp;
  imgpel** Y;
  static int old_pn=-1;
  static int first = 1;

  printf("Spare picture SEI message\n");
#endif

  UsedBits = 0;

  assert( payload!=NULL);
  assert( img!=NULL);

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  target_frame_num = ue_v("SEI: target_frame_num", buf);

#ifdef WRITE_MAP_IMAGE
  printf( "target_frame_num is %d\n", target_frame_num );
#endif

  num_spare_pics = 1 + ue_v("SEI: num_spare_pics_minus1", buf);

#ifdef WRITE_MAP_IMAGE
  printf( "num_spare_pics is %d\n", num_spare_pics );
#endif

  get_mem3D(&map, num_spare_pics, img->height/16, img->width/16);

  for (i=0; i<num_spare_pics; i++)
  {
    if (i==0)
    {
      CandidateSpareFrameNum = target_frame_num - 1;
      if ( CandidateSpareFrameNum < 0 ) CandidateSpareFrameNum = MAX_FN - 1;
    }
    else
      CandidateSpareFrameNum = SpareFrameNum;

    delta_spare_frame_num = ue_v("SEI: delta_spare_frame_num", buf);

    SpareFrameNum = CandidateSpareFrameNum - delta_spare_frame_num;
    if( SpareFrameNum < 0 )
      SpareFrameNum = MAX_FN + SpareFrameNum;

    ref_area_indicator = ue_v("SEI: ref_area_indicator", buf);

    switch ( ref_area_indicator )
    {
    case 0:   // The whole frame can serve as spare picture
      for (y=0; y<img->height/16; y++)
        for (x=0; x<img->width/16; x++)
          map[i][y][x] = 0;
      break;
    case 1:   // The map is not compressed
      for (y=0; y<img->height/16; y++)
        for (x=0; x<img->width/16; x++)
        {
          map[i][y][x] = u_1("SEI: ref_mb_indicator", buf);
        }
      break;
    case 2:   // The map is compressed
              //!KS: could not check this function, description is unclear (as stated in Ed. Note)
      bit0 = 0;
      bit1 = 1;
      bitc = bit0;
      no_bit0 = -1;

      x = ( img->width/16 - 1 ) / 2;
      y = ( img->height/16 - 1 ) / 2;
      left = right = x;
      top = bottom = y;
      directx = 0;
      directy = 1;

      for (m=0; m<img->height/16; m++)
        for (n=0; n<img->width/16; n++)
        {

          if (no_bit0<0)
          {
            no_bit0 = ue_v("SEI: zero_run_length", buf);
          }
          if (no_bit0>0) map[i][y][x] = bit0;
          else map[i][y][x] = bit1;
          no_bit0--;

          // go to the next mb:
          if ( directx == -1 && directy == 0 )
          {
            if (x > left) x--;
            else if (x == 0)
            {
              y = bottom + 1;
              bottom++;
              directx = 1;
              directy = 0;
            }
            else if (x == left)
            {
              x--;
              left--;
              directx = 0;
              directy = 1;
            }
          }
          else if ( directx == 1 && directy == 0 )
          {
            if (x < right) x++;
            else if (x == img->width/16 - 1)
            {
              y = top - 1;
              top--;
              directx = -1;
              directy = 0;
            }
            else if (x == right)
            {
              x++;
              right++;
              directx = 0;
              directy = -1;
            }
          }
          else if ( directx == 0 && directy == -1 )
          {
            if ( y > top) y--;
            else if (y == 0)
            {
              x = left - 1;
              left--;
              directx = 0;
              directy = 1;
            }
            else if (y == top)
            {
              y--;
              top--;
              directx = -1;
              directy = 0;
            }
          }
          else if ( directx == 0 && directy == 1 )
          {
            if (y < bottom) y++;
            else if (y == img->height/16 - 1)
            {
              x = right+1;
              right++;
              directx = 0;
              directy = -1;
            }
            else if (y == bottom)
            {
              y++;
              bottom++;
              directx = 1;
              directy = 0;
            }
          }


        }
      break;
    default:
      printf( "Wrong ref_area_indicator %d!\n", ref_area_indicator );
      exit(0);
      break;
    }

  } // end of num_spare_pics

#ifdef WRITE_MAP_IMAGE
  // begin to write map seq
  if ( old_pn != img->number )
  {
    old_pn = img->number;
    get_mem2Dpel(&Y, img->height, img->width);
    if (first)
    {
      fp = fopen( filename, "wb" );
      first = 0;
    }
    else
      fp = fopen( filename, "ab" );
    assert( fp != NULL );
    for (kk=0; kk<num_spare_pics; kk++)
    {
      for (i=0; i < img->height/16; i++)
        for (j=0; j < img->width/16; j++)
        {
          tmp=map[kk][i][j]==0? img->max_imgpel_value : 0;
          for (i0=0; i0<16; i0++)
            for (j0=0; j0<16; j0++)
              Y[i*16+i0][j*16+j0]=tmp;
        }

      // write the map image
      for (i=0; i < img->height; i++)
        for (j=0; j < img->width; j++)
          fwrite(&(Y[i][j]), symbol_size_in_bytes, 1, p_out);

      for (k=0; k < 2; k++)
        for (i=0; i < img->height/2; i++)
          for (j=0; j < img->width/2; j++)
            fwrite(&(img->dc_pred_value_chroma), symbol_size_in_bytes, 1, p_out);
    }
    fclose( fp );
    free_mem2Dpel( Y );
  }
  // end of writing map image
#undef WRITE_MAP_IMAGE
#endif

  free_mem3D( map, num_spare_pics );

  free(buf);
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Sub-sequence information SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_subsequence_info( byte* payload, int size, ImageParameters *img )
{
  Bitstream* buf;
  int sub_seq_layer_num, sub_seq_id, first_ref_pic_flag, leading_non_ref_pic_flag, last_pic_flag,
      sub_seq_frame_num_flag, sub_seq_frame_num;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  sub_seq_layer_num        = ue_v("SEI: sub_seq_layer_num"       , buf);
  sub_seq_id               = ue_v("SEI: sub_seq_id"              , buf);
  first_ref_pic_flag       = u_1 ("SEI: first_ref_pic_flag"      , buf);
  leading_non_ref_pic_flag = u_1 ("SEI: leading_non_ref_pic_flag", buf);
  last_pic_flag            = u_1 ("SEI: last_pic_flag"           , buf);
  sub_seq_frame_num_flag   = u_1 ("SEI: sub_seq_frame_num_flag"  , buf);
  if (sub_seq_frame_num_flag)
  {
    sub_seq_frame_num        = ue_v("SEI: sub_seq_frame_num"       , buf);
  }

#ifdef PRINT_SUBSEQUENCE_INFO
  printf("Sub-sequence information SEI message\n");
  printf("sub_seq_layer_num        = %d\n", sub_seq_layer_num );
  printf("sub_seq_id               = %d\n", sub_seq_id);
  printf("first_ref_pic_flag       = %d\n", first_ref_pic_flag);
  printf("leading_non_ref_pic_flag = %d\n", leading_non_ref_pic_flag);
  printf("last_pic_flag            = %d\n", last_pic_flag);
  printf("sub_seq_frame_num_flag   = %d\n", sub_seq_frame_num_flag);
  if (sub_seq_frame_num_flag)
  {
    printf("sub_seq_frame_num        = %d\n", sub_seq_frame_num);
  }
#endif

  free(buf);
#ifdef PRINT_SUBSEQUENCE_INFO
#undef PRINT_SUBSEQUENCE_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Sub-sequence layer characteristics SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_subsequence_layer_characteristics_info( byte* payload, int size, ImageParameters *img )
{
  Bitstream* buf;
  long num_sub_layers, accurate_statistics_flag, average_bit_rate, average_frame_rate;
  int i;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  num_sub_layers = 1 + ue_v("SEI: num_sub_layers_minus1", buf);

#ifdef PRINT_SUBSEQUENCE_LAYER_CHAR
  printf("Sub-sequence layer characteristics SEI message\n");
  printf("num_sub_layers_minus1 = %d\n", num_sub_layers - 1);
#endif

  for (i=0; i<num_sub_layers; i++)
  {
    accurate_statistics_flag = u_1(   "SEI: accurate_statistics_flag", buf);
    average_bit_rate         = u_v(16,"SEI: average_bit_rate"        , buf);
    average_frame_rate       = u_v(16,"SEI: average_frame_rate"      , buf);

#ifdef PRINT_SUBSEQUENCE_LAYER_CHAR
    printf("layer %d: accurate_statistics_flag = %ld \n", i, accurate_statistics_flag);
    printf("layer %d: average_bit_rate         = %ld \n", i, average_bit_rate);
    printf("layer %d: average_frame_rate       = %ld \n", i, average_frame_rate);
#endif
  }
  free (buf);
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Sub-sequence characteristics SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_subsequence_characteristics_info( byte* payload, int size, ImageParameters *img )
{
  Bitstream* buf;
  int i;
  int sub_seq_layer_num, sub_seq_id, duration_flag, average_rate_flag, accurate_statistics_flag;
  unsigned long sub_seq_duration, average_bit_rate, average_frame_rate;
  int num_referenced_subseqs, ref_sub_seq_layer_num, ref_sub_seq_id, ref_sub_seq_direction;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  sub_seq_layer_num = ue_v("SEI: sub_seq_layer_num", buf);
  sub_seq_id        = ue_v("SEI: sub_seq_id", buf);
  duration_flag     = u_1 ("SEI: duration_flag", buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("Sub-sequence characteristics SEI message\n");
  printf("sub_seq_layer_num = %d\n", sub_seq_layer_num );
  printf("sub_seq_id        = %d\n", sub_seq_id);
  printf("duration_flag     = %d\n", duration_flag);
#endif

  if ( duration_flag )
  {
    sub_seq_duration = u_v (32, "SEI: duration_flag", buf);
#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("sub_seq_duration = %ld\n", sub_seq_duration);
#endif
  }

  average_rate_flag = u_1 ("SEI: average_rate_flag", buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("average_rate_flag = %d\n", average_rate_flag);
#endif

  if ( average_rate_flag )
  {
    accurate_statistics_flag = u_1 (    "SEI: accurate_statistics_flag", buf);
    average_bit_rate         = u_v (16, "SEI: average_bit_rate", buf);
    average_frame_rate       = u_v (16, "SEI: average_frame_rate", buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("accurate_statistics_flag = %d\n", accurate_statistics_flag);
    printf("average_bit_rate         = %ld\n", average_bit_rate);
    printf("average_frame_rate       = %ld\n", average_frame_rate);
#endif
  }

  num_referenced_subseqs  = ue_v("SEI: num_referenced_subseqs", buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("num_referenced_subseqs = %d\n", num_referenced_subseqs);
#endif

  for (i=0; i<num_referenced_subseqs; i++)
  {
    ref_sub_seq_layer_num  = ue_v("SEI: ref_sub_seq_layer_num", buf);
    ref_sub_seq_id         = ue_v("SEI: ref_sub_seq_id", buf);
    ref_sub_seq_direction  = u_1 ("SEI: ref_sub_seq_direction", buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("ref_sub_seq_layer_num = %d\n", ref_sub_seq_layer_num);
    printf("ref_sub_seq_id        = %d\n", ref_sub_seq_id);
    printf("ref_sub_seq_direction = %d\n", ref_sub_seq_direction);
#endif
  }

  free( buf );
#ifdef PRINT_SUBSEQUENCE_CHAR
#undef PRINT_SUBSEQUENCE_CHAR
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Scene information SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_scene_information( byte* payload, int size, ImageParameters *img )
{
  Bitstream* buf;
  int scene_id, scene_transition_type, second_scene_id;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  scene_id              = ue_v("SEI: scene_id"             , buf);
  scene_transition_type = ue_v("SEI: scene_transition_type", buf);
  if ( scene_transition_type > 3 )
  {
    second_scene_id     = ue_v("SEI: scene_transition_type", buf);;
  }

#ifdef PRINT_SCENE_INFORMATION
  printf("Scene information SEI message\n");
  printf("scene_transition_type = %d\n", scene_transition_type);
  printf("scene_id              = %d\n", scene_id);
  if ( scene_transition_type > 3 )
  {
    printf("second_scene_id       = %d\n", second_scene_id);
  }
#endif
  free( buf );
#ifdef PRINT_SCENE_INFORMATION
#undef PRINT_SCENE_INFORMATION
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Filler payload SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_filler_payload_info( byte* payload, int size, ImageParameters *img )
{
  int payload_cnt = 0;

  while (payload_cnt<size)
  {
    if (payload[payload_cnt] == 0xFF)
    {
       payload_cnt++;
    }
  }


#ifdef PRINT_FILLER_PAYLOAD_INFO
  printf("Filler payload SEI message\n");
  if (payload_cnt==size)
  {
    printf("read %d bytes of filler payload\n", payload_cnt);
  }
  else
  {
    printf("error reading filler payload: not all bytes are 0xFF (%d of %d)\n", payload_cnt, size);
  }
#endif

#ifdef PRINT_FILLER_PAYLOAD_INFO
#undef PRINT_FILLER_PAYLOAD_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the User data unregistered SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_user_data_unregistered_info( byte* payload, int size, ImageParameters *img )
{
  int offset = 0;
  byte payload_byte;

#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
  printf("User data unregistered SEI message\n");
  printf("uuid_iso_11578 = 0x");
#endif
  assert (size>=16);

  for (offset = 0; offset < 16; offset++)
  {
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
    printf("%02x",payload[offset]);
#endif
  }

#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
    printf("\n");
#endif

  while (offset < size)
  {
    payload_byte = payload[offset];
    offset ++;
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
    printf("Unreg data payload_byte = %d\n", payload_byte);
#endif
  }
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
#undef PRINT_USER_DATA_UNREGISTERED_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the User data registered by ITU-T T.35 SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_user_data_registered_itu_t_t35_info( byte* payload, int size, ImageParameters *img )
{
  int offset = 0;
  byte itu_t_t35_country_code, itu_t_t35_country_code_extension_byte, payload_byte;

  itu_t_t35_country_code = payload[offset];
  offset++;
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
  printf("User data registered by ITU-T T.35 SEI message\n");
  printf(" itu_t_t35_country_code = %d \n", itu_t_t35_country_code);
#endif
  if(itu_t_t35_country_code == 0xFF)
  {
    itu_t_t35_country_code_extension_byte = payload[offset];
    offset++;
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
    printf(" ITU_T_T35_COUNTRY_CODE_EXTENSION_BYTE %d \n", itu_t_t35_country_code_extension_byte);
#endif
  }
  while (offset < size)
  {
    payload_byte = payload[offset];
    offset ++;
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
    printf("itu_t_t35 payload_byte = %d\n", payload_byte);
#endif
  }
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
#undef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Pan scan rectangle SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_pan_scan_rect_info( byte* payload, int size, ImageParameters *img )
{
  int pan_scan_rect_cancel_flag;
  int pan_scan_cnt_minus1, i;
  int pan_scan_rect_repetition_period;
  int pan_scan_rect_id, pan_scan_rect_left_offset, pan_scan_rect_right_offset;
  int pan_scan_rect_top_offset, pan_scan_rect_bottom_offset;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  pan_scan_rect_id = ue_v("SEI: pan_scan_rect_id", buf);

  pan_scan_rect_cancel_flag = u_1("SEI: pan_scan_rect_cancel_flag", buf);
  if (!pan_scan_rect_cancel_flag) {
    pan_scan_cnt_minus1 = ue_v("SEI: pan_scan_cnt_minus1", buf);
    for (i = 0; i <= pan_scan_cnt_minus1; i++) {
      pan_scan_rect_left_offset   = se_v("SEI: pan_scan_rect_left_offset"  , buf);
      pan_scan_rect_right_offset  = se_v("SEI: pan_scan_rect_right_offset" , buf);
      pan_scan_rect_top_offset    = se_v("SEI: pan_scan_rect_top_offset"   , buf);
      pan_scan_rect_bottom_offset = se_v("SEI: pan_scan_rect_bottom_offset", buf);
#ifdef PRINT_PAN_SCAN_RECT
      printf("Pan scan rectangle SEI message %d/%d\n", i, pan_scan_cnt_minus1);
      printf("pan_scan_rect_id            = %d\n", pan_scan_rect_id);
      printf("pan_scan_rect_left_offset   = %d\n", pan_scan_rect_left_offset);
      printf("pan_scan_rect_right_offset  = %d\n", pan_scan_rect_right_offset);
      printf("pan_scan_rect_top_offset    = %d\n", pan_scan_rect_top_offset);
      printf("pan_scan_rect_bottom_offset = %d\n", pan_scan_rect_bottom_offset);
#endif
    }
    pan_scan_rect_repetition_period = ue_v("SEI: pan_scan_rect_repetition_period", buf);
  }

  free (buf);
#ifdef PRINT_PAN_SCAN_RECT
#undef PRINT_PAN_SCAN_RECT
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Random access point SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_recovery_point_info( byte* payload, int size, ImageParameters *img )
{
  int recovery_frame_cnt, exact_match_flag, broken_link_flag, changing_slice_group_idc;


  Bitstream* buf;


  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  recovery_frame_cnt       = ue_v(    "SEI: recovery_frame_cnt"      , buf);
  exact_match_flag         = u_1 (    "SEI: exact_match_flag"        , buf);
  broken_link_flag         = u_1 (    "SEI: broken_link_flag"        , buf);
  changing_slice_group_idc = u_v ( 2, "SEI: changing_slice_group_idc", buf);

  img->recovery_point = 1;
  img->recovery_frame_cnt = recovery_frame_cnt;

#ifdef PRINT_RECOVERY_POINT
  printf("Recovery point SEI message\n");
  printf("recovery_frame_cnt       = %d\n", recovery_frame_cnt);
  printf("exact_match_flag         = %d\n", exact_match_flag);
  printf("broken_link_flag         = %d\n", broken_link_flag);
  printf("changing_slice_group_idc = %d\n", changing_slice_group_idc);
#endif
  free (buf);
#ifdef PRINT_RECOVERY_POINT
#undef PRINT_RECOVERY_POINT
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Decoded Picture Buffer Management Repetition SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_dec_ref_pic_marking_repetition_info( byte* payload, int size, ImageParameters *img )
{
  int original_idr_flag, original_frame_num;

  DecRefPicMarking_t *tmp_drpm;

  DecRefPicMarking_t *old_drpm;
  int old_idr_flag , old_no_output_of_prior_pics_flag, old_long_term_reference_flag , old_adaptive_ref_pic_buffering_flag;


  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  original_idr_flag     = u_1 (    "SEI: original_idr_flag"    , buf);
  original_frame_num    = ue_v(    "SEI: original_frame_num"   , buf);

#ifdef PRINT_DEC_REF_PIC_MARKING
  printf("Decoded Picture Buffer Management Repetition SEI message\n");
  printf("original_idr_flag       = %d\n", original_idr_flag);
  printf("original_frame_num      = %d\n", original_frame_num);
#endif

  // we need to save everything that is probably overwritten in dec_ref_pic_marking()
  old_drpm = img->dec_ref_pic_marking_buffer;
  old_idr_flag = img->idr_flag;

  old_no_output_of_prior_pics_flag = img->no_output_of_prior_pics_flag;
  old_long_term_reference_flag = img->long_term_reference_flag;
  old_adaptive_ref_pic_buffering_flag = img->adaptive_ref_pic_buffering_flag;

  // set new initial values
  img->idr_flag = original_idr_flag;
  img->dec_ref_pic_marking_buffer = NULL;

  dec_ref_pic_marking(buf);

  // print out decoded values
#ifdef PRINT_DEC_REF_PIC_MARKING
  if (img->idr_flag)
  {
    printf("no_output_of_prior_pics_flag = %d\n", img->no_output_of_prior_pics_flag);
    printf("long_term_reference_flag     = %d\n", img->long_term_reference_flag);
  }
  else
  {
    printf("adaptive_ref_pic_buffering_flag  = %d\n", img->adaptive_ref_pic_buffering_flag);
    if (img->adaptive_ref_pic_buffering_flag)
    {
      tmp_drpm=img->dec_ref_pic_marking_buffer;
      while (tmp_drpm != NULL)
      {
        printf("memory_management_control_operation  = %d\n", tmp_drpm->memory_management_control_operation);

        if ((tmp_drpm->memory_management_control_operation==1)||(tmp_drpm->memory_management_control_operation==3))
        {
          printf("difference_of_pic_nums_minus1        = %d\n", tmp_drpm->difference_of_pic_nums_minus1);
        }
        if (tmp_drpm->memory_management_control_operation==2)
        {
          printf("long_term_pic_num                    = %d\n", tmp_drpm->long_term_pic_num);
        }
        if ((tmp_drpm->memory_management_control_operation==3)||(tmp_drpm->memory_management_control_operation==6))
        {
          printf("long_term_frame_idx                  = %d\n", tmp_drpm->long_term_frame_idx);
        }
        if (tmp_drpm->memory_management_control_operation==4)
        {
          printf("max_long_term_pic_idx_plus1          = %d\n", tmp_drpm->max_long_term_frame_idx_plus1);
        }
        tmp_drpm = tmp_drpm->Next;
      }
    }
  }
#endif

  while (img->dec_ref_pic_marking_buffer)
  {
    tmp_drpm=img->dec_ref_pic_marking_buffer;

    img->dec_ref_pic_marking_buffer=tmp_drpm->Next;
    free (tmp_drpm);
  }

  // restore old values in img
  img->dec_ref_pic_marking_buffer = old_drpm;
  img->idr_flag = old_idr_flag;
  img->no_output_of_prior_pics_flag = old_no_output_of_prior_pics_flag;
  img->long_term_reference_flag = old_long_term_reference_flag;
  img->adaptive_ref_pic_buffering_flag = old_adaptive_ref_pic_buffering_flag;

  free (buf);
#ifdef PRINT_DEC_REF_PIC_MARKING
#undef PRINT_DEC_REF_PIC_MARKING
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Full-frame freeze SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_full_frame_freeze_info( byte* payload, int size, ImageParameters *img )
{
#ifdef PRINT_FULL_FRAME_FREEZE_INFO
  printf("Full-frame freeze SEI message\n");
  if (size)
  {
    printf("payload size of this message should be zero, but is %d bytes.\n", size);
  }
#endif

#ifdef PRINT_FULL_FRAME_FREEZE_INFO
#undef PRINT_FULL_FRAME_FREEZE_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Full-frame freeze release SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_full_frame_freeze_release_info( byte* payload, int size, ImageParameters *img )
{
#ifdef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
  printf("Full-frame freeze release SEI message\n");
  if (size)
  {
    printf("payload size of this message should be zero, but is %d bytes.\n", size);
  }
#endif

#ifdef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
#undef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Full-frame snapshot SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_full_frame_snapshot_info( byte* payload, int size, ImageParameters *img )
{
  int snapshot_id;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  snapshot_id = ue_v("SEI: snapshot_id", buf);

#ifdef PRINT_FULL_FRAME_SNAPSHOT_INFO
  printf("Full-frame snapshot SEI message\n");
  printf("snapshot_id = %d\n", snapshot_id);
#endif
  free (buf);
#ifdef PRINT_FULL_FRAME_SNAPSHOT_INFO
#undef PRINT_FULL_FRAME_SNAPSHOT_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Progressive refinement segment start SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_progressive_refinement_start_info( byte* payload, int size, ImageParameters *img )
{
  int progressive_refinement_id, num_refinement_steps_minus1;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  progressive_refinement_id   = ue_v("SEI: progressive_refinement_id"  , buf);
  num_refinement_steps_minus1 = ue_v("SEI: num_refinement_steps_minus1", buf);

#ifdef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
  printf("Progressive refinement segment start SEI message\n");
  printf("progressive_refinement_id   = %d\n", progressive_refinement_id);
  printf("num_refinement_steps_minus1 = %d\n", num_refinement_steps_minus1);
#endif
  free (buf);
#ifdef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
#undef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Progressive refinement segment end SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_progressive_refinement_end_info( byte* payload, int size, ImageParameters *img )
{
  int progressive_refinement_id;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  progressive_refinement_id   = ue_v("SEI: progressive_refinement_id"  , buf);

#ifdef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
  printf("Progressive refinement segment end SEI message\n");
  printf("progressive_refinement_id   = %d\n", progressive_refinement_id);
#endif
  free (buf);
#ifdef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
#undef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Motion-constrained slice group set SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_motion_constrained_slice_group_set_info( byte* payload, int size, ImageParameters *img )
{
  int num_slice_groups_minus1, slice_group_id, exact_match_flag, pan_scan_rect_flag, pan_scan_rect_id;
  int i;
  int sliceGroupSize;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  num_slice_groups_minus1   = ue_v("SEI: num_slice_groups_minus1"  , buf);
  sliceGroupSize = CeilLog2( num_slice_groups_minus1 + 1 );
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
  printf("Motion-constrained slice group set SEI message\n");
  printf("num_slice_groups_minus1   = %d\n", num_slice_groups_minus1);
#endif

  for (i=0; i<=num_slice_groups_minus1;i++)
  {

    slice_group_id   = u_v (sliceGroupSize, "SEI: slice_group_id" , buf)    ;
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
    printf("slice_group_id            = %d\n", slice_group_id);
#endif
  }

  exact_match_flag   = u_1("SEI: exact_match_flag"  , buf);
  pan_scan_rect_flag = u_1("SEI: pan_scan_rect_flag"  , buf);

#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
  printf("exact_match_flag         = %d\n", exact_match_flag);
  printf("pan_scan_rect_flag       = %d\n", pan_scan_rect_flag);
#endif

  if (pan_scan_rect_flag)
  {
    pan_scan_rect_id = ue_v("SEI: pan_scan_rect_id"  , buf);
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
    printf("pan_scan_rect_id         = %d\n", pan_scan_rect_id);
#endif
  }

  free (buf);
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
#undef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the film grain characteristics SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_film_grain_characteristics_info( byte* payload, int size, ImageParameters *img )
{
  int film_grain_characteristics_cancel_flag;
  int model_id, separate_colour_description_present_flag;
  int film_grain_bit_depth_luma_minus8, film_grain_bit_depth_chroma_minus8, film_grain_full_range_flag, film_grain_colour_primaries, film_grain_transfer_characteristics, film_grain_matrix_coefficients;
  int blending_mode_id, log2_scale_factor, comp_model_present_flag[3];
  int num_intensity_intervals_minus1, num_model_values_minus1;
  int intensity_interval_lower_bound, intensity_interval_upper_bound;
  int comp_model_value;
  int film_grain_characteristics_repetition_period;

  int c, i, j;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  film_grain_characteristics_cancel_flag = u_1("SEI: film_grain_characteristics_cancel_flag", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
  printf("film_grain_characteristics_cancel_flag = %d\n", film_grain_characteristics_cancel_flag);
#endif
  if(!film_grain_characteristics_cancel_flag)
  {

    model_id                                    = u_v(2, "SEI: model_id", buf);
    separate_colour_description_present_flag    = u_1("SEI: separate_colour_description_present_flag", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
    printf("model_id = %d\n", model_id);
    printf("separate_colour_description_present_flag = %d\n", separate_colour_description_present_flag);
#endif
    if (separate_colour_description_present_flag)
    {
      film_grain_bit_depth_luma_minus8          = u_v(3, "SEI: film_grain_bit_depth_luma_minus8", buf);
      film_grain_bit_depth_chroma_minus8        = u_v(3, "SEI: film_grain_bit_depth_chroma_minus8", buf);
      film_grain_full_range_flag                = u_v(1, "SEI: film_grain_full_range_flag", buf);
      film_grain_colour_primaries               = u_v(8, "SEI: film_grain_colour_primaries", buf);
      film_grain_transfer_characteristics       = u_v(8, "SEI: film_grain_transfer_characteristics", buf);
      film_grain_matrix_coefficients            = u_v(8, "SEI: film_grain_matrix_coefficients", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
      printf("film_grain_bit_depth_luma_minus8 = %d\n", film_grain_bit_depth_luma_minus8);
      printf("film_grain_bit_depth_chroma_minus8 = %d\n", film_grain_bit_depth_chroma_minus8);
      printf("film_grain_full_range_flag = %d\n", film_grain_full_range_flag);
      printf("film_grain_colour_primaries = %d\n", film_grain_colour_primaries);
      printf("film_grain_transfer_characteristics = %d\n", film_grain_transfer_characteristics);
      printf("film_grain_matrix_coefficients = %d\n", film_grain_matrix_coefficients);
#endif
    }
    blending_mode_id                            = u_v(2, "SEI: blending_mode_id", buf);
    log2_scale_factor                           = u_v(4, "SEI: log2_scale_factor", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
    printf("blending_mode_id = %d\n", blending_mode_id);
    printf("log2_scale_factor = %d\n", log2_scale_factor);
#endif
    for (c = 0; c < 3; c ++)
    {
      comp_model_present_flag[c]                = u_1("SEI: comp_model_present_flag", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
      printf("comp_model_present_flag = %d\n", comp_model_present_flag[c]);
#endif
    }
    for (c = 0; c < 3; c ++)
      if (comp_model_present_flag[c])
      {
        num_intensity_intervals_minus1          = u_v(8, "SEI: num_intensity_intervals_minus1", buf);
        num_model_values_minus1                 = u_v(3, "SEI: num_model_values_minus1", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
        printf("num_intensity_intervals_minus1 = %d\n", num_intensity_intervals_minus1);
        printf("num_model_values_minus1 = %d\n", num_model_values_minus1);
#endif
        for (i = 0; i <= num_intensity_intervals_minus1; i ++)
        {
          intensity_interval_lower_bound        = u_v(8, "SEI: intensity_interval_lower_bound", buf);
          intensity_interval_upper_bound        = u_v(8, "SEI: intensity_interval_upper_bound", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
          printf("intensity_interval_lower_bound = %d\n", intensity_interval_lower_bound);
          printf("intensity_interval_upper_bound = %d\n", intensity_interval_upper_bound);
#endif
          for (j = 0; j <= num_model_values_minus1; j++)
          {
            comp_model_value                    = se_v("SEI: comp_model_value", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
            printf("comp_model_value = %d\n", comp_model_value);
#endif
          }
        }
      }
    film_grain_characteristics_repetition_period = ue_v("SEI: film_grain_characteristics_repetition_period", buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
    printf("film_grain_characteristics_repetition_period = %d\n", film_grain_characteristics_repetition_period);
#endif
  }

  free (buf);
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
#undef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the deblocking filter display preference SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_deblocking_filter_display_preference_info( byte* payload, int size, ImageParameters *img )
{
  int deblocking_display_preference_cancel_flag;
  int display_prior_to_deblocking_preferred_flag, dec_frame_buffering_constraint_flag, deblocking_display_preference_repetition_period;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  deblocking_display_preference_cancel_flag             = u_1("SEI: deblocking_display_preference_cancel_flag", buf);
#ifdef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
  printf("deblocking_display_preference_cancel_flag = %d\n", deblocking_display_preference_cancel_flag);
#endif
  if(!deblocking_display_preference_cancel_flag)
  {
    display_prior_to_deblocking_preferred_flag            = u_1("SEI: display_prior_to_deblocking_preferred_flag", buf);
    dec_frame_buffering_constraint_flag                   = u_1("SEI: dec_frame_buffering_constraint_flag", buf);
    deblocking_display_preference_repetition_period       = ue_v("SEI: deblocking_display_preference_repetition_period", buf);
#ifdef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
    printf("display_prior_to_deblocking_preferred_flag = %d\n", display_prior_to_deblocking_preferred_flag);
    printf("dec_frame_buffering_constraint_flag = %d\n", dec_frame_buffering_constraint_flag);
    printf("deblocking_display_preference_repetition_period = %d\n", deblocking_display_preference_repetition_period);
#endif
  }

  free (buf);
#ifdef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
#undef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the stereo video info SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_stereo_video_info_info( byte* payload, int size, ImageParameters *img )
{
  int field_views_flags;
  int top_field_is_left_view_flag, current_frame_is_left_view_flag, next_frame_is_second_view_flag;
  int left_view_self_contained_flag;
  int right_view_self_contained_flag;

  Bitstream* buf;

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  field_views_flags = u_1("SEI: field_views_flags", buf);
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
  printf("field_views_flags = %d\n", field_views_flags);
#endif
  if (field_views_flags)
  {
    top_field_is_left_view_flag         = u_1("SEI: top_field_is_left_view_flag", buf);
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
    printf("top_field_is_left_view_flag = %d\n", top_field_is_left_view_flag);
#endif
  }
  else
  {
    current_frame_is_left_view_flag     = u_1("SEI: current_frame_is_left_view_flag", buf);
    next_frame_is_second_view_flag      = u_1("SEI: next_frame_is_second_view_flag", buf);
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
    printf("current_frame_is_left_view_flag = %d\n", current_frame_is_left_view_flag);
    printf("next_frame_is_second_view_flag = %d\n", next_frame_is_second_view_flag);
#endif
  }

  left_view_self_contained_flag         = u_1("SEI: left_view_self_contained_flag", buf);
  right_view_self_contained_flag        = u_1("SEI: right_view_self_contained_flag", buf);
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
  printf("left_view_self_contained_flag = %d\n", left_view_self_contained_flag);
  printf("right_view_self_contained_flag = %d\n", right_view_self_contained_flag);
#endif

  free (buf);
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
#undef PRINT_STEREO_VIDEO_INFO_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Reserved SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_reserved_info( byte* payload, int size, ImageParameters *img )
{
  int offset = 0;
  byte payload_byte;

#ifdef PRINT_RESERVED_INFO
  printf("Reserved SEI message\n");
#endif

  while (offset < size)
  {
    payload_byte = payload[offset];
    offset ++;
#ifdef PRINT_RESERVED_INFO
    printf("reserved_sei_message_payload_byte = %d\n", payload_byte);
#endif
  }
#ifdef PRINT_RESERVED_INFO
#undef PRINT_RESERVED_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Buffering period SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_buffering_period_info( byte* payload, int size, ImageParameters *img )
{
  int seq_parameter_set_id, initial_cpb_removal_delay, initial_cpb_removal_delay_offset;
  unsigned int k;

  Bitstream* buf;
  seq_parameter_set_rbsp_t *sps;


  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;

  seq_parameter_set_id   = ue_v("SEI: seq_parameter_set_id"  , buf);

  sps = &SeqParSet[seq_parameter_set_id];

  activate_sps(sps);

#ifdef PRINT_BUFFERING_PERIOD_INFO
  printf("Buffering period SEI message\n");
  printf("seq_parameter_set_id   = %d\n", seq_parameter_set_id);
#endif

  // Note: NalHrdBpPresentFlag and CpbDpbDelaysPresentFlag can also be set "by some means not specified in this Recommendation | International Standard"
  if (sps->vui_parameters_present_flag)
  {

    if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
    {
      for (k=0; k<sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1+1; k++)
      {
        initial_cpb_removal_delay        = u_v(sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, "SEI: initial_cpb_removal_delay"        , buf);
        initial_cpb_removal_delay_offset = u_v(sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, "SEI: initial_cpb_removal_delay_offset" , buf);

#ifdef PRINT_BUFFERING_PERIOD_INFO
        printf("nal initial_cpb_removal_delay[%d]        = %d\n", k, initial_cpb_removal_delay);
        printf("nal initial_cpb_removal_delay_offset[%d] = %d\n", k, initial_cpb_removal_delay_offset);
#endif
      }
    }

    if (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
    {
      for (k=0; k<sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1+1; k++)
      {
        initial_cpb_removal_delay        = u_v(sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, "SEI: initial_cpb_removal_delay"        , buf);
        initial_cpb_removal_delay_offset = u_v(sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, "SEI: initial_cpb_removal_delay_offset" , buf);

#ifdef PRINT_BUFFERING_PERIOD_INFO
        printf("vcl initial_cpb_removal_delay[%d]        = %d\n", k, initial_cpb_removal_delay);
        printf("vcl initial_cpb_removal_delay_offset[%d] = %d\n", k, initial_cpb_removal_delay_offset);
#endif
      }
    }
  }

  free (buf);
#ifdef PRINT_BUFFERING_PERIOD_INFO
#undef PRINT_BUFFERING_PERIOD_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Picture timing SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *
 ************************************************************************
 */
void interpret_picture_timing_info( byte* payload, int size, ImageParameters *img )
{
  int cpb_removal_delay, dpb_output_delay, picture_structure_present_flag, picture_structure;
  int clock_time_stamp_flag;
  int ct_type, nuit_field_based_flag, counting_type, full_timestamp_flag, discontinuity_flag, cnt_dropped_flag, nframes;
  int seconds_value, minutes_value, hours_value, seconds_flag, minutes_flag, hours_flag, time_offset;
  int NumClockTs = 0;
  int i;

  int cpb_removal_len = 24;
  int dpb_output_len  = 24;

  Boolean CpbDpbDelaysPresentFlag;

  Bitstream* buf;

  if (NULL==active_sps)
  {
    fprintf (stderr, "Warning: no active SPS, timing SEI cannot be parsed\n");
    return;
  }

  buf = malloc(sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;

  UsedBits = 0;


#ifdef PRINT_PCITURE_TIMING_INFO
  printf("Picture timing SEI message\n");
#endif

  // CpbDpbDelaysPresentFlag can also be set "by some means not specified in this Recommendation | International Standard"
  CpbDpbDelaysPresentFlag =  (Boolean) (active_sps->vui_parameters_present_flag
                              && (   (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag != 0)
                                   ||(active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag != 0)));

  if (CpbDpbDelaysPresentFlag )
  {
    if (active_sps->vui_parameters_present_flag)
    {
      if (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
      else if (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
    }

    if ((active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)||
      (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag))
    {
      cpb_removal_delay = u_v(cpb_removal_len, "SEI: cpb_removal_delay" , buf);
      dpb_output_delay  = u_v(dpb_output_len,  "SEI: dpb_output_delay"  , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
      printf("cpb_removal_delay = %d\n",cpb_removal_delay);
      printf("dpb_output_delay  = %d\n",dpb_output_delay);
#endif
    }
  }

  if (!active_sps->vui_parameters_present_flag)
  {
    picture_structure_present_flag = 0;
  }
  else
  {
    picture_structure_present_flag  =  active_sps->vui_seq_parameters.pic_struct_present_flag;
  }

  if (picture_structure_present_flag)
  {
    picture_structure = u_v(4, "SEI: pic_struct" , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
    printf("picture_structure = %d\n",picture_structure);
#endif
    switch (picture_structure)
    {
    case 0:
    case 1:
    case 2:
      NumClockTs = 1;
      break;
    case 3:
    case 4:
    case 7:
      NumClockTs = 2;
      break;
    case 5:
    case 6:
    case 8:
      NumClockTs = 3;
      break;
    default:
      error("reserved picture_structure used (can't determine NumClockTs)", 500);
    }
    for (i=0; i<NumClockTs; i++)
    {
      clock_time_stamp_flag = u_1("SEI: clock_time_stamp_flag"  , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
      printf("clock_time_stamp_flag = %d\n",clock_time_stamp_flag);
#endif
      if (clock_time_stamp_flag)
      {
        ct_type               = u_v(2, "SEI: ct_type"               , buf);
        nuit_field_based_flag = u_1(   "SEI: nuit_field_based_flag" , buf);
        counting_type         = u_v(5, "SEI: counting_type"         , buf);
        full_timestamp_flag   = u_1(   "SEI: full_timestamp_flag"   , buf);
        discontinuity_flag    = u_1(   "SEI: discontinuity_flag"    , buf);
        cnt_dropped_flag      = u_1(   "SEI: cnt_dropped_flag"      , buf);
        nframes               = u_v(8, "SEI: nframes"               , buf);

#ifdef PRINT_PCITURE_TIMING_INFO
        printf("ct_type               = %d\n",ct_type);
        printf("nuit_field_based_flag = %d\n",nuit_field_based_flag);
        printf("full_timestamp_flag   = %d\n",full_timestamp_flag);
        printf("discontinuity_flag    = %d\n",discontinuity_flag);
        printf("cnt_dropped_flag      = %d\n",cnt_dropped_flag);
        printf("nframes               = %d\n",nframes);
#endif
        if (full_timestamp_flag)
        {
          seconds_value         = u_v(6, "SEI: seconds_value"   , buf);
          minutes_value         = u_v(6, "SEI: minutes_value"   , buf);
          hours_value           = u_v(5, "SEI: hours_value"     , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("seconds_value = %d\n",seconds_value);
          printf("minutes_value = %d\n",minutes_value);
          printf("hours_value   = %d\n",hours_value);
#endif
        }
        else
        {
          seconds_flag          = u_1(   "SEI: seconds_flag" , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("seconds_flag = %d\n",seconds_flag);
#endif
          if (seconds_flag)
          {
            seconds_value         = u_v(6, "SEI: seconds_value"   , buf);
            minutes_flag          = u_1(   "SEI: minutes_flag" , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
            printf("seconds_value = %d\n",seconds_value);
            printf("minutes_flag  = %d\n",minutes_flag);
#endif
            if(minutes_flag)
            {
              minutes_value         = u_v(6, "SEI: minutes_value"   , buf);
              hours_flag            = u_1(   "SEI: hours_flag" , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
              printf("minutes_value = %d\n",minutes_value);
              printf("hours_flag    = %d\n",hours_flag);
#endif
              if(hours_flag)
              {
                hours_value           = u_v(5, "SEI: hours_value"     , buf);
#ifdef PRINT_PCITURE_TIMING_INFO
                printf("hours_value   = %d\n",hours_value);
#endif
              }
            }
          }
        }
        {
          int time_offset_length;
          if (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
            time_offset_length = active_sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length;
          else if (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
            time_offset_length = active_sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length;
          else
            time_offset_length = 24;
          if (time_offset_length)
            time_offset = u_v(time_offset_length, "SEI: time_offset"   , buf); // TODO interpretation is unsigned, need signed interpretation (i_v)
          else
            time_offset = 0;
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("time_offset   = %d\n",time_offset);
#endif
        }
      }
    }
  }

  free (buf);
#ifdef PRINT_PCITURE_TIMING_INFO
#undef PRINT_PCITURE_TIMING_INFO
#endif
}
