/* APPLE LOCAL file radar 6017984 */
/* Warn if @implementation does not implement methods declared
   in anonymous category's protocol list. */
/* { dg-do compile { target *-*-darwin* } } */

@protocol Bar
- (void) baz;
@end

@interface Foo 
@end

@interface Foo () <Bar>
@end

@implementation Foo
@end /* { dg-warning "incomplete implementation of class 'Foo'" } */
     /* { dg-warning "method definition for '-baz' not found" "" { target *-*-* } 17 } */
