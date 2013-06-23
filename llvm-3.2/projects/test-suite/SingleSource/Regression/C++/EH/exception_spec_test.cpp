// This tests that exception specifications interact properly with unexpected
// handlers.

#include <exception>
#include <stdio.h>
#include <stdlib.h>

static void TerminateHandler0() {
  printf("std::terminate called, as expected\n");
  exit(0);
}

static void TerminateHandler1() {
  printf("std::terminate called, but it was not expected!\n");
  exit(1);
}

static void UnexpectedHandler1() {
  printf("std::unexpected called: throwing a double\n");
  throw 1.0;
}

static void UnexpectedHandler2() {
  printf("std::unexpected called: throwing an int!\n");
  throw 1;
}

void test(bool Int) throw (double) {
  if (Int) {
    printf("Throwing an int from a function which only allows doubles!\n");
    throw 1;
  } else {
    printf("Throwing a double from a function which allows doubles!\n");
    throw 1.0;
  }
}

int main() {
  std::set_terminate(TerminateHandler1);

  try {
    test(false);
  } catch (double D) {
    printf("Double successfully caught!\n");
  }

  std::set_unexpected(UnexpectedHandler1);

  try {
    test(true);
  } catch (double D) {
    printf("Double successfully caught!\n");
  }

  std::set_terminate(TerminateHandler0);
  std::set_unexpected(UnexpectedHandler2);
  test(true);

  printf("TerminateHandler0 should have been called!\n");
  return 1;
}
