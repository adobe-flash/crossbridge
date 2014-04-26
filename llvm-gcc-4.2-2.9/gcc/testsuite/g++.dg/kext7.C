/* APPLE LOCAL file KEXT double destructor --bowdidge */
/* Radar 3943783  kernel extensions built with gcc-4.0 can't be loaded */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-S -fapple-kext" } */
/* { dg-require-effective-target ilp32 } */

struct Base {
  virtual ~Base();
} __attribute__((apple_kext_compatibility));

struct Derived : Base {
  void operator delete(void *) { }
  Derived();
};

void foo() {
  Derived d1;			// ok
}
/* An implicit in-charge destructor shouldn't be created when -fapple-kext
   is declared. */
/* { dg-final { scan-assembler-not "_ZN7DerivedD1Ev" } } */
