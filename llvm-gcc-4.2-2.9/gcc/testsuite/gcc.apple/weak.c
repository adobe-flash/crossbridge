/* APPLE LOCAL file weak variables 6822086 */
/* { dg-do run { target "i?86-*-darwin*" } } */
/* { dg-options "-O2 -m32" } */
extern int i __attribute__((weak));

double sin(double);

int j;

void foo(int j);
double ed;

main() {
  int l;
  double d;
  for (l=0; l < 100; ++l) {
    if (&i)
      j = i;
    else
      j = 0;
    d += sin(j);
  }
  ed = d;
  return 0;
}

/* Hide: void foo(int j) { } from the optimizer. */
asm(".globl _foo");
asm("_foo: ret");
asm(".globl _i");
asm(".set _i, 0");
