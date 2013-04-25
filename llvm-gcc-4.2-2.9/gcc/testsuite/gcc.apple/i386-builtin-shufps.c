/* APPLE LOCAL begin radar 4099352 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O1 -msse2" } */
/* { dg-final { scan-assembler "xorps" } } */

typedef float __m128 __attribute__ ((__vector_size__ (16)));
typedef float __v4sf __attribute__ ((__vector_size__ (16)));

int main()
{ 
 __m128 f2;

 volatile __m128 votf = (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };

 do { 
   XXXXXX();
   volatile __v4sf __t1 = __builtin_ia32_shufps (f2, (__m128){ 0.0f, 0.0f, 0.0f, 0.0f }, 0x44);
   XXXXXX();
 } while (0);
}
/* APPLE LOCAL end radar 4099352 */
