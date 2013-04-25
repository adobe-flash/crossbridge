/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-fno-asynchronous-unwind-tables" } */
/* APPLE LOCAL 64-bit default objective-c 6348517 */
/* { dg-options "-fno-common -fno-asynchronous-unwind-tables -m32" { target { i?86-*-darwin* x86_64-*-darwin* } } } */
/* { dg-require-weak "" } */

/* { dg-final { scan-assembler-not "coalesced" } } */

extern void foo(void) __attribute__((weak_import));

void foo(void)
{
}
