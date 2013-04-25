// APPLE LOCAL file 6755006
//#include <cassert>
/* { dg-do run } */
/* { dg-options { -m32 -Os } { target powerpc*-*-darwin* i?86-*-darwin* } } */
/* { dg-options { -Os } { target arm*-*-darwin* } } */
#include <stdio.h>
#include <stdlib.h>
enum foo {
  some_keyword0,
  NUM_OBJC_KEYWORDS = 23
};
struct IdentifierInfo {
  unsigned ObjCOrBuiltinID : 13;
};
unsigned X = 429;
int main() {
  IdentifierInfo IDs;
  for (unsigned i = 0, e = X; i != e; ++i) {
    IDs.ObjCOrBuiltinID = i+6583 + 23;
    //    assert(IDs.ObjCOrBuiltinID - 23 == i+6583 && "ID too large for field!");
    if (IDs.ObjCOrBuiltinID - 23 != i+6583) {
      abort();
      // printf("ID too large for field!  i == %d\n", i);
      // return -1;
    }
  }
  return 0;
}

