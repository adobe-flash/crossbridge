/* APPLE LOCAL file 5559195 */
// { dg-do compile }
// { dg-final { scan-assembler-not "(\tcall|\tbl)\[ 	\]*__ZN6ClassAD2Ev" } }

// Verify that implicit calls to empty destructors are omitted.

class ClassA {
public:
  ~ClassA() {} // Empty destructor.  Should not be called by B's destructor.
};

class ClassB : public ClassA {
};

int
main() {
  { ClassB b; } // Destructor called as b falls out of scope.
  return 0;
}
