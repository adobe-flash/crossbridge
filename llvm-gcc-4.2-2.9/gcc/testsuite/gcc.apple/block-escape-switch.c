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

int foo(int p, int q) {
  __block int O1;
  switch (p) {
  case 1:
    {
      __block int I1;
      I1 += 1;
      break;
    }
  case 10:
    {
      __block int J1;
      break;
    }
  default :
    {
      __block int D1;
      __block int D2;
      switch (q) {
      case 11:
	{
	  __block int  Q1;
	  break;
	}
      default:
	{
	  __block int  ID1;
	  __block int  ID2;
	}
      };
      break;
    }
  }
  return 0;
}

int main() {
  foo (1, 0);
  if (count != 2)
    abort();

  count = 0;
  foo (12, 11);
  if (count != 4)
    abort();

  count = 0;
  foo (12, 13);
  if (count != 5)
    abort();

  return 0;
}
