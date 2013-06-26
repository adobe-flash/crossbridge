
/*!
 ************************************************************************
 *  \file
 *     global.h
 *
 *  \brief
 *     global definitions for H.264 encoder.
 *
 *  \author
 *     Copyright (C) 1999  Telenor Satellite Services,Norway
 *                         Ericsson Radio Systems, Sweden
 *
 *     Inge Lille-Langoy               <inge.lille-langoy@telenor.com>
 *
 *     Telenor Satellite Services
 *     Keysers gt.13                       tel.:   +47 23 13 86 98
 *     N-0130 Oslo,Norway                  fax.:   +47 22 77 79 80
 *
 *     Rickard Sjoberg                 <rickard.sjoberg@era.ericsson.se>
 *
 *     Ericsson Radio Systems
 *     KI/ERA/T/VV
 *     164 80 Stockholm, Sweden
 *
 ************************************************************************
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>

#include "win32.h"
#include "defines.h"
#include "parsetcommon.h"
#include "q_matrix.h"
#include "q_offsets.h"
#include "minmax.h"
#include "ifunctions.h"

/***********************************************************************
 * T y p e    d e f i n i t i o n s    f o r    T M L
 ***********************************************************************
 */


//typedef byte imgpel;
//typedef unsigned short distpel;
typedef unsigned short imgpel;
typedef int distpel;

enum {
  YUV400 = 0,
  YUV420 = 1,
  YUV422 = 2,
  YUV444 = 3
} color_formats;

enum {
  LIST_0 = 0,
  LIST_1 = 1,
  BI_PRED = 2,
  BI_PRED_L0 = 3,
  BI_PRED_L1 = 4
};

enum {
  ERROR_SAD = 0,
  ERROR_SSE = 1,
  ERROR_SATD = 2,
  ERROR_PSATD = 3
};

enum {
  ME_Y_ONLY = 0,
  ME_YUV_FP = 1,
  ME_YUV_FP_SP = 2
};

enum {
  DISTORTION_MSE = 0,
  DISTORTION_WEIGHTED_MSE = 1,
  DISTORTION_OVERLAP_TRANSFORM = 2,
  DISTORTION_EDGE_MSE = 3
};

//! Data Partitioning Modes
typedef enum
{
  PAR_DP_1,   //!< no data partitioning is supported
  PAR_DP_3    //!< data partitioning with 3 partitions
} PAR_DP_TYPE;


//! Output File Types
typedef enum
{
  PAR_OF_ANNEXB,    //!< Annex B byte stream format
  PAR_OF_RTP       //!< RTP packets in outfile
} PAR_OF_TYPE;

//! Field Coding Types
typedef enum
{
  FRAME_CODING,
  FIELD_CODING,
  ADAPTIVE_CODING,
  FRAME_MB_PAIR_CODING
} CodingType;

//! definition of H.264 syntax elements
typedef enum
{
  SE_HEADER,
  SE_PTYPE,
  SE_MBTYPE,
  SE_REFFRAME,
  SE_INTRAPREDMODE,
  SE_MVD,
  SE_CBP,
  SE_LUM_DC_INTRA,
  SE_CHR_DC_INTRA,
  SE_LUM_AC_INTRA,
  SE_CHR_AC_INTRA,
  SE_LUM_DC_INTER,
  SE_CHR_DC_INTER,
  SE_LUM_AC_INTER,
  SE_CHR_AC_INTER,
  SE_DELTA_QUANT,
  SE_BFRAME,
  SE_EOS,
  SE_MAX_ELEMENTS  //!< number of maximum syntax elements
} SE_type;         // substituting the definitions in elements.h


typedef enum
{
  INTER_MB,
  INTRA_MB_4x4,
  INTRA_MB_16x16
} IntraInterDecision;


typedef enum
{
  BITS_HEADER,
  BITS_TOTAL_MB,
  BITS_MB_MODE,
  BITS_INTER_MB,
  BITS_CBP_MB,
  BITS_COEFF_Y_MB,
  BITS_COEFF_UV_MB,
  BITS_DELTA_QUANT_MB,
  MAX_BITCOUNTER_MB
} BitCountType;


typedef enum
{
  NO_SLICES,
  FIXED_MB,
  FIXED_RATE,
  CALLBACK,
  FMO
} SliceMode;


typedef enum
{
  UVLC,
  CABAC
} SymbolMode;

typedef enum
{
  FULL_SEARCH      = -1,
  FAST_FULL_SEARCH =  0,
  UM_HEX           =  1,
  UM_HEX_SIMPLE    =  2,
  EPZS             =  3
} SearchType;


typedef enum
{
  FRAME,
  TOP_FIELD,
  BOTTOM_FIELD
} PictureStructure;           //!< New enum for field processing

typedef enum
{
  P_SLICE = 0,
  B_SLICE = 1,
  I_SLICE = 2,
  SP_SLICE = 3,
  SI_SLICE = 4
} SliceType;

//Motion Estimation levels
typedef enum
{
  F_PEL,   //!< Full Pel refinement
  H_PEL,   //!< Half Pel refinement
  Q_PEL    //!< Quarter Pel refinement
} MELevel;

typedef enum
{
  FAST_ACCESS = 0,    //!< Fast/safe reference access
  UMV_ACCESS = 1      //!< unconstrained reference access
} REF_ACCESS_TYPE;

typedef enum
{
  IS_LUMA = 0,
  IS_CHROMA = 1
} Component_Type;

typedef enum
{
  RC_MODE_0 = 0,
  RC_MODE_1 = 1,
  RC_MODE_2 = 2,
  RC_MODE_3 = 3
} RCModeType;

/***********************************************************************
 * D a t a    t y p e s   f o r  C A B A C
 ***********************************************************************
 */

//! struct to characterize the state of the arithmetic coding engine
typedef struct
{
  unsigned int  Elow, Erange;
  unsigned int  Ebuffer;
  unsigned int  Ebits_to_go;
  unsigned int  Ebits_to_follow;
  byte          *Ecodestrm;
  int           *Ecodestrm_len;
  int           C;
  int           E;

} EncodingEnvironment;

typedef EncodingEnvironment *EncodingEnvironmentPtr;

//! struct for context management
typedef struct
{
  unsigned short state;         // index into state-table CP
  unsigned char  MPS;           // Least Probable Symbol 0/1 CP
  unsigned long  count;
} BiContextType;

typedef BiContextType *BiContextTypePtr;


/**********************************************************************
 * C O N T E X T S   F O R   T M L   S Y N T A X   E L E M E N T S
 **********************************************************************
 */


#define NUM_MB_TYPE_CTX  11
#define NUM_B8_TYPE_CTX  9
#define NUM_MV_RES_CTX   10
#define NUM_REF_NO_CTX   6
#define NUM_DELTA_QP_CTX 4
#define NUM_MB_AFF_CTX 4

