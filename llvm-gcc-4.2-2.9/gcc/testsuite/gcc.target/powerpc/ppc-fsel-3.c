/* { dg-do compile { target powerpc*-*-* } } */
/* APPLE LOCAL begin radar 3961392 */
/* { dg-options "-O -mpowerpc-gfxopt -ftrapping-math" } */
/* APPLE LOCAL end radar 3961392 */
/* { dg-final { scan-assembler-not "fsub" } } */

/* Check that an fsub isn't generated when no arithmetic was requested;
   such an fsub might incorrectly set floating-point exception flags.  */

double foo(double a, double b, double c, double d)
{
  return a < b ? c : d;
}
