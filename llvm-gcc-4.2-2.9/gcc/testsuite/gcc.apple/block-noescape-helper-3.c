/* APPLE LOCAL file radar 6083129 byref escapes */
/* Test for generation of escape _Block_object_dispose call when a __block
   variable inside a block is declared and used. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

void *_NSConcreteStackBlock[32];
void _Block_object_assign(void * dst, void *src, int flag){}

extern void abort(void);

static int count;
static void _Block_object_dispose(void * arg, int flag) {
        ++count;
}

void junk(void (^block)(void)) {
  block();
}

int test() {
  void (^dummy)(void) = ^{ int __block i = 10; printf("i = %d\n", i); };
  junk(dummy);
  return count;
}

int main()
{
	if ( test() != 1)
	  abort();
	return 0;
}

