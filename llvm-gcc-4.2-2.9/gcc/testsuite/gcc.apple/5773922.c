/* APPLE LOCAL file 5773922 */
/* { dg-options "-std=c99" } */
/* { dg-do run } */

#include <stdarg.h>
typedef int __attribute__((mode(QI))) qi;
typedef qi __attribute__((vector_size (16))) v16qi;
struct w { v16qi x[0]; } empty4;

void abort (void);

int foo(int x, ...) {
  va_list ap;
  va_start(ap, x);
  struct w W = va_arg(ap, struct w);
  int y = va_arg(ap, int);
  /* printf("%d\n", y); *//* should print 4 */
  return y;
}

int main() {
  int r;
  r = foo(3, empty4, 4);
  if (r != 4)
    abort ();
  return 0;
}
