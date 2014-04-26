/* APPLE LOCAL file 4196991 */
/* Avoid changing the fp control word for fp to uint */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O2 -msse3" } */
unsigned foo(float *a) { return *a; }
/* { dg-final { scan-assembler-not "fldcw" } } */
