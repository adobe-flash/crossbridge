/* APPLE LOCAL file radar 5732232 - blocks */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

void take(void*);

void test() {
  take(^(int x){});
  take(^(int x, int y){});
  take(^(int x, int y){});
  take(^(int x, int x){});  /* { dg-error "redeclaration of" } */
			    /* { dg-error "previously declared" "" { target *-*-* } 11 } */


  take(^(int x) { return x+1; });

  int (^CP)(int) = ^(int x) { return x*x; };
  take(CP);

  int arg;
  ^{return 1;}();
  ^{return 2;}(arg);  /* { dg-error "too many arguments to block call" } */
  ^(void){return 3;}(1);  /* { dg-error "too many arguments to block call" } */
  ^(){return 4;}(arg);   /* { dg-error "too many arguments to block call" } */
  ^(int x, ...){return 5;}(arg, arg);   /* Explicit varargs, ok. */
}
