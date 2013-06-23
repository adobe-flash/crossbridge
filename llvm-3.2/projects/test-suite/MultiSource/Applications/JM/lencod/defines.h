
/*!
 **************************************************************************
 * \file defines.h
 *
 * \brief
 *    Header file containing some useful global definitions
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

#define GET_METIME      1       //!< Enables or disables ME computation time
#define DUMP_DPB        0       //!< Dump dbp for debug purposes
typedef unsigned char byte;    //!< byte type definition

#define RC_MAX_TEMPORAL_LEVELS   5

//#define BEST_NZ_COEFF 1   // yuwen 2005.11.03 => for high complexity mode decision (CAVLC, #TotalCoeff)

//FREXT Profile IDC definitions
#define FREXT_HP        100      //!< YUV 4:2:0/8 "High"
#define FREXT_Hi10P     110      //!< YUV 4:2:0/10 "High 10"
#define FREXT_Hi422     122      //!< YUV 4:2:2/10 "High 4:2:2"
#define FREXT_Hi444     144      //!< YUV 4:4:4/12 "High 4:4:4"

#define ZEROSNR 1

// CAVLC
#define LUMA              0
#define LUMA_INTRA16x16DC 1
#define LUMA_INTRA16x16AC 2

#define LEVEL_NUM      6
#define TOTRUN_NUM    15
#define RUNBEFORE_NUM  7

#define CAVLC_LEVEL_LIMIT 2063

//--- block types for CABAC
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

#define _FULL_SEARCH_RANGE_
#define _ADAPT_LAST_GROUP_
#define _CHANGE_QP_
#define _LEAKYBUCKET_

// ---------------------------------------------------------------------------------
// FLAGS and DEFINES for new chroma intra prediction, Dzung Hoang
// Threshold values to zero out quantized transform coefficients.
// Recommend that _CHROMA_COEFF_COST_ be low to improve chroma quality
#define _LUMA_COEFF_COST_       4 //!< threshold for luma coeffs
#define _CHROMA_COEFF_COST_     4 //!< threshold for chroma coeffs, used to be 7
#define _LUMA_MB_COEFF_COST_    5 //!< threshold for luma coeffs of inter Macroblocks
#define _LUMA_8x8_COEFF_COST_   5 //!< threshold for luma coeffs of 8x8 Inter Partition

#define IMG_PAD_SIZE           20 //!< Number of pixels padded around the reference frame (>=4)
#define IMG_PAD_SIZE_TIMES4    80 //!< Number of pixels padded around the reference frame in subpel units(>=16)

#define MAX_VALUE       999999   //!< used for start value for some variables
#define INVALIDINDEX  (-135792468)

#define P8x8    8
#define I4MB    9
#define I16MB   10
#define IBLOCK  11
#define SI4MB   12
#define I8MB    13
#define IPCM    14
#define MAXMODE 15


#define  LAMBDA_ACCURACY_BITS         16
#define  LAMBDA_FACTOR(lambda)        ((int)((double)(1<<LAMBDA_ACCURACY_BITS)*lambda+0.5))
#define  WEIGHTED_COST(factor,bits)   (((factor)*(bits))>>LAMBDA_ACCURACY_BITS)
#define  MV_COST(f,s,cx,cy,px,py)     (WEIGHTED_COST(f,mvbits[((cx)<<(s))-px]+mvbits[((cy)<<(s))-py]))
#define  MV_COST_SMP(f,cx,cy,px,py)     (WEIGHTED_COST(f,mvbits[cx-px]+mvbits[cy-py]))
#define  REF_COST(f,ref,list_offset) (WEIGHTED_COST(f,((listXsize[list_offset]<=1)? 0:refbits[(ref)])))

#define IS_INTRA(MB)    ((MB)->mb_type==I4MB  || (MB)->mb_type==I16MB || (MB)->mb_type==I8MB || (MB)->mb_type==IPCM)
#define IS_NEWINTRA(MB) ((MB)->mb_type==I16MB)
#define IS_OLDINTRA(MB) ((MB)->mb_type==I4MB)
#define IS_IPCM(MB)     ((MB)->mb_type==IPCM)

#define IS_INTER(MB)    ((MB)->mb_type!=I4MB  && (MB)->mb_type!=I16MB && (MB)->mb_type!=I8MB)
#define IS_INTERMV(MB)  ((MB)->mb_type!=I4MB  && (MB)->mb_type!=I16MB && (MB)->mb_type!=I8MB  && (MB)->mb_type!=0)
#define IS_DIRECT(MB)   ((MB)->mb_type==0     && (img->type==B_SLICE))
#define IS_COPY(MB)     ((MB)->mb_type==0     && (img->type==P_SLICE||img ->type==SP_SLICE))
#define IS_P8x8(MB)     ((MB)->mb_type==P8x8)

// Quantization parameter range

#define MIN_QP          0
#define MAX_QP          51
#define SHIFT_QP        12

// Direct Mode types
#define DIR_TEMPORAL    0   //!< Temporal Direct Mode
#define DIR_SPATIAL     1   //!< Spatial Direct Mode

#define MAX_REFERENCE_PICTURES 32

#define BLOCK_SHIFT     2
#define BLOCK_SIZE      4
#define BLOCK_SIZE8x8   8
#define MB_BLOCK_SIZE   16
#define MB_BLOCK_SHIFT  4

// These variables relate to the subpel accuracy supported by the software (1/4)
#define BLOCK_SIZE_SP      16  // BLOCK_SIZE << 2
#define BLOCK_SIZE8x8_SP   32  // BLOCK_SIZE8x8 << 2

// number of intra prediction modes
#define NO_INTRA_PMODE  9

// 4x4 intra prediction modes
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

#define INIT_FRAME_RATE 30
#define EOS             1         //!< End Of Sequence


#define MVPRED_MEDIAN   0
#define MVPRED_L        1
#define MVPRED_U        2
#define MVPRED_UR       3

#define BLOCK_MULTIPLE        4   //(MB_BLOCK_SIZE/BLOCK_SIZE)
#define MB_BLOCK_PARTITIONS   16  //(BLOCK_MULTIPLE * BLOCK_MULTIPLE)
#define MB_PIXELS             256 //(MB_BLOCK_SIZE * MB_BLOCK_SIZE)
#define BLOCK_CONTEXT         64  //(4 * MB_BLOCK_PARTITIONS)

#define MAX_SYMBOLS_PER_MB  1200  //!< Maximum number of different syntax elements for one MB
                                  // CAVLC needs more symbols per MB


#define MAX_PART_NR     3 /*!< Maximum number of different data partitions.
                               Some reasonable number which should reflect
                               what is currently defined in the SE2Partition map (elements.h) */

//Start code and Emulation Prevention need this to be defined in identical manner at encoder and decoder
#define ZEROBYTES_SHORTSTARTCODE 2 //indicates the number of zero bytes in the short start-code prefix

#define Q_BITS          15
#define DQ_BITS         6
#define DQ_ROUND        (1<<(DQ_BITS-1))

#define Q_BITS_8        16
#define DQ_BITS_8       6
#define DQ_ROUND_8      (1<<(DQ_BITS_8-1))

// Context Adaptive Lagrange Multiplier (CALM)
#define CALM_MF_FACTOR_THRESHOLD 512.0

#endif

