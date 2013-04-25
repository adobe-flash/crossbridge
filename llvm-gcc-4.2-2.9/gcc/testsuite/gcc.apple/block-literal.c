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

  noop = ^noop /* { dg-error "expected specifier" } */ 
	       /*  { dg-error "blocks require" "" { target *-*-* } 26 } */
    ;  /* { dg-error "argument list is required for block expression literals" } */

  return ^{printf("\nBlock\n"); };  
}

void test2() {
  int x = 4;

  takeblock(^{ printf("%d\n", x); });
  takeblock(^{ x = 4; });  /* { dg-error "assignment of read-only variable" } */

  takeblock(^test2() /* { dg-error "expected specifier" } */  
		     /* { dg-error "blocks require" "" { target *-*-* } 39 } */
	    );	/* { dg-error "argument list is required for block expression literals" } */

  takeblock(^(void)(void)printf("hello world!\n")); /* { dg-error "blocks require" } */
}

void (^test3())(void) {
  /* APPLE LOCAL radar 6230297 */
  return ^{};    
}

void test4() {
  void (^noop)(void) = ^{};
  void (*noop2)() = 0;
}

void test5() {
  takeintint(^(int x)(x+1)); /* { dg-error "blocks require" } */

  // Block expr of statement expr.
  takeintint(^(int x)({ /* { dg-error "blocks require" } */
			return 42; }));   /* { dg-error "return not allowed in block expression literal" } */

  int y;
  takeintint(^(int x)(x+y)); /* { dg-error "blocks require" } */

  void *X = ^(x+r);  /* { dg-error "expected" } */

  int (^c)(char);
  (1 ? c : 0)('x');
  (1 ? 0 : c)('x');

  (1 ? c : c)('x');
}
