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

/* 
 * 
 * Sample: Using SWIG typemaps to expose a simpler ActionScript API
 * 
 *
 * Let's use the SWIG %typemap directive to make it easier for people using our library
 * within an ActionScript project.  Instead of asking our user to pass in pointers, we're
 * going to allow the customer to provide ActionScript objects.
 *
 * This example will use a few different typemaps together:
 * 
 * %typemap(astype) - maps C types to ActionScript types
 * %typemap(in) - does the conversion from ActionScript objects to C objects
 * %typemap(freearg) - cleans up any memory that we malloc'd during typemap(in)
 * %typemap(out) - does the conversion from C objects to ActionScript objects
 *
 * See the SWIG documentation for more information on using typemaps:
 * http://www.swig.org/Doc1.3/Typemaps.html
 */
#ifdef SWIG
%module IntArrayLibModule

%{
#include "intarraylib.h"
%}

// C functions that have the int* buffer, int bufferSize parameters will be exposed as 
// a Vector.<int> in ActionScript
%typemap(astype) (int* buffer, int bufferSize) "Vector.<int>";

// C functions that return the IntArray type will expose that in ActionScript as a Vector.<int>
%typemap(astype) (IntArray) "Vector.<int>";

%typemap(in) (int* buffer, int bufferSize) {
	//
	// Inside this typemap block you have a few variables that SWIG supplies:
	//
	// $1 - the first parameter that the C function is expecting
	// $2 - the second parameter that the C function is expecting
	//
	// $input - the actual input from ActionScript.  It's an ActionScript object
	//		    so it's only useful within an inline_as3() block.  This is the object
	//   		we need to bring into the C world by populating values for $1 and $2
	//
	
	// setup some new C variables that we're going to modify from within our inline ActionScript
	int* newBuffer;
	int newBufferSize;
	
	// Use the inline_as3() function that is defined in AS3.h to write the ActionScript code 
	// that will convert the Vector into something C can use.  Notice that we are using $input
	// inside this inline_as3() call.
	inline_as3("var ptr:int = CModule.malloc($input.length);\n");
	
	// This next inline call is a little more complicated.  Here we use the %0 flag to pass
	// the value of the ActionScript $input.length variable to the C variable named newBufferSize.
	inline_as3("%0 = $input.length;\n": "=r"(newBufferSize));
	
	// Similarly we'll pass the value of the ptr variable in ActionScript to the C newBuffer variable
	inline_as3("%0 = ptr;\n": "=r"(newBuffer));
	
	// Now push that Vector into flascc memory
	inline_as3("for (var i:int = 0; i < $input.length; i++){\n");
	inline_as3("	CModule.write32(ptr + 4*i, $input[i]);\n");
	inline_as3("}\n");
	
	// Finally assign the parameters that C is expecting to our new values
	$1 = newBuffer;
	$2 = newBufferSize;
	
	// FYI - Here is a simple trace statement in ActionScript that can be useful for debugging.
	// This is an example of passing something from C to ActionScript (as opposed to going from
	// ActionScript to C as demonstrated above).  Note the "r" instead of "=r" and the extra colon.
	//
	// See http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html to learn more 
	// about how to work with inline assembly calls
	inline_as3("trace('Vector length: ' + %0)": : "r"(newBufferSize));
}

// Free the memory that we CModule.malloc'd in the equivalent typemap(in)
%typemap(freearg) (int* buffer, int bufferSize) {
    inline_as3("CModule.free(ptr)");
};

// Convert C IntArray objects into ActionScript Vector.<int> objects
%typemap(out) (IntArray) {

   //
   // Inside this typemap block you have a few variables that SWIG supplies:
   // 
   // result - the output from the C world (an IntArray)
   // $result - the name of the ActionScript object that will be returned
   //

   // create the ActionScript variable that will get returned
   inline_as3("var $result:Vector.<int> = new Vector.<int>();\n");
   
   // get the output from the C world
   int* resultArray = result.array;
   int resultArraySize = result.length;
   
   // write that output into the ActionScript $result variable
   int i;
   for (i = 0; i < resultArraySize; i++){ 
   	   int number = *(resultArray + i);
	   inline_as3("$result.push(%0);\n" : : "r"(number));
       
       // Note: We could also have written these last two lines completely within inline_as3, for example:
       //
       // inline_as3("$result.push(CModule.read32(%0 + 4*%1));\n" : : "r"(resultArray), "r"(i));
       // 
       // But in general you should avoid calling CModule.read32() etc. from C when you can
       // just do it in C directly.  This will result in better performance.
   }
}

// Tell SWIG to generate wrappers for all of the functions in the library
%include "intarraylib.h"

#else
#include "intarraylib.h"
#endif
