/* APPLE LOCAL file blocks 7184496 */
/* Test __weak attribute on __block objects. */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-fblocks -ObjC++ -fobjc-gc" { target *-*-darwin* } } */
/* Radar 7184496 */

void nonweakfofo() {
  __block void (^weakSelf)();
}

void weakfofo() {
  __block __weak void (^weakSelf)();
}

/* { dg-final { scan-assembler "151" } } */
/* { dg-final { scan-assembler "135" } } */
