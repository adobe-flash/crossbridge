#include <stdio.h>

struct C {
  unsigned bv;
  ~C() {}
};

static unsigned test(C b) {
  return b.bv;
}


int main() {
  C c;
  c.bv = 1234;
  printf("%d\n", test(c));
  return 0;
}
