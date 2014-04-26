/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-dominator-opts -fdump-tree-store_copyprop-details" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

typedef struct { int i; int j; } A;
int foo(A *a, int i)
{
  a->i = i;
  return a->i;
}

/* { dg-final { scan-tree-dump "return i" "store_copyprop" } } */
/* { dg-final { cleanup-tree-dump "store_copyprop" } } */
