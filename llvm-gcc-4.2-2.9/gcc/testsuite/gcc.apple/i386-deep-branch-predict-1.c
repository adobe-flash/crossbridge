/* APPLE LOCAL file Radar 4010498 et. al. */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-march=pentium4 -fPIC" } */
/* { dg-final { scan-assembler "86.get_pc_thunk" } } */
#define TEST_STRING "test string %d"
#define TEST_STRING0 "test string 0"
#include <stdlib.h>
#include <stdio.h>
int globalvar_i;
main (int argc, char *argv[])
{
  char buf[90];

  sprintf (buf, TEST_STRING, globalvar_i);
  if (strcmp (buf, TEST_STRING0))
    abort ();
  exit (0);
}
