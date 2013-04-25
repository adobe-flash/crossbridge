/* { dg-do compile { target "i?86-*-*" } } */
/* { dg-options "-m64 -Os" } */
/* { dg-final { scan-assembler-not " orq\t" } } */

/* Radar 6020402 - GCC 4.2.1/x86_64 coalesces accesses to 32-bit bitfield and
   adjacent volatile qualified "int" embedded in a struct.

   This test case makes sure that a 64-bit OR is not used to store to
   the bit field "a", which would interfere with the volatile field "i".
*/

typedef struct {
  long l;
  unsigned a:1, b:31;
  volatile unsigned i;
} S;

S s;

void f() {
  s.a = 1;
}
