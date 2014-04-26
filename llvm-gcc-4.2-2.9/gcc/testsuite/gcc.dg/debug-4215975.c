/* APPLE LOCAL file 4215975 */
/* Test to check Radar 4215975 fix.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -feliminate-unused-debug-symbols" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */


int main( void )
{
  const int mantissa_size = 52;
    union
    {
      int    u;
      double      d;
    } startExp;
    
    startExp.u = mantissa_size; 
    


    return 0;
}

/* { dg-final { scan-assembler ".stabs.*t:" } } */

