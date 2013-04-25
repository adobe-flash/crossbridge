/* APPLE LOCAL file radar 4426814 */
/* A run-time test for insertion of write barriers for __weak objects. */

/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -Wno-non-lvalue-assign" } */
/* { dg-require-effective-target objc_gc } */

#include <objc/objc.h>
#include <stdio.h>
#include <stdlib.h>

typedef const struct __CFDictionary * CFDictionaryRef;

// callouts to these are generated with cc -fobjc-gc

int GlobalAssigns;

id objc_read_weak(id *value) {
  return *value;
}

id objc_assign_weak(id value, id *dest) {
  ++GlobalAssigns;
  return ((__weak id)*dest = value);
}

// The test case elements;
@class NSObject;
@class NSString;

@interface Foo  {
@public
// assignments to any/all of these fields should generate objc_assign_weak
  __weak CFDictionaryRef dict;
  __weak CFDictionaryRef dictArray[3];
  __weak id ivar;
  __weak id array[10];
  __weak NSObject *nsobject;
  __weak NSString *stringArray[10];
}

@end

// assignments to these should generate objc_assign_weak
__weak id GlobalId;
__weak id GlobalArray[20];
__weak NSObject *GlobalObject;
__weak NSObject *GlobalObjectArray[20];
__weak CFDictionaryRef Gdict;
__weak CFDictionaryRef Gdictarray[10];

// The test cases
void *rhs = 0;

#define ASSIGNTEST(expr, global) expr = (typeof(expr))rhs; if (!global) { printf(# expr " is busted\n"); ++counter; }  global = 0

int testGlobals() {
  // Everything in this function generates objc_assign_weak intercepts
  int counter = 0;

  static __weak id staticGlobalId;
  static __weak id staticGlobalArray[20];
  static __weak NSObject *staticGlobalObject;
  static __weak NSObject *staticGlobalObjectArray[20];
  static __weak CFDictionaryRef staticGdict;
  static __weak CFDictionaryRef staticGdictarray[10];
  ASSIGNTEST(GlobalId, GlobalAssigns);				// objc_assign_weak
  ASSIGNTEST(GlobalArray[0], GlobalAssigns);			// objc_assign_weak
  ASSIGNTEST(GlobalObject, GlobalAssigns);			// objc_assign_weak
  ASSIGNTEST(GlobalObjectArray[0], GlobalAssigns);		// objc_assign_weak
  ASSIGNTEST(Gdict, GlobalAssigns);				// objc_assign_weak
  ASSIGNTEST(Gdictarray[1], GlobalAssigns);			// objc_assign_weak


  ASSIGNTEST(staticGlobalId, GlobalAssigns);			// objc_assign_weak
  ASSIGNTEST(staticGlobalArray[0], GlobalAssigns);		// objc_assign_weak
  ASSIGNTEST(staticGlobalObject, GlobalAssigns);		// objc_assign_weak
  ASSIGNTEST(staticGlobalObjectArray[0], GlobalAssigns);	// objc_assign_weak
  ASSIGNTEST(staticGdict, GlobalAssigns);			// objc_assign_weak
  ASSIGNTEST(staticGdictarray[1], GlobalAssigns);		// objc_assign_weak

    
  return counter;
}


int testIvars() {
  Foo *foo = (Foo *)malloc(sizeof(Foo)); // don't call in ObjC
  int counter = 0;

  ASSIGNTEST(foo->ivar, GlobalAssigns);					// objc_assign_weak
  ASSIGNTEST(foo->dict, GlobalAssigns);					// objc_assign_weak
  ASSIGNTEST(foo->dictArray[0], GlobalAssigns);				// objc_assign_weak
  ASSIGNTEST(foo->array[0], GlobalAssigns);				// objc_assign_weak
  ASSIGNTEST(foo->nsobject, GlobalAssigns);				// objc_assign_weak
  ASSIGNTEST(foo->stringArray[0], GlobalAssigns);				// objc_assign_weak
  return counter;
}
@implementation Foo
@end

int main(int argc, char *argv[]) {
  int errors = 0;
  errors += testGlobals();
  errors += testIvars();
  return (errors != 0);
}
