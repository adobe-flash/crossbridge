/* APPLE LOCAL file 4134283 */
/* Test stabs for uninitialized globals */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -O2" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

static int foo;
int bar;
int main(void)
{
  foo += 3;
  bar *= 5;
  return 0;
}

/* { dg-final { scan-assembler ".stabs.*foo" } } */
/* { dg-final { scan-assembler ".stabs.*bar" } } */

