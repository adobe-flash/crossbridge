/* APPLE LOCAL file 4412214, 4172200 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-O3 -msse2" } */
/* { dg-final { scan-assembler-not "\tmovl\t-12\\\(%ebp\\\)" } } */
typedef long long __v2di __attribute__ ((__vector_size__ (16)));
typedef __v2di __m128i;
typedef int __v4si __attribute__ ((__vector_size__ (16)));
int _mm_cvtsi128_si32 (__m128i __A)
{
  return __builtin_ia32_vec_ext_v4si ((__v4si)__A, 0);
}

__m128i _mm_or_si128 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_por128 ((__v2di)__A, (__v2di)__B);
}

int foo (void )
{
    __m128i result, overflowResult;
    result = _mm_or_si128 (result, overflowResult);
    return _mm_cvtsi128_si32 (result );
}

