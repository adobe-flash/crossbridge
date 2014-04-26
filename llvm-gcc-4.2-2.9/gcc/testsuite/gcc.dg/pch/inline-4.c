#include "inline-4.h"
extern int printf (const char *, ...);
int main(void) {
  /* APPLE LOCAL default to Wformat-security 5764921 */
  printf (getstring()); /* { dg-warning "format not a string" } */
  return 0;
}
