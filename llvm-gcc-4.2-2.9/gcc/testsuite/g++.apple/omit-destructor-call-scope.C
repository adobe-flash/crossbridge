/* APPLE LOCAL file 5559195 */
// LLVM LOCAL - -m32
// { dg-options "-m32" }
// { dg-do compile }
// { dg-final { scan-assembler-not "(\tcall|\tbl)\[ 	\]*__ZN6ClassAD1Ev" } }
// { dg-final { scan-assembler "(\tcall|\tbl)\[ 	\]*__ZN6ClassBD1Ev" } }

// Verify that implicit calls to empty destructors are omitted and implicit
// calls to non-empty destructors are not omitted when instances fall out of
// scope.

int n;

class ClassA {
public:
  // Empty destructor.  Should not be called when ClassA instance falls out
  // of scope.
  ~ClassA() {}
};

class ClassB {
public:
  // Non-empty destructor.  Should be called when ClassB instance falls out
  // of scope.
  ~ClassB() {
    n = 0;
  }
};

int
main() {
  n = 1;
  {
    ClassA a; // Destructor not called.
    ClassB b; // Destructor called.
  }
  return n;
}
