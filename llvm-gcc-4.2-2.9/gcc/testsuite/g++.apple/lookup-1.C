/* APPLE LOCAL file 6322334 */
/* Radar 6322334 */

class A {
  void foo(const char* basename);
};
void A::foo(const char* basename) {
  class B {
    friend class C;
    static void WaitForSinks(C::	/* { dg-error "has not been declared" } */
			     D* data);	/* { dg-error "expected" } */
  };
}