#define NUM_TRANSFORM_SIZE_CTX 3

typedef struct
{
  BiContextType mb_type_contexts [3][NUM_MB_TYPE_CTX];
  BiContextType b8_type_contexts [2][NUM_B8_TYPE_CTX];
  BiContextType mv_res_contexts  [2][NUM_MV_RES_CTX];
  BiContextType ref_no_contexts  [2][NUM_REF_NO_CTX];
  BiContextType delta_qp_contexts   [NUM_DELTA_QP_CTX];
  BiContextType mb_aff_contexts     [NUM_MB_AFF_CTX];
  BiContextType transform_size_contexts   [NUM_TRANSFORM_SIZE_CTX];
} MotionInfoContexts;


#define NUM_IPR_CTX    2
#define NUM_CIPR_CTX   4
#define NUM_CBP_CTX    4
#define NUM_BCBP_CTX   4
#define NUM_MAP_CTX   15
#define NUM_LAST_CTX  15
#define NUM_ONE_CTX    5
#define NUM_ABS_CTX    5


typedef struct
{
  BiContextType  ipr_contexts [NUM_IPR_CTX];
  BiContextType  cipr_contexts[NUM_CIPR_CTX];
  BiContextType  cbp_contexts [3][NUM_CBP_CTX];
  BiContextType  bcbp_contexts[NUM_BLOCK_TYPES][NUM_BCBP_CTX];
  BiContextType  map_contexts [NUM_BLOCK_TYPES][NUM_MAP_CTX];
  BiContextType  last_contexts[NUM_BLOCK_TYPES][NUM_LAST_CTX];
  BiContextType  one_contexts [NUM_BLOCK_TYPES][NUM_ONE_CTX];
  BiContextType  abs_contexts [NUM_BLOCK_TYPES][NUM_ABS_CTX];
  BiContextType  fld_map_contexts [NUM_BLOCK_TYPES][NUM_MAP_CTX];
  BiContextType  fld_last_contexts[NUM_BLOCK_TYPES][NUM_LAST_CTX];
} TextureInfoContexts;

//*********************** end of data type definition for CABAC *******************

//! Pixel position for checking neighbors
typedef struct pix_pos
{
  int available;
  int mb_addr;
  int x;
  int y;
  int pos_x;
  int pos_y;
} PixelPos;

//! Buffer structure for decoded reference picture marking commands
typedef struct DecRefPicMarking_s
{
  int memory_management_control_operation;
  int difference_of_pic_nums_minus1;
  int long_term_pic_num;
  int long_term_frame_idx;
  int max_long_term_frame_idx_plus1;
  struct DecRefPicMarking_s *Next;
} DecRefPicMarking_t;

//! Syntax Element
typedef struct syntaxelement
{
  int                 type;           //!< type of syntax element for data part.
  int                 value1;         //!< numerical value of syntax element
  int                 value2;         //!< for blocked symbols, e.g. run/level
  int                 len;            //!< length of code
  int                 inf;            //!< info part of UVLC code
  unsigned int        bitpattern;     //!< UVLC bitpattern
  int                 context;        //!< CABAC context

#if TRACE
  #define             TRACESTRING_SIZE 100            //!< size of trace string
  char                tracestring[TRACESTRING_SIZE];  //!< trace string
#endif

  //!< for mapping of syntaxElement to UVLC
  void    (*mapping)(int value1, int value2, int* len_ptr, int* info_ptr);

} SyntaxElement;

//! Macroblock
typedef struct macroblock
{
  int                 slice_nr;
  int                 delta_qp;
  int                 qp;                         //!< QP luma
  int                 qpc[2];                     //!< QP chroma
  int                 qpsp ;
  int                 bitcounter[MAX_BITCOUNTER_MB];

  struct macroblock   *mb_available_up;   //!< pointer to neighboring MB (CABAC)
  struct macroblock   *mb_available_left; //!< pointer to neighboring MB (CABAC)

  int                 mb_type;
  int                 mvd[2][BLOCK_MULTIPLE][BLOCK_MULTIPLE][2];          //!< indices correspond to [list][block_y][block_x][x,y]
  signed char         intra_pred_modes[MB_BLOCK_PARTITIONS];
  signed char         intra_pred_modes8x8[MB_BLOCK_PARTITIONS];           //!< four 8x8 blocks in a macroblock
  int                 cbp ;
  int64               cbp_blk ;    //!< 1 bit set for every 4x4 block with coefs (not implemented for INTRA)
  int                 b8mode[4];
  int                 b8pdir[4];
  int64               cbp_bits;

  int                 c_ipred_mode;      //!< chroma intra prediction mode
  int                 IntraChromaPredModeFlag;

  int                 mb_field;
  int                 is_field_mode;
  int                 list_offset;

  int                 mbAddrA, mbAddrB, mbAddrC, mbAddrD;
  int                 mbAvailA, mbAvailB, mbAvailC, mbAvailD;

  int                 all_blk_8x8;
  int                 luma_transform_size_8x8_flag;
  int                 NoMbPartLessThan8x8Flag;

  short               bi_pred_me;

  // rate control
  double              actj;               // macroblock activity measure for macroblock j
  int                 prev_qp;
  int                 prev_delta_qp;
  int                 prev_cbp;
  int                 predict_qp;
  int                 predict_error;

  int                 LFDisableIdc;
  int                 LFAlphaC0Offset;
  int                 LFBetaOffset;

  int                 skip_flag;
} Macroblock;



//! Bitstream
typedef struct
{
  int             byte_pos;           //!< current position in bitstream;
  int             bits_to_go;         //!< current bitcounter
  byte            byte_buf;           //!< current buffer for last written byte
  int             stored_byte_pos;    //!< storage for position in bitstream;
  int             stored_bits_to_go;  //!< storage for bitcounter
  byte            stored_byte_buf;    //!< storage for buffer of last written byte

  byte            byte_buf_skip;      //!< current buffer for last written byte
  int             byte_pos_skip;      //!< storage for position in bitstream;
  int             bits_to_go_skip;    //!< storage for bitcounter

  byte            *streamBuffer;      //!< actual buffer for written bytes
  int             write_flag;         //!< Bitstream contains data and needs to be written

#if TRACE
  Boolean             trace_enabled;
#endif

} Bitstream;

//! DataPartition
typedef struct datapartition
{

  Bitstream           *bitstream;
  EncodingEnvironment ee_cabac;
  EncodingEnvironment ee_recode;
} DataPartition;

