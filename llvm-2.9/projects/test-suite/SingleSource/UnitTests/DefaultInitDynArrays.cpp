#include <assert.h>

void func() {
  int *buckets = new int[111]();
  for (int i = 0; i < 111; i++)
    assert(buckets[i] == 0);
  for (int i = 0; i < 111; i++)
    buckets[i] = i;

  delete [] buckets;
}

int main(int argc, char **argv) {
    func();
    func();
    func();
    func();
    func();
    return 0;
}
