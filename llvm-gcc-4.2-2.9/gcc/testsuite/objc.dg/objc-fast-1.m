/* APPLE LOCAL file ObjC direct dispatch */
/* A compile-only test for insertion of 'bla' comm page jumps. */
/* Developed by Ziemowit Laski  <zlaski@apple.com>  */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -fobjc-direct-dispatch -Wassign-intercept" } */
/* { dg-require-effective-target ilp32 } */
/* { dg-require-effective-target objc_gc } */

#include <objc/Object.h>

@interface Derived: Object {
@public
  Object *other;
}
@end

void foo(void) {
  Derived *o = [Derived new];
  o->other = 0;  /* { dg-warning "instance variable assignment has been intercepted" } */
}

/* { dg-final { scan-assembler-not "objc_msgSend" } } */
/* { dg-final { scan-assembler-not "objc_assign_ivar" } } */

/* { dg-final { scan-assembler "bla.*fffeff00" } } */
/* { dg-final { scan-assembler "bla.*fffefec0" } } */