//! Slice
typedef struct
{
  int                 picture_id;
  int                 qp;
  int                 picture_type; //!< picture type
  int                 start_mb_nr;
  int                 max_part_nr;  //!< number of different partitions
  int                 num_mb;       //!< number of MBs in the slice
  DataPartition       *partArr;     //!< array of partitions
  MotionInfoContexts  *mot_ctx;     //!< pointer to struct of context models for use in CABAC
  TextureInfoContexts *tex_ctx;     //!< pointer to struct of context models for use in CABAC

  int                 ref_pic_list_reordering_flag_l0;
  int                 *reordering_of_pic_nums_idc_l0;
  int                 *abs_diff_pic_num_minus1_l0;
  int                 *long_term_pic_idx_l0;
  int                 ref_pic_list_reordering_flag_l1;
  int                 *reordering_of_pic_nums_idc_l1;
  int                 *abs_diff_pic_num_minus1_l1;
  int                 *long_term_pic_idx_l1;

  Boolean             (*slice_too_big)(int bits_slice); //!< for use of callback functions

  int                 field_ctx[3][2]; //GB

} Slice;



#define MAXSLICEPERPICTURE 100
typedef struct
{
  int   no_slices;
  int   idr_flag;
  Slice *slices[MAXSLICEPERPICTURE];
  int bits_per_picture;
  float distortion_y;
  float distortion_u;
  float distortion_v;
} Picture;

Picture *top_pic;
Picture *bottom_pic;
Picture *frame_pic;
Picture *frame_pic_1;
Picture *frame_pic_2;
Picture *frame_pic_3;
Picture *frame_pic_si;

#ifdef _LEAKYBUCKET_
long *Bit_Buffer;
#endif

// global picture format dependend buffers, mem allocation in image.c
imgpel **imgY_org;           //!< Reference luma image
imgpel ***imgUV_org;         //!< Reference chroma image
int    **imgY_sub_tmp;       //!< Y picture temporary component (Quarter pel)

int **PicPos;
unsigned int log2_max_frame_num_minus4;
unsigned int log2_max_pic_order_cnt_lsb_minus4;

time_t  me_tot_time,me_time;
pic_parameter_set_rbsp_t *active_pps;
seq_parameter_set_rbsp_t *active_sps;


int dsr_new_search_range; //!<Dynamic Search Range.
//////////////////////////////////////////////////////////////////////////
// B pictures
// motion vector : forward, backward, direct
int  mb_adaptive;       //!< For MB level field/frame coding tools
int  MBPairIsField;     //!< For MB level field/frame coding tools


//Weighted prediction
int ***wp_weight;  // weight in [list][index][component] order
int ***wp_offset;  // offset in [list][index][component] order
int ****wbp_weight;  // weight in [list][fwd_index][bwd_idx][component] order
int luma_log_weight_denom;
int chroma_log_weight_denom;
int wp_luma_round;
int wp_chroma_round;

// global picture format dependend buffers, mem allocation in image.c (field picture)
imgpel   **imgY_org_top;
imgpel   **imgY_org_bot;

imgpel  ***imgUV_org_top;
imgpel  ***imgUV_org_bot;

imgpel   **imgY_org_frm;
imgpel  ***imgUV_org_frm;

imgpel   **imgY_com;               //!< Encoded luma images
imgpel  ***imgUV_com;              //!< Encoded croma images

signed char ***direct_ref_idx;     //!< direct mode reference index buffer
signed char **direct_pdir;         //!< direct mode reference index buffer

// Buffers for rd optimization with packet losses, Dim. Kontopodis
byte **pixel_map;   //!< Shows the latest reference frame that is reliable for each pixel
byte **refresh_map; //!< Stores the new values for pixel_map
int intras;         //!< Counts the intra updates in each frame.

int  frame_ctr[5];
int  frame_no, nextP_tr_fld, nextP_tr_frm;

time_t  tot_time;

#define ET_SIZE 300      //!< size of error text buffer
char errortext[ET_SIZE]; //!< buffer for error message for exit with error()

// Residue Color Transform
signed char b8_ipredmode8x8[4][4], b8_intra_pred_modes8x8[16];

//! Info for the "decoders-in-the-encoder" used for rdoptimization with packet losses
typedef struct
{
  int  **resY;               //!< Residue of Luminance
  imgpel ***decY;            //!< Decoded values at the simulated decoders
  imgpel ****decref;         //!< Reference frames of the simulated decoders
  imgpel ***decY_best;       //!< Decoded frames for the best mode for all decoders
  imgpel **RefBlock;
  byte **status_map;
  byte **dec_mb_mode;
} Decoders;
extern Decoders *decs;

//! SNRParameters
typedef struct
{
  float snr_y;               //!< current Y SNR
  float snr_u;               //!< current U SNR
  float snr_v;               //!< current V SNR
  float snr_y1;              //!< SNR Y(dB) first frame
  float snr_u1;              //!< SNR U(dB) first frame
  float snr_v1;              //!< SNR V(dB) first frame
  float snr_yt[5];           //!< SNR Y(dB) based on frame type
  float snr_ut[5];           //!< SNR U(dB) based on frame type
  float snr_vt[5];           //!< SNR V(dB) based on frame type
  float snr_ya;              //!< Average SNR Y(dB) remaining frames
  float snr_ua;              //!< Average SNR U(dB) remaining frames
  float snr_va;              //!< Average SNR V(dB) remaining frames
  float sse_y;               //!< SSE Y
  float sse_u;               //!< SSE U
  float sse_v;               //!< SSE V
  float msse_y;              //!< Average SSE Y
  float msse_u;              //!< Average SSE U
  float msse_v;              //!< Average SSE V
  int   frame_ctr;           //!< number of coded frames
} SNRParameters;

#define FILE_NAME_SIZE 256
                             //! all input parameters
