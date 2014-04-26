/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

static const char f[3] = "?";

int foo()
{
  int i = 0;
  return f[i] != '?';
}

/* { dg-final { scan-tree-dump "return 0;" "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */

