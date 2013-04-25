/* APPLE LOCAL file radar 4805321 */
/* Test that we call objc_assign_weak and objc_read_weak */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -framework Foundation" { target arm*-*-darwin* } } */

#include <Foundation/Foundation.h>

@interface Foo : NSObject 
{
  __weak id ivar;
}
@property (assign) __weak id delegate; 
@end

@implementation Foo
@synthesize delegate=ivar;
@end

main () {
  [NSAutoreleasePool new];

  Foo *foo = [Foo new];

  id obj = [NSObject new];

  if ([obj retainCount] != 1)
    abort();

  [foo setDelegate:obj];

  if ([obj retainCount] != 1)
    abort();

  return 0;
}

