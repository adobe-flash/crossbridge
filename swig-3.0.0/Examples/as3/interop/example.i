/* File : example.i */
%module example

%{
    #include "example.h"
    #include <AS3/AS3++.h>
    #include <Flash++.h>
%}

%include "example.h"

%{
    BaseClass::BaseClass() {}
%}

// with inline we both declare class and wrapper at once
%inline %{ 

class SpecialClass : public BaseClass {
public:
	SpecialClass() {
        AS3_Trace("SpecialClass::Created");
        AS3::local::var s = AS3::local::internal::new_String("SpecialClass loaded.");
        AS3::local::internal::trace(s);
    }
	~SpecialClass() {AS3_Trace("SpecialClass::Destroyed");}
    
};

%}

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