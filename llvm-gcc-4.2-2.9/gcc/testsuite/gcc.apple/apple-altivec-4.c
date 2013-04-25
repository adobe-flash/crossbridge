/* APPLE LOCAL file AltiVec */
/* Additional AltiVec PIM argument type combinations.  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec" } */

typedef float R;
typedef vector float V;
V gV = (vector float)(0.3, 0.4, 0.5, 0.6);

void foo(int ovs) {
  vector float vFGravity1, vFGravity2, vCombined1OverR_12 = (vector float)(0.3);
  V v; R *x = (R *)&gV;

  vFGravity1 = vec_mergeh(vCombined1OverR_12, vCombined1OverR_12);
  vFGravity2 = vec_mergel(vCombined1OverR_12, vCombined1OverR_12);

  vec_ste (v, 0, x);
  vec_ste (v, 4 + ovs, x);
}
