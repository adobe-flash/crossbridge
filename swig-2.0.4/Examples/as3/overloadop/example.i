/* File : example.i */
#ifdef SWIG
%module ExampleModule

%rename(divop) Shape::operator/;
%{
#include "example.h"
%}

/* Let's just grab the original header file here */
%include "example.h"

#else
/* This is what clang sees */
#include "example.h"

#endif


