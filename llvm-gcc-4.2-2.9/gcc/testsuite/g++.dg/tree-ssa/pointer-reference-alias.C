/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-optimized" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

int f(int *a)
{
  int &b = *a;
  b = 0;
  return *a;
}

/* There should be only one dereferencing of a. */
/* { dg-final { scan-tree-dump-times "\\*a" 1 "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
