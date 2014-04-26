/* APPLE LOCAL file radar 4733555 */
/* weak_import attribute on methods must be silently ignored, as such methods are
   already 'weak'. */
/* { dg-do compile { target *-*-darwin* } } */

@interface Foo
- (void) a __attribute__((weak_import));
- (void) b __attribute__((weak_import));
+ (void) c __attribute__((weak_import));
@end


int func (Foo *p)
{
	[p a];
	[p b];
	[Foo c];
}
