/* APPLE LOCAL file KEXT double destructor --bowdidge */
/* Radar 3943783  kernel extensions built with gcc-4.0 can't be loaded */
/* { dg-do compile { target powerpc*-apple-darwin* } } */
/* { dg-options "-S -fapple-kext -fno-exceptions" } */

/* Here's some kext class hierarchy code. */


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

/* Here's some inlined functions to try to trick the compiler into creating coalesced
   sections. */
inline unsigned f(unsigned n) { return n == 0 ? 0 : n + f(n-1); }

unsigned g(unsigned n) { return f(n); }

/* Here's some template stuff to try to trick the compiler into creating coalesced sections
   another way. */

  template <typename X> X ident(X x) { return x; }
  int foo(int n) { return ident(n); }

/*  See if there's any sections with the coalesced flag.  coalesced sections
    currently aren't loaded by the kernel loader into memory. */
/* { dg-final { scan-assembler-not "coalesced" } } */
