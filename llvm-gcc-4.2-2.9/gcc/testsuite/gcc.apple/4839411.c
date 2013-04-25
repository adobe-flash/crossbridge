/* APPLE LOCAL file */
/* { dg-do compile { target "i?86-*-* x86_64-*-*" } } */
/* { dg-options "-mssse3" } */
#include <tmmintrin.h>

__m128i foo (__m128i a)
{
  __m128i tmp = a;

  return _mm_alignr_epi8(tmp, a, 4);
}
