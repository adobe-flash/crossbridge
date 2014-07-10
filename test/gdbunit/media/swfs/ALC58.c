#include "AS3/AS3.h"
#include <stdio.h> 

int someotherfunction(int arg2, int arg3, int arg4, char const *arg5) 
{ 
    printf("someotherfunction: %d, %d, %d, %d\n", arg2, arg3, arg4, arg5); 
    return arg2 + arg3 + arg4; 
} 

__attribute__((annotate("as3sig:public function brokenfunction(user_data:int, chunk:int, size:int, filename:String):int"))) 
void brokenfunction() { 
    
    int arg2 = 5;
    int arg3 = 5;
    int arg4 = 5;
    char *arg5 = (char*)0;
    
    someotherfunction(arg2,arg3,arg4,(char const *)arg5); 
} 

int main() 
{ 
    inline_as3("var result = brokenfunction(2, 3, 4, \"\");\n"); 
    printf("After A");
    printf("After B");    
}
