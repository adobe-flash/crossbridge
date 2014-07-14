/*BHEADER****************************************************************
 * (c) 2007   The Regents of the University of California               *
 *                                                                      *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright       *
 * notice and disclaimer.                                               *
 *                                                                      *
 *EHEADER****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "Crystal.h"

//-------------- 
//  init
//-------------- 
void init(double slipRate[MS_XTAL_NSLIP_MAX],
          double dSlipRate[MS_XTAL_NSLIP_MAX],
          double tau[MS_XTAL_NSLIP_MAX],
          double tauc[MS_XTAL_NSLIP_MAX],
          double rhs[MS_XTAL_NSLIP_MAX],
          double dtcdgd[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	  double dtdg[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	  double matrix[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX])
{

  int m = 0;
  int n = 0;


  for (n = 0; n <MS_XTAL_NSLIP_MAX; n++){
      slipRate[n]   = 1.9 + (0.2 * n) / MS_XTAL_NSLIP_MAX;
      dSlipRate[n]  = slipRate[n] * 0.001;
      tau[n]        = n + 2.0;
      tauc[n]       = 30.0;
      rhs[n]        = n + 3.5;
      for (m = 0; m < MS_XTAL_NSLIP_MAX; m++) {
            dtcdgd[n][m] = 0.01 + 0.00001 * m * n;
            dtdg[n][m]   = m + n; 
            matrix[n][m] = 0.0; 
      }
  } 
  
}
