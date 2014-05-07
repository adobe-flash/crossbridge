%module simple
%{
#include "simple.h"
%}

#ifndef __SIMPLE_H__
#define __SIMPLE_H__

//int foo;
char *bar();
void print_int(int d);
void print_float(float f);
void print_double(double d);
int return42();
float return42f();
double return42d();
void set_callback(int (*func)());
void set_callback2(int (*funct)(int, double));
void set_callback3(double (*func)());
//void set_callback4(sbaz (*func)());
//void set_callback5(void (*func)(sbaz));
void do_call();
//int main(int argc, char **argv);

struct baz {
    int a;
    char b;
};

#endif // __SIMPLE_H__
