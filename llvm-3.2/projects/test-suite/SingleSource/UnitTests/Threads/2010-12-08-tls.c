#include <stdio.h>

__thread int a = 4;

int foo (void)
{
  return a;
}

int main (void) {
  printf("a is %d\n", foo());
  return 0;
}
