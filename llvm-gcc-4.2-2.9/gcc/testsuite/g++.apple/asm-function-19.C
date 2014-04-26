/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { -fasm-blocks -msse3 -O3 -Winline -Wall } } */
/* Radar 4381918 */

/* This should have no prologue/epilogue */
inline int numberFive (void);
asm int numberFive (void)
{
    mov eax, 1
    mov edi, 1
}

void foo() {
  numberFive ();
}
