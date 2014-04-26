/* File : example.i */
#ifdef SWIG
%module ExampleModule

%{
    #include "example.h"
%}

%include "example.h"

%template(intList) List<int>; 

#else
#include "example.h"
#endif
