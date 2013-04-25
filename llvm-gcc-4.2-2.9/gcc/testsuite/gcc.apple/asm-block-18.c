/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 -O2 } } */
/* Radar 4248228 */

int packedw0x80;
typedef int DWORD;
typedef unsigned char unsigned8;
typedef int int32;
#define M_m0		0
#define M_m8		8
#define M_m16		16
#define M_m24		24

extern void e1(const unsigned8 *, unsigned8 *, int32, int32, int32, int32);

typedef struct
{
  DWORD	m0[2];
  DWORD	m8[2];
  DWORD	m16[2];
  DWORD	m24[2];
} M_2;

void
e2(const unsigned8 *srcPtr, unsigned8 *dstPtr, int32 rows, int32 cols,
   int32 sRowBytes, int32 dRowBytes)
{
  int32	sRowB, dRowB, MMXColCnt, r0sum, r0sq, extras;
  M_2	qArray, *pqArray;
  if (rows <= 0  ||  cols <= 0)
    return;
  if (rows <= 1  ||  cols <= 7)
    {
      e1(srcPtr, dstPtr, rows, cols, sRowBytes, dRowBytes);
      return;
    }
  asm {
	mov		ebx, cols
	sub		rows, 1
	mov		ecx, ebx
	and		ecx, 3
	sar		ebx, 2
	mov		extras, ecx
	mov		MMXColCnt, ebx
	mov		eax, sRowBytes
	mov		ebx, cols
   	mov		ecx, dRowBytes
	and		ebx, 0fffffffCh
	mov		esi, eax
	mov		sRowB, eax
	mov		dRowB, ecx
	mov		eax, srcPtr
	lea		edx, qArray
	sub		eax, esi
	add		edx, 7
	and		edx, 0fffffff8h
	mov		srcPtr, eax
	mov		pqArray, edx
	mov		edi, dstPtr
Row:
	movd		mm1, [-1][eax][esi]
	pxor		mm3, mm3
	movd		mm0, [-1][eax]
	pslld		mm1, 24
	movd		mm2, [-1][eax][esi*2]
	punpcklbw	mm1, mm3
	movq		mm4, mm1
	pslld		mm0, 24
	pslld		mm2, 24
	pmullw		mm1, mm1
	punpcklbw	mm0, mm3
	paddw		mm4, mm0
	punpcklbw	mm2, mm3
	pmullw		mm0, mm0
	paddw		mm4, mm2
	pmullw		mm2, mm2
	punpckhwd	mm1, mm3
	movd		mm6, [eax][esi]
	psrlq		mm4, 48
	movd		mm7, [eax]
	punpckhwd	mm0, mm3
	movd		r0sum, mm4
	paddd		mm0, mm1
	movd		mm5, [eax][esi*2]
	punpckhwd	mm2, mm3
	punpcklbw	mm6, mm3
	paddd		mm0, mm2
	psrlq		mm0, 32
	movq		mm1, mm6
	punpcklbw	mm7, mm3
	pmullw		mm1, mm1
	movd		r0sq, mm0
	punpcklbw	mm5, mm3
	paddw		mm6, mm5
	pmullw		mm5, mm5
	paddw		mm6, mm7
	pmullw		mm7, mm7
	movq		mm4, mm1
	punpcklwd	mm1, mm3
	movq		mm0, mm5
	punpckhwd	mm4, mm3
	movq		mm2, mm7
	punpcklwd	mm0, mm3
	paddd		mm1, mm0
	punpcklwd	mm2, mm3

	punpckhwd	mm5, mm3
	paddd		mm1, mm2
	punpckhwd	mm7, mm3
	paddd		mm4, mm5
	paddd		mm4, mm7
	movq		mm0, mm1
	movd		mm2, r0sq
	movq		mm7, mm4
	movq		mm5, mm4
	psrlq		mm1, 32
	paddd		mm2, mm0
	psllq		mm5, 32
	paddd		mm2, mm1
	paddd		mm7, mm1
	paddd		mm2, mm5
	psllq		mm0, 32
	paddd		mm7, mm5
	psrlq		mm4, 32
	paddd		mm2, mm0
	paddd		mm7, mm4
	movq		mm0, mm2
	pslld		mm2, 3
	movd		r0sq, mm4
	paddd		mm2, mm0

	movd		mm1, r0sum
	movq		mm5, mm6
	paddw		mm5, mm1
	movq		mm1, mm6
	psrlq		mm1, 16
	movq		mm4, mm6
	psllq		mm4, 16
	paddw		mm5, mm1
	psrlq		mm6, 48
	paddw		mm5, mm4
	movq		mm0, mm5
	punpcklwd	mm5, mm3
	mov		ecx, MMXColCnt
	pmaddwd		mm5, mm5
	mov		ebx, pqArray
	movq		mm4, mm7
	add		eax, 4
	movd		r0sum, mm6
	psubd		mm2, mm5
Col:
	movd		mm6, [eax][esi]
	movd		mm7, [eax]
	punpcklbw	mm6, mm3
	movd		mm5, [eax][esi*2]
	movq		mm1, mm6
	punpcklbw	mm7, mm3
	pmullw		mm1, mm1
	punpcklbw	mm5, mm3
	paddw		mm6, mm5
	pmullw		mm5, mm5
	paddw		mm6, mm7
	pmullw		mm7, mm7
	movq		[ebx][M_m0], mm6
	psllq		mm6, 48

	movq		[ebx][M_m8], mm4
	paddw		mm6, mm0
	movq		[ebx][M_m16], mm2
	punpckhwd	mm6, mm3

	pmaddwd		mm6, mm6
	movq		mm4, mm1
	punpcklwd	mm1, mm3
	movq		mm0, mm5
	punpckhwd	mm4, mm3
	movq		mm2, mm7
	punpcklwd	mm0, mm3
	paddd		mm1, mm0
	punpcklwd	mm2, mm3

	punpckhwd	mm5, mm3
	paddd		mm1, mm2
	punpckhwd	mm7, mm3
	paddd		mm4, mm5
	paddd		mm4, mm7
	movq		mm0, mm1
	movq		mm2, [ebx][M_m8]
	psllq		mm0, 32
	movq		mm5, [ebx][M_m16]
	paddd		mm0, mm2
	movq		mm2, mm0
	pslld		mm0, 3
	movq		mm7, mm5
	paddd		mm0, mm2
	psubd		mm0, mm6
	movq		mm6, mm5
	movq		mm2, mm0
	pslld		mm6, 10
	movq		mm3, mm0
	pslld		mm7, 4
	pslld		mm2, 10
	paddd		mm6, mm7
	pslld		mm3, 4
	movq		mm7, mm6
	paddd		mm2, mm3
	paddd		mm6, mm6
	movq		mm3, mm2
	paddd		mm6, mm7
	paddd		mm2, mm2
	movq		mm7, mm5
	pslld		mm7, 1
	paddd		mm5, mm7
	pslld		mm7, 1
	paddd		mm5, mm7
	pslld		mm7, 5
	paddd		mm5, mm7
	pslld		mm7, 1
	paddd		mm5, mm7
	psrld		mm5, 9
	paddd		mm2, mm3
	movq		mm7, packedw0x80
	paddd		mm5, mm6
	psrld		mm5, 16
	movq		mm3, mm0
	pslld		mm3, 1
	paddd		mm0, mm3
	pslld		mm3, 1
	paddd		mm0, mm3
	pslld		mm3, 5
	paddd		mm0, mm3
	pslld		mm3, 1
	paddd		mm0, mm3
	psrld		mm0, 9
	movq		mm3, mm5
	push		ecx
	paddd		mm0, mm2
	mov		ecx, 8
	psrld		mm0, 16
	punpckhdq	mm3, mm0
	pxor		mm2, mm2
	punpckldq	mm5, mm0
	pxor		mm0, mm0
	psllq		mm3, 16
	por		mm5, mm3
sqroot:
	por		mm2, mm7
	movq		mm6, mm5
	movq		mm3, mm2
	pmullw		mm2, mm2
	psubusw		mm6, mm2
	psubusw		mm2, mm5
	pcmpeqw		mm2, mm6
	pcmpeqw		 mm6, mm0
	pxor		mm2, mm6
	pand		mm2, mm7
	psrlw		mm7, 1
	pxor		mm2, mm3
	dec		ecx
	jnz		sqroot
	pop		ecx
	packuswb	mm2, mm2
	movq		mm6, [ebx][M_m0]
	pxor		mm3, mm3

	movd		[edi], mm2
	movq		mm0, mm1
	movd		mm2, r0sq
	movq		mm7, mm4
	paddd		mm2, mm0
	psrlq		mm1, 32
	movq		mm5, mm4
	paddd		mm2, mm1
	psllq		mm5, 32
	paddd		mm7, mm1
	paddd		mm2, mm5
	paddd		mm7, mm5
	psllq		mm0, 32
	paddd		mm2, mm0
	psrlq		mm4, 32
	paddd		mm7, mm4
	movq		mm0, mm2
	pslld		mm2, 3
	movd		mm1, r0sum
	paddd		mm2, mm0

	movd		r0sq, mm4
	movq		mm5, mm6
	paddw		mm5, mm1
	movq		mm1, mm6
	psrlq		mm1, 16
	movq		mm4, mm6
	psllq		mm4, 16
	paddw		mm5, mm1
	psrlq		mm6, 48
	paddw		mm5, mm4
	movq		mm0, mm5
	punpcklwd	mm5, mm3
	movd		r0sum, mm6
	pmaddwd		mm5, mm5
	add		eax, 4
	add		edi, 4
	psubd		mm2, mm5
	movq		mm4, mm7
	dec		ecx
   	jnz		Col
	mov		ecx, extras
	cmp		ecx, 0
	je		EndRow
	movd		mm6, [eax][esi]
	movd		mm7, [eax]
	punpcklbw	mm6, mm3
	movd		mm5, [eax][esi*2]
	movq		mm1, mm6
	punpcklbw	mm7, mm3
	pmullw		mm1, mm1
	punpcklbw	mm5, mm3
	paddw		mm6, mm5
	pmullw		mm5, mm5
	paddw		mm6, mm7
	pmullw		mm7, mm7
	movq		[ebx][M_m0], mm6
	psllq		mm6, 48

	movq		[ebx][M_m8], mm4
	paddw		mm6, mm0
	movq		[ebx][M_m16], mm2
	punpckhwd	mm6, mm3

	pmaddwd		mm6, mm6
	movq		mm4, mm1
	punpcklwd	mm1, mm3
	movq		mm0, mm5
	punpckhwd	mm4, mm3
	movq		mm2, mm7
	punpcklwd	mm0, mm3
	paddd		mm1, mm0
	punpcklwd	mm2, mm3

	punpckhwd	mm5, mm3
	paddd		mm1, mm2
	punpckhwd	mm7, mm3
	paddd		mm4, mm5
	paddd		mm4, mm7
	movq		mm0, mm1
	movq		mm2, [ebx][M_m8]
	psllq		mm0, 32
	movq		mm5, [ebx][M_m16]
	paddd		mm0, mm2
	movq		mm2, mm0
	pslld		mm0, 3
	movq		mm7, mm5
	paddd		mm0, mm2
	psubd		mm0, mm6
	movq		mm6, mm5
	movq		mm2, mm0
	pslld		mm6, 10
	movq		mm3, mm0
	pslld		mm7, 4
	pslld		mm2, 10
	paddd		mm6, mm7
	pslld		mm3, 4
	movq		mm7, mm6
	paddd		mm2, mm3
	paddd		mm6, mm6
	movq		mm3, mm2
	paddd		mm6, mm7
	paddd		mm2, mm2
	movq		mm7, mm5
	pslld		mm7, 1
	paddd		mm5, mm7
	pslld		mm7, 1
	paddd		mm5, mm7
	pslld		mm7, 5
	paddd		mm5, mm7
	pslld		mm7, 1
	paddd		mm5, mm7
	psrld		mm5, 9
	paddd		mm2, mm3
	movq		mm7, packedw0x80
	paddd		mm5, mm6
	psrld		mm5, 16
	movq		mm3, mm0
	pslld		mm3, 1
	paddd		mm0, mm3
	pslld		mm3, 1
	paddd		mm0, mm3
	pslld		mm3, 5
	paddd		mm0, mm3
	pslld		mm3, 1
	paddd		mm0, mm3
	psrld		mm0, 9
	movq		mm3, mm5
	push		ecx
	paddd		mm0, mm2
	mov		ecx, 8
	psrld		mm0, 16
	punpckhdq	mm3, mm0
	pxor		mm2, mm2
	punpckldq	mm5, mm0
	pxor		mm0, mm0
	psllq		mm3, 16
	por		mm5, mm3
sqrootExtras:
	por		mm2, mm7
	movq		mm6, mm5
	movq		mm3, mm2
	pmullw		mm2, mm2
	psubusw		mm6, mm2
	psubusw		mm2, mm5
	pcmpeqw		mm2, mm6
	pcmpeqw		mm6, mm0
	pxor		mm2, mm6
	pand		mm2, mm7
	psrlw		mm7, 1
	pxor		mm2, mm3
	dec		ecx
	jnz		sqrootExtras
	pop		ecx
	packuswb	mm2, mm2
	movq		mm6, [ebx][M_m0]
	pxor		mm3, mm3

	movd		ebx, mm2
	mov		ecx, extras
StoreExtras:
	mov		[edi], bl
	inc		edi
	shr		ebx, 8
	dec		ecx
	jg		StoreExtras
EndRow:
	mov		eax, srcPtr
	mov		edi, dstPtr
   	mov		edx, dRowB
	add		eax, esi
	mov		ebx, rows
	mov		srcPtr, eax
	add		edi, edx
	dec		ebx
	mov		dstPtr, edi
	mov		rows, ebx
	jnz		Row
	mov		rows, 1
	add		eax, esi
	mov		srcPtr, eax
	mov		dstPtr, edi
	emms
  }
  e1(srcPtr, dstPtr, rows, cols, sRowBytes, dRowBytes);
}
