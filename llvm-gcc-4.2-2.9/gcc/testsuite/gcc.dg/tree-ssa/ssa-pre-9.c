/* { dg-do compile } */ 
/* { dg-options "-O2 -fno-tree-dominator-opts -fdump-tree-fre-stats" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */
int
foo (unsigned long a)
{
  int b = __builtin_clzl (a);
  int c = __builtin_clzl (a);
  if (b == c)
    return 1;
  return 0;
}
/* { dg-final { scan-tree-dump-times "Eliminated: 1" 1 "fre"} } */
/* { dg-final { cleanup-tree-dump "fre" } } */
