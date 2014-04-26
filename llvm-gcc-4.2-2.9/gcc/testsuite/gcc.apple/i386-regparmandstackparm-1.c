/* APPLE LOCAL file 420513 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O2" } */

/* Look for both entry points to the regparmandstackparm function.  */
/* { dg-final { scan-assembler "_?munge_fast:" } } */
/* { dg-final { scan-assembler "_?munge_fast\\\$3SSE:" } } */

/* All calls to this function should use the fast version.  */
/* { dg-final { scan-assembler-not "^\tcall\[	 \]*_?munge_fast\[^$\]" } } */
/* { dg-final { scan-assembler "call\[	 \]*_?munge_fast\\\$3SSE" } } */

/* { dg-final { scan-assembler-not "munge_regular\\\$3SSE" } } */

#include <stdlib.h>

#define RANDSP __attribute__ ((regparmandstackparm, noinline))

double RANDSP
munge_fast (double a, double b)
{
  return a + b;
}

double
munge_regular (double a, double b)
{
  return a + b;
}

double v1 = 31, v2 = 11;

int
main ()
{
  double c_fast, c_regular, difference;

  c_fast = munge_fast (v1, v2);
  c_regular = munge_regular (v1, v2);

  difference = abs (c_fast - 42.0);

  if (difference > 1e-50)
    abort();

  difference = abs (c_fast - c_regular);

  if (difference > 1e-50)
    abort();

  return 0;
}
