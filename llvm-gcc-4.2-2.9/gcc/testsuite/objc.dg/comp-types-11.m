/* APPLE LOCAL file radar 5839123 */
/* { dg-do compile } */
#include <objc/Object.h>

@interface Derived: Object
@end

extern Object* foo(void);
static Derived *test(void)
{
   Derived *m = foo();   /* { dg-warning "incompatible Objective-C types initializing \\'struct Object \\*\\', expected \\'struct Derived \\*\\'" } */

   return m;
}

