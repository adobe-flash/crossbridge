/* APPLE LOCAL 4875151 make sure PRE is off */
/* { dg-options "-O1 -funsafe-math-optimizations -fno-trapping-math -fdump-tree-recip -fno-tree-pre" } */
/* { dg-do compile } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

/* Test inserting in a block that does not contain a division.  */

extern double h();

double f(int x, double z, double w)
{
  double b, c, d, e, f;
  double y = h ();

  if (x)
    b = 1 / y, c = z / y;
  else
    b = 3 / y, c = w / y;

  d = b / y;
  e = c / y;
  f = 1 / y;

  return d + e + f;
}

/* { dg-final { scan-tree-dump-times " / " 1 "recip" } } */
/* { dg-final { cleanup-tree-dump "recip" } } */

