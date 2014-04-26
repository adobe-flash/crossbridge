/* APPLE LOCAL file stack-protector default 5095227 */
/* { dg-do compile { target i?86-*-darwin*i powerpc*-*-darwin* } } */
/* Verify that -fstack-protector is on by default and that it at least
   appears to work.  */
extern void f2(char *);
void f1(void)
{
  char array[256];
  f2(array);
}
/* { dg-final { scan-assembler "__stack_chk_guard" } } */
/* { dg-final { scan-assembler "__stack_chk_fail" } } */
