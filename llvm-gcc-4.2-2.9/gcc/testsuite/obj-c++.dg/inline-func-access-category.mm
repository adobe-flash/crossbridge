/* APPLE LOCAL file radar 5471096 */
/* Test that an inlinable function in cateogry block has access to
   protected/private member of its class though pointer to the class. */
/* { dg-do compile } */

@interface B 
  {
    int intValue;
  }
@end


@interface B()

inline static int B_intValue(B *self)
  { return self->intValue; }

@end

@implementation B

@end
