/* File : example.i */
#ifdef SWIG
%module ExampleModule

%{
#include <stdio.h>
%}

%inline %{
extern int foo[5];
%}

%immutable stdout;
#endif

struct doubles {
    char a;
    double array_member[10];
};

struct chars {
    double d;
    char char_member[10];
};

struct simple {
    int a;
    double d;
};

void print_doubles(struct doubles *);
void print_chars(struct chars *);

