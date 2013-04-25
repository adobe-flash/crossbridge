/* APPLE LOCAL file shorten-64-to-32 5909621 4604239 */
/* { dg-do compile } */
/* { dg-options "-Wshorten-64-to-32" } */
/* Radar 4604239 */

#include <tgmath.h>

double f;
int e;

typedef double CFTimeInterval;
typedef CFTimeInterval CFAbsoluteTime;

double foo(void) { return cos(0.); }

int main(int argc, const char *argv[])
{
  CFAbsoluteTime absoluteTime = CFAbsoluteTimeGetCurrent();
  CFAbsoluteTime divisor = 2.0;
  CFAbsoluteTime theRemainder = remainder(absoluteTime, divisor);
  return 0;

  f = pow (f, f);
  f = fma (f, f, f);
  f = frexp ( f, &e);
  f = remquo (f, f, &e);
}
