/* APPLE LOCAL file 4113078 */
/* Check that stack alignment is correct when callee inlined function uses
   a vector and caller does not.  */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O2 -msse3 -march=pentium4 -march=prescott" } */
#include <xmmintrin.h>
extern void bar(__m128i*);
__m128i *global;
static void vfunc(void) {
  volatile __m128i xone = _mm_cvtsi32_si128(0x00010001);
  global = (__m128i*)&xone;
}
void baz() {
  int x;
  vfunc();
}
/* { dg-final { scan-assembler "\\-24\\(\\%ebp\\)" } } */
