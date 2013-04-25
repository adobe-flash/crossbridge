/* APPLE LOCAL file 4695109 - modified for radar 6255913 */
/* Cechk that both protocols metadata generated for them. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */


@protocol Proto1
@end

@protocol Proto2
@end

@interface Foo
@end

@interface Foo (Category) <Proto1, Proto2>
@end

@implementation Foo (Category)
@end
/* { dg-final { scan-assembler "l_OBJC_PROTOCOL_\\\$_Proto1:" } } */
/* { dg-final { scan-assembler "l_OBJC_PROTOCOL_\\\$_Proto2:" } } */
