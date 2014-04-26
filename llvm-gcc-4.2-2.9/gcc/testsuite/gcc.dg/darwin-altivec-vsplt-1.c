/* APPLE LOCAL file 4119059  */
/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec  -O3" } */
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef signed short SInt16;
typedef signed long SInt32;
typedef unsigned long UInt32;
typedef __attribute__((altivec(vector__))) signed char VSInt8;
typedef __attribute__((altivec(vector__))) unsigned char VUInt8;
typedef __attribute__((altivec(vector__))) signed short VSInt16;

typedef __attribute__((altivec(vector__))) signed int VSInt32;


extern unsigned char *dPtr;
extern  VSInt8 xyz;
unsigned long ix;

void foo ()
{
  VSInt16 Y;
  const VSInt32 vC1 = (VSInt32) (128, 128, 0, 0);
  const VUInt8 vC2 = vec_splat((const VUInt8) vC1,3);

  xyz = vec_sub(xyz, vC2);
  Y = vec_unpackl (xyz);
  vec_st ((VUInt8) Y, ix, dPtr);
}

