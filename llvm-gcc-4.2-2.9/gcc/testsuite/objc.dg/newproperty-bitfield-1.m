/* APPLE LOCAL file radar 5389292 */
/* Test that ivar bitfields work for property of same underlying type. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#import <Foundation/Foundation.h>

@interface Foo : NSObject {

   BOOL  _field1 : 5;
   BOOL  _field2    : 3;
}

@property BOOL field1;
@property BOOL field2;
@end

@implementation Foo

@synthesize field1 = _field1;
@synthesize field2 = _field2;

@end

int main()
{
  Foo *f = [[Foo alloc] init];
  f.field1 = 0xF;
  f.field2 = 0x3;
  f.field1 = f.field1 & f.field2;
  if (f.field1 != 0x3)
    abort ();
  return 0; 
}


