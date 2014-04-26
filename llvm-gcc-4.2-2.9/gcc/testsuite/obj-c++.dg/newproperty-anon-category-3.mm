/* APPLE LOCAL file radar 4965989 */
/* Check for a variety misue of anonymous category. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Foo
{
int _foo;
}
- (void) bar;
- (int)  garf;
@end

@interface Foo()
- (void) baz;
- (void) bob;
- (void)  garf;
// @property(readwrite) int foo; /* dg-error { "property 'foo' in anonymous category was not previously declared" } */
@end			      /* { dg-error "duplicate declaration of method" } */

#if 0
@implementation Foo
- (void) bar {;}
- (void) baz {;}
- (int) garf {return 0;}
@synthesize foo = _foo;	     /* dg-error { "no declaration of property 'foo' found in the interface" } */
@end			     /* dg-warning { "incomplete implementation of class 'Foo'" } */
			     /* dg-warning { "method definition for '-bob' not found" "" { target *-*-* } 26 } */
#endif
