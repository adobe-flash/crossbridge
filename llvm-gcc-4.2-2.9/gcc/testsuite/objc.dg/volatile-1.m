/* Test for proper handling of volatile parameters in ObjC methods.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */
/* Contributed by Ziemowit Laski  <zlaski@apple.com>  */
/* APPLE LOCAL radar 4894756 */
/* { dg-require-effective-target ilp32 } */

@interface Test
-(void) test2: (volatile int) a;
@end

@implementation Test
-(void) test2: (volatile int) a
{
  /* The following assignment should NOT be optimized away.  */
  a = 1;
}
@end

/* { dg-final { scan-assembler "li r\[0-9\]+,1" { target powerpc*-*-darwin* } } } */
/* APPLE LOCAL testing */
/* { dg-final { scan-assembler "movl\t\\\$1, 16\\(%ebp\\)" { target i?86*-*-darwin* } } } */
