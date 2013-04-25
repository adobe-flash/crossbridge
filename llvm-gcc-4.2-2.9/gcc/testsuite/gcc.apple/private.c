/* APPLE LOCAL file private extern */
/* { dg-do compile } */
/* Radar 5487726 */

void foo() {
  __private_extern__ void bar();
  bar();
}
