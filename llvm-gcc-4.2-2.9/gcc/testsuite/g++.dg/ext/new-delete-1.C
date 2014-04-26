/* APPLE LOCAL file coalescing */
/* Make sure that non-weak '::new' and '::delete' operators do not wind
   up in a coalesced section.  */
/* Author: Ziemowit Laski <zlaski@apple.com>  */
/* { dg-do compile { target powerpc*-*-darwin* } } */

extern "C" void free(void *);

void operator delete(void*) throw();
void operator delete(void* p) throw() { free(p); }

void operator delete(void*, int) throw();

void *operator new(unsigned long) throw();
void *operator new(unsigned long) throw() { return (void *)0; }

int *foo(void) {
  int *n = new int();
  ::operator delete(n, 0);
  ::operator delete(n);
  return 0;
}

/* { dg-final { scan-assembler-not "coal" } } */
