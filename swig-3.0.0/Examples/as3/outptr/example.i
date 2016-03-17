/*
 * Sometimes C libraries have methods that don't return values, but instead 
 * stuff the value into one of the parameters.
 *
 * This example demonstrates how you can use SWIG to make this interface much
 * simpler to your ActionScript consumers by making void methods actually 
 * return the output as an ActionScript object.
 *
 * See the Typemaps example in the SWIG tutorial for more in depth explanation
 * of some of these typemaps.
 *
 */

#ifdef SWIG
%module ExampleModule

%{
#include "example.h"
%}

// change functions that follow the in/out pointer style to accept an ActionScript Vector
%typemap(astype) (int* in, int* out, int length) "Vector.<int>";

// change functions that return void to return an ActionScript Vector of ints
%typemap(astype) (void) "Vector.<int>";

%typemap(in) (int* in, int* out, int length)(int* generatedLocalOut, int generatedLocalLength) {
	// setup some new C variables that we're going to modify from within our inline ActionScript
	int* newBuffer;
	int newBufferSize;
	
	inline_as3("var ptr:int = CModule.malloc($input.length);\n");
	inline_as3("%0 = $input.length;\n": "=r"(newBufferSize));
	inline_as3("%0 = ptr;\n": "=r"(newBuffer));
    
	// push the values of the input Vector into C memory
	int i;
	for (i = 0; i < newBufferSize; i++){
		int number;
		inline_as3("%0 = $input[%1];\n": "=r"(number) : "r"(i));
		*(newBuffer + i) = number;
	}
	
	// assign the parameters that the C function is expecting to our new values
	$1 = newBuffer;
	$2 = malloc(4 * newBufferSize);
	$3 = newBufferSize;
    
	// save some local values for use in %typemap(argout)
	generatedLocalLength = newBufferSize;
	generatedLocalOut = $2;
}

%typemap(argout) (int* in, int* out, int length) {
    
    // get the output from the C world that we saved in %typemap(in)
    int* resultPtr = generatedLocalOut$argnum;
    int resultLength = generatedLocalLength$argnum;
    
    // create the ActionScript variable that will get returned
    inline_as3("var $result:Vector.<int> = new Vector.<int>();\n");
    
    // write the C output into the ActionScript $result variable
    int i;
    for (i = 0; i < resultLength; i++){ 
        int number = *(resultPtr + i);
        inline_as3("$result.push(%0);\n" : : "r"(number));
    }
}

%typemap(freearg) (int* in, int* out, int length) {
	free($2);
    inline_as3("CModule.free(ptr)");
};

%typemap(asreturn) void {
	// make the void function actually return something
	AS3_ReturnAS3Var(asresult);
}

%include "example.h"

#else
#include "example.h"
#endif
