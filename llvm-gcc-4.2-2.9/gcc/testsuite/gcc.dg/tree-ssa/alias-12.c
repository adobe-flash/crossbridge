/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-alias1-vops" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

struct {
	int i;
	int x[128];
	int j;
} a;

int foo(int i)
{
	a.x[i] = 0;
	return a.x[i];
}

/* { dg-final { scan-tree-dump "V_MAY_DEF" "alias1" } } */
/* { dg-final { cleanup-tree-dump "alias1" } } */

