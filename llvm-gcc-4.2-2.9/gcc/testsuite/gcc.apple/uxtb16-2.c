/* APPLE LOCAL file uxtb16 support */
/* { dg-do run { target arm*-*-darwin* } } */
/* { dg-options "-O2 -mno-thumb -march=armv6" } */
/* Make sure the uxtb16 sequences work. */

/* uxtb16 x
 2, 0 */
unsigned test1(unsigned x) { return x & 0x00FF00FF; }

/* uxtb16 x, ror #8
 3, 1 */
unsigned test2(unsigned x) { return (x >> 8) & 0x00FF00FF; }

/* uxtb16 x, ror #8
 3, 1 */
unsigned test3(unsigned x) { return (x & 0xFF00FF00) >> 8; }

/* uxtb16 x, ror #8
 3, 1 */
unsigned test4(unsigned x) { return ((x & 0xFF000000) >> 8) | ((x & 0x0000FF00) >> 8); }

/* uxtb16 x, ror #8
 3, 1 */
unsigned test5(unsigned x) { return ((x >> 8) & 0x00FF0000) | ((x >> 8) & 0x000000FF); }

/* uxtb16 x, ror #16
 0, 2 */
unsigned test6(unsigned x) { return ((x & 0x00FF0000) >> 16) | ((x & 0x000000FF) << 16); }

/* uxtb16 x, ror #16
 0, 2 */
unsigned test7(unsigned x) { return ((x >> 16) & 0x000000FF) | ((x << 16) & 0x00FF0000); }

/* uxtb16 x, ror #24
 1, 3 */
unsigned test8(unsigned x) { return ((x & 0x0000FF00) << 8) | ((x & 0xFF000000) >> 24); }

/* uxtb16 x, ror #24
 1, 3 */
unsigned test9(unsigned x) { return ((x >> 24) & 0x000000FF) | ((x << 8) & 0x00FF0000); }

int main() {
    if (test1(0x12345678u) != 0x00340078u)
      return 1;
    if (test2(0x12345678u) != 0x00120056u)
      return 1;
    if (test3(0x12345678u) != 0x00120056u)
      return 1;
    if (test4(0x12345678u) != 0x00120056u)
      return 1;
    if (test5(0x12345678u) != 0x00120056u)
      return 1;
    if (test6(0x12345678u) != 0x00780034u)
      return 1;
    if (test7(0x12345678u) != 0x00780034u)
      return 1;
    if (test8(0x12345678u) != 0x00560012u)
      return 1;
    if (test9(0x12345678u) != 0x00560012u)
      return 1;
    return 0;
}
    

