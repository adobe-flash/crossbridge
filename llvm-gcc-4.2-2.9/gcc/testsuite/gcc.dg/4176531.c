/* APPLE LOCAL file radar 4176531 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-O2 -mfpmath=sse" } */
/* Runnable as a standalone test program.  Looks for SSE-based
   FP<->uint conversions.  Turn on optimize_size ("-Oz") to see x87
   sequences.  */
#include <stdio.h>
__attribute__ ((__noinline__))
d2ui32 (double x)
{
  printf("convert double to uint32: %f = %u\n", x, (unsigned int)x);
}
__attribute__ ((__noinline__))
f2ui32 (float x)
{
  printf("convert float to uint32: %f = %u\n", x, (unsigned int)x);
}
__attribute__ ((__noinline__))
ui64_2_d (long long unsigned int x)
{
  printf("convert uint64 to double: %llu = %f\n", x, (double)x);
}

main()
{
  d2ui32 (42.0);
  f2ui32 (42.0);
  ui64_2_d (42);
}
/* { dg-final { scan-assembler-not "fild" } } */
/* { dg-final { scan-assembler-not "fist" } } */
