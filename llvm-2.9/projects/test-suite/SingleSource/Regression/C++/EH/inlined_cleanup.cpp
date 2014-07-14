#include <stdio.h>

class Cleanup {
public:
  ~Cleanup() {
    printf("In cleanup!\n");
  }
};

static void foo() {
  Cleanup C;
  throw 3;
}

int main(void) {
  try {
    foo();
  } catch (int i) {
    printf("Caught %d!\n", i);
  }
  return 0;
}
