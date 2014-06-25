/* File : example.i */
%module ExampleModule

#ifdef SWIG
%include "example.h"
#else
#include "example.h"
#endif

