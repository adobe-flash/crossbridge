/* APPLE LOCAL file 64-bit */
/* Darwin (Mac OS X) alignment exercises.  */

/* { dg-do compile { target powerpc-*-darwin[8912]* } } */
/* { dg-options "-m64 -malign-mac68k" } */

/* { dg-error "-malign-mac68k is not allowed for 64-bit Darwin" "" { target *-*-* } 0 } */

main() {}
