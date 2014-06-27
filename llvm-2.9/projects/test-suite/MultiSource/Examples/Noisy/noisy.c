#include <stdlib.h>
#include <string.h>
#include <time.h>

double get_time() {
  return (double) clock() / CLOCKS_PER_SEC;
}

int cmp(const void *a, const void *b) {
  return 0;
}

int main() {
  static int big_array[1 << 12] = { 0 };
  double base_time = .2, noise_range = .1;
  double delay, start_time;

  srand48(clock());
  delay = base_time + noise_range * drand48();

  // We spin here to make sure we consume user time.
  start_time = get_time();
  while (get_time() < start_time + delay) {
    // Ok until someone on the backend gets crazy.
    qsort(big_array, sizeof(big_array) / sizeof(big_array[0]),
          sizeof(big_array[0]), cmp);
  }

  return 0;
}
