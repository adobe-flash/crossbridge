/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options { "-fasm-blocks" } } */
/* Radar 4211947 */

void foo() {
#ifdef __cplusplus
  /* This should be fixed with the recursive descent rewrite for the C
     frontend.  */
  asm
#endif
  int i;
  asm nop
  ; int l;
  asm asm
  int j = i, k = l;
  int m = j;
  asm nop ; nop
  asm ; nop
  asm nop ; nop asm nop ; nop 
  asm {
    ; nop
    nop ; nop
  }
}
