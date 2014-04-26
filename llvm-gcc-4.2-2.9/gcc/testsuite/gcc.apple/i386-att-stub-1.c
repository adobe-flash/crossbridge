/* APPLE LOCAL file Radar 4164563 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* APPLE LOCAL axe stubs 5571540 */
/* { dg-options "-matt-stubs -mmacosx-version-min=10.4" } */
/* { dg-final { scan-assembler "hlt" } } */

#include <stdio.h>

int main()
{ 
  printf ("hello, world\n");
}
