/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { "-fasm-blocks" } } */
/* Radar 4211947 */

void foo() {
#ifdef __cplusplus
  /* This should be fixed with the recursive descent rewrite for the C
     frontend.  */
  asm
#endif
  int k;
  asm ; This is a comment
#ifndef __cplusplus
  /* This should work in C++, but the lexer runs up front and it doesn't
     have all the state we'd like to handle ; asm comments.  */
  asm ; This is a comment to test unpaired '" characters
#endif
  int j;
  asm nop ; This is a comment
  asm nop
  ; int i;
  asm {
    ; This is a comment
    nop ; This is a comment
    nop ; bad ouch } ouch
  }
  k = i = j;
}
