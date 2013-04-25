/* APPLE LOCAL file bincl/eincl stabs */
/* Test BINCL/EINCL stabs.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs -fno-eliminate-unused-debug-symbols" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

#include "bincl-1.h"
int
main ()
{
  my_int j = 0;
  return j;
}

/* { dg-final { scan-assembler ".stabs.*130,0,0,0" } } */
/* { dg-final { scan-assembler ".stabs.*162,0,0,0" } } */

