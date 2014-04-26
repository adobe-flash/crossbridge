/* APPLE LOCAL file radar 3533972 */
/* Test that type conversion function is applied to the 'receiver' in a method call,
   if one is available. */
/* { dg-options "-framework Foundation -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */
#include <Foundation/Foundation.h>

struct wrapper
{
      id object;
      wrapper (id obj) : object(obj) { }
      operator id () const { return object; }
};

@interface NamedObject : NSObject
- (int) val;
@end

@implementation NamedObject
- (int) val { return 123; }
@end

int main()
{

   wrapper foo([[NamedObject alloc] init]);

   if ([foo val] != 123)
     abort ();
   return 0;
}

