// APPLE LOCAL file radar 4949034
// Check if the '- .cxx_construct' and '-.cxx_destruct' methods get called
// and if they perform their desired function.
// This test checks that -fobjc-call-cxx-cdtors is the default with -m64. 
// This test must compile with no diagnostics.
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -m64" } */


/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"
#include <stdlib.h>
#define CHECK_IF(expr) if(!(expr)) abort()

static int ctor1_called, ctor2_called, dtor1_called;

struct bar {
  int a, b;
  bar(void) {
    a = 5; b = 6;
    ctor1_called++;
  }
  ~bar(void) {
    a = b = 99;
    dtor1_called++;
  }
};

struct boo: bar {
  int c;
  boo(int _c = 9): c(_c) {
    ctor2_called++;
  }
};

@interface Baz: Object {
@public
  bar aa;
}
@end

@implementation Baz
@end

@interface Foo: Baz {
@public
  int a;
  boo bb;
  bar b;
  float c;
  bar d;
}
@end

@implementation Foo
@end

int main (void)
{
  CHECK_IF(!ctor1_called && !ctor2_called && !dtor1_called); /* are we sane? */

  Baz *baz = [Baz new];
  CHECK_IF(ctor1_called && !ctor2_called && !dtor1_called);
  CHECK_IF(baz->aa.a == 5 && baz->aa.b == 6);
  ctor1_called = 0;  /* reset */
  
  [baz free];
  CHECK_IF(!ctor1_called && !ctor2_called && dtor1_called);
  dtor1_called = 0;  /* reset */

  Foo *foo = [Foo new];
  CHECK_IF(ctor1_called && ctor2_called && !dtor1_called);
  CHECK_IF(foo->bb.a == 5 && foo->bb.b == 6 && foo->bb.c == 9);
  CHECK_IF(foo->b.a == 5 && foo->b.b == 6);
  CHECK_IF(foo->d.a == 5 && foo->d.b == 6);
  ctor1_called = ctor2_called = 0;  /* reset */
  
  [foo free];
  CHECK_IF(!ctor1_called && !ctor2_called && dtor1_called);

}
