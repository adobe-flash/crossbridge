/* APPLE LOCAL file Wglobal-constructors 6324584 */
/* { dg-options "-Wglobal-constructors" } */
/* Radar 6324584 */

volatile int i;

class A {
public:
  A() { i = 1; }
} a;		/* { dg-warning "requires global construction" } */

class B {
public:
  ~B() { i = 1; }
} b;		/* { dg-warning "requires global destruction" } */

void ctor() __attribute__((constructor));
void ctor() {	/* { dg-warning "requires global construction" } */
}
void dtor() __attribute__((destructor));
void dtor() {	/* { dg-warning "requires global destruction" } */
}
