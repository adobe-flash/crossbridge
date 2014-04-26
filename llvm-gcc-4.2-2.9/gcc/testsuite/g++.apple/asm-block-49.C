/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target *-*-darwin* } } */
/* { dg-options { -fasm-blocks } } */
/* Radar 4477426 4466768 */

void foo() {
  asm .machine all
#ifdef __i386__
  asm .word 1
#endif
  asm opword 1
  asm .byte 1,2
  asm .short 1,2
  asm .long 1,2
  asm .quad 3,4,5
  asm _emit 5, 6
}
