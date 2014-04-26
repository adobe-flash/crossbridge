/* APPLE LOCAL file 4334498 */
/* In C++ method stab, 'void type' is used to indicate that it is
   not a variadic function.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* *-*-netware* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs+ -feliminate-unused-debug-symbols" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

class A
{
public:
  int one(int);
private:
  char data;
};
int A::one(int i)
{
  return data + 1;
}
int main(int argc, char **argv)
{
  A a;
  return a.one(1);
}

/* { dg-final { scan-assembler ".stabs.*:t\\(0,1\\)=\\(0,1\\)" } } */
