/* APPLE LOCAL file */
/* Test whether -fverbose-asm emits option values.  */
/* Contibuted by Devang Patel <dpatel@apple.com>.  */

/* { dg-do compile } */
/* { dg-options "-fverbose-asm" } */
/* { dg-final { scan-assembler "fpeephole" } } */

int
main (int argc, char *argv [])
{
  return 0;
}
