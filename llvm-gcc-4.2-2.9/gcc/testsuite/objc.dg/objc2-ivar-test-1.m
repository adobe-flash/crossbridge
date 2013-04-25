/* APPLE LOCAL file 4540451  & 6386976 */
/* Test that the new-ABI ivar symbols look like OBJC_IVAR_$_ClassName.IvarName
   not like .objc_ivar.ClassName.IvarName */
/* { dg-options "-g -dA -mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Rootclass { 
} @end

@interface Superclass : Rootclass { 
@public
  double FILLER;
  int IVAR; 

  struct INNER {
	int filler;
	int MyMainField;
  } F1;

@private
  int private_ivar;

}
- (int) IVAR_VALUE_FUNC;
@end

@implementation Superclass : Rootclass
- (int) IVAR_VALUE_FUNC { return IVAR + FILLER + private_ivar; }
@end

Superclass *PPPPPP;

int main()
{
	int III = PPPPPP->F1.MyMainField;
	if (III)
	  return PPPPPP->IVAR + PPPPPP->IVAR - PPPPPP->FILLER;
	else
	  return PPPPPP->FILLER;
}
/* { dg-final { scan-assembler "OBJC_IVAR_\\\$_" } } */
/* { dg-final { scan-assembler-not "\\\.objc_ivar\\\." } } */
/* APPLE LOCAL radar 6386976 - Add the following two checks.  */
/* { dg-final { scan-assembler "DW_AT_APPLE_major_runtime_vers" } } */
/* { dg-final { scan-assembler "DW_AT_APPLE_runtime_class" } } */
