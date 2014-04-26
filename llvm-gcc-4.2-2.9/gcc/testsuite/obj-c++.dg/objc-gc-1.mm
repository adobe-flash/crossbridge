/* APPLE LOCAL file ObjC GC */
/* A compile-only test for insertion of write barriers. */
/* Developed by Ziemowit Laski  <zlaski@apple.com>  */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -Wno-non-lvalue-assign -Wassign-intercept" } */
/* { dg-require-effective-target objc_gc } */

#ifndef __OBJC_GC__
#error Missing __OBJC_GC__ manifest constant
#endif

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@class Underspecified;
@class MyClass;

@interface AnotherClass: Object {
@public
  __strong void *storage;
  MyClass *SomeObj;
}
- (id)assignObj:(id)obj;
@end

struct Struct1 {
  MyClass *someobj;
  void *obj2;
  __strong void *obj3;
};

struct Struct1 *str1a, str1aa;
__strong struct Struct1 *str1b, str1bb, **str1c, *str1d[3][3];

extern MyClass *externFunc(void);

@interface MyClass: Object {
@public
  id ivar1, *ivar1a;
  void *ivar2;
  __strong void *ivar3;
  Underspecified *ivar4[2], **ivar4a;
  union {
    struct {
      Underspecified *data;
      const unsigned char *dataBytes;
    } d;
    struct {
      __strong void *storage;
      AnotherClass *another;
    } s;
  } contents;
  struct {
    struct {
      void *yy;
    } z;
  } y;
}
@end

@implementation AnotherClass
- (id)assignObj:(id)obj {
  static MyClass *m_myclass;
  static id *indirect;
  str1a = 0;
  str1b = 0; /* { dg-warning "global\\/static variable assignment" } */
  str1c = 0; /* { dg-warning "global\\/static variable assignment" } */
  str1d[1][1] = 0; /* { dg-warning "global\\/static variable assignment" } */
  str1a->someobj = 0; /* { dg-warning "strong\\-cast assignment" } */
  str1b->someobj = 0; /* { dg-warning "strong\\-cast assignment" } */
  str1aa.someobj = 0; /* { dg-warning "global\\/static variable assignment" } */
  str1bb.someobj = 0; /* { dg-warning "global\\/static variable assignment" } */
  str1a->obj2 = 0;
  str1b->obj2 = 0;
  str1a->obj3 = 0; /* { dg-warning "strong\\-cast assignment" } */
  str1b->obj3 = 0; /* { dg-warning "strong\\-cast assignment" } */
  SomeObj->contents.s.another = 0; /* { dg-warning "instance variable assignment" } */
  obj = 0;
  externFunc()->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
  externFunc()->contents.s.another->SomeObj = 0; /* { dg-warning "instance variable assignment" } */
  m_myclass = 0; /* { dg-warning "global\\/static variable assignment" } */
  *indirect = obj; /* { dg-warning "strong\\-cast assignment" } */
  (__strong id)*indirect = obj; /* { dg-warning "strong\\-cast assignment" } */
  (__strong id)(MyClass *)*indirect = obj; /* { dg-warning "strong\\-cast assignment" } */
  self = 0;
  self->isa = 0; /* { dg-warning "instance variable assignment" } */
  return SomeObj = obj; /* { dg-warning "instance variable assignment" } */
}
@end

typedef MyClass MyClass1;
@compatibility_alias MyClass2 MyClass;

MyClass *g_myclass;
MyClass1 *g_myclass1;
MyClass2 *g_myclass2;
MyClass2 **g_myclass2a, ***g_myclass2b;
MyClass2 *g_myclass2c[6], *g_myclass2d[4][5];
__strong void *g_myclass2e[3];

id *g_myid, ***g_myid3;

void function(void) {
	static MyClass *l_myclass;
	MyClass2 *l_myclass2;
	
	g_myclass = 0; /* { dg-warning "global\\/static variable assignment" } */
	l_myclass = 0; /* { dg-warning "global\\/static variable assignment" } */
	g_myclass->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
	*g_myclass->ivar1a = 0; /* { dg-warning "strong\\-cast assignment" } */
	l_myclass2 = 0;
	l_myclass2->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
	(__strong id)*g_myclass->ivar1a = 0; /* { dg-warning "strong\\-cast assignment" } */
	g_myclass->ivar2 = 0;
	(__strong void *)g_myclass->ivar2 = 0; /* { dg-warning "assignment has been intercepted" } */
	g_myclass->ivar3 = 0; /* { dg-warning "instance variable assignment" } */
	l_myclass->ivar4[1] = 0; /* { dg-warning "instance variable assignment" } */
        l_myclass->ivar4a = 0; /* { dg-warning "instance variable assignment" } */
        *l_myclass->ivar4a = 0; /* { dg-warning "strong\\-cast assignment" } */
        (__strong id)*l_myclass->ivar4a = 0; /* { dg-warning "strong\\-cast assignment" } */
	l_myclass->contents.d.data = 0; /* { dg-warning "instance variable assignment" } */
	l_myclass->contents.d.dataBytes = 0;
	(__strong const unsigned char *)l_myclass->contents.d.dataBytes = 0; /* { dg-warning "strong\\-cast assignment" } */
	l_myclass->contents.s.storage = 0; /* { dg-warning "instance variable assignment" } */
	l_myclass->contents.s.another->SomeObj = 0; /* { dg-warning "instance variable assignment" } */
	l_myclass->contents.s.another->storage = 0; /* { dg-warning "instance variable assignment" } */
	(__strong void *)l_myclass->contents.s.another->storage = 0; /* { dg-warning "assignment has been intercepted" } */
	g_myclass->y.z.yy = 0;
	(__strong void *)g_myclass->y.z.yy = 0; /* { dg-warning "strong\\-cast assignment" } */
	g_myclass1->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
	g_myclass2->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
        (*g_myclass2a)->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
	*g_myid = 0; /* { dg-warning "strong\\-cast assignment" } */
	***g_myid3 = 0; /* { dg-warning "strong\\-cast assignment" } */
	(__strong id)*g_myid = 0; /* { dg-warning "strong\\-cast assignment" } */
	(__strong id)***g_myid3 = 0; /* { dg-warning "strong\\-cast assignment" } */
	g_myclass2[3] = g_myclass1[4];
	g_myclass2a[1] = 0; /* { dg-warning "strong\\-cast assignment" } */
	g_myclass2b[1][2] = 0; /* { dg-warning "strong\\-cast assignment" } */
	g_myclass2c[1] = 0; /* { dg-warning "global\\/static variable assignment" } */
	g_myclass2e[1] = 0; /* { dg-warning "global\\/static variable assignment" } */
	g_myclass2d[1][2] = 0; /* { dg-warning "global\\/static variable assignment" } */
	g_myclass2a[1]->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
	g_myclass2b[1][2]->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
	g_myclass2c[1]->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
	g_myclass2d[1][2]->ivar1 = 0; /* { dg-warning "instance variable assignment" } */
}

