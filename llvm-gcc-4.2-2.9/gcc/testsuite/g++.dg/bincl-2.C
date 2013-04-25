/* APPLE LOCAL file bincl/eincl stabs */
/* Test BINCL/EINCL stabs.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -fno-eliminate-unused-debug-symbols" } */

#include "bincl-2.h"
int
main ()
{
  return 0;
}

/* { dg-final { scan-assembler-not ".stabs.*bincl-22.h\",130,0,0,0" } } */

