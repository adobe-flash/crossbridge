/* APPLE LOCAL file radar 4084991 */
/* { dg-do run } */

#include <assert.h>

enum Foo { A, B };

template<typename T> T &qMin(T &a, T &b) 
{
  return a < b ? a : b;
}

int main (int,  char **)
{
  Foo f = A;
  Foo g = B;
  Foo &h = qMin(f, g);
  assert (&h == &f || &h == &g);
  const Foo &i = qMin((const Foo&)f, (const Foo&)g);
  assert (&i == &f || &i == &g);
  return 0;
}

