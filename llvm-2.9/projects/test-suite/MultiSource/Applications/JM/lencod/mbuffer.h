
/*!
 ***********************************************************************
 *  \file
 *      mbuffer.h
 *
 *  \brief
 *      Frame buffer functions
 *
 *  \author
 *      Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Sühring          <suehring@hhi.de>
 ***********************************************************************
 */
#ifndef _MBUFFER_H_
#define _MBUFFER_H_

#define MAX_LIST_SIZE 33

//! definition a picture (field or frame)
typedef struct storable_picture
{
  PictureStructure structure;

  int         poc;
  int         top_poc;
  int         bottom_poc;
  int         frame_poc;
  int         order_num;
  int64       ref_pic_num[6][MAX_LIST_SIZE];
  int64       frm_ref_pic_num[6][MAX_LIST_SIZE];
  int64       top_ref_pic_num[6][MAX_LIST_SIZE];
  int64       bottom_ref_pic_num[6][MAX_LIST_SIZE];
  unsigned    frame_num;
  int         pic_num;
  int         long_term_pic_num;
  int         long_term_frame_idx;

  int         is_long_term;
  int         used_for_reference;
  int         is_output;
  int         non_existing;

  int         size_x, size_y, size_x_cr, size_y_cr;
  int         size_x_pad, size_y_pad;
  int         size_x_cr_pad, size_y_cr_pad;
  int         chroma_vector_adjustment;
  int         coded_frame;
  int         MbaffFrameFlag;

  imgpel **   imgY;          //!< Y picture component
  imgpel ****   imgY_sub;      //!< Y picture component upsampled (Quarter pel)
  imgpel ****   imgY_sub_w;    //!< Y picture component upsampled (Quarter pel) for weighted prediction
  imgpel *****  imgUV_sub;      //!< UV picture component upsampled (Quarter/One-Eighth pel)
  imgpel ***  imgUV;         //!< U and V picture components

  byte *      mb_field;      //!< field macroblock indicator

  signed char *** ref_idx;  //!< reference picture   [list][subblock_y][subblock_x]

  int64 ***   ref_pic_id;    //!< reference picture identifier [list][subblock_y][subblock_x]
                             //   (not  simply index)

  int64 ***   ref_id;        //!< reference picture identifier [list][subblock_y][subblock_x]
                             //   (not  simply index)

  short ****  mv;            //!< motion vector       [list][subblock_x][subblock_y][component]

  byte **     moving_block;
  byte **     field_frame;         //!< indicates if co_located is field or frame.

  struct storable_picture *top_field;     // for mb aff, if frame for referencing the top field
  struct storable_picture *bottom_field;  // for mb aff, if frame for referencing the bottom field
  struct storable_picture *frame;         // for mb aff, if field for referencing the combined frame

  int         chroma_format_idc;
  int         frame_mbs_only_flag;
  int         frame_cropping_flag;
  int         frame_cropping_rect_left_offset;
  int         frame_cropping_rect_right_offset;
  int         frame_cropping_rect_top_offset;
  int         frame_cropping_rect_bottom_offset;
} StorablePicture;


//! definition a picture (field or frame)
typedef struct colocated_params
{
  int         mb_adaptive_frame_field_flag;
  int         size_x, size_y;

  int64       ref_pic_num[6][MAX_LIST_SIZE];

  signed char *** ref_idx;   //!< reference picture   [list][subblock_y][subblock_x]
  int64 ***   ref_pic_id;    //!< reference picture identifier [list][subblock_y][subblock_x]
  short ****  mv;            //!< motion vector       [list][subblock_x][subblock_y][component]
  byte **     moving_block;

  // Top field params
  int64       top_ref_pic_num[6][MAX_LIST_SIZE];
  signed char *** top_ref_idx;       //!< reference picture   [list][subblock_y][subblock_x]
  int64 ***   top_ref_pic_id;    //!< reference picture identifier [list][subblock_y][subblock_x]
  short ****  top_mv;            //!< motion vector       [list][subblock_x][subblock_y][component]
  byte **     top_moving_block;

  // Bottom field params
  int64       bottom_ref_pic_num[6][MAX_LIST_SIZE];
  signed char *** bottom_ref_idx;       //!< reference picture   [list][subblock_y][subblock_x]
  int64 ***   bottom_ref_pic_id;    //!< reference picture identifier [list][subblock_y][subblock_x]
  short ****  bottom_mv;            //!< motion vector       [list][subblock_x][subblock_y][component]
  byte **     bottom_moving_block;

  byte        is_long_term;
  byte **     field_frame;         //!< indicates if co_located is field or frame.

} ColocatedParams;

//! Frame Stores for Decoded Picture Buffer
typedef struct frame_store
{
  int       is_used;                //!< 0=empty; 1=top; 2=bottom; 3=both fields (or frame)
  int       is_reference;           //!< 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used
  int       is_long_term;           //!< 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used
  int       is_orig_reference;      //!< original marking by nal_ref_idc: 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used

  int       is_non_existent;

  unsigned  frame_num;
  int       frame_num_wrap;
  int       long_term_frame_idx;
  int       is_output;
  int       poc;

  StorablePicture *frame;
  StorablePicture *top_field;
  StorablePicture *bottom_field;

} FrameStore;


//! Decoded Picture Buffer
typedef struct decoded_picture_buffer
{
  FrameStore  **fs;
  FrameStore  **fs_ref;
  FrameStore  **fs_ltref;
  unsigned      size;
  unsigned      used_size;
  unsigned      ref_frames_in_buffer;
  unsigned      ltref_frames_in_buffer;
  int           last_output_poc;
  int           max_long_term_pic_idx;

  int           init_done;

  FrameStore   *last_picture;
} DecodedPictureBuffer;


extern DecodedPictureBuffer dpb;
extern StorablePicture **listX[6];
extern int listXsize[6];

void             init_dpb(void);
void             free_dpb(void);
FrameStore*      alloc_frame_store(void);
void             free_frame_store(FrameStore* f);
StorablePicture* alloc_storable_picture(PictureStructure type, int size_x, int size_y, int size_x_cr, int size_y_cr);
void             free_storable_picture(StorablePicture* p);
void             store_picture_in_dpb(StorablePicture* p);
void             replace_top_pic_with_frame(StorablePicture* p);
void             flush_dpb(void);

void             dpb_split_field(FrameStore *fs);
void             dpb_combine_field(FrameStore *fs);
void             dpb_combine_field_yuv(FrameStore *fs);

void             init_lists(int currSliceType, PictureStructure currPicStructure);
void             reorder_ref_pic_list(StorablePicture **list, int *list_size,
                                      int num_ref_idx_lX_active_minus1, int *reordering_of_pic_nums_idc,
                                      int *abs_diff_pic_num_minus1, int *long_term_pic_idx);

void             init_mbaff_lists(void);
void             alloc_ref_pic_list_reordering_buffer(Slice *currSlice);
void             free_ref_pic_list_reordering_buffer(Slice *currSlice);

void             fill_frame_num_gap(ImageParameters *img);

ColocatedParams* alloc_colocated(int size_x, int size_y,int mb_adaptive_frame_field_flag);
void free_colocated(ColocatedParams* p);
void compute_colocated(ColocatedParams* p, StorablePicture **listX[6]);

#endif

