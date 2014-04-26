/* APPLE LOCAL file 5543378 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-skip-if "" { i?86-*-* } { "" } { "" } } */
/* { dg-options "-msse2" } */
#include <xmmintrin.h>
void foo1 (int n)
{
  __m128i a = _mm_slli_epi32 (a, n);
}

void foo2 (int n) {
  __m128i a = _mm_srli_epi32 (a, n);
}

void foo3 (int n) {
  __m128i a = _mm_srai_epi32 (a, n);
}
