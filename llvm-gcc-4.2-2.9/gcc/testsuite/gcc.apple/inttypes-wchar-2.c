/* APPLE LOCAL file test that the definition of wchar_t in
   inttypes.h gives sizeof(wchar_t) == 2 when using -fshort-wchar. */

/* { dg-do run } */
/* { dg-options "-Wall -W -fshort-wchar" } */

#include <inttypes.h>
#include <stdint.h>

/* Verify that WCHAR_MAX is the largest unsigned 16-bit integer. */
#if WCHAR_MAX != 0xffffU
#  error WCHAR_MAX has incorrect value
#endif

/* Verify that WCHAR_MIN is 0 */
#if WCHAR_MIN != 0
#  error WCHAR_MIN has incorrect value
#endif

int dummy1[3 - sizeof(wchar_t)] = { 0 };
int dummy2[sizeof(wchar_t) - 1] = { 0 };

int main ()
{
  return (sizeof(wchar_t) == 2) ? 0 : 1;
}
