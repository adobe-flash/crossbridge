/* APPLE LOCAL file opt diary */
/* { dg-do compile } */
/* { dg-require-effective-target vect_int } */
/* { dg-options "-dA -O2 -ftree-vectorize -fdump-tree-vect-stats -maltivec -gdwarf-2 -fopt-diary" { target powerpc*-*-* } } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */
/* { dg-options "-dA -O2 -ftree-vectorize -fdump-tree-vect-stats -msse2 -gdwarf-2 -fopt-diary" { target i?86-*-* x86_64-*-* } } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */


#define N 16


void ibar (int *);


/* multiple loops */

foo (int n)
{
  int ia[N];
  int ib[N];
  int ic[N];
  int i;

  /* Vectorizable.  */
  for (i = 0; i < N; i++){
    ia[i] = ib[i];
  }
  ibar (ia);
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 1 "vect" } } */
/* { dg-final { scan-assembler "DW_TAG_GNU_OD_entry" } } */
/* { dg-final { scan-assembler "DW_AT_GNU_OD_msg" } } */
/* { dg-final { scan-assembler "DW_AT_GNU_OD_category" } } */
