#include <stdio.h>
#include "simple.h"

char *bar() {
    printf("hi from c\n");
    return NULL;
}

void print_int(int d) {
    printf("here's an int: %d\n", d);
}

void print_double(double d) {
    printf("a double: %f\n", d);
}

void print_float(float d) {
    printf("a float: %f\n", d);
}

int return42() {
    return 42;
}

float return42f() {
    return 4.2;
}

double return42d() {
    return 4.2;
}

static int (*callback)();
static int (*callback2)(int, double);
static double (*callback3)();
static sbaz (*callback4)();
static void (*callback5)(sbaz);

void set_callback(int (*func)()) {
    callback = func;
}

void set_callback2(int (*funct)(int, double)) {
    callback2 = funct;
}

void set_callback3(double (*func)()) {
    callback3 = func;
}

void set_callback4(sbaz (*func)()) {
    callback4 = func;
}

void set_callback5(void (*func)(sbaz)) {
    callback5 = func;
}

void do_call() {
    int i = callback();
    printf("callback returned %d to c\n", i);
    int j = callback2(123, 8.76);
    printf("callback2 returned %d to c\n", j);
    double d = callback3();
    printf("callback3 returned %f to c\n", d);
}

