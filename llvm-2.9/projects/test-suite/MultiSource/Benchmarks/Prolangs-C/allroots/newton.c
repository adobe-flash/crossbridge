#include "values.h"
#include "horners.h"
#include <stdio.h>


double newton (int N,double P[],double A,double B)
{
  double T_DOUBLE;    /* for temporary storage */
  double Xk,Xk1;      /* the kth and k+1rst quess at the root */
  int K = 0;          /* number of iterations so far */

/* make sure that A is lower bound of the interval and B is the upper bound */
  if (B < A) {
    T_DOUBLE = A;
    A = B;
    B = T_DOUBLE;
  }

  printf("     NEWTON Called on interval [%g,%g]\n",A,B);
  Xk  = A;
  Xk1 = (A + B)/2;     /* initial quess is the midpoint of the interval */

  while ( (d_abs(Xk1-Xk)/d_abs(Xk1) > Episolon_n) && K <= MAX_1 ) {
    printf("     X[%d] = %g\n",K+1,Xk1);
    Xk = Xk1;
    Xk1 = Xk1 - HORNERS(N,P,Xk1)/DERIV_X;
    K ++;
  }
  printf("ROOT: %g (approx.)\n\n",Xk1);
  return Xk1;
}

