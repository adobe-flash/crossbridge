/* APPLE LOCAL file */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -fno-eliminate-unused-debug-symbols" } */

#include "stab-types.h"
int main()
{
	B b;

	return 0;
}

/* { dg-final { scan-assembler-not ".stabs.*:t\\(1,4\\)=f\\(0,36\\)" } } */
