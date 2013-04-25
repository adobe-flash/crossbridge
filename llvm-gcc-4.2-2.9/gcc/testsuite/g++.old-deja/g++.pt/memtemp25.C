// { dg-do link  }
// GROUPS passed templates membertemplates
extern "C" int printf(const char*, ...);

template <class X>
struct S
{
  template <class U>
  /* APPLE LOCAL default to Wformat-security 5764921 */
  void f(U u) { printf ("%d\n", (int)sizeof (U)); }

  int i[4];
};


int main()
{
  S<char*> s;
  s.f(3);
  s.f(s);
}
