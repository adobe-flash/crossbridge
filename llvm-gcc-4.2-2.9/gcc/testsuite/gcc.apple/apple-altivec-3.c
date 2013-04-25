/* APPLE LOCAL file AltiVec */
/* Providing '-maltivec' should ensure that AltiVec codegen for block copies is enabled,
   even if '-faltivec' is also specified.  */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec -maltivec" } */
typedef struct { vector int a; vector int b; } Scld1;

void testvaScld1 (int n, Scld1);

void foo ()
{
  Scld1 g1sScld1;
  testvaScld1 (1, g1sScld1);  /* { dg-bogus "disabled" } */
}
