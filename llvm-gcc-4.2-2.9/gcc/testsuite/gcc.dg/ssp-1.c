/* APPLE LOCAL file mainline */
/* { dg-do run { target native } } */
/* { dg-options "-fstack-protector" } */
#include <stdlib.h>

static void
__stack_chk_fail (void)
{
  exit (0); /* pass */
}

int main ()
{
  static int i;		/* Can't allocate this on the stack.  */
  char foo[255];

  // smash stack
  for (i = 0; i <= 400; i++)
    foo[i] = 42;

  return 1; /* fail */
}
