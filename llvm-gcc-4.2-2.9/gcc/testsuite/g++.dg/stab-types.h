/* APPLE LOCAL file */
#ifndef STAB_TYPES_H
#define STAB_TYPES_H

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

int xyz;
#endif
