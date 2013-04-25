/* APPLE LOCAL file AV data dependence */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-O2 -ftree-vectorize -fdump-tree-vect-stats -maltivec" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

#include <stdarg.h>
#include <signal.h>

#define N 64
#define MAX 42

extern void abort(void); 

int main ()
{  
  int A[N];
  int B[N];
  int C[N];
  int D[N];

  int i, j;

  for (i = 0; i < N; i++)
    {
      A[i] = i;
      B[i] = i;
      C[i] = i;
      D[i] = i;
    }

  /* Vectorizable */
  for (i = 0; i < 16; i++)
    {
      A[i+20] = A[i];
    }

  /* check results:  */
  for (i = 0; i < 16; i++)
    {
      if (A[i] != A[i+20])
	abort ();
    }

  return 0;
}



/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 1 "vect"  } } */
