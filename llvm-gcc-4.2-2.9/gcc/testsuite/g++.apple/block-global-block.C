/* APPLE LOCAL file radar 5822844 */
/* Test that blocks can be declared at global scope. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -fblocks" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

static void* _NSConcreteGlobalBlock;


typedef void (^ HelloBlock_t)(const char * name);

HelloBlock_t helloBlock = ^(const char * name)
{
  printf("Hello there, %s!\n", name);
};

static HelloBlock_t s_helloBlock = ^(const char * name)
{
  printf("Hello there, %s!\n", name);
};

int X = 1234;
int (^CP)(void) = ^{ X = X+1;  return X; };

int
main(int argc, char * argv[])
{
  helloBlock("world");
  s_helloBlock("world");

  CP();
  printf ("X = %d\n", X);
  return X - 1235;
}
