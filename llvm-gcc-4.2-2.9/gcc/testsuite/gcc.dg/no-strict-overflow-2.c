/* { dg-do compile } */
/* { dg-options "-fno-strict-overflow -O2 -fdump-tree-final_cleanup" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

/* Source: Ian Lance Taylor.  Dual of strict-overflow-2.c.  */

/* We can only simplify the division when using strict overflow
   semantics.  */

int
foo (int i)
{
  return (i * 100) / 10;
}

/* { dg-final { scan-tree-dump "100" "final_cleanup" } } */
/* { dg-final { cleanup-tree-dump "final_cleanup" } } */
