#include <stdio.h>
#include <stdlib.h>    
#include <math.h>
#include "newton.h"
#include "horners.h"
#include "values.h"

extern void allroots(int No,double Po[],int N,double Pn[]);
extern void deflat(int No,double Po[],int N,double Pn[],double ROOT); 

int main(void) {
  static double A[] = {4.1,-3.9,-1.0,1.0};
  int N = 3;
  int J;
  printf("DEBUG: %g %g\n",2.69065*2.69065*2.69065,2.69065*2.69065);
  printf("==============================================================\n");
  printf("Find all roots of\n");

  for (J=N;J>0;J--) {
    printf("%g",d_abs(A[J]));
    if (A[J-1] < 0)
      printf("x**%d - ",J);
    else
      printf("x**%d + ",J);
  }

  printf("%g\n",d_abs(A[0]));
  printf("using NEWTON method.\n");
  printf("==============================================================\n");
  allroots(N,A,N,A);
  return 0;
}

void allroots(int No,double Po[],int N,double Pn[])

/* Computes the Maximum interval that all the roots for the polynomial P
   can contain with |root| < |P[0]| + |P[1]| + ... + |P[n]| \ |P[n]| where
   P[i] is the coefficent of the Ith degree of the polynomial

   Next it looks for a change of sign of F(x) on the range, when it finds one
   it calls a METHOD for finding an individual root and then repeats the
   process with the range now starting just after the last found root */

{
  int I;              /* counter */
  double ROOT;

  double LOWER,UPPER; /* lower and upperbound of all roots of P */
  

  UPPER = 0;
  for (I=0;I<=N;I++)
    UPPER += d_abs(Pn[I]);

  UPPER /= d_abs(Pn[N]);
  LOWER = -UPPER - 1.0;

  if (N == 0)
    printf("No roots\n");
  else
    if (N == 1) {
      ROOT = -Pn[0]/Pn[1];
      printf("   ROOT = %g\n",ROOT);
    }
    else
      if (N == 2) {
	ROOT = (-Pn[1] + sqrt(Pn[1]*Pn[1] - 4*Pn[2]*Pn[0]))/(2*Pn[2]);
	printf("  ROOT = %g (from quadratic formula)\n",ROOT);
	ROOT = (-Pn[1] - sqrt(Pn[1]*Pn[1] - 4*Pn[2]*Pn[0]))/(2*Pn[2]);
	printf("  ROOT = %g (from quadratic formula)\n",ROOT);
      }
      else {
	ROOT = newton(N,Pn,LOWER,UPPER);
	deflat(No,Po,N,Pn,ROOT);
      }
}


void deflat(int No,double Po[],int N,double Pn[],double ROOT)
{
  double *TP;
  int I,J;

  if (N != No) {
    printf("----> Refine Root on the Orginal Polynomial (non-deflated)\n");
    newton(No,Po,ROOT-.5,ROOT+.5);
  }

  TP = (double *) calloc(N,sizeof(ROOT));

  TP[N-1]=Pn[N];
  for (I=N-2;I>=0;I--) 
    TP[I] = TP[I+1]*ROOT+Pn[I+1];

  for (J=N;J>0;J--) {
    printf("%g",d_abs(Pn[J]));
    if (Pn[J-1] < 0)
      printf("x**%d - ",J);
    else
      printf("x**%d + ",J);
  }

  printf("%g\n",d_abs(Pn[0]));
  printf("     DEFLATED to\n(x - %g)*(",ROOT);

  for (J=N-1;J>0;J--) {
    printf("%g",d_abs(TP[J]));
    if (TP[J-1] < 0)
      printf("x**%d - ",J);
    else
      printf("x**%d + ",J);
  }

  printf("%g)\n",d_abs(TP[0]));

  if (N == 3) {
    ROOT = (-TP[1] + sqrt(TP[1]*TP[1] - 4*TP[2]*TP[0]))/(2*TP[2]);
    printf("\n  ROOT = %g (from quadratic formula)\n",ROOT);
    printf("----> Refine Root on the Orginal Polynomial (non-deflated)\n");
    newton(No,Po,ROOT-.5,ROOT+.5);

    ROOT = (-TP[1] - sqrt(TP[1]*TP[1] - 4*TP[2]*TP[0]))/(2*TP[2]);
    printf("  ROOT = %g (from quadratic formula)\n",ROOT);
    printf("----> Refine Root on the Orginal Polynomial (non-deflated)\n");
    newton(No,Po,ROOT-.5,ROOT+.5);
  }
  else {
    allroots(No,Po,N-1,TP);
  }
  free(TP);
}
