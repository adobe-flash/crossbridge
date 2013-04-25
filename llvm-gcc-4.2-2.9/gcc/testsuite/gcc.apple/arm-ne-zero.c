/* APPLE LOCAL file 4382996 */
/* Verify that the result of a binary operator != 0 doesn't generate a
   redundant move instruction.  */

/* { dg-do compile { target arm*-*-darwin* } } */
/* { dg-options "-Os -marm" } */
/* { dg-skip-if "Not valid with -mthumb" { *-*-* } { "-mthumb" } { "" } } */

int test_plus     (int a, int b) { return (a + b) != 0; }
int test_and      (int a, int b) { return (a & b) != 0; }
int test_minus    (int a, int b) { return (a - b) != 0; }
int test_mult     (int a, int b) { return (a * b) != 0; }
int test_ior      (int a, int b) { return (a | b) != 0; }
int test_xor      (int a, int b) { return (a ^ b) != 0; }
int test_ashift   (int a, int b) { return (a << b) != 0; }
int test_ashiftrt (int a, int b) { return (a >> b) != 0; }
unsigned int test_lshiftrt (unsigned int a, unsigned int b) { return (a >> b) != 0; }

/* { dg-final { scan-assembler-times "movne" 9 } } */
/* { dg-final { scan-assembler-not "moveq" } } */
