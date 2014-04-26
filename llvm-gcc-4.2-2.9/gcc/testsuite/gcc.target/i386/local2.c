/* APPLE LOCAL file mainline */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-O2 -funit-at-a-time -fomit-frame-pointer" } */
/* { dg-skip-if "PR 25214" { ilp32 } { "-fpic" "-fPIC" } { "" } } */
/* APPLE LOCAL begin radar 4875094 */
/* Thr test relies on -mtune=i386 and the value of STACK_BOUNDARY which was
   changed for -mpreferred-stack-boundary (radar 3232990). */
/* { dg-skip-if "" { *-*-darwin* } { "*" } { "" } } */
/* APPLE LOCAL end radar 4875094 */
/* { dg-final { scan-assembler-not "sub\[^\\n\]*sp" } } */

static __attribute__ ((noinline)) q ();
int a;

/* This function should not require any stack manipulation
   for preferred stack bounday.  */
void
e ()
{
  if (a)
  {
    e ();
    a--;
  }
  q ();
}

static __attribute__ ((noinline)) q ()
{
}
