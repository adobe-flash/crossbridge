/* APPLE LOCAL file radar 4469713 */
/* { dg-do run { target i?86-*-* } } */
/* { dg-options "-msse2" } */
#include <emmintrin.h>

extern void abort (void);

int extract0(__m128i *pv)
{
  return _mm_extract_epi16(*pv, 0);
}

int main()
{
  __m128i v = _mm_set1_epi16(0x80FF);

  if (extract0(&v) != 0x000080ff)
    abort();
  return 0;
}
