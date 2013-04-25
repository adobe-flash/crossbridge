/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4419735 */

void foo(short* pSrc) {
  short oldcw;
  asm {
    mov esi, pSrc
    fild [WORD PTR esi]
    fild [WORD PTR esi + eax*2]
    fstcw oldcw
    fnstcw oldcw
  }
}
