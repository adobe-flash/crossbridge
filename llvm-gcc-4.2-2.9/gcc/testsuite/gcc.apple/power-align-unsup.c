/* Darwin (Mac OS X) alignment exercises.  */

/* APPLE LOCAL darwin10 */
/* { dg-do compile { target powerpc-*-darwin[8912]* } } */
/* { dg-options "-m64 -malign-power" } */

/* { dg-error "-malign-power is not supported for 64-bit Darwin" "" { target *-*-* } 0 } */

main() {}
