/* APPLE LOCAL file radar 5370783 */
/* Test that if a method defined in the class does not match in a reasonable way to a declaration
   in its super class, then issue a warning. */
/* { dg-do compile } */

#include <Foundation/Foundation.h>

@interface Foo  : NSObject
- (void) doSomething: (float) x;  /* { dg-warning "previous declaration of" } */
@end

@implementation Foo
- (void) doSomething: (float) x
{
  printf("Foo::doSomething (%f)\n", x);
}
@end

@interface Bar : Foo
@end

@implementation Bar
- (void) doSomething: (int) x
{				/* { dg-warning "conflicting types for" } */
  printf("Bar::doSomething (%d)\n", x);
}
@end

int main()
{
  Bar *x = [[Bar alloc] init];
  Foo *foo = x;
  [foo doSomething:5];
}

