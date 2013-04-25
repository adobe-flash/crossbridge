/* { dg-do run } */
/* { dg-options "-Wno-long-long" } */

#include <stdlib.h>

long long foo()
{
  static long long rat = (1LL<<62);
  return rat;
}

int main()
{

	if (foo() != (1LL<<62))
	  abort();
	exit(0);
}
	
