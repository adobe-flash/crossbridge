/* APPLE LOCAL begin math-errno off by default 4094534 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O3" } */
/* { dg-final { scan-assembler-not "ucomiss" } } */
extern double sqrt(double);
float foo(float y) {
        return sqrt(y);
}
/* APPLE LOCAL end math-errno off by default 4094534 */
