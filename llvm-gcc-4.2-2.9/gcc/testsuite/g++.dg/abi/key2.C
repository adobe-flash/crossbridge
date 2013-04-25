// On Darwin, key methods that are inline result in comdat style things.  */
// PR darwin/25908

// { dg-do compile { target *-*-darwin* } }
// LLVM LOCAL begin rearrange tests to check current syntax
// { dg-final { scan-assembler ".section\[ \t]+__DATA,__const_coal,coalesced\\n\[ \t]+.globl\[ \t]+__ZTV1f\\n\[ \t]+.weak_definition\[ \t]+__ZTV1f\\n" } }
// { dg-final { scan-assembler ".section\[ \t]+__TEXT,__const_coal,coalesced\\n\[ \t]+.globl\[ \t]+__ZTS1f\\n\[ \t]+.weak_definition\[ \t]+__ZTS1f\\n" } }

//  With llvm ZTI1f is in the right place, but the ordering is different
// so the .section directive is not needed.  Do the best we can.
// (It belongs in the same place as ZTV1f.)
// { dg-final { scan-assembler ".globl\[ \t]+__ZTI1f\\n\[ \t]+.weak_definition\[ \t]+__ZTI1f\\n" } }
// LLVM LOCAL end

class f
{
  virtual void g();
  virtual void h();
} c;
inline void f::g() {}
int sub(void)
{}
