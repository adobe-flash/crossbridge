/* Test lang in N_SO stab.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* *-*-sysv5* } { "*" } { "" } } */
/* { dg-options "-gstabs" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

int
main ()
{
  return 0;
}

/* { dg-final { scan-assembler ".stabs.*100,0,2" } } */

