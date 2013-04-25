/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4433955 */

void X2_Interpolate2DNoPinFourCol(const char *sPtr,
                                  int sRowBytes)
{
  int rowfraclo[2];
  asm {
    movd [rowfraclo+4], mm6
    movzx ebx, byte ptr [esi + eax]
    movq mm2, [edx+32]
    mov ecx, ecx
    mov edi, edi
  }
}
