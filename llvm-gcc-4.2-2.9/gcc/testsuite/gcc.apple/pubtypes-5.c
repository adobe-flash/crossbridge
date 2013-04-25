/* APPLE LOCAL file  */
/* Radar 5619139  */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-O0 -gdwarf-2 -dA -c" } */
/* { dg-skip-if "Unmatchable assembly" { mmix-*-* } { "*" } { "" } } */
/* { dg-final { scan-assembler "__debug_pubtypes" } } */
/* { dg-final { scan-assembler-not "Length of Public Type Names Info" } } */

struct Foo;
typedef struct Foo Foo;

extern Foo *foo_getter (int);
extern int foo_taker (Foo *);

int main ()
{
  Foo *mine;
  
  mine = foo_getter (5);
  foo_taker (mine);
  
  return 0;
}
