/* File : example.i */
#ifdef SWIG
%module ExampleModule

// When the generated wrapper for a function needs custom imports
// You can add them via the as3import directive. Multiple namespaces
// should be comma delimited.
%as3import("flash.utils.ByteArray") printNumber;

%as3import("foo.package1,foo.package2") printNumber2;


%typemap(in) (int i) {	
	int newBufferSize = 0;
    
	// Simple test to make sure that we can access a static property of ByteArray
	inline_as3("%0 = (ByteArray.defaultObjectEncoding) + 1;\n": "=r"(newBufferSize));
	
	$1 = newBufferSize;
}

%{
#include "example.h"
%}

%include "example.h"
#else
#include "example.h"
#endif


