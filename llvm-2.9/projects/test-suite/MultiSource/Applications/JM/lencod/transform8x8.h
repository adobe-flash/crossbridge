
/*!
 ***************************************************************************
 *
 * \file transform8x8.h
 *
 * \brief
*    prototypes of 8x8 transform functions
  *
 * \date
 *    9. October 2003
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    Yuri Vatis  vatis@hhi.de
 **************************************************************************/

#ifndef _TRANSFORM8X8_H_
#define _TRANSFORM8X8_H_

int    Mode_Decision_for_new_Intra8x8Macroblock (double lambda, int *min_cost);
int    Mode_Decision_for_new_8x8IntraBlocks (int b8, double lambda, int *min_cost);

void   intrapred_luma8x8(int img_x,int img_y, int *left_available, int *up_available, int *all_available);

double RDCost_for_8x8IntraBlocks(int *c_nz, int b8, int ipmode, double lambda, double min_rdcost, int mostProbableMode);

int    dct_luma8x8(int b8,int *coeff_cost, int intra);

void   LowPassForIntra8x8Pred(imgpel *PredPel, int block_up_left, int block_up, int block_left);

#endif //_TRANSFORM8X8_H_
