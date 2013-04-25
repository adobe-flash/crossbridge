/* APPLE LOCAL file 6951876 */
/* { dg-do compile { target { { i?86-*-* x86_64-*-* } && ilp32 } } } */
/* { dg-options "-O2" } */
/* Kludge: assuming PIC-base labels have a particular format:  */
/* { dg-final { scan-assembler "\"L00\[0-9\]*\\\$pb\":" } } */
extern unsigned char*GetLine(int *s, int y);
typedef struct {
  int dst;
} PIXWEIGHT ;
typedef union {
  int i;
  float f;
} INTTORFLOAT;
void __Rescale(int *src)
{
  int i, y;
  INTTORFLOAT bias;
  INTTORFLOAT f;
  bias.i = 22;
  for (;;)
    {
      GetLine(src, y);
      float * dstata;
      PIXWEIGHT * _p;
      f.f-=bias.f;
      dstata[_p->dst] += f.f;
    }
}
