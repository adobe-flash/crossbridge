/* Test that we don't ICE when issuing a -Wselector warning.  */
/* { dg-options "-Wselector" } */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Foo
@end
@implementation Foo
-(void) foo
{
  SEL a;
  a = @selector(b1ar);
}
@end
/* { dg-warning "creating selector for nonexistent method .b1ar." "" { target *-*-* } 0 } */

