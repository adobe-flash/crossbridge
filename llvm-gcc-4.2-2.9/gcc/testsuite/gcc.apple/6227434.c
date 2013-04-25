/* APPLE LOCAL file 6227434 */
/* { dg-do compile { target i?86-apple-darwin* x86_64-apple-darwin* } } */
/* { dg-options { -mfix-and-continue } } */
/* { dg-require-effective-target lp64 } */
/* { scan-assembler-not "_count\[12\]-" } */
static unsigned count1 = 0;
extern unsigned count2;

void t(void) {
  count1 = 1;
  count2 = 2;
}
