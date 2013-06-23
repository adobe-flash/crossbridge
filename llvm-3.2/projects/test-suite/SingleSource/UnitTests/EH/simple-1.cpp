#include <iostream>
#include <string>

#define DECLARE_FUNC(FUNC)                                \
  void throw_ ## FUNC() __attribute__((noinline));        \
  void throw_ ## FUNC()

DECLARE_FUNC(char) {
  char c = 'a';
  std::cout << "Throwing char: " << c << "\n";
  throw c;
}

DECLARE_FUNC(int) {
  int i = 37;
  std::cout << "Throwing int: " << i << "\n";
  throw i;
}

DECLARE_FUNC(float) {
  float f = 37.927;
  std::cout << "Throwing float: " << f << "\n";
  throw f;
}

DECLARE_FUNC(double) {
  double d = 37.927;
  std::cout << "Throwing double: " << d << "\n";
  throw d;
}

DECLARE_FUNC(string) {
  std::string s = "hello world";
  std::cout << "Throwing std::string: " << s << "\n";
  throw s;
}

struct A {};

DECLARE_FUNC(A) {
  std::cout << "Throwing A\n";
  throw A();
}

int main() {
  try {
    throw_char();
  } catch (char e) {
    std::cout << "Caught char: " << e << "\n";
  }

  try {
    throw_int();
  } catch (int e) {
    std::cout << "Caught int: " << e << "\n";
  }

  try {
    throw_float();
  } catch (float e) {
    std::cout << "Caught float: " << e << "\n";
  }

  try {
    throw_double();
  } catch (double e) {
    std::cout << "Caught double: " << e << "\n";
  }

  try {
    throw_string();
  } catch (const std::string &e) {
    std::cout << "Caught std::string: " << e << "\n";
  }

  try {
    throw_A();
  } catch (const A &e) {
    std::cout << "Caught A\n";
  }
}
