#include <stdio.h>
#include <string.h>
#include "AS3/AS3.h"

int main (int argc, const char * argv[])
{   
    double somenumber = 45.0;
    double result = 0.0;
    
    inline_as3(
               "%0 = ASFile.getNumber(%1);\n"
               : "=r"(result) : "r"(somenumber)
               );

    // show the result
    printf("number of %f is %f\n", somenumber, result);
    
    // call the same method without debugging information
    inline_as3(
               "%0 = ASFileNonDebug.getNumberNonDebug(%1);\n"
               : "=r"(result) : "r"(somenumber)
               );

    // show the result
    printf("number of %f is %f\n", somenumber, result);

}
