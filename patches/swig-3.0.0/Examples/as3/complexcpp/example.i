/* File : example.i */
%module example

%{
#include "example.h"    
#include <AS3/AS3++.h>
#include <Flash++.h>  
%}

%feature("autodoc");

%text %{
AS3 AUTODOC TEST
%} 

// STDINT

%include <stdint.i> 

%inline %{
extern int32_t a;
extern int_least16_t b;
extern int_fast8_t c;
extern uint8_t d;
extern intptr_t e;
%} 

// COPY CONSTRUCTOR

/* Ignore the Copy Constructor */
%ignore CRectangle(const CRectangle& c);

// AS3 IMPORT

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
 

// CONSTANTS

/* A few preprocessor macros */

#define    ICONST      42
#define    FCONST      2.1828
#define    CCONST      'x'
#define    CCONST2     '\n'
#define    SCONST      "Hello World"
#define    SCONST2     "\"Hello World\""

/* This should work just fine */
#define    EXPR        ICONST + 3*(FCONST)

/* This shouldn't do anything */
#define    EXTERN      extern

/* Neither should this (BAR isn't defined) */
#define    FOO         (ICONST + BAR)

/* The following directives also produce constants */

%constant int iconst = 37;
%constant double fconst = 3.14;
%constant int b = ICONST;

// SUPPRESSOVERRIDE

// Because no destructor is exposed in the base class
// we must suppress the use of the AS3 override keyword
// when wrapping the derived class's destructor
%as3suppressoverride derived::~derived; 

// MULTI TYPEMAP

%typemap(in) (int len, char *str) {
    AS3_MallocString($2, $input);
    AS3_StringLength($1, $input);
};

%typemap(astype) (int len, char *str) "String";

%typemap(freearg) (int len, char *str) {
    printf("cleaning up\n");
    free($2);
};

%inline %{
extern void print_str(int len, char *str);
%} 
    
// GRAB HEADER

%include "example.h"

// INTEROP

%{ 
 
////
//
// Get a property of an ActionScript object in C++
//
////
 
void getDelta() __attribute__((used,
    annotate("as3import:flash.events.MouseEvent"),
    annotate("as3sig:public function getDelta(m:MouseEvent):int"),
    annotate("as3package:utils")));
 
void getDelta()
{
    // marshall the AS3 parameter into a C++ variable
    AS3::local::var mouseEvent;
    AS3_GetVarxxFromVar(mouseEvent, m);
 
    // now that we have it in C++ let's get a property on that object
    AS3::local::var propertyName = AS3::local::internal::new_String("delta");
    AS3::local::var outputValue = AS3::local::internal::getproperty(mouseEvent, propertyName);
 
    // convert the var to a scalar
    int output = AS3::local::internal::int_valueOf(outputValue);
 
    // return that scalar value
    AS3_Return(output);
} 
 
////
//
// Set a property on an ActionScript object in C++
//
////
 
void setDelta() __attribute__((used,
    annotate("as3import:flash.events.MouseEvent"),
    annotate("as3sig:public function setDelta(m:MouseEvent):void"),
    annotate("as3package:utils")));
 
void setDelta()
{
    // marshall the AS3 parameter into a C++ variable
    AS3::local::var mouseEvent;
    AS3_GetVarxxFromVar(mouseEvent, m);
 
    // now that we have it in C++ let's set a property on that object
    AS3::local::var propertyName = AS3::local::internal::new_String("delta");
    AS3::local::var newValue = AS3::local::internal::new_int(5);
    AS3::local::internal::setproperty(mouseEvent, propertyName, newValue);
}
 
 
////
//
// Call a method of an ActionScript object in C++
//
////
 
void getString() __attribute__((used,
    annotate("as3import:flash.events.MouseEvent"),
    annotate("as3sig:public function getString(m:MouseEvent):String"),
    annotate("as3package:utils")));
 
void getString()
{
    // marshall the AS3 parameter into a C++ variable
    AS3::local::var mouseEvent;
    AS3_GetVarxxFromVar(mouseEvent, m);
 
    // demonstrate how to call a function of that object from C++
    AS3::local::var functionName = AS3::local::internal::new_String("toString");
    // first get the property of the object called "toString" which is a Function var
    AS3::local::var function = AS3::local::internal::getproperty(mouseEvent, functionName);
 
    // then call that Function object (in this case it takes no parameters)
    AS3::local::var output = AS3::local::internal::call(function, AS3::local::internal::_null, 0, NULL);
 
    // Since AS3_Return (as shown in getDelta()) can only accept a scalar value we need
    // to marshall this C++ object into an ActionScript String and return it using AS3_ReturnVar
    AS3_DeclareVar(asOutputString, String);
    AS3_CopyVarxxToVar(asOutputString, output);
    AS3_ReturnAS3Var(asOutputString);
}

%}

// EXTENDS

%{
    BaseClass::BaseClass() {}
%}


// with inline we both declare class and wrapper at once
%inline %{ 

class SpecialClass : public BaseClass {
public:
	SpecialClass() {}
	~SpecialClass() {}
    void DrawCircle(float32 radius) {}
	void DrawSegment() {}
};

%}

// declare class and wrapper separately
%{ 

class ExtraClass : public BaseClass {
public:
	ExtraClass() {}
	~ExtraClass() {}
    void DrawCircle(float32 radius) {}
	void DrawSegment() {}
};

%}

class ExtraClass : public BaseClass {
public:
	ExtraClass() {}
	~ExtraClass() {}
    void DrawCircle(float32 radius) {}
	void DrawSegment() {}
};
