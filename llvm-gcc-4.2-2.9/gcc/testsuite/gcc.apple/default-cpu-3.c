/* Test default cpu type */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -mcpu=G3" } */
int main()
{
  return 0;
}

/* { dg-final { scan-assembler "machine ppc" } } */
/* { dg-final { scan-assembler-not "machine ppc7400" } } */

