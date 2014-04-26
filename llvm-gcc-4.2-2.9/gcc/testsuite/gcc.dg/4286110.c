/* APPLE LOCAL begin radar 4286110 */
/* { dg-do run { target "i?86-*-darwin*" } } */
/* { dg-options "-O3 -msse2" } */
extern void abort();
#include <emmintrin.h>
__m128i foo (short) __attribute__((noinline));
__m128i foo (short x) {
  return _mm_set1_epi16(x);
}
__m128i bar (short) __attribute__((noinline));
__m128i bar (short x) {
  return _mm_set_epi16 (x,x,x,x,x,x,x,x);
}

main() {
  int i, j;
  union u { __m128i v; short c[8]; };
  union u x, y;
  for (i = -128; i <= 127; i++)
    {
      x.v = foo ((short)i);
      y.v = bar ((short)i);
      for (j=0; j<8; j++)
	if (x.c[j] != y.c[j])
	  abort();
    }
  return 0;
}
/* APPLE LOCAL end radar 4286110 */
