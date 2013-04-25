/* APPLE LOCAL file 4515157 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* Ensure that 32-bit x86 does not imply -msse3 on Darwin/x86.  */
#ifdef __SSE3__
#error "SSE3 should be disabled by default for x86 -m32"
#endif
main ()
{
  return 0;
}
