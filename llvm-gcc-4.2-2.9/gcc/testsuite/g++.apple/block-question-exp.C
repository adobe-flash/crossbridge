/* APPLE LOCAL file radar 5957801 */
/* Cannot convert void * to block pointer in c++ mode */
/* { dg-options "-fblocks" } */
/* { dg-do compile } */

typedef int (^myblock)(void);
void *_NSConcreteStackBlock[32];

myblock foo(int i, myblock b) {
  if (!i ? (void *)0 : b)
    return (myblock)(i ? b : (void *)0);
}

int main () {
  myblock b = ^{ return 1; };
  if (foo (1, b))
    return 0;
  return 1;
}
