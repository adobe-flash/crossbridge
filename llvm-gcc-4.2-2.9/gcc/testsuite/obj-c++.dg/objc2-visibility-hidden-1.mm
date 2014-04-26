/* APPLE LOCAL file radar 4705298 */
/* Test effect of "hidden" visibility setting of a class on exported symbols
   int objc2 abi. */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */

__attribute__((visibility("hidden"))) 
@interface Foo {
  @public
    id publicIvar;
  @protected
    id protectedIvar;
  @private
    id privateIvar;
}
@end
@implementation Foo @end
/* { dg-final { scan-assembler ".private_extern _OBJC_METACLASS_\\\$_Foo" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_CLASS_\\\$_Foo" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_Foo.publicIvar" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_Foo.protectedIvar" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_Foo.privateIvar" } } */
