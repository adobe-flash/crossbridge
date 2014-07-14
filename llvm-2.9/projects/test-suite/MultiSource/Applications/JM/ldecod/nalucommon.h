
/*!
 **************************************************************************************
 * \file
 *    nalucommon.h.h
 * \brief
 *    NALU handling common to encoder and decoder
 *  \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */


#ifndef _NALUCOMMON_H_
#define _NALUCOMMON_H_

#define MAXRBSPSIZE 64000

#define NALU_TYPE_SLICE    1
#define NALU_TYPE_DPA      2
#define NALU_TYPE_DPB      3
#define NALU_TYPE_DPC      4
#define NALU_TYPE_IDR      5
#define NALU_TYPE_SEI      6
#define NALU_TYPE_SPS      7
#define NALU_TYPE_PPS      8
#define NALU_TYPE_AUD      9
#define NALU_TYPE_EOSEQ    10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL     12

#define NALU_PRIORITY_HIGHEST     3
#define NALU_PRIORITY_HIGH        2
#define NALU_PRIRITY_LOW          1
#define NALU_PRIORITY_DISPOSABLE  0


typedef struct
{
  int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
  unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
  unsigned max_size;            //! Nal Unit Buffer size
  int nal_unit_type;            //! NALU_TYPE_xxxx
  int nal_reference_idc;        //! NALU_PRIORITY_xxxx
  int forbidden_bit;            //! should be always FALSE
  byte *buf;        //! conjtains the first byte followed by the EBSP
} NALU_t;


NALU_t *AllocNALU();
void FreeNALU(NALU_t *n);

#endif
