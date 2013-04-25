/* APPLE LOCAL begin radar 4152603 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-gstabs+ -gused -msse2" } */
/* LLVM LOCAL test not applicable (no stabs) */
/* { dg-require-fdump "" } */
/* Test that no header file information for xmmintrin.h is generated. */

#include "xmmintrin.h"

__m128  COMPUTE ()
{
	__m128 v0 = _mm_setzero_ps();
	__m128 v1 = _mm_set1_ps(1.0);
	__m128 v2 = _mm_set1_ps(1.0);;
	__m128 v3 = _mm_sub_ss (v1, v2);
	return _mm_add_ss (v3, v0);
}

int main()
{
	COMPUTE ();
	return 0;
}

/* { dg-final { scan-assembler-not "xmmintrin.h" } } */
/* APPLE LOCAL end radar 4152603 */
