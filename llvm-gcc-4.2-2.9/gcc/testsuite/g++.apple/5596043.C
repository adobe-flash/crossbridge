/* APPLE LOCAL file radar 5591491 */
/* { dg-options "-O2" } */
/* { dg-do compile { target powerpc**-*-darwin* } } */

struct T {
  volatile unsigned int word;
  inline void foo ()
  {
    __asm__ __volatile__(
                        "isync\n\t"
                        "eieio\n\t"
                        "stw %1, %0"
                        :"=m" (word)
                        :"r" (0)
                        :"memory"
                        );
  }
};

struct Span {
  unsigned int size:8;
};

static T data[10];

void bar ()
{
  Span* span;
  const long unsigned int cl = span->size;
  data[cl].foo();
}

int main ()
{
  bar ();
  return 0;
}
