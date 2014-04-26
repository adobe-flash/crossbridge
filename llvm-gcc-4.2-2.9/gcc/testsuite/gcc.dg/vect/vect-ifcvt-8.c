/* APPLE LOCAL file AV data dependence */
/* { dg-do run } */
/* { dg-options "-O2 -ftree-vectorize -fdump-tree-vect-stats -maltivec" { target powerpc*-*-* } } */


#include <stdarg.h>
#include <signal.h>

#define N 16
#define MAX 42

extern void abort(void); 

int main ()
{  
  unsigned short A[N] = {36,39,42,45,43,32,21,12,23,34,45,56,67,78,89,11};
  unsigned short B[N] = {0,0,0,3,1,0,0,0,0,0,3,14,25,36,47,0};
  unsigned int i, j;
  unsigned m;

  for (i = 0; i < 16; i++)
    {
      m = A[i];
      A[i] =  (unsigned short) ( m >= MAX ? m-MAX : 0); 
    }

  /* check results:  */
  for (i = 0; i < N; i++)
    if (A[i] != B[i])
      abort ();

  return 0;
}



/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 1 "vect" { xfail *-*-darwin* i?86-*-* x86_64-*-* } } } */
