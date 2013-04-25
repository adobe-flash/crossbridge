/* APPLE LOCAL file 6205688 */
/* { dg-do run } */
#include <stdio.h>

void fn2(int i) __attribute__((noinline));
void fn2(int i) {
  printf("fn2 %d\n", i);
}

int fn3(int i) __attribute__((noinline));
int fn3(int i) {
  if (i) return i+1;
  else throw 1;
}

void fn(int i) __attribute__((noinline));
void fn(int i) {
  if (i) {
    fn2(i);
  } else {
    int j = fn3(i);
    printf("j %d\n", j);
  }
}

volatile int arg = 0;

int main(int argc, char **argv) {
  try {
    fn(arg);
  } catch (int e) {
    printf("caught %d\n", e);
  }
  return 0;
}
