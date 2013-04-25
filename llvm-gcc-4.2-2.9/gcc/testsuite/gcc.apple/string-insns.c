/* APPLE LOCAL Disable string insns with -Os on Darwin (radar 3509006) */
/* { dg-do compile { target powerpc*-apple-darwin* } } */
/* { dg-options "-Os" } */
/* On ppc at Apple, -Os should not use string instructions. 3509006. */
struct s { int a; int b; int c; };
int foo (struct s* p, struct s* q) {
  *p = *q;
}
/* { dg-final { scan-assembler-not "lswi" } } */
