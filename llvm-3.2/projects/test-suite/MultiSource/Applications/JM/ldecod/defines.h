
/*!
 **************************************************************************
 * \file defines.h
 *
 * \brief
 *    Headerfile containing some useful global definitions
 *
 * \author
 *    Detlev Marpe
 *    Copyright (C) 2000 HEINRICH HERTZ INSTITUTE All Rights Reserved.
 *
 * \date
 *    21. March 2001
 **************************************************************************
 */

#ifndef _DEFINES_H_
#define _DEFINES_H_

#if defined _DEBUG
#define TRACE           0                   //!< 0:Trace off 1:Trace on 2:detailed CABAC context information
#else
#define TRACE           0                   //!< 0:Trace off 1:Trace on 2:detailed CABAC context information
#endif

// Dump dbp for debug purposes
#define DUMP_DPB        0
//#define PAIR_FIELDS_IN_OUTPUT

//#define MAX_NUM_SLICES 150
#define MAX_NUM_SLICES 50

//FREXT Profile IDC definitions
#define FREXT_HP        100      //!< YUV 4:2:0/8 "High"
#define FREXT_Hi10P     110      //!< YUV 4:2:0/10 "High 10"
#define FREXT_Hi422     122      //!< YUV 4:2:2/10 "High 4:2:2"
#define FREXT_Hi444     144      //!< YUV 4:4:4/12 "High 4:4:4"

#define YUV400 0
#define YUV420 1
#define YUV422 2
#define YUV444 3


#define ZEROSNR 0

// CAVLC
#define LUMA              0
#define LUMA_INTRA16x16DC 1
#define LUMA_INTRA16x16AC 2

#define TOTRUN_NUM    15
#define RUNBEFORE_NUM  7


//--- block types for CABAC ----
#define LUMA_16DC       0
#define LUMA_16AC       1
#define LUMA_8x8        2
#define LUMA_8x4        3
#define LUMA_4x8        4
#define LUMA_4x4        5
#define CHROMA_DC       6
#define CHROMA_AC       7
#define CHROMA_DC_2x4   8
#define CHROMA_DC_4x4   9
#define NUM_BLOCK_TYPES 10


#define MAX_CODED_FRAME_SIZE 8000000         //!< bytes for one frame

//#define _LEAKYBUCKET_

#define P8x8    8
#define I4MB    9
#define I16MB   10
#define IBLOCK  11
#define SI4MB   12
#define I8MB    13
#define IPCM    14
#define MAXMODE 15

#define IS_INTRA(MB)    ((MB)->mb_type==I4MB  || (MB)->mb_type==I16MB ||(MB)->mb_type==IPCM || (MB)->mb_type==I8MB || (MB)->mb_type==SI4MB)
#define IS_NEWINTRA(MB) ((MB)->mb_type==I16MB  || (MB)->mb_type==IPCM)
#define IS_OLDINTRA(MB) ((MB)->mb_type==I4MB)

#define IS_INTER(MB)    ((MB)->mb_type!=I4MB  && (MB)->mb_type!=I16MB && (MB)->mb_type!=I8MB  && (MB)->mb_type!=IPCM)
#define IS_INTERMV(MB)  ((MB)->mb_type!=I4MB  && (MB)->mb_type!=I16MB && (MB)->mb_type!=I8MB  && (MB)->mb_type!=0 && (MB)->mb_type!=IPCM)
#define IS_DIRECT(MB)   ((MB)->mb_type==0     && (img->type==B_SLICE ))
#define IS_COPY(MB)     ((MB)->mb_type==0     && (img->type==P_SLICE || img->type==SP_SLICE))
#define IS_P8x8(MB)     ((MB)->mb_type==P8x8)


// Quantization parameter range

#define MIN_QP          0
#define MAX_QP          51

#define BLOCK_SIZE      4
#define MB_BLOCK_SIZE   16
#define MB_BLOCK_PIXELS 256    // MB_BLOCK_SIZE * MB_BLOCK_SIZE
#define BLOCK_MULTIPLE  4      // (MB_BLOCK_SIZE/BLOCK_SIZE)

#define NO_INTRA_PMODE  9        //!< #intra prediction modes
/* 4x4 intra prediction modes */
#define VERT_PRED             0
#define HOR_PRED              1
#define DC_PRED               2
#define DIAG_DOWN_LEFT_PRED   3
#define DIAG_DOWN_RIGHT_PRED  4
#define VERT_RIGHT_PRED       5
#define HOR_DOWN_PRED         6
#define VERT_LEFT_PRED        7
#define HOR_UP_PRED           8

// 16x16 intra prediction modes
#define VERT_PRED_16    0
#define HOR_PRED_16     1
#define DC_PRED_16      2
#define PLANE_16        3

// 8x8 chroma intra prediction modes
#define DC_PRED_8       0
#define HOR_PRED_8      1
#define VERT_PRED_8     2
#define PLANE_8         3

#define EOS             1                       //!< End Of Sequence
#define SOP             2                       //!< Start Of Picture
#define SOS             3                       //!< Start Of Slice

#define DECODING_OK     0
#define SEARCH_SYNC     1
#define PICTURE_DECODED 2

#define MAX_REFERENCE_PICTURES 32               //!< H264 allows 32 fields

#define INVALIDINDEX  (-135792468)

#define MVPRED_MEDIAN   0
#define MVPRED_L        1
#define MVPRED_U        2
#define MVPRED_UR       3

#define DECODE_COPY_MB  0
#define DECODE_MB       1
//#define DECODE_MB_BFRAME 2


//Start code and Emulation Prevention need this to be defined in identical manner at encoder and decoder
#define ZEROBYTES_SHORTSTARTCODE 2 //indicates the number of zero bytes in the short start-code prefix

#endif

