/* APPLE LOCAL file ObjC GC */
/* A run-time test for insertion of write barriers. */

/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc" } */
/* { dg-options "-fnext-runtime -fobjc-gc -mmacosx-version-min=10.3" { target powerpc*-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */
/* { dg-require-effective-target objc_gc } */

#include <objc/objc.h>
#include <stdio.h>
#include <stdlib.h>

// callouts to these are generated with cc -fobjc-gc

int IvarAssigns;

static
id objc_assign_ivar(id value, id dest, unsigned int offset) {
  id *slot = (id*) ((char *)dest + offset);

  ++IvarAssigns;
  return (*slot = value);
}

// The test case elements;
@class NSObject;

@interface Foo  {
@public
// assignments to 'obj' below should generate objc_assign_ivar
  Foo *obj[20];
  short idx[5];
}
@end

int testIvars() {
  Foo *foo = (Foo *)calloc(1,sizeof(Foo)); // don't call in ObjC
  int counter = 0, errors = 0;
#define ASSIGNTEST(expr, global) expr = foo; if (!global) { printf(# expr " is busted\n"); ++errors; }  global = 0

  ASSIGNTEST(foo->obj[5], IvarAssigns);			// objc_assign_ivar
  ASSIGNTEST(foo->obj[++counter], IvarAssigns);		// objc_assign_ivar
  foo->idx[++counter] = 15;
  ASSIGNTEST(foo->obj[foo->idx[2]], IvarAssigns);	// objc_assign_ivar

  if (foo->obj[5] != foo || foo->obj[1] != foo || foo->obj[15] != foo) 
    abort();
 
  return errors;
}

@implementation Foo
@end

int main(int argc, char *argv[]) {
  return testIvars();
}
