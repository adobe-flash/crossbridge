/* APPLE LOCAL file radar 4193359 */
/* Ensure that variable *not* declared 'volatile' by user does not
   trigger "discards qualifiers from target pointer type" warning 
   when synthesized by the EH-volatization machinery. */

/* { dg-options "-fobjc-exceptions -fnext-runtime" } */
/* { dg-do compile } */

#include <string.h>

@interface Foo
-(void) foo;
@end

@implementation Foo

-(void) foo
{
  float f = 0;

  @try
  {
  }
  @finally
  {
  }

  memcpy(&f, 
	 &f, sizeof(f));
}

@end
//-------------------------------

