/*BHEADER****************************************************************
 * (c) 2007   The Regents of the University of California               *
 *                                                                      *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright       *
 * notice and disclaimer.                                               *
 *                                                                      *
 *EHEADER****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "Crystal.h"

void Crystal_div(int nSlip,  
                double deltaTime,
                double slipRate[MS_XTAL_NSLIP_MAX],
                double dSlipRate[MS_XTAL_NSLIP_MAX],
                double tau[MS_XTAL_NSLIP_MAX],
                double tauc[MS_XTAL_NSLIP_MAX],
                double rhs[MS_XTAL_NSLIP_MAX],
                double dtcdgd[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	        double dtdg[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	        double matrix[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX]);

double Crystal_pow(int nSlip,  
		  double slipRate[MS_XTAL_NSLIP_MAX]);


void Crystal_Cholesky(int nSlip,  
                     double a[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
                     double r[MS_XTAL_NSLIP_MAX],
                     double g[MS_XTAL_NSLIP_MAX]);

//-------------- 
//  SPE driver
//-------------- 
double SPEdriver(double slipRate[MS_XTAL_NSLIP_MAX],
                 double dSlipRate[MS_XTAL_NSLIP_MAX],
                 double tau[MS_XTAL_NSLIP_MAX],
                 double tauc[MS_XTAL_NSLIP_MAX],
                 double rhs[MS_XTAL_NSLIP_MAX],
                 double dtcdgd[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	         double dtdg[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	         double matrix[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX])
{
  struct timeval  t0, t1;
  clock_t t0_cpu = 0,
          t1_cpu = 0;

  double del_wtime = 0.0;
  double tmp;

#ifdef SMALL_PROBLEM_SIZE
  const int noIter = 200000;
#else
  const int noIter = 2000000;
#endif
  int i = 0;
  int j, k;


  gettimeofday(&t0, ((void *)0));
  t0_cpu = clock();
  for (i=0; i<noIter; ++i) {
      Crystal_div(MS_XTAL_NSLIP_MAX,  
                 0.01,
                 slipRate,
                 dSlipRate,
                 tau,
                 tauc,
                 rhs,
                 dtcdgd,
                 dtdg,
                 matrix);
  }

  for (i=0; i<noIter; ++i) {
       tmp = Crystal_pow(MS_XTAL_NSLIP_MAX,
	     slipRate);
  }

  for (i=0; i<MS_XTAL_NSLIP_MAX; i++){
    for (j=0; j<MS_XTAL_NSLIP_MAX; j++)
      matrix[i][j] = dtcdgd[i][j];
  }

  for (i=0; i<noIter; ++i) { 
       for (j=0; j<MS_XTAL_NSLIP_MAX; j++){
         for (k=0; k<MS_XTAL_NSLIP_MAX; k++)
           dtcdgd[j][k] = matrix[j][k];
       }
       Crystal_Cholesky(MS_XTAL_NSLIP_MAX,  
                       dtcdgd,
                       tau,
                       rhs);
  }
  return tmp;

}
