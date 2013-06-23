#include <cstdio>
#include <csetjmp>

class C {
  const char *name;
public:
  C(const char *n) : name(n) {}
  ~C() { printf("Destroying C in function %s\n", name); }
};

jmp_buf buf;

void foo()
{
  C Obj("foo");
  printf("Longjmping from foo() function\n");
  longjmp(buf, 37);
}

int main()
{
  try {
    C Obj("main");
    if (!setjmp(buf)) foo();
    printf("longjmp throw NOT caught in catch(...) block\n");
  } catch(...) {
    printf("Caught longjmp throw in catch(...) block\n");
  }
}

