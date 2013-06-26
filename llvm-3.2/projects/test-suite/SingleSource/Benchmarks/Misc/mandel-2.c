#include <stdio.h>
#include <stdlib.h>
#include <complex.h>

int max_i = 65536;

double sqr(double x) { return x*x; }

double cnorm2(complex z) { return sqr(creal(z)) + sqr(cimag(z)); }

int loop(complex c) {
    complex z=c;
    int i=1;
    while (cnorm2(z) <= 4.0 && i++ < max_i)
        z = z*z + c;
    return i;
}

int main() {
  int i, j;
    for (j = -39; j < 39; ++j) {
        for (i = -39; i < 39; ++i)
            printf(loop(j/40.0-0.5 + i/40.0*I) > max_i ? "*" : " ");
        printf("\n");
    }
    return 0;
}

