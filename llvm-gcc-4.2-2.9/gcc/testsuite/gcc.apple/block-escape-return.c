/* APPLE LOCAL file radar 6083129 byref escapes */
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


int main1() {
  __block  int X = 1234;
  if (X) {
    __block int local_BYREF = 100;
    X += 100 + local_BYREF;
    return count-2;
  }
  return -1;
}

int main() {
  main1();
  return count-2;
}
