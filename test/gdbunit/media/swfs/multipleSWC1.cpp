#include <stdlib.h>
#include "AS3/AS3.h"

int square1(int a){
	return a * a;
}

// First we mark the function declaration with a GCC attribute specifying the
// AS3 signature we want it to have in the generated SWC. The function will
// be located in the sample.MurmurHash namespace.
void multipleSWC1() __attribute__((used,
	annotate("as3sig:public function multipleSWC1(keystr:String):uint"),
	annotate("as3package:sample.MultipleSWC1")));

void multipleSWC1()
{
    // simple return
    int result = 5;
    int squared = square1(result);
    // return the result (using an AS3 return rather than a C/C++ return)
    AS3_Return(result);
}
