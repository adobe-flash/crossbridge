/* APPLE LOCAL file 64-bit */
/* 64-bit Darwin officially doesn't like the mac68k alignment pragma.  */

/* { dg-do compile { target powerpc*-*-darwin[8912]* } } */
/* { dg-options "-m64" } */

#pragma options align=mac68k /* { dg-warning "mac68k alignment pragma is deprecated for 64-bit Darwin" } */

#pragma options align=reset /* this shouldn't error out */



