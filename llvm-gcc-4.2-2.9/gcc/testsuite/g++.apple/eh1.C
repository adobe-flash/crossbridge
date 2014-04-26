/* APPLE LOCAL file EH __DATA __gcc_except_tab 5819051 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-final { scan-assembler "__TEXT,__gcc_except_tab" } } */
/* Radar 5819051 */

#include <stdio.h>

void foo() {
  try {
    throw 1;
  } catch (int i) {
    printf("Hi\n");
  }
}
