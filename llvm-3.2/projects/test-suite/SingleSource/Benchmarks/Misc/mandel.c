/* Sparc is not C99-compliant */
#if defined(sparc) || defined(__sparc__) || defined(__sparcv9)

int main() { return 0; }

#else /* sparc */

#define ESCAPE 2
#ifdef SMALL_PROBLEM_SIZE
#define IMAGE_SIZE 500
#else
#define IMAGE_SIZE 5000
#endif
#define START_X -2.0
#define START_Y START_X
#define MAX_ITER 10
#define step (-START_X - START_X)/IMAGE_SIZE

#define I 1.0iF

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__MINGW32__)
#include <complex.h>
#elif defined(__APPLE__)
#include <math.h>
#else
#include <tgmath.h>
#endif

#include <stdio.h>

volatile double __complex__ accum;
void emit(double __complex__ X) {
  accum += X;
}

void mandel() {
  int x, y, n;
  for (x = 0; x < IMAGE_SIZE; ++x) {
    for (y = 0; y < IMAGE_SIZE; ++y) {
      double __complex__ c = (START_X+x*step) + (START_Y-y*step) * I;
      double __complex__ z = 0.0;

      for (n = 0; n < MAX_ITER; ++n) {
        z = z * z + c;
        if (hypot(__real__ z, __imag__ z) >= ESCAPE)
          break;
      }
      emit(z);
    }
  }
}

int main() {
  mandel();
  printf("%d\n", (int)accum);
  return 0;
}

#endif /* sparc */
