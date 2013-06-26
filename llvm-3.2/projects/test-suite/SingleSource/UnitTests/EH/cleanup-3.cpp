#include <iostream>

struct A {
  ~A() {
    std::cout << "In A's d'tor\n";
  }
};

struct B {
  ~B() {
    std::cout << "In B's d'tor\n";
  }
};

struct C {
  void throw_int() {
    throw 42;
  }

  ~C() {
    std::cout << "In C's d'tor\n";
    try {
      B b;
      throw_int();
    } catch (char e) {
      std::cout << "(C::~C) Caught char: " << e << "\n";
    }
  }
};

#define DECLARE(FUNC)                                \
  void FUNC() __attribute__((always_inline));        \
  void FUNC()

DECLARE(throw_char) {
  C c;
  throw 'c';
}

DECLARE(cleanup) {
  A a;
  try {
    B b;
    throw_char();
  } catch (char e) {
    std::cout << "Caught char: " << e << "\n";
  }
}

void term() {
  printf("Inside the terminator\n");
  exit(EXIT_SUCCESS);
}

int main() {
  std::set_terminate(term);
  A a;
  cleanup();
}
