#include <stdlib.h>
#include "AS3/AS3.h"

int square3(int a){
	return a * a;
}

// First we mark the function declaration with a GCC attribute specifying the
// AS3 signature we want it to have in the generated SWC. The function will
// be located in the sample.MurmurHash namespace.
void multipleSWC3() __attribute__((used,
	annotate("as3sig:public function multipleSWC3(keystr:String):uint"),
	annotate("as3package:sample.MultipleSWC3")));

void multipleSWC3()
{
    // simple return
    int result = 15;
    int squared = square3(result);
    // return the result (using an AS3 return rather than a C/C++ return)
    AS3_Return(result);
}
