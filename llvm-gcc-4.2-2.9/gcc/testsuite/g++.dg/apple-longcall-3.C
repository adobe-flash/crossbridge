/* APPLE LOCAL file 4380289 */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-mlongcall -mkernel" } */
/* { dg-final { scan-assembler "\tjmp" } } */
/* { dg-final { scan-assembler "\tjbsr" } } */
/* Fail if any short calls ("bl foo") are present.  */
/* { dg-final { scan-assembler-not "\tbl\[	 \]" } } */
/* Fail if any short branches to non-labels ("b __Z_blabla") are
   present.  Short branches to labels ("b L42") are O.K.  */
/* { dg-final { scan-assembler-not "\tb\[	 \]*_" } } */
/* { dg-final { scan-assembler-not "__ZN7derivedD1Ev " } } */
/* Radar 4299630: insure C++ thunks get long jmps.  */

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

struct base3
{
  virtual ~base3() {}
  void foo3() {}
  void voodoo()  __attribute__ ((__noinline__));
};

extern int global_counter;

void base3::voodoo()
{
  global_counter++;
}

struct derived
  : public base1, public base2, base3
{
  void d_voodoo() __attribute__ ((__noinline__)) ;
};

base3 *pbase3;

void derived::d_voodoo() { pbase3->voodoo(); }


int main()
{
  typedef void (base1::*F1)();
  typedef void (base2::*F2)();
  derived* dp;
  derived* dd;
  F1 f1 = &base1::foo1;
  F2 f2 = &base2::foo2;
  pbase3 = new base3;
  dd = new derived;
  pbase3->voodoo();
  dd->d_voodoo();
  (dp->*f1)();
  (dp->*f2)();
}
