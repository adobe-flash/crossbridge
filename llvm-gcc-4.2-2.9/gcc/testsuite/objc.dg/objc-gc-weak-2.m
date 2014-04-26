/* APPLE LOCAL file radar 4426814 */
/* A run-time test for insertion of read barriers for __weak objects. */

/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

#include <objc/objc.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct __CFDictionary * CFDictionaryRef;

// callouts to these are generated with cc -fobjc-gc

int GlobalReads;


static id objc_read_weak(id *location) {
  ++GlobalReads;
  return *location;
}

static id objc_assign_weak(id value, id *dest) {
  /* APPLE LOCAL radar 5276985 */
  return (*dest = value);
}


// The test case elements;
@class NSObject;
@class NSString;

@interface Foo  
{
@public
// read of  __weak fields should generate objc_read_weak
  __weak CFDictionaryRef dict;
  __weak CFDictionaryRef dictArray[3];
  __weak id ivar;
  __weak id array[10];
  __weak NSObject *nsobject;
  __weak NSString *stringArray[10];
}
@end

@implementation Foo
- (void)message {}
- (void)message1 : (id)arg {}
@end

#define READTEST(expr, count)	if (GlobalReads != count) { printf(# expr " is busted\n"); ++counter; } --GlobalReads;

id get_id()
{
  static id x;
  return (id)&x;
}

int testGlobals() {
  // Everything in this function generates objc_assign_weak intercepts
  int counter = 0;

  static __weak id staticGlobalId = 0;
  static __weak id staticGlobalArray[20] = {0};
  static __weak NSObject *staticGlobalObject;
  static __weak NSObject *staticGlobalObjectArray[20];
  static __weak CFDictionaryRef staticGdict;
  static __weak CFDictionaryRef staticGdictarray[10] = {0,0,0};


  if (!staticGlobalId)
    READTEST(staticGlobalId,1);

  if (!staticGlobalArray[0])
    READTEST(staticGlobalArray[0],1);

  staticGlobalObject = get_id();

  if (staticGlobalObject)
    READTEST(staticGlobalObject,1);

  staticGdict = (__weak CFDictionaryRef)get_id();
  READTEST(get_id, 1);
  if (staticGlobalId || staticGdictarray[1] || staticGdict)
    {
	READTEST(staticGlobalId,3);
	READTEST(staticGlobalArray[1],2);
	READTEST(staticGdict,1);
    }

  return counter;
}


int testIvars() {
  int counter = 0;
  Foo *foo = (Foo *)calloc(sizeof(Foo), 1); // don't call in ObjC

  if (!foo->ivar)
    READTEST(foo->ivar, 1);
  if (!foo->dict)
    READTEST(foo->dict, 1);

  foo->ivar = get_id();
  if (foo->dict || foo->dictArray[0] || foo->stringArray[0] || foo->ivar)
    {
  	READTEST(foo->dict, 4);			
  	READTEST(foo->dictArray[0], 3);	
  	READTEST(foo->stringArray[0], 2);
  	READTEST(foo->ivar, 1);	
    }
  [foo->array[0] message];
  READTEST(foo->array, 1);

  [foo->array[0] message1 : foo->ivar];
  READTEST(foo->array, 2);

  return counter;
}

int main(int argc, char *argv[]) {
  int errors = 0;
  errors += testGlobals();
  errors += testIvars();
  return (errors);
}
