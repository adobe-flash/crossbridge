/* APPLE LOCAL file blocks 6065211 */
/* { dg-options "-fblocks" } */

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
  f(1 ? bp : (void*)0);
  f(1 ? bp : bp1);		/* { dg-error "type mismatch" } */
  bp > rp;			/* { dg-error "invalid operands" } */
  bp > 0;			/* { dg-error "invalid operands" } */
  bp > bp;			/* { dg-error "invalid operands" } */
  bp > vp;			/* { dg-error "invalid operands" } */
  f(1 ? bp : rp);		/* { dg-error "type mismatch" } */
}
