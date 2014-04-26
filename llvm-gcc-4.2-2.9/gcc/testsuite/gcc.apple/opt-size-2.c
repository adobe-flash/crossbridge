/* APPLE LOCAL file 4231773 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-Oz -static" } */
/* { dg-final { scan-assembler "\tincl?\[ 	\]*_?global_x" } } */

/* Insure that -Oz generates INC on Darwin/x86 (see also opt-size-1.c).  */

int global_x;

int
main ()
{
  global_x++;
  return 0;
}
