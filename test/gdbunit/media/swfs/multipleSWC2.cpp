#include <stdlib.h>
#include "AS3/AS3.h"

int square2(int a){
	return a * a;
}

// First we mark the function declaration with a GCC attribute specifying the
// AS3 signature we want it to have in the generated SWC. The function will
// be located in the sample.MurmurHash namespace.
void multipleSWC2() __attribute__((used,
	annotate("as3sig:public function multipleSWC2(keystr:String):uint"),
	annotate("as3package:sample.MultipleSWC2")));

void multipleSWC2()
{
    // simple return
    int result = 10;
    int squared = square2(result);
    // return the result (using an AS3 return rather than a C/C++ return)
    AS3_Return(result);
}
