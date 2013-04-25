/* APPLE LOCAL file radar 4342153 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O3" } */
/* { dg-final { scan-assembler "jmp\[ \t\]L_g\\\$stub\n|jmp\[ \t\]_g" } } */
/* { dg-final { scan-assembler-not "jmp\[ \t\]L_h\\\$stub\n" } } */
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

