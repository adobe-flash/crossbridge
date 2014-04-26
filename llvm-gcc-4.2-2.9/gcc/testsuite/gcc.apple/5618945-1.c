/* APPLE LOCAL file radar 5618945 */
/* { dg-do compile { target "i?86-*-darwin*" } } */
/* { dg-options "-mssse3 -std=gnu89" } */
#include <tmmintrin.h>
inline __m128i foo1 (short x) {
  return _mm_set1_epi16(x);
}

inline __m64 foo2 (char x) {
  return _mm_set1_pi8 (x);
}

inline __m128 foo3 (float x) {
  return _mm_set_ps1 (x);
}

inline __m128 foo4 (__m128 x, __m128 y) {
  return _mm_addsub_ps (x, y);
}

inline __m128i foo5 (__m128i x, __m128i y) {
  return _mm_hadd_epi32 (x, y);
}
