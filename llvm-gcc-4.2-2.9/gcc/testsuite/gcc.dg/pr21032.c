/* { dg-do compile } */
/* { dg-options "-O -fdump-tree-optimized -frounding-math" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

void bar(float x);
void foo(double x)
{
  bar(-x);
}

/* { dg-final { scan-tree-dump-not "-\\(float\\)" "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
