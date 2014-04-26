/* APPLE LOCAL file radar 5925781 */
/* Test for the "nonnull" function attribute.  */
/* { dg-do compile } */
/* { dg-options "-Wnonnull" } */

#include <stddef.h>

extern void func1 (void (^block1)(), void (^block2)(), int) __attribute__((nonnull));

extern void func3 (void (^block1)(), int, void (^block2)(), int)
  __attribute__((nonnull(1,3)));

extern void func4 (void (^block1)(), void (^block2)()) __attribute__((nonnull(1)))
  __attribute__((nonnull(2)));

void
foo (int i1, int i2, int i3, void (^cp1)(), void (^cp2)(), void (^cp3)())
{
  func1(cp1, cp2, i1);

  func1(NULL, cp2, i1); /* { dg-warning "null" "null with argless nonnull 1" } */
  func1(cp1, NULL, i1); /* { dg-warning "null" "null with argless nonnull 2" } */
  func1(cp1, cp2, 0);


  func3(NULL, i2, cp3, i3); /* { dg-warning "null" "null with explicit nonnull 1" } */
  func3(cp3, i2, NULL, i3); /* { dg-warning "null" "null with explicit nonnull 3" } */

  func4(NULL, cp1); /* { dg-warning "null" "null with multiple attributes 1" } */
  func4(cp1, NULL); /* { dg-warning "null" "null with multiple attributes 2" } */
}
