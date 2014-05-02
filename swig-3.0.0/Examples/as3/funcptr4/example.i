/* File : example.i */
%module ExampleModule
%{
#include <stdio.h>
#include "example.h"
%}

%typemap(in) (int, char *) {
    AS3_MallocString($2, $input);
    AS3_StringLength($1, $input);
};

%typemap(out) (int, char *) {
    AS3_DeclareVar($input, String);
    AS3_CopyCStringToVar($input, $2, $1);
};

%typemap(astype) (int , char *) "String";

%typemap(freearg) (int , char *) {
    printf("cleaning up\n");
    free($2);
};
%include "example.h"

