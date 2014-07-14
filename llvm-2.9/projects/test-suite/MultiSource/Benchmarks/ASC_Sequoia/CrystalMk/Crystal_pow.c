/*BHEADER****************************************************************
 * (c) 2007   The Regents of the University of California               *
 *                                                                      *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright       *
 * notice and disclaimer.                                               *
 *                                                                      *
 *EHEADER****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Crystal.h"

//-------------- 
//  test POW
//-------------- 
double Crystal_pow(int nSlip,  
		double slipRate[MS_XTAL_NSLIP_MAX])

{
   double rateFact[MS_XTAL_NSLIP_MAX];
   double sgn[MS_XTAL_NSLIP_MAX];

   double muNew       = 60000.;
   double muRef       = 50000.;
   double rate_offset = 1.e-6;
   double rate_norm   = 2.0;
   double rate_exp    = 0.01;

   int n = 0;

   for ( n = 0; n < nSlip; n++){
     sgn[n] = 1.0;
     rateFact[n] = 0.9 * (0.2 * n) / MS_XTAL_NSLIP_MAX;
   }

   for ( n = 0; n < nSlip; n++){
      rateFact[n] = (muNew / muRef) * 
         pow((slipRate[n]*sgn[n]+rate_offset)/rate_norm, rate_exp);
   }

   return rateFact[3];

}
