/* APPLE LOCAL file 64bit shorten warning 3865314 */
/* { dg-do compile } */
/* { dg-options "-Wshorten-64-to-32" } */
/* Radar 3865314 */

long long ll;
int i;
char c;

void bar (int);

void foo() {
  c = i;
  c = ll;
  i = (int) ll;
  i = ll;	/* { dg-warning "implicit conversion shortens 64-bit value into a 32-bit value" } */
  i += ll;	/* { dg-warning "implicit conversion shortens 64-bit value into a 32-bit value" } */
  i = i ? ll : i;/* { dg-warning "implicit conversion shortens 64-bit value into a 32-bit value" } */
  bar (ll);	/* { dg-warning "implicit conversion shortens 64-bit value into a 32-bit value" } */
}
