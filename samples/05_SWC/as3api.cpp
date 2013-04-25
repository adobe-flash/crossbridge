// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "MurmurHash3.h"
#include <stdlib.h>
#include "AS3/AS3.h"

// First we mark the function declaration with a GCC attribute specifying the
// AS3 signature we want it to have in the generated SWC. The function will
// be located in the sample.MurmurHash namespace.
void MurmurHash3() __attribute__((used,
	annotate("as3sig:public function MurmurHash3(keystr:String):uint"),
	annotate("as3package:sample.MurmurHash")));

void MurmurHash3()
{
    // Copy the AS3 string to the C heap (must be free'd later)
    char *key = NULL;
    AS3_MallocString(key, keystr);

    int keylen = 0;
    AS3_StringLength(keylen, keystr);
    
    // Call hash function
    int result;
    uint32_t seed = 42;
    MurmurHash3_x86_32(key, keylen, seed, &result);
    
    // don't forget to free the string we allocated with malloc previously
    free(key);
    
    // return the result (using an AS3 return rather than a C/C++ return)
    AS3_Return(result);
}
