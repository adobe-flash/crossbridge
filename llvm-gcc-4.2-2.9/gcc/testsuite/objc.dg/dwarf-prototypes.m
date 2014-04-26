/* APPLE LOCAL file radar 4355182 */
/* Check that AT_prototyped is generated for objective-c as well. */
/* { dg-options "-mmacosx-version-min=10.5 -gdwarf-2 -dA" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-gdwarf-2 -dA" { target arm*-*-darwin* } } */
/* { dg-final { scan-assembler "DW_AT_prototyped" } } */

int foo (int);

int
foo (int x)
{
  return x;
}

int 
main (int argc, char *argv)
{
  return foo (5);
}
