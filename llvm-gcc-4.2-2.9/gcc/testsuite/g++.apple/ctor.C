/* APPLE LOCAL file ctor name 6202462 */
class A {
  virtual int B() { return 0; }
};

class B: A {
  B() { }
};
