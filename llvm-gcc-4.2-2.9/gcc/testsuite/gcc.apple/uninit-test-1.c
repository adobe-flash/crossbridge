/* Radar 4964532 */
/* { dg-do compile } */
/* LLVM LOCAL add x86_64 Darwin */
/* { dg-options "-O2 -gdwarf-2 -dA -mmacosx-version-min=10.4 -m32" { target powerpc*-*-darwin* i?86*-*-darwin* x86_64-*-darwin* } } */
/* { dg-options "-O2 -gdwarf-2 -dA" { target arm*-*-darwin* } } */
/* { dg-final { scan-assembler "DW_OP_APPLE_uninit" } } */
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
  int x;
  int y;

  int sum;

  fprintf (stdout, "x is %d [uninit]\n", x);
  fprintf (stdout, "y is %d [uninit]\n", y);

  sum = x + y;
  fprintf (stdout, "final sum is %d (%d + %d)\n", sum, x, y);
}