typedef struct
{
  int ProfileIDC;                    //!< value of syntax element profile_idc
  int LevelIDC;                      //!< value of syntax element level_idc

  int no_frames;                     //!< number of frames to be encoded
  int qp0;                           //!< QP of first frame
  int qpN;                           //!< QP of remaining frames
  int jumpd;                         //!< number of frames to skip in input sequence (e.g 2 takes frame 0,3,6,9...)
  int DisableSubpelME;               //!< Disable sub-pixel motion estimation
  int search_range;                  /*!< search range - integer pel search and 16x16 blocks.  The search window is
                                          generally around the predicted vector. Max vector is 2xmcrange.  For 8x8
                                          and 4x4 block sizes the search range is 1/2 of that for 16x16 blocks.       */
  int num_ref_frames;                //!< number of reference frames to be used
  int P_List0_refs;                  //!< number of reference picture in list 0 in P pictures
  int B_List0_refs;                  //!< number of reference picture in list 0 in B pictures
  int B_List1_refs;                  //!< number of reference picture in list 1 in B pictures
  int Log2MaxFNumMinus4;             //!< value of syntax element log2_max_frame_num
  int Log2MaxPOCLsbMinus4;           //!< value of syntax element log2_max_pic_order_cnt_lsb_minus4

  int img_width;                     //!< image width  (must be a multiple of 16 pels)
  int img_height;                    //!< image height (must be a multiple of 16 pels)
  int yuv_format;                    //!< YUV format (0=4:0:0, 1=4:2:0, 2=4:2:2, 3=4:4:4)
  int intra_upd;                     /*!< For error robustness. 0: no special action. 1: One GOB/frame is intra coded
                                          as regular 'update'. 2: One GOB every 2 frames is intra coded etc.
                                          In connection with this intra update, restrictions is put on motion vectors
                                          to prevent errors to propagate from the past                                */
  int blc_size[8][2];                //!< array for different block sizes
  int part_size[8][2];               //!< array for different partition sizes
  int blocktype_lut[4][4];           //!< array for different partition sizes
  int slice_mode;                    //!< Indicate what algorithm to use for setting slices
  int slice_argument;                //!< Argument to the specified slice algorithm
  int UseConstrainedIntraPred;       //!< 0: Inter MB pixels are allowed for intra prediction 1: Not allowed
  int  infile_header;                //!< If input file has a header set this to the length of the header
  char infile[FILE_NAME_SIZE];       //!< YUV 4:2:0 input format
  char outfile[FILE_NAME_SIZE];      //!< H.264 compressed output bitstream
  char ReconFile[FILE_NAME_SIZE];    //!< Reconstructed Pictures
  char TraceFile[FILE_NAME_SIZE];    //!< Trace Outputs
  char QmatrixFile[FILE_NAME_SIZE];  //!< Q matrix cfg file
  int intra_period;                  //!< Random Access period though intra
  int EnableOpenGOP;                 //!< support for open gops.

  int idr_enable;                    //!< Encode intra slices as IDR
  int start_frame;                   //!< Encode sequence starting from Frame start_frame

  int GenerateMultiplePPS;
  int Generate_SEIVUI;
  char SEIMessageText[500];
  int VUISupport;

  int ResendSPS;
  int ResendPPS;

  // B pictures
  int successive_Bframe;             //!< number of B frames that will be used
  int PReplaceBSlice;
  int qpB;                           //!< QP for non-reference B slice coded pictures
  int qpBRSOffset;                   //!< QP for reference B slice coded pictures
  int direct_spatial_mv_pred_flag;   //!< Direct Mode type to be used (0: Temporal, 1: Spatial)
  int directInferenceFlag;           //!< Direct Mode Inference Flag

  int BiPredMotionEstimation;
  int BiPredMERefinements;
  int BiPredMESearchRange;
  int BiPredMESubPel;


  // SP Pictures
  int sp_periodicity;                //!< The periodicity of SP-pictures
  int qpsp;                          //!< SP Picture QP for prediction error
  int qpsp_pred;                     //!< SP Picture QP for predicted block

  int si_frame_indicator;            //!< Flag indicating whether SI frames should be encoded rather than SP frames (0: not used, 1: used)
  int sp2_frame_indicator;           //!< Flag indicating whether switching SP frames should be encoded rather than SP frames (0: not used, 1: used)
  int sp_output_indicator;           //!< Flag indicating whether coefficients are output to allow future encoding of switchin SP frames (0: not used, 1: used)
  char sp_output_filename[FILE_NAME_SIZE];    //!<Filename where SP coefficients are output
  char sp2_input_filename1[FILE_NAME_SIZE];   //!<Filename of coefficients of the first bitstream when encoding SP frames to switch bitstreams
  char sp2_input_filename2[FILE_NAME_SIZE];   //!<Filenames of coefficients of the second bitstream when encoding SP frames to switch bitstreams

  int WeightedPrediction;            //!< Weighted prediction for P frames (0: not used, 1: explicit)
  int WeightedBiprediction;          //!< Weighted prediction for B frames (0: not used, 1: explicit, 2: implicit)
  int UseWeightedReferenceME;        //!< Use Weighted Reference for ME.
  int RDPictureDecision;             //!< Perform RD optimal decision between various coded versions of same picture
  int RDPictureIntra;                //!< Enabled RD pic decision for intra as well.
  int RDPSliceWeightOnly;            //!< If enabled, does not check QP variations for P slices.
  int RDPSliceBTest;                 //!< Tests B slice replacement for P.
  int RDBSliceWeightOnly;            //!< If enabled, does not check QP variations for B slices.
  int SkipIntraInInterSlices;        //!< Skip intra type checking in inter slices if best_mode is skip/direct
  int BRefPictures;                  //!< B coded reference pictures replace P pictures (0: not used, 1: used)
  int HierarchicalCoding;
  int HierarchyLevelQPEnable;
  char ExplicitHierarchyFormat[1024];//!< Explicit GOP format (HierarchicalCoding==3).
  int ReferenceReorder;              //!< Reordering based on Poc distances
  int PocMemoryManagement;           //!< Memory management based on Poc distances for hierarchical coding

  int symbol_mode;                   //!< Specifies the mode the symbols are mapped on bits
  int of_mode;                       //!< Specifies the mode of the output file
  int partition_mode;                //!< Specifies the mode of data partitioning

  int InterSearch16x16;
  int InterSearch16x8;
  int InterSearch8x16;
  int InterSearch8x8;
  int InterSearch8x4;
  int InterSearch4x8;
  int InterSearch4x4;

  int IntraDisableInterOnly;
  int Intra4x4ParDisable;
  int Intra4x4DiagDisable;
  int Intra4x4DirDisable;
  int Intra16x16ParDisable;
  int Intra16x16PlaneDisable;
  int ChromaIntraDisable;

  int EnableIPCM;

  double FrameRate;

  int EPZSPattern;
  int EPZSDual;
  int EPZSFixed;
  int EPZSTemporal;
  int EPZSSpatialMem;
  int EPZSMinThresScale;
  int EPZSMaxThresScale;
  int EPZSMedThresScale;
  int EPZSSubPelGrid;
  int EPZSSubPelME;
  int EPZSSubPelMEBiPred;
  int EPZSSubPelThresScale;

  int chroma_qp_index_offset;
#ifdef _FULL_SEARCH_RANGE_
  int full_search;
#endif
#ifdef _ADAPT_LAST_GROUP_
  int last_frame;
#endif
#ifdef _CHANGE_QP_
  int qpN2, qpB2, qp2start;
  int qp02, qpBRS2Offset;
#endif
  int rdopt;
  int CtxAdptLagrangeMult;    //!< context adaptive lagrangian multiplier
  int FastCrIntraDecision;
  int disthres;
  int nobskip;

#ifdef _LEAKYBUCKET_
  int NumberLeakyBuckets;
  char LeakyBucketRateFile[FILE_NAME_SIZE];
  char LeakyBucketParamFile[FILE_NAME_SIZE];
#endif

  int PicInterlace;           //!< picture adaptive frame/field
  int MbInterlace;            //!< macroblock adaptive frame/field

  int IntraBottom;            //!< Force Intra Bottom at GOP periods.

  int LossRateA;              //!< assumed loss probablility of partition A (or full slice), in per cent, used for loss-aware R/D optimization
  int LossRateB;              //!< assumed loss probablility of partition B, in per cent, used for loss-aware R/D
  int LossRateC;              //!< assumed loss probablility of partition C, in per cent, used for loss-aware R/D
  int NoOfDecoders;
  int RestrictRef;
  int NumFramesInELSubSeq;
  int NumFrameIn2ndIGOP;

  int RandomIntraMBRefresh;     //!< Number of pseudo-random intra-MBs per picture

  int LFSendParameters;
  int LFDisableIdc;
  int LFAlphaC0Offset;
  int LFBetaOffset;

  int SparePictureOption;
  int SPDetectionThreshold;
  int SPPercentageThreshold;

  // FMO
  char SliceGroupConfigFileName[FILE_NAME_SIZE];    //!< Filename for config info fot type 0, 2, 6
  int num_slice_groups_minus1;           //!< "FmoNumSliceGroups" in encoder.cfg, same as FmoNumSliceGroups, which should be erased later
  int slice_group_map_type;

  int *top_left;                         //!< top_left and bottom_right store values indicating foregrounds
  int *bottom_right;
  byte *slice_group_id;                   //!< slice_group_id is for slice group type being 6
  int *run_length_minus1;                //!< run_length_minus1 is for slice group type being 0

  int slice_group_change_direction_flag;
  int slice_group_change_rate_minus1;
  int slice_group_change_cycle;

  int redundant_pic_flag;   //! encoding of redundant pictures
  int pic_order_cnt_type;   //! POC type

  int context_init_method;
  int model_number;
  int Transform8x8Mode;
  int ReportFrameStats;
  int DisplayEncParams;
  int Verbose;

  //! Rate Control on JVT standard
  int RCEnable;
  int bit_rate;
  int SeinitialQP;
  unsigned int basicunit;
  int channel_type;
  int RCUpdateMode;
  double RCIoverPRatio;
  double RCBoverPRatio;
  double RCISliceBitRatio;
  double RCBSliceBitRatio[RC_MAX_TEMPORAL_LEVELS];

  int ScalingMatrixPresentFlag;
  int ScalingListPresentFlag[8];

  // Search Algorithm
  SearchType SearchMode;

  int UMHexDSR;//!< Dynamic Search Range
  int UMHexScale;
//////////////////////////////////////////////////////////////////////////
  // Fidelity Range Extensions
  int BitDepthLuma;
  int BitDepthChroma;
  int img_height_cr;
  int img_width_cr;
  int rgb_input_flag;
  int cb_qp_index_offset;
  int cr_qp_index_offset;

  // Lossless Coding
  int lossless_qpprime_y_zero_flag;

  // Lambda Params
  int UseExplicitLambdaParams;
  double LambdaWeight[6];
  double FixedLambda[6];

  char QOffsetMatrixFile[FILE_NAME_SIZE];        //!< Quantization Offset matrix cfg file
  int  OffsetMatrixPresentFlag;                  //!< Enable Explicit Quantization Offset Matrices

  int AdaptiveRounding;                          //!< Adaptive Rounding parameter based on JVT-N011
  int AdaptRndPeriod;                            //!< Set period for adaptive rounding of JVT-N011 in MBs
  int AdaptRndChroma;
  int AdaptRndWFactor[2][5];                     //!< Weighting factors for luma component based on reference indicator and slice type
  int AdaptRndCrWFactor[2][5];                   //!< Weighting factors for chroma components based on reference indicator and slice type
  // Fast Mode Decision
  int EarlySkipEnable;
  int SelectiveIntraEnable;
  int DisposableP;
  int DispPQPOffset;

  //Redundant picture
  int NumRedundantHierarchy;   //!< number of entries to allocate redundant pictures in a GOP
  int PrimaryGOPLength;        //!< GOP length of primary pictures
  int NumRefPrimary;           //!< number of reference frames for primary picture

  // Chroma interpolation and buffering
  int ChromaMCBuffer;
  int ChromaMEEnable;
  int MEErrorMetric[3];
  int ModeDecisionMetric;

} InputParameters;

