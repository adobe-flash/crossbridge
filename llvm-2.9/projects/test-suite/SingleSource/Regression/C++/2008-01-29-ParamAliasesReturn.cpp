#include <stdio.h>

class foo
{
public:
  int a;
  int b;
  int c;
  int d;

  foo(void) : a(0), b(0) {}

  foo(int aa, int bb) : a(aa), b(bb) {}

  const foo operator+(const foo& in) const;

  foo operator+=(const foo& in);
};

const foo foo::operator+(const foo& in) const {
  foo Out;
  Out.a = a + in.a;
  Out.b = b + in.b;
  return Out;
}

foo foo::operator+=(const foo& in) {
  *this = *this + in;
  return *this;
}

int main() {
  foo x(1, 2);
  foo y(3, 4);
  x += y;
  printf("%d %d\n", x.a, x.b);
  return 0;
}
