/* APPLE LOCAL file radar 4900615 - long double encoding */
/* { dg-do run } */

#include <stdio.h>

extern void abort();

int main()
{
  if (strcmp (@encode(typeof(long double)), "D"))
   abort();
  return 0;
}
