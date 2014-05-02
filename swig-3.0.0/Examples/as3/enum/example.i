/* File : example.i */
%module ExampleModule

%{
#include "example.h"
%}

/* Let's just grab the original header file here */

%include "example.h"

