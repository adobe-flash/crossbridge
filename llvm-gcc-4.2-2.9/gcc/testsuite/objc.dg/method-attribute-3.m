/* APPLE LOCAL file radar 3803157 */
/* Test implementaion of attributes on methods. */
/* { dg-do compile } */

@interface INTF
- (void) foo1 __attribute__((deprecated));
- (int*) foo2  __attribute__((deprecated)) : (int) x1 __attribute__((deprecated)); /* { dg-error "expected \';\' before \\':\\' token" } */
- (void) foo3 __attribute__((deprecated)) : (int) x1, ... __attribute__((deprecated)); /* { dg-error "expected \';\' before \\':\\' token" } */
- (int*) foo2  : (int) x1 __attribute__((deprecated)) : (char) y1 __attribute__((deprecated)) ; /* { dg-error "expected \';\' before \\':\\' token" } */
//
- (void) foo4 __attribute__((deprecated)) : (int) x3; /* { dg-error "expected \';\' before \\':\\' token" } */
- (void) foo5 __attribute__((deprecated)) : (int) x3 , ... ; /* { dg-error "expected \';\' before \\':\\' token" } */
@end
