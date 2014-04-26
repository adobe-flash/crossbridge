/* APPLE LOCAL file constrain visibility for templates 5813435 */
/* { dg-require-visibility "" } */
/* { dg-options "-fvisibility=hidden" } */

/* { dg-final { scan-hidden "__ZN1s6vectorI1AEC1Ev" } } */
/* { dg-final { scan-hidden "__ZN1s3fooI1AEEvT_" } } */
/* Radar 5813435 */

namespace s __attribute__((visibility("default"))) {
  template <class T>
    class vector {
  public:
    vector() { }
  };
  template <class T>
    void foo(T t) {
  }
}

class A {
public:
  A() { }
};

s::vector<A> v;

int main() {
  A a;
  s::foo(a);
}
