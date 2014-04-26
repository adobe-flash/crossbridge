/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { -fasm-blocks } } */
/* Radar 4197305 */

#if 1
@@		/* { dg-error "expected identifier or '\\(' before '@' token" } */
#endif
