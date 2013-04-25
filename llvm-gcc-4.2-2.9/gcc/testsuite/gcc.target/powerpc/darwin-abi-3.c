/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* APPLE LOCAL begin 5431747 */
/* Removed xfail.  */
/* APPLE LOCAL end 5431747 */
/* { dg-options "-Wno-long-long" } */
struct f
{
  int i;
  long long ll;
};

int f[sizeof(struct f)!=12?-1:1];
