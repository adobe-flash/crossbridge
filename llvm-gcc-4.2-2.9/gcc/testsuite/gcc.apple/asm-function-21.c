/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options { -fasm-blocks } } */


asm void myasm1(register int i) {
  fralloc
  frfree
}		/* { dg-error "blr must follow frfree" } */

asm void myasm2(register int i) {
  fralloc
  frfree
  blr
  nop		/* { dg-error "not allowed after frfree blr" } */
}

void myasm3() {
  asm {
    fralloc
    frfree
  }		/* { dg-error "blr must follow frfree" } */
}
