/* APPLE LOCAL file radar 6083129 byref escapes */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

extern void abort(void);

static int count;
static void _Block_object_dispose(void * arg, int flag) {
  printf ("%p\n", arg);
  ++count;
}

int main() {
 {
  __block int O1;
  int i;
  int p;
  for (i = 1; i <= 5; i++) {
    __block int I1;
    p = 0;
    while (p != 10) {
      __block int II1;
      if (p == 2)
	break;
      ++p;
    }
  }
 }
  return count-21;
}
