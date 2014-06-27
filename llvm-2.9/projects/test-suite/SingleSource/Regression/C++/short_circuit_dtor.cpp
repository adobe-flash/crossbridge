#include <stdio.h>

struct X {
  int Val;
  X(int i) : Val(i) { printf("X::X(%d)\n", Val); }
  ~X() { printf("X::~X(%d)\n", Val); }
};

bool foo(const X &) { return true; }
void bar() {}
int main() {
  if ((foo(1) || foo(2)))
    bar();
  return 0;
}
