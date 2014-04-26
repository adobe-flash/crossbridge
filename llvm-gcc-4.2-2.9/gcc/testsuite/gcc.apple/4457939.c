/* APPLE LOCAL file 4457939 stack alignment mishandled */
/* { dg-do run } */
/* { dg-options "-O2" } */
#include <inttypes.h>
#include <stdlib.h>

#define ALIGN_PTR(p,a) ((void *)( (((size_t)(p))+(a)-1)&~((size_t)(a)-1)))

void __attribute__ ((__noinline__))
foo (void *v)
{
  if (((size_t)v) & 15)
    abort ();
}

int
main (void)
{
  float x[13];
  foo (ALIGN_PTR (x, 16));
  return 0;
}
