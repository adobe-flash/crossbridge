/* APPLE LOCAL file 6440204 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-O2 -m64" } */
/* { dg-final { scan-assembler "pslldq" } } */
#include <xmmintrin.h>
const short rgb[64] = {
    0x00ff, 0x01ff, 0x02ff, 0x03ff,
    0x04ff, 0x05ff, 0x06ff, 0x07ff
};
__m128i
shuf(__m128i __vparm)
{
  __m128i __va;
  __va = _mm_slli_si128(__vparm, 2);
  return __va;
}

