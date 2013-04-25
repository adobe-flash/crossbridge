/* APPLE LOCAL file shorten-64-to-32 5909621 */
/* { dg-do compile } */
/* { dg-options "-Wshorten-64-to-32" } */
/* Radar 5909621 */

#include <tgmath.h>
#include <stdio.h>

long long int i;

int main (int argc, const char * argv[]) {
  double value = 123.45;
  i = llround(value);
  return 0;
}
