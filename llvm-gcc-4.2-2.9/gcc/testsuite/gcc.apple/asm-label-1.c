/* APPLE LOCAL begin CW asm blocks */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O3" } */

inline void func (void)
{
   asm {
       bne again
     again:
   }
}

void SomeOtherFunc (void)
{
   func();
   func();
}
/* APPLE LOCAL end CW asm blocks */
