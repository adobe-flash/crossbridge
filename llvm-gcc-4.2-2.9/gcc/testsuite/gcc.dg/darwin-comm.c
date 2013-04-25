/* APPLE LOCAL darwin10 */
/* { dg-do compile { target *-*-darwin[912]* } } */
/* LLVM LOCAL allow for whitespace difference */
/* { dg-final { scan-assembler ".comm( |\t)_foo,1,15" } } */

char foo __attribute__ ((aligned(32768)));