//! ImageParameters
typedef struct
{
  int number;                  //!< current image number to be encoded
  int pn;                      //!< picture number
  int LevelIndex;              //!< mapped level idc
  int current_mb_nr;
  int current_slice_nr;
  int type;
  int structure;               //!< picture structure
  int num_ref_frames;          //!< number of reference frames to be used
  int max_num_references;      //!< maximum number of reference pictures that may occur
  int qp;                      //!< quant for the current frame
  int qpsp;                    //!< quant for the prediction frame of SP-frame
  int qp_scaled;
  float framerate;
  int width;                   //!< Number of pels
  int width_padded;            //!< Width in pels of padded picture
  int width_blk;               //!< Number of columns in blocks
  int width_cr;                //!< Number of pels chroma
  int height;                  //!< Number of lines
  int height_padded;           //!< Number in lines of padded picture
  int height_blk;              //!< Number of lines in blocks
  int height_cr;               //!< Number of lines  chroma
  int height_cr_frame;         //!< Number of lines  chroma frame
  int size;                    //!< Luma Picture size in pels
  int size_cr;                 //!< Chroma Picture size in pels
  int subblock_x;              //!< current subblock horizontal
  int subblock_y;              //!< current subblock vertical
  int is_intra_block;
  int is_v_block;
  int mb_y_upd;
  int mb_y_intra;              //!< which GOB to intra code
  int block_c_x;               //!< current block chroma vertical
  signed char **ipredmode;     //!< intra prediction mode
  signed char **ipredmode8x8;  //!< help storage for 8x8 modes, inserted by YV

  int cod_counter;             //!< Current count of number of skipped macroblocks in a row
  int ***nz_coeff;             //!< number of coefficients per block (CAVLC)

  int mb_x;                    //!< current MB horizontal
  int mb_y;                    //!< current MB vertical
  int block_x;                 //!< current block horizontal
  int block_y;                 //!< current block vertical
  int pix_x;                   //!< current pixel horizontal
  int pix_y;                   //!< current pixel vertical
  int pix_c_x;                 //!< current pixel chroma horizontal
  int pix_c_y;                 //!< current pixel chroma vertical

  int opix_x;                   //!< current original picture pixel horizontal
  int opix_y;                   //!< current original picture pixel vertical
  int opix_c_x;                 //!< current original picture pixel chroma horizontal
  int opix_c_y;                 //!< current original picture pixel chroma vertical


  // some temporal buffers
  imgpel mprr[9][16][16];      //!< all 9 prediction modes? // enlarged from 4 to 16 for ABT (is that neccessary?)

  imgpel mprr_2[5][16][16];    //!< all 4 new intra prediction modes
  imgpel mprr_3[9][8][8];      //!< all 9 prediction modes for 8x8 transformation
  imgpel mprr_c[2][4][16][16]; //!< chroma intra prediction modes
  imgpel mpr[16][16];          //!< current best prediction mode
  int m7[16][16];              //!< the diff pixel values between orginal image and prediction

  int ****cofAC;               //!< AC coefficients [8x8block][4x4block][level/run][scan_pos]
  int ***cofDC;                //!< DC coefficients [yuv][level/run][scan_pos]

  int ***fadjust4x4;           //!< Transform coefficients for 4x4 luma. Excludes DC for I16x16
  int ***fadjust8x8;           //!< Transform coefficients for 8x8 luma
  int ****fadjust4x4Cr;        //!< Transform coefficients for 4x4 chroma. Excludes DC chroma.
  int ****fadjust8x8Cr;        //!< Transform coefficients for 4x4 chroma within 8x8 inter blocks.


  Picture       *currentPicture; //!< The coded picture currently in the works (typically frame_pic, top_pic, or bottom_pic)
  Slice         *currentSlice;                                //!< pointer to current Slice data struct
  Macroblock    *mb_data;                                   //!< array containing all MBs of a whole frame

  int *quad;               //!< Array containing square values,used for snr computation  */                                         /* Values are limited to 5000 for pixel differences over 70 (sqr(5000)).
  int *intra_block;

  int tr;
  int fld_type;                        //!< top or bottom field
  unsigned int fld_flag;
  unsigned int rd_pass;
  int direct_intraP_ref[4][4];
  int pstruct_next_P;
  int imgtr_next_P_frm;
  int imgtr_last_P_frm;
  int imgtr_next_P_fld;
  int imgtr_last_P_fld;

  // B pictures
  double b_interval;
  int p_interval;
  int b_frame_to_code;
  int fw_mb_mode;
  int bw_mb_mode;

  short****** pred_mv;                 //!< motion vector predictors for all block types and all reference frames
  short****** all_mv;                  //!< replaces local all_mv

  short****** bipred_mv1;              //!< Biprediction MVs
  short****** bipred_mv2;              //!< Biprediction MVs
  short bi_pred_me[MAXMODE];

  int LFDisableIdc;
  int LFAlphaC0Offset;
  int LFBetaOffset;

  int direct_spatial_mv_pred_flag;              //!< Direct Mode type to be used (0: Temporal, 1: Spatial)

  int num_ref_idx_l0_active;
  int num_ref_idx_l1_active;

  int field_mode;     //!< For MB level field/frame -- field mode on flag
  int top_field;      //!< For MB level field/frame -- top field flag
  int mvscale[6][MAX_REFERENCE_PICTURES];
  int buf_cycle;
  int i16offset;

  int layer;             //!< which layer this picture belonged to
  int old_layer;         //!< old layer number
  int NoResidueDirect;
  int AdaptiveRounding;                          //!< Adaptive Rounding parameter based on JVT-N011

  int redundant_pic_cnt; // JVT-D101

  int MbaffFrameFlag;    //!< indicates frame with mb aff coding

  //the following should probably go in sequence parameters
  unsigned int pic_order_cnt_type;

  // for poc mode 1
  Boolean      delta_pic_order_always_zero_flag;
  int          offset_for_non_ref_pic;
  int          offset_for_top_to_bottom_field;
  unsigned int num_ref_frames_in_pic_order_cnt_cycle;
  int          offset_for_ref_frame[1];

  //the following is for slice header syntax elements of poc
  // for poc mode 0.
  unsigned int pic_order_cnt_lsb;
  int          delta_pic_order_cnt_bottom;
  // for poc mode 1.
  int          delta_pic_order_cnt[2];


  unsigned int field_picture;
    signed int toppoc;      //!< poc for this frame or field
    signed int bottompoc;   //!< for completeness - poc of bottom field of a frame (always = poc+1)
    signed int framepoc;    //!< min (toppoc, bottompoc)
    signed int ThisPOC;     //!< current picture POC
  unsigned int frame_num;   //!< frame_num for this frame

  unsigned int PicWidthInMbs;
  unsigned int PicHeightInMapUnits;
  unsigned int FrameHeightInMbs;
  unsigned int PicSizeInMbs;
  unsigned int FrameSizeInMbs;

  //the following should probably go in picture parameters
  Boolean pic_order_present_flag; // ????????

  //the following are sent in the slice header
//  int delta_pic_order_cnt[2];
  int nal_reference_idc;

  int adaptive_ref_pic_buffering_flag;
  int no_output_of_prior_pics_flag;
  int long_term_reference_flag;

  DecRefPicMarking_t *dec_ref_pic_marking_buffer;

  int model_number;

  // rate control variables
  int NumberofCodedMacroBlocks;
  int BasicUnitQP;
  int NumberofMBTextureBits;
  int NumberofMBHeaderBits;
  unsigned int BasicUnit;
  int write_macroblock;
  int bot_MB;
  int write_mbaff_frame;

  int DeblockCall;

  int last_pic_bottom_field;
  int last_has_mmco_5;
  int pre_frame_num;

  int slice_group_change_cycle;

  int pic_unit_size_on_disk;
  int bitdepth_luma;
  int bitdepth_chroma;
  int bitdepth_luma_qp_scale;
  int bitdepth_chroma_qp_scale;
  int bitdepth_lambda_scale;
  int max_bitCount;
  int max_qp_delta;
  int min_qp_delta;
  // Lagrangian Parameters
  double  **lambda_md;     //!< Mode decision Lambda
  double ***lambda_me;     //!< Motion Estimation Lambda
  int    ***lambda_mf;     //!< Integer formatted Motion Estimation Lambda

  double **lambda_mf_factor; //!< Motion Estimation Lamda Scale Factor

  unsigned int dc_pred_value_luma;   //!< luma value for DC prediction (depends on luma pel bit depth)
  unsigned int dc_pred_value_chroma; //!< chroma value for DC prediction (depends on chroma pel bit depth)
  int max_imgpel_value;              //!< max value that one picture element (pixel) can take (depends on pic_unit_bitdepth)
  int max_imgpel_value_uv;

  int num_blk8x8_uv;
  int num_cdc_coeff;
  int yuv_format;
  int lossless_qpprime_flag;
  int mb_cr_size_x;
  int mb_cr_size_y;
  int mb_size[3][2];

  int chroma_qp_offset[2];      //!< offset for qp for chroma [0-Cb, 1-Cr]

  int auto_crop_right;
  int auto_crop_bottom;

  short checkref;
  int last_valid_reference;
  int bytes_in_picture;

  int AverageFrameQP;
  int SumFrameQP;
  int GopLevels;
} ImageParameters;

