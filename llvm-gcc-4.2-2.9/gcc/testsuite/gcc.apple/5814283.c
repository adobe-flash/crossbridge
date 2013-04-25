/* APPLE LOCAL file 5814283 */
/* { dg-do compile { target "i?86-*-darwin*" } } */
/* { dg-options "-O2 -mssse3" } */
#include <emmintrin.h>
#include <tmmintrin.h>
extern int i1, i2;
__m128i b, c, d[10];
__m128d dd[5];
nonsense (void)
{
  /* These are all macros.  Insure their parameters are parenthesized
     at expansion.  */
  dd[0] = _mm_shuffle_pd(i1 ? b : c, i2 ? b : c, 0);
  d[1] = _mm_shufflehi_epi16(i1 ? b : c, 0);
  d[2] = _mm_shufflelo_epi16(i1 ? b : c, 0);
  d[3] = _mm_shuffle_epi32(i1 ? b : c, 0);
  d[4] = _mm_alignr_epi8(i1 ? b : c, i2 ? b : c, 0);
}
