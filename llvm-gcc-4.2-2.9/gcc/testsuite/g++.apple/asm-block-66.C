/* APPLE LOCAL file CW asm blocks 6276214 */
/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks } } */
/* Radar 6276214 */

void foo () {
  asm test esi, 24 ; 11000b
}
