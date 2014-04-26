/* APPLE LOCAL file radar 4805321 - test modified because of radar 5435299 */
/* Test that no bogus warning is issued in the synthesize compound-expression. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -Wall" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -Wall" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface test
{
  int i;
  int i1;
  int i2;
}
@property (assign) int foo;
@property (assign) int foo1;
@property (assign) int foo2;
@end
extern int one ();
extern int two ();

@implementation test
@synthesize foo=i,foo1=i1,foo2=i2;
- (void) pickWithWarning:(int)which { 
	   self.foo = (which ? 1 : 2); 
	   self.foo1 = self.foo2 = (which ? 1 : 2); 
	   self.foo = (which ? one() : two() ); 
	   self.foo1 = self.foo2 = (which ? one() : two ()); 
}
@end
