/* APPLE LOCAL file inline fixups 5580320 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "" } */

#include <emmintrin.h>

extern inline __m128i vect_add(__m128i a, __m128i b) {
  return _mm_add_epi8(a, b);
}