#define NUM_PIC_TYPE 5
                                //!< statistics
typedef struct
{
  int   quant0;                      //!< quant for the first frame
  int   quant1;                      //!< average quant for the remaining frames
  float bitr;                        //!< bit rate for current frame, used only for output til terminal
  float bitrate;                     //!< average bit rate for the sequence except first frame
  int64 bit_ctr;                     //!< counter for bit usage
  int64 bit_ctr_n;                   //!< bit usage for the current frame
  int   bit_slice;                   //!< number of bits in current slice
  int   stored_bit_slice;            //!< keep number of bits in current slice (to restore status in case of MB re-encoding)
  int   bit_ctr_emulationprevention; //!< stored bits needed to prevent start code emulation
  int   b8_mode_0_use[NUM_PIC_TYPE][2];
  int   mode_use_transform_8x8[NUM_PIC_TYPE][MAXMODE];
  int   mode_use_transform_4x4[NUM_PIC_TYPE][MAXMODE];
  int   intra_chroma_mode[4];

  // B pictures
  int   successive_Bframe;
  int   *mode_use_Bframe;
  int   *bit_use_mode_Bframe;
  int64   bit_ctr_I;
  int64   bit_ctr_P;
  int64   bit_ctr_B;
  float bitrate_I;
  float bitrate_P;
  float bitrate_B;

  int64   mode_use            [NUM_PIC_TYPE][MAXMODE]; //!< Macroblock mode usage for Intra frames
  int64   bit_use_mode        [NUM_PIC_TYPE][MAXMODE]; //!< statistics of bit usage
  int64   bit_use_stuffingBits[NUM_PIC_TYPE];
  int64   bit_use_mb_type     [NUM_PIC_TYPE];
  int64   bit_use_header      [NUM_PIC_TYPE];
  int64   tmp_bit_use_cbp     [NUM_PIC_TYPE];
  int64   bit_use_coeffY      [NUM_PIC_TYPE];
  int64   bit_use_coeffC      [NUM_PIC_TYPE];
  int64   bit_use_delta_quant [NUM_PIC_TYPE];

  int   em_prev_bits_frm;
  int   em_prev_bits_fld;
  int  *em_prev_bits;
  int   bit_ctr_parametersets;
  int   bit_ctr_parametersets_n;
  } StatParameters;

