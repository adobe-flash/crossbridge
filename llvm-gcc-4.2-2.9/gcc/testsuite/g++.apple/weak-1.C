/* APPLE LOCAL file weak types 5954418 */
/* { dg-do compile } */
/* LLVM LOCAL begin - Whitespace */
/* { dg-final { scan-assembler "weak_definition\[ \t]+__ZTI1B" } } */
/* { dg-final { scan-assembler "weak_definition\[ \t]+__ZTI1A" } } */
/* LLVM LOCAL end - Whitespace */
/* Radar 5954418 */

#define WEAK __attribute__ ((weak)) 

class WEAK A {
  virtual void foo();
};

class B : public A {
  virtual void foo();
};

void A::foo() { }

void B::foo() { }
