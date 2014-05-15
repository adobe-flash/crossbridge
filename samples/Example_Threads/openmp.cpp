// Simple example of OpenMP

#include <math.h>
#include <time.h>
#include <omp.h>
#include <iostream>

static const int sSineCount = 1024 * 1024 * 16;
static volatile double sManySines[sSineCount];

int main() {
  // try different #s of threads
  for(int threads = 1; threads <= 4; threads++) {
    omp_set_num_threads(threads); // set thread count (thread creation happens here!)
    for(int iters = 0; iters < 5; iters++) { // 5 iterations of each config
      clock_t start = clock(); // check the start time
      #pragma omp parallel for schedule(dynamic, 1024 * 16) // parallel!
      for(int i = 0; i < sSineCount; i++) { // fill the array
        double d = (double)i / (double)sSineCount;
        sManySines[i] = sin(d);
      }
      clock_t diff = clock() - start; // calc elapsed time

      std::cout << threads << " threads took " << (diff * 1000) / CLOCKS_PER_SEC << " ms";
      if(iters == 0 && threads != 1)
        std::cout << " (adding new thread)";
      std::cout << "\n";
    }
  }
  std::cout << "done!\n";
  return 0;
}
