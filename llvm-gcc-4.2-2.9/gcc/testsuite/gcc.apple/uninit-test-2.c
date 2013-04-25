/* Radar 4964532 */
/* { dg-do compile } */
/* { dg-options "-O2 -gdwarf-2 -dA" } */
/* { dg-final { scan-assembler-not "DW_OP_APPLE_uninit" } } */
#include <stdio.h>
#include <stdlib.h>

int
main (argc, argv)
     int argc;
     char **argv;
{

  int i, j;
  int prod = 1;

  for (i = 10, j = 3;  i >= 1; i--) {
    prod = prod * i;
  }

  fprintf (stdout, "Answer is %d.\n", prod);
}
