/* APPLE LOCAL file 4525731 */
/* { dg-do run { target i?86-*-* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O3" } */

#include <stdlib.h>

static void __attribute__ ((__force_align_arg_pointer__, __noinline__, __regparm__(2)))
callee (int i1, int i2, int i3, int i4, int i5, int i6, int i7)
{
  if (i1 != 1)
    abort ();
  if (i2 != 2)
    abort ();
  if (i3 != 3)
    abort ();
  if (i4 != 4)
    abort ();
  if (i5 != 5)
    abort ();
  if (i6 != 6)
    abort ();
  if (i7 != 7)
    abort ();
}

main()
{
  callee (1, 2, 3, 4, 5, 6, 7);
  return 0;
}
