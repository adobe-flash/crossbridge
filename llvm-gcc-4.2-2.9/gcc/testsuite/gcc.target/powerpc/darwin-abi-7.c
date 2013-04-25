/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* APPLE LOCAL begin 5431747 */
/* Removed xfail.  */
/* APPLE LOCAL end 5431747 */
/* { dg-options "-Wno-long-long" } */

struct b
{
  long long t;
  int i;
};
struct h
{
  int tt;
  struct b d;
  int t;
};

int f[sizeof(struct h)!=24?-1:1];
