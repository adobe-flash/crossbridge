/* APPLE LOCAL file radar 5957801 */
/* Test for use of block pointer in a ?-exp expression. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

typedef int (^myblock)(void);
void *_NSConcreteStackBlock[32];

myblock foo(int i, myblock b) {
  if (!i ? (void *)0 : b)
    return (i ? b : (void *)0);
}

int main () {
  myblock b = ^{ return 1; };
  if (foo (1, b))
    return 0;
  return 1;
}
