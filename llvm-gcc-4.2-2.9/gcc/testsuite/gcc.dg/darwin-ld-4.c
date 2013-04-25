/* Test Darwin linker option -bundle.  */
/* Developed by Devang Patel <dpatel@apple.com>.  */

/* { dg-options "-bundle" } */
/* APPLE LOCAL begin mainline 4.3 2007-06-14 */ \
/* { dg-do link { target *-*-darwin* } } */

/* APPLE LOCAL end mainline 4.3 2007-06-14 */ \
int main()
{
  return 0;
}

