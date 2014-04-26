/* APPLE LOCAL file radar 4441551 */
/* Issue warning wherevr @defs is used. */
/* { dg-options "-lobjc -Wobjc2" } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */

#include <objc/objc.h>
#include <objc/Object.h>

extern void abort(void);

#define CHECK_IF(expr) if(!(expr)) abort();

@interface Base: Object {
@public
  int a;
  float b;
  char c;
}
@end

@interface Derived: Base {
@public
  double d;
  unsigned e;
  id f;
} 
- init;
@end

struct Derived_defs {
  @defs(Derived);	/* { dg-warning "@defs will not be supported in future" } */
};

@implementation Base
@end
@implementation Derived
- init {
  [super init];
  a = 123;
  b = 1.23;
  c = 'c';
  d = 123.456;
  e = 456;
  f = isa;
  return self;
}
@end

int main(void) {
  Derived *derived = [[Derived alloc] init];
  struct Derived_defs *derived_defs = (struct Derived_defs *)derived;

  CHECK_IF(derived->a == derived_defs->a && derived_defs->a == 123);
  CHECK_IF(derived->b == derived_defs->b && derived_defs->b == (float)1.23);  
  CHECK_IF(derived->c == derived_defs->c && derived_defs->c == 'c');  
  CHECK_IF(derived->d == derived_defs->d && derived_defs->d == (double)123.456);  
  CHECK_IF(derived->e == derived_defs->e && derived_defs->e == 456);  
  CHECK_IF(derived->f == derived_defs->f && derived_defs->f == derived_defs->isa);

  /* Try out the "inline" notation as well.  */
  CHECK_IF(((struct { @defs(Derived); } *)derived)->a == 123); /* { dg-warning "@defs will not be supported in future" } */
  CHECK_IF(((struct { @defs(Derived); } *)derived)->c == 'c'); /* { dg-warning "@defs will not be supported in future" } */
  CHECK_IF(((struct { @defs(Derived); } *)derived)->e == 456); /* { dg-warning "@defs will not be supported in future" } */

  return 0;
}
