/* APPLE LOCAL file 4168392 */
/* Support for Microsoft-style anonymous union and
   struct aggregation.  */

/* { dg-options "-fms-extensions -pedantic" } */
/* { dg-do run } */

#include <stdlib.h>
#include <stddef.h>
#define CHECK_IF(E) if(!(E)) abort()

typedef struct _FOO {
  int a;
  int b;
} FOO;

typedef struct _BAZ {
  int c;
} BAZ;

typedef struct _BAR {
  int bar;
  struct {
    BAZ;  /* { dg-warning "ISO C doesn't support unnamed structs/unions" } */
    BAZ baz;
  }; /* { dg-warning "ISO C doesn't support unnamed structs/unions" } */
  union {
    int e;
    FOO; /* { dg-warning "ISO C doesn't support unnamed structs/unions" } */
    FOO foo;
  }; /* { dg-warning "ISO C doesn't support unnamed structs/unions" } */
} BAR;

BAR g;

int main(void) {
  CHECK_IF (sizeof (g) == 5 * sizeof (int));

  g.e = 4;
  g.c = 5;
  g.baz.c = 6;
  g.foo.b = 7;
  CHECK_IF (g.b == 7);
  CHECK_IF (g.a == 4);
  CHECK_IF (g.foo.a == 4);

  CHECK_IF (offsetof(BAR, a) == offsetof(BAR, e));
  CHECK_IF (offsetof(BAR, a) == offsetof(BAR, foo));
  CHECK_IF (offsetof(BAR, e) - offsetof(BAR, c) >= 2 * sizeof(BAZ));

  return 0;
}   
