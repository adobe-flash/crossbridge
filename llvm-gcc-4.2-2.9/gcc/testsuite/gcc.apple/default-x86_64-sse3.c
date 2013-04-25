/* APPLE LOCAL file 4515157 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-m64" } */
/* Insure that -m64 implies -msse3 on Darwin/x86.  */
#ifndef __SSE3__
#error "expected -m64 to enable -msse3 by default"
#endif
#include <pmmintrin.h>
/* LLVM LOCAL pmmintrin no longer declares exit */
#include <stdlib.h>
main ()
{
  static __m128 x, y, z;
  x = _mm_hadd_ps (y, z);	/* An SSE3 intrinsic.  */
  exit (0);
}
