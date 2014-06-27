// rdar://8453812
extern "C" void abort();

_Complex int getComplex(_Complex int val) {
  static int count;
  if (count++)
    abort();
  return val;
}

_Complex int cmplx_test() {
    _Complex int cond;
    _Complex int rhs;

    return getComplex(1+2i) ? : rhs;
}

// lvalue test
int global = 1;
void foo (int& lv) {
  ++lv;
}

int &cond() {
  static int count;
  if (count++)
    abort();
  return global;
}

int  main() {
    cmplx_test();
    int rhs = 10;
    foo (cond()? : rhs);
    return  global-2;
}
