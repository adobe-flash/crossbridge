// { dg-do link  }
// GROUPS passed templates membertemplates
extern "C" int printf(const char*, ...);

template <class X>
struct S
{
  S()
  { printf ("In S::S()\n"); f(3); }
  
  S(char)
  { printf ("In S::S(char)\n"); f(*this); }

  template <class U>
  void f(U u)
  /* APPLE LOCAL default to Wformat-security 5764921 */
  { printf ("In S::f(U)\nsizeof(U) == %d\n", (int)sizeof(u)); }

  int c[16];
};

int main()
{
  S<char*> s;
}
