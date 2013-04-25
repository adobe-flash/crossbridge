/* APPLE LOCAL file radar 5599048 */
/* { dg-do run { target *-*-darwin* } } */ 
/* { dg-options "-O -framework Foundation" } */
#import <Foundation/Foundation.h>
const NSPoint orig = NSMakePoint(20, 8);

@interface foo:NSObject 
{
}
@end

@implementation foo
- (id) init
{
  NSPoint nameorig = orig;

  if (nameorig.x != orig.x || nameorig.y != orig.y)
    {
      abort ();
    }

  return [super init];
}

int main(void)
{
  foo *obj = [[foo alloc] init];
  return 0;
}
@end
