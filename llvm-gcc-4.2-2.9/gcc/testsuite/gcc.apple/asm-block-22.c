/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4248205 */

typedef int DWORD;

struct MMARRAY
{
  DWORD RESULT1[ 2 ];
  DWORD RESULT2[ 2 ];
  DWORD Fst4DSTPIX[ 2 ];
  DWORD Snd4DSTPIX[ 2 ];
  DWORD _scr[ 2 ];
};
#define MMARRAY_RESULT1     0
#define MMARRAY_RESULT2     8
#define MMARRAY_Fst4DSTPIX 16
#define MMARRAY_Snd4DSTPIX 24
#define MMARRAY__scr       32

void foo() {
  asm {
    movq [eax].MMARRAY_Snd4DSTPIX, mm7
    movq mm4, [ebx].MMARRAY_RESULT2
  }
}
