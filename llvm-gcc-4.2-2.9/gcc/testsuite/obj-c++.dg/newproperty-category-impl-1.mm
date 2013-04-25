/* APPLE LOCAL file radar 4966565 */
/* This test is for categories which don't implement the accessors but some accessors are
   implemented in their base class implementation. In this case,no warning must be issued.
*/
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface MyClass 
{
    int        _foo;
}
@property(readonly)    int        foo;
@end

@implementation MyClass
- (int) foo        { return _foo; }
@end

@interface MyClass (personal)
@property(readwrite)    int        foo;
@end

@implementation MyClass (personal)
- (void) setFoo:(int)foo    { _foo = foo; }
@end

@interface MyClass (Public)
@property(readwrite)    int        foo;
@end

@implementation MyClass (Public)
@end /* { dg-warning "property 'foo' requires the method 'setFoo:'" } */
