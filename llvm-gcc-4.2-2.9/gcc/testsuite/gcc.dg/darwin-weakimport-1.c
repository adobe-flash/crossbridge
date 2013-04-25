/* { dg-do compile { target *-*-darwin* } } */
/* { dg-require-weak "" } */
/* { dg-options "-fno-common -fno-asynchronous-unwind-tables" } */
/* APPLE LOCAL 64-bit default objective-c 6348517 */
/* { dg-options "-fno-common -fno-asynchronous-unwind-tables -m32" { target { i?86-*-darwin* x86_64-*-darwin* } } } */

/* { dg-final { scan-assembler "weak_reference _a" } } */
/* { dg-final { scan-assembler-not "weak_\[a-z \t\]*_b" } } */

extern void a (void) __attribute__((weak_import));
extern void b (void) __attribute__((weak_import));

void b(void)
{
  a();
}
