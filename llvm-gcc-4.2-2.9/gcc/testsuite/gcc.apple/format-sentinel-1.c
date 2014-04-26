/* APPLE LOCAL file two arg sentinel 5631180 */
/* { dg-do compile } */
/* PR34516 */
/* Radar 5631180 */

void foo(void *last, ...) __attribute__((sentinel(0,1)));

void bar() {
  foo((void*)0);
}
