/* LLVM LOCAL file 6501843 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-O2 -mmmx -std=c99" } */
#include <emmintrin.h>

typedef float vFloat __attribute__ ((__vector_size__ (16)));

vFloat t(const float *A) {
  return _mm_loadu_ps(&A[4]);
}
/* { dg-final { scan-assembler-not "call" } } */
/* { dg-final { scan-assembler-not "mm_" } } */
