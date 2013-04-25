/* APPLE LOCAL file elide global inits 5642351 */
/* { dg-do compile } */
/* { dg-options "-fno-use-cxa-atexit" } */
/* { dg-final { scan-assembler-not "_GLOBAL__I_" } } */

struct Foo {
  ~Foo() { }
};
Foo foo;
