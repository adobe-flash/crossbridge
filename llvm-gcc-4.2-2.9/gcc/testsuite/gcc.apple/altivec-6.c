/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

/* Check whether we have too many args... */

typedef __attribute__((altivec(vector__))) unsigned short vUInt16;
typedef __attribute__((altivec(vector__))) unsigned int vUInt32;

int main(void)
{
  vUInt16 vZero = (vUInt16) ( 0 );
  vUInt16 v0;

  vUInt32 sum = vec_unpackl (vZero, v0); /* { dg-error "Too many" } */

  return 0;
  
}

