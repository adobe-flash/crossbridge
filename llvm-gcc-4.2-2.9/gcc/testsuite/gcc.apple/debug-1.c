/* APPLE LOCAL file .file/.loc 6349436 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-g" } */
/* { dg-final { scan-assembler "\\.file" } } */
/* { dg-final { scan-assembler "\\.loc" } } */

void foo() {
}
