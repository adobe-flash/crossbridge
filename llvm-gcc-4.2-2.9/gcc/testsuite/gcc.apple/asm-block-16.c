/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4273781 */

void lock(int);

void foo() {
  asm lock cmpxchg [ecx], ebx
  asm lock
  lock (1);
}
