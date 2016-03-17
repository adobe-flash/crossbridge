/* File : example.i */
#ifdef SWIG
%module ExampleModule

%{
    #include "example.h"
%}

%include "example.h"
#else
#include "example.h"
#endif


