/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */
int a (int x) {
	return ~x + 1; /* -x */
}

int b (int x) {
	return -x -1; /* ~x */
}

/* { dg-final { scan-tree-dump "~x;" "optimized" } } */
/* { dg-final { scan-tree-dump "-x;" "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
