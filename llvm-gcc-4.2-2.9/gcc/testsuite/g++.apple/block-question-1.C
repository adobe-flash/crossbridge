/* APPLE LOCAL file blocks 6065211 */
/* { dg-options "-fblocks" } */

template <class T>
void f(T a) { }

enum e { no, yes };

void foo() {
  void (^bp)(int);
  void (*rp)(int);
  void (^bp1)();
  double *dp;
  void *vp = bp;

  vp = bp;
  f(1 ? dp : vp);
  f(1 ? bp : vp);
  bp != vp;
  f(1 ? bp : bp1);		/* { dg-error "distinct pointer types" } */
  bp > rp;			/* { dg-error "invalid operands" } */
  bp > 0;			/* { dg-error "invalid operands" } */
  bp > bp;			/* { dg-error "invalid operands" } */
  bp > vp;			/* { dg-error "invalid operands" } */
  f(1 ? bp : rp);		/* { dg-error "distinct pointer types" } */
}
