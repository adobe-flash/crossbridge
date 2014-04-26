/* { dg-do run } */
/* { dg-options { -ffast-math -O2 } } */
#include <stdlib.h>
#include <math.h>
double globx;
main ()
{
  if (finite(globx) != 1)
    abort ();
  return 0;
}
