/* APPLE LOCAL file radar 4738176 */
/* Test that no bogus warning is issued in the synthesize compound-expression. */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5 -Wall" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-Wall" { target arm*-*-darwin* } } */
/* { dg-do compile } */

@interface test
@property int foo;
@property int foo1;
@property int foo2;
@end
extern int one ();
extern int two ();

@implementation test
@dynamic foo, foo1, foo2;
- (void) pickWithWarning:(int)which { 
	   self.foo = (which ? 1 : 2); 
	   self.foo1 = self.foo2 = (which ? 1 : 2); 
	   self.foo = (which ? one() : two() ); 
	   self.foo1 = self.foo2 = (which ? one() : two ()); 
}
@end
