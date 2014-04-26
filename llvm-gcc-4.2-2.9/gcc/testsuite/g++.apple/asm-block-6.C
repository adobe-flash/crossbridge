/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4150131 */

int main() {
  asm {
    add r0,r0,r0
  }
}

#pragma options align=natural
#pragma options align=reset
