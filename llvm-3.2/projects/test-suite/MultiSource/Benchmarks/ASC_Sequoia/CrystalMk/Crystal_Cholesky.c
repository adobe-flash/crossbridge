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
//  test Cholesky solver on matrix
//-------------- 
void Crystal_Cholesky(int nSlip,  
                      double a[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
                      double r[MS_XTAL_NSLIP_MAX],
                      double g[MS_XTAL_NSLIP_MAX])
{
   int i, j, k;
   double fdot;

   
      /* transfer rhs to solution vector to preserve rhs */
   for ( i = 0; i < nSlip; i++) g[i] = r[i];
      
      /* matrix reduction */
   for ( i = 1; i < nSlip; i++)
      a[i][0] = a[i][0] / a[0][0];


   for ( i = 1; i < nSlip; i++){
      fdot = 0.0;
      for ( k = 0; k < i; k++)
         fdot += a[i][k] * a[k][i];
      a[i][i] = a[i][i] - fdot;
      for ( j = i+1; j < nSlip; j++){
         fdot = 0.0;
         for ( k = 0; k < i; k++)
            fdot += a[i][k] * a[k][j];
         a[i][j] = a[i][j] - fdot;
         fdot = 0.0;
         for ( k = 0; k < i; k++)
            fdot += a[j][k] * a[k][i];
         a[j][i] = ( a[j][i] - fdot) / a[i][i];
      }
   }

   
      /* forward reduction of RHS */
   for ( i = 1; i < nSlip; i++ ){
      for ( k = 0; k < i; k++)
         g[i] = g[i] - a[i][k] * g[k];
   } 
   
      /* back substitution */
   g[nSlip-1] = g[nSlip-1] / a[nSlip-1][nSlip-1];
   for ( i = nSlip - 2; i >= 0; i=i-1){
      for ( k = i+1; k < nSlip; k++)
         g[i] = g[i] - a[i][k]*g[k];
      g[i] = g[i] / a[i][i];
   }
}