//! For MB level field/frame coding tools
//! temporary structure to store MB data for field/frame coding
typedef struct
{
  double min_rdcost;

  imgpel rec_mbY[16][16];       // hold the Y component of reconstructed MB
  imgpel rec_mbU[16][16], rec_mbV[16][16];
  int    ****cofAC;
  int    ***cofDC;
  int    mb_type;
  short  bi_pred_me;

  int    b8mode[4], b8pdir[4];
  signed char **ipredmode;
  signed char intra_pred_modes[16];
  signed char intra_pred_modes8x8[16];
  int    cbp;
  int64  cbp_blk;
  int    mode;
  short  ******pred_mv;        //!< predicted motion vectors
  short  ******all_mv;         //!< all modes motion vectors
  signed char   refar[2][4][4];//!< reference frame array [list][y][x]
  int    i16offset;
  int    c_ipred_mode;

  int    luma_transform_size_8x8_flag;
  int    NoMbPartLessThan8x8Flag;

  int    qp;
  int    prev_qp;
  int    prev_delta_qp;
  int    delta_qp;
  int    prev_cbp;
} RD_DATA;


//! Set Explicit GOP Parameters.
//! Currently only supports Enhancement GOP but could be easily extended
typedef struct
{
  int slice_type;       //! Slice type
  int display_no;       //! GOP Display order
  int reference_idc;    //! Is reference?
  int slice_qp;         //! Assigned QP
  int hierarchy_layer;    //! Hierarchy layer (used with GOP Hierarchy option 2)
  int hierarchyPocDelta;  //! Currently unused
} GOP_DATA;


typedef struct
{
  int cost8x8;
  int rec_resG_8x8[16][16];
  int resTrans_R_8x8[16][16];
  int resTrans_B_8x8[16][16];
  int mprRGB_8x8[3][16][16];
  short part8x8mode[4];
  signed char part8x8pdir[4];
  signed char part8x8fwref[4];
  signed char part8x8bwref[4];
  imgpel rec_mbY8x8[16][16];
  imgpel mpr8x8[16][16];
  int lrec[16][16]; // transform and quantized coefficients will be stored here for SP frames
} RD_8x8DATA;

typedef struct
{
  double lambda_md;        //!< Mode decision Lambda
  double lambda_me[3];     //!< Motion Estimation Lambda
  int    lambda_mf[3];     //!< Integer formatted Motion Estimation Lambda

  short  valid[MAXMODE];
  short  list_offset[2];
  short  curr_mb_field;
  short  best_ref[2];
  int    best_mcost[2];
} RD_PARAMS;

GOP_DATA *gop_structure;
RD_DATA *rdopt;
RD_DATA rddata_top_frame_mb, rddata_bot_frame_mb; //!< For MB level field/frame coding tools
RD_DATA rddata_top_field_mb, rddata_bot_field_mb; //!< For MB level field/frame coding tools

extern InputParameters *input;
extern ImageParameters *img;
extern StatParameters  *stats;

extern SNRParameters *snr;

// files
FILE *p_stat;                    //!< status file for the last encoding session
FILE *p_log;                     //!< SNR file
FILE *p_trace;                   //!< Trace file
int  p_in;                       //!< original YUV file handle
int  p_dec;                      //!< decoded image file handle


/***********************************************************************
 * P r o t o t y p e s   f o r    T M L
 ***********************************************************************
 */

void intrapred_luma(int CurrPixX,int CurrPixY, int *left_available, int *up_available, int *all_available);
int  dct_luma(int pos_mb1,int pos_mb2,int *cnt_nonz, int intra);
int  dct_luma_sp(int pos_mb1,int pos_mb2,int *cnt_nonz);
void copyblock_sp(int pos_mb1,int pos_mb2);
int  dct_chroma(int uv,int i11);
int  dct_chroma_sp(int uv,int i11);

void intrapred_luma_16x16(void);

int dct_luma_16x16(int);

void init_poc(void);

void init_img(void);
void report(void);
int  get_picture_type(void);
void DeblockFrame(ImageParameters *img, imgpel **, imgpel ***) ;

int  distortion4x4(int*);
int  distortion8x8(int*);

extern int*   refbits;
extern int**** motion_cost;
double *mb16x16_cost_frame;

void  Get_Direct_Motion_Vectors (void);
void  PartitionMotionSearch     (int, int, int*);
int   BIDPartitionCost          (int, int, short, short, int);
int   writeAbpCoeffIndex        (int, int, int, int);

void estimate_weighting_factor_B_slice(void);
void estimate_weighting_factor_P_slice(int offset);
int  test_wp_P_slice(int offset);
int  test_wp_B_slice(int method);
void poc_based_ref_management(int current_pic_num);
int  picture_coding_decision (Picture *picture1, Picture *picture2, int qp);

