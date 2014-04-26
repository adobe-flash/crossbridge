/* APPLE LOCAL file radar 5732232, 6034839, 6230297 - blocks */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

void I( void (^)(void));
void (^noop)(void);

void nothing();
int printf(const char*, ...);

typedef void (^T) (void);

void takeblock(T);
int takeintint(int (^C)(int)) { return C(4); }

T somefunction() {
  if (^{ })
    nothing();

  noop = ^{};

  noop = ^{printf("\nBlock\n"); };

  I(^{ });

  noop = ^; /* { dg-error "expected" } */

  return ^{printf("\nBlock\n"); };  
}

void test2() {
  int x = 4;

  takeblock(^{ printf("%d\n", x); });
  takeblock(^{ x = 4; });  /* { dg-error "assignment of read-only variable" } */
}

void (^test3())(void) {
  __block int i;
  return ^{ i = 1; };    /* { dg-error "returning block that lives on the local stack" } */
}

void test4() {
  void (^noop)(void) = ^{};
  void (*noop2)() = 0;
}

void test5() {
  int y;

  int (^c)(char);
  (1 ? c : 0)('x');
  (1 ? 0 : c)('x');

  (1 ? c : c)('x');
}
