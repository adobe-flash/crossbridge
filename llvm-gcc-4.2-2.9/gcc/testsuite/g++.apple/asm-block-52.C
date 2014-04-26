/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -O } } */
/* Radar 4527214 */

inline int foo(int x, int y) {
  int result;
  __asm {
    MOV EAX, x
    IMUL y
    MOV result, EDX
  }
  return result;
}

inline int bar(int mhigh) {
  int temp = foo(mhigh, 3);
  return temp;
}

typedef struct S {
  long a;
  int c;
} S;

void bee(S *fill) {
  int a, top = 2;
  int b = 1;
  int i = 0;
  int c = fill->c;
  int bottom = 30;

  if (b < top)
    i = (fill->a - b) / c;

  while (i < bottom - top) {
    bar(a);
    bar(a);
    i++;
  }
}
