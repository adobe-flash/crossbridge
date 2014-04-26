/* APPLE LOCAL file 6382081 */
/* { dg-do compile { target i?86-apple-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-m32 -std=c99" } */
/* { dg-final { scan-assembler-not "fldll" } } */
float foo(long long *p)
{
  return *p;
}
