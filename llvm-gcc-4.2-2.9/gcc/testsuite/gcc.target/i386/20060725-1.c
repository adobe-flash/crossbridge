/* APPLE LOCAL file 4643646 */
/* { dg-do compile } */
/* { dg-options "-O2 -msse3 -S" } */
/* { dg-final { scan-assembler-not "%mm" } } */
//gcc -S -O2 -msse3 -c ggg.c 
#include <pmmintrin.h>
__m128 ggg(float* m)
{
  return (__m128) {m[0], m[5], m[10], m[10]};
}
