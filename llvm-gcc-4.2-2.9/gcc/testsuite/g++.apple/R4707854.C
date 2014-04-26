/* APPLE LOCAL file Radar 4707854  */
/* Test generation of DWARF abstract origin DIEs in C++.  */

// { dg-do compile }
// { dg-options "-Os -finline -gdwarf-2 -S -fno-eliminate-unused-debug-types" }

struct s1
{ 
  virtual void f1();
  void f2();
  void ftrouble();
};

void s1::f1() { }
void s1::f2() { ftrouble(); }
void s1::ftrouble() {  }

