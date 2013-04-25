/* APPLE LOCAL file 4104248 */
/* { dg-do compile { target "i?86*-*-darwin*" } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-Os" } */
register char foo asm("edi");
char x;
int bar() {
  foo = x;
}
/* { dg-final { scan-assembler "movzbl" } } */
