// { dg-do compile }
// { dg-options "-fopenmp -fdump-tree-ompexp" }
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

void bar(void);
void foo(void)
{
  #pragma omp critical (xyzzy)
    bar();
}

// { dg-final { scan-tree-dump-times "\\&\\.gomp_critical_user_xyzzy" 2 "ompexp" } }
// { dg-final { cleanup-tree-dump "ompexp" } }
