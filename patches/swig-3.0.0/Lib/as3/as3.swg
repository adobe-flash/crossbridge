/*
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
** 
** You may copy, modify, distribute, and make derivative works based on
** this software, in source code or object code form, without
** restriction. If you distribute the software to others, you may do
** so according to the terms of your choice. This software is offered as
** is, without warranty of any kind.
**/

%include <as3kw.swg>

%{
#include <stdlib.h>
#include <string.h>
#include "AS3/AS3.h"
#define swig_as3(X) inline_as3("import com.adobe.flascc.swig.*; " X)
%}

%typemap(astype) char                   "String";
%typemap(astype) int                    "int";
%typemap(astype) float                  "Number";
%typemap(astype) double                 "Number";
%typemap(astype) char *                 "String";
%typemap(astype) unsigned char          "String";
%typemap(astype) bool                   "Boolean";
%typemap(astype) void                   "void";
%typemap(astype) SWIGTYPE ((*)(ANY))    "Function";
%typemap(astype) SWIGTYPE *             "int";
%typemap(astype) SWIGTYPE               "*";

%typemap(in) int {
    AS3_GetScalarFromVar($1, $input);
}

%typemap(in) SWIGTYPE * {
    AS3_GetScalarFromVar($1, $input);
}

// Note that this typemap yields a pointer
/*
%typemap(in) SWIGTYPE {
    AS3_GetScalarFromVar($1, $input.swigCPtr);
}
*/

%typemap(in) double {
    AS3_GetScalarFromVar($1, $input);
}

%typemap(in) float {
    AS3_GetScalarFromVar($1, $input);
}

%typemap(in) char * {
    AS3_MallocString($1, $input);
}

// TODO: assumes string has one character that fits in a char
// assert if violated
%typemap(in) char {
    inline_nonreentrant_as3(
        "%0 = $input.charCodeAt(0)" : "=r"($1)
    );
}

%typemap(in) bool {
    AS3_GetScalarFromVar($1, $input);
}

%typemap(in) SWIGTYPE ((*)(ANY)) {
    swig_as3(
        "var f:Function = $input;"
        "var ptr:int = SWIG_AS3GetCCallPtr(f);"
        "if (!ptr) {"
        "  var wrap = function() { $wrapper_name($input);};"
        "  SWIG_AS3RegCCallWrapper(f, wrap);"
        "  ptr = SWIG_AS3GetCCallPtr(f);"
        "}"
        "%0 = ptr;"
        : "=r"($1)
    );
}

%typemap(freearg) char * {
    free($1);
}

%typemap(out) void {}

%typemap(out) int {
    AS3_DeclareVar($result, int);
    AS3_CopyScalarToVar($result, $1);
}

%typemap(out) float {
    AS3_DeclareVar($result, Number);
    AS3_CopyScalarToVar($result, $1);
}

%typemap(out) double {
    AS3_DeclareVar($result, Number);
    AS3_CopyScalarToVar($result, $1);
}

%typemap(out) char * {
    int len = strlen($1);
    AS3_DeclareVar($result, String);
    AS3_CopyCStringToVar($result, $1, len);
}

%typemap(out) char {
    AS3_DeclareVar($result, String);
    AS3_CopyCStringToVar($result, &$1, 1);
}

%typemap(out) bool {
    AS3_DeclareVar($result, Boolean);
    AS3_CopyScalarToVar($result, $1);
}

%typemap(out) SWIGTYPE ((*)(ANY)) {
    swig_as3(
        "var ptr:int = %0;"
        "var $result:Function = SWIG_AS3GetASCallWrapper(ptr);"
        "if (!$result) {"
        "  var wrap:Function = function() {"
        "    var args:Array = arguments.concat(ptr);"
        "    return $wrapper_name.apply(null, args);"
        "  };"
        "  SWIG_AS3RegASCallWrapper(ptr, wrap);"
        "  $result = SWIG_AS3GetASCallWrapper(ptr);"
        "}" : : "r"($1)
    );
}

%typemap(out) SWIGTYPE * {
    AS3_DeclareVar($result, int);
    AS3_CopyScalarToVar($result, $1);
}

