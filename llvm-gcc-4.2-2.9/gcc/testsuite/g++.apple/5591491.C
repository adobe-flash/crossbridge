/* APPLE LOCAL file radar 5591491 */
/* { dg-options "-O2" } */
/* { dg-do compile { target *-*-darwin* } } */

typedef union {
  unsigned char f1[4];
} u1;
class c1 {
public:
  u1 f2;
};

class c3 {
public:
  c1             f4;
};

class c4 {
  virtual int m1();
};

int c4::m1()
{
  c3 *v1;
  struct {
    u1        f2;
  } v3 = {};

 v1->f4.f2 = v3.f2;
}

