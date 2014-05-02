/* File : example.c */
#include <stdio.h>

void print_str(int len, char *str) {
  char buf[len + 1];
  snprintf(buf, len + 1, "%s", str);
  printf("%s\n", buf);
}

