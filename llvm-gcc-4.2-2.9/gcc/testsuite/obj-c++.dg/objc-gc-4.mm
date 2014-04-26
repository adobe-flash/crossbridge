/* APPLE LOCAL file ObjC GC */
/* A run-time test for insertion of write barriers. */

/* APPLE LOCAL radar 5706927 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -mmacosx-version-min=10.3" } */
/* APPLE LOCAL radar 4894756 */
/* { dg-require-effective-target ilp32 } */
/* { dg-require-effective-target objc_gc } */

#include <objc/objc.h>
#include <stdio.h>
#include <stdlib.h>

typedef const struct __CFDictionary * CFDictionaryRef;

// callouts to these are generated with cc -fobjc-gc

int GlobalAssigns;
int IvarAssigns;
int StrongCastAssigns;


id objc_assign_global(id value, __weak id *dest) {
  ++GlobalAssigns;
  return (*dest = value);
}

id objc_assign_ivar(id value, id dest, unsigned int offset) {
  id *slot = (id*) ((char *)dest + offset);

  ++IvarAssigns;
  return (*slot = value);
}

id objc_assign_strongCast(id value, __weak id *dest) {
  id base;

  ++StrongCastAssigns;
  return (*dest = value);
}

// The test case elements;
@class NSObject;
@class NSString;

typedef struct {
  id  element;
  id elementArray[10];
  __strong CFDictionaryRef cfElement;
  __strong CFDictionaryRef cfElementArray[10];
} struct_with_ids_t;

@interface Foo  {
@public
// assignments to any/all of these fields should generate objc_assign_ivar
  __strong CFDictionaryRef dict;
  __strong CFDictionaryRef dictArray[3];
  id ivar;
  id array[10];
  NSObject *nsobject;
  NSString *stringArray[10];
  struct_with_ids_t inner;
}

@end

// assignments to these should generate objc_assign_global
id GlobalId;
id GlobalArray[20];
NSObject *GlobalObject;
NSObject *GlobalObjectArray[20];
__strong CFDictionaryRef Gdict;
__strong CFDictionaryRef Gdictarray[10];
struct_with_ids_t GlobalStruct;
struct_with_ids_t GlobalStructArray[10];


// The test cases
void *rhs = 0;

#define ASSIGNTEST(expr, global) expr = (typeof(expr))rhs; if (!global) { printf(# expr " is busted\n"); ++counter; }  global = 0

int testGlobals() {
  // Everything in this function generates assign_global intercepts
  int counter = 0;

  static id staticGlobalId;
  static id staticGlobalArray[20];
  static NSObject *staticGlobalObject;
  static NSObject *staticGlobalObjectArray[20];
  static __strong CFDictionaryRef staticGdict;
  static __strong CFDictionaryRef staticGdictarray[10];
  static struct_with_ids_t staticGlobalStruct;
  static struct_with_ids_t staticGlobalStructArray[10];

  ASSIGNTEST(GlobalId, GlobalAssigns);				// objc_assign_global
  ASSIGNTEST(GlobalArray[0], GlobalAssigns);			// objc_assign_global
  ASSIGNTEST(GlobalObject, GlobalAssigns);			// objc_assign_global
  ASSIGNTEST(GlobalObjectArray[0], GlobalAssigns);		// objc_assign_global
  ASSIGNTEST(Gdict, GlobalAssigns);				// objc_assign_global
  ASSIGNTEST(Gdictarray[1], GlobalAssigns);			// objc_assign_global

  ASSIGNTEST(GlobalStruct.element, GlobalAssigns);		// objc_assign_global
  ASSIGNTEST(GlobalStruct.elementArray[0], GlobalAssigns);	// objc_assign_global
  ASSIGNTEST(GlobalStruct.cfElement, GlobalAssigns);		// objc_assign_global
  ASSIGNTEST(GlobalStruct.cfElementArray[0], GlobalAssigns);	// objc_assign_global

  ASSIGNTEST(staticGlobalId, GlobalAssigns);			// objc_assign_global
  ASSIGNTEST(staticGlobalArray[0], GlobalAssigns);		// objc_assign_global
  ASSIGNTEST(staticGlobalObject, GlobalAssigns);		// objc_assign_global
  ASSIGNTEST(staticGlobalObjectArray[0], GlobalAssigns);	// objc_assign_global
  ASSIGNTEST(staticGdict, GlobalAssigns);			// objc_assign_global
  ASSIGNTEST(staticGdictarray[1], GlobalAssigns);		// objc_assign_global

  ASSIGNTEST(staticGlobalStruct.element, GlobalAssigns);		// objc_assign_global
  ASSIGNTEST(staticGlobalStruct.elementArray[0], GlobalAssigns);	// objc_assign_global
  ASSIGNTEST(staticGlobalStruct.cfElement, GlobalAssigns);		// objc_assign_global
  ASSIGNTEST(staticGlobalStruct.cfElementArray[0], GlobalAssigns);	// objc_assign_global
    
  return counter;
}


int testIvars() {
  Foo *foo = (Foo *)malloc(sizeof(Foo)); // don't call in ObjC
  int counter = 0;

  ASSIGNTEST(foo->ivar, IvarAssigns);					// objc_assign_ivar
  ASSIGNTEST(foo->dict, IvarAssigns);					// objc_assign_ivar
  ASSIGNTEST(foo->dictArray[0], IvarAssigns);				// objc_assign_ivar
  ASSIGNTEST(foo->array[0], IvarAssigns);				// objc_assign_ivar
  ASSIGNTEST(foo->nsobject, IvarAssigns);				// objc_assign_ivar
  ASSIGNTEST(foo->stringArray[0], IvarAssigns);				// objc_assign_ivar
  ASSIGNTEST(foo->inner.element, IvarAssigns);				// objc_assign_ivar
  ASSIGNTEST(foo->inner.elementArray[0], IvarAssigns);			// objc_assign_ivar
  ASSIGNTEST(foo->inner.cfElement, IvarAssigns);			// objc_assign_ivar
  ASSIGNTEST(foo->inner.cfElementArray[0], IvarAssigns);		// objc_assign_ivar

  return counter;
}

int testStrongCasts() {
  id x = nil;
  int counter = 0;
  typedef struct { @defs(Foo) } Foo_defs;
  Foo_defs *foo = (Foo_defs *)malloc(sizeof(Foo));
    
  // strong casts should always be issued, even if the compiler could know better
    
  ASSIGNTEST((__strong id)foo->ivar, StrongCastAssigns);		// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->dict, StrongCastAssigns);		// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->dictArray[0], StrongCastAssigns);	// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->array[0], StrongCastAssigns);		// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->nsobject, StrongCastAssigns);		// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->stringArray[0], StrongCastAssigns);	// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->inner.element, StrongCastAssigns);	// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->inner.elementArray[0], StrongCastAssigns);// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->inner.cfElement, StrongCastAssigns);	// objc_assign_strongCast
  ASSIGNTEST((__strong id)foo->inner.cfElementArray[0], StrongCastAssigns);// objc_assign_strongCast

  // assignments to declared __strong on plain structure elements shouldn't work
    
  return counter;
}

@implementation Foo
@end

int main(int argc, char *argv[]) {
  int errors = 0;
  errors += testGlobals();
  errors += testIvars();
  errors += testStrongCasts();
  return (errors != 0);
}
