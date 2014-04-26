/* { dg-do compile } */
/* { dg-options "-O2 -fprofile-generate -fprofile-use" } */
/* LLVM LOCAL profiling support needed */
/* { dg-require-profiling "" } */

unsigned test (unsigned a, unsigned b)
{
  return a / b;
} /* { dg-warning "execution counts estimated" } */
