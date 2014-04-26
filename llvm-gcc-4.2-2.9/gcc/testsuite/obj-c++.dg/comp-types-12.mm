/* APPLE LOCAL file mainline */
/* { dg-do compile } */
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Derived: Object
@end

extern Object* foo(void);
static Derived *test(void)
{
   Derived *m = foo();   /* { dg-warning "incompatible Objective-C types initializing \\'Object\\*\\', expected \\'Derived\\*\\'" } */

   return m;
}

