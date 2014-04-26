/* APPLE LOCAL file 4684674 */
/* { dg-do compile { target "i?86-*-darwin*" } } */
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
