/* APPLE LOCAL file 5774213 */
/* We must not generate global symbol for category names in the ObjC2 ABI. */
/* { dg-options "-mmacosx-version-min=10.5 -m64" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Foo
@end

@interface Foo (stuff)
@end

@implementation Foo (stuff)
@end
/* { dg-final { scan-assembler-not ".objc_category_name" } } */
