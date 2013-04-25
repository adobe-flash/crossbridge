/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */

void foo() {
  /* Radar 4273624 */
  asm movd xmm3, [ebx*4]
  asm movd xmm3, [8*ebx]
  asm movd xmm3, [ecx+ebx*4]
  asm movd xmm3, [ecx+8*ebx]
  asm movd xmm3, [ebx*4+ecx]
  asm movd xmm3, [8*ebx+ecx]

  /* Radar 4300268 */
  asm movd xmm4, [ebx*4+ecx+4]
  asm movd xmm4, [ebx*4+4+ecx]
  asm movd xmm4, [4*ebx+4+ecx]
  asm movd xmm4, [4*ebx+ecx+4]
  asm movd xmm4, [ecx+ebx*4+4]
  asm movd xmm4, [ecx+4*ebx+4]
  asm movd xmm4, [ecx+4+ebx*4]
  asm movd xmm4, [ecx+4+4*ebx]
  asm movd xmm4, [4+ecx+ebx*4]
  asm movd xmm4, [4+ecx+4*ebx]
  asm movd xmm4, [4+ebx*4+ecx]
  asm movd xmm4, [4+4*ebx+ecx]

  asm movd xmm4, [ebx+ecx+4]
  asm movd xmm4, [ebx+4+ecx]
  asm movd xmm4, [ecx+ebx+4]
  asm movd xmm4, [ecx+4+ebx]
  asm movd xmm4, [4+ecx+ebx]
  asm movd xmm4, [4+ebx+ecx]

  asm movd xmm4, [ebx][ecx][4]
  asm movd xmm4, [ebx][4][ecx]
  asm movd xmm4, [ecx][ebx][4]
  asm movd xmm4, [ecx][4][ebx]
  asm movd xmm4, [4][ecx][ebx]
  asm movd xmm4, [4][ebx][ecx]
}
