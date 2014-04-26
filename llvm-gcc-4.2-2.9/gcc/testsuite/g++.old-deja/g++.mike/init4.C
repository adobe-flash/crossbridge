/* APPLE LOCAL file elide global inits 5718329 */
/* { dg-do compile } */
/* { dg-options -O2 } */
/* { dg-final { scan-assembler-not "_GLOBAL__I_" } } */
/* Radar 5718329 */

int j;
struct B {
  const int ci;
  const double d;
  int rwi;
  B() :ci(0), rwi(0), d(0) {
    this->rwi = 0;
  }
} b;
