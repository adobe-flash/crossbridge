/* APPLE LOCAL begin radar 4095567 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O2 -mfpmath=387 -mdynamic-no-pic" } */
/* { dg-final { scan-assembler-not "sub\[^\\n\]*sp" } } */

/* -Os -mfpmath=387 -mdynamic-no-pic */
double foo(double x) {
        return x+1.0;
}
/* APPLE LOCAL end radar 4095567 */
