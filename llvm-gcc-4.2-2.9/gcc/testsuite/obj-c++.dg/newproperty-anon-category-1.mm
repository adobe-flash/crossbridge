/* APPLE LOCAL file radar 4965989 */
/* Test for checking out property execution with anonymous category adding setter to
   primary category. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <Foundation/Foundation.h>

@interface MyClass : NSObject
{
int _foo;
}
@property(readonly) int foo;
@end

@interface MyClass ()
@property(readwrite) int foo;
@end

@implementation MyClass
@synthesize foo = _foo;
@end

int main(void)
{
  NSAutoreleasePool* pool = [NSAutoreleasePool new];
  MyClass* obj  = [MyClass new];
  obj.foo = 1;
  printf("obj.foo = %d", obj.foo);
  [pool drain];
  return 0;
}
