/* APPLE LOCAL file objc gc 5547128 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */
/* Radar 5547128 */

__strong float *fp;

void foo() {
  fp[0] = 3.14;
}
