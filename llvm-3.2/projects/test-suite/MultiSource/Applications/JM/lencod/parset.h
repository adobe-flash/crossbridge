
/*!
 **************************************************************************************
 * \file
 *    parset.h
 * \brief
 *    Picture and Sequence Parameter Sets, encoder operations
 *    This code reflects JVT version xxx
 *  \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */


#ifndef _PARSET_H_
#define _PARSET_H_

#include "parsetcommon.h"
#include "nalu.h"
#include "sei.h"

void GenerateParameterSets (void);
void FreeParameterSets (void);

NALU_t *GenerateSeq_parameter_set_NALU (void);
NALU_t *GeneratePic_parameter_set_NALU (int);
NALU_t *GenerateSEImessage_NALU();

// The following are local helpers, but may come handy in the future, hence public
void GenerateSequenceParameterSet(seq_parameter_set_rbsp_t *sps, int SPS_id);
void GeneratePictureParameterSet( pic_parameter_set_rbsp_t *pps, seq_parameter_set_rbsp_t *sps, int PPS_id,
                                 int WeightedPrediction, int WeightedBiprediction,
                                 int cb_qp_index_offset, int cr_qp_index_offset);

int Scaling_List(short *scalingListinput, short *scalingList, int sizeOfScalingList, short *UseDefaultScalingMatrix, Bitstream *bitstream);
int GenerateSeq_parameter_set_rbsp (seq_parameter_set_rbsp_t *sps, byte *buf);
int GeneratePic_parameter_set_rbsp (pic_parameter_set_rbsp_t *pps, byte *buf);
int GenerateSEImessage_rbsp (int id, byte *buf);
void FreeSPS (seq_parameter_set_rbsp_t *sps);
void FreePPS (pic_parameter_set_rbsp_t *pps);

pic_parameter_set_rbsp_t *AllocPPS (void);
seq_parameter_set_rbsp_t *AllocSPS (void);


#endif
