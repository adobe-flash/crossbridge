#include <cstdio>

// Test for member function pointers.


struct A {
  virtual void f0() {
    printf("CHECK 0\n");
  }
  virtual void f1() {
    printf("CHECK 1\n");
  }
};
typedef void (A::*MFP)();

MFP f0_a() {
  return &A::f0;
}
MFP f0_b() {
  return &A::f1;
}
int main() {
  A a;

  // Check conversion to bool. We explicitly check a virtual function whose
  // offset will be 0, on some platforms this may be a corner case.
  MFP x = f0_a();
  MFP y = f0_b();
  if (x)
    (a.*x)();
  if (y)
    (a.*y)();

  return 0;
}
