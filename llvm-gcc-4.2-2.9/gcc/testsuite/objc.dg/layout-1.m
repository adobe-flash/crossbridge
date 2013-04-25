/* Ensure that we do not get bizarre warnings referring to
   __attribute__((packed)) or some such.  */
/* { dg-do compile } */
/* { dg-options "-Wpadded -Wpacked" } */
/* APPLE LOCAL radar 4894756 */
/* { dg-require-effective-target ilp32 } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Derived1: Object
{ }
@end

@interface Derived2: Object
- (id) foo;
@end

