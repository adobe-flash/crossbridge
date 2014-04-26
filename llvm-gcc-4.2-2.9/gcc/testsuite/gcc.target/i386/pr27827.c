/* { dg-do compile { target ilp32 } } */
/* APPLE LOCAL begin mainline 4.3 2006-10-30 4875113 */
/* { dg-options "-O2 -mfpmath=387" } */
/* APPLE LOCAL end mainline 4.3 2006-10-30 4875113 */

double a, b;
double f(double c)
{
  double x = a * b;
  return x + c * a;
}

/* { dg-final { scan-assembler-not "fld\[ \t\]*%st" } } */
/* { dg-final { scan-assembler "fmul\[ \t\]*%st" } } */
