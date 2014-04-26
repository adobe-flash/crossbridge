/* APPLE LOCAL file 4505813 */
/* { dg-do compile { target i?86-*-* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-msse2" } */

#include <xmmintrin.h>

const __m64 one __attribute__ ((aligned(16))) = { 0x0001000100010001LL };
/* { dg-final { scan-assembler-times "long\t65537" 2 } } */
