/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated
** All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
** terms of the Adobe license agreement accompanying it.  If you have received this file from a
** source other than Adobe, then your use, modification, or distribution of it requires the prior
** written permission of Adobe.
*/
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
    // 64bit int     | Number
    // 32bit float   | Number
    // 64bit double  | Number
    // pointer       | int
    
    double somenumber = 45.0;
    double result = 0.0;
    inline_as3(
        "%0 = Math.sqrt(%1);\n"
        : "=r"(result) : "r"(somenumber)
    );
    
    // Back in C we can take the result and print it out:
    printf("sqrt of %f is %f\n", somenumber, result);
    
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
