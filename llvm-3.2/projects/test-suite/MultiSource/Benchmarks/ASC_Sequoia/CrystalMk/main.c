/*BHEADER****************************************************************
 * (c) 2007   The Regents of the University of California               *
 *                                                                      *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright       *
 * notice and disclaimer.                                               *
 *                                                                      *
 *EHEADER****************************************************************/


//--------------
//  A micro kernel 
//--------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "Crystal.h"


void init(double slipRate[MS_XTAL_NSLIP_MAX],
          double dSlipRate[MS_XTAL_NSLIP_MAX],
          double tau[MS_XTAL_NSLIP_MAX],
          double tauc[MS_XTAL_NSLIP_MAX],
          double rhs[MS_XTAL_NSLIP_MAX],
          double dtcdgd[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	  double dtdg[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	  double matrix[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX]);
double SPEdriver(double slipRate[MS_XTAL_NSLIP_MAX],
                 double dSlipRate[MS_XTAL_NSLIP_MAX],
                 double tau[MS_XTAL_NSLIP_MAX],
                 double tauc[MS_XTAL_NSLIP_MAX],
                 double rhs[MS_XTAL_NSLIP_MAX],
                 double dtcdgd[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	         double dtdg[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX],
	         double matrix[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX]);


int main()
{
  struct timeval  t0, t1;
  clock_t t0_cpu = 0,
          t1_cpu = 0;

  double slipRate[MS_XTAL_NSLIP_MAX];
  double dSlipRate[MS_XTAL_NSLIP_MAX];
  double tau[MS_XTAL_NSLIP_MAX];
  double tauc[MS_XTAL_NSLIP_MAX];
  double rhs[MS_XTAL_NSLIP_MAX];
  double dtcdgd[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX];
  double dtdg[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX];
  double matrix[MS_XTAL_NSLIP_MAX][MS_XTAL_NSLIP_MAX];

  double del_wtime = 0.0;
  double returnVal = 0.0;  

  int i = 0;
  int j = 0;

  printf ("\nSequoia benchmark version 1.0\n");

  init(slipRate, dSlipRate, tau, tauc, rhs, dtcdgd, dtdg, matrix);

  returnVal = SPEdriver(slipRate, dSlipRate, tau, tauc, rhs, dtcdgd, dtdg, matrix);

  printf("\n***** results \n");  

  printf("returnVal = %f \n", returnVal); 
  
  for (i=0; i<MS_XTAL_NSLIP_MAX; i+=MS_XTAL_NSLIP_MAX/3) {
     for (j=0; j<MS_XTAL_NSLIP_MAX; j+=MS_XTAL_NSLIP_MAX/3) {
         printf("i = %5d j = %5d    dtcdgd[i][j]   = %e \n", i,j,dtcdgd[i][j]);
     }
  }
  
  return  0;

}
