/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-O2 -march=k8" } */
/* APPLE LOCAL fix-and-continue 6360409 */
/* { dg-options "-mno-fix-and-continue -O2 -march=k8" { target *-*-darwin* } } */
/* { dg-final { scan-assembler "and\[^\\n\]*magic" } } */

/* Should be done as "andw $32767, magic".  */
static unsigned short magic;
void t(void)
{
	magic%=(unsigned short)0x8000U;
}
