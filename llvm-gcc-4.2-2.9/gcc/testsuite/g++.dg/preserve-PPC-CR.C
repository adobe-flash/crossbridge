// APPLE LOCAL file preserve CR2 for save_world prologues
// This testcase failed at -O2 due to a missing EH note describing the PowerPC Condition Register.
// Thanks to Dale Johannesen.

// { dg-do run }
// { dg-options "-fpascal-strings" }
#include <stdlib.h>
#include <stdio.h>
int tick = 1;
int caught_x = 1;
int h() { return 2; }
void f()
{ throw(3); }
extern int h();
void ff() {
  bool xx = h() == 0;
  if ( !xx ) {
    try {
      f();
    } catch (float f) {
      if (!xx) printf("%f\n", f);
    }
  }
}
int g(int y)
{
  bool x = h() != 0;
  if ( x) {
    try {
      ff();
    } catch (int ex) {
      //      if (x) printf("%d\n", ex);
      if (x) { tick++; caught_x = ex; }
    }}
}
main()
{
  g(3);
  if (tick != 2 || caught_x != 3)
    abort();
}
