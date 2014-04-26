/* APPLE LOCAL file bincl/eincl stabs */
/* Test BINCL/EINCL stabs.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

#ifndef BINCL_2_H
#define BINCL_2_H

class A
{
public:
  int a;
};

class B {
public:
  static void bm1(const int& node, A& out);
  static void bm2(int& node, const A& in);
};
#include "bincl-22.h"

int xyz;
#endif

