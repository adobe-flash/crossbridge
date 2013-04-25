/* APPLE LOCAL file 4283414 mainline candidate */
/* Generate -1 with pcmpeq for SSE2 (x86) */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O2 -msse2" } */

typedef long long __v2di __attribute__ ((__vector_size__ (16)));
typedef int __v4si __attribute__ ((__vector_size__ (16)));
typedef __v2di __m128i;

static __inline __m128i __attribute__((__always_inline__, __nodebug__))
_mm_set_epi32 (int __q3, int __q2, int __q1, int __q0)
{
  return __extension__ (__m128i)(__v4si){ __q0, __q1, __q2, __q3 };
}

static __inline __m128i __attribute__((__always_inline__, __nodebug__))
_mm_set1_epi32 (int __A)
{
  return _mm_set_epi32 (__A, __A, __A, __A);
}

__m128i set1_32bit() {
        return _mm_set1_epi32(-1);
}
/* { dg-final { scan-assembler "pcmpeq" } } */
