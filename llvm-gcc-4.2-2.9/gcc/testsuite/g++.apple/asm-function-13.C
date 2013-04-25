/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */

asm void foo()
{
   .align 15 
   andi. r1,r2, 3
   .align  10
   b 	.+8
   .align 4
   b .-16
   .align 3
   lha    r1, 4(r3)
   .align 4
   add   r1,r2,r3
   .align 3
   .align 4
}

int main (int argc, char * const argv[])
{
        foo();
}
