/* APPLE LOCAL file test of wchar_t's definition in inttypes.h */

/* { dg-do run } */
/* { dg-options "-Wall -W" } */

#include <inttypes.h>
#include <stdint.h>

/* Verify that WCHAR_MAX is the largest positive 32-bit integer */
#if WCHAR_MAX != 0x7fffffff
#  error WCHAR_MAX has incorrect value
#endif

/* Verify that WCHAR_MIN is negative and consistent with WCHAR_MAX. */
#if WCHAR_MIN != (-WCHAR_MAX-1)
#  error WCHAR_MIN has incorrect value
#endif

int dummy1[5 - sizeof(wchar_t)] = { 0 };
int dummy2[sizeof(wchar_t) - 3] = { 0 };

int main ()
{
  /* Verify that wchar_t is a 32-bit type. */
  return (sizeof(wchar_t) == 4) ? 0 : 1;
}
