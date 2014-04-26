/* APPLE LOCAL file radar 6269491 */
/* Test that lvalue cast with -fobjc-gc also results in a warning. */
/* { dg-options "-fobjc-gc" } */

@interface I @end

void foo(I* pI)
{
  (id) pI = 0;  /* { dg-warning "target of assignment not really an lvalue" } */
}	
	
