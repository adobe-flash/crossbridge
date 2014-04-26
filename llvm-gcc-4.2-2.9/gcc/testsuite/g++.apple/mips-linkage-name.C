/* Radar 6066486 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-O0 -gdwarf-2 -dA" } */
/* { dg-final { scan-assembler-not "DW_AT_MIPS_linkage_name" } }*/

#include <unistd.h>

int main (int argc, char **argv)
{
  static int spin = 1;
  while (spin) sleep (1);
}

