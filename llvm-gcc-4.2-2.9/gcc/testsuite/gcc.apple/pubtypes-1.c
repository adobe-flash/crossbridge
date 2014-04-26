/* Radar 4535968 */
/* { dg-do compile { target *-*-darwin* } } */
/* LLVM LOCAL -fverbose-asm */
/* { dg-options "-O0 -gdwarf-2 -dA -fverbose-asm -fno-eliminate-unused-debug-types" } */
/* { dg-skip-if "Unmatchable assembly" { mmix-*-* } { "*" } { "" } } */
/* { dg-final { scan-assembler "__debug_pubtypes" } } */
/* APPLE LOCAL begin ARM assembler uses @ for comments */
/* LLVM LOCAL begin - Adjust for different (but apparently correct) output */
/* { dg-final { scan-assembler "Lset\[0-9]+\[ \t]+=\[ \t]+Lpubtypes_end\[0-9]+-Lpubtypes_begin\[0-9]+\[ \t]+\[#;@]\[ \t]+Length of Public Types Info" } } */
/* LLVM LOCAL end */
/* { dg-final { scan-assembler "used_struct\\\\0\"\[ \t]+\[#;@]\[ \t]+external name" } } */
/* { dg-final { scan-assembler "unused_struct\\\\0\"\[ \t]+\[#;@]\[ \t]+external name" } } */
/* APPLE LOCAL end ARM assembler uses @ for comments */

#include <stdlib.h>
#include <stdio.h>

struct used_struct 
{
  int key;
  char *name;
};

struct unused_struct
{
  int key1;
  int f2;
  double f3;
  char *f4;
  struct unused_struct *next;
};

int
main (int argc, char **argv)
{
  int i;
  struct used_struct *my_list;

  my_list = (struct used_struct *) malloc (10 * sizeof (struct used_struct));
  
  for (i = 0; i < 10; i++)
    {
      my_list[i].key = i;
      my_list[i].name = (char *) malloc (11);
      sprintf (my_list[i].name, "Caroline_%d", i);
    }

  for (i = 0; i < 10; i++)
    fprintf (stdout, "Key: %d, Name: %s\n", my_list[i].key, my_list[i].name);
  
  return 0;
}
