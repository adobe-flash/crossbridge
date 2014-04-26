/* APPLE LOCAL file 4299630 */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-mlongcall" } */
/* { dg-final { scan-assembler "\tjmp[ 	]" } } */
/* Radar 4299630: insure C++ thunks get long jmps.  */
/* Contributed by Howard Hinnant.  */

struct base1
{
  virtual ~base1() {}
  void foo1() {}
};

struct base2
{
  virtual ~base2() {}
  void foo2() {}
};

struct derived
  : public base1, public base2
{
};

int main()
{
  typedef void (base1::*F1)();
  typedef void (base2::*F2)();
  derived d;
  derived* dp = &d;
  F1 f1 = &base1::foo1;
  F2 f2 = &base2::foo2;
  (dp->*f1)();
  (dp->*f2)();
}
