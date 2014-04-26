/* APPLE LOCAL file stdint.h */

/* { dg-do run } */
/* { dg-options "-Wall -W" } */

/* Check SIG_ATOMIC_{MIN,MAX}, WINT_{MIN,MAX}.  Also check that
   stdint.h and sys/types.h can be included simultaneously.  */

#include <stdint.h>
#include <sys/types.h>
#include <signal.h>
#include <wchar.h>
#include <stdlib.h>

int main(void)
{
  sig_atomic_t s_a;
  wint_t wi;
  
  s_a = SIG_ATOMIC_MIN;
  if (s_a != SIG_ATOMIC_MIN)
    abort ();
  if (s_a > 0)
    abort ();
  s_a--;
  if (s_a != SIG_ATOMIC_MAX)
    abort ();
  if (s_a <= 0)
    abort ();

  wi = WINT_MIN;
  if (wi != WINT_MIN)
    abort ();
  if (wi > 0)
    abort ();
  wi--;
  if (wi != WINT_MAX)
    abort ();
  if (wi <= 0)
    abort ();

  exit (0);
}
