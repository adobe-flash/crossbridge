/* File : example.c */

#include <limits.h>
#include <stdio.h>
#include <stddef.h>

struct Bar {
    int a;
    unsigned b;
    char c;
    double d;
    int nums[4];
};

void set_struct_members(struct Bar *bar) {
    bar->a = -1;
    bar->b = UINT_MAX - 1;
    bar->c = 'A';
    bar->d = 2.654321;
}

void print_struct_members(struct Bar *bar) {
    fprintf(stdout, "%d, ", bar->a);
    fprintf(stdout, "%u, ", bar->b);
    fprintf(stdout, "%c, ", bar->c);
    fprintf(stdout, "%f\n", bar->d);
}

void print_struct_offsets() {
    fprintf(stdout, "a: %u\n", offsetof(struct Bar, a));
    fprintf(stdout, "b: %u\n", offsetof(struct Bar, b));
    fprintf(stdout, "c: %u\n", offsetof(struct Bar, c));
    fprintf(stdout, "d: %u\n", offsetof(struct Bar, d));
}

