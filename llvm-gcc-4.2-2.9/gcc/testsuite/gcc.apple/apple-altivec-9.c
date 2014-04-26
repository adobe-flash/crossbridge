/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec -ftrapping-math" } */

int vConvert_PlanarFtoPlanar16F( )
{
    vector float twoP10 = (vector float) (0x1.0p+10f, 0x1.0p+24f, 0x1.0p+102f, 1.0f/0.0f ); 
    return 0;
}
