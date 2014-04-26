/* APPLE LOCAL file radar 5803005 */
/* Test that all global variables referenced in blocks are treated as 'byref' as default. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.5 -fblocks" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
void * _NSConcreteStackBlock[32];

void CallBlock (void (^voidvoidBlock)(void)) {
   voidvoidBlock();
}

int glob = 10;
static int stat = 10;

int foo() {
  static int local = 10;
  CallBlock( ^ { ++glob; ++stat; ++local;
	         CallBlock(^ { ++glob; ++stat; ++local; });
		 ++glob; ++stat; ++local; });

  if (glob != 13 || stat != 13 || local != 13)
    return 1;
  return 0;
}

int main() {
  return foo ();
}

