/* APPLE LOCAL file 4285232 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-O2" } */
extern void bar(char[4]);
int a, b, c, d;
void foo() {
  char x[4] = { a, b, c, d };
  bar(x);
}
/* s/xfail/target/ to re-enable this test: */
/* { dg-final { scan-assembler-not "\\\$0, " { xfail i?86-*-darwin* } } }*/
