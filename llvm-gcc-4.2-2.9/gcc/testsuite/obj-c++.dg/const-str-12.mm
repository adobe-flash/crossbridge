/* APPLE LOCAL file 4154928 */
/* Test if ObjC types play nice in conditional expressions.  */
/* Author: Ziemowit Laski  */

/* { dg-options "-fno-constant-cfstrings -fconstant-string-class=Foo" } */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */

#include <objc/Object.h>

@interface Foo: Object {
  char *cString;
  unsigned int len;
}
+ (id)description;
@end

@interface Bar: Object
+ (Foo *) getString: (int) which;
@end

#if OBJC_API_VERSION >= 2
Class _FooClassReference;
#else
struct objc_class _FooClassReference;
#endif

@implementation Bar
+ (Foo *) getString: (int) which {
  return which? [Foo description]: @"Hello";
}
@end
