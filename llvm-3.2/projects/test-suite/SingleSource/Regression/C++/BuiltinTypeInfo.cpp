#include <typeinfo>
#include <cstdio>

int main() {
  printf("%d", typeid(int) == typeid(float));
}
