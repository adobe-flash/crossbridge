/* File : example.i */
%module ExampleModule

%{
#include "example.h"
%}

/* Let's just grab the original header file here */
%include "example.h"

/* Now instantiate some specific template declarations */

%template(maxint) max<int>;
%template(maxdouble) max<double>;
%template(Vecint) vector<int>;
%template(Vecdouble) vector<double>;

