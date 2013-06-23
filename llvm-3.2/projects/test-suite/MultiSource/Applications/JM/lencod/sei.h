
/*!
 ************************************************************************
 *  \file
 *     sei.h
 *  \brief
 *     definitions for Supplemental Enhanced Information
 *  \author(s)
 *      - Dong Tian                             <tian@cs.tut.fi>
 *      - TBD
 *
 * ************************************************************************
 */

#ifndef SEI_H
#define SEI_H

#define MAX_LAYER_NUMBER 2
#define MAX_DEPENDENT_SUBSEQ 5


//! definition of SEI payload type
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

#define AGGREGATION_PACKET_TYPE 4
#define SEI_PACKET_TYPE 5  // Tian Dong: See VCEG-N72, it need updates

#define NORMAL_SEI 0
#define AGGREGATION_SEI 1

//! SEI structure
typedef struct
{
  Boolean available;
  int payloadSize;
  unsigned char subPacketType;
  byte* data;
} sei_struct;

//!< sei_message[0]: this struct is to store the sei message packtized independently
//!< sei_message[1]: this struct is to store the sei message packtized together with slice data
extern sei_struct sei_message[2];

void InitSEIMessages(void);
void CloseSEIMessages(void);
Boolean HaveAggregationSEI(void);
void write_sei_message(int id, byte* payload, int payload_size, int payload_type);
void finalize_sei_message(int id);
void clear_sei_message(int id);
void AppendTmpbits2Buf( Bitstream* dest, Bitstream* source );

void PrepareAggregationSEIMessage(void);


//! Spare Picture
typedef struct
{
  int target_frame_num;
  int num_spare_pics;
  int payloadSize;
  Bitstream* data;
} spare_picture_struct;

extern Boolean seiHasSparePicture;
//extern Boolean sei_has_sp;
extern spare_picture_struct seiSparePicturePayload;

void InitSparePicture();
void CloseSparePicture();
void CalculateSparePicture();
void ComposeSparePictureMessage(int delta_spare_frame_num, int ref_area_indicator, Bitstream *tmpBitstream);
Boolean CompressSpareMBMap(unsigned char **map_sp, Bitstream *bitstream);
void FinalizeSpareMBMap();

//! Subseq Information
typedef struct
{
  int subseq_layer_num;
  int subseq_id;
  unsigned int last_picture_flag;
  unsigned int stored_frame_cnt;

  int payloadSize;
  Bitstream* data;
} subseq_information_struct;

extern Boolean seiHasSubseqInfo;
extern subseq_information_struct seiSubseqInfo[MAX_LAYER_NUMBER];

void InitSubseqInfo(int currLayer);
void UpdateSubseqInfo(int currLayer);
void FinalizeSubseqInfo(int currLayer);
void ClearSubseqInfoPayload(int currLayer);
void CloseSubseqInfo(int currLayer);

//! Subseq Layer Information
typedef struct
{
  unsigned short bit_rate[MAX_LAYER_NUMBER];
  unsigned short frame_rate[MAX_LAYER_NUMBER];
  byte data[4*MAX_LAYER_NUMBER];
  int layer_number;
  int payloadSize;
} subseq_layer_information_struct;

extern Boolean seiHasSubseqLayerInfo;
extern subseq_layer_information_struct seiSubseqLayerInfo;

void InitSubseqLayerInfo();
void CloseSubseqLayerInfo();
void FinalizeSubseqLayerInfo();

//! Subseq Characteristics
typedef struct
{
  int subseq_layer_num;
  int subseq_id;
  int duration_flag;
  unsigned int subseq_duration;
  unsigned int average_rate_flag;
  unsigned int average_bit_rate;
  unsigned int average_frame_rate;
  int num_referenced_subseqs;
  int ref_subseq_layer_num[MAX_DEPENDENT_SUBSEQ];
  int ref_subseq_id[MAX_DEPENDENT_SUBSEQ];

  Bitstream* data;
  int payloadSize;
} subseq_char_information_struct;

extern Boolean seiHasSubseqChar;
extern subseq_char_information_struct seiSubseqChar;

void InitSubseqChar();
void ClearSubseqCharPayload();
void UpdateSubseqChar();
void FinalizeSubseqChar();
void CloseSubseqChar();


typedef struct
{
  int scene_id;
  int scene_transition_type;
  int second_scene_id;

  Bitstream* data;
  int payloadSize;
} scene_information_struct;

extern Boolean seiHasSceneInformation;
extern scene_information_struct seiSceneInformation;

