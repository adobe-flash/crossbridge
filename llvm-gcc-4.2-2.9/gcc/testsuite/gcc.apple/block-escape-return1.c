/* APPLE LOCAL file radar 6083129 block escapes */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

extern void abort(void);
void _Block_object_assign(void *a, void *b, int flag){};

static int count;
static void _Block_object_dispose(void * arg, int flag) {
  printf ("%p\n", arg);
  ++count;
}

void *_NSConcreteStackBlock[32];

void FOO(int arg) {
  __block  int X = 1234;
  if (arg) {
    __block int local_BYREF = 100;
    X += 100 + local_BYREF;
    return;
  }
  ^{ X++; };	
  X = 1000;
}

int main() {
  FOO(1);
  if (count != 2)
    abort();

  count = 0;
  FOO(0);
  if (count != 1)
    abort();
  return 0;
}
