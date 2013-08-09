/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in
** accordance with the terms of the Adobe license agreement accompanying it.
** If you have received this file from a source other than Adobe, then your use,
** modification, or distribution of it requires the prior written permission of Adobe.
*/

#ifndef AS3_H
#define AS3_H

#include <AS3/AVM2.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* The package_as3 macro allows you to inject arbitrary AS3 into the module namespace
* associated with the current C/C++ translation unit. This is useful for declaring global
* AS3 variables which you want to reference from inline as3 asm statements within C/C++ functions.
*/
#define package_as3(...) __asm__ ("#package private\n" __VA_ARGS__)

/**
* The inline_as3 macro correctly restores the global stack pointer and lets you inject arbitrary
* AS3 code wihin the body of a C/C++ function. Refer to the flascc documentation for examples
* of how you can use GCC's inline asm syntax for interoperating between C/C++ and AS3.
*/ 
#define inline_as3(...) __asm__ volatile("ESP = esp & ~15;\n" __VA_ARGS__)

/**
* If your AS3 code doesn't re-enter FlasCC code you can use this
* But if it does (either directly or indirectly via something like
* CModule.mallocString) then your stack will be smashed.
* Use inline_as3 unless you are really sure this is what you want!
*/
#define inline_nonreentrant_as3(...) __asm__ volatile(__VA_ARGS__)

/**
* When breaking a C/C++ run-loop so that FlasCC code is suitable for use
* without using multi-threading you need to execute main, so the static initializers for your code are run, but
* you want to prevent the static destructors from running so it is important that main does not return.
*
* By throwing an AS3 exception and preserving the stack we can effectively interrupt the execution of main.
*/
#define AS3_GoAsync() do { avm2_self_unlock(); inline_nonreentrant_as3("import com.adobe.flascc.GoingAsync;\nthrow new GoingAsync;\n"); } while(0)
    
/* Returns a value via an AS3 return. Must be a scalar value. */
#define AS3_Return(CVAR) do { __asm__ volatile("_as3ReturnValue = %0;\n" : : "r"(CVAR)); return; } while(0)
    
/* Returns an AS3 variable via an AS3 return. */
#define AS3_ReturnAS3Var(AS3VAR) do { __asm volatile("_as3ReturnValue = " #AS3VAR ";\n"); return; } while(0)

/**
* Simple wrapper to trace strings to the flash log file which can be found in one of these locations:
*
* Windows 95/98/ME/2000/XP  C:\Documents and Settings\username\Application Data\Macromedia\Flash Player\Logs
* Windows Vista             C:\Users\username\AppData\Roaming\Macromedia\Flash Player\Logs
* Macintosh OS X            /Users/username/Library/Preferences/Macromedia/Flash Player/Logs/
* Linux                     /home/username/.macromedia/Flash_Player/Logs/
*/
#define AS3_Trace(STR) inline_nonreentrant_as3("trace(" #STR ");\n")

/* Marshalls an AS3 string into a C char*. This will call malloc behind the scenes so you must free it later. */
#define AS3_MallocString(CVAR, STR) inline_as3("%0 = CModule.mallocString(" #STR ");\n": "=r"(CVAR))

/* Sets the value of CVAR to the length of the ActionScript string STR. */
#define AS3_StringLength(CVAR, STR) inline_nonreentrant_as3("%0 = " #STR ".length;\n": "=r"(CVAR))

/* Declares an AS3 variable in the current function. */
#define AS3_DeclareVar(AS3NAME, AS3TYPE) inline_nonreentrant_as3("var " #AS3NAME ":" #AS3TYPE ";\n")

/* Marshalls a C string  character into a local AS3 string variable. */
#define AS3_CopyCStringToVar(AS3VAR, CSTRPTR, LEN) inline_nonreentrant_as3(#AS3VAR " = CModule.readString(%0, %1);\n": : "r"(CSTRPTR), "r"(LEN))
#define AS3_CopyCharToVar(AS3VAR, CSTRPTR) inline_nonreentrant_as3(#AS3VAR " = CModule.readString(%0, 1);\n": : "r"(CSTRPTR))
    
/* Copies a scalar C value into a local AS3 variable. */
#define AS3_CopyScalarToVar(AS3VAR, VAL) inline_nonreentrant_as3(#AS3VAR " = %0;\n" : : "r"(VAL));

/* Copies an AS3 local variable into a local C variable. */
#define AS3_GetScalarFromVar(CVAR, AS3VAR) inline_nonreentrant_as3("%0 = " #AS3VAR ";\n" : "=r"(CVAR));

/* Copies an AS3 local variable into an AS3::local::var C++ variable. */
#define AS3_GetVarxxFromVar(CVARXX, AS3VAR) do { CVARXX = ({ AS3::local::var r; unsigned id; inline_nonreentrant_as3("%0 = as3_valAcquireId(" #AS3VAR ")" : "=r"(id)); (((AS3::local::internal::id_holder *)&(r))->_id) = id; r; }); } while(0)
    
/* Copies an AS3::local::var C++ variable into an AS3 local variable. */
#define AS3_CopyVarxxToVar(AS3VAR, CVARXX) inline_nonreentrant_as3(#AS3VAR " = as3_id2rcv[%0].val" : : "r"((((AS3::local::internal::id_holder *)&(CVARXX))->_id)))

/**
* This function will send a metric to Adobe Scout with a string value. The metric will 
* show up in the metric summary panel for the frame it was sent in.
*/
void AS3_SendMetricString(const char* metric, const char *value);

/**
* This function will send a metric to Adobe Scout with an integer value. The metric will 
* show up in the metric summary panel for the frame it was sent in.
*/
void AS3_SendMetricInt(const char* metric, int value);

#ifdef __cplusplus
}
#endif /* AS3_H */

#endif
