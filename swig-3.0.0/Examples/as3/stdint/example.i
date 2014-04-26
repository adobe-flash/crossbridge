/* File : example.i */
%module ExampleModule

%include <stdint.i>

%{
#include <stdint.h>
%}

%inline %{
extern int32_t a;
extern int_least16_t b;
extern int_fast8_t c;
extern uint8_t d;
extern intptr_t e;
%}
