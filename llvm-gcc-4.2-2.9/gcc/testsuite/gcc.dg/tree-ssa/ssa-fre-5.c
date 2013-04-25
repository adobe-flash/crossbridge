/* { dg-do compile } */
/* { dg-options "-O -fdump-tree-fre-details" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

/* From PR19792.  */

int
bar (unsigned int t)
{
  int a = t;
  return a == t;
}

/* { dg-final { scan-tree-dump "Replaced \\\(unsigned int\\\) a_.*with t_" "fre" } } */
/* { dg-final { cleanup-tree-dump "fre" } } */
