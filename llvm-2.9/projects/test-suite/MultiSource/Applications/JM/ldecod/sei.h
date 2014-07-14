
/*!
 *************************************************************************************
 * \file sei.h
 *
 * \brief
 *    Prototypes for sei.c
 *************************************************************************************
 */

#ifndef SEI_H
#define SEI_H

typedef enum {
  SEI_BUFFERING_PERIOD = 0,
  SEI_PIC_TIMING,
  SEI_PAN_SCAN_RECT,
  SEI_FILLER_PAYLOAD,
  SEI_USER_DATA_REGISTERED_ITU_T_T35,
  SEI_USER_DATA_UNREGISTERED,
  SEI_RECOVERY_POINT,
  SEI_DEC_REF_PIC_MARKING_REPETITION,
  SEI_SPARE_PIC,
  SEI_SCENE_INFO,
  SEI_SUB_SEQ_INFO,
  SEI_SUB_SEQ_LAYER_CHARACTERISTICS,
  SEI_SUB_SEQ_CHARACTERISTICS,
  SEI_FULL_FRAME_FREEZE,
  SEI_FULL_FRAME_FREEZE_RELEASE,
  SEI_FULL_FRAME_SNAPSHOT,
  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,
  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,
  SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,
  SEI_FILM_GRAIN_CHARACTERISTICS,
  SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE,
  SEI_STEREO_VIDEO_INFO,

  SEI_MAX_ELEMENTS  //!< number of maximum syntax elements
} SEI_type;

#define MAX_FN 256

void InterpretSEIMessage(byte* msg, int size, ImageParameters *img);
void interpret_spare_pic( byte* payload, int size, ImageParameters *img );
void interpret_subsequence_info( byte* payload, int size, ImageParameters *img );
void interpret_subsequence_layer_characteristics_info( byte* payload, int size, ImageParameters *img );
void interpret_subsequence_characteristics_info( byte* payload, int size, ImageParameters *img );
void interpret_scene_information( byte* payload, int size, ImageParameters *img ); // JVT-D099
void interpret_user_data_registered_itu_t_t35_info( byte* payload, int size, ImageParameters *img );
void interpret_user_data_unregistered_info( byte* payload, int size, ImageParameters *img );
void interpret_pan_scan_rect_info( byte* payload, int size, ImageParameters *img );
void interpret_recovery_point_info( byte* payload, int size, ImageParameters *img );
void interpret_filler_payload_info( byte* payload, int size, ImageParameters *img );
void interpret_dec_ref_pic_marking_repetition_info( byte* payload, int size, ImageParameters *img );
void interpret_full_frame_freeze_info( byte* payload, int size, ImageParameters *img );
void interpret_full_frame_freeze_release_info( byte* payload, int size, ImageParameters *img );
void interpret_full_frame_snapshot_info( byte* payload, int size, ImageParameters *img );
void interpret_progressive_refinement_start_info( byte* payload, int size, ImageParameters *img );
void interpret_progressive_refinement_end_info( byte* payload, int size, ImageParameters *img );
void interpret_motion_constrained_slice_group_set_info( byte* payload, int size, ImageParameters *img );
void interpret_reserved_info( byte* payload, int size, ImageParameters *img );
void interpret_buffering_period_info( byte* payload, int size, ImageParameters *img );
void interpret_picture_timing_info( byte* payload, int size, ImageParameters *img );
void interpret_film_grain_characteristics_info( byte* payload, int size, ImageParameters *img );
void interpret_deblocking_filter_display_preference_info( byte* payload, int size, ImageParameters *img );
void interpret_stereo_video_info_info( byte* payload, int size, ImageParameters *img );

#endif
