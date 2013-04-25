/* APPLE LOCAL file 5919583 */
/* { dg-do run { target i?86*-*-* } } */
/* { dg-options { -msse3 } } */
#include <stdio.h>
#include <emmintrin.h>

typedef union {
  char ary[16];
  __m128i vec;
} vectype;

vectype ref;

int __attribute__ ((__noinline__)) scare (vectype);
int __attribute__ ((__noinline__))
scare (vectype vec)
{
  int i;
  for (i=0; i<16; i++)
    if (vec.ary[i] != ref.ary[i])
      return -1;	/* fail */
  return 0;	/* pass */
}

int main( void )
{
  vectype v;

  ref.vec = (const __m128i) { 0x32100123456789ABULL, 0xFEDCBA987654ULL };
  v.vec = _mm_srli_si128( (const __m128i) { 0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL }, 2 );
  /*
  {
    int i;
    for( i = 0; i < 16; i++ )
      printf( "%2.2x", ((unsigned char*) &v)[i] );
  }
  printf ("scare = %d\n", scare(v));
  */

  return scare (v);
}
