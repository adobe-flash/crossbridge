/* APPLE LOCAL file KEXT indirect-virtual-calls */
/* Radar 3972840: Positive C++ test case */
/* Origin: Matt Austern  <austern@apple.com> */
/* { dg-do compile } */
/* { dg-options "-fapple-kext" } */

struct Base { virtual char* abc(void) const; };

char* Base::abc() const { return 0; }

void f(Base* p) {
  char* c = p->Base::abc();
}
