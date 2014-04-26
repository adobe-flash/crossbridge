/* APPLE LOCAL file radar 5130983 */
/* Test that only lvalue may be a selector expression. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

id foo();

int main() {
  id a;
  for (foo() in a)  /* { dg-error "selector element must be an lvalue" } */
  { 
  } 
}
