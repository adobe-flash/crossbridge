/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -O2 } } */
/* Radar 5315594 */

typedef unsigned char unsigned8;
typedef long long int64;
typedef long int32;
typedef short int16;
typedef char int8;
typedef unsigned long uint32;
typedef uint32 DWORD;
struct MMARRAY
{
 DWORD RESULT1[ 2 ];
 DWORD RESULT2[ 2 ];
 DWORD Fst4DSTPIX[ 2 ];
 DWORD Snd4DSTPIX[ 2 ];
 DWORD _scr[ 2 ];
};

void X_ScreenBlend ( const unsigned8 *sPtr, unsigned8 *dPtr, const unsigned8 *mPtr,
      int32 rows, int32 cols, int32 sRowBytes, int32 dRowBytes,
      int32 mRowBytes)
{
 int32 sRowB, dRowB, mRowB, Loop8Extras, NumCols;
 int64 RNDVAL = 0x007F007F007F007FLL;
 int64 LoByteMask = 0x00FF00FF00FF00FFLL;
 int64 val254 = 0x00fe00fe00fe00feLL;
 int64 ZEROMASK = 0x0;
 int64 ONESMASK = 0x0001000100010001LL;
 int64 SIGNMASK = 0x8000800080008000LL;

 MMARRAY *pqArray;
 MMARRAY qArray;

 if ( rows <= 0 || cols <= 0 )
   return;

 asm {
   xor eax, eax
   xor edx, edx
   mov ax, WORD PTR sRowBytes
   xor ecx, ecx
   mov dx, WORD PTR dRowBytes
   mov sRowB, eax
   mov cx, WORD PTR mRowBytes
   xor eax, eax
   mov ax, WORD PTR cols
   mov mRowB, ecx
   mov dRowB, edx
   lea ecx, qArray
   mov ebx, eax
   add ecx, 7
   shr eax, 3
   and ecx, 0fffffff8h
   mov pqArray, ecx
   and ebx, 7
   mov NumCols, eax
   mov Loop8Extras, ebx
  Row:
   mov esi, sPtr
   mov edi, dPtr
   mov edx, mPtr
   mov ebx, sRowB
   mov ecx, dRowB
   add ebx, esi
   mov eax, mRowB
   mov sPtr, ebx
   add ecx, edi
   add eax, edx
   mov dPtr, ecx
   mov mPtr, eax
   mov ecx, NumCols
   mov ebx, 4
 }
}
