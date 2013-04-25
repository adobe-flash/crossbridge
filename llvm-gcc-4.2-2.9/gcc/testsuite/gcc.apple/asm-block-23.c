/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4248139 */

void foo() {
  asm {
    nop	; 1st insn
    nop	; 2nd insn
    nop	; Don't die here
    nop	; 24 = 11000b
    nop	; # of cols
    nop	; ## of cols
    nop	;  of cols
  }
  int i = 1st;		/* { dg-error "invalid suffix" } */
}
