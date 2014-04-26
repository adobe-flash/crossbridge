/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4259442 */

asm int bar4() { int 1 }
void bar() { asm { foo: int     1 } }
int bar1() { asm { int 1 } }
int bar2() { asm int 1 asm int 1 }
int bar3() { int i; asm { int 1 } }
