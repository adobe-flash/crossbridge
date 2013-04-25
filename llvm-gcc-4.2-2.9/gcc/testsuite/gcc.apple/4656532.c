/* APPLE LOCAL file 4656532 */
/* { dg-do compile { target "i?86-*-darwin*" } } */
/* { dg-options "-O2" } */
#include <mmintrin.h>
__m64 x, y;
void t1(int n) {
  y = _mm_slli_si64(x, n);
}
void t2(void) {
  y = _mm_slli_si64(x, 7);
}
void t3(int n) {
  y = _mm_srli_si64(x, n);
}
void t4() {
  y = _mm_srli_si64(x, 7);
}

/* { dg-final { scan-assembler-not "cltd" } } */
/* LLVM LOCAL remove scan-assembler-not test for optimizing MMX */
