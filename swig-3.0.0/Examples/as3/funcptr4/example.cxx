/* File : example.cxx */
#include <stdio.h>
#include <stdlib.h>
#include "example.h"

char *str;

static const char *default_str_func() {
    return "this string is from c";
}

static char *c_str_func(char *str, int size) {
    printf("%s (size: %d)\n", str, size);
    char *buf = (char *) malloc(size * 2 + 1);
    sprintf(buf, "%s%s", str, str);
    return buf;
}

static str_func default_getter() {
    return c_str_func;
}

static str_func a_str_func;


const char *(*char_func)() = default_str_func;
str_func (*str_func_getter)() = default_getter;
void (*str_func_setter)(str_func) = NULL;

void call_getter() {
    char *s = str_func_getter()("foo", 4);
    printf("valued returned to c: %s\n", s);
}

void call_str_func() {
    char *s = a_str_func("test string", 12);
    printf("the stored str_func returned: %s\n", s);
}

void a_str_func_setter(str_func a) {
    a_str_func = a;
}

