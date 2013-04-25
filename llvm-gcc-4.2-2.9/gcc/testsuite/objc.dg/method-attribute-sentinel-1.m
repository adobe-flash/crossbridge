/* APPLE LOCAL file 3803157 */
/* Check for sentinel attribute on method declarations and their use. */
/* { dg-do compile } */
/* { dg-options "-Wformat" } */

#include <stddef.h> /* For NULL, which must be (ptr)0.  */

#define ATTR __attribute__ ((__sentinel__))

@interface INTF
- (void) foo1 : (int)x, ... ATTR;
- (void) foo3 : (int)x ATTR;     /* { dg-warning "attribute only applies to variadic functions" "sentinel" } */
- (void) foo4 : (int)x ATTR, ... ATTR; /* { dg-error "method attributes must be specified" } */
- (void) foo5 : (int)x, ... __attribute__ ((__sentinel__(1)));
- (void) foo6 : (int)x, ... __attribute__ ((__sentinel__(5)));
- (void) foo7 : (int)x, ... __attribute__ ((__sentinel__(0)));
- (void) foo8 : (int)x, ... __attribute__ ((__sentinel__("a"))); /* { dg-warning "not an integer constant" "sentinel" } */
- (void) foo9 : (int)x, ... __attribute__ ((__sentinel__(-1))); /* { dg-warning "less than zero" "sentinel" } */
/* APPLE LOCAL begin two arg sentinel 5631180 */
- (void) foo10 : (int)x, ... __attribute__ ((__sentinel__(1,1)));
- (void) foo11 : (int)x, ... __attribute__ ((__sentinel__(1,1,3))); /* { dg-error "wrong number of arguments" "sentinel" } */
/* APPLE LOCAL end two arg sentinel 5631180 */
@end

int main ()
{
  INTF *p;

  [p foo1:1, NULL]; // OK
  [p foo1:1, 0]; /* { dg-warning "missing sentinel in function call" } */
  [p foo5:1, NULL, 2]; // OK
  [p foo5:1, 2, NULL, 1]; // OK
  [p foo5:1, NULL, 2, 1]; /* { dg-warning "missing sentinel in function call" } */

  [p foo6:1,2,3,4,5,6,7]; /* { dg-warning "missing sentinel in function call" } */
  [p foo6:1,NULL,3,4,5,6,7]; // OK
  [p foo7:1];	/* { dg-warning "not enough variable arguments to fit a sentinel" } */
  [p foo7:1, NULL]; // ok
}
 
