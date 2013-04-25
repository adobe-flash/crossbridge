/* APPLE LOCAL file - testcase for Radar 5811943  */
/* { dg-do compile } */
/* LLVM LOCAL - -fverbose-asm */
/* { dg-options "-g -O0 -fblocks -dA -fverbose-asm" } */
/* { dg-final { scan-assembler "DW_AT_APPLE_block" } } */

#include <stdio.h>
void * _NSConcreteStackBlock[32];

int
blockTaker (int (^myBlock)(int), int other_input)
{
  return 5 * myBlock (other_input);
}

int main (int argc, char **argv)
{
  int (^blockptr) (int) = ^(int inval) {
    printf ("Inputs: %d, %d.\n", argc, inval);
    return argc * inval;
  };


  argc = 10;
  printf ("I got: %d.\n",
	  blockTaker (blockptr, 6));
  return 0;
}
