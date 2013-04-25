// { dg-do compile { target *-*-darwin* } }
// LLVM LOCAL adjust for different syntax
// { dg-final { scan-assembler "GCC_except_table" } }
void needed();
void unneeded();

/* APPLE LOCAL begin omit calls to empty destructors 5559195 */
int n = 0;
/* APPLE LOCAL end omit calls to empty destructors 5559195 */

class Bar
{
public:
  Bar() {}
  /* APPLE LOCAL begin omit calls to empty destructors 5559195 */
  virtual ~Bar() {
    // Without this nontrivial operation, destructor is optimized away and
    // GCC_except_table0 is not generated.
    n = 1;
  }
  /* APPLE LOCAL end omit calls to empty destructors 5559195 */

  void unneeded();
};

void needed()
{
	Bar b;
}

//#if 0
void unneeded()
{
	Bar b;
	b.unneeded();
}
//#endif

int main()
{
	needed();

	return 0;
}
