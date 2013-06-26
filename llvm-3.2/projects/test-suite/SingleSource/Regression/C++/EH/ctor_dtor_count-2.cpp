// Testcase for proper handling of
// c++ type, constructors and destructors.

#include <stdio.h>

int c, d;

struct A
{
  int i;
  A () { i = ++c; printf ("A() %d\n", i); }
  A (const A&) { i = ++c; printf ("A(const A&) %d\n", i); }
  ~A() { printf ("~A() %d\n", i); ++d; }
};

void
f()
{
  printf ("Throwing 1...\n");
  throw A();
}


int main() {
  try
    {
      f();
    }
  catch (A)
    {
      printf ("Caught.\n");
    }
  printf ("c == %d, d == %d\n", c, d);
  return c != d;
}

