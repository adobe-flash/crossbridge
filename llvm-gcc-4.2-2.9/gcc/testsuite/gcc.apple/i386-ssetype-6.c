/* APPLE LOCAL file 4099020 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O2 -msse2 -march=pentium4" } */
/* { dg-final { scan-assembler-times "movq\[^\\n\]*" 3} } */

/* Verify that we generate proper instruction with memory operand.  */

#include <emmintrin.h>

__m128i
t1(__m128i *p)
{
  return _mm_loadl_epi64(p);	/* 64-bit, zero-extended result in %xmm. */
}
void
t2(__m128i *p, __m128i a)
{
  _mm_storel_epi64(p, a);	/* 64-bit store from %xmm.  */
}
__m128i
t3(__m128i a)
{
  return _mm_move_epi64(a);	/* 64-bit move between %xmm registers.  */
}
