/* APPLE LOCAL file elide global inits 3814991 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-use-cxa-atexit" } */
/* { dg-final { scan-assembler-not "_GLOBAL__I_" } } */
/* Radar 3814991 */

/* virtual inheritance or virtual functions require a ctor for now.  */
int j;
struct B {
  ~B() { }
  B() {
    int i;
    i = 1;
    if (0) ++j;
    if (1) i = 3;
  }
};
struct Foo : public B {
  Foo() { }
};
Foo foo;
