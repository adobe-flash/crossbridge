/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4505741 */

static int c[5];
void foo(int pa[5], int j) {
  unsigned int *ptr = (unsigned int *)0x12345678;
  static int b[5];
  int i;
  int a[5];
  _asm {
    mov   esi, [ptr][0]
    mov   esi, [ptr]
    mov   esi, [esi][eax]
    mov   esi, [esi+eax]
    mov   esi, [esi+eax+4]
    mov   esi, [esi][eax][4]
    mov   esi, [a][4]
    mov   esi, [pa]
    mov   esi, [j]
    mov   esi, [i]
    mov   esi, i
    mov   esi, [b][4]	/* { dg-warning "non-pic addressing form not suitible for pic code" } */
    mov   esi, [c][4]	/* { dg-warning "non-pic addressing form not suitible for pic code" } */
    mov   esi, [b]	/* { dg-warning "non-pic addressing form not suitible for pic code" } */
    mov   esi, [c]	/* { dg-warning "non-pic addressing form not suitible for pic code" } */
    mov   esi, [ptr][4]        /* { dg-warning "will consume extra register" } */
    mov   esi, [ptr+4]         /* { dg-warning "will consume extra register" } */
    mov   esi, [ptr][eax]      /* { dg-warning "will consume extra register" } */
    mov   esi, [ptr+eax]       /* { dg-warning "will consume extra register" } */
    mov   esi, [-4][pa+esi]    /* { dg-warning "will consume extra register" } */
    mov   esi, [-4][j+esi]     /* { dg-warning "will consume extra register" } */
    mov   esi, [pa-4+esi]      /* { dg-warning "will consume extra register" } */
    mov   esi, [a][3]          /* { dg-warning "will consume extra register" } */
    jmp   [a+4*ebx]            /* { dg-warning "will consume extra register" } */
  }
}
