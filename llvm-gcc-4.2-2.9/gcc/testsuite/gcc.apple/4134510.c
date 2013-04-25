/* APPLE LOCAL file radar 4134510 */
/* { dg-do run { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-save-temps -O1" } */

#include <stdlib.h>
#include <stdio.h>

char *abc31 = "abcdefghijklmnopqrstuvwxyz12345";
char *def31 = "abcdefghijklmnopqrstuvwxyz12345";

char *other_ptr;

void __attribute__ ((noinline))
assign_other (char *str)
{
  other_ptr = str;
}

int
main (int argc, char *argv[])
{
  /* frighten optimizer */
  assign_other (abc31);
  assign_other (def31);

  /* Length <= 30: use rep|cmpsb */
  if (bcmp (abc31, def31, 30))
    abort ();

  /* Length > 30: call library */
  if (bcmp (abc31, def31, 31))
    abort ();

  /* frighten optimizer */
  assign_other (abc31);
  assign_other (def31);

  if (memcmp (abc31, def31, 30))
    abort ();

  if (memcmp (abc31, def31, 31))
    abort ();

  exit (0);
}
/* { dg-final { scan-assembler-times "cmpsb" "2"} } */
/* { dg-final { scan-assembler-times "call\\s*\\w*memcmp" "2"} } */
