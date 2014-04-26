/* APPLE LOCAL file 4356747 stack realign */
/* { dg-do compile { target i?86-*-darwin* i[6789]86-*-* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* APPLE LOCAL Note: if/when this file arrives in a merge, the FSF
   version probably won't have the -fnested-functions option, and it's
   required on OS X.  */
/* { dg-options "-std=gnu99 -fnested-functions -mstackrealign" } */
#include <stdio.h>
#include <stdlib.h>

int
outer_function (int x, int y)
{
  int __attribute__ ((__noinline__))
  nested_function (int x, int y)
    { /* { dg-error "-mstackrealign ignored for nested functions" } */
      return (x + y);
    }
  return (3 + nested_function (x, y));
}
