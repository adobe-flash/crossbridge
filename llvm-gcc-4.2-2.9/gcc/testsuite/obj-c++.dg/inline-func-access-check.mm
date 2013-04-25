/* APPLE LOCAL file radar 4805612 */
/* Test that an inlinable function in @interface block has access to 
   protected/private member of the class though pointer to the class. */
/* { dg-do compile } */

@interface X 
{ 
  int protected_count; 
@private int private_count;
@public  int public_count;
}

inline static void X_inc(X *self)
  { self->protected_count + self->private_count + self->public_count; } // OK
@end

inline static int Unrelated (X *self)
{
  return self->protected_count +  /* { dg-warning "instance variable 'protected_count' is @protected; this will be a hard error in the future" } */
         self->private_count + self->public_count; /* { dg-warning "warning: instance variable 'private_count' is @private; this will be a hard error in the future" } */ 
}
