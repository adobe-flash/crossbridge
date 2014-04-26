/* APPLE LOCAL file 5533365 */
/* { dg-do compile { target i?86*-*-darwin* } } */
/* { dg-options "-O2" } */
typedef long long __m64 __attribute__ ((__vector_size__ (8)));
typedef float __m128 __attribute__ ((__vector_size__ (16)));
typedef double __v2df __attribute__ ((__vector_size__ (16)));
typedef long long __v2di __attribute__ ((__vector_size__ (16)));
typedef __v2di __m128i;
typedef __v2df __m128d;

static inline __m128d __attribute__ ((__always_inline__, __nodebug__))
_mm_min_pd (__m128d __A, __m128d __B)
{
  return (__m128d)__builtin_ia32_minpd ((__v2df)__A, (__v2df)__B);
}

static inline __m128d __attribute__ ((__always_inline__, __nodebug__))
_mm_move_sd (__m128d __A, __m128d __B)
{
  return (__m128d) __builtin_ia32_movsd ((__v2df)__A, (__v2df)__B);
}

static inline __m128i __attribute__ ((__always_inline__, __nodebug__))
_mm_cvtpd_epi32 (__m128d __A)
{
  return (__m128i)__builtin_ia32_cvtpd2dq ((__v2df) __A);
}

union __attribute__((aligned(16))) XMM128
{
    __m128d d;
    __m128i i;
};

__inline__ __m128i foo (const __m128d &srcLo, const __m128i &src)
{
  const static __m128d max_val = {(double)0x7FFFFFFF, (double)0x7FFFFFFF};
  __m128d lo = _mm_min_pd(srcLo, max_val);
  XMM128 Lo,s1;
  Lo.i= _mm_cvtpd_epi32(lo);
  s1.i = src;
  Lo.d = _mm_move_sd(s1.d,Lo.d);
  return Lo.i;
}

int main()
{
  XMM128 tmp, tmp1, tmp2;
  tmp.i = foo (tmp1.d, tmp2.i);
  return 0;
}
