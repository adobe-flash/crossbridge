/* APPLE LOCAL file 4278461 */
/* { dg-do run { target "i?86-*-darwin*" } } */
/* { dg-options "-O2 -fPIC -msse2" } */
/* Radar 4278461, GCC used a pic-base it neglected to set.  */
#include <stdio.h>
#include <emmintrin.h>
double __attribute__ ((noinline))
xsqrt( double x )
{
  __v2df f, g;
  double _d;
  x += x;
  g = __extension__ (__v2df){ x, 0 };
  f = _mm_sqrt_pd( g );
  _d = _mm_cvtsd_f64 (f);
  return (_d);
}

double global_x, global_y;

main ()
{
  global_x = 100.0;
  printf ("", &global_x, &global_y);	/* Frighten the optimizer.  */
  global_y = xsqrt (global_x / 2.0);	/* Compensate for the "x += x;" in xsqrt().  */
  if (global_y != 10.0)
    abort ();
  exit (0);
}
