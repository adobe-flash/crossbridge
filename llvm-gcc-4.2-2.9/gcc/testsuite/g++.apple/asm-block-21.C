/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -m64 -fasm-blocks -msse3 } } */
/* Radar 4249602 */

void foo() {
  asm {
    mov dx, WORD PTR [ebx]
    lock cmpxchg dword ptr[ecx], ebx 
    inc byte ptr [ecx]
    inc word ptr [ecx]
    inc dword ptr [ecx]
    inc qword ptr [ecx] /* { dg-bogus "no such 386 instruction" "" } */
  }
}
