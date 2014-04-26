/* APPLE LOCAL file 4253848 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-O2 -msse2" } */
#include <xmmintrin.h>
__m128i foo(__m128i x, __m128i y) {
  y = _mm_add_epi16( x, y );
  return y;
}
__m128i foo2(__m128i x, __m128i y) {
  y = _mm_add_epi16( y, x );
  return y;
}
/* { dg-final { scan-assembler-not "movdqa\t%xmm\[0-7\], %xmm\[0-7\]" } } */
