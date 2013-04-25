/* APPLE LOCAL file 4134283 */
/* Test BNSYM/ENSYM stabs in -gused mode.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -feliminate-unused-debug-symbols" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

void foo()
{
	int i;
	i = 0;
}

/* { dg-final { scan-assembler ".stabd.*46" } } */
/* { dg-final { scan-assembler ".stabd.*78" } } */