void InitSceneInformation();
void CloseSceneInformation();
void UpdateSceneInformation(Boolean HasSceneInformation, int sceneID, int sceneTransType, int secondSceneID);
void FinalizeSceneInformation();

//! PanScanRect Information
typedef struct
{
  int pan_scan_rect_id;
  int pan_scan_rect_left_offset;
  int pan_scan_rect_right_offset;
  int pan_scan_rect_top_offset;
  int pan_scan_rect_bottom_offset;

  Bitstream *data;
  int payloadSize;
} panscanrect_information_struct;

extern Boolean seiHasPanScanRectInfo;
extern panscanrect_information_struct seiPanScanRectInfo;

void InitPanScanRectInfo();
void ClearPanScanRectInfoPayload();
void UpdatePanScanRectInfo();
void FinalizePanScanRectInfo();
void ClosePanScanRectInfo();

//! User_data_unregistered Information
typedef struct
{
  signed char *byte;
  int total_byte;
  Bitstream *data;
  int payloadSize;
} user_data_unregistered_information_struct;

extern Boolean seiHasUser_data_unregistered_info;
extern user_data_unregistered_information_struct seiUser_data_unregistered;

void InitUser_data_unregistered();
void ClearUser_data_unregistered();
void UpdateUser_data_unregistered();
void FinalizeUser_data_unregistered();
void CloseUser_data_unregistered();

//! User_data_registered_itu_t_t35 Information
typedef struct
{
  signed char *byte;
  int total_byte;
  int itu_t_t35_country_code;
  int itu_t_t35_country_code_extension_byte;
  Bitstream *data;
  int payloadSize;
} user_data_registered_itu_t_t35_information_struct;

extern Boolean seiHasUser_data_registered_itu_t_t35_info;
extern user_data_registered_itu_t_t35_information_struct seiUser_data_registered_itu_t_t35;

void InitUser_data_registered_itu_t_t35();
void ClearUser_data_registered_itu_t_t35();
void UpdateUser_data_registered_itu_t_t35();
void FinalizeUser_data_registered_itu_t_t35();
void CloseUser_data_registered_itu_t_t35();

//! Recovery Point Information
typedef struct
{
  unsigned int  recovery_frame_cnt;
  unsigned char exact_match_flag;
  unsigned char broken_link_flag;
  unsigned char changing_slice_group_idc;

  Bitstream *data;
  int payloadSize;
} recovery_point_information_struct;

extern Boolean seiHasRecoveryPoint_info;
extern recovery_point_information_struct seiRecoveryPoint;

void InitRandomAccess();
void ClearRandomAccess();
void UpdateRandomAccess();
void FinalizeRandomAccess();
void CloseRandomAccess();


// This is only temp
//! Buffering Period Information
#define MAX_CPB_CNT_MINUS1 31
#define MAX_PIC_STRUCT_VALUE 16
typedef struct
{
  int seq_parameter_set_id;
  int initial_cpb_removal_delay[MAX_CPB_CNT_MINUS1+1];
  int initial_cpb_removal_delay_offset[MAX_CPB_CNT_MINUS1+1];

  Bitstream *data;
  int payloadSize;
} bufferingperiod_information_struct;
Boolean seiHasBufferingPeriod_info;
bufferingperiod_information_struct seiBufferingPeriod;

void InitBufferingPeriod();
void ClearBufferingPeriod();
void CloseBufferingPeriod();
void UpdateBufferingPeriod();
void FinalizeBufferingPeriod();

//! Picture timing Information
typedef struct
{
  int cpb_removal_delay;
  int dpb_output_delay;
  int pic_struct;
  Boolean clock_timestamp_flag[MAX_PIC_STRUCT_VALUE];
  int ct_type;
  Boolean nuit_field_based_flag;
  int counting_type;
  Boolean full_timestamp_flag;
  Boolean discontinuity_flag;
  Boolean cnt_dropped_flag;
  int n_frames;
  int seconds_value;
  int minutes_value;
  int hours_value;
  Boolean seconds_flag;
  Boolean minutes_flag;
  Boolean hours_flag;
  int time_offset;

  Bitstream *data;
  int payloadSize;
} pictiming_information_struct;
Boolean seiHasPicTiming_info;
pictiming_information_struct seiPicTiming;

void InitPicTiming();
void ClearPicTiming();
void ClosePicTiming();
void UpdatePicTiming();
void FinalizePicTiming();

// end of temp additions
#endif
