/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4415216 */

void foo() {
  asm {
    LEA         ESI, [ESI + 4*EBX]
    MOVQ        MM0, [ESI + 4*EBX - 8]
    PUNPCKHWD   MM0, [ESI + 2*EBX - 8]
    PUNPCKHWD   MM2, [ESI + 2*EAX - 8]
    MOVD        [EDI + 4*EBX], MM3
    MOVD        [EDI + 4*EBX - 4], MM0
  }
}
