/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dom2" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

float foo(float x)
{
  x += 1;
  x -= 1;
  return x;
}

/* We should *not* fold the arithmetic.  */
/* { dg-final { scan-tree-dump-times "0\\.0" 0 "dom2"} } */
/* { dg-final { cleanup-tree-dump "dom2" } } */
