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

void objc_get_type_qualifiers (int flag, int type) {
  while (flag--)
    while (type++ < 4) {
      __block int W1;
      __block int W2;
      if (type == 2)
	break;
    }
}

int main() {
  objc_get_type_qualifiers (1, 0);
  if (count != 4)
    abort();
  return 0;
}
