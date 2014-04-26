/* APPLE LOCAL file __block assign sequence point 6724165 */
/* { dg-do compile { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks -Wall" } */

int foo() {
 __block int i = 0;
 int value = 0;
 i = (value != 0);
 return i;
}
