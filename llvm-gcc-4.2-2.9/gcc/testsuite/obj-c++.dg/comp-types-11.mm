/* APPLE LOCAL file mainline */
/* { dg-do compile } */


/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@protocol Foo
- (id)meth1;
- (id)meth2:(int)arg;
@end

@interface Derived1: Object
@end

@interface Derived2: Object
+ (Derived1 *)new;
@end

id<Foo> func(void) {
  Object *o = [Object new];
  return o;  /* { dg-warning "class .Object. does not implement the .Foo. protocol" } */
}

@implementation Derived2
+ (Derived1 *)new {
  Derived2 *o = [super new];
  return o;  /* { dg-warning "incompatible Objective-C types returning \\'Derived2\\*\\', expected \\'Derived1\\*\\'" } */
}
@end

