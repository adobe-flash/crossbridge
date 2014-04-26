/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* APPLE LOCAL begin 5431747 */
/* Removed xfail.  */
/* APPLE LOCAL end 5431747 */

struct c
{
  double d;
  int i;
};

struct j
{
  int tt;
  struct c d;
  int t;
};

int f[sizeof(struct j)!=24?-1:1];
