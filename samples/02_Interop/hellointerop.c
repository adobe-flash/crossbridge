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

#include <stdio.h>
#include <string.h>
#include "AS3/AS3.h"

int main(int argc, char **argv)
{
    // flascc uses GCC's inline asm syntax to allow you to write arbitrary
    // ActionScript inside your C/C++ code. This allows for very easy interop
    // between AS3 and C/C++.
    //
    // For a complete guide to the asm statement and how it is used read the
    // GCC documentation: http://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
    // or: http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
    //
    // Behind the scenes flascc turns each function/method in your C/C++ code
    // into an AS3 function. The only reason for mentioning this is that this
    // of course limits what AS3 you can put in an inline asm statement inside
    // a function. You can't write an AS3 class declaration inside an inline
    // asm statement inside a C/C++ function. You can however write a class
    // declaration outside any C/C++ function.
    
    // The following asm statment declares an AS3 variable and traces it out.
    // Because this asm statement doesn't mention any output parameters it must
    // be marked as volatile or GCC will optimise it away.
    //
    // Note: This will not be visible in the SWF console; trace() output goes to
    // the flashlog file. For more information on how to enable trace output
    // visit: http://kb2.adobe.com/cps/403/kb403009.html
    
    inline_as3(
        "var foo:int = 42;\n"
        "trace(\"This is inline AS3: \" + foo);\n"
        : : 
    );
    
    // asm statements can take input and output parameters like this. This
    // allows for very easy interop between the two languages.
    //
    // Not all C/C++ types have an equivalent in actionscript. They are mapped
    // as follows:
    //
    // C Type        | AS3 Type
    // 32bit int     | int
    // 64bit int     | special handling required!
    // 32bit float   | Number
    // 64bit double  | Number
    // pointer       | int
    
    // Use AS3 to negate a 32bit int!
    int someint = 123;
    int intresult = 0;
    inline_as3(
        "%0 = -%1;\n"
        : "=r"(intresult) : "r"(someint)
    );

    // Back in C we can take the result and print it out:
    printf("-%d is %d\n", someint, intresult);

    // Use AS3 to sqrt a double!
    double somenumber = 45.0;
    double result = 0.0;
    inline_as3(
        "%0 = Math.sqrt(%1);\n"
        : "=r"(result) : "r"(somenumber)
    );
    
    // Back in C we can take the result and print it out:
    printf("sqrt of %f is %f\n", somenumber, result);
    
    // Use AS3 to manipulate a 64bit int!
    unsigned long long somelonglong = 0x243F6A8885A308D3ULL; // 64 fractional pi bits
    double piapprox = 0.0;

    inline_as3(
        // cast to uint because %1 and %2 will be AS3 int despite "unsigned" C expression type
        "%0 = 3 + uint(%1) / 0x100000000 + uint(%2) / 0x10000000000000000\n"
        : "=r"(piapprox) : "r"((unsigned)(somelonglong >> 32)), "r"((unsigned)somelonglong));
    printf("pi approx is %.15f\n", piapprox);

    // Now, 64-bit out
    unsigned hi32, lo32;
    inline_as3(
        "%0 = uint(Math.sqrt(2) * 0x100000000); %1 = uint(Math.sqrt(2) * 0x10000000000000000)\n"
        : "=r"(hi32), "=r"(lo32));
    somelonglong = ((unsigned long long)hi32 << 32) | lo32;
    printf("52 fractional bits of sqrt(2): %llx\n", somelonglong); // only 52 bit mantissa in double!

    // Handling C strings involves a bit more work. Although they aren't
    // automatically converted to AS3 String objects when passed in/out of an
    // asm statement there are helper methods available.
    
    const char* words[] = {"flascc", "is", "awesome!"};
    int i;
    for(i=0; i<3; i++) {
        inline_as3(
            "trace(\"trace: \" + %0 +  \": \" + CModule.readString(%1, %2));\n"
            : : "r"(i), "r"(words[i]), "r"(strlen(words[i]))
        );
    }
    
    // What about passing strings from AS3 into C/C++? Because flascc uses a
    // ByteArray for storage we need to malloc() some space in that ByteArray
    // and copy the string data in.
    //
    // The mallocString helper function takes an AS3 string and allocates a
    // copy in the flascc heap. This can then be freed from C using the
    // normal free() function.
    // 
    // This example also shows that variables declared in one asm block are
    // visible to other asm blocks.
    
    inline_as3("var as3words = ['Interop', 'is', 'easy!'];\n");
    char* wordptrs[] = {NULL, NULL, NULL};
    for(i=0; i<3; i++) {
        inline_as3(
            "var stringptr:int = CModule.mallocString(as3words[%0]);\n"
            "CModule.write32(%1, stringptr);\n"
            : : "r"(i), "r"(&wordptrs[i])
        );
    }
    
    for(i=0; i<3; i++) {
        printf(">>> %s\n", wordptrs[i]);
        free(wordptrs[i]);
    }
}
