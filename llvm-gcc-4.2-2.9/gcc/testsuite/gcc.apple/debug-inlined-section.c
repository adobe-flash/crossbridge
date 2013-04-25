/* APPLE LOCAL file, Radar 6275985  */
/* { dg-do compile { target *-*-darwin* } } */
/* LLVM LOCAL -fverbose-asm */
/* { dg-options "-g -dA -save-temps -mmacosx-version-min=10.6 -fverbose-asm" } */
/* { dg-final { scan-assembler "__debug_inlined" } } */
/* { dg-final { scan-assembler "\[#;@]\[ \t]+MIPS linkage name: \"getData\"" } } */
/* { dg-final { scan-assembler "\[#;@]\[ \t]+Function name: \"getData\"" } } */
/* { dg-final { scan-assembler "\[#;@]\[ \t]+MIPS linkage name: \"get_length\"" } } */
/* { dg-final { scan-assembler "\[#;@]\[ \t]+Function name: \"get_length\"" } } */

#include <stdio.h>
#include <stdlib.h>

struct mystruct {
  int data;
};

int globl;

static __inline__ __attribute__((always_inline)) getData (struct mystruct *a) {
  return a->data * globl;
}

#include "debug-inlined-section.h"


main (int argc, char **argv)
{
  struct mystruct c;
  c.data = argc;
  globl = c.data;
//  globl = getData (&c);
  int b = get_length (&c);
  printf ("%d\n", b);
  return (c.data);
}
