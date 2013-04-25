/* APPLE LOCAL file __block assign sequence point 6639533 */
/* { dg-options "-framework Foundation -lobjc" } */
/* { dg-do run { target i?86*-*-darwin* } } */

#import <Foundation/Foundation.h>

@interface MyClass : NSObject {
}

+ (int)doSomethingWithBlock:(void (^)(void))block;

@end

@implementation MyClass

+ (int)doSomethingWithBlock:(void (^)(void))block {
  Block_copy(block);
  return 2;
}

@end


int main(void) {
  [[NSAutoreleasePool alloc] init];
  __block int foo = 0;
  void (^block)(void) = ^{
    foo = 1;
  };
  foo = [MyClass doSomethingWithBlock:block];
  if (foo == 2)
    return 0;
  printf("foo: %d\n", foo);
  return 1;
}
