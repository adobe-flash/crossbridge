/* { dg-options "-Os" } */
/* { dg-final { scan-assembler-not " orl\t" } } */

/* Radar 6020402 - GCC 4.2.1/x86_64 coalesces accesses to 32-bit bitfield and
   adjacent volatile qualified "int" embedded in a struct.

   This test case makes sure that a 32-bit OR is not used to store to
   the bit field "a", which would interfere with the volatile field "i".
   Note that -m64 is not required for this issue to occur.
*/

typedef struct {
  unsigned u;
  short a:1, b:15;
  volatile short i;
} S;

S s;

void f() {
  s.a = 1;
}
