/* APPLE LOCAL file 4223137 */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -feliminate-unused-debug-symbols -Wno-format" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

#include <stdio.h>
int main (int argc, const char *argv)
{
  argc = 5;
  printf ("%s\n", argv[0]);
  return 0;
}

/* { dg-final { scan-assembler ".stabs.*:t\\(\[0-9\],\[0-9\]\\)\=k\\(\[0-9\],\[0-9\]\\)" } } */
