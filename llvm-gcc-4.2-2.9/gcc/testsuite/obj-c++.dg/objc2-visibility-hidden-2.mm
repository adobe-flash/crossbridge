/* APPLE LOCAL file radar 4843145 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2 -fvisibility=hidden" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fvisibility=hidden" { target arm*-*-darwin* } } */

@interface Unspecified {
@private
    int privateIvar;
@public
    int publicIvar;
}
@end

@implementation Unspecified @end

__attribute__((visibility("default")))
@interface Default {
    @private
    int privateIvar;
    @public
    int publicIvar;
}
@end
@implementation Default @end

__attribute__((visibility("hidden")))
@interface Hidden {
    @private
    int privateIvar;
    @public
    int publicIvar;
}
@end
@implementation Hidden @end
/* { dg-final { scan-assembler ".globl _OBJC_METACLASS_\\\$_Default" } } */
/* { dg-final { scan-assembler ".globl _OBJC_CLASS_\\\$_Default" } } */
/* { dg-final { scan-assembler ".globl _OBJC_IVAR_\\\$_Default.publicIvar" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_Default.privateIvar" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_METACLASS_\\\$_Hidden" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_CLASS_\\\$_Hidden" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_Hidden.publicIvar" } } */
/* { dg-final { scan-assembler ".private_extern _OBJC_IVAR_\\\$_Hidden.privateIvar" } } */
