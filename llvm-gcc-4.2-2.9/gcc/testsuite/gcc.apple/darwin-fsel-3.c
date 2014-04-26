/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-O -mpowerpc-gfxopt" } */
/* { dg-final { scan-assembler "fsub" } } */

/* This is the same as gcc.dg/ppc-fsel-3.c, which is checking to see
   that this optimization is *not* done, that is, that 
   -fno-trapping-math is the default.  They can't both pass.  */

double foo(double a, double b, double c, double d)
{
  return a < b ? c : d;
}
