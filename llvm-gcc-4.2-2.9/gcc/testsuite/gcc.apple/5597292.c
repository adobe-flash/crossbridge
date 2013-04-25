/* APPLE LOCAL file 5597292 */
/* { dg-do compile } */
/* { dg-options "-static -O0 -gstabs+" } */
/* LLVM LOCAL test not applicable (no stabs) */
/* { dg-require-fdump "" } */
void * foo(unsigned int size)
{
  union {
    char _m[size];
  } *mem;
}
