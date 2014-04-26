/* APPLE LOCAL file radar 5040740 */
/* Test use of new @package ivars. */
/* { dg-options "-mmacosx-version-min=10.5 -m64" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */
@interface INTF
{
@private
  int PRIVATE_IVAR;
@public
  int PUBLIC_IVAR;
@protected
  int PROTECTED_IVAR;
@package
  int PACKAGE_IVAR;
  int PACKAGE_IVAR1;
@public
  int PUBLIC_IVAR1;
@protected
  int PROTECTED_IVAR1;
@package
  int PACKAGE_IVAR2;
  int PACKAGE_IVAR3;
}
@end

@implementation INTF
@end

int foo ()
{
  INTF *p;
  return p->PACKAGE_IVAR + p->PACKAGE_IVAR3 + p->PUBLIC_IVAR1;
}
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_INTF.PACKAGE_IVAR" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_INTF.PACKAGE_IVAR1" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_INTF.PACKAGE_IVAR2" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_INTF.PACKAGE_IVAR3" } } */
