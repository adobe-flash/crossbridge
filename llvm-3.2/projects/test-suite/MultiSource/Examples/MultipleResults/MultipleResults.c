#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv) {
  assert(argc == 1);
  FILE *f = fopen(argv[1], "w");
  fprintf(f, "Result-A: %.2f\n", 0.1);
  fprintf(f, "Result-B: %.2f\n", 0.3);
  fclose(f);
  return 0;
}
