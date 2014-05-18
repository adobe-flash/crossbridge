#include "cmath.h"
#include <stdlib.h>

int ccfibonacci(int n)
{
	if (n<2) {
		return n;
	}
	return ccfibonacci(n-2)+ccfibonacci(n-1);
}