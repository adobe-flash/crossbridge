#include <stdio.h>

id a = @"Hello World!";

int main() {
  printf("a: %s\n", [a cString]);
  return 0;
}
