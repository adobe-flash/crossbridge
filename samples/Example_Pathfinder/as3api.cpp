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
#include "pathfinder.h"
#include "AS3/AS3.h"

// First we mark the function declaration with a GCC attribute specifying the
// AS3 signature

////** flascc_createPather **////
void flascc_createPather() __attribute__((used,
	annotate("as3sig:public function flascc_createPather():int"),
	annotate("as3package:com.renaun.flascc_interface")));

void flascc_createPather()
{
    // Create an instance of the Class
    Pather *result;
    result = (Pather *)new Pather();
    
    // Create int for the pointer into memory for this object instance
    AS3_DeclareVar(asresult, int);
    AS3_CopyScalarToVar(asresult, result);
    
    // return the pointer value of this object in memory
    AS3_ReturnAS3Var(asresult);
}

////** flascc_deletePather **////
void flascc_deletePather() __attribute__((used,
      annotate("as3sig:public function flascc_deletePather(self):void"),
      annotate("as3package:com.renaun.flascc_interface")));

void flascc_deletePather()
{
    Pather *arg1 = (Pather *) 0 ;
    AS3_GetScalarFromVar(arg1, self);
    delete arg1;
    AS3_ReturnAS3Var(undefined);
}


////** flascc_setMap **////
void flascc_setMap() __attribute__((used,
     annotate("as3sig:public function flascc_setMap(self, buffer:int, colSize:int, rowSize:int):void"),
     annotate("as3package:com.renaun.flascc_interface")));

void flascc_setMap()
{
    Pather *arg1 = (Pather *) 0 ;
    unsigned char *arg2 = (unsigned char *) 0 ;
    int arg3;
    int arg4;
    // convert arguments
    AS3_GetScalarFromVar(arg1, self);
    AS3_GetScalarFromVar(arg2, buffer);
    AS3_GetScalarFromVar(arg3, colSize);
    AS3_GetScalarFromVar(arg4, rowSize);
    // apply to object
    (arg1)->setMap((unsigned char *)arg2, arg3, arg4);
    // return void
    AS3_ReturnAS3Var(undefined);
}



////** flascc_getPath **////
void flascc_getPath() __attribute__((used,
      annotate("as3sig:public function flascc_getPath(self, sx:int, sy:int, nx:int, ny:int):int"),
      annotate("as3package:com.renaun.flascc_interface")));

void flascc_getPath()
{
    Pather *arg1 = (Pather *) 0 ;
    int arg2;
    int arg3;
    int arg4;
    int arg5;
    int* result;
    // convert arguments
    AS3_GetScalarFromVar(arg1, self);
    AS3_GetScalarFromVar(arg2, sx);
    AS3_GetScalarFromVar(arg3, sy);
    AS3_GetScalarFromVar(arg4, nx);
    AS3_GetScalarFromVar(arg5, ny);
    // apply to object
    result = (arg1)->getPath(arg2, arg3, arg4, arg5);
    // return Number
    AS3_DeclareVar(asresult, int);
    AS3_CopyScalarToVar(asresult, result);
    AS3_ReturnAS3Var(asresult);
}

