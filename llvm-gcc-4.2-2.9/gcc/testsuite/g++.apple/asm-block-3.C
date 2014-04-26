/* APPLE LOCAL file CW asm blocks */
/* Test C++ keywords in asm-syntax blocks within functions.  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

void
bar ()
{
  register unsigned int theColor;
  register unsigned char bbb;
  asm {and++     bbb,theColor,24; };
  asm {and       bbb,theColor,24; };
  asm {and_eq    bbb,theColor,24; };
  asm {bitand    bbb,theColor,24; };
  asm {bitor     bbb,theColor,24; };
  asm {compl     bbb,theColor,24; };
  asm {not       bbb,theColor,24; };
  asm {not_eq    bbb,theColor,24; };
  asm {or        bbb,theColor,24; };
  asm {or_eq     bbb,theColor,24; };
  asm {xor       bbb,theColor,24; };
  asm {xor_eq    bbb,theColor,24; };
}
