/* APPLE LOCAL file 4080358 */
/* Test if constant CFstrings play nice with -fwritable-strings.  */
/* Author: Ziemowit Laski  */

/* { dg-options "-fconstant-cfstrings -fwritable-strings -framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */

#include <Foundation/NSString.h>
#include <stdlib.h>
#include <memory.h>

typedef const struct __CFString * CFStringRef;

static CFStringRef foobar = (CFStringRef)@"Apple";

int main(void) {
  char *c, *d;

  c = (char *)[(id)foobar cString];
  d = (char *)[(id)@"Hello" cString];

  if (*c != 'A')
    abort ();

  if (*d != 'H')
    abort ();

  return 0;
}
