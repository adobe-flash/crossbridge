/* APPLE LOCAL file 5951842 */
/* { dg-do compile { target "i?86-*-darwin*" } } */
/* { dg-options "-O1" } */
  /* Should generate one SSE2 insn; under -O1 -m64, was shifting in rdx:rax.  */
/* { dg-final { scan-assembler "psrldq" } } */
#include <emmintrin.h>
__m128i
f (__m128i v_row)
{
  /* This is a 40-bit left shift.  */
  return _mm_srli_si128(v_row, 5);
}
