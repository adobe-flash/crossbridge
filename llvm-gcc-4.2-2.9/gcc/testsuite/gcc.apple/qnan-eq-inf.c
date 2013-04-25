/* APPLE LOCAL file 5752613 */
/* { dg-options "-std=c99" } */
/* { dg-do run { target arm-*-darwin* } } */
#include <stdio.h>
#include <stdlib.h>
#include <fenv.h>

int main (int argc, const char *argv[])
{
  volatile union{ double d; unsigned long long u; }u;
  volatile int fred;
  unsigned invalid;

  feclearexcept (FE_ALL_EXCEPT);

  /* Create a QNaN */
  u.u = 0xffffffffffffffffULL;

  /* Comparing to inf should not set 'invalid'. testing > DBL_MAX will.
     testing == inf will not. */
  fred = (u.d == __builtin_inf()) ;
  
  invalid = fetestexcept (FE_INVALID);
  if (invalid)
    abort();
  exit (0);
}
