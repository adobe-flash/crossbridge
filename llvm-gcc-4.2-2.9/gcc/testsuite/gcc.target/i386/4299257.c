/* APPLE LOCAL file 4299257 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-require-effective-target sse4 } */
/* Require a test of the parity flag.  */
/* { dg-final { scan-assembler "\t\(j|set\)n?p\t" } } */
typedef float vFloat __attribute__((vector_size(16)));
char *t(vFloat a, vFloat b)
{
  return __builtin_ia32_ucomieq(a, b) ? "y" : "n";
}
