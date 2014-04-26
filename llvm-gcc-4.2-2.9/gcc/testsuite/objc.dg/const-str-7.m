/* Test to make sure that the const objc strings are the same across
   scopes.  */
/* Developed by Andrew Pinski <pinskia@physics.uc.edu> */

/* APPLE LOCAL radar 4621575 */
/* { dg-options "-fnext-runtime -fno-constant-cfstrings -fconstant-string-class=Foo -lobjc" } */
/* { dg-do run { target *-*-darwin* } } */


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"


@interface Foo: Object {
  char *cString;
  unsigned int len;
}
- (char *)customString;
@end

/* APPLE LOCAL begin objc2 */
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
Class _FooClassReference;
#else
struct objc_class _FooClassReference;
#endif
/* APPLE LOCAL end objc2 */


@implementation Foo : Object
- (char *)customString {
  return cString;
}
@end


int main () {
  Foo *string = @"bla";
  {
    Foo *string2 = @"bla";


    if(string != string2)
      abort();
    printf("Strings are being uniqued properly\n");
   }
  return 0;
}

