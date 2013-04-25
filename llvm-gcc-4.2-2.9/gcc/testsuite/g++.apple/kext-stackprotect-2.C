/* APPLE LOCAL file stack-protector default 5095227 */
/* { dg-do compile { target i?86*-*-darwin* } } */
/* { dg-options "-fapple-kext -mmacosx-version-min=10.6" } */
/* { dg-skip-if "" { *-*-darwin* } { "-fstack-protector" } { "" } } */
/* { dg-skip-if "" { *-*-darwin* } { "-fstack-protector-all" } { "" } } */
extern void f2(char *);
void f1(void)
{
  char array[256];
  f2(array);
}
/* { dg-final { scan-assembler "__stack_chk_guard" } } */
/* { dg-final { scan-assembler "__stack_chk_fail" } } */