// Note that this typemap yields a pointer that must be freed later
%typemap(out) SWIGTYPE {
    #ifdef __cplusplus
    $type *retval = new $type($1);
    #else
    $type *retval = ($type *) malloc(sizeof($type));
    memcpy(retval, &$1, sizeof($type));
    #endif
    swig_as3(
        "var ptr:int = %0;" 
        "var out = new $astype();"
        "out.swigCPtr = ptr;"
        "var $result = out;"
        : : "r"(retval)
    );
}

// How values are returned to Actionscript
%typemap(asreturn) SWIGTYPE {
    AS3_ReturnAS3Var($input);
}

%typemap(asreturn) void {
    AS3_ReturnAS3Var(undefined);
}

%apply int {signed int, unsigned int, long, unsigned long, signed long, 
                enum SWIGTYPE, short, unsigned short, signed short};
%apply char {unsigned char, signed char};
%apply SWIGTYPE * {SWIGTYPE &};

%insert(runtime) %{
/* Contract support */

#define SWIG_contract_assert(expr, msg) if (!(expr)) {int msglen = strlen(msg);__asm__ volatile ("throw new Exception(CModule.readString(%0, %1))": : "r"(msg), "r"(msglen));}

%}

%insert(asruntime) %{
/* Function pointers support */
package com.adobe.flascc.swig {
import flash.utils.Dictionary;
import com.adobe.flascc.CModule;

[Weak]
var _SWIG_AS3Func2Ptr:Dictionary = new Dictionary();
[Weak]
var _SWIG_AS3Ptr2Func:Dictionary = new Dictionary();
[Weak]
var _SWIG_AS3FreeFuncPtrs:Array = new Array();

[Weak]
public function SWIG_AS3RegASCallWrapper(ptr:int, wrapper:Function):void {
    if (_SWIG_AS3Ptr2Func[ptr]) {
        throw("tried to register AS-callable wrapper for existing AS wrapper");
    }
    _SWIG_AS3Func2Ptr[wrapper] = ptr;
    _SWIG_AS3Ptr2Func[ptr] = wrapper;
}

[Weak]
public function SWIG_AS3UnregASCallWrapper(wrapper:Function):void {
    var ptr:int = _SWIG_AS3Func2Ptr[wrapper];
    delete _SWIG_AS3Func2Ptr[wrapper];
    delete _SWIG_AS3Ptr2Func[ptr];
}

// Returns Function objects callable from AS
[Weak]
public function SWIG_AS3GetASCallWrapper(ptr:int):Function {
    if (_SWIG_AS3Ptr2Func[ptr]) {
        return _SWIG_AS3Ptr2Func[ptr];
    } else {
        return null;
    }
}

// Returns the number of wrapped C and AS functions
[Weak]
public function SWIG_AS3WrapperNum():uint {
    var num:uint = 0;
    for (var key:int in _SWIG_AS3Ptr2Func) {
        num++;
    }
    return num;
}

[Weak]
public function SWIG_AS3RegCCallWrapper(func:Function, wrapper:Function):void {
    var ptr:int;
    
    if (_SWIG_AS3Func2Ptr[func]) {
        throw("tried to register C-callable wrapper for native C function");
    }

    if (_SWIG_AS3FreeFuncPtrs.length) {
        ptr = _SWIG_AS3FreeFuncPtrs.pop();
    } else {
        ptr = CModule.allocFunPtrs(null, 1, 4);
    }
    CModule.regFun(ptr, wrapper);
    _SWIG_AS3Func2Ptr[func] = ptr;
    _SWIG_AS3Ptr2Func[ptr] = func;

}

[Weak]
public function SWIG_AS3UnregCCallWrapper(func:Function):void {
    var ptr:int = _SWIG_AS3Func2Ptr[func];
    delete _SWIG_AS3Func2Ptr[func];
    delete _SWIG_AS3Ptr2Func[ptr];
    _SWIG_AS3FreeFuncPtrs.push(ptr);
}

// Returns a C function pointer corresponding to an Actionscript-callable 
// wrapper Function. (The C function is callable from C, of course).
[Weak]
public function SWIG_AS3GetCCallPtr(func:Function):int {
    if (_SWIG_AS3Func2Ptr[func]) {
        return _SWIG_AS3Func2Ptr[func];
    } else {
        return 0;
    }
}
} /* package */

%}

#define %as3suppressoverride %feature("as3:suppressoverride", "1")

#define %as3import(IMPORTS) %feature("as3:import", IMPORTS)
