/* APPLE LOCAL file blocks stabs 6034272 */
/* { dg-do compile } */
/* { dg-options "-gstabs" } */
/* LLVM LOCAL test not applicable (no stabs) */
/* { dg-require-fdump "" } */

void foo() {
   void (^x)(void) = ^{ foo(); };
   void *y = (void *)x;
}
