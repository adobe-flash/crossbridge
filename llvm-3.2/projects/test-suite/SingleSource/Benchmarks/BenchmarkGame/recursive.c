/*
 * The Computer Language Shootout
 * http://shootout.alioth.debian.org/

 * contributed by bearophile, Jan 24 2006
 * modified by wolfjb, Feb 28 2007
 */
#include <stdio.h>

int ack(int x, int y) {
  if (x == 0) {
    return y + 1;
  }

  return ack(x - 1, ((y | 0) ? ack(x, y - 1) : 1));
}

int fib(int n) {
  if (n < 2) {
    return 1;
  }
  return fib(n - 2) + fib(n - 1);
}

double fibFP(double n) {
  if (n < 2.0) {
    return 1.0;
  }
  return fibFP(n - 2.0) + fibFP(n - 1.0);
}

int tak(int x, int y, int z) {
  if (y < x) {
    return tak(tak(x - 1, y, z), tak(y - 1, z, x), tak(z - 1, x, y));
  }
  return z;
}

double takFP(double x, double y, double z) {
    if (y < x)
        return takFP( takFP(x-1.0, y, z), takFP(y-1.0, z, x), takFP(z-1.0, x, y) );
    return z;
}

int main(int argc, char ** argv) {
  int n = 10;

  printf("Ack(3,%d): %d\n", n + 1, ack(3, n+1));
  printf("Fib(%.1f): %.1f\n", 28.0 + n, fibFP(28.0+n));
  printf("Tak(%d,%d,%d): %d\n", 3 * n, 2 * n, n, tak(3*n, 2*n, n));
  printf("Fib(3): %d\n", fib(3));
  printf("Tak(3.0,2.0,1.0): %.1f\n", takFP(3.0, 2.0, 1.0));

  return 0;
}