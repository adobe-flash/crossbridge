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
 *    - Yuri Vatis  vatis@hhi.de
 **************************************************************************/

#ifndef _TRANSFORM8X8_H_
#define _TRANSFORM8X8_H_

#include "global.h"
#include "image.h"
#include "mb_access.h"
#include "elements.h"
#include <math.h>


int   **cofAC8x8_intra, ****cofAC8x8_iintra; // [level/run][scan_pos]


void intrapred_luma8x8(int img_x,int img_y, int *left_available, int *up_available, int *all_available);
int intrapred8x8(struct img_par *img, int b8);
void itrans8x8(struct img_par *img, int ioff, int joff);
double RDCost_for_8x8IntraBlocks(int *c_nz, int b8, int ipmode, double lambda, double min_rdcost, int mostProbableMode);
int dct_luma8x8(int block_x,int block_y,int *coeff_cost);
void LowPassForIntra8x8Pred(imgpel *PredPel, int block_up_left, int block_up, int block_left);


#endif
