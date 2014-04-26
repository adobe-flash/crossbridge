/* APPLE LOCAL file radar 6180456  */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -fblocks" { target *-*-darwin*  } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

int main(int argc, char **argv) {
  __block void(*bobTheFunction)(void);
  __block void(^bobTheBlock)(void);

  bobTheBlock = ^{;};

  return 0;
}

