/* APPLE LOCAL file radar 5988995 */
/* Type compatibility issue for blocks which return blocks. */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

int (^f)(char, int);

int (^farray[10])(float, int);

int (^func(int x))(char, int) {
   return f;
}

// intent: block taking int returning block that takes char,int and returns int
int (^(^block)(double x))(char, short);

void foo() {
   int one = 1;
   /* APPLE LOCAL radar 6230297 */
   block = ^(double x){ return ^(char c, short y) { return one + (int)c + y; };}; /* { dg-error "returning block that lives on the local stack" } */
}

