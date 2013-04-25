/* Test the -fconstant-string-class=Foo option under the NeXT
   runtime.  */
/* Developed by Markus Hitter <mah@jump-ing.de>.  */
/* APPLE LOCAL radar 4621575 */
/* { dg-options "-fnext-runtime -fno-constant-cfstrings -fconstant-string-class=Foo -lobjc -Wno-format" } */
/* { dg-do run { target *-*-darwin* } } */
/* APPLE LOCAL radar 5272224 */
/* { dg-require-effective-target ilp32 } */

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
#if (!__OBJC2__)
# if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
id _FooClassReference[20];
# else
 struct objc_class _FooClassReference;
# endif
#endif
/* APPLE LOCAL end objc2 */

@implementation Foo : Object
- (char *)customString {
  return cString;
}
@end

int main () {
  Foo *string = @"bla";
  Foo *string2 = @"bla";

  if(string != string2)
    abort();
  printf("Strings are being uniqued properly\n");

/* APPLE LOCAL begin objc2 */
#if !__OBJC2__
  /* This memcpy has to be done before the first message is sent to a
     constant string object. Can't be moved to +initialize since _that_
     is already a message. */

  memcpy(&_FooClassReference, objc_getClass("Foo"), sizeof(_FooClassReference));
#endif
/* APPLE LOCAL end objc2 */
  if (strcmp ([string customString], "bla")) {
    abort ();
  }

  printf([@"This is a working constant string object\n" customString]);
  return 0;
}
