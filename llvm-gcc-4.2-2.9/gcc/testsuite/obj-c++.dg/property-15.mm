/* APPLE LOCAL file radar 4675792 */
/* Test that 'class' can be a valid getter name in a property. */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

@interface Base {
    Class _isa;
}
- (Class)class;
@property(readonly, getter=class) Class isa;
@end

@implementation Base
@synthesize isa=_isa;

- (Class)class {
    return self.isa;
}
@end
