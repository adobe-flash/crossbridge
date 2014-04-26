/* APPLE LOCAL file non lvalue assign */
/* { dg-do run } */
/* { dg-options "-fnon-lvalue-assign" } */

#include <stdlib.h>
#define CHECK_IF(expr) if (!(expr)) abort ()

static int global;

void f(int &) { global = 35; }
void f(const int &) { global = 78; }

long long_arr[2];

int main(void) {

  char *p;

  (long *)p = long_arr; /* { dg-warning "target of assignment not really an lvalue" } */
  ((long *)p)++;        /* { dg-warning "target of assignment not really an lvalue" } */
  *(long *)p = -1;

  *p = -2;
  CHECK_IF(p[-1] == 0 && p[0] == -2 && p[1] == -1);

  (long *)p += 2;  /* { dg-warning "target of assignment not really an lvalue" } */
  (long *)p -= 2;  /* { dg-warning "target of assignment not really an lvalue" } */

  long x = 0;
  f((int)x);
  CHECK_IF(global == 78);
  
  return 0;
}
