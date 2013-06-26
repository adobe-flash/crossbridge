#include <stdio.h>

/* Timing test for unrolled 4x4 matrix multiplication, double precision. */

static void mul4(double *Out, const double A[4][4], const double B[4][4]) {
  unsigned n;

  /* Assume that Out may alias A or B. The simple array also lures SROA into
   * creating a single i1024 scalar. */
  double Res[16];

  Res[ 0] = A[0][0]*B[0][0] + A[0][1]*B[1][0] + A[0][2]*B[2][0] + A[0][3]*B[3][0];
  Res[ 1] = A[0][0]*B[0][1] + A[0][1]*B[1][1] + A[0][2]*B[2][1] + A[0][3]*B[3][1];
  Res[ 2] = A[0][0]*B[0][2] + A[0][1]*B[1][2] + A[0][2]*B[2][2] + A[0][3]*B[3][2];
  Res[ 3] = A[0][0]*B[0][3] + A[0][1]*B[1][3] + A[0][2]*B[2][3] + A[0][3]*B[3][3];
  Res[ 4] = A[1][0]*B[0][0] + A[1][1]*B[1][0] + A[1][2]*B[2][0] + A[1][3]*B[3][0];
  Res[ 5] = A[1][0]*B[0][1] + A[1][1]*B[1][1] + A[1][2]*B[2][1] + A[1][3]*B[3][1];
  Res[ 6] = A[1][0]*B[0][2] + A[1][1]*B[1][2] + A[1][2]*B[2][2] + A[1][3]*B[3][2];
  Res[ 7] = A[1][0]*B[0][3] + A[1][1]*B[1][3] + A[1][2]*B[2][3] + A[1][3]*B[3][3];
  Res[ 8] = A[2][0]*B[0][0] + A[2][1]*B[1][0] + A[2][2]*B[2][0] + A[2][3]*B[3][0];
  Res[ 9] = A[2][0]*B[0][1] + A[2][1]*B[1][1] + A[2][2]*B[2][1] + A[2][3]*B[3][1];
  Res[10] = A[2][0]*B[0][2] + A[2][1]*B[1][2] + A[2][2]*B[2][2] + A[2][3]*B[3][2];
  Res[11] = A[2][0]*B[0][3] + A[2][1]*B[1][3] + A[2][2]*B[2][3] + A[2][3]*B[3][3];
  Res[12] = A[3][0]*B[0][0] + A[3][1]*B[1][0] + A[3][2]*B[2][0] + A[3][3]*B[3][0];
  Res[13] = A[3][0]*B[0][1] + A[3][1]*B[1][1] + A[3][2]*B[2][1] + A[3][3]*B[3][1];
  Res[14] = A[3][0]*B[0][2] + A[3][1]*B[1][2] + A[3][2]*B[2][2] + A[3][3]*B[3][2];
  Res[15] = A[3][0]*B[0][3] + A[3][1]*B[1][3] + A[3][2]*B[2][3] + A[3][3]*B[3][3];

  for (n = 0; n < 16; ++n)
    Out[n] = Res[n];
}

/* Allow mul4 to be inlined into wrap_mul4. This actually enables further
 * optimizations. */
__attribute__((__noinline__))
void wrap_mul4(double *Out, const double A[4][4], const double B[4][4])
{
  mul4(Out, A, B);
}

int main() {
#ifdef SMALL_PROBLEM_SIZE
  const unsigned Iterations = 1000000;
#else
  const unsigned Iterations = 50000000;
#endif
  const double A[4][4] = {
    { 4.5, 1.3, 6.0, 4.1 },
    { 2.5, 7.2, 7.7, 1.7 },
    { 6.7, 1.3, 9.4, 1.3 },
    { 1.1, 2.2, 3.0, 2.1 }
  };
  const double B[4][4] = {
    { 1.0, 7.9, 5.1, 3.4 },
    { 6.6, 2.8, 5.4, 9.2 },
    { 5.0, 4.1, 4.1, 9.9 },
    { 8.4, 3.7, 9.5, 6.4 }
  };
  double C[4][4];
  unsigned n, m;

  for (n = 0; n != Iterations; ++n)
    wrap_mul4(&C[0][0], A, B);

  for (n = 0; n != 4; ++n) {
    for (m = 0; m != 4; ++m)
      printf("%8.2f", C[n][m]);
    puts("");
  }

  return 0;
}
