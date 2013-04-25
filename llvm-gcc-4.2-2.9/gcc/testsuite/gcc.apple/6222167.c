/* { dg-do compile { target i?86-apple-darwin* x86_64-apple-darwin* } } */
/* { dg-options "-msse4.1" } */
/* { dg-final { scan-assembler "pmaxsd" } } */
/* { dg-final { scan-assembler "pminsd" } } */
#include <stdio.h>
#include "smmintrin.h"
typedef int  int4 __attribute__ ((vector_size(16)));

int main( void )
{
  volatile int4  a = (int4) { 0, 1, 2, 3};
  volatile int4  b = (int4) { 2, 2, 2, 2};

  int4 max = __builtin_ia32_pmaxsd128( a, b );
  int4 min = __builtin_ia32_pminsd128( a, b );

  printf( "max: { %d, %d %d, %d}\n", ((int*) &max)[0], ((int*) &max)[1], ((int*) &max)[2], ((int*) &max)[3] );
  printf( "min: { %d, %d %d, %d}\n", ((int*) &min)[0], ((int*) &min)[1], ((int*) &min)[2], ((int*) &min)[3] );

  return 0;
}
