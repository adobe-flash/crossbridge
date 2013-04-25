/* APPLE LOCAL begin CW asm blocks */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O3" } */

inline void func (void)
{
   asm {
        li r0,0
        stw r0,24(r30)
        b L2
L3:
        lwz r0,24(r30)
        slwi r0,r0,2
        add r2,r0,r2
        li r0,0
        stw r0,0(r2)
        lwz r2,24(r30)
        addi r0,r2,1
        stw r0,24(r30)
L2:
        lwz r0,24(r30)
        cmpwi cr7,r0,100
        ble cr7,L3

   }
}

void SomeOtherFunc (void)
{
   func();
   func();
}
/* APPLE LOCAL end CW asm blocks */
