/* APPLE LOCAL file radar 4966565 */
/* This test is for categories which don't implement the accessors but some accessors are
   implemented in their base class implementation. In this case,no warning must be issued.
*/
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
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

@interface MyClass (private)
@property(readwrite)    int        foo;
@end

@implementation MyClass (private)
- (void) setFoo:(int)foo    { _foo = foo; }
@end

@interface MyClass (public)
@property(readwrite)    int        foo;
@end

@implementation MyClass (public)
@end /* { dg-warning "property 'foo' requires the method 'setFoo:'" } */
