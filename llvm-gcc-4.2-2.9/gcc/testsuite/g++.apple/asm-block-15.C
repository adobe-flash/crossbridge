/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4248159 */

void foo() {
  asm {
    movd	mm4, [esi+4]            ; get set 2 source data
    prefetchnta [esi + 32]          ; get index from esi into cache
    movq    	[edi-8], mm1            ; store sets
    mov		dl, [esi + ecx]	// byte to add
    mov		[esi+0], al
  }
}
