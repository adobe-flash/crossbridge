/* APPLE LOCAL file 3399553 */
/* Verify that FLT_ROUNDS changes with a successful call to
   fesetround().  */

/* { dg-do run } */

#include <fenv.h>
#include <float.h>

int main (void)
{
  if (fesetround (FE_TONEAREST) >= 0
      && FLT_ROUNDS != 1)
    return 1;

  if (fesetround (FE_TOWARDZERO) >= 0
      && FLT_ROUNDS != 0)
    return 1;

  if (fesetround (FE_UPWARD) >= 0
      && FLT_ROUNDS != 2)
    return 1;

  if (fesetround (FE_DOWNWARD) >= 0
      && FLT_ROUNDS != 3)
    return 1;

  return 0;
}

