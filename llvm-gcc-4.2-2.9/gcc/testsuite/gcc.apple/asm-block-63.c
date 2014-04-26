/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { -fasm-blocks } } */
/* Radar 4766972 */

int f() { @@ }	/* { dg-error "expected expression before '@' token" } */
