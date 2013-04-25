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
    int p = 0;
    do {
      __block int I1;
      do {
	__block int J1;
	if (p == 2)
	  break;
      } while ( ++p < 3);
      if (p == 4)
	break;
    } while (++p != 5);
  }

  return count-7;
}
