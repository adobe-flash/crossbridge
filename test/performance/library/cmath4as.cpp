#include <stdlib.h>
#include "AS3/AS3.h"
#include "cmath.h"

void fibonacci() __attribute__((used,
	annotate("as3sig:public function fibonacci(keynum:int):int"),
	annotate("as3package:ccsample.cmath")));

void fibonacci()
{
	int num,sum;
	AS3_GetScalarFromVar(num, keynum);

	sum = ccfibonacci(num);

	AS3_Return(sum);
}

