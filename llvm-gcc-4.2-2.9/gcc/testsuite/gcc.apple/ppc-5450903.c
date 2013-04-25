/* APPLE LOCAL file radar 5450903 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-Os" } */
float foo (float a, float b, float c)
{
  float result;
  asm volatile ("fmul %0, %1, %2 \n fadds %0, %0, %3" : "=&f" (result) :
                "f" (a), "f" (b), "f" (c) );  
  return result;
}
