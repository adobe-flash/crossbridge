/* APPLE LOCAL begin radar 4266982 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-O2 -msse" } */

typedef double __v2df __attribute__ ((__vector_size__ (16)));
typedef long long __v2di __attribute__ ((__vector_size__ (16)));
typedef int __v4si __attribute__ ((__vector_size__ (16)));
typedef short __v8hi __attribute__ ((__vector_size__ (16)));
typedef char __v16qi __attribute__ ((__vector_size__ (16)));

typedef __v2di __m128i;
typedef __v2df __m128d;

static inline __m128i __attribute__((__always_inline__, __nodebug__))
_mm_packus_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_packuswb128 ((__v8hi)__A, (__v8hi)__B);
}


static inline __m128i __attribute__((__always_inline__, __nodebug__))
_mm_load_si128 (__m128i const *__P)
{
  return *__P;
}

static inline __m128i __attribute__((__always_inline__, __nodebug__))
_mm_or_si128 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_por128 ((__v2di)__A, (__v2di)__B);
}
static inline __m128i __attribute__((__always_inline__, __nodebug__))
_mm_add_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_paddw128 ((__v8hi)__A, (__v8hi)__B);
}
static inline int __attribute__((__always_inline__, __nodebug__))
_mm_cvtsi128_si32 (__m128i __A)
{
  return __builtin_ia32_vec_ext_v4si ((__v4si)__A, 0);
}
void FUNC( unsigned char const* pixPtr)
{

 __m128i vu16YPredictionHi, vu16YPredictionLo;

  __m128i vu16YPrediction3;
  __m128i vtemp0, vtemp1, vtemp2, vtemp3;

  vtemp2 = ((__m128i)__builtin_ia32_pslldqi128 (vu16YPrediction3, (4) * 8));
  vtemp2 = ((__m128i)__builtin_ia32_psrldqi128 (vtemp2, (8) * 8));
  vtemp3 = ((__m128i)__builtin_ia32_psrldqi128 (vu16YPrediction3, (6) * 8));
  vtemp3 = ((__m128i)__builtin_ia32_pslldqi128 (vtemp3, (8) * 8));

  vu16YPredictionLo = _mm_or_si128(vtemp2, vtemp3);

 __m128i vs16YResidualHi, vs16YResidualLo, vs16YResultHi, vs16YResultLo;
 __m128i vu8YResult;
 unsigned int itemp0, itemp1, itemp2, itemp3;
 vs16YResultHi = _mm_add_epi16(vu16YPredictionHi, vs16YResidualHi);
 vs16YResultLo = _mm_add_epi16(vu16YPredictionLo, vs16YResidualLo);
 vu8YResult = _mm_packus_epi16(vs16YResultHi, vs16YResultLo);
 itemp0 = _mm_cvtsi128_si32(vu8YResult);
 vu8YResult = ((__m128i)__builtin_ia32_psrldqi128 (vu8YResult, (4) * 8));
 itemp1 = _mm_cvtsi128_si32(vu8YResult);
 vu8YResult = ((__m128i)__builtin_ia32_psrldqi128 (vu8YResult, (4) * 8));
 itemp2 = _mm_cvtsi128_si32(vu8YResult);
 vu8YResult = ((__m128i)__builtin_ia32_psrldqi128 (vu8YResult, (4) * 8));
 itemp3 = _mm_cvtsi128_si32(vu8YResult);
  *(unsigned int*)pixPtr = itemp0;
}
/* APPLE LOCAL end radar 4266982 */
