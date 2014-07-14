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
//  test FDIV
//-------------- 
void Crystal_div(int nSlip,  
         double deltaTime,
         double slipRate[MS_XTAL_NSLIP_MAX],
         double dSlipRate[MS_XTAL_NSLIP_MAX],
         double tau[MS_XTAL_NSLIP_MAX],
         double tauc[MS_XTAL_NSLIP_MAX],
         double rhs[MS_XTAL_NSLIP_MAX],
         double dtcdgd[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	 double dtdg[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	 double matrix[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX])

{
   double bor_array[MS_XTAL_NSLIP_MAX];
   double sgn[MS_XTAL_NSLIP_MAX];
   double rateFact[MS_XTAL_NSLIP_MAX];
   double tauN[MS_XTAL_NSLIP_MAX];
   double err[MS_XTAL_NSLIP_MAX];

   double rate_offset = 1.e-6;
   double tauA        = 30.;
   double tauH        = 1.2;
   double rate_exp    = 0.01;
   double bor_s_tmp   = 0.0;

   int n = 0;
   int m = 0;

   for ( n = 0; n < nSlip; n++){
     sgn[n] = 1.0;
     rateFact[n] = 0.9 + (0.2 * n) / MS_XTAL_NSLIP_MAX;
   }

//----MS_Xtal_PowerTay
   for ( n = 0; n < nSlip; n++){
        bor_array[n] = 1 / ( slipRate[n]*sgn[n] + rate_offset);
   }

   for ( n = 0; n < nSlip; n++){
        tau[n] = tauA * rateFact[n] * sgn[n];
        for ( m = 0; m < nSlip; m++)
           dtcdgd[n][m] = tauH * deltaTime * rateFact[n];
        dtcdgd[n][n] += tau[n] * rate_exp * sgn[n] * bor_array[n];
   }  
 
//-----MS_Xtal_SlipRateCalc  
   for (n = 0; n < nSlip; n++) {
        bor_array[n] = 1/dtcdgd[n][n];
   }

   for (n = 0; n < nSlip; n++){
        tauN[n] = tau[n];
        for (m = 0; m < nSlip; m++){
	    bor_s_tmp = dtdg[n][m]* deltaTime;
            tauN[n] +=  bor_s_tmp * dSlipRate[m] ;
            matrix[n][m] = (-bor_s_tmp + dtcdgd[n][m])*bor_array[n];
        }
        err[n] = tauN[n] - tauc[n];
        rhs[n] = err[n] * bor_array[n];
 
   }

}
