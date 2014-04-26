/* APPLE LOCAL file radar 4705250 */
/* @defs is allowed with -fobjc-atdefs option. */
/* Compile with no error or warning. */
/* { dg-options "-mmacosx-version-min=10.5 -m64 -fobjc-atdefs" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
#include <Foundation/Foundation.h>

extern void abort(void);

@interface A : NSObject
{
  @public
    int a;
}
@end

struct A_defs 
{
  @defs(A);	
};

@implementation A
- init 
{
  a = 42;
  return self;
}
@end


int main() 
{
  A *a = [[A alloc] init];
  struct A_defs *a_defs = (struct A_defs *)a;
  
  if (a->a != a_defs->a)
    abort ();	
  
  return 0;
}
