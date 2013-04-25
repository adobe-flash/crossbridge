/* APPLE LOCAL file 6308664 */
/* { dg-do compile { target i?86-*-darwin* x86_64-*-darwin* } } */
/* { dg-options { -m64 -mfix-and-continue } } */
/* { dg-final { scan-assembler-not " L.*@GOTPCREL" } } */
extern void doit(double x);

void test()
{
  doit(16.0);
  doit(32.0);
}
