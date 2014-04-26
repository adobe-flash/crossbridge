/* APPLE LOCAL file 5559195 */
// { dg-do run }

// Verify that an empty statement list does not cause the destructor to be
// marked as trivial.

int N = 1;

class A {
public:
  ~A () {
    N = 0;
    {}
  }
};

class B : public A {
};

int
main ()
{
  { B b; }
  return N;
}
