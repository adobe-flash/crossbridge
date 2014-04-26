/* Test default cpu type */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* This test doesn't make sense on 64-bit */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-mmacosx-version-min=10.5" } */
int main()
{
  return 0;
}

/* { dg-final { scan-assembler "machine ppc7400" } } */
