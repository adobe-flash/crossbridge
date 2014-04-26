/* APPLE LOCAL math-errno off by default 3619824 */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-mpowerpc-gpopt -O2" } */
/* { dg-final { scan-assembler-not "fcmpu" } } */
extern double sqrt(double);
float foo(float y) {
        return sqrt(y);
}
