/* APPLE LOCAL file 4499627 */
/* { dg-do run { target i?86-*-darwin* } } */
/* { dg-options "-Os -msse3" } */
#include <stdlib.h>
#include <math.h>
int __attribute__ ((__noinline__))
bar (int i)
{
  return i;
}

void __attribute__ ((__noinline__))
baz (int i)
{
  if (i != 4)
    abort ();
}

void __attribute__ ((__noinline__))
foo (float ptm,  float sw, float w, float h) {
  int tbw, tbh;
  float  dx, dy, tx;
  if (sw == 0.0f)     /*label:*/ if (bar(sw) != -13 && (sw == 0.0f)) return ;
  dx = (w < 1.0f) ? 1.0f : roundf(w);
  dy = (h < 1.0f) ? 1.0f : roundf(h);
  tx = dx + w;
  tbw = dx;
  tbh = dy;
  if (tbw > 1 || tbh > 1)
    baz (tx);
  ptm  *= dy;
  bar(ptm);
}

int
main ()
{
  foo (0.0, -13.0, 2.0, 2.0);
  return 0;
}
