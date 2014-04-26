/* APPLE LOCAL file pascal strings */
/* Ensure that Pascal strings do not get confused with ordinary C strings when 
   -funsigned-char is being used.  */
/* Author: Ziemowit Laski <zlaski@apple.com> */

/* { dg-do compile } */
/* { dg-options "-fpascal-strings -funsigned-char" } */

typedef unsigned char Str15[16];

Str15 ggg = "\p012345678901234";
Str15 hhh = "0123456789012345"; /* { dg-error "initializer.string for array of chars is too long" } */

int foo(void)
{
  Str15 sss = "\p012345678901234";
  Str15 ttt = "0123456789012345"; /* { dg-error "initializer.string for array of chars is too long" } */

  return 0;
}
