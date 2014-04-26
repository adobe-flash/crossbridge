/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

void _sqrt()
{
     __vector float vNAN = (__vector float)(__builtin_nanf("0x7fc00000"));
}
