/* APPLE LOCAL file */
#include <stdio.h>
/* Radar 3124235 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-O0" } */
void f4(int);
#pragma optimization_level 3
void f1(int x) {
  printf("%d\n", x);
}
#pragma GCC optimize_for_size on
#pragma GCC optimization_level 3
void f4(int  x) {
  printf("%d\n", x);
}
#pragma GCC optimization_level 0
void f5(int x) {
#pragma GCC optimization_level 2
  printf("%d\n", x);
}
#pragma GCC optimization_level reset
void f6(int x) {
  printf("%d\n", x);
}
/* { dg-final { scan-assembler-times "b L?_printf" 2 } } */
/* { dg-final { scan-assembler-times "bl L?_printf" 2 } } */