unsigned CeilLog2( unsigned uiVal);

int  GetDirectCost8x8 (int, int*);

int   BPredPartitionCost  (int, int, short, short, int, int);

int  GetDirectCostMB  (void);

int  GetSkipCostMB (void);
void FindSkipModeMotionVector (void);


// dynamic mem allocation
int  init_global_buffers(void);
void free_global_buffers(void);
void no_mem_exit  (char *where);

int  get_mem_mv  (short*******);
void free_mem_mv (short******);
void free_img    (void);

int  get_mem_ACcoeff  (int*****);
int  get_mem_DCcoeff  (int****);
void free_mem_ACcoeff (int****);
void free_mem_DCcoeff (int***);

int  decide_fld_frame(float snr_frame_Y, float snr_field_Y, int bit_field, int bit_frame, double lambda_picture);
void combine_field(void);

Picture *malloc_picture(void);
void     free_picture (Picture *pic);

int   encode_one_slice(int SLiceGroupId, Picture *pic, int TotalCodedMBs);   //! returns the number of MBs in the slice

void free_slice_list(Picture *currPic);

void report_stats_on_error(void);

#if TRACE
void  trace2out(SyntaxElement *se);
void  trace2out_cabac(SyntaxElement *se);
#endif


void error(char *text, int code);
int  start_sequence(void);
int  rewrite_paramsets(void);
int  terminate_sequence(void);
int  start_slice(void);
int  terminate_slice(int);
int  write_PPS(int, int);

// B pictures
int  get_fwMV(int *min_fw_sad, int tot_intra_sad);
void get_bwMV(int *min_bw_sad);
void get_bid(int *bid_sad, int fw_predframe_no);
void get_dir(int *dir_sad);
void compare_sad(int tot_intra_sad, int fw_sad, int bw_sad, int bid_sad, int dir_sad, int);
int  BlkSize2CodeNumber(int blc_size_h, int blc_size_v);

void InitMotionVectorSearchModule(void);

int  field_flag_inference(void);

void set_mbaff_parameters(void);  // For MB AFF
void writeVlcByteAlign(Bitstream* currStream);


int   writeMB_bits_for_4x4_luma   (int, int, int);
int   writeMB_bits_for_16x16_luma (void);
int   writeMB_bits_for_luma       (int);
int   writeMB_bits_for_DC_chroma  (int);
int   writeMB_bits_for_AC_chroma  (int);
int   writeMB_bits_for_CBP        (void);

int   SingleUnifiedMotionSearch   (int, int, int**, int***, int*****, int, int*****, double);

//============= rate-distortion optimization ===================
void  clear_rdopt      (void);
void  init_rdopt       (void);
void  RD_Mode_Decision (void);
//============= rate-distortion opt with packet losses ===========
void decode_one_macroblock(void);
void decode_one_mb (int, Macroblock*);
void decode_one_b8block (int, int, int, int, int);
void Get_Reference_Block(imgpel **imY, int block_y, int block_x, int mvhor, int mvver, imgpel **out);
byte Get_Reference_Pixel(imgpel **imY, int y, int x);
int  Half_Upsample(imgpel **imY, int j, int i);
void DecOneForthPix(imgpel **dY, imgpel ***dref);
void compute_residue(int mode);
void compute_residue_b8block (int, int);
void compute_residue_mb (int);
void UpdateDecoders(void);
void Build_Status_Map(byte **s_map);
void Error_Concealment(imgpel **inY, byte **s_map, imgpel ***refY);
void Conceal_Error(imgpel **inY, int mb_y, int mb_x, imgpel ***refY, byte **s_map);
//============= restriction of reference frames based on the latest intra-refreshes==========
void UpdatePixelMap(void);

//============= fast full integer search =======================
void  ClearFastFullIntegerSearch    (void);
void  ResetFastFullIntegerSearch    (void);

void process_2nd_IGOP(void);
void SetImgType(void);

// Tian Dong: for IGOPs
extern Boolean In2ndIGOP;
extern int start_frame_no_in_this_IGOP;
extern int start_tr_in_this_IGOP;
extern int FirstFrameIn2ndIGOP;
#define IMG_NUMBER (img->number - start_frame_no_in_this_IGOP)

void AllocNalPayloadBuffer(void);
void FreeNalPayloadBuffer(void);
void SODBtoRBSP(Bitstream *currStream);
int RBSPtoEBSP(byte *streamBuffer, int begin_bytepos, int end_bytepos, int min_num_bytes);
int Bytes_After_Header;

// Fast ME enable
int BlockMotionSearch (short,int,int,int,int,int, int*);
void low_complexity_encode_md (void);
void encode_one_macroblock_low (void);
void encode_one_macroblock_high (void);
void encode_one_macroblock_highfast (void);
void encode_one_macroblock_highloss (void);
void (*encode_one_macroblock) (void);


void set_chroma_qp(Macroblock *currMB);


#include "context_ini.h"

void store_coding_state_cs_cm(void);
void reset_coding_state_cs_cm(void);

int writeIPCMBytes(Bitstream *currStream);
int writePCMByteAlign(Bitstream *currStream);


int  dct_luma_sp2(int pos_mb1,int pos_mb2,int *cnt_nonz);
int  dct_chroma_sp2(int ,int);

int check_for_SI16(void);
int **lrec ;
int ***lrec_uv;
int si_frame_indicator;

int sp2_frame_indicator;
int number_sp2_frames;
//#define sp_output_indicator 0 //will be in the config file
//#define sp_output_filename "sp_stored.txt" // will be in the config file
void output_SP_coefficients(void);
void read_SP_coefficients(void);

int giRDOpt_B8OnlyFlag;

#ifdef BEST_NZ_COEFF
int gaaiMBAFF_NZCoeff[4][12];
#endif

// Redundant picture
imgpel **imgY_tmp;
imgpel **imgUV_tmp[2];
int frameNuminGOP;
int redundant_coding;
int key_frame;
int redundant_ref_idx;
void Init_redundant_frame(void);
void Set_redundant_frame(void);
void encode_one_redundant_frame(void);

int img_pad_size_uv_x;
int img_pad_size_uv_y;

unsigned char chroma_mask_mv_y;
unsigned char chroma_mask_mv_x;
int chroma_shift_y, chroma_shift_x;
int shift_cr_x, shift_cr_y;
int img_padded_size_x;
int img_cr_padded_size_x;

// struct with pointers to the sub-images
typedef struct {
  imgpel ****luma; // component 0 (usually Y, X, or R)
  imgpel ****crcb[2]; // component 2 (usually U/V, Y/Z, or G/B)
} SubImageContainer;

int start_me_refinement_hp; // if set then recheck the center position when doing half-pel motion refinement
int start_me_refinement_qp; // if set then recheck the center position when doing quarter-pel motion refinement

#endif

