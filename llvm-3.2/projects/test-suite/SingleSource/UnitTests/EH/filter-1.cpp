#include <exception>
#include <iostream>

void bar() throw (int) {
  std::cout << "Throwing in bar(): \"hello world\"\n";
  throw "hello world";
}

void foo() {
  try {
    bar();
  } catch (int i) {
    std::cout << "Caught in foo(): " << i << "\n";
  }
}

void unexpected() {
  std::cout << "Throwing in unexpected(): 42\n";
  throw 42;
}

int main() {
  std::set_unexpected(unexpected);
  foo();
}
