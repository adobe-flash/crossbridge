/* Basic test of the -mmacosx-version-min option.  */

/* APPLE LOCAL 64-bit default */
/* { dg-options "-mmacosx-version-min=10.1 -m32" } */
/* APPLE LOCAL ARM */
/* { dg-do link { target powerpc*-*-darwin* i?86*-*-darwin* } } */

int main()
{
  return 0;
}

