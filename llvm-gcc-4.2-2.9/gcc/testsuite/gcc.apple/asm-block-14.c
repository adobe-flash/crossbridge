/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4271787 */

inline int myrintf(float d)
{
  int i;
  asm {
    fld d
    fistp i
  };
  return i;
}

float d;
int i;

void foo() {
  i = myrintf(d);
}
