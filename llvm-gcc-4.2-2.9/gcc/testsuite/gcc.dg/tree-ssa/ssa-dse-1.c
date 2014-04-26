/* { dg-do compile } */
/* APPLE LOCAL 4875151 needs strict aliasing */
/* { dg-options "-O2 -fdump-tree-dse1 -fstrict-aliasing" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

int
foo (int a, int b, int c)
{
  int *p;
  if (c)
    p = &a;
  else
    p = &b;

  *p = 3;
  *p = 4;
  return *p;
}


/* We should eliminate both assignments to *p.  */
/* { dg-final { scan-tree-dump-times " = 3" 0 "dse1"} } */
/* { dg-final { scan-tree-dump-times " = 4" 0 "dse1"} } */

/* The return *p should be turned into return 4.  */
/* { dg-final { scan-tree-dump-times " return 4" 1 "dse1"} } */

/* { dg-final { cleanup-tree-dump "dse1" } } */
