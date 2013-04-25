/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */
/* Existing AltiVec PIM implementations allow numeric literals with
   any number of bits, so long as the value is within allowed range.  */
int foo(void)
{
        vector unsigned char amask = vec_splat_u8 ( 0xFFFFFFF3L );
        vector signed short bmask = vec_splat_s16 ( 0xFFFFFFFFFFFFFFF1LL );
}


/* { dg-final { scan-assembler "vspltisb.+\\-13" } } */
/* { dg-final { scan-assembler "vspltish.+\\-15" } } */

