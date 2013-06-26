#include "stdlib.h"

// PR8175

int count = 0;

struct L {
  int n;
  L(int n) : n(n) { count += n; }
  ~L() { count -= n; }
};

void test0(int n) {
  {
    L b(n);
    if (n) goto l;
  }

 l: if (count) abort();
}

void test1(int n) {
  {
    L a(n);
    L b(n);
    if (n) goto l;
  }

 l: if (count) abort();
}

void test2(int n) {
  {
    L a(n);
    {
      L b(n);
      if (n) goto l1;
    }

  l1: goto l2;
  }
 l2: if (count) abort();
}

void test3(int n) {
  {
    L a(n);
    if (n) goto l1;
    goto l1;
  }

 l1: if (count) abort();
}

int main(int argc, char *argv[]) {
  test0(1);
  test1(1);
  test2(1);
  test3(1);
  return 0;
}
