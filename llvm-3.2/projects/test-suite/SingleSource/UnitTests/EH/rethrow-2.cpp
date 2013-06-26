#include <iostream>
#include <string>

#define DECL_ATTR_FUNC(FUNC)                            \
  void FUNC() __attribute__((noinline));                \
  void FUNC()

#define DECLARE_FUNC(FUNC)                      \
  DECL_ATTR_FUNC(throw_ ## FUNC)

#define DECLARE_RETHROW_FUNC(TYPE)              \
  DECL_ATTR_FUNC(rethrow_ ## TYPE) {            \
    try {                                       \
      throw_ ## TYPE ();                        \
    } catch (TYPE e) {                          \
      std::cout << "Rethrowing: " << e << "\n"; \
      throw;                                    \
    }                                           \
  }

DECLARE_FUNC(char) {
  char c = 'a';
  std::cout << "Throwing char: " << c << "\n";
  throw c;
}

DECLARE_RETHROW_FUNC(char)

DECLARE_FUNC(int) {
  int i = 37;
  std::cout << "Throwing int: " << i << "\n";
  throw i;
}

DECLARE_RETHROW_FUNC(int)

DECLARE_FUNC(float) {
  float f = 37.927;
  std::cout << "Throwing float: " << f << "\n";
  throw f;
}

DECLARE_RETHROW_FUNC(float)

DECLARE_FUNC(double) {
  double d = 37.927;
  std::cout << "Throwing double: " << d << "\n";
  throw d;
}

DECLARE_RETHROW_FUNC(double)

DECLARE_FUNC(string) {
  std::string s = "hello world";
  std::cout << "Throwing std::string: " << s << "\n";
  throw s;
}

DECL_ATTR_FUNC(rethrow_string) {
  try {
    throw_string();
  } catch (const std::string &s) {
    std::cout << "Rethrowing: " << s << "\n";
    throw;
  }
}

struct A {};

DECLARE_FUNC(A) {
  std::cout << "Throwing A\n";
  throw A();
}

DECL_ATTR_FUNC(rethrow_A) {
  try {
    throw_A();
  } catch (const A &) {
    std::cout << "Rethrowing A\n";
    throw;
  }
}

int main() {
  try {
    rethrow_char();
  } catch (char e) {
    std::cout << "Caught char: " << e << "\n";
  }

  try {
    rethrow_int();
  } catch (int e) {
    std::cout << "Caught int: " << e << "\n";
  }

  try {
    rethrow_float();
  } catch (float e) {
    std::cout << "Caught float: " << e << "\n";
  }

  try {
    rethrow_double();
  } catch (double e) {
    std::cout << "Caught double: " << e << "\n";
  }

  try {
    rethrow_string();
  } catch (const std::string &e) {
    std::cout << "Caught std::string: " << e << "\n";
  }

  try {
    rethrow_A();
  } catch (const A &) {
    std::cout << "Caught A\n";
  }
}
