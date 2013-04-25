/* APPLE LOCAL file radar 4290840 - Modified for radar 6271728 */
/* { dg-do compile } */

@interface Foo
-(void) someMethod;
@end

@implementation Foo
-(void)
-(void) someMethod /* { dg-error "expected before .-." } */
		   /* { dg-error "expected" "" { target *-*-* } 10 } */ 
{
}
@end
