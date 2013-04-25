/* APPLE LOCAL file radar 5809099 */
/* compile-only test to test that we can cast back and forth a block pointer
   type to an 'id' type and sending a message to an object of block pointer type.
*/
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC" { target *-*-darwin* } } */
#import <Foundation/Foundation.h>

void bar() { }

void foo() {
   id object = [[NSObject new] init];
   void (^x)(void) = ^{ bar(); };
   void *y = (void *)x;
   x = (void (^)(void))y;
   /* Test for type-cast of an 'id' type to a block pointer. */
   x = (void (^)(void))[object result];

   /* Test for cat of a block pointer to an 'id'. */
   object = (id)x;

  /* Test for sending a message to a block pointer. */
  [x result];
}
