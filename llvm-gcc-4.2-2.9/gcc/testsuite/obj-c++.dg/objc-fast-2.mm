/* APPLE LOCAL file ObjC direct dispatch */
/* A compile-only test for insertion of 'ba' sibcall comm page jumps. */
/* Developed by Ziemowit Laski  <zlaski@apple.com>  */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -fobjc-direct-dispatch -Wno-assign-intercept -O2" } */
/* { dg-require-effective-target ilp32 } */
/* { dg-require-effective-target objc_gc } */

#include <objc/Object.h>

@interface Derived: Object {
@public
  Object *other;
}
@end

void foo(void) {
  Derived *o;
  o->other = 0;   /* sibcall to objc_assign_ivar_Fast() */
}

void bar(void) {
  Derived *o = nil;
  [Derived new];  /* sibcall to objc_msgSend_Fast() */
}

/* { dg-final { scan-assembler-not "objc_msgSend" } } */
/* { dg-final { scan-assembler-not "objc_assign_ivar" } } */

/* { dg-final { scan-assembler "ba.*fffeff00" } } */
/* { dg-final { scan-assembler "ba.*fffefec0" } } */
