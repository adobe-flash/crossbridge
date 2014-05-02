/* File : example.i */
#ifdef SWIG
%module ExampleModule

%{
#include <stdio.h>
#include "example.h"
%}

/* Let's just grab the original header file here */
%include "example.h"

#else
/* This is what clang sees */
#include "example.h"

#endif


