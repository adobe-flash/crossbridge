/* APPLE LOCAL file */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -fno-eliminate-unused-debug-symbols" } */


struct A
{
  struct {
    int aa;
    int ab;
  } data;
};

int main()
{
  return 0;
}

/* { dg-final { scan-assembler-not ".stabs.*A::\$\_:" } } */
