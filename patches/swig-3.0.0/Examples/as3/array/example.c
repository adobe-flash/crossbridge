/* File : example.c */

#include <stdio.h>
#include "example.i"

/* A global variable */
int foo[5];

void print_doubles(struct doubles *d) {
    int i;
    d->array_member[5] = 1.23;
    for (i = 0; i < 10; i++) {
        printf("%.2f ", d->array_member[i]);
    }
    printf("\n");
}

void print_chars(struct chars *c) {
    int i;
    for (i = 0; i < 10; i++) {
        printf("%c ", c->char_member[i]);
    }
    printf("\n");
}

