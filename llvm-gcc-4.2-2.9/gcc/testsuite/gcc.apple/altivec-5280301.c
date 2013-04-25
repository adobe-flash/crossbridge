/* APPLE LOCAL file 5280301 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-m64 -O3 -faltivec -std=c99" } */
typedef unsigned int uint32_t;
typedef uint32_t vuint32 __attribute__((__vector_size__(16)));

typedef union
{
  unsigned long long ul[2];
  vuint32 v;
} vec;

extern void abort ();

static int foo (vec *a, vec *b)
{
  for (int i = 0; i < 2; ++i)
  if (a->ul[i] != b->ul[i])
    return 1;
  return 0;
}

int main (void)
{
  vec t = { 0 };
  vec zero, one, signbit, test;
  int i;
  zero.ul[0] = 0;
  zero.ul[1] = 0;
  one.v = zero.v;
  one.ul[1] = 1;
  signbit.v = zero.v;
  signbit.ul[0] = 0;
  test.v = t.v & signbit.v;
  if (foo(&test, &zero))
    ;

  vec p = one;
  for (i = 0; i < 16; ++i)
  {
    if ((i < 15 && i[(unsigned char *) &p] != 0)
         || (i == 15 && i[(unsigned char *) &p] != 1))
      abort ();
  }
  return 0;
}
