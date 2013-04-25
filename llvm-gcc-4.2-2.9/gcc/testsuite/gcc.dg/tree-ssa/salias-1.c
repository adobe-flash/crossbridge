/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-salias" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

struct {
  struct {
    struct {
	int i, j;
    } c;
  } b;
} a;

int foo(void)
{
  a.b.c.i = 0;
  return a.b.c.j;
}

/* { dg-final { scan-tree-dump-times "SFT" 2 "salias" } } */
/* { dg-final { cleanup-tree-dump "salias" } } */
