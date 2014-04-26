/* APPLE LOCAL file Radar 4388360  */
/* Contributed by Devang Patel <dpatel@apple.com> */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -fobjc-exceptions" } */

void foo(int *p) 
{
  @try {
    p++;
  }
  @finally {}
}

/* { dg-final { scan-assembler-not "=B" } } */
