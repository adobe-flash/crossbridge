/* APPLE LOCAL file radar 6177162 */
/* local statics need be treated same as file static (byref). */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

int main(int argc, char **argv) {
  static int numberOfSquesals = 5;

  ^{ numberOfSquesals = 6; }();

  if (numberOfSquesals == 6) { printf("%s: success\n", argv[0]); }

  return 0;
}
