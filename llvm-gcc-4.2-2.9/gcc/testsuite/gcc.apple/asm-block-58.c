/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { -m64 -fasm-blocks -msse3 } } */
/* Radar 4699524 */

void bar() {
  asm {    	
    pushfq
    pushfd
    cld
    REP movsB
    popfd
    popfq
  }
}
