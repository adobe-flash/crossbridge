/* APPLE LOCAL file 5813921 */
/* { dg-do compile { target powerpc-*-* i?86-*-* x86_64-*-* } } */
/* { dg-options { -Os } } */
/* { dg-final { scan-assembler "cntlzw" { target powerpc-*-* } } } */
/* { dg-final { scan-assembler "bsr" { target i?86-*-* x86_64-*-* } } } */
typedef long unsigned int __darwin_size_t;
typedef __darwin_size_t size_t;
typedef enum RoundingMode {
  kRoundTowardNearestEven = 1,  kRoundTowardZero = 2,  kRoundTowardInf = 4,
  kRoundTowardMinusInf = 8,
  kRoundAll = kRoundTowardNearestEven | kRoundTowardZero | kRoundTowardInf
  | kRoundTowardMinusInf } RoundingMode;
extern const int roundingModes[4];
void SetRoundingMode( RoundingMode mode ) {
  long m = mode;
  size_t r = (8*sizeof(long)-1) - __builtin_clzl( m );
  fesetround( roundingModes[ r ] );
}
