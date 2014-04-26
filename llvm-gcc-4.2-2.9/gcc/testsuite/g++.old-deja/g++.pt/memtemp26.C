// { dg-do link  }
// GROUPS passed templates membertemplates
extern "C" int printf(const char*, ...);

template <class X>
struct S
{
  template <class U>
  void f(U u);

  int i[4];
};


template <class X>
template <class U>
void S<X>::f(U u)
{
  /* APPLE LOCAL default to Wformat-security 5764921 */
  printf ("%d\n", (int)sizeof (U));
}


int main()
{
  S<char*> s;
  s.f(3);
  s.f(s);
}
