/* APPLE LOCAL file ObjC GC */
/* Do _not_ generate write barriers for global function pointers,
   even ones returning 'id'.  */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -Wno-non-lvalue-assign -Wassign-intercept" } */
/* { dg-require-effective-target objc_gc } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Base: Object {
  int a;
}
@end

static IMP globalIMP = 0;

void foo(void) {
/* APPLE LOCAL radar 4923914 */
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
   Class ObjectClass = objc_getClass ("Object");
   IMP myIMP = method_getImplementation(class_getInstanceMethod (ObjectClass, @selector(new)));
#else
   IMP myIMP = [Object methodFor:@selector(new)];
#endif
   globalIMP = myIMP;
}

void bar(void) {
  Object *obj = 0;
  obj = [Object new];
  (Base *)obj = [Base new];  
}

void baz(id *b1) {
  id a1[4];
  int i;
  for(i = 0; i < 4; ++i) {
    a1[i] = b1[i];
  }
}
