/* APPLE LOCAL file ObjC GC */
/* A compile-only test for insertion of write barriers. */
/* Provided by Blaine Garst  <blaine@apple.com>  */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -Wassign-intercept" } */
/* { dg-require-effective-target objc_gc } */

#ifndef __OBJC_GC__
#error Missing __OBJC_GC__ manifest constant
#endif

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

extern id **somefunc(void);
extern id *somefunc2(void);


// Globals

id W, *X, **Y;

void func(id a, id *b, id **c) {
   static id w, *x, **y;
   W = a;  /* { dg-warning "global\\/static variable assignment" } */
   w = a;  /* { dg-warning "global\\/static variable assignment" } */
   X = b;  /* { dg-warning "global\\/static variable assignment" } */
   x = b;  /* { dg-warning "global\\/static variable assignment" } */
   Y = c;  /* { dg-warning "global\\/static variable assignment" } */
   y = c;  /* { dg-warning "global\\/static variable assignment" } */
}

// Instances

@interface something {
    id w, *x, **y;
}
@end
@implementation something
- (void)amethod {
    id badIdea = *somefunc2();
    w = badIdea;   /* { dg-warning "instance variable assignment" } */
    x = &badIdea;  /* { dg-warning "instance variable assignment" } */
    y = &x;        /* { dg-warning "instance variable assignment" } */
}
@end

// Structure access, pointer dereferencing

typedef struct {
    int junk;
    id  alfred;
} AStruct;

void funct2(AStruct *aptr) {
    id **ppptr = somefunc();
    aptr->alfred = [Object new];  /* { dg-warning "strong\\-cast assignment" } */
    **ppptr = aptr->alfred;       /* { dg-warning "strong\\-cast assignment" } */
    *ppptr = somefunc2();         /* { dg-warning "strong\\-cast assignment" } */
}

