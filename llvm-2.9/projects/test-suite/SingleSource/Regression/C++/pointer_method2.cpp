#include <stdio.h>

struct A {
  int a;
  virtual void foo() = 0;
  void bar() { printf("A::bar(): a=%x\n", a); }
};

struct B {
  int b;
  virtual void foo() = 0;
  void bar() { printf("B::bar(): b=%x\n", b); }
};

struct C : A, B {
  int c;
  virtual void foo() { printf("C::foo(), c=%x\n", c); }
  void bar() { printf("C::bar(), c=%x\n", c); }
};

template <class T> void invoke(C &c, void (T::*fn)()) {
  (c.*fn)();
}

int main() {
  C c;
  c.a = 0xff;
  c.b = 0xf0f;
  c.c = 0xf00f;

  invoke(c, &A::foo);
  invoke(c, &A::bar);
  invoke(c, &B::foo);
  invoke(c, &B::bar);
  invoke(c, &C::foo);
  invoke(c, &C::bar);
}
