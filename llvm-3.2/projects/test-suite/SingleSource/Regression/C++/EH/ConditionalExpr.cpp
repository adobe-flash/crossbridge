#include <stdio.h>

static int X = 0;

struct T {
  int V;
  T() : V(++X) { printf("Construct %d\n", V); }
  T(const T &) : V(++X) { printf("Copy Construct %d\n", V); }
  ~T() { printf("Destruct %d\n", V); }

  void operator=(const T &t) {
    printf("Overwrite %d with %d\n", V, t.V);
    V = t.V;
  }
};

T func(const T &t) { return T(); }

T test(bool C) {
  return C ? T() : func(T());
}

int main() {
  T x;
  x = test(true);
  return 0;
}
