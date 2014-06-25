/* File : example.h */

class A {
public:
  A() {
    printf("A ctor\n");
  }
};

class B : public A {
public:
  B() {
    printf("B ctor\n");
  }
};

class C : public B {
public:
  C() {
    printf("C ctor\n");
  }
};

