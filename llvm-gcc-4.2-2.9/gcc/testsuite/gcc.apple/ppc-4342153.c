/* APPLE LOCAL file radar 4342153 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-O3" } */
/* { dg-final { scan-assembler "b\[ \t\]L?_g" } } */
/* { dg-final { scan-assembler-not "b\[ \t\]L?_h" } } */
void f ();
void g ();
void h (unsigned char *j)
{
  int a = *j;
  if (a > 0xFF)
    f ();
  else
    g ();
}

