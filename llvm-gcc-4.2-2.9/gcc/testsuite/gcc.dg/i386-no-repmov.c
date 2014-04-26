/* APPLE LOCAL file 4217585 FSF deferred until stage 1 */
/* { dg-do compile { target "i?86-*-darwin*" } } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-not "rep" } } */
struct S { int x[15]; };
volatile struct S s1;
volatile struct S s2;
void bar() {
  s1 = s2;
}
