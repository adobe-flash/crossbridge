/* APPLE LOCAL file */
/* Contributed by Devang Patel  <dpatel@apple.com> */
/* Tesst debug info for virtual base class.  */
/* { dg-do compile }  */
/* { dg-options "-gstabs+ -gfull" } */
/* { dg-require-effective-target ilp32 } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

class A
{
public:
  int a;
  A() { a = 2; };
  virtual int m();
};

class B: virtual A 
{
public:
  int b;
  B() { b = 4; };
  int m();
};


int B::m()
{
  return 1;
}

/* { dg-final { scan-assembler "B:Tt\\(\[0-9\],\[0-9\]*\\)=s16!1,10-96" } } */
