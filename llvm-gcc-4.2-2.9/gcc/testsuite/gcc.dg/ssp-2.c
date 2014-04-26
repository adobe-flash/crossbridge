/* APPLE LOCAL file mainline */
/* { dg-do run { target native } } */
/* { dg-options "-fstack-protector" } */
#include <stdlib.h>

void
__stack_chk_fail (void)
{
  exit (0); /* pass */
}

void
overflow()
{
  static int i = 0;	/* Can't allocate this on the stack.  */
  char foo[30];

  /* Overflow buffer.  */
  for (i = 0; i < 50; i++)
      foo[i] = 42;
}

int main (void)
{
  overflow ();
  return 1; /* fail */
}
