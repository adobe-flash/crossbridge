/* APPLE LOCAL file weak types 6524730 */
/* { dg-do compile } */
/* Radar 6524730 */

int * __attribute__((weak)) foo(int * x) {
  return x;
}
