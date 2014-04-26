/* APPLE LOCAL file pascal strings */
/* Ensure that there are no warnings or errors issued when a Pascal string is used to
   initialize an array and the NUL terminator does not fit.  */
/* Author: Ziemowit Laski <zlaski@apple.com> */

/* { dg-do compile } */
/* { dg-options "-fpascal-strings" } */

typedef unsigned char Str15[16];

Str15 ggg = "\p012345678901234";
Str15 hhh = "\p0123456789012345"; /* { dg-warning "initializer.string for array of chars is too long" } */

int foo(void)
{
  Str15 sss = "\p012345678901234";
  Str15 ttt = "\p0123456789012345"; /* { dg-warning "initializer.string for array of chars is too long" } */

  return 0;
}
