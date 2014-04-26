/* { dg-do compile } */
/* { dg-options { -std=c99 } } */
/* Radar 4336222 */

inline int foo() { return 0; }

void bar() {
  extern int foo();
}
