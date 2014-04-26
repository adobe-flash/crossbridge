/* APPLE LOCAL file ObjC direct dispatch */
/* A compile-only test for insertion of 'bla' and 'ba' sibcall comm page jumps
   for methods returning 'void'. */
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
- (void) do_nothing;
@end

int a;

void foo(void) {
  Derived *o;
  [o do_nothing];   /* objc_msgSend_Fast() */
  a = 2;  
}

void bar(void) {
  Derived *o = nil;
  [o do_nothing];  /* sibcall to objc_msgSend_Fast() */
}

/* { dg-final { scan-assembler-not "objc_msgSend" } } */

/* { dg-final { scan-assembler "bla.*fffeff00" } } */
/* { dg-final { scan-assembler "ba.*fffeff00" } } */
