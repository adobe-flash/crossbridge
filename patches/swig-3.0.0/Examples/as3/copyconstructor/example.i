/* File : example.i */
#ifdef SWIG
%module ExampleModule

%{
#include "example.h"
%}


/* Ignore the Copy Constructor */
%ignore CRectangle(const CRectangle& c);

/* Let's just grab the original header file here */
%include "example.h"

#else
/* This is what clang sees */
#include "example.h"

#endif


