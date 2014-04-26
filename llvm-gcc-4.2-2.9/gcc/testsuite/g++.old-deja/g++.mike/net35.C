// { dg-do run  }
extern "C" int printf(const char *, ...);

class A {
 public:
   char *x;
};

class B1:public virtual A { };

class B2:public virtual A { };

class C:public B1, public B2 {
public:
  C() { }
};

int main() {
  C c;
  /* APPLE LOCAL begin default to Wformat-security 5764921 */
  printf("&c.x = %p\n", (void*)&c.x);
  printf("&c.B1::x = %p\n", (void*)&c.B1::x);
  printf("&c.B2::x = %p\n", (void*)&c.B2::x);
  printf("&c.A::x = %p\n", (void*)&c.A::x);
  /* APPLE LOCAL end default to Wformat-security 5764921 */
  if (&c.x != &c.B1::x
      || &c.x != &c.B2::x
      || &c.x != &c.A::x)
    return 1;
}
