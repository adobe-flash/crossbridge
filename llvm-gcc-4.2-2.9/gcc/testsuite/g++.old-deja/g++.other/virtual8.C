// { dg-do run  }
extern "C" int printf (const char*, ...);

struct A
{
  virtual void f () {
    /* APPLE LOCAL default to Wformat-security 5764921 */
    printf ("%p\n", (void*)this);
  }
};

struct B : public A
{
};

struct C : public A
{
};

struct D : virtual public B, public C
{
};

int main ()
{
  D d;

  A* a1 = (A*) (B*) &d;
  A* a2 = (A*) (C*) &d;

  a1->f ();
  a2->f ();
}
