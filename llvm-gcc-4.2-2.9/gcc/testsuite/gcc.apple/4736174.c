/* APPLE LOCAL file 4736174 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-O3 -msse2" } */
typedef double __v2df __attribute__ ((__vector_size__ (16)));
typedef __v2df __m128d;
__m128d _mm_cmpgt_sd (__m128d __A, __m128d __B)
{
  return (__m128d) __builtin_ia32_movsd ((__m128d) __A,
         (__m128d) __builtin_ia32_cmpltsd ((__m128d) __B,
         (__m128d) __A));
}

__m128d _mm_and_pd (__m128d __A, __m128d __B)
{
  return (__m128d)__builtin_ia32_andpd ((__m128d)__A, (__m128d)__B);
}

__m128d _mm_add_sd (__m128d __A, __m128d __B)
{
  return (__m128d)__builtin_ia32_addsd ((__m128d)__A, (__m128d)__B);
}

__m128d _mm_sub_sd (__m128d __A, __m128d __B)
{
  return (__m128d)__builtin_ia32_subsd ((__m128d)__A, (__m128d)__B);
}

__m128d _mm_mul_sd (__m128d __A, __m128d __B)
{
  return (__m128d)__builtin_ia32_mulsd ((__m128d)__A, (__m128d)__B);
}

__m128d _mm_storer_pd (double *__P, __m128d __A)
{
  _mm_store_pd (__P, __builtin_ia32_shufpd (__A, __A, (((0) << 1) | (1))));
}

__m128d _mm_add_pd (__m128d __A, __m128d __B)
{
  return (__m128d)__builtin_ia32_addpd ((__m128d)__A, (__m128d)__B);
}

__m128d _mm_sub_pd (__m128d __A, __m128d __B)
{
  return (__m128d)__builtin_ia32_subpd ((__m128d)__A, (__m128d)__B);
}

void foo ()
{
  __m128d t1r, t1i, t2r, t2i, t3r, t3i, t5r, ttp4x, carry4x, bj4x;
  __m128d MM_bigA, MM_bc[4], MM_c;
  double * px, pd1, pd2, pd3;
  int i, j, k, l, ll, UPDATE, pad2;
  for (i = 0, j = 0; j < pad2; j += UPDATE, i++)
    {
      for (k = 0; k < UPDATE; k += 4, px += 8)
        {
          { 
            __m128d maskj;
            int imaskj; 
            maskj = _mm_cmpgt_sd (bj4x, MM_c);
            maskj = _mm_and_pd (maskj, ttp4x);
            __asm__ volatile ("addsd %2, %0 \n" "        subsd %2, %0" : "=&x" (carry4x) : "0" (t5r), "X" (MM_bigA));
            ttp4x = _mm_add_sd (ttp4x, maskj);
            t5r = _mm_sub_sd (t5r, carry4x);
            t5r = _mm_mul_sd (t5r, ttp4x);
            bj4x = _mm_add_sd (bj4x, MM_bc[imaskj]);
          }
          { 
            __m128d _ar, _ai, _br, _bi;
            t1r = _mm_sub_pd (t1r, t3i);
            t1i = _mm_add_pd (t1i, t3r);
            _mm_store_pd (pd1 + ll, t1r);
            _mm_store_pd ((pd1 + ll + 2), t1i);
            t3r = _mm_add_pd (t2r, _ai);
            t3i = _mm_sub_pd (t2i, _ar);
            t2r = _mm_sub_pd (t2r, _ai);
            t2i = _mm_add_pd (t2i, _ar);
            _mm_store_pd (pd3 + ll, t3r);
            _mm_store_pd ((pd3 + ll + 2), t3i);
            _mm_store_pd (pd2 + ll, t2r);
            _mm_store_pd ((pd2 + ll + 2), t2i);
          }
        }
    }
}
